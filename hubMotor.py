# HubMotor

# This class holds all the state methods and fields for one particular hub motor controller.

import RPi.GPIO as GPIO
import time
import threading

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
        self.hallPulsesPerRevolution = 25
        self.secondsPerMinute = 60
        
        #self.measuredDirecton = directions.forward
        #self.measuredCurrent = 0

        self.pinHall = pinHall
        self.pinReverse = pinReverse
        self.pinThrottle = pinThrottle
        self.pinBrake = pinBrake
        
        GPIO.setup(self.pinHall, GPIO.IN)
        GPIO.setup(self.pinReverse, GPIO.OUT)
        GPIO.setup(self.pinThrottle, GPIO.OUT)
        GPIO.setup(self.pinBrake, GPIO.OUT)

        # PWM frequency 100Hz, 0% duty cycle
        self.throttle = GPIO.PWM(self.pinThrottle, 100)
        self.throttle.start(0)

        GPIO.add_event_detect(self.pinHall,
                              GPIO.RISING,
                              callback=self.hallSensorRisingEdgeCallback)

        self.setState(States.NEUTRAL)


    def setState(self, state):
        self.state = state

        if state == States.NEUTRAL:
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


    def hallSensorRisingEdgeCallback(self, channel):
        currentTime = time.time()
        
        try:
            self.secondsPerHallPulse = currentTime - self.timeOfLastHallRisingEdge
            
        except AttributeError:
            # self.timeOfLastHallRisingEdge is not defined the first time that this
            # method is called.
            pass
            
        self.timeOfLastHallRisingEdge = currentTime
        

    def getMeasuredSpeed(self):
        # Returns wheel speed in revolutions per minute
        try:
            if time.time() - self.timeOfLastHallRisingEdge > 2:
                # If it has been at least two seconds since the last hall pulse, use the current
                # time as the time of the last hall pulse instead of the last recorded pulse.
                # This causes speed to drop toward zero over time.
                self.secondsPerHallPulse = time.time() - self.timeOfLastHallRisingEdge
                
            return self.secondsPerMinute / (self.hallPulsesPerRevolution * self.secondsPerHallPulse)
            
        except (ZeroDivisionError, AttributeError) as e:
            return 0
            
        
        
 
