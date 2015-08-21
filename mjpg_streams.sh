export LD_LIBRARY_PATH=/usr/local/lib

mjpg_streamer -i "input_file.so -f /run/shm -n camera1.jpg" -o "output_http.so -p 8080"
#mjpg_streamer -i "input_file.so -f /run/shm -n camera2.jpg" -o "output_http.so -p 8081"
#mjpg_streamer -i "input_file.so -f /run/shm -n camera3.jpg" -o "output_http.so -p 8082"
#mjpg_streamer -i "input_file.so -f /run/shm -n camera4.jpg" -o "output_http.so -p 8083"
