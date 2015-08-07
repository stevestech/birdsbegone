class States:
    NEUTRAL = 0
    BRAKING = 1
    FORWARD = 2
    REVERSE = 3
    
class Directions:
    FORWARD = 0
    BACKWARD = 1

class Wheel:
    def __init__(self):
        self.state = States.NEUTRAL
        self.throttle = 0
        self.desiredAngle = 0
        
        # TODO:
        # self.measuredAngle = SPI.getCurrentAngle()
        # self.desiredAngle = self.measuredAngle
        
        # Measured values
        self.measuredWheelRPM = 0
        self.measuredDirecton = Directions.FORWARD
        self.measuredCurrent = 0
        self.measuredAngle = 0
        
    def setState(self, state):
        # When changing to a different state, always set throttle to 0
        if state != self.state:
            self.setThrottle(0)
        
        # If state passed as string
        if isinstance(state, basestring):
            state = state.lower()
            
            if state == "neutral":
                self.state = States.NEUTRAL
            
            elif state == "forward":
                self.state = States.FORWARD
                
            elif state == "reverse":
                self.state = States.REVERSE
                
            elif state == "braking":
                self.state = States.BRAKING
                
            else:
                raise ValueError("Wheel state was set to an invalid value: " + str(state))
                
                
        elif 0 <= state <= 3:
            self.state = state
            
        else:
            raise ValueError("Wheel state was set to an invalid value: " + str(state))
            
            
    def setThrottle(self, throttle):
        if isinstance(throttle, basestring):
            throttle = int(throttle)
        
        if 0 <= throttle <= 255:
            self.throttle = throttle
            
        else:
            self.setState(States.NEUTRAL)
            raise ValueError("Wheel throttle was set to an invalid value: " + str(throttle))
            
            
    def setDesiredAngle(self, angle):
        if isinstance(angle, basestring):
            angle = int(angle)
        
        if -720 <= angle <= 720:
            self.desiredAngle = angle
            
        else:
            raise ValueError("Wheel angle was set to an invalid value: " + str(angle))
        
        
    def getStateAsString(self):
        if self.state == States.NEUTRAL:
            return "neutral"
            
        elif self.state == States.FORWARD:
            return "forward"
            
        elif self.state == States.BRAKING:
            return "braking"

        elif self.state == States.REVERSE:
            return "reverse"
            
        else:
            raise ValueError("Encountered an unexpected wheel state: " + str(self.state))
