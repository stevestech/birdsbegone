# Import required packages
import spidev
import time
import RPi.GPIO as GPIO
from wheel import Wheel

class SPI:
    """
    SLAVE COMMANDS
    These imperatives are sent from the SPI master and
    inform the slave how to behave.
    """
    commands = { 'RECEIVE_A_ORIENTATION': chr(0),            # Prepare the slave to receive data from the master
                 'RECEIVE_HM_STATE': chr(1),
                 'RECEIVE_HM_THROTTLE': chr(2),
                 'LOAD_A_MEASURED_ORIENTATION': chr(100),    # Instruct the slave to load data onto its send buffer for the master to read
                 'LOAD_A_CONTROLLER_OUTPUT': chr(101),
                 'LOAD_SHUTDOWN_ERROR': chr(102) }
    
    """
    SPI ERROR MESSAGES
    "Errors detected by the slave" are reserved values and cannot be used inside messages
    to the slave or to the master.
    """
    slaveErrorMin = 252
    
    errors = { 'SLAVE_ECHO_FAILED': chr(0),                  # Errors detected by the master
               'MISSING_NUL_TERMINATOR': chr(1),
               'MESSAGE_HAS_FORBIDDEN_CHARS': chr(252),      # Errors detected by the slave
               'COMMAND_NOT_RECOGNISED': chr(253),
               'NOT_READY': chr(254),
               'MASTER_ECHO_FAILED': chr(255) }
    
    # Slave select pins
    ssPins = { 'POWER_CONTROL': 5,
               'FRONT_LEFT': 6,
               'FRONT_RIGHT': 13,
               'BACK_LEFT': 19,
               'BACK_RIGHT': 26 }
               
    bufferSize = 32
    attemptsPerByte = 3
    
    # SPI delay between transfers (s)
    delay = 0.001

    def __init__(self, state):
        self.state = state
        
        # Dict of errors, containing only error codes detected by the slave. This subset of the "errors"
        # dict is extracted using a Python "list comprehension"
        self.slaveErrors = dict((key, value) for key, value in SPI.errors.iteritems() if SPI.slaveErrorMin >= 252)
        
        GPIO.setmode(GPIO.BCM)
        GPIO.setwarnings(False)
  
        for ssPin in SPI.ssPins.itervalues():
            GPIO.setup(ssPin, GPIO.OUT) # set SS pin as an output
            GPIO.output(ssPin, GPIO.HIGH) # set SS pin to high

        self.spi = spidev.SpiDev()
        self.spi.open(0,0)
        self.spi.mode = 0b00
        self.spi.cshigh = False
        self.spi.bits_per_word = 8
        
        
    
    def sendString(self, channel, command, string):
        """
        Sends a string across to a SPI slave
        """
        # Convert string into a list of characters
        outgoingBuffer = list(string)
        
        # Trim the string to fit inside the slave's buffer
        outgoingBuffer = outgoingBuffer[:(SPI.bufferSize - 1)]
        
        # NUL terminate the string
        outgoingBuffer.append('\0')
        
        # Prepend the SPI command to the buffer
        outgoingBuffer.insert(0, SPI.commands[command])
        
        # This prepares the slave for a new SPI operation
        GPIO.output(SPI.ssPins[channel], GPIO.LOW)
        
        completeSuccess = False
        
        try:
            # Make multiple attempts if the Arduino is busy, then move onto the next Arduino.
            # All other errors will cause the master to move on immediately.
            for outgoingByte in outgoingBuffer:
                for attempt in range(SPI.attemptsPerByte):
                    incomingByte = self.transferByte(outgoingByte)
                    
                    self.state.spiErrorCounts[channel]['NUM_TRANSFERS'] += 1
                    
                    if incomingByte == SPI.errors['NOT_READY']:
                        self.state.spiErrorCounts[channel]['NOT_READY'] += 1
                    
                    # No more attempts needed as slave has responded, either with error or with data
                    else:
                        break
                
                if incomingByte in self.slaveErrors:
                    self.state.spiErrorCounts[channel][incomingByte] += 1
                    break
                
            else:
                # For loop finished with no errors detected! (No breaks)
                completeSuccess = True

        # This guarantees that slave select goes back to high
        finally:
            GPIO.output(SPI.ssPins[channel], GPIO.HIGH)
            
        return completeSuccess

        
    def receiveString(self, channel, command):
        """
        Receives a string from an SPI slave
        The slave must have the string ready to send in a buffer
        When the slave receives the correct command returns the corresponding string
        """
        incomingBuffer = []
        
        GPIO.output(SPI.ssPins[channel], GPIO.LOW)
        self.transferByte(SPI.commands[command])
        
        completeSuccess = False
        
        try:
            # Make multiple attempts if the Arduino is busy, then move onto the next Arduino.
            # All other errors will cause the master to move on immediately.
            while True:
                for attempt in range(SPI.attemptsPerByte):
                    # Echo the received bytes to enable verification by the slave
                    incomingByte = self.transferByte('\0')
                    self.state.spiErrorCounts[channel]['NUM_TRANSFERS'] += 1
                    
                    if incomingByte == SPI.errors['NOT_READY']:
                        self.state.spiErrorCounts[channel]['NOT_READY'] += 1
                    
                    # No more attempts needed as slave has responded, either with error or with data
                    else:
                        break
                        
                if incomingByte in self.slaveErrors:
                    self.state.spiErrorCounts[channel][incomingByte] += 1
                    break
                    
                # We have received the last character.
                # Don't actually append the NUL terminator to the buffer, because Python doesn't
                # NUL terminate its strings.
                if incomingByte == '\0':
                    completeSuccess = True
                    break
                
                elif len(incomingBuffer) >= SPI.bufferSize:
                    self.state.spiErrorCounts[channel]['MISSING_NUL_TERMINATOR'] += 1
                    break
                    
                incomingBuffer.append(incomingByte)
                
        
        # This guarantees that slave select goes back to high            
        finally:
            GPIO.output(SPI.ssPins[channel], GPIO.HIGH)

        if completeSuccess:
            return "".join(incomingBuffer)
            
        return None
        
        
    def transferByte(self, outgoingByte):
        """
        Send a single byte across to the slave
        Assumes slave select has already been pulled low
        To receive a byte you must use this function multiple times.
        
        Receiving Example:
        transferByteAndWait(COMMAND)
        transferByteAndWait(DUMMY)
        RECIEVED_BYTE = transferByteAndWait(DUMMY)
        """
        time.sleep(SPI.delay)
        
        # If outgoingByte passed in as a char, convert to an ASCII int
        outgoingByte = ord(outgoingByte)
        incomingByte = self.spi.xfer2([outgoingByte])
        
        # Convert the int value into a char and return it
        return chr(incomingByte.pop())
        
    
    def readArduinoState(self, channel):
        try:
            angle = self.receiveString(channel, 'LOAD_A_MEASURED_ORIENTATION')
            actuator = self.receiveString(channel, 'LOAD_A_CONTROLLER_OUTPUT')
            
            # Check for null values
            if not angle or not actuator:
                return False
                
            with self.state.lock:
                self.state.wheels['FRONT_LEFT'].aMeasuredOrientation = int(angle)
                self.state.wheels['FRONT_LEFT'].aThrottle = int(actuator)
            
        except ValueError:
            # SPI did not return the expected value / is not online
            return False
            
        else:
            return True
                
        
    def run(self):
        # Wait until SPI is online and then set the current wheel position
        # as the wheel angle setpoint.
        
        print("Establishing SPI connection to Arduino...")
        
        while not self.readArduinoState('FRONT_LEFT'):
            if not self.state.running:
                return
            
        print("Success.")
            
        with self.state.lock:
            self.state.wheels['FRONT_LEFT'].setADesiredOrientation(self.state.wheels['FRONT_LEFT'].aMeasuredOrientation)
            
        
        while self.state.running:
            self.sendString('FRONT_LEFT', 'RECEIVE_A_ORIENTATION', str(self.state.wheels['FRONT_LEFT'].aDesiredOrientation))
            self.sendString('FRONT_LEFT', 'RECEIVE_HM_STATE', str(self.state.wheels['FRONT_LEFT'].hmMode))
            self.sendString('FRONT_LEFT', 'RECEIVE_HM_THROTTLE', str(self.state.wheels['FRONT_LEFT'].hmThrottle))
            
            self.readArduinoState('FRONT_LEFT')
