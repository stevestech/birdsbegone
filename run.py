import signal
import sys
import threading
import time

from networking import Networking
from state import State
from ArduinoSPI import SPI

state = State()
networking = Networking(state)
spi = SPI(state)

networkThread = threading.Thread(target=networking.run)
spiThread = threading.Thread(target=spi.run)


def goodbye(signum, frame):
    spi.running = False
    networking.running = False

    print("SPI thread is stopping...")
    spiThread.join()
    
    print("Network thread is stopping...")
    networkThread.join()
    sys.exit(0)

    
# Register goodbye to handle all possible signals
for i in [x for x in dir(signal) if x.startswith("SIG")]:
    try:
        signum = getattr(signal, i)
        signal.signal(signum, goodbye)
    except (ValueError, RuntimeError):
        pass

        
networkThread.start()
spiThread.start()


if __name__ == "__main__":
    # Block so that the program doesn't terminate. Not necessary if running
    # from the interpreter ("import run" will execute the program and allow
    # access to member variables while running).
    while True:
        time.sleep(1)
