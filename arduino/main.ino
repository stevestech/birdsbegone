// C++ libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>

// Arduino libraries
#include <SPI.h>

// Modules
#include "actuator.h"

//#define DEBUG_SPI

#define LENGTH_SPI_BUFFER               13

#define CHARS_THROTTLE                  3      // Within incoming messages, this many characters are allocated to the throttle parameter
#define CHARS_STATE                     1
#define CHARS_COMMAND                   1
#define CHARS_ANGLE                     5

#define CHAR_ETX                        3      // ASCII code for ETX (end of text) character

#define CMD_RECEIVE                     16
#define CMD_GET_ANGLE                   17
#define CMD_GET_ACTUATOR                18

#define ACTUATOR_PWM_MAX                (127 - ACTUATOR_SLEW_OFFSET)       // Limit the max duty cycle that can be applied to the actuator driver. 255 corresponds to 100% duty cycle.
#define ACTUATOR_SLEW_OFFSET            60     // This is used to reduce the deadzone caused by slew rate from high frequency PWM. High value, less dead zone. Max 72.

#define ANALOG_IN_MAX                   1023

#define BASE_10                         10



// Actuator PID gains
#define GAIN_PROPORTIONAL               1
#define GAIN_INTEGRAL                   0
#define GAIN_DIFFERENTIAL               0

#define STATE_NEUTRAL                   '0'
#define STATE_BRAKING                   '1'
#define STATE_FORWARD                   '2'
#define STATE_REVERSE                   '3'

#define CMD_SET_STATE                   'A'
#define CMD_SET_THROTTLE                'B'
#define CMD_SET_ANGLE                   'C'





char hubMotorState;

char char_throttle [CHARS_THROTTLE + 1];
// Long because strtol returns a long
long hubMotorThrottle;

char char_angle [CHARS_ANGLE + 1];

PID actuatorController(
  &measuredActuatorAngle,
  &actuatorControllerOutput,
  &desiredActuatorAngle,
  GAIN_PROPORTIONAL,
  GAIN_INTEGRAL,
  GAIN_DIFFERENTIAL,
  DIRECT);  
  
char send_buffer[LENGTH_SPI_BUFFER];
volatile byte send_index;

boolean recv_buffer_full;
char recv_buffer[LENGTH_SPI_BUFFER];
volatile byte recv_index;

char loop_buffer[LENGTH_SPI_BUFFER];
volatile byte loop_index;

volatile boolean ignore_message;
boolean init_complete;
byte command;

// start of transaction, no command yet
void ss_falling ()
{
  command = 0;
}  // end of interrupt service routine (ISR) ss_falling


/* 
 * Outputs a char array in the format "0007", for a value of 7
 * or "-015" for a value of -15, if CHARS_ANGLE = 4.
 * The last character in toSend will be set to ETX, not NULL.
 * The size of toSend will be CHARS_ANGLE + 1 due to ETX
 **/
void intToCharArray(char *toSend, int value)
{  
  char format[16];
  int lowerLimit;
  int upperLimit;
  
  // Example: "%04d" if CHARS_ANGLE is 4
  sprintf(format, "%%0%dd", CHARS_ANGLE);
  
  // Does value fit into CHARS_ANGLE chars?
  lowerLimit = pow(10, CHARS_ANGLE - 1);
  lowerLimit *= -1;
  
  upperLimit = pow(10, CHARS_ANGLE);
  
  if (!((lowerLimit < value) && (value < upperLimit)))
  {
     value = 0;
  }
  
  sprintf(toSend, format, value);
  toSend[CHARS_ANGLE] = CHAR_ETX;
}


void processSPIMessage(void)
{
  memcpy(loop_buffer, recv_buffer, LENGTH_SPI_BUFFER);
  recv_buffer_full = false;
  loop_index = 0;
  
  #ifdef DEBUG_SPI
    boolean etx_present = false;
    byte x;
    for (x=0; x < sizeof(loop_buffer); x++)
    {
      if (loop_buffer[x] == CHAR_ETX)
      {
        loop_buffer[x] = 0;
        etx_present = true;
      }
    }
    
    Serial.println(loop_buffer);
    if (etx_present) loop_buffer[x] = CHAR_ETX;
  #endif
  
  while ((loop_index < sizeof(loop_buffer)) && (loop_buffer[loop_index] != CHAR_ETX))
  {
    char *ptr;
    
    switch (loop_buffer[loop_index])
    {
      case CMD_SET_STATE:
        loop_index += CHARS_COMMAND;
      
        if ((loop_buffer[loop_index] == STATE_NEUTRAL)
            || (loop_buffer[loop_index] == STATE_BRAKING)
            || (loop_buffer[loop_index] == STATE_FORWARD)
            || (loop_buffer[loop_index] == STATE_REVERSE))
        {
          hubMotorState = loop_buffer[loop_index];
        }
       
        loop_index += CHARS_STATE;
      break;
      
      case CMD_SET_THROTTLE:
        loop_index += CHARS_COMMAND;
        
        memcpy(char_throttle, loop_buffer + loop_index, CHARS_THROTTLE);
        // Append null terminator
        char_throttle[CHARS_THROTTLE] = 0;
        hubMotorThrottle = strtol(char_throttle, &ptr, BASE_10);
        
        loop_index += CHARS_THROTTLE;
      break;
      
      case CMD_SET_ANGLE:
        loop_index += CHARS_COMMAND;
        
        memcpy(char_angle, loop_buffer + loop_index, CHARS_ANGLE);
        // Append null terminator
        char_angle[CHARS_ANGLE] = 0;          
        desiredActuatorAngle = strtod(char_angle, &ptr);
        
        loop_index += CHARS_ANGLE;
      break;      
      
      default:
        loop_index += CHARS_COMMAND;
      break;
    }
  }
}  


