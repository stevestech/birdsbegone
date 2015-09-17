import threading
import json

from wheel import Wheel
from visionTrackingResult import VisionTrackingResult
from cameraCapture import Camera
from spiMaster import SPI

class State:
    def __init__(self):
        self.lock = threading.RLock()
        
        # Set to false when this application is shutting down
        self.running = True
        
        # If computer vision navigation is enabled
        self.autoPilot = False
        
        # When true, disable all motors
        self.emergencyStop = False
        
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
        
        state['WHEELS'] = {}
        for wheel in self.wheels:
            state['WHEELS'][wheel] = self.wheels[wheel].getJsonObject()
        
        return json.dumps(state)
