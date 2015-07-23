Useful RPi Commands
----------

Set date manually if no internet:
```
sudo date -s "Wed Jul 22 14:05:00 NZST 2015"
```

Work in cv virtual enviroment:

```
source ~/.profile
workon cv
```

Author commits using:

```
git commit --author="username" -m "commit msg"
```
Repo is now setup to allow direct pushing and pulling, no origin master required =]

Using the WiFI Dongle on University Internet:
```
Plug in WiFi Dongle
Connect to UCVisitor
In web browser navigate to bookmarks -> UC Visitor Enabler
Alternativly visit: https://ucwlc1auth.canterbury.ac.nz
Log in and enjoy the sweet sweet internet!
```



Supervisor
----------

This supervisor always runs in the background. It holds the desired state of the robot in memory and
issues GPIO and serial bus commands in order to achieve this state.

The supervisor can be contacted using a network socket in order to set a new desired state for the robot.

```
Socket commands:

setDirection(channel, direction)
getDirection(channel)
    channel = front_left | front_right | rear_left | rear_right
    direction = neutral | braking | forward | reverse

setThrottle(channel, throttle)
getThrottle(channel)
    throttle = 0-100

getMeasuredDirection(channel)

getMeasuredSpeed(channel)

getMeasuredCurrent(channel)
```
