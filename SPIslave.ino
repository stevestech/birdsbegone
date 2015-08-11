#include <SPI.h>
#include <PID_v1.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>

#define DEBUG

#define LENGTH_SPI_BUFFER               13

#define CHARS_THROTTLE                  3      // Within incoming messages, this many characters are allocated to the throttle parameter
#define CHARS_STATE                     1
#define CHARS_COMMAND                   1
#define CHARS_ANGLE                     5

#define CHAR_ETX                        3      // ASCII code for ETX (end of text) character

#define CMD_RECEIVE                     16
#define CMD_GET_ANGLE                   17

#define PWM_OUT_MAX                     255
#define ANALOG_IN_MAX                   1023

#define BASE_10                         10

// Hub motor pins
#define PIN_HM_THROTTLE                 5      // D5 Timer0B
#define PIN_HM_BRAKE                    4      // D4
#define PIN_HM_REVERSE                  7      // D7
#define PIN_HM_CURRENT_SENSE            0      // A0

// Actuator pins
#define PIN_A_THROTTLE                  6      // D6 Timer0A
#define PIN_A_CLOCKWISE                 8      // D8
#define PIN_A_ANTICLOCKWISE             9      // D9 Timer1A    
#define PIN_A_CURRENT_SENSE             1      // A1
#define PIN_A_POSITION_SENSE            2      // A2

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
double desiredActuatorAngle;
double measuredActuatorAngle;
double actuatorControllerOutput;

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
  
  #ifdef DEBUG
/*  boolean etx_present = false;
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
*/
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


void updateActuator(void)
{
  if (desiredActuatorAngle != -1)
  {
    measuredActuatorAngle = analogRead(PIN_A_POSITION_SENSE);
    actuatorController.Compute();
    
    /*
    analogRangeToPwmRange(&actuatorControllerOutput, &clockwise);

    if (clockwise)
    {
      digitalWrite(PIN_A_CLOCKWISE, HIGH);
      digitalWrite(PIN_A_ANTICLOCKWISE, LOW);
    }
    
    else
    {
      digitalWrite(PIN_A_CLOCKWISE, LOW);
      digitalWrite(PIN_A_ANTICLOCKWISE, HIGH);
    }
    
    analogWrite(PIN_A_THROTTLE, actuatorControllerOutput);
    
    */
    
    #ifdef DEBUG
      char message[100];
      sprintf(message, "IN %d  OUT %d  SETPOINT %d", (int)measuredActuatorAngle, (int)actuatorControllerOutput, (int)desiredActuatorAngle);
      Serial.println(message);
    #endif
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
  
  Serial.begin(9600);
  Serial.print("Start setup...");
  
  // Have to send on MISO
  pinMode(MISO, OUTPUT);
  pinMode(PIN_HM_THROTTLE, OUTPUT);
  pinMode(PIN_HM_BRAKE, OUTPUT);
  pinMode(PIN_HM_REVERSE, OUTPUT);
  
  pinMode(PIN_A_THROTTLE, OUTPUT);
  pinMode(PIN_A_CLOCKWISE, OUTPUT);
  pinMode(PIN_A_ANTICLOCKWISE, OUTPUT);
  
  analogWrite(PIN_HM_THROTTLE, 0);
  digitalWrite(PIN_HM_BRAKE, HIGH);
  digitalWrite(PIN_HM_REVERSE, HIGH);
  
  analogWrite(PIN_A_THROTTLE, 0);
  digitalWrite(PIN_A_CLOCKWISE, LOW);
  digitalWrite(PIN_A_ANTICLOCKWISE, LOW); 
  
  actuatorController.SetMode(AUTOMATIC);
  
  // turn on SPI in slave mode
  SPCR |= bit (SPE);
  
   // turn on interrupts
  SPCR |= _BV(SPIE);
  // interrupt for SS falling edge
  attachInterrupt(0, ss_falling, FALLING);
  Serial.println("Setup complete!");
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
    SPDR = 0;
    
    if (command == CMD_GET_ANGLE)
    {
      send_index = 0;
      intToCharArray(send_buffer, analogRead(PIN_A_POSITION_SENSE));
    }
    
    else if (command == CMD_RECEIVE)
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
    }
  break;
  
  // Command 16 recieve string
  case CMD_RECEIVE:
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
    if (send_index < sizeof(send_buffer))
    {
      c = send_buffer[send_index++];
    }
    
    else
    {
      c = CHAR_ETX;
    }
    
    SPDR = c;
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
