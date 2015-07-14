# HubMotor

# This class holds all the state methods and fields for one particular hub motor controller.

import RPi.GPIO as GPIO
from datetime import datetime
 
class HubMotor:
    # Class variables shared by all instances

    states = {'neutral': 0,
              'braking': 1,
              'forward': 2,
              'reverse': 3}

    directions = {'forward': 0,
                  'reverse': 1}

    channels = {'front_left': 0,
                'front_right': 1,
                'rear_left': 2,
                'rear_right': 3}

    def __init__(self, pinHall, pinRev, pinThrottle, pinBrake):
        # Instance variables unique to each instance

        self.state = states.neutral
        
        self.timeOfLastHallRisingEdge = datetime.now()
        self.hallRisingEdgeTimeDeltas = []
        
        #self.measuredDirecton = directions.forward
        #self.measuredCurrent = 0

        self.pinHall = pinHall
        self.pinRev = pinRev
        self.pinThrottle = pinThrottle
        self.pinBrake = pinBrake

        GPIO.setup(self.pinHall, GPIO.input)
        GPIO.setup(self.pinRev, GPIO.output)
        GPIO.setup(self.pinThrottle, GPIO.output)
        GPIO.setup(self.pinBrake, GPIO.output)

        # PWM frequency 100Hz, 0% duty cycle
        self.throttle = GPIO.PWM(self.pinThrottle, 100)
        self.throttle.start(0)

        GPIO.add_event_detect(self.pinHall,
                              GPIO.RISING,
                              callback=self.hallSensorRisingEdgeCallback)

        self.setState(states.neutral)


    def setState(self, state):
        self.state = state

        if state == states.neutral:
            self.setThrottle(0)
            GPIO.output(self.pinRev, 1)
            GPIO.output(self.pinBrake, 1)

        elif state == states.braking:
            self.setThrottle(0)
            GPIO.output(self.pin_rev, 1)
            GPIO.output(self.pin_brake, 0)

        elif state == states.forward:
            GPIO.output(self.pin_rev, 1)
            GPIO.output(self.pin_brake, 1)

        elif state == states.reverse:
            GPIO.output(self.pin_rev, 0)
            GPIO.output(self.pin_brake, 1)
        

    def setThrottle(self, throttle):
        self.throttle.ChangeDutyCycle(throttle)

    def hallSensorRisingEdgeCallback(self):
        hallRisingEdgeTimeDeltas.append(datetime.now() - self.timeOfLastHallRisingEdge)
        self.timeOfLastHallRisingEdge = datetime.now()

        # Only keep 10 entries in the hallRisingEdgeTimeDeltas list, 0 through to 9
        try:
            self.hallRisingEdgeTimeDeltas.pop(10)

        except IndexError:
            pass
        

    def getMeasuredSpeed(self):
        # Average the last 10 time deltas to get the measured speed
        timeDeltaSum = 0
        
        for timeDelta in self.hallRisingEdgeTimeDeltas:
            timeDeltaSum = timeDeltaSum + timeDelta

        return timeDeltaSum / len(self.hallRisingEdgeTimeDeltas)
        
        
        
    

    
