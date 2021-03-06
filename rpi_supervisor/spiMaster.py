# Import required packages
import spidev
import time
import RPi.GPIO as GPIO
import subprocess
from wheel import Wheel

class SPI:
    """
    SLAVE COMMANDS
    These imperatives are sent from the SPI master and
    inform the slave how to behave.
    """
    commands = { # WHEEL HUB COMMANDS
                 'RECEIVE_A_ORIENTATION': chr(0),            # Prepare the slave to receive data from the master
                 'RECEIVE_HM_STATE': chr(1),
                 'RECEIVE_HM_THROTTLE': chr(2),
                 
                 'LOAD_A_MEASURED_ORIENTATION': chr(100),    # Instruct the slave to load data onto its send buffer for the master to read
                 'LOAD_A_CONTROLLER_OUTPUT': chr(101),
                 'LOAD_EMERGENCY_STOP': chr(102),
                 'LOAD_A_STATUS_L': chr(103),
                 'LOAD_A_STATUS_R': chr(104),
                 'LOAD_HM_SPEED': chr(105),
                 
                 'SET_EMERGENCY_STOP': chr(255),
                 
                 # CENTRAL CONTROL COMMANDS
                 'RECEIVE_SHUTDOWN_CMD': chr(0),
                 'RECEIVE_RUNNING_CMD': chr(1),
                 'RECEIVE_EMERGENCY_STOP_CMD': chr(2),
                 'RECEIVE_POWER_DOWN_CMD': chr(3),
                 
                 'LOAD_SHUTDOWN_STATUS': chr(100),
                 'LOAD_24V_READING': chr(101),
                 'LOAD_12V_READING': chr(102),
                 'LOAD_ENERGY_CONSUMED': chr(103),
                 'LOAD_CENTRAL_ARDUINO_STATE': chr(104)
                 
                 }
    
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
    ssPins = { 'FRONT_LEFT': 5,             # 1
               'FRONT_RIGHT': 6,            # 2
               'BACK_RIGHT': 13,            # 3
               'BACK_LEFT': 19,             # 4
               'POWER_CONTROL': 26 }        # 5
               
    bufferSize = 32
    attemptsPerByte = 3
    
    # SPI delay between transfers (s)
    delay = 0.001

    def __init__(self, state):
        self.state = state
        
        # Dict of errors, containing only error codes detected by the slave. This subset of the "errors"
        # dict is extracted using a Python "list comprehension"
        self.slaveErrors = dict((key, value) for key, value in SPI.errors.iteritems() if ord(value) >= SPI.slaveErrorMin)
        
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
        
        
    
    def sendString(self, channel, command, string=''):
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
        
    
    def readWheelModuleState(self, channel):
        try:
            angle = self.receiveString(channel, 'LOAD_A_MEASURED_ORIENTATION')
            actuator = self.receiveString(channel, 'LOAD_A_CONTROLLER_OUTPUT')
            
            # Check for null values
            if not angle or not actuator:
                return False
                
            with self.state.lock:
                self.state.wheels[channel].aMeasuredOrientation = int(angle)
                self.state.wheels[channel].aThrottle = int(actuator)
            
        except ValueError:
            # SPI did not return the expected value / is not online
            return False
            
        else:
            return True
    
    def readCentralArduinoState(self, channel):
        try:
            state = self.receiveString(channel, 'LOAD_CENTRAL_ARDUINO_STATE')
            
            if not state:
                return False
                
            with self.state.lock:
                self.state.central_arduino_state = int(state)
        
        except ValueError:
            return False
            
        else:
            return True
            
    def readCentralArduinoBattery(self, channel):
        try:
            battery24v = self.recieveString(channel, 'LOAD_24V_READING')
            battery12v = self.recieveString(channel, 'LOAD_12V_READING')
            energy_consumed = self.recieveString(channel, 'LOAD_ENERGY_CONSUMED')
            
            if not battery24v or not battery12v or not energy_consumed:
                return False
            
            """    
            with self.state.lock:
                self.state.robot_state = int(state)
            """
        
        except ValueError:
            return False
            
        else:
            return True
        
    def run(self):
        # Wait until SPI is online and then set the current wheel position
        # as the wheel angle setpoint.
        
        """
        print("Connecting to front left Arduino...")
        
        while not self.readWheelModuleState('FRONT_LEFT'):
            if not self.state.running:
                return
        
        print("Connecting to front right Arduino...")
        
        while not self.readWheelModuleState('FRONT_RIGHT'):
            if not self.state.running:
                return
        
        print("Connecting to back right Arduino...")
        
        while not self.readWheelModuleState('BACK_RIGHT'):
            if not self.state.running:
                return
        """
        print("Connecting to back left Arduino...")
        with self.state.lock:
            self.state.errorMessage = "Connecting to back left Arduino..."
        
        while not self.readWheelModuleState('BACK_LEFT'):
            if not self.state.running:
                return
        
        print("Connecting to central control Arduino...")
        with self.state.lock:
            self.state.errorMessage = "Connecting to central control Arduino..."
            
        while not self.readCentralArduinoState('POWER_CONTROL'):
            if not self.state.running:
                return
                
        print("SPI devices are online.")
        with self.state.lock:
            self.state.errorMessage = ""

        # Initial steering actuator setpoint should be equal to the wheel's present position
        with self.state.lock:
            self.state.wheels['FRONT_LEFT'].setADesiredOrientation(self.state.wheels['FRONT_LEFT'].aMeasuredOrientation)
            self.state.wheels['FRONT_RIGHT'].setADesiredOrientation(self.state.wheels['FRONT_RIGHT'].aMeasuredOrientation)
            self.state.wheels['BACK_RIGHT'].setADesiredOrientation(self.state.wheels['BACK_RIGHT'].aMeasuredOrientation)
            self.state.wheels['BACK_LEFT'].setADesiredOrientation(self.state.wheels['BACK_LEFT'].aMeasuredOrientation)
        
        print("Setting robot state to running...")
        
        # Send command to central arduino saying that rpi has been turned on
        # Wait until confirmed that Arduino is in running state, otherwise continually send set running command
        while(self.state.central_arduino_state != self.state.robot_states['RUNNING']):
            self.sendString('POWER_CONTROL', 'RECEIVE_RUNNING_CMD')
            self.readCentralArduinoState('POWER_CONTROL')
        
        with self.state.lock:
            self.state.robot_state = self.state.robot_states['RUNNING'] # Set central state to running
        
        print("Robot in running state!")
        
        while self.state.running:
            for channel in Wheel.channels:            
                self.sendString(channel, 'RECEIVE_A_ORIENTATION', str(self.state.wheels[channel].aDesiredOrientation))
                self.sendString(channel, 'RECEIVE_HM_STATE', str(self.state.wheels[channel].hmMode))
                self.sendString(channel, 'RECEIVE_HM_THROTTLE', str(self.state.wheels[channel].hmThrottle))
                self.readWheelModuleState(channel)
                
                with self.state.lock:
                    if (self.receiveString(channel, 'LOAD_EMERGENCY_STOP') == 1):
                        self.state.errorMessage = channel + " has emergency stopped."
            
            # Update central arduino state
            self.readCentralArduinoState('POWER_CONTROL')
            # If either central Arduino or robot is in emergency stop state, set both to emergency stop 
            if (self.state.robot_states['EMERGENCY_STOP'] in [self.state.central_arduino_state, self.state.robot_state]):
                self.sendString('POWER_CONTROL', 'RECEIVE_EMERGENCY_STOP_CMD')
                with self.state.lock:
                    self.state.central_arduino_state = self.state.robot_states['EMERGENCY_STOP']
                    self.state.robot_state = self.state.robot_states['EMERGENCY_STOP']
                    
            # If either central Arduino or robot is in shutdown state, set both to shutdown
            if (self.state.robot_states['SHUTTING_DOWN'] in [self.state.central_arduino_state, self.state.robot_state]):
                self.sendString('POWER_CONTROL', 'RECEIVE_SHUTDOWN_CMD')
                with self.state.lock:
                    self.state.central_arduino_state = self.state.robot_states['SHUTTING_DOWN']
                    self.state.robot_state = self.state.robot_states['SHUTTING_DOWN']
                #subprocess.call(["shutdown", "-h", "now"]) # call the shutdown command
                
            # If power down mode is set on the pi, then swap the central arduino to power down mode and everything will shutdown instantly
            if (self.state.robot_states['POWER_DOWN'] == self.state.robot_state):
                self.sendString('POWER_CONTROL', 'RECEIVE_POWER_DOWN_CMD')
                
            # readCentralArduinoBattery('POWER_CONTROL') # FUNCTION UNDER CONSTRUCTION
            
            # SPI PLAN
            
            # STARTUP
            #   - Wait until recieve response from central arduino (check if state is 'starting up')
            #   - Wait until recieve response from Arduino's 1,2,3 & 4 (Read pot measurement)
            #   - Set central arduino to running state
            
            # Update Arduino 1, 2, 3 & 4 (Francis Stuff!)
            #   - Send across desired wheel orientation
            #   - Send acroos desired hub motor state
            #   - Send across desured hub motor throttle
            #   - Read the actuator angle
            #   - Read the controller output
            
            # Update Central Arduino
            #   - Read the state of the Arduino
            #   - Compare RPi state to read arduino state, decide which is high priority
            #   - Update Arduino state if nessecary,
            #   - Battery 1 & 2 Read
            #   - Current Transducer Read
