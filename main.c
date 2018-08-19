//////////////////////////////////////////////////////////////////////
// Main.c
//
// Raspberry Pi driving the Max7219 and reading 2 buttons
//
// to compile : gcc -Wall -lwiringPi demo2.c -o demo
//
//////////////////////////////////////////////////////////////////////

#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <stdbool.h>

// Program constants
#define DELAY		500000

// pin definitions
#define DATA        0 // GPIO 17 : header pin 11
#define CLOCK       3 // GPIO 22 : header pin 15
#define LOAD        4 // GPIO 23 : header pin 16
#define BUTT_L      5 // GPIO 24 : header pin 18
#define BUTT_R      6 // GPIO 25 : header pin 22
#define LEFT        1
#define RIGHT       2
#define BOTH        3

// Max7219 Registers
#define DECODE_MODE   0x09
#define INTENSITY     0x0a
#define SCAN_LIMIT    0x0b
#define SHUTDOWN      0x0c
#define DISPLAY_TEST  0x0f

char matrix[8] = {0,0,0,0,0,0,0,0};

/// Clock out the data bits
static void Send16bits (unsigned short output)
{
  unsigned char i;

  for (i=16; i>0; i--)
  {
    unsigned short mask = 1 << (i - 1); // bitmask

    digitalWrite(CLOCK, 0);  // set clock to 0

    // Send one bit on the data pin

    if (output & mask)
      digitalWrite(DATA, 1);
    else
      digitalWrite(DATA, 0);

    digitalWrite(CLOCK, 1);  // set clock to 1
  }
}

// Take a reg number and data then send to the max7219
static void MAX7219Send (unsigned char reg_number, unsigned char dataout)
{
  digitalWrite(LOAD, 1);  // set LOAD 1 to start
  Send16bits((reg_number << 8) + dataout);   // send 16 bits ( reg number + dataout )
  digitalWrite(LOAD, 0);  // LOAD 0 to latch
  digitalWrite(LOAD, 1);  // set LOAD 1 to finish
  delay(100);
}

/// delay function
/// mseconds delay duration in micro seconds
void delay( unsigned int mseconds )
{
    clock_t goal = mseconds + clock();
    while (goal > clock());
}

/// Sends 0x00 to all display columns
void blank(void) 
{
  for( int i = 0; i < 9; i++)
  {
    MAX7219Send(i + 1, 0);
    matrix[i] = 0;
  }
}

/// Turn on the pixle at X,Y
/// The Columns are the display number 0 to 7
/// The rows are bytes in array
void SetXY(int x, int y)
{
  char temp = matrix[y];
  char t2 = 1<<x;
  matrix[y] = t2 | temp;
}

void ResetXY(int x, int y)
{
  char temp = matrix[y];
  char t2 =  1<<x;
  t2 = ~t2;	// flip the bits
  matrix[y] = t2 & temp;
}

/// Configure the pins and set the MAX 7219 registers
void Setup(void)
{
  //We need 3 output pins to control the Max7219: Data, Clock and Load
  pinMode(DATA, OUTPUT);
  pinMode(CLOCK, OUTPUT);
  pinMode(LOAD, OUTPUT);
  pinMode(BUTT_L, INPUT);
  pinMode(BUTT_R, INPUT);
  pullUpDnControl (BUTT_L, PUD_UP);
  pullUpDnControl (BUTT_R, PUD_UP);

  // setup the 7219
  MAX7219Send(SCAN_LIMIT, 7);
  MAX7219Send(DECODE_MODE, 0);   // Set BCD decode mode on
  MAX7219Send(DISPLAY_TEST, 0);  // Disable test mode
  MAX7219Send(INTENSITY, 1);     // set brightness 0 to 15
  MAX7219Send(SHUTDOWN, 1);      // come out of shutdown mode	/ turn on the digits
}

/// read both buttons, NO debounce implemented !
int ReadButtons()
{
  int result = 0;

  if (digitalRead(BUTT_L) == LOW)
    result += 1;
  if (digitalRead(BUTT_R) == LOW)
    result += 2;

  return result;
}

void RefreshDisplay()
{
  for(int i = 0; i < 8; i++)// refresh display
  {
    // Note corrects for 7219 display number - 1 to 8 and data 0 to 7 !
    MAX7219Send(i+1,matrix[i]);
  }
}

void DrawMyBat(int x)
{
  int y = 0;
  SetXY(x, y);
  SetXY(x+1, y);
}

//enum e_State{ init = 0, patten1 = 1, patten2 = 2, patten3 = 3, quit = 4};
//enum e_State SysState;

int main (void)
{
  int SysState = 0;
  int BatX = 3;		// initial position of bat

  if (wiringPiSetup() == -1)
    exit(1);

  Setup();
//  SysState = patten1;
  SysState = 1;

  /// Blank the matrix
  blank();

printf("Start State = %i\n", SysState);

//printf("Matrix [%i],[%i],[%i],[%i],[%i],[%i],[%i],[%i]\n", matrix[0], matrix[1],  matrix[2],  matrix[3],  matrix[4],  matrix[5],  matrix[6],  matrix[7]);

  while (true)
  {
    int butts = ReadButtons();

    switch(SysState)
    {
//      case init:
      case 0:
      default:
        printf("State = %i  Buttons = %i\n", SysState, butts);
//        printf("OK here\n");
        break;
//      case patten1:
      case 1:
        switch(butts)
        {
          case LEFT:
//            printf("LEFT\n");
            if (BatX >= 1) // room to draw the bat?
              BatX--;
            break;
          case RIGHT:
  //          printf("RIGHT\n");
            if (BatX < 6)
              BatX++;
            break;
          case BOTH:
            printf("LEFT & RIGHT\n");
            break;

          default:
           // printf("patten1 default error\n");
            break;
        }

        break;
    }

// Update display
    blank();
    DrawMyBat(BatX);
    RefreshDisplay();

    delay(500000);
  }
  return 0;
}

