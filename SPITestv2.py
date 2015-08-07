# Import required packages
import spidev
import time
import RPi.GPIO as GPIO

SLAVE_SELECT_PIN = 25
TIME_DELAY = 1000 # in us (100 default)
CLOCK_SPEED_HZ = 16000000/8

def main():
    
    
    GPIO.cleanup()
    GPIO.setmode(GPIO.BCM)
    GPIO.setup(SLAVE_SELECT_PIN, GPIO.OUT)
    GPIO.output(SLAVE_SELECT_PIN, 1) # set SS high

    spi = spidev.SpiDev()
    spi.open(0,0)
    #spi.max_speed_hz=CLOCK_SPEED_HZ
    
    angle = 22
    
    while True:
        send_str = raw_input("Enter message to send: ")
        send_list = list(send_str)
        send_list.append(chr(3)) # 3 = ETX (End of Text)

        GPIO.output(SLAVE_SELECT_PIN, 0) # set SS low
        spi.xfer2([16]) # Send command 16 for sending text
        time.sleep(TIME_DELAY/1000000.0) # Delay
        for send_byte in send_list:
            spi.xfer2([ord(send_byte)])
            print("Sent: %s" % send_byte)
            time.sleep(TIME_DELAY/1000000.0)
        
        GPIO.output(SLAVE_SELECT_PIN, 1) # set SS high

        time.sleep(TIME_DELAY/1000000.0) # Delay
        
        GPIO.output(SLAVE_SELECT_PIN, 0) # set SS low
        recieve_byte = spi.xfer2([17]) # Send command 17 for angle
        time.sleep(TIME_DELAY/1000000.0) # Delay
        print("Sent 17: "),
        print recieve_byte[0]
        recieve_byte = spi.xfer2([0]) # Send dummy data and will poll for the angle
        time.sleep(TIME_DELAY/1000000.0) # Delay
        print("Sent 0: "),
        print recieve_byte[0]
        recieve_byte = spi.xfer2([0]) # Recieve the angle using final dummy data
        time.sleep(TIME_DELAY/1000000.0) # Delay
        angle = recieve_byte[0]
        print("Recieve1: "),
        print angle
        
        GPIO.output(SLAVE_SELECT_PIN, 1) # set SS high

        print("Complete!")
        print("")

if __name__ == '__main__':
	main()
