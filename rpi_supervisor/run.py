import signal
import sys
import threading
import time
import RPi.GPIO as GPIO

from networking import Networking
from state import State
from spiMaster import SPI
from cameraCapture import Camera


def goodbye(signum, frame):
    state.running = False

    print("\nSPI thread is stopping...")
    spiThread.join()
    
#    print("Camera thread is stopping...")
#    cameraThread.join()
    
    print("Network thread is stopping...")
    networkThread.join()
    
    GPIO.cleanup()
    sys.exit(0)

    
if __name__ == "__main__":
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
#    camera = Camera(state)

    networkThread = threading.Thread(target=networking.run)
    spiThread = threading.Thread(target=spi.run)
#    cameraThread = threading.Thread(target=camera.run)

    networkThread.start()
    spiThread.start()
#    cameraThread.start()
    
    while True:
        # Wait for a signal
        time.sleep(1)
