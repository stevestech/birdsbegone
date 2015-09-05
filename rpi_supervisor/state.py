import threading
import json

from wheel import Wheel
from wheel import Channels
from visionTrackingResult import VisionTrackingResult

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
        
        
    def getWheel(self, channel):
        if channel == Channels.FRONT_LEFT:
            return self.frontLeftWheel
            
        if channel == Channels.FRONT_RIGHT:
            return self.frontRightWheel
            
        if channel == Channels.BACK_LEFT:
            return self.backLeftWheel
            
        if channel == Channels.BACK_RIGHT:
            return self.backRightWheel
            
        raise ValueError('Invalid channel sent to State.getWheel(): ' + str(channel))
    
    
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
        state["fl-angle-pot"] = self.frontLeftWheel.measuredAngle
        state["fl-actuator"] = self.frontLeftWheel.actuator
        
        #state["fr-state"] = self.frontRightWheel.getStateAsString()
        #state["fr-throttle"] = self.frontRightWheel.throttle
        #state["fr-angle"] = self.frontRightWheel.desiredAngle
        #state["fr-current"] = self.frontRightWheel.measuredCurrent
        
        #state["bl-state"] = self.backLeftWheel.getStateAsString()
        #state["bl-throttle"] = self.backLeftWheel.throttle
        #state["bl-angle"] = self.backLeftWheel.desiredAngle
        
        #state["br-state"] = self.backRightWheel.getStateAsString()
        #state["br-throttle"] = self.backRightWheel.throttle
        #state["br-angle"] = self.backRightWheel.desiredAngle
        
        return json.dumps(state)
