import socket
import json
from hubMotor import States

class Networking:
    def __init__(self, port, robot):
        # Which port to connect to
        self.port = port
        
        # Holds instance of the robot class, used for updating the state of the robot after receiving network commands
        self.robot = robot
        
        # How much data to read from a socket
        self.bufferSize = 4096

        self.serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.serversocket.bind(('localhost', self.port))
        self.serversocket.listen(5)
        
    def run(self):
        while True:
            connection, address = self.serversocket.accept()
            
            while True:
                # Repeat until message fully read (message is null)
                message = connection.recv(self.bufferSize)
                
                if not message:
                    break
                    
                self.executeJSON(message)
                
                        
    def executeJSON(self, message):
        try:
            messageDict = json.loads(message)
            
            # Set the throttle
            if messageDict["commandName"] == "setThrottle":
                self.robot.setThrottle(int(messageDict["throttle"]))
                
            # Set the hub motor state
            elif messageDict["commandName"] == "setState":
                if messageDict["state"] == "neutral":
                    self.robot.setState(States.NEUTRAL)
                    
                if messageDict["state"] == "braking":
                    self.robot.setState(States.BRAKING)
                    
                if messageDict["state"] == "forward":
                    self.robot.setState(States.FORWARD)
                    
                if messageDict["state"] == "reverse":
                    self.robot.setState(States.REVERSE)
            
        except (ValueError, KeyError) as e:
            print(e.message)
            return
            
            
          
