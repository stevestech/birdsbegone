from hubMotor import *
from networking import Networking
from threading import Thread
import time

hm = HubMotor(6, 5, 17, 7)
n = Networking(1234, hm)

networkThread = Thread(target=n.run)
networkThread.daemon = True
networkThread.start()

if __name__ == "__main__":
    # Block so that the program doesn't terminate. Not necessary if running
    # from the interpreter ("import run" will execute the program and allow
    # access to member variables while running).
    while True:
        time.sleep(1)
