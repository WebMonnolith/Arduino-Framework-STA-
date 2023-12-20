/*
 License
 **********************************************************************************
 This program is free software; you can redistribute it 
 and/or modify it under the terms of the GNU General    
 Public License as published by the Free Software       
 Foundation; either version 3 of the License, or        
 (at your option) any later version.                    
                                                        
 This program is distributed in the hope that it will   
 be useful, but WITHOUT ANY WARRANTY; without even the  
 implied warranty of MERCHANTABILITY or FITNESS FOR A   
 PARTICULAR PURPOSE. See the GNU General Public        
 License for more details.                              
                                                        
 You should have received a copy of the GNU General    
 Public License along with this program.
 If not, see <http://www.gnu.org/licenses/>.
                                                        
 Licence can be viewed at                               
 http://www.gnu.org/licenses/gpl-3.0.txt

 Please maintain this license information along with authorship
 and copyright notices in any redistribution of this code
 **********************************************************************************
 */

#ifndef _STA_DISPLAY_
#define _STA_DISPLAY_

#include <Arduino.h>
#include "sta.h"
#include "utility.h"


BEGIN_NP_BLOCK

#pragma GCC visibility push(default)
#pragma GCC diagnostic ignored "-Wshift-count-overflow"

#if defined(HARDWARE_SERIAL)
# define USE_HARDWARE_SERIAL
#else
# define USE_SOFTWARE_SERIAL 
#endif

#if defined(USE_SOFTWARE_SERIAL)
# include <SoftwareSerial.h>
#elif defined(USE_HARDWARE_SERIAL)
# include <HardwareSerial.h>
#endif

class _STAXXEXPORT nextion_serial {
public:
  nextion_serial() = default;//Empty contructor
#if defined(USE_SOFTWARE_SERIAL)
  nextion_serial(SoftwareSerial &next, uint32_t baud)
    : nextion(&next)
  {
    nextion->begin(baud);
    flushSerial();
  }
#else
  nextion_serial(HardwareSerial &next, uint32_t baud)
  : nextion(&next)
  {
    nextion->begin(baud);
    flushSerial();
  }
#endif
  void buttonToggle(boolean &buttonState, String objName, uint8_t picDefualtId, uint8_t picPressedId);

  uint8_t buttonOnOff(String find_component, String unknown_component, uint8_t pin, int btn_prev_state);

  boolean setComponentValue(String component, int value);

  //boolean ack(void);//Deprecated

  boolean ack(void);

  unsigned int getComponentValue(String component);

  boolean setComponentText(String component, String txt);
  
  boolean updateProgressBar(int x, int y, int maxWidth, int maxHeight, int value, int emptyPictureID, int fullPictureID, int orientation=0);

  String getComponentText(String component, uint32_t timeout = 100);

  String listen(unsigned long timeout=100);
  //  String listennextionGeneric(unsigned long timeout=100);

  void sendCommand(const char* cmd);

  uint8_t pageId(void);

  boolean init(const char* pageId = "0");

private:
  void flushSerial();
#if defined(USE_SOFTWARE_SERIAL)
  SoftwareSerial *nextion;
#else
  HardwareSerial *nextion;
#endif
};
#endif

/* Implementations */

void nextion_serial::buttonToggle(boolean &buttonState, String objName, uint8_t picDefualtId, uint8_t picSelected){
  String tempStr = "";
  if (buttonState) {
    tempStr = objName + ".picc="+String(picDefualtId);//Select this picture
    this->sendCommand(tempStr.c_str());
    tempStr = "ref "+objName;//Refresh component
    this->sendCommand(tempStr.c_str());
    buttonState = false;
  } else {
    tempStr = objName + ".picc="+String(picSelected);//Select this picture
    this->sendCommand(tempStr.c_str());
    tempStr = "ref "+objName;//Refresh this component
    this->sendCommand(tempStr.c_str());
    buttonState = true;
  }
}

uint8_t nextion_serial::buttonOnOff(String find_component, String unknown_component, uint8_t pin, int btn_prev_state){  
  uint8_t btn_state = btn_prev_state;
  if((unknown_component == find_component) && (!btn_state)){
    btn_state = 1;//Led is ON
    sta::pin_write(pin, HIGH);
  }else if((unknown_component == find_component) && (btn_state)){
    btn_state = 0;
    sta::pin_write(pin, LOW);
  }else{
    //return -1;
  }
  return btn_state;
}

boolean nextion_serial::setComponentValue(String component, int value){
  String compValue = component +".val=" + value;
  this->sendCommand(compValue.c_str());
  boolean acki = this->ack();
  return acki;
}

boolean nextion_serial::ack(void){
  uint8_t bytes[4] = {0};
  this->nextion->setTimeout(20);
  if (sizeof(bytes) != nextion->readBytes((char *)bytes, sizeof(bytes))){
    return 0;
  }

  if((bytes[1]==0xFF)&&(bytes[2]==0xFF)&&(bytes[3]==0xFF)){
    switch (bytes[0]) {
      case 0x00:
        return false; break;
        //return "0"; break;      
      case 0x01:
        return true; break;
      default: 
        return false;
    }
  }

  return NULL;
}

