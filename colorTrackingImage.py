# Import required packages
from picamera.array import PiRGBArray
from picamera import PiCamera
import time
import cv2
import numpy as np
import math

class ColorTracker:
	def __init__(self):
		# Init camera
		self.camera = PiCamera()
		self.rawCapture = PiRGBArray(self.camera)
		# allow camera to warmup
		time.sleep(0.1)
		
		# Images
		self.raw_img = None
		self.tracked_img = None
		
		# Scale image down so faster to process
		self.scale_down = 4
		# How much to erode and dilate by
		self.morphology = 20
		# Blur
		self.gaussian_blur = 5
		
		# Color Range (input the range of colors to detect in HSV)
		self.color_lower = np.array([130, 100, 0], np.uint8)
		self.color_upper = np.array([200, 255, 255], np.uint8)
		
		
	def captureImage(self):
		# grab image from camera
		self.camera.capture(self.rawCapture, format="bgr")
		self.raw_img = self.rawCapture.array
		
	def trackColor(self):
		# Init return values
		x_centre = 0
		y_centre = 0
		box_abs_size = 0
		
		img = self.raw_img
		# Gaussian Blur on image
		img = cv2.GaussianBlur(img, (self.gaussian_blur, self.gaussian_blur), 0)
		# Convert to HSV colorspace
		img = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
		# Scale down the image for easier processing
		img = cv2.resize(img, (len(img[0]) / self.scale_down, len(img) / self.scale_down))
		
		# Create binary image of colors within range
		color_binary = cv2.inRange(img, self.color_lower, self.color_upper)
		
		# Erode and Dilate
		dilation = np.ones((self.morphology,self.morphology), "uint8")
		erosion = np.ones((self.morphology,self.morphology), "uint8")
		color_binary = cv2.erode(color_binary, erosion)
		color_binary = cv2.dilate(color_binary, dilation)
		
		# Find Contours
		contours, hierarchy = cv2.findContours(color_binary, cv2.RETR_LIST, cv2.CHAIN_APPROX_SIMPLE)
		
		# Determine largest contour
		max_area = 0
		largest_contour = None
		for idx, contour in enumerate(contours):
			area = cv2.contourArea(contour)
			if area > max_area:
				max_area = area
				largest_contour = contour
		
		if not largest_contour == None:
			moment = cv2.moments(largest_contour)
			if moment["m00"] > 1000 / self.scale_down:
				# Find minimum-area bounding rectangle
				rect = cv2.minAreaRect(largest_contour)
				rect = ((rect[0][0] * self.scale_down, rect[0][1] * self.scale_down), (rect[1][0] * self.scale_down, rect[1][1] * self.scale_down), 0)
				box = cv2.cv.BoxPoints(rect)
				box = np.int0(box)
				# Find box height, width, abs size and centre point
				box_width = int(rect[1][0])
				box_height = int(rect[1][1])
				box_abs_size = math.sqrt(box_width**2 + box_height**2)
				x_centre = int(rect[0][0])
				y_centre = int(rect[0][1])
				# Overlay bounding rectangle and centre point onto raw image
				self.tracked_img = self.raw_img
				cv2.drawContours(self.tracked_img , [box] , 0, (255, 0, 0), 2)
				cv2.circle(self.tracked_img , (x_centre,y_centre), 5, (255, 0, 0))
		return x_centre, y_centre, box_abs_size
	
	def displayTrackedImage(self):
		# Display image on screen and wait for keypress
		cv2.imshow("ColorTracking", self.tracked_img)
		cv2.waitKey(0)


def main():
	
	trackRed = ColorTracker()
	trackRed.captureImage()
	trackRed.trackColor()
	trackRed.displayTrackedImage()
		
	return 0

if __name__ == '__main__':
	main()
