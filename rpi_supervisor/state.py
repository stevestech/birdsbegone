import threading
import json

from wheel import Wheel
from visionTrackingResult import VisionTrackingResult
from cameraCapture import Camera
from spiMaster import SPI

class State:
    robot_states = {
                    'STARTING_UP' : 0,
                    'RUNNING': 1,
                    'SHUTTING_DOWN': 2,
                    'EMERGENCY_STOP': 3,
                    'POWER_DOWN': 4,
                    'NULL': 100
                    }
    
    def __init__(self):
        self.lock = threading.RLock()
        
        # Set to false when this application is shutting down
        self.running = True
        
        # When true, disable all motors
        self.emergencyStop = False
        
        self.errorMessage = ""
        
        # Battery vars
        self.battery24v = 0;
        self.battery12v = 0;
        self.energy_consumed = 0;
        
        # Central Arduino State
        self.central_arduino_state = self.robot_states['STARTING_UP']
        self.robot_state = self.robot_states['STARTING_UP']
        
        # Kinematic state for each wheel module
        self.wheels = {}
        for channel in Wheel.channels:
            self.wheels[channel] = Wheel()
            
        # Keep a track of any errors encountered during SPI transfers
        self.spiErrorCounts = {}
        for channel in SPI.ssPins:
            self.spiErrorCounts[channel] = {}
            self.spiErrorCounts[channel]['NUM_TRANSFERS'] = 0
            
            for error in SPI.errors:
                self.spiErrorCounts[channel][error] = 0
        
        # Output from the computer vision processing, one for each side of the robot
        self.visionTrackingResults = {}
        for side in Camera.sides:
            self.visionTrackingResults[side] = VisionTrackingResult()
        
        
    def getStateAsJSON(self):
        state = {}
            
        state['ERRORS'] = self.spiErrorCounts
        state['ERRORS']['MSG'] = self.errorMessage
        
        state['WHEELS'] = {}
        for wheel in self.wheels:
            state['WHEELS'][wheel] = self.wheels[wheel].getJsonObject()
        
        return json.dumps(state)
