# Import required packages
import picamera
import time
import os
import subprocess

"""
In order for the cameras to be visible from the web interface, mjpg-streamer is required
to be installed on the system and added to the system PATH.

Installation guide that I used:

1. Install build dependencies
$sudo apt-get install libjpeg8-dev imagemagick libv4l-dev

2. Add missing videodev.h (mjpg-streamer needs videodev2.h, library provides videodev.h, they are the same file, symbolic link to the rescue)
$sudo ln -s /usr/include/linux/videodev2.h /usr/include/linux/videodev.h

3. Download MJPG-Streamer
$ wget http://sourceforge.net/code-snapshots/svn/m/mj/mjpg-streamer/code/mjpg-streamer-code-182.zip

4. Unzip and build the MJPG-Streamer source code
$ unzip mjpg-streamer-code-182.zip
$ cd mjpg-streamer-code-182/mjpg-streamer
$ make mjpg_streamer input_file.so output_http.so

5.  Install MJPG-Streamer
$ sudo cp mjpg_streamer /usr/local/bin
$ sudo cp output_http.so input_file.so /usr/local/lib/
<OPTIONAL> $ sudo cp -R www /usr/local/www

<OPTIONAL> 6.  Start the camera
$ mkdir /tmp/stream
$ raspistill --nopreview -w 640 -h 480 -q 5 -o /tmp/stream/pic.jpg -tl 100 -t 9999999 -th 0:0:0 &

<OPTIONAL> 7. Start MJPG-Streamer
$ LD_LIBRARY_PATH=/usr/local/lib mjpg_streamer -i "input_file.so -f /tmp/stream -n pic.jpg" -o "output_http.so -w /usr/local/www"

<OPTIONAL> 8. Watch the Stream!
Pop a web browser, and enter http://localhost:8080

Enjoy it.
"""



class Camera:
    sides = { 'LEFT': 0,
              'RIGHT': 1,
              'FRONT': 2,
              'BACK': 3 }

    def __init__(self, state):
        self.state = state
               
        self.camera = None
    
        # This is a RAM-disk provided by Pi out of the box. Writing here
        # avoids thrashing the SD card, which only has a limited number
        # of read/write cycles and is slow as heck.
        self.filepathLoading = "/run/shm/loadingCamera1.jpg"
        self.filepathComplete = "/run/shm/camera1.jpg"
        
           
    def run(self):
        # Wait until camera initialises sucessfully
        for attempt in range(15):
            if not self.state.running:
                return
        
            try:
                self.camera = picamera.PiCamera()
                break
                
            except Exception:
                time.sleep(1)
        
        # Else statement executes if we do not reach the break statement.
        else:
            return
                
                
        self.camera.resolution = (640, 480)
        self.camera.framerate = 4
        
        # allow camera to warmup
        time.sleep(0.1)
        
        # Environment variables inherited from the currently running application.
        # LD_LIBRARY_PATH shows mjpeg streamer where to find its input and output modules.
        environ = os.environ.copy()
        environ['LD_LIBRARY_PATH'] = '/usr/local/lib'
        
        mjpegStreamServer1 = subprocess.Popen(["mjpg_streamer",
                                               "-i", "input_file.so", "-f", "/run/shm", "-n", "camera1.jpg",
                                               "-o", "output_http.so", "-p", "8080"],
                                              env=environ)
        
        
        while self.state.running:
            # This method blocks until a new frame becomes available,
            # so setting the framerate determines the rate at which 
            # images are written.
            self.camera.capture(self.filepathLoading,
                                format="jpeg",
                                use_video_port=True,
                                quality=25,
                                thumbnail=None)
                                
            os.rename(self.filepathLoading, self.filepathComplete)
            
        
        # Send a SIG_TERM to the MJPEG streaming servers, and wait for them to close
        mjpegStreamServer1.terminate()
        mjpegStreamServer1.wait()
        