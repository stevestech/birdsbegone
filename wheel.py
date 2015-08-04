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
