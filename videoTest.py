
# Import required packages
from picamera.array import PiRGBArray
from picamera import PiCamera
import time
import cv2

def main():
	
	print("Starting Program")
	
	# Init camera
	camera = PiCamera()
	camera.resolution = (640,480)
	camera.framerate = 24
	rawCapture = PiRGBArray(camera, size=(640,480))
	
	# allow camera to warmup
	time.sleep(0.1)
	
	# capture frames from camera
	for frame in camera.capture_continuous(rawCapture, format="bgr", use_video_port=True):
		image = frame.array
		
		cv2.imshow("Frame", image)
		key = cv2.waitKey(1) & 0xFF
		rawCapture.truncate(0)
		
		if key == ord("q"):
				break
	
	print("Completed!")
	
	return 0

if __name__ == '__main__':
	main()

"""
export WORKON_HOME=$HOME/.virtualenvs
source /usr/local/bin/virtualenvwrapper.sh


"""
