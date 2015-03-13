 //    serialctl
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#include "packet.h"
#include "hw.h"
#include "zserio.h"
#include <Servo.h>
#include "globals.h"
packet_t pA, pB, safe;
packet_t *astate, *incoming;
comm_state cs;
long last_p,last_s=0,usec;

#define SerComm Serial
#define htons(x) ( ((x)<<8) | (((x)>>8)&0xFF) )
#define ntohs(x) htons(x)
#define htonl(x) ( ((x)<<24 & 0xFF000000UL) | ((x)<< 8 & 0x00FF0000UL) | ((x)>> 8 & 0x0000FF00UL) | ((x)>>24 & 0x000000FFUL) )
#define ntohl(x) htonl(x)
#define DEADBAND_HALF_WIDTH 5

#define WATCHDOG_

#ifdef WATCHDOG_
#include <avr/wdt.h>      //watchdog library timer loop resets the watch dog
#endif

int getButton(int num){
        if(num<=7){
                return (astate->btnlo >> num) & 0x01;
        } else if(num>7 && num <= 15){
                return (astate->btnhi >> (num - 8)) & 0x01;
        } else {
                return 0;
        }
}
void setup() {
  #ifdef WATCHDOG_
  wdt_enable(WDTO_250MS);  //Set 250ms WDT 
  wdt_reset();             //watchdog timer reset 
  #endif
  //Initialize safe to safe values!!
  safe.stickX = 127;
  safe.stickY = 127;
  safe.btnhi = 0;
  safe.btnlo = 0;
  safe.cksum = 0b1000000010001011;
  SerComm.begin(115200);
  comm_init();
  init_pins();
  last_p = millis();
  drive_left(0);
  drive_right(0);
  //copy safe values over the current state
  memcpy(astate, &safe, sizeof(packet_t));
}
void loop(){
  //Every line sent to the computer gets us a new state
  wdt_reset();
  print_data();
  comm_parse();
/*
//arm
  if((getButton(5) ^ getButton(7))){
    //both up and down buttons at same time is invalid
    if(getButton(5)){
        arm.writeMicroseconds(1000);
        }
    if(getButton(7)){
        arm.writeMicroseconds(2000);
        }
    } else{
    arm.writeMicroseconds(1500);
    }
    //winch
 if((getButton(0) ^ getButton(1))){
    //both up and down buttons at same time is invalid
    if(getButton(0)){
        winch.writeMicroseconds(1000);
        }
    if(getButton(1)){
        winch.writeMicroseconds(2000);
        }
    } else{
    winch.writeMicroseconds(1500);
    }

 
  //Runs this every 500ms
  if(millis()-last_p >= 500){
    //no compressor
    
    last_p=millis();
  }
  */
  tank_drive();
  
  //limits data rate
  delay(75);
}
void tank_drive(){
  int power_out = 0;
  int turn_out  = 0;
  int zeroed_power =    ((int)(astate->stickX) - 127);
  int zeroed_turn =     -1*((int)(astate->stickY) - 127);
  
  if(abs(zeroed_power) > DEADBAND_HALF_WIDTH){
    if(zeroed_power>0){
      power_out = zeroed_power - DEADBAND_HALF_WIDTH;
    } else {
      power_out = zeroed_power + DEADBAND_HALF_WIDTH;
    }
  }
  if(abs(zeroed_turn) > DEADBAND_HALF_WIDTH){
    if(zeroed_turn>0){
      turn_out = zeroed_turn - DEADBAND_HALF_WIDTH;
    } else {
      turn_out = zeroed_turn + DEADBAND_HALF_WIDTH;
    }
  }
  int left_out =     2*(power_out + (turn_out/8));
  int right_out = 2*(-1*power_out + (turn_out/8));

  drive_left(left_out);
  drive_right(right_out);
}
