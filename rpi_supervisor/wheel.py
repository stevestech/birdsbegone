class Wheel:
    channels = { 'FRONT_LEFT': 0,
                 'FRONT_RIGHT': 1,
                 'BACK_LEFT': 2,
                 'BACK_RIGHT': 3 }
                 
    modes = { 'NEUTRAL': 0,
              'BRAKING': 1,
              'FORWARD': 2,
              'REVERSE': 3 }
              
    PWM_MIN = 0
    PWM_MAX = 255
    
    ANALOGUE_MIN = 0
    ANALOGUE_MAX = 1023

    def __init__(self):
        # Values stored in the supervisor
        self.hmMode = Wheel.modes['NEUTRAL']
        self.hmThrottle = 0
        self.aDesiredOrientation = 0
               
        # Values obtained from the arduino
        self.hmRpm = 0
        self.aMeasuredOrientation = 0
        self.aThrottle = 0
        
    def setHmMode(self, mode):
        # When changing to a different mode, always set throttle to 0
        if mode != self.hmMode:
            self.setHmThrottle(0)
        
        # Check that the new mode is a valid value
        if mode.upper() in Wheel.modes:
            self.hmMode = Wheel.modes[mode.upper()]
            
        else:
            raise ValueError("Hub motor mode was set to an invalid value: " + str(mode))
            
            
    def getHmModeAsString(self):
        # Lookup dictionary key given its value (don't judge me!)
        for key, value in Wheel.modes.iteritems():
            if self.hmMode == value:
                return key
                
        raise ValueError("Couldn't find hub motor mode key with value: " + str(self.hmMode))
            
            
    def setHmThrottle(self, throttle):
        if isinstance(throttle, basestring):
            throttle = int(throttle)
        
        if Wheel.PWM_MIN <= throttle <= Wheel.PWM_MAX:
            self.hmThrottle = throttle
            
        else:
            self.setHmMode(modes['NEUTRAL'])
            raise ValueError("Wheel throttle was set to an invalid value: " + str(throttle))
            
            
    def setADesiredOrientation(self, orientation):
        if isinstance(orientation, basestring):
            orientation = int(orientation)
        
        if Wheel.ANALOGUE_MIN <= orientation <= Wheel.ANALOGUE_MAX:
            self.aDesiredOrientation = orientation
            
        else:
            raise ValueError("Wheel orientation was set to an invalid value: " + str(orientation))
            
            
    # Used to produce a JSON string
    def getJsonObject(self):
        state = {}
        
        state['HM_MODE'] = self.getHmModeAsString()
        state['HM_THROTTLE'] = self.hmThrottle
        state['HM_RPM'] = self.hmRpm
        state['A_THROTTLE'] = self.aThrottle
        state['A_ANGLE_M'] = self.aMeasuredOrientation
        state['A_ANGLE_D'] = self.aDesiredOrientation
        
        return state
        
