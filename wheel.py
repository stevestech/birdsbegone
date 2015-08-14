from ArduinoSPI import Commands

class Channels:
    FRONT_LEFT = 0
    FRONT_RIGHT = 1
    BACK_LEFT = 2
    BACK_RIGHT = 3

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
        # Values stored in the supervisor
        self.state = States.NEUTRAL
        self.throttle = 0
        self.desiredAngle = 0
               
        # Values stored on the arduino
        self.measuredWheelRPM = 0
        self.measuredDirecton = Directions.FORWARD
        self.measuredAngle = 0
        self.actuator = 0
        
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
        
        if 0 <= angle <= 1023:
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
        state = state + "{:01d}".format(self.state)
        
        state = state + Commands.SET_THROTTLE
        state = state + "{:03d}".format(self.throttle)
        
        state = state + Commands.SET_ANGLE
        state = state + "{:05d}".format(self.desiredAngle)
        
        return state
