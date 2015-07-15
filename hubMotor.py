# HubMotor

# This class holds all the state methods and fields for one particular hub motor controller.

import RPi.GPIO as GPIO
import time

class States:
    NEUTRAL = 0
    BRAKING = 1
    FORWARD = 2
    REVERSE = 3
    
class Channels:
    FRONT_LEFT = 0
    FRONT_RIGHT = 1
    REAR_LEFT = 2
    REAR_RIGHT = 3
 
class HubMotor:
    def __init__(self, pinHall, pinReverse, pinThrottle, pinBrake):
        # Used to find a moving mean of the wheel's velocity
        self.hallRisingEdgeTimeDeltas = []
        
        #self.measuredDirecton = directions.forward
        #self.measuredCurrent = 0

        self.pinHall = pinHall
        self.pinReverse = pinReverse
        self.pinThrottle = pinThrottle
        self.pinBrake = pinBrake

        GPIO.setup(self.pinHall, GPIO.input)
        GPIO.setup(self.pinReverse, GPIO.output)
        GPIO.setup(self.pinThrottle, GPIO.output)
        GPIO.setup(self.pinBrake, GPIO.output)

        # PWM frequency 100Hz, 0% duty cycle
        self.throttle = GPIO.PWM(self.pinThrottle, 100)
        self.throttle.start(0)

        GPIO.add_event_detect(self.pinHall,
                              GPIO.RISING,
                              callback=self.hallSensorRisingEdgeCallback)

        self.setState(States.NEUTRAL)


    def setState(self, state):
        self.state = state

        if state == States.NEUTRAL
            self.setThrottle(0)
            GPIO.output(self.pinReverse, 1)
            GPIO.output(self.pinBrake, 1)

        elif state == States.BRAKING:
            self.setThrottle(0)
            GPIO.output(self.pinReverse, 1)
            GPIO.output(self.pinBrake, 0)

        elif state == States.FORWARD:
            GPIO.output(self.pinReverse, 1)
            GPIO.output(self.pinBrake, 1)

        elif state == States.REVERSE:
            GPIO.output(self.pinReverse, 0)
            GPIO.output(self.pinBrake, 1)
        

    def setThrottle(self, throttle):
        self.throttle.ChangeDutyCycle(throttle)

    def hallSensorRisingEdgeCallback(self):
        try:
            # Prepend value to list
            self.hallRisingEdgeTimeDeltas.insert(0, time.time() - self.timeOfLastHallRisingEdge)
            
        except NameError:
            # self.timeOfLastHallRisingEdge is not defined the first time this method is called
            pass
           
        self.timeOfLastHallRisingEdge = time.time()
        
        # Only keep 3 entries in the hallRisingEdgeTimeDeltas list, 0 through to 2
        try:
            self.hallRisingEdgeTimeDeltas.pop(3)
            
        except IndexError:
            pass
        

    def getMeasuredSpeed(self):
        # Average the last 10 time deltas to get the measured speed
        timeDeltaSum = 0
        
        for timeDelta in self.hallRisingEdgeTimeDeltas:
            timeDeltaSum = timeDeltaSum + timeDelta

        return timeDeltaSum / len(self.hallRisingEdgeTimeDeltas)
        
 
