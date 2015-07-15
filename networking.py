import socket
import re

class Networking:
    def __init__(self, port, robot)
        # Which port to connect to
        self.port = port
        
        # Holds instance of the robot class, used for updating the state of the robot after receiving network commands
        self.robot = robot
        
        # Used to terminate this thread
        self.running = True
        
        # How much data to read from a socket
        self.bufferSize = 4096

        self.serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.serversocket.bind(('localhost', self.port))
        self.serversocket.listen(5)
        
    def run(self):
        while self.running:
            connection, address = serversocket.accept()
            message = connection.recv(self.bufferSize)
            
            # A complete command will terminate in a ) character.
            completeMessage = re.search(r'\)$', message)
            if completeMessage:
                # Obtains the name of the command in the format "commandName(param1, param2)"
                commandName = re.search(r'(^.+)\(', message).group(1)
            

          
