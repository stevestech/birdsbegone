#include <SPI.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>

#define DEBUG

#define LENGTH_SPI_BUFFER               12

#define CHARS_THROTTLE                  3      // Within incoming messages, this many characters are allocated to the throttle parameter
#define CHARS_STATE                     1
#define CHARS_COMMAND                   1
#define CHARS_ANGLE                     4

#define CHAR_ETX                        3      // ASCII code for ETX (end of text) character

#define CMD_RECEIVE                     16
#define CMD_GET_ANGLE                   17

#define THROTTLE_MAX                    255

#define PIN_THROTTLE                    5      // D5
#define PIN_BRAKE                       4      // D4
#define PIN_REVERSE                     7      // D7

#define STATE_NEUTRAL                   '0'
#define STATE_BRAKING                   '1'
#define STATE_FORWARD                   '2'
#define STATE_REVERSE                   '3'

#define CMD_SET_STATE                   '0'
#define CMD_SET_THROTTLE                '1'
#define CMD_SET_ANGLE                   '2'





char state = STATE_NEUTRAL;

char char_throttle [CHARS_THROTTLE + 1];
// Long because strtol returns a long
long throttle = 0;

char char_angle [CHARS_ANGLE + 1];
long angle = 0;

char *ptr;

char send_buffer[LENGTH_SPI_BUFFER];
volatile byte send_index;

char recv_buffer[LENGTH_SPI_BUFFER];
boolean recv_buffer_full;
volatile byte recv_index;

char loop_buffer[LENGTH_SPI_BUFFER];
volatile byte loop_index;

volatile boolean ignore_message;
byte command = 0;

// start of transaction, no command yet
void ss_falling ()
{
  command = 0;
}  // end of interrupt service routine (ISR) ss_falling



/* 
 * Outputs a char array in the format "0007", for a value of 7
 * or "-015" for a value of -15, if LENGTH_ANGLE = 4.
 * The last character in toSend will be set to ETX, not NULL.
 * The size of toSend will be LENGTH_ANGLE + 1 due to ETX
 **/
void intToCharArray(char *toSend, int value)
{  
  char format[16];
  int lowerLimit;
  int upperLimit;
  
  // Example: "%04d" if LENGTH_ANGLE is 5
  sprintf(format, "%%0%dd", CHARS_ANGLE);  
  
  // Does value fit into LENGTH_ANGLE chars?
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

void setup (void)
{
  Serial.begin (9600);
  Serial.print("Start setup...");
  // Have to send on MISO
  pinMode(MISO, OUTPUT);
  pinMode(PIN_THROTTLE, OUTPUT);
  pinMode(PIN_BRAKE, OUTPUT);
  pinMode(PIN_REVERSE, OUTPUT);
  
  analogWrite(PIN_THROTTLE, 0);
  digitalWrite(PIN_BRAKE, HIGH);
  digitalWrite(PIN_REVERSE, HIGH);

  // Init Interrupt Vars
  send_index = 0;   // buffer empty
  recv_index = 0;
  loop_index = 0;
  recv_buffer_full = false;
  ignore_message = false;
  
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
      intToCharArray(send_buffer, -987);
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
        
        
    //Serial.print("Command recieved: ");
    //Serial.println(command);
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

void updateGPIO(void)
{
  switch (state)
  {
    default:
    case STATE_NEUTRAL:
      analogWrite(PIN_THROTTLE, 0);
      digitalWrite(PIN_BRAKE, HIGH);
      digitalWrite(PIN_REVERSE, HIGH);
    break;
    
    case STATE_BRAKING:
      analogWrite(PIN_THROTTLE, 0);
      digitalWrite(PIN_BRAKE, LOW);
      digitalWrite(PIN_REVERSE, HIGH);
    break;
    
    case STATE_FORWARD:
      analogWrite(PIN_THROTTLE, throttle);
      digitalWrite(PIN_BRAKE, HIGH);
      digitalWrite(PIN_REVERSE, HIGH);
    break;
    
    case STATE_REVERSE:
      analogWrite(PIN_THROTTLE, throttle);
      digitalWrite(PIN_BRAKE, HIGH);
      digitalWrite(PIN_REVERSE, LOW);
    break;
  }
}

// main loop - wait for flag set in interrupt routine
void loop (void)
{ 
  if (recv_buffer_full)
  {
    memcpy(loop_buffer, recv_buffer, LENGTH_SPI_BUFFER);
    recv_buffer_full = false;
    loop_index = 0;
    
    #ifdef DEBUG
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
      switch (loop_buffer[loop_index])
      {
        case CMD_SET_STATE:
          loop_index += CHARS_COMMAND;
        
          if ((loop_buffer[loop_index] == STATE_NEUTRAL)
              || (loop_buffer[loop_index] == STATE_BRAKING)
              || (loop_buffer[loop_index] == STATE_FORWARD)
              || (loop_buffer[loop_index] == STATE_REVERSE))
          {
            state = loop_buffer[loop_index];
          }
         
          loop_index += CHARS_STATE;
        break;
        
        case CMD_SET_THROTTLE:
          loop_index += CHARS_COMMAND;
          
          memcpy(char_throttle, loop_buffer + loop_index, CHARS_THROTTLE);
          char_throttle[CHARS_THROTTLE] = 0;
          throttle = strtol(char_throttle, &ptr, 10);
          
          loop_index += CHARS_THROTTLE;
        break;
        
        case CMD_SET_ANGLE:
          loop_index += CHARS_COMMAND;
          
          memcpy(char_angle, loop_buffer + loop_index, CHARS_ANGLE);
          char_angle[CHARS_ANGLE] = 0;          
          angle = strtol(char_angle, &ptr, 10);
          
          loop_index += CHARS_ANGLE;
        break;
        
        default:
          loop_index += CHARS_COMMAND;
        break;
      }
    }
    
    updateGPIO();
    
  }  // end of flag set

}  // end of loop