unsigned int nextion_serial::getComponentValue(String component){
  String getValue = "get "+ component +".val";//Get componetn value
    unsigned int value = 0;
  this->sendCommand(getValue.c_str());
  uint8_t temp[8] = {0};
  this->nextion->setTimeout(20);
  if (sizeof(temp) != this->nextion->readBytes((char *)temp, sizeof(temp)))
    return -1;
  if((temp[0]==(0x71))&&(temp[5]==0xFF)&&(temp[6]==0xFF)&&(temp[7]==0xFF))
    value = (temp[4] << 24) | (temp[3] << 16) | (temp[2] << 8) | (temp[1]);
  return value;
}

boolean nextion_serial::setComponentText(String component, String txt){
  String componentText = component + ".txt=\"" + txt + "\"";
  this->sendCommand(componentText.c_str());
  return this->ack();
}

boolean nextion_serial::updateProgressBar(
  int x,
  int y,
  int maxWidth,
  int maxHeight,
  int value, 
  int emptyPictureID, 
  int fullPictureID, 
  int orientation)
{
	int w1 = 0;
	int h1 = 0;
	int w2 = 0;
	int h2 = 0;
	int offset1 = 0;
	int offset2 = 0;

	if(orientation == 0) {
    value = map(value, 0, 100, 0, maxWidth);
    w1 = value;
    h1 = maxHeight;
    w2 = maxWidth - value;
    h2 = maxHeight;
    offset1 = x + value;
    offset2 = y;
	}
  else {
    value = map(value, 0, 100, 0, maxHeight);
    offset2 = y;	
    y = y + maxHeight - value;
    w1 = maxWidth;
    h1 = value;
    w2 = maxWidth;
    h2 = maxHeight - value;
    offset1 = x;
	}
	
	String wipe = "picq " + String(x) + "," + String(y) + "," + String(w1) + "," + String(h1) + "," + String(fullPictureID);
	this->sendCommand(wipe.c_str());
	wipe = "picq " + String(offset1) + "," + String(offset2) + "," + String(w2) + "," + String(h2) + "," + String(emptyPictureID);
	this->sendCommand(wipe.c_str());

	return this->ack();

}//end updateProgressBar

String nextion_serial::getComponentText(String component, uint32_t timeout){
  String tempStr = "get " + component + ".txt";
  this->sendCommand(tempStr.c_str());
  tempStr = "";
  tempStr = listen(timeout);
  return tempStr;
}

String nextion_serial::listen(unsigned long timeout){
  char _bite;
  char _end = 0xff;
  String cmd;
  int countEnd = 0;

  while(this->nextion->available()>0){
    sta::safe_delay(10);
    if(nextion->available()>0){
      _bite = this->nextion->read();
      cmd += _bite;
      if(_bite == _end) countEnd++;
      if(countEnd == 3) break;
    }
  }

  String temp = "";
  switch (cmd[0]) {
    case 'e':
      countEnd = 0;
      for(uint8_t i = 0; i<cmd.length(); i++){
        if(cmd[i] == _end) countEnd++;
        temp += String(cmd[i], HEX);
        if(countEnd == 3)
          return temp;
        temp += " ";
      }
      break;
    case 'f'://0x66
      return String(cmd[1], DEC);
      break;
    case 'g'://0x67
      cmd = String(cmd[2], DEC) + "," + String(cmd[4], DEC) +","+ String(cmd[5], DEC);
      return cmd;
      break;
    case 'h'://0x68
      cmd = String(cmd[2], DEC) + "," + String(cmd[4], DEC) +","+ String(cmd[5], DEC);
      cmd = "68 " + cmd;	
      return cmd;
      break;
    case 'p'://0x70
      cmd = cmd.substring(1, cmd.length()-3);
      cmd = "70 " + cmd;
      return cmd;
      break;
    default: 
      return cmd;
      break;
  }
  return "";
}

uint8_t nextion_serial::pageId(void){
  this->sendCommand("sendme");
  String pagId = listen();
  if(pagId != "")
	  return pagId.toInt();
  return -1;
  
}

void nextion_serial::sendCommand(const char* cmd){
  while (nextion->available())
	  this->nextion->read();
  
  this->nextion->print(cmd);
  this->nextion->write(0xFF);
  this->nextion->write(0xFF);
  this->nextion->write(0xFF);
}

boolean nextion_serial::init(const char* pageId){
  String page = "page " + String(pageId);
  this->sendCommand("");
  this->ack();
  this->sendCommand(page.c_str());
  delay(100);
  return ack();
}

void nextion_serial::flushSerial(){
  Serial.flush();
  this->nextion->flush();
}

 
END_NP_BLOCK





