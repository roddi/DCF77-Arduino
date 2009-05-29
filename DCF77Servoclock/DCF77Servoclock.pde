#include <DCF77.h>

#define redPin 13


int DCF77Pin=12;
int blinkPin=13;
int seconds=0;
int previousSecond =0;
int minutes=0;
int hours=0;
DCF77 myDCF=DCF77(DCF77Pin);


void setup(void) 
{
  Serial.begin(9600); 

  pinMode(blinkPin, OUTPUT);
  pinMode(8, OUTPUT);
  digitalWrite(8, HIGH);
  delay(300);
  digitalWrite(8, LOW);
}


void loop(void) 
{
  //Serial.println("loop!");
  int DCFsignal = myDCF.scanSignal();
  if (DCFsignal) 
  {
    digitalWrite(blinkPin, HIGH);
  } 
  else 
  {
    digitalWrite(blinkPin, LOW);
  }
  hours=myDCF.hh;
  minutes=myDCF.mm;
  seconds=myDCF.ss;
  if (seconds != previousSecond) 
    myDCF.serialDumpTime();
  delay(20);
  previousSecond = seconds;
}
