"""
http://suksant.com/2013/04/26/fast-colour-based-object-tracking-with-opencv/
"""

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
		self.camera.resolution = (640,480)
		self.camera.framerate = 24
		
		self.rawCapture = PiRGBArray(self.camera, size=(640,480))
		# Allow camera to warmup
		time.sleep(0.1)
		
		# Images
		self.raw_img = None
		self.tracked_img = None
		
		# Scale image down so faster to process
		self.scale_down = 2
		# How much to erode and dilate by
		self.morphology = 1
		# Blur
		self.gaussian_blur = 3
		
		# Color Range Init (input the default range of colors to detect in HSV)
		self.Hmin = 165
		self.Hmax = 179
		self.Smin = 80
		self.Smax = 255
		self.Vmin = 0
		self.Vmax = 255
		self.minArea = 3
		self.color_lower = np.array([self.Hmin, self.Smin, self.Vmin ], np.uint8)
		self.color_upper = np.array([self.Hmax , self.Smax, self.Vmax], np.uint8)
		
		
		# Controls for adjusting ranges
		cv2.namedWindow("Adjustment")
		cv2.createTrackbar("Hmin", "Adjustment", self.Hmin, 179, self.nothing)
		cv2.createTrackbar("Hmax", "Adjustment", self.Hmax, 179, self.nothing)
		cv2.createTrackbar("Smin", "Adjustment", self.Smin, 255, self.nothing)
		cv2.createTrackbar("Smax", "Adjustment", self.Smax, 255, self.nothing)
		cv2.createTrackbar("Vmin", "Adjustment", self.Vmin, 255, self.nothing)
		cv2.createTrackbar("Vmax", "Adjustment", self.Vmax, 255, self.nothing)
		cv2.createTrackbar("minArea", "Adjustment", self.minArea, 1000, self.nothing)
	
	def nothing(self, *args):
		pass
	
	def updateRanges(self):
		self.Hmin = cv2.getTrackbarPos("Hmin", "Adjustment")
		self.Hmax = cv2.getTrackbarPos("Hmax", "Adjustment")
		self.Smin = cv2.getTrackbarPos("Smin", "Adjustment")
		self.Smax = cv2.getTrackbarPos("Smax", "Adjustment")
		self.Vmin = cv2.getTrackbarPos("Vmin", "Adjustment")
		self.Vmax = cv2.getTrackbarPos("Vmax", "Adjustment")
		self.minArea = cv2.getTrackbarPos("minArea", "Adjustment")
		self.color_lower = np.array([self.Hmin, self.Smin, self.Vmin ], np.uint8)
		self.color_upper = np.array([self.Hmax , self.Smax, self.Vmax], np.uint8)
	
	def trackColor(self):
		# capture frames from camera
		for frame in self.camera.capture_continuous(self.rawCapture, format="bgr", use_video_port=True):
			img_stream = frame.array
			
			
			# Init return values
			x_centre = 0
			y_centre = 0
			box_abs_size = 0
			
			img = img_stream
			
			# Update HSV ranges
			self.updateRanges()
			
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
			cv2.imshow("BinaryDiff", color_binary)
			
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
				if moment["m00"] > self.minArea / self.scale_down:
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
					cv2.drawContours(img_stream , [box] , 0, (255, 0, 0), 2)
					cv2.circle(img_stream , (x_centre,y_centre), 5, (255, 0, 0))
			
			cv2.imshow("Frame", img_stream)
			
			key = cv2.waitKey(1) & 0xFF
			self.rawCapture.truncate(0)
			if key == ord("q"):
				break

def main():
	
	trackRed = ColorTracker()
	trackRed.trackColor()
		
	return 0

if __name__ == '__main__':
	main()
