#include <SoftwareSerial.h>
#include "DFRobotDFPlayerMini.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

SoftwareSerial mySoftwareSerial(10, 11);
DFRobotDFPlayerMini DFPlayer;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

int fileCount = 0;
int selected = 0;
int playing = -1;
int last = LOW;
bool pause = false;
int start = 0;

int leftPin = 4;
int rightPin = 3;
int playPin = 2;
int busyPin = 5;

void setup() {
  Serial.begin(9600);

  pinMode(leftPin, INPUT);
  pinMode(rightPin, INPUT);
  pinMode(playPin, INPUT);
  pinMode(busyPin, INPUT);

  mySoftwareSerial.begin(9600);
  if (!DFPlayer.begin(mySoftwareSerial, false)) {
    Serial.println("Failure :(");
  }
  delay(2000);

  DFPlayer.volume(20);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  delay(1000);

  display.clearDisplay();
  display.setTextColor(WHITE, BLACK);
  display.setTextSize(1);

  fileCount = DFPlayer.readFileCountsInFolder(1);
}

void loop() {
  if (digitalRead(leftPin) == HIGH || digitalRead(rightPin) == HIGH) {
    if (last == LOW) {
      if (digitalRead(leftPin) == HIGH) {
        selected--;
      } else {
        selected++;
      }
    }

    last = HIGH;
  } else {
    last = LOW;
  }

  if (selected < 0) {
    selected = 0;
  } else if (selected > fileCount - 1) {
    selected = fileCount - 1;
  }

  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextColor(WHITE, BLACK);
  display.setTextSize(2);
  if(pause){
    display.println("Paused");
  } else if(digitalRead(busyPin)==LOW){
    display.println("Playing");
  } else{
    display.println("Play");
  }
  
  /*
  display.setCursor(116, 0);
  display.setTextSize(1);
  display.println(fileCount);
  */

  if(selected>start+3){
    start++;
  }
  if(selected<start+1){
    start--;
  }
  if(start<0){
    start=0;
  }
  display.setCursor(0, 16);
  for (int i = start; i < fileCount; i++) {
    if (playing == i) {
      display.setTextColor(BLACK, WHITE);
    } else {
      display.setTextColor(WHITE, BLACK);
    }
    if (selected == i) {
      display.setTextSize(2);
    } else {
      display.setTextSize(1);
    }
    display.print("Song ");
    display.println(i + 1);
  }

  display.display();

  if (digitalRead(playPin) == HIGH) {
    if (selected == playing) {
      if (digitalRead(busyPin) == LOW) {
        pause = true;
        DFPlayer.pause();
        while (digitalRead(playPin) == HIGH) {}
      } else if (pause) {
        pause = false;
        DFPlayer.start();
        delay(200);
        while (digitalRead(playPin) == HIGH) {}
      }
    } else {
      DFPlayer.playFolder(1, selected + 1);
      playing = selected;
      pause=false;
      delay(200);
      while (digitalRead(playPin) == HIGH) {}
    }
  }
  if (digitalRead(busyPin) == HIGH && !pause) {
    playing = -1;
  }
}