void updateHubMotor(void)
{
  switch (hubMotorState)
  {
    default:
    case STATE_NEUTRAL:
      analogWrite(PIN_HM_THROTTLE, 0);
      digitalWrite(PIN_HM_BRAKE, HIGH);
      digitalWrite(PIN_HM_REVERSE, HIGH);
    break;
    
    case STATE_BRAKING:
      analogWrite(PIN_HM_THROTTLE, 0);
      digitalWrite(PIN_HM_BRAKE, LOW);
      digitalWrite(PIN_HM_REVERSE, HIGH);
    break;
    
    case STATE_FORWARD:
      analogWrite(PIN_HM_THROTTLE, hubMotorThrottle);
      digitalWrite(PIN_HM_BRAKE, HIGH);
      digitalWrite(PIN_HM_REVERSE, HIGH);
    break;
    
    case STATE_REVERSE:
      analogWrite(PIN_HM_THROTTLE, hubMotorThrottle);
      digitalWrite(PIN_HM_BRAKE, HIGH);
      digitalWrite(PIN_HM_REVERSE, LOW);
    break;
  }
}



    
    
void setup (void)
{
  hubMotorState = STATE_NEUTRAL;
  hubMotorThrottle = 0;
  desiredActuatorAngle = -1;
  command = 0;
  
  send_index = 0;   // buffer empty
  recv_index = 0;
  loop_index = 0;
  
  recv_buffer_full = false;
  ignore_message = false;
  init_complete = false;
  
  Serial.begin(9600);
  Serial.print("Start setup...");
  

  
  analogWrite(PIN_HM_THROTTLE, 0);
  digitalWrite(PIN_HM_BRAKE, HIGH);
  digitalWrite(PIN_HM_REVERSE, HIGH);
  
  analogWrite(PIN_A_THROTTLE_CW, 0);
  analogWrite(PIN_A_THROTTLE_ACW, 0);
  
  actuatorController.SetMode(AUTOMATIC);
  actuatorController.SetOutputLimits(ACTUATOR_PWM_MAX * -1, ACTUATOR_PWM_MAX);
  
  // turn on SPI in slave mode
  SPCR |= bit (SPE);
  
   // turn on interrupts
  SPCR |= _BV(SPIE);
  // interrupt for SS falling edge
  attachInterrupt(0, ss_falling, FALLING);
  Serial.println("Setup complete!");
}


void sendNextByteInBuffer(void)
{
  byte c;
  
  if (send_index < sizeof(send_buffer))
  {
    c = send_buffer[send_index++];
  }
  
  else
  {
    c = CHAR_ETX;
  }

  SPDR = c;
}


// SPI interrupt routine
ISR (SPI_STC_vect)
{
  byte c = SPDR;  // grab byte from SPI Data Register
  switch (command)
  {
  // If command is zero, then incoming byte is a command
  case 0:
    command = c;
    init_complete = false;
    SPDR = 0;    
  break;
  
  // Command 16 recieve string
  case CMD_RECEIVE:
    // Initialisation!
    if (!init_complete)
    {
      // If recv_buffer is not empty, then we should wait before filling it with a new message
      if (recv_buffer_full)
      {
        ignore_message = true;
      }
      
      else
      {
        ignore_message = false;
        recv_index = 0;
      }
      
      init_complete = true;
    }
  
    if (ignore_message)
      break;
  
    // add to buffer if room and ignore if a command message
    if (recv_index < sizeof(recv_buffer))
    {
      if (c > 30)
      {
        recv_buffer[recv_index] = c;
      }
      
      else if (c == CHAR_ETX)
      {
        recv_buffer[recv_index] = c;
        recv_buffer_full = true;
        // recv_buffer_full should only be set to true once per message.
        ignore_message = true;
      }
    }
     
    else
    {
      recv_buffer_full = true;
      ignore_message = true;
    }
      
    recv_index++;
  break;
    
  // Return angle on request
  case CMD_GET_ANGLE:
    // Initialisation!
    if (!init_complete)
    {
      send_index = 0;
      intToCharArray(send_buffer, analogRead(PIN_A_POSITION_SENSE));
      init_complete = true;
    }
  
    sendNextByteInBuffer();
  break;
  
  // Return actuator signal on request
  case CMD_GET_ACTUATOR:
    // Initialisation!
    if (!init_complete)
    {
      send_index = 0;
      intToCharArray(send_buffer, (int)actuatorControllerOutput);
      init_complete = true;
    }
  
    sendNextByteInBuffer();
  break;  
  
  } // end of switch
}  // end of interrupt routine SPI_STC_vect


// main loop - wait for flag set in interrupt routine
void loop (void)
{
  // If an SPI message has arrived, then process it
  if (recv_buffer_full)
  {
    processSPIMessage();
  }
  
  updateHubMotor();
  updateActuator();

}  // end of loop
