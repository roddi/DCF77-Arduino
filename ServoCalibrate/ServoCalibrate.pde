// Sweep
// by BARRAGAN <http://barraganstudio.com> 

#include <Servo.h> 
 
Servo servo2;               // a maximum of eight servo objects can be created 
Servo servo3;
 
int servo2Pin=10;
int servo3Pin=9;
int blinkPin=13;

/*
int pos2Min = 47;   
int pos2Max = 124;
int pos2Inc = 7;
*/

int hourMin = 1;
int hourMax = 12;

/*
char hourTable[] = 
{
  47,  // 1 (1)
  54,  // 2 (4)
  61,  // 3 (7)
  68,  // 4 (10)
  75,  // 5 (2)
  82,  // 6 (5)
  89,  // 7 (8)
  96,  // 8 (11)
  103, // 9 (3)
  110, // 10 (6)
  117, // 11 (9)
  124  // 12 (12)
};
*/

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

/*
int pos3Min = 32;   
int pos3Max = 150;
int pos3Inc = 2;
*/

int minuteMin = 1;
int minuteMax = 60;

int positionForMinute(char inMinute)
{
  if (inMinute < 1 || inMinute > 60)
    return 32;
    
   return 30 + (2 * inMinute);
}

void setup() 
{ 
  servo2.attach(servo2Pin);
  servo3.attach(servo3Pin);
  pinMode(blinkPin, OUTPUT);  
} 
 
int hour = hourMax;
int minute = minuteMax;

unsigned int wait = 2000;
char blinkState = HIGH;
 
void loop() 
{ 
  if (hour > hourMax)
    hour = hourMin;

  if (minute > minuteMax)
    minute = minuteMin;
  
  servo2.write(positionForHour(hour));              // tell servo to go to position in variable 'pos' 
  servo3.write(positionForMinute(minute));              // tell servo to go to position in variable 'pos' 
  delay(wait);                       // waits 15ms for the servo to reach the position 
  digitalWrite(blinkPin, blinkState);

  hour++;
  minute++;
  minute++;
  blinkState = (blinkState == LOW) ? HIGH : LOW;
} 

