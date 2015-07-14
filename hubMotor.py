# HubMotor

# This class holds all the state methods and fields for one particular hub motor controller.

import RPi.GPIO as GPIO
import datetime 
 
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

    def __init__(self, pin_hall, pin_rev, pin_throttle, pin_brake):
        # Instance variables unique to each instance

        self.state = states.neutral
        self.measuredSpeed = 0
        self.lastHallRisingEdge = 
        #self.measuredDirecton = directions.forward
        #self.measuredCurrent = 0

        self.pin_hall = pin_hall
        self.pin_rev = pin_rev
        self.pin_throttle = pin_throttle
        self.pin_brake = pin_brake

        GPIO.setup(pin_hall, GPIO.input)
        GPIO.setup(pin_rev, GPIO.output)
        GPIO.setup(pin_throttle, GPIO.output)
        GPIO.setup(pin_brake, GPIO.output)

        # PWM frequency 100Hz, 0% duty cycle
        self.throttle = GPIO.PWM(pin_throttle, 100)
        self.throttle.start(0)

        self.setState(states.neutral)


    def setState(self, state):
        self.state = state

        if state == states.neutral:
            self.setThrottle(0)
            GPIO.output(pin_rev, 1)
            GPIO.output(pin_brake, 1)

        elif state == states.braking:
            self.setThrottle(0)
            GPIO.output(pin_rev, 1)
            GPIO.output(pin_brake, 0)

        elif state == states.forward:
            GPIO.output(pin_rev, 1)
            GPIO.output(pin_brake, 1)

        elif state == states.reverse:
            GPIO.output(pin_rev, 0)
            GPIO.output(pin_brake, 1)
        

    def setThrottle(self, throttle):
        self.throttle.ChangeDutyCycle(throttle)

    def g
        
    

    
