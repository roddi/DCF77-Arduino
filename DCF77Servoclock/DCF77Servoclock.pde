#include <Servo.h>

#include "DCF77.h"

#define redPin 13


int DCF77Pin=12;
int blinkPin=13;
int servo1Pin=11;
int servo2Pin=10;
int servo3Pin=9;

int testPin=6;


DCF77 myDCF=DCF77(DCF77Pin);
Servo servo2;
Servo servo3;


int hourMin = 1;
int hourMax = 12;

char hourTable[] = 
{
  47,  // 1 (1)
  75,  // 5 (2)
  103, // 9 (3)
  54,  // 2 (4)
  82,  // 6 (5)
  110, // 10 (6)
  61,  // 3 (7)
  89,  // 7 (8)
  117, // 11 (9)
  68,  // 4 (10)
  96,  // 8 (11)
  124  // 12 (12)
};


char positionForHour(char inHour)
{
  if (inHour < 1 || inHour > 12)
    return 47;
    
  char position = hourTable[inHour-1];
  
  return position;
}


int minuteMin = 1;
int minuteMax = 60;

int positionForMinute(char inMinute)
{
  if (inMinute < 1 || inMinute > 60)
    return 32;
    
   return 30 + (2 * inMinute);
}



void setup(void) 
{
  Serial.begin(9600); 

  pinMode(blinkPin, OUTPUT);
  digitalWrite(blinkPin, HIGH);

  pinMode(7, OUTPUT);  
  pinMode(8, OUTPUT);
  
  servo2.attach(servo2Pin);
  servo3.attach(servo3Pin);
  
  delay(100);

  digitalWrite(blinkPin, LOW);
}

char previousSecond = 0;
char previousMinute = 0;
char previousHour = 0;
char previousWeekday = 0;

unsigned char testServo = 0;

void loop(void) 
{
  int DCFsignal = myDCF.scanSignal();
  if (DCFsignal) 
  {
    digitalWrite(blinkPin, HIGH);
  } 
  else 
  {
    digitalWrite(blinkPin, LOW);
  }

  char hours=myDCF.hh;
  char minutes=myDCF.mm;
  char seconds=myDCF.ss;
  char weekday = myDCF.weekday;
  if (seconds != previousSecond) 
  {
    myDCF.serialDumpTime();

    if (previousMinute != minutes)
    {
      servo3.write(positionForMinute(minutes));
      
      previousMinute = minutes;
    }
    
    if (hours > 12)
      hours -= 12;
      
    servo2.write(positionForHour(hours));
  }    
  previousSecond = seconds;
}
