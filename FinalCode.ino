#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include "Arduino_AVRSTL.h"
//#include "DFPlayerMini_Fast.h"
#include <SPI.h> 
#include <RFID.h>
#include <Servo.h> 



//DFPLAYER
SoftwareSerial mySoftwareSerial(3,2); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
//DFPlayerMini_Fast myDFPlayer;
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



//constants that store the number of songs in each folder
int folderNumber[] = {4,2,60};

void detectButton(){
  //play pause button
  if (digitalRead(5) == HIGH && millis() - previousTime > 1000){
    previousTime = millis();
    Serial.println ("Play/Pause button pressed");
    
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
  if (digitalRead(6) == HIGH && millis() - previousTime > 1000) {
    previousTime = millis();
    Serial.println ("Skip Song button pressed");

    myDFPlayer.next();
    
  }
  if (digitalRead(7) == HIGH && millis() - previousTime > 500) {
    previousTime = millis();
    Serial.println ("Volume Up button pressed");

    myDFPlayer.volumeUp();
    Serial.println(myDFPlayer.readVolume());
  }
  if (digitalRead(8) == HIGH && millis() - previousTime > 500) {
    previousTime = millis();
    Serial.println ("Volume Down button pressed");

    myDFPlayer.volumeDown();
    Serial.println(myDFPlayer.readVolume());
    
  }
}
//returns random song in range of the number of tracks in the folder
void shuffleFolder(int folder){
  
  int maxVal = folderNumber[folder-1];
  int minVal = 1;

  std::vector<int> vec;
  for (int i=1; i <= maxVal; i++){
    vec.push_back(i);
  }
  
  if(vec.empty() == false && myDFPlayer.readState() == 0){
    
    int songNumber = random(minVal, vec.size()+1);

    int songName = vec[songNumber];
    Serial.println(songName);
    myDFPlayer.playFolder(folder, songName);
    vec.erase(songNumber-1);
    Serial.println("Next Song in the folder is playing on shuffle");
  }
}


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
  detectButton();
  
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
         
          //shuffleFolder(1);
          myDFPlayer.playLargeFolder(03, 10);
          //myDFPlayer.randomAll();
          //pastSong = 0001;
        }
        else if(temp == "880401435")
        {
//          myDFPlayer.play(0002);
//          pastSong = 0002;
            myDFPlayer.playLargeFolder(03, 1);
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
