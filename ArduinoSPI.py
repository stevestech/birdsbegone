# Import required packages
import spidev
import time
import RPi.GPIO as GPIO
import wheel

class Commands:
    SET_STATE = 'A'
    SET_THROTTLE = 'B'
    SET_ANGLE = 'C'
    
    GET_MEASURED_ANGLE = 17
    GET_ACTUATOR = 18


class SPI:
    def __init__(self, state, ss_pin_list, spi_delay=1000):
        """
        Handle SPI communication with a slave
        Note the slave must be programmed in a specific way to work with this code
        """
        self.running = True
	self.recieve_text_limit = 10
		
        self.state = state
        
		
        self.ss_pin_list = ss_pin_list # Slave Select Pin
        self.spi_delay = spi_delay # SPI delay between transfers (us)
        
	GPIO.cleanup()
	GPIO.setmode(GPIO.BCM)
		
	for ss_pin in ss_pin_list:
		GPIO.setup(ss_pin, GPIO.OUT) # set SS pin as an output
		GPIO.output(ss_pin, 1) # set SS pin to high

        self.spi = spidev.SpiDev()
        self.spi.open(0,0)
        
    
    
    def sendString(self, string, ss_pin_list_index):
        """
        Sends a string across to a SPI slave
        String length currently limited to length of 12, can modify on slave.
        """
	ss_pin = self.ss_pin_list[ss_pin_list_index]
        send_list = list(string)
        send_list = send_list[:12]
        send_list.append(chr(3)) # 3 = ETX (End of Text) to signify string finished 
        
        GPIO.output(ss_pin, 0) # set SS low
        self.transferByteAndWait(16) # Send command 16 to signify string being sent
        for send_byte in send_list:
            self.transferByteAndWait(ord(send_byte))
        GPIO.output(ss_pin, 1) # set SS high
        
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
        
    def transferByteAndWait(self, send_byte):
        """
        Send a single byte across to the slave
        Assumes slave select has already been pulled low
        To receive a byte you must use this function multiple times.
        
        Receiving Example:
        transferByteAndWait(COMMAND)
        transferByteAndWait(DUMMY)
        RECIEVED_BYTE = transferByteAndWait(DUMMY)
        """
        recieve_byte = self.spi.xfer2([send_byte])
        time.sleep(self.spi_delay/1000000.0)
        return recieve_byte
        
    
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
        
        while not self.readArduinoState(wheel.Channels.FRONT_LEFT):
            pass
            
        with self.state.lock:
            self.state.frontLeftWheel.desiredAngle = self.state.frontLeftWheel.measuredAngle
            
        
        while self.running:
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
