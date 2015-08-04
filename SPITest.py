# Import required packages
import spidev
import time
import RPi.GPIO as GPIO



def main():
    GPIO.cleanup()
    GPIO.setmode(GPIO.BCM)
    GPIO.setup(24, GPIO.OUT)
    GPIO.output(24, 1)
    
    spi = spidev.SpiDev()
    spi.open(0,0)
    
    while True:
        send_str = raw_input("Enter message to send: ")
        send_list = list(send_str)
        send_list.append("\n")
        
        GPIO.output(24, 0) # set SS low
        for send_byte in send_list:
            spi.xfer2([ord(send_byte)])
        GPIO.output(24, 1)
        print("Message Sent!")
        print("")
        
    # Need this for some reason or else it exits above loop?
    #while True:
    #    continue
    
	#return 0

if __name__ == '__main__':
	main()
