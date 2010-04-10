/* Arduino DCF77 library v0.1
 * Copyright (C) 2006 Mathias Dalheimer (md@gonium.net)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#undef abs
#include "WConstants.h"
#include "HardwareSerial.h"
// include this library's description file
#include "DCF77.h"

DCF77::DCF77(unsigned char dcfPin)
{
  DCF77Pin = dcfPin;
  previousSignalState=0;
  previousFlankTime=0;
  bufferPosition=0;
  dcf_rx_buffer=0;
  ss=mm=hh=day=mon=year=weekday=0;
  Serial.begin(9600);
#ifdef DCF_DEBUG 
  Serial.println("Initializing DCF77 lib");
  Serial.print("Using DCF77 pin #");
  Serial.println(DCF77Pin, DEC);
#endif
  pinMode(DCF77Pin, INPUT);
}

void DCF77::appendSignal(unsigned char signal) {
#ifdef DCF_DEBUG
  Serial.print(", appending value ");
  Serial.print((signal?"HIGH":"low"));
  Serial.print(" at position ");
  Serial.println(bufferPosition);
#endif
  dcf_rx_buffer = dcf_rx_buffer | ((unsigned long long) signal << bufferPosition);
  bufferPosition++;
  if (bufferPosition > 59) {
    finalizeBuffer();
  }
}

void DCF77::finalizeBuffer(void) {
#ifdef DCF_DEBUG
  Serial.println("Finalizing Buffer");
#endif
  if (bufferPosition == 59) {
    struct DCF77Buffer *rx_buffer;
    rx_buffer = (struct DCF77Buffer *)(unsigned long long)&dcf_rx_buffer;

    startBitValid = rx_buffer->S;
#ifdef DCF_DEBUG
  Serial.print("Start bit ");
  Serial.println(startBitValid?"valid":"*** FAULTY ***");
#endif
      
    char parity = 0;  
    //convert the received bits from BCD
    for (char i = 0; i<7; i++)
      parity ^= ((rx_buffer->Min >> i) & 0x01);

    char valid = (parity == rx_buffer->P1);
#ifdef DCF_DEBUG
  Serial.print("Minute ");
  Serial.println(valid?"valid":"*** FAULTY ***");
#endif

    if (startBitValid && valid)
    {
      mm = rx_buffer->Min-((rx_buffer->Min/16)*6);
      minValid = 1;
    }

    parity = 0;    
    for (char i = 0; i<6; i++)
      parity ^= ((rx_buffer->Hour >> i) & 0x01);

    valid = (parity == rx_buffer->P2);
#ifdef DCF_DEBUG
  Serial.print("Hour ");
  Serial.println(valid?"valid":"*** FAULTY ***");
#endif

    if (startBitValid && valid)    
    {
      hh = rx_buffer->Hour-((rx_buffer->Hour/16)*6);
      hourValid = 1;
    }
    
    parity = 0;    
    for (char i = 0; i<6; i++) 
      parity ^= ((rx_buffer->Day >> i) & 0x01);
   
    for (char i = 0; i<3; i++) 
      parity ^= ((rx_buffer->Weekday >> i) & 0x01);

    for (char i = 0; i<5; i++) 
      parity ^= ((rx_buffer->Month >> i) & 0x01);
    
    for (char i = 0; i<8; i++) 
      parity ^= ((rx_buffer->Year >> i) & 0x01);

    valid = (parity == rx_buffer->P3);
#ifdef DCF_DEBUG
  Serial.print("Date ");
  Serial.println(valid?"valid":"*** FAULTY ***");
#endif

    if (startBitValid && valid)
    {
      day= rx_buffer->Day-((rx_buffer->Day/16)*6); 
      mon= rx_buffer->Month-((rx_buffer->Month/16)*6);
      year= 2000 + rx_buffer->Year-((rx_buffer->Year/16)*6);
      weekday = rx_buffer->Weekday;    
      dateValid = 1;
    }
  }
#ifdef DCF_DEBUG
  else
  {
    Serial.println("No buffer of correct length received, all fields faulty");
  }
#endif

  // reset stuff
  ss = 0;
  bufferPosition = 0;
  dcf_rx_buffer=0;
}

void DCF77::serialDumpTime(void){
  Serial.print("Time: ");
  
  if (hourValid)
    Serial.print(hh, DEC);
  else
    Serial.print("h?");      
  
  Serial.print(":");
  
  if (minValid)
    Serial.print(mm, DEC);
  else
    Serial.print("m?");      
  Serial.print(":");
  Serial.print(ss, DEC);
  Serial.print(" Date: ");

  if (!dateValid)
  {
    Serial.println("invalid");
    return;
  }
  
  switch(weekday)
  {
  case 1: 
    Serial.print("Mo");
    break;
  case 2: 
    Serial.print("Di");
    break;
  case 3: 
    Serial.print("Mi");
    break;
  case 4: 
    Serial.print("Do");
    break;
  case 5: 
    Serial.print("Fr");
    break;
  case 6: 
    Serial.print("Sa");
    break;
  case 7: 
    Serial.print("So");
    break;
  default:
    Serial.print("??");
    break;      
  }
  Serial.print(" ");
  Serial.print(day, DEC);
  Serial.print(".");
  Serial.print(mon, DEC);
  Serial.print(".");
  Serial.println(year, DEC);
}

int DCF77::scanSignal(void){
  unsigned char DCFsignal = digitalRead(DCF77Pin);
  if (DCFsignal != previousSignalState) {
    if (DCFsignal == 1) {
      /* We detected a raising flank, increase per one second! */
      addSecond();
#ifdef DCF_DEBUG
      serialDumpTime();
#endif
      int thisFlankTime=millis();
      if (thisFlankTime - previousFlankTime > DCF_sync_millis) {
#ifdef DCF_DEBUG
        Serial.println("####");
        Serial.println("#### Begin of new Minute!!!");
        Serial.println("####");
#endif
        finalizeBuffer();
      }
      previousFlankTime=thisFlankTime;
#ifdef DCF_DEBUG
      Serial.print(previousFlankTime);
      Serial.print(": DCF77 Signal detected, ");
#endif
    } 
    else {
      /* or a falling flank */
      int difference=millis() - previousFlankTime;
#ifdef DCF_DEBUG
      Serial.print("duration: ");
      Serial.print(difference);
#endif
      if (difference < DCF_split_millis) {
        appendSignal(0);
      } 
      else {
        appendSignal(1);
      }
    }
    previousSignalState = DCFsignal;
  }
  return DCFsignal;
}

/**
 * ########################################################
 * ###                 Helpers below                    ###
 * ########################################################
 */

/**
 * Add one second to the hh:mm:ss variables.
 */
void DCF77::addSecond() {
  ss++;
  if (ss==60) {
    ss=0;
    mm++;
    if (mm==60) {
      mm=0;
      hh++;
      if (hh==24) 
        hh=0;
    }
  }
}

