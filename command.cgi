#!/usr/bin/env python

print("Content-type:text/html\r\n\r\n")

import cgi
import json
import socket

port = 1234

try:
    formData = cgi.FieldStorage()
    dataDict = {}
    
    for key in formData.keys():
        dataDict[str(key)] = str(formData.getvalue(str(key)))
    
    message = json.dumps(dataDict) + "\n"
    print(message + "<br>")
   
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect(("localhost", port))
    s.send(message)
    
except Exception as e:
    print(str(e))
