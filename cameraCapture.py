# Import required packages
from picamera.array import PiRGBArray
from picamera import PiCamera
import time
import cv2
import os


class Camera:
    def __init__(self):
        self.camera = PiCamera()
        self.camera.resolution = (640,480)
        self.camera.framerate = 24
        self.rawCapture = PiRGBArray(self.camera, size=(640,480))
        
        
        self.filepathLoading = "/var/www/cameras/loadingCamera1.jpg"
        self.filepathComplete = "/var/www/cameras/camera1.jpg"
        
        # allow camera to warmup
        time.sleep(0.1)
    
    def run(self):
        for frame in self.camera.capture_continuous(self.rawCapture, format="bgr", use_video_port=True):
            image = frame.array
            
            cv2.imshow("Frame", image)
            key = cv2.waitKey(1) & 0xFF
            
            
            self.saveImage(self.filepathLoading, self.filepathComplete, image)
            
            self.rawCapture.truncate(0)
            
            
    def saveImage(self, loadingFilePath, completeFilePath, image):
        cv2.imwrite(loadingFilePath, image)
        os.rename(loadingFilePath, completeFilePath)
        

cam = Camera()
cam.run()
