import wheel
import threading
import visionTrackingResult

class Modes:
    MANUAL = 0
    AUTO = 1

class State:
    def __init__():
        lock = threading.Lock()
        
        mode = Modes.MANUAL
        
        frontLeftWheel = wheel.Wheel()
        frontRightWheel = wheel.Wheel()
        backLeftWheel = wheel.Wheel()
        backRightWheel = wheel.Wheel()
        
        frontLeftVisionResult = visionTrackingResult.VisionTrackingResult()
        frontRightVisionResult = visionTrackingResult.VisionTrackingResult()
        backLeftVisionResult = visionTrackingResult.VisionTrackingResult()
        backRightVisionResult = visionTrackingResult.VisionTrackingResult()
