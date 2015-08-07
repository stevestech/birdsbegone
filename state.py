import threading
import json

from wheel import Wheel
from visionTrackingResult import VisionTrackingResult
from ArduinoSPI import Commands

class Modes:
    MANUAL = 0
    AUTO = 1

class State:
    def __init__(self):
        self.lock = threading.RLock()
        
        self.mode = Modes.MANUAL
        
        self.frontLeftWheel = Wheel()
        self.frontRightWheel = Wheel()
        self.backLeftWheel = Wheel()
        self.backRightWheel = Wheel()
        
        self.leftVisionResult = VisionTrackingResult()
        self.rightVisionResult = VisionTrackingResult()
        self.frontVisionResult = VisionTrackingResult()
        self.backVisionResult = VisionTrackingResult()
        
        
    def getModeAsString(self):
        if self.mode == Modes.MANUAL:
            return "manual"
            
        elif self.mode == Modes.AUTO:
            return "auto"
            
        else:
            raise ValueError('state.mode was set to an unexpected value: ' + str(self.mode))        
        
        
    def getStateAsJSON(self):
        state = {}
            
        state["mode"] = self.getModeAsString()
        
        state["fl-state"] = self.frontLeftWheel.getStateAsString()
        state["fl-throttle"] = self.frontLeftWheel.throttle
        state["fl-angle"] = self.frontLeftWheel.desiredAngle
        
        state["fr-state"] = self.frontRightWheel.getStateAsString()
        state["fr-throttle"] = self.frontRightWheel.throttle
        state["fr-angle"] = self.frontRightWheel.desiredAngle
        
        state["bl-state"] = self.backLeftWheel.getStateAsString()
        state["bl-throttle"] = self.backLeftWheel.throttle
        state["bl-angle"] = self.backLeftWheel.desiredAngle
        
        state["br-state"] = self.backRightWheel.getStateAsString()
        state["br-throttle"] = self.backRightWheel.throttle
        state["br-angle"] = self.backRightWheel.desiredAngle
        
        return json.dumps(state)
        
        
    def getStateAsSPIMessage(self):
        """
        SPI commands are in the format
        AxByyyCzzzzz
        
        x indicates the state        
        x=0 neutral
        x=1 braking
        x=2 forward
        x=3 reverse
        
        y is the throttle, between 0 and 255
        
        z is the wheel angle, between -1800 and 1800
        
        string.format is used to ensure that the numbers are padded with 
        enough zeroes to maintain the format described above.
        """
        
        state = Commands.SET_STATE
        state = state + "{:01d}".format(self.frontLeftWheel.state)
        
        state = state + Commands.SET_THROTTLE
        state = state + "{:03d}".format(self.frontLeftWheel.throttle)
        
        state = state + Commands.SET_ANGLE
        state = state + "{:05d}".format(self.frontLeftWheel.desiredAngle)
        
        return state
