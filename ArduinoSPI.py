# Import required packages
import spidev
import time
import RPi.GPIO as GPIO


class Commands:
    SET_STATE = 'A'
    SET_THROTTLE = 'B'
    SET_ANGLE = 'C'


class SPI:
    def __init__(self, state, ss_pin=25, spi_delay=1000):
        """
        Handle SPI communication with a slave
        Note the slave must be programmed in a specific way to work with this code
        """
        self.running = True
        
        self.state = state
        
        self.ss_pin = ss_pin # Slave Select Pin
        self.spi_delay = spi_delay # SPI delay between transfers (us)
        
        GPIO.cleanup()
        GPIO.setmode(GPIO.BCM)
        GPIO.setup(ss_pin, GPIO.OUT)
        GPIO.output(ss_pin, 1) # set SS high

        self.spi = spidev.SpiDev()
        self.spi.open(0,0)
    
    
    def sendString(self, string):
        """
        Sends a string across to a SPI slave
        String length currently limited to length of 12, can modify on slave.
        """
        send_list = list(string)
        send_list = send_list[:12]
        send_list.append(chr(3)) # 3 = ETX (End of Text) to signify string finished 
        
        GPIO.output(self.ss_pin, 0) # set SS low
        self.transferByteAndWait(16) # Send command 16 to signify string being sent
        for send_byte in send_list:
            self.transferByteAndWait(ord(send_byte))
        GPIO.output(self.ss_pin, 1) # set SS high
        
    def recieveString(self, command):
        """
        Recieves a string from an SPI slave
        The slave must have the string ready to send in a buffer
        When the slave recieves the correct command returns the corresponding string
        """
        recieve_byte = [0]
        recieved_list = []
        GPIO.output(self.ss_pin, 0) # set SS low
        self.transferByteAndWait(command) # Send command asking for string
        self.transferByteAndWait(0) # Sending dummy data while waiting for response
        while (recieve_byte[0] !=  3): # 3 = ETX (End of Text) to signify recieving finished 
            recieve_byte = self.transferByteAndWait(0) # Recieve a byte from the slave
            recieved_list.append(chr(recieve_byte[0])) # Append the character to the list
            print recieve_byte
            if (len(recieved_list) >= 10): # If don't recieve end of text, break
                recieved_list = []
                break
        GPIO.output(self.ss_pin, 1) # set SS high
        
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
        To recieve a byte you must use this function multiple times.
        
        Recieving Example:
        transferByteAndWait(COMMAND)
        transferByteAndWait(DUMMY)
        RECIEVED_BYTE = transferByteAndWait(DUMMY)
        """
        recieve_byte = self.spi.xfer2([send_byte])
        time.sleep(self.spi_delay/1000000.0)
        return recieve_byte
        
    def run(self):
        while self.running:
            # No time.sleep() needed as we have spi_delay
            self.sendString(self.state.getStateAsSPIMessage())




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