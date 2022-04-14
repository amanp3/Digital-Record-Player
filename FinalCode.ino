#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include <SPI.h> 
#include <RFID.h>
#include <Servo.h> 


//DFPLAYER
SoftwareSerial mySoftwareSerial(3,2); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

//Timer
unsigned long previousTime;

//RFID
RFID rfid(10, 9);       //D10:pin of tag reader SDA. D9:pin of tag reader RST 
unsigned char status; 
unsigned char str[MAX_LEN]; //MAX_LEN is 16: size of the array 

int redLEDPin = A0;
//int greenLEDPin = 6;

String oldCard = "x";

int pastSong = 0000;





void setup() {
  pinMode(5, INPUT); //for play pause button

  
  mySoftwareSerial.begin(9600);
  Serial.begin(9600);

  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true);
  }
  Serial.println(F("DFPlayer Mini online."));

  myDFPlayer.volume(10);  //Set volume value. From 0 to 30
  
  

  ///rfid
  //Serial.begin(9600);     //Serial monitor is only required to get tag ID numbers and for troubleshooting
  SPI.begin();            //Start SPI communication with reader
  rfid.init();            //initialization 
  pinMode(redLEDPin, OUTPUT);     //LED startup sequence
  //pinMode(greenLEDPin, OUTPUT);
//  digitalWrite(redLEDPin, HIGH);
//  delay(200);
//  //digitalWrite(greenLEDPin, HIGH);
//  delay(200);
//  digitalWrite(redLEDPin, LOW);
//  delay(200);
  //digitalWrite(greenLEDPin, LOW);
  
  Serial.println("Place card/tag near reader...");

  
}

void loop() {
  String temp = "";  //variable to store the read RFID number
  //Serial.println (myDFPlayer.readState());
  if (myDFPlayer.readState() == 1){
    digitalWrite(redLEDPin, HIGH);
  }
  else {
    digitalWrite(redLEDPin, LOW);
  }

  //button stuff
  if (digitalRead(5) == HIGH && millis() - previousTime > 1000){
    previousTime = millis();
    Serial.println ("Button pressed");
    
          if (myDFPlayer.readState()==1){
            myDFPlayer.pause();
            Serial.println("Paused");
          }

          else if (myDFPlayer.readState() == 2){
            myDFPlayer.start();
            Serial.println("Playing");
          }
          else if (myDFPlayer.readState() == 0 && pastSong != 0000){
            myDFPlayer.play(pastSong);
          }
  }
  
  if (rfid.findCard(PICC_REQIDL, str) == MI_OK)   //Wait for a tag to be placed near the reader
  { 
    
    Serial.println("Card found"); 
                               
    if (rfid.anticoll(str) == MI_OK)              //Anti-collision detection, read tag serial number 
    { 
      Serial.print("The card's ID number is : "); 
      for (int i = 0; i < 4; i++)                 //Record and display the tag serial number 
      { 
        temp = temp + (0x0F & (str[i] >> 4)); 
        temp = temp + (0x0F & str[i]); 
      } 
      Serial.println (temp);
      
        

      if (temp != oldCard){
        //this part plays song based on tag
        
        if (temp == "880401335")
        {
          myDFPlayer.play(0001);
          pastSong = 0001;
        }
        else if(temp == "880401435")
        {
          myDFPlayer.play(0002);
          pastSong = 0002;
        }
        else if(temp == "88041513815")
        {
          myDFPlayer.play(0003);
          pastSong = 0003;
        }
        else if(temp == "88042590")
        {
          myDFPlayer.play(0004);
          pastSong = 0004;
        }


        
      }
    }
    
    
    
    rfid.selectTag(str); //Lock card to prevent a redundant read, removing the line will make the sketch read cards continually
    
  }
  rfid.halt();
  
  oldCard = temp;
//  delay(100);
//  
//  if (myDFPlayer.readState()== 512){
//    isPlaying = false;
//    Serial.println("===== isPlaying has been set to false");
//  }

}

void printDetail(uint8_t type, int value){
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}
