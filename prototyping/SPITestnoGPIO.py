# Import required packages
import spidev
import time

def main():
    spi = spidev.SpiDev()
    spi.open(0,0)
    
    while True:
        send_str = raw_input("Enter message to send: ")
        
        send_list = list(send_str)
        send_list.append("\n")
        for send_byte in send_list:
            spi.xfer2([ord(send_byte)], 8000000, 20)
        print("Message Sent!")
        print("")
        


if __name__ == '__main__':
	main()
