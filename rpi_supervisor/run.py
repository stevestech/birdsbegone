import signal
import sys
import threading
import time
import RPi.GPIO as GPIO

from networking import Networking
from state import State
from ArduinoSPI import SPI
from cameraCapture import Camera

def goodbye(signum, frame):
    spi.running = False
    networking.running = False
    camera.running = False

    print("\nSPI thread is stopping...")
    spiThread.join()
    
    print("Camera thread is stopping...")
    cameraThread.join()
    
    print("Network thread is stopping...")
    networkThread.join()
    
    GPIO.cleanup()
    sys.exit(0)

    
# Register goodbye to handle all possible signals
for i in [x for x in dir(signal) if x.startswith("SIG")]:
    try:
        signum = getattr(signal, i)
        signal.signal(signum, goodbye)
    except (ValueError, RuntimeError):
        pass

        
state = State()
networking = Networking(state)
spi = SPI(state)
camera = Camera(state)

networkThread = threading.Thread(target=networking.run)
spiThread = threading.Thread(target=spi.run)
cameraThread = threading.Thread(target=camera.run)

networkThread.start()
spiThread.start()
cameraThread.start()

if __name__ == "__main__":
    # Block so that the program doesn't terminate. Not necessary if running
    # from the interpreter ("import run" will execute the program and allow
    # access to member variables while running).
    while True:
        time.sleep(1)