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
        # TEMPORARY
        GPIO.cleanup()
        GPIO.setmode(GPIO.BCM)
        
        # Used to determine wheel RPM
        self.hallPulseCount = 0
        self.secondsPerRPMCalculation = 0.5
        self.hallPulsesPerRevolution = 25
        self.secondsPerMinute = 60        
        self.wheelRPM = 0
        
        #self.measuredDirecton = directions.forward
        #self.measuredCurrent = 0

        self.pinHall = pinHall
        self.pinReverse = pinReverse
        self.pinThrottle = pinThrottle
        self.pinBrake = pinBrake
        
        GPIO.setup(self.pinHall, GPIO.IN, pull_up_down=GPIO.PUD_UP)
        GPIO.setup(self.pinReverse, GPIO.OUT)
        GPIO.setup(self.pinThrottle, GPIO.OUT)
        GPIO.setup(self.pinBrake, GPIO.OUT)

        # PWM frequency 100Hz, 0% duty cycle
        self.throttle = GPIO.PWM(self.pinThrottle, 100)
        self.throttle.start(0)

        GPIO.add_event_detect(self.pinHall,
                              GPIO.RISING,
                              callback=self.incrementHallPulseCount)
                              
        # Run the self.determineWheelSpeed method repeatedly.
        self.wheelRPMUpdateThread = threading.Timer(self.secondsPerRPMCalculation, self.determineWheelSpeed)
        self.wheelRPMUpdateThread.daemon = True
        self.wheelRPMUpdateThread.start()

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
        if (self.state != States.NEUTRAL) and (self.state != States.BRAKING):
            if throttle > 100:
                throttle = 100
                
            elif throttle < 0:
                throttle = 0
                
            self.throttle.ChangeDutyCycle(throttle)
            
        else:
            self.throttle.ChangeDutyCycle(0)
        
        
    def incrementHallPulseCount(self, channel):
        self.hallPulseCount = self.hallPulseCount + 1


    def determineWheelSpeed(self):
        # Sets the value of self.wheelRPM
        hallPulsesPerSecond = self.hallPulseCount / self.secondsPerRPMCalculation
        revolutionsPerSecond = hallPulsesPerSecond / self.hallPulsesPerRevolution     
        self.wheelRPM = revolutionsPerSecond * self.secondsPerMinute
        
        self.hallPulseCount = 0
        
        if self.running:
            threading.Timer(self.secondsPerRPMCalculation, self.determineWheelSpeed).start()
        
    
    def getWheelRPM(self):
        return self.wheelRPM
