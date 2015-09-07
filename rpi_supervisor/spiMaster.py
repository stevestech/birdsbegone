# Import required packages
import spidev
import time
import RPi.GPIO as GPIO
import wheel

class SPI:
    """
    SLAVE COMMANDS
    These imperatives are sent from the SPI master and
    inform the slave how to behave.
    """
    commands = { 'RECEIVE_A_ORIENTATION': 0,            # Prepare the slave to receive data from the master
                 'RECEIVE_HM_STATE': 1,
                 'RECEIVE_HM_THROTTLE': 2,
                 'LOAD_A_MEASURED_ORIENTATION': 100,    # Instruct the slave to load data onto its send buffer for the master to read
                 'LOAD_A_CONTROLLER_OUTPUT': 101
                 'LOAD_SHUTDOWN_ERROR': 102 }
    
    """
    SPI ERROR MESSAGES
    "Errors detected by the slave" are reserved values and cannot be used inside messages
    to the slave or to the master.
    """
    SLAVE_ERROR_MIN = 252
    
    errors = { 'SLAVE_ECHO_FAILED': 0,                  # Errors detected by the master
               'MISSING_NUL_TERMINATOR': 1,
               'MESSAGE_HAS_FORBIDDEN_CHARS': 252,      # Errors detected by the slave
               'COMMAND_NOT_RECOGNISED': 253,
               'NOT_READY': 254,
               'MASTER_ECHO_FAILED': 255 }
    
    # Dict of errors, containing only error codes detected by the slave. This subset of the "errors"
    # dict is extracted using a Python "list comprehension"
    slaveErrors = dict((key, value) for key, value in errors.iteritems() if value >= SLAVE_ERROR_MIN)

    # Slave select pins
    ssPins = { 'POWER_CONTROL': 27,
               wheel.channels['FRONT_LEFT']: 22,
               wheel.channels['FRONT_RIGHT']: 23,
               wheel.channels['BACK_LEFT']: 24,
               wheel.channels['BACK_RIGHT']: 25 }
               
    bufferSize = 32

    def __init__(self, state):
        self.state = state
        
        # SPI delay between transfers (us)
        self.spiDelay = 1000
        
        GPIO.setmode(GPIO.BCM)
        GPIO.setwarnings(False)
  
        for ssPin in ssPins.itervalues():
            GPIO.setup(ssPin, GPIO.OUT) # set SS pin as an output
            GPIO.output(ssPin, GPIO.HIGH) # set SS pin to high

        self.spi = spidev.SpiDev()
        self.spi.open(0,0)
        
    
    def sendString(self, channel, command, string):
        """
        Sends a string across to a SPI slave
        """
        # Convert string into a list of characters
        buffer = list(string)
        
        # Trim the string to fit inside the slave's buffer
        buffer = buffer[:(bufferSize - 1)]
        
        # NUL terminate the string
        buffer.append('\0')
        
        # Prepend the SPI command to the buffer
        buffer.insert(0, command)
        
        # This prepares the slave for a new SPI operation
        GPIO.output(ssPins[channel], GPIO.LOW)
        
        # If an error is detected, give up straight away.
        # Move onto the next wheel module and come back to this one.
        for outgoingByte in buffer:
            incomingByte = self.transferByte(outgoingByte)
            
            self.state.spiErrorCounts[channel]['NUM_TRANSFERS'] += 1
            
            if incomingByte in slaveErrors:
                self.state.spiErrorCounts[channel][incomingByte] += 1
                break
            
        
              
        GPIO.output(ssPins[channel], GPIO.HIGH) # set SS high
        
    def recieveString(self, command, ss_pin_list_index):
        """
        Receives a string from an SPI slave
        The slave must have the string ready to send in a buffer
        When the slave receives the correct command returns the corresponding string
        """
        ss_pin = self.ss_pin_list[ss_pin_list_index]
        recieve_byte = [0]
        recieved_list = []
        GPIO.output(ss_pin, 0) # set SS low
        self.transferByteAndWait(command) # Send command asking for string
        self.transferByteAndWait(0) # Sending dummy data while waiting for response
        while (recieve_byte[0] !=  3): # 3 = ETX (End of Text) to signify receiving finished 
            recieve_byte = self.transferByteAndWait(0) # Receive a byte from the slave
            recieved_list.append(chr(recieve_byte[0])) # Append the character to the list
            if (len(recieved_list) >= self.recieve_text_limit): # If don't receive end of text after a certain character limit, break
                recieved_list = []
                break
        GPIO.output(ss_pin, 1) # set SS high
        
        if (len(recieved_list) != 0):
            del recieved_list[-1] # Delete the end of text command
            recieved_string = ''.join(recieved_list) # Join into a string
        else:
            recieved_string = "End of Text not recieved"
        return recieved_string
        
        
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
        return self.spi.xfer2([outgoingByte])
        
    
    def readArduinoState(self, channel):
        try:
            angle = int(self.recieveString(Commands.GET_MEASURED_ANGLE, channel))
            actuator = int(self.recieveString(Commands.GET_ACTUATOR, channel))
            
        except ValueError:
            # SPI did not return the expected value / is not online
            return False
            
        else:
            with self.state.lock:
                wheel = self.state.getWheel(channel)
                wheel.measuredAngle = angle
                wheel.actuator = actuator
                
            return True
                
        
    def run(self):
        # Wait until SPI is online and then set the current wheel position
        # as the wheel angle setpoint.
        
        print("Establishing SPI connection to Arduino...")
        
        while self.state.running and not self.readArduinoState(wheel.Channels.FRONT_LEFT):
            pass
            
        print("Success.")
            
        with self.state.lock:
            self.state.frontLeftWheel.desiredAngle = self.state.frontLeftWheel.measuredAngle
            
        
        while self.state.running:
            # No time.sleep() needed as we have spi_delay
            self.sendString(self.state.frontLeftWheel.getStateAsSPIMessage(), wheel.Channels.FRONT_LEFT)
            self.readArduinoState(wheel.Channels.FRONT_LEFT)
                        

"""
def main():
    arduinoSPI = SPI()
    
    while True:
        send_str = raw_input("Enter message to send: ")
        arduinoSPI.sendString(send_str)
        return_str = arduinoSPI.recieveString(17) # Retrieve string stored in command 17
        print return_str
if __name__ == '__main__':
 main()
"""
