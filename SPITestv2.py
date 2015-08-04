# Import required packages
import spidev
import time
import RPi.GPIO as GPIO

def main():
    GPIO.cleanup()
    GPIO.setmode(GPIO.BCM)
    GPIO.setup(24, GPIO.OUT)
    GPIO.output(24, 1) # set SS high

    spi = spidev.SpiDev()
    spi.open(0,0)
    
    while True:
        send_str = raw_input("Enter message to send: ")
        send_list = list(send_str)

        GPIO.output(24, 0) # set SS low
        spi.xfer2([116]) # Send command 't' for text
        time.sleep(20/1000000.0) # Delay 100us
        for send_byte in send_list:
            spi.xfer2([ord(send_byte)])
            time.sleep(20/1000000.0)
        print("Sent: %s" % send_str)
        time.sleep(10000/1000000.0)
        GPIO.output(24, 1) # set SS high
        
        time.sleep(100/1000000.0) # Delay 100us
        
        GPIO.output(24, 0) # set SS low
        spi.xfer2([ord('a')]) # Send command 'a' for angle
        time.sleep(20/1000000.0) # Delay 100us
        spi.xfer2([0]) # Send dummy data and will poll for the angle
        time.sleep(20/1000000.0) # Delay 100us
        recieve_byte = spi.xfer2([0]) # Recieve the angle using final dummy data
        time.sleep(20/1000000.0) # Delay 100us
        angle = recieve_byte[0]
        print angle
        GPIO.output(24, 1) # set SS high
        
        print("Complete!")
        print("")

if __name__ == '__main__':
	main()
