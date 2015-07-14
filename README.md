Supervisor
----------

This supervisor always runs in the background. It holds the desired state of the robot in memory and
issues GPIO and serial bus commands in order to achieve this state.

The supervisor can be contacted using a network socket in order to set a new desired state for the robot.

```
Socket commands:

setDirection(channel, direction)
getDirection(channel)
    channel = frontleft | frontright | rearleft | rearright
    direction = neutral | breaking | forward | reverse

setThrottle(channel, throttle)
getThrottle(channel)
    throttle = 0-100

getMeasuredDirection(channel)

getMeasuredSpeed(channel)

getMeasuredCurrent(channel)
```