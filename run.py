from hubMotor import *
from networking import Networking
from threading import Thread

hm = HubMotor(6, 5, 17, 7)
n = Networking(1234, hm)

networkThread = Thread(target=n.run)
networkThread.daemon = True
networkThread.start()
