
# Import required packages
from picamera.array import PiRGBArray
from picamera import PiCamera
import time
import cv2

def main():
	
	print("Starting Program")
	
	# Init camera
	camera = PiCamera()
	rawCapture = PiRGBArray(camera)
	
	# allow camera to warmup
	time.sleep(0.1)
	
	# grab image from camera
	camera.capture(rawCapture, format="bgr")
	image = rawCapture.array
	
	# Display image on screen and wait for keypress
	cv2.imshow("Image", image)
	cv2.waitKey(0)
	
	print("Completed!")
	
	return 0

if __name__ == '__main__':
	main()

"""
export WORKON_HOME=$HOME/.virtualenvs
source /usr/local/bin/virtualenvwrapper.sh


"""
