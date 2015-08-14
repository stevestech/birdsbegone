#!/usr/bin/env python

import cgi
import json
import socket
import sys

# Which port the supervisor is listening on
port = 1234

# How much data to recieve in one go
bufferSize = 4096

# Data passed back to JavaScript on the webpage
toWebBrowser = {}

# Assemble GET data from the webserver into a Python dict
formData = cgi.FieldStorage()
dataDict = {}

for key in formData.keys():
	dataDict[str(key)] = str(formData.getvalue(str(key)))

# Convert the dict into a JSON string
message = json.dumps(dataDict)

while True:
	# Send the JSON string to the supervisor using a network socket
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

	try:
		s.connect(("localhost", port))
		
	except socket.error:
		print("Content-type: text/html\r\n\r\n")
		print(json.dumps({"error": "The robot supervisor program is not running."}))
		s.close()
		sys.exit(0)
		
	s.settimeout(2)
	s.sendall(message)

	# Receive state from the supervisor
	try:
		message = s.recv(bufferSize)
		
	except socket.timeout:
		continue
		
	if message.startswith("{") and message.endswith("}"):
		break
	
s.close()
print("Content-type: text/html\r\n\r\n")
print(message)
