import spidev
import RPi.GPIO as GPIO
import time

spi = spidev.SpiDev()
spi.open(0, 0)

GPIO.setmode(GPIO.BCM)
GPIO.setup(5, GPIO.OUT)
GPIO.output(5, GPIO.HIGH)
time.sleep(0.5)
GPIO.output(5, GPIO.LOW)

while True:
    outgoing = int(input("Send >"))
    print(str(spi.xfer2([outgoing])))
