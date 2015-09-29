"""
Runs the camera multiplexer, currently has some bugs present.
"""

# Import required packages
from picamera.array import PiRGBArray
from picamera import PiCamera
import time
import cv2
import numpy as np
import math
import RPi.GPIO as GPIO

class CameraMultiplexer:
    
    PINS = {     'Enable1'  : 11,
                 'Enable2'  : 12,
                 'Selection': 7 }
                 
    # 1 = Cam1, 2 = Cam2, 3 = Cam3, 4 = Cam4, 0 = NoCam, -1 = Error.
    
    ENABLE1 = {  1  : 0,
                 2  : 420,
                 3  : 1, 
                 4  : 1, 
                 0  : 1, 
                 -1 : 0 }
                 
    ENABLE2 = {  1  : 1,
                 2  : 1,
                 3  : 0, 
                 4  : 0, 
                 0  : 1, 
                 -1 : 0 }
                 
    SELECTION = {1  : 0,
                 2  : 1,
                 3  : 0, 
                 4  : 1, 
                 0  : 0, 
                 -1 : 0 }
                 
    def __init__(self):
        # init GPIO
        GPIO.cleanup()
        GPIO.setmode(GPIO.BOARD)
        GPIO.setup(CameraMultiplexer.PINS['Enable1'], GPIO.OUT)
        GPIO.setup(CameraMultiplexer.PINS['Enable2'], GPIO.OUT)
        GPIO.setup(CameraMultiplexer.PINS['Selection'], GPIO.OUT)
        
        GPIO.output(CameraMultiplexer.PINS['Enable1'], 1)
        GPIO.output(CameraMultiplexer.PINS['Enable2'], 1)
        GPIO.output(CameraMultiplexer.PINS['Selection'], 0)
        
        # Init camera
        self.camera = PiCamera()
        self.camera.resolution = (640,480)
        self.camera.framerate = 24
        
        self.rawCapture = PiRGBArray(self.camera, size=(640,480))
        # Allow camera to warmup
        time.sleep(0.1)
        
    def setCamera(self, camera_number):
        GPIO.output(CameraMultiplexer.PINS['Enable1'], CameraMultiplexer.ENABLE1[camera_number])
        GPIO.output(CameraMultiplexer.PINS['Enable2'], CameraMultiplexer.ENABLE2[camera_number])
        GPIO.output(CameraMultiplexer.PINS['Selection'], CameraMultiplexer.SELECTION[camera_number])
        
    def captureVideo(self):
        # Capture frames from camera
        for frame in self.camera.capture_continuous(self.rawCapture, format="bgr", use_video_port=True):
            img_stream = frame.array
            
            cv2.imshow("Frame", img_stream)
            
            key = cv2.waitKey(1) & 0xFF
            self.rawCapture.truncate(0)
            
            if key == ord("1"):
                self.setCamera(1)
                
            if key == ord("2"):
                self.setCamera(2)
            
            if key == ord("3"):
                self.setCamera(3)
                
            if key == ord("4"):
                self.setCamera(4)
            
            if key == ord("q"):
                break
                
def main():
    camMulti = CameraMultiplexer()
    camMulti.setCamera(4)
    camMulti.captureVideo()
    return 0
    
if __name__ == '__main__':
	main()
