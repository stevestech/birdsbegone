import socket
import json

class Networking:
    def __init__(self, state, port=1234):
        self.running = True
        
        # Which port to connect to
        self.port = port
        
        # Holds instance of the robot class, used for updating the state of the robot after receiving network commands
        self.state = state
        
        # How much data to read from a socket
        self.bufferSize = 4096



    def run(self):
        self.serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        # Reuse the same address to stop those pesky [Errno 98] Address already in use
        self.serversocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.serversocket.bind(('localhost', self.port))        
        self.serversocket.listen(5)
        self.serversocket.settimeout(2)        
        
        while self.running:
            try:
                client, address = self.serversocket.accept()
                
            except (socket.error, socket.timeout):
                continue
            
            message = client.recv(self.bufferSize)
                    
            # DEBUG:
            print("Webclient sent: " + message)
            
            if message.startswith("{") and message.endswith("}"):
                self.executeJSON(message)
            
                # Send current state to the web client
                client.sendall(self.state.getStateAsJSON())
                client.close()
                
            else:
                client.sendall("retry")
                client.close()
                

        self.serversocket.shutdown(socket.SHUT_RDWR)
        self.serversocket.close()
            
                            
                        
    def executeJSON(self, message):
        try:
            with self.state.lock:
                messageDict = json.loads(message)
                
                # Set the throttle
                if messageDict["cmd"] == "setThrottle":
                    if (messageDict["wheel"] == "fl") or (messageDict["wheel"] == "all"):
                        self.state.frontLeftWheel.setThrottle(messageDict["throttle"])
                        
                    if (messageDict["wheel"] == "fr") or (messageDict["wheel"] == "all"):
                        self.state.frontRightWheel.setThrottle(messageDict["throttle"])
                        
                    if (messageDict["wheel"] == "bl") or (messageDict["wheel"] == "all"):
                        self.state.backLeftWheel.setThrottle(messageDict["throttle"])
                        
                    if (messageDict["wheel"] == "br") or (messageDict["wheel"] == "all"):
                        self.state.backRightWheel.setThrottle(messageDict["throttle"])
                        
                    
                # Set the hub motor state
                elif messageDict["cmd"] == "setState":
                    if (messageDict["wheel"] == "fl") or (messageDict["wheel"] == "all"):
                        self.state.frontLeftWheel.setState(messageDict["state"])
                        
                    if (messageDict["wheel"] == "fr") or (messageDict["wheel"] == "all"):
                        self.state.frontRightWheel.setState(messageDict["state"])
                        
                    if (messageDict["wheel"] == "bl") or (messageDict["wheel"] == "all"):
                        self.state.backLeftWheel.setState(messageDict["state"])
                        
                    if (messageDict["wheel"] == "br") or (messageDict["wheel"] == "all"):
                        self.state.backRightWheel.setState(messageDict["state"])
                        
                # Set the wheel's desired angle
                elif messageDict["cmd"] == "setAngle":
                    if (messageDict["wheel"] == "fl") or (messageDict["wheel"] == "all"):
                        self.state.frontLeftWheel.setDesiredAngle(messageDict["angle"])
                        
                    if (messageDict["wheel"] == "fr") or (messageDict["wheel"] == "all"):
                        self.state.frontRightWheel.setDesiredAngle(messageDict["angle"])
                        
                    if (messageDict["wheel"] == "bl") or (messageDict["wheel"] == "all"):
                        self.state.backLeftWheel.setDesiredAngle(messageDict["angle"])
                        
                    if (messageDict["wheel"] == "br") or (messageDict["wheel"] == "all"):
                        self.state.backRightWheel.setDesiredAngle(messageDict["angle"])
                                
            
        except (ValueError, KeyError) as e:
            return
            
            
          
