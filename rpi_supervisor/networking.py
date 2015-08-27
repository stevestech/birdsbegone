import socket
import json

class Networking:
    def __init__(self, state, port=1234):
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
        self.serversocket.bind(('0.0.0.0', self.port))        
        self.serversocket.listen(5)
        self.serversocket.settimeout(2)        
        
        while self.state.running:
            try:
                client, address = self.serversocket.accept()
                
            except (socket.error, socket.timeout):
                continue
            
            message = client.recv(self.bufferSize)
                    
            # DEBUG:
            #print("Webclient sent: " + message)
            
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
                if messageDict["cmd"] == "setHmThrottle":
                    if (messageDict["wheel"] == "fl") or (messageDict["wheel"] == "all"):
                        self.state.wheels['FRONT_LEFT'].setHmThrottle(messageDict["throttle"])
                        
                    if (messageDict["wheel"] == "fr") or (messageDict["wheel"] == "all"):
                        self.state.wheels['FRONT_RIGHT'].setHmThrottle(messageDict["throttle"])
                        
                    if (messageDict["wheel"] == "bl") or (messageDict["wheel"] == "all"):
                        self.state.wheels['BACK_LEFT'].setHmThrottle(messageDict["throttle"])
                        
                    if (messageDict["wheel"] == "br") or (messageDict["wheel"] == "all"):
                        self.state.wheels['BACK_RIGHT'].setHmThrottle(messageDict["throttle"])
                        
                    
                # Set the hub motor state
                elif messageDict["cmd"] == "setHmMode":
                    if (messageDict["wheel"] == "fl") or (messageDict["wheel"] == "all"):
                        self.state.wheels['FRONT_LEFT'].setHmMode(messageDict["mode"])
                        
                    if (messageDict["wheel"] == "fr") or (messageDict["wheel"] == "all"):
                        self.state.wheels['FRONT_RIGHT'].setHmMode(messageDict["mode"])
                        
                    if (messageDict["wheel"] == "bl") or (messageDict["wheel"] == "all"):
                        self.state.wheels['BACK_LEFT'].setHmMode(messageDict["mode"])
                        
                    if (messageDict["wheel"] == "br") or (messageDict["wheel"] == "all"):
                        self.state.wheels['BACK_RIGHT'].setHmMode(messageDict["mode"])
                        
                # Set the wheel's desired angle
                elif messageDict["cmd"] == "setActAngle":
                    if (messageDict["wheel"] == "fl") or (messageDict["wheel"] == "all"):
                        self.state.wheels['FRONT_LEFT'].setADesiredOrientation(messageDict["angle"])
                        
                    if (messageDict["wheel"] == "fr") or (messageDict["wheel"] == "all"):
                        self.state.wheels['FRONT_RIGHT'].setADesiredOrientation(messageDict["angle"])
                        
                    if (messageDict["wheel"] == "bl") or (messageDict["wheel"] == "all"):
                        self.state.wheels['BACK_LEFT'].setADesiredOrientation(messageDict["angle"])
                        
                    if (messageDict["wheel"] == "br") or (messageDict["wheel"] == "all"):
                        self.state.wheels['BACK_RIGHT'].setADesiredOrientation(messageDict["angle"])
                                
            
        except (ValueError, KeyError) as e:
            return
            
            
          
