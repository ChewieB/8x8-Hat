
//////////////////////////////////////////////////////////////////////
// Demo2.c
//
// Raspberry Pi reading GPIO 24/25
//
// to compile : gcc -Wall -lwiringPi demo3.c -o demo
//
//////////////////////////////////////////////////////////////////////

#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <stdbool.h>

// pin definitions
#define BUTT_L      5 // GPIO 24 ( header pin 18 )
#define BUTT_R      6 // GPIO 25 ( header pin 22 )

/// delay function
/// mseconds delay duration in micro seconds
void delay( unsigned int mseconds )
{
    clock_t goal = mseconds + clock();
    while (goal > clock());
}

/// Setup the IO pins
void Setup(void)
{
  pinMode(BUTT_L, INPUT);
  pinMode(BUTT_R, INPUT);
  pullUpDnControl (BUTT_L, PUD_UP);
  pullUpDnControl (BUTT_R, PUD_UP);
}

int main (void)
{
  if (wiringPiSetup() == -1)
    exit(-1);

  Setup();
  while (true)
  {
    int left = digitalRead(5);
    int right = digitalRead(6);
    if ( left == LOW)
      printf("LEFT\n");
    if (right == LOW)
      printf("RIGHT\n");
    delay(500000);
  }
  return 0;
}


