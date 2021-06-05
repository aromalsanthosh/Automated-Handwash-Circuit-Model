/*A simple circuit with a timer, that should be "installed" on your tap and
 can also control the water flow. It'll notify you with a sound and a message 
 on the display if you didn't wash your hands enough.
There is also a button to restart the timer, if necessary*/

#include <LiquidCrystal.h>

const int sigPin = 2; 		//one pin for ECHO & SIGNAL
const int outPin = 13; 		//pin for signal output(this case piezo)
const int wash1Time = 2; 		//min time for performing action (in seconds)
const int rubTime = 5; 		//min time for performing action (in seconds)
const int wash2Time = 3; 		//min time for performing action (in seconds)
const int distance = 100; 	//distance for trigger the timer (in CM)
const int actionPin = 12;	
const int interrPin = 3;	//pin to trigger for resetting the timer


bool perfAction, wash1Complete, rubComplete, wash2Complete, dirtyLCD;	
unsigned long startingTime;
long distanceCM, remainSec, totalTime;

LiquidCrystal lcd(10, 9, 4, 5, 6, 7);


void setup()
{
  	Serial.begin(9600);
	pinMode(outPin, OUTPUT);
	pinMode(actionPin, OUTPUT);
  	pinMode(interrPin, INPUT_PULLUP);
  	perfAction = false;
  	completeToFalse();
  
  	totalTime = wash1Time + rubTime + wash2Time;
    
  	attachInterrupt(digitalPinToInterrupt(interrPin), reset, RISING);
  	
  	lcd.begin(16, 2);
  	printHeaderLCD("    WELCOME!");
  	delay(500);
  	printIdleLCD();
}

void loop()
{    	
  	distanceCM=calculateDistanceCM();
   	if (distanceCM < distance){
      	if(!perfAction){
           	printHeaderLCD("     WASH!!");
        	perfAction=true;
          	startingTime=millis();
          	digitalWrite(actionPin, HIGH);
        }
      	if(startingTime!=0){
        	remainSec=totalTime-(millis()-startingTime)/1000;
          	if(remainSec < rubTime + wash2Time && ! wash1Complete){
          		wash1Complete = true;
              	printHeaderLCD("      RUB!");
                digitalWrite(actionPin, LOW);
            }
          	if(remainSec < wash2Time && ! rubComplete){
          		rubComplete = true;
              	printHeaderLCD("     WASH!!");
                digitalWrite(actionPin, HIGH);
            }
          
          
        	if (remainSec>=0){
              	printLineLCD("  left: " + String(remainSec) + "sec.  ");
            }else if (!wash2Complete){
              	wash2Complete = true;
            	printLinesLCD("   COMPLETED!","  hands clean!  ");
               	digitalWrite(actionPin, LOW);
            }
      }
   }else{          
        if((millis()-startingTime) < totalTime*1000 && perfAction){
			alert();
        }else if (((millis()-startingTime) < totalTime*1000 || perfAction)){
            printIdleLCD();
        }
       	completeToFalse();
    	perfAction = false;
    }
  	digitalWrite(outPin,LOW);
  	delay(200);
}

void alert(){
  	printLinesLCD("   NOT WASHED","    ENOUGH!");
  	tone(outPin,2000,200);
  	digitalWrite(actionPin, LOW);
   	startingTime=0;        
  	delay(1000);
	printIdleLCD();
  
}

unsigned long calculateDistanceCM(){
	long duration;
  
 	pinMode(sigPin, OUTPUT);
  	digitalWrite(sigPin, LOW);
  	delayMicroseconds(2);
  	digitalWrite(sigPin, HIGH);
  	delayMicroseconds(5);
  	digitalWrite(sigPin, LOW);
  
  	pinMode(sigPin, INPUT);
  	return pulseIn(sigPin, HIGH)/58;
}

void printHeaderLCD(const String& text){
 	dirtyLCD=true;
	lcd.clear();
  	lcd.print(text);
}
void printLineLCD(const String& text){
  	dirtyLCD=true;
	lcd.setCursor(0,1);
  	lcd.print(text);
}
void printLinesLCD(const String& head,const String& line){
	printHeaderLCD(head);
  	printLineLCD(line);
}
void printIdleLCD(){
  	if(dirtyLCD){
		printLinesLCD("PUT HANDS UNDER","  TAP TO START");
      	dirtyLCD = false;
  	}
}
void completeToFalse(){
  	Serial.println("toFalse");
	wash1Complete = false;
	rubComplete = false;
    wash2Complete = false;
}
void reset(){
  if(perfAction){
    	completeToFalse();
  		printHeaderLCD("     WASH!!");
	 	startingTime=millis();
    	digitalWrite(actionPin, HIGH);
  }
}