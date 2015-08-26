# Import required packages
import picamera
import time
import os


class Camera:
    def __init__(self, state):
        self.state = state
        self.running = True
               
        self.camera = None
    
        # This is a RAM-disk provided by Pi out of the box. Writing here
        # avoids thrashing the SD card, which only has a limited number
        # of read/write cycles and is slow as heck.
        self.filepathLoading = "/run/shm/loadingCamera1.jpg"
        self.filepathComplete = "/run/shm/camera1.jpg"
        
           
    def run(self):
        # Wait until camera initialises sucessfully
        while not self.camera:
            try:
                self.camera = picamera.PiCamera()
                
            except picamera.exc.PiCameraMMALError:
                time.sleep(10)
                
        self.camera.resolution = (640, 480)
        self.camera.framerate = 4
        
        # allow camera to warmup
        time.sleep(0.1)
        
        
        while self.running:
            # This method blocks until a new frame becomes available,
            # so setting the framerate determines the rate at which 
            # images are written.
            self.camera.capture(self.filepathLoading,
                                format="jpeg",
                                use_video_port=True,
                                quality=25,
                                thumbnail=None)
                                
            os.rename(self.filepathLoading, self.filepathComplete)
