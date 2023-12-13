//Include relevant libraries
#include <U8glib.h>
#include <Wire.h>
#include <Stepper.h>

int motorUppNedPin1 = 9;   // Blue - 28BYJ48 pin 1
int motorUppNedPin2 = 10;  // Pink - 28BYJ48 pin 2
int motorUppNedPin3 = 11;  // Yellow - 28BYJ48 pin 3
int motorUppNedPin4 = 12;  // Orange - 28BYJ48 pin 4

// Variables for the which buttons and how many steps correspond to different floors
int floorButtons[] = {7, 4, 2};  // Floor buttons representing different floors
int floorSteps[] = {0, 1408, 2816};  // Number of steps to reach each floor

// Variables saving the current step at which the elevator is located
int stepCount = 0; // Adds a step everytime the loop runs
int stepCountSaved = 0; // Last saved stepCount to inform the elevator of where it needs to travel

// Variables to save and check if the elevator needs to stop at a specific floor after traveling to its last one
int buttonRequest = 5; // Random base value 5 since 0, 1, 2 are values its supposed to check for
int buttonRequestSaved = 5; // Saved value if another button is requested during the loop for buttonRequest

// Saved value if a button is pressed during elevator movment and its deemed more benefitial to stop here first
int checker = 0;

int motorSpeed = 1200;  //variable to set stepper speed
int lookup[8] = { B01000, B01100, B00100, B00110, B00010, B00011, B00001, B01001 };

U8GLIB_SSD1306_128X64 oled(U8G_I2C_OPT_NO_ACK);
void setup() {
  // Setup pins
  pinMode(2, INPUT);
  pinMode(4, INPUT);
  pinMode(7, INPUT);
  pinMode(motorUppNedPin1, OUTPUT);
  pinMode(motorUppNedPin2, OUTPUT);
  pinMode(motorUppNedPin3, OUTPUT);
  pinMode(motorUppNedPin4, OUTPUT);

  oled.setFont(u8g_font_helvB10);

  Serial.begin(9600);

  // Start the screen at its base floor
  updateOled("floor:", "1");
}

void loop() {
  // Loop that runs through each button to check if pressed
  for (int j = 0; j <= 2; j++) {
    if (digitalRead(floorButtons[j]) == 1) {
      // Check if the elevator needs to move up
      if (floorSteps[j] > stepCountSaved) {
        // Calculate how many steps the elevator has to move
      for (int i = 0; i < floorSteps[j] - stepCountSaved; i++) {  //Flytta servot 90 grader
        uppat();
        stepCount++;
        // Check if the elevator is at a new floor and update the screen accordingly
        if (stepCount == 0) {
          updateOled("Floor:", "1");
        }else if (stepCount== 1408) {
          updateOled("Floor:", "2");
        }else if(stepCount == 2816) {
          updateOled("Floor:", "3");
        }
        // Delay at the first step to ensure the user releases the button
        if (i <= 1) {
          delay(500);
        }
        // Checks if another button has been pressed and the elevator has to stop
        if (checker > 0) {
          if (stepCount == floorSteps[checker]) {
            delay(5000);
            checker = 0;
          }
        }else {
          // Checks if a button is pressed during elevator movment
          for (int u = 0; u <= 2; u++) {
           if (digitalRead(floorButtons[u]) == 1) {
            // Checks if the requested floor is on the path of the elevator
             if (stepCount <= floorSteps[u] && floorSteps[u]<floorSteps[j]){
              // Checks to make sure the elevator isnt at the requested floor, if it is it stops there
               if (stepCount == floorSteps) {
                 delay(5000);
                 checker = 0;
               }else{
                 checker = u; // Set the checker to tell the code to stop when it reaches the requested floor
               }
              }else {
                buttonRequest = u; // If the requested floor has been passed save the floor for later use
              }
            }
          }
        }
    }
    stepCountSaved = stepCount; // Save stepcount so the next requested floor moves to the corresponding step
  }else{
      // If the requested floor is not up do the exact same thing as the same loop but instead in the opposite direction
        for (int i = 0; i < (stepCountSaved - floorSteps[j]); i++) { //Flytta servot 90 grader
        nerat();
        stepCount--;

        if (stepCount == 0) {
          updateOled("Floor:", "1");
        }else if (stepCount== 1408) {
          updateOled("Floor:", "2");
        }else if(stepCount == 2816) {
          updateOled("Floor:", "3");
        }

        if (i <= 1) {
          delay(500);
        }
        if (checker > 0) {
          if (stepCount == floorSteps[checker]) {
            delay(5000);
          }
        }else {
          for (int u = 0; u <= 2; u++) {
           if (digitalRead(floorButtons[u]) == 1) {
             if (stepCount > floorSteps[u]  && floorSteps[u]>floorSteps[j]){
               if (stepCount == floorSteps) {
                 delay(5000);
               }else{
                 checker = u; 
               }
              }else {
                buttonRequest = u;
              }
            }
          }
        }
      }
      stepCountSaved = stepCount; 
  }
  }
}
// After the elevator has moved to the corresponding floor check if a buttonRequest has been made, if so move to the corresponding floor
// with the same code 
while (buttonRequest < 5) {
  buttonRequestSaved = buttonRequest; // Save buttonRequest incase buttonRequest is changed during this loop
  buttonRequest = 5; // Change buttonRequest to its original value
  if (floorSteps[buttonRequestSaved] > stepCountSaved) {
    for (int i = 0; i < floorSteps[buttonRequestSaved] - stepCountSaved; i++) {  //Flytta servot 90 grader
      uppat();
      stepCount++;
      if (stepCount == 0) {
        updateOled("Floor:", "1");
      }else if (stepCount== 1408) {
        updateOled("Floor:", "2");
      }else if(stepCount == 2816) {
        updateOled("Floor:", "3");
      }
      if (i <= 1) {
         delay(500);
      }
        if (checker > 0) {
          if (stepCount == floorSteps[checker]) {
            delay(5000);
          }
        }else {
          for (int u = 0; u <= 2; u++) {
           if (digitalRead(floorButtons[u]) == 1) {
             if (stepCount < floorSteps[u] && floorSteps[u]<floorSteps[buttonRequestSaved]){
               if (stepCount == floorSteps) {
                 delay(5000);
               }else{
                 checker = u; 
               }
              }else {
                buttonRequest = u;
              }
            }
          }
        }
    }
    stepCountSaved = stepCount; 
  }else{
        for (int i = 0; i < (stepCountSaved - floorSteps[buttonRequestSaved]); i++) { //Flytta servot 90 grader
        nerat();
        stepCount--;
        if (stepCount == 0) {
          updateOled("Floor:", "1");
        }else if (stepCount== 1408) {
          updateOled("Floor:", "2");
        }else if(stepCount == 2816) {
          updateOled("Floor:", "3");
        }
        if (i <= 1) {
          delay(500);
        }
        if (checker > 0) {
          if (stepCount == floorSteps[checker]) {
            delay(5000);
          }
        }else {
          for (int u = 0; u <= 2; u++) {
           if (digitalRead(floorButtons[checker]) == 1) {
             if (stepCount > floorSteps[u] && floorSteps[u]>floorSteps[buttonRequestSaved]){
               if (stepCount == floorSteps) {
                 delay(5000);
               }else{
                 checker = u; 
               }
              }else {
                buttonRequest = u;
              }
            }
          }
        }
      }
      stepCountSaved = stepCount; 
  }
}
}

// Function to update OLED-Screen
void updateOled(String text, String floor) {

  oled.firstPage();
  do {

    oled.drawStr(30, 31, text.c_str());
    oled.drawStr(30, 44, floor.c_str());
  } while (oled.nextPage());
}

// Function to move elevator down
void nerat(){
  for (int i = 0; i < 8; i++)
  {
    setOutputUppNed(i);
    delayMicroseconds(motorSpeed);
  }
}

// Function to move elevator up
void uppat(){
  for (int i = 7; i >= 0; i--)
  {
    setOutputUppNed(i);
    delayMicroseconds(motorSpeed);
  }
}
void setOutputUppNed(int out) {
  digitalWrite(motorUppNedPin1, bitRead(lookup[out], 0));
  digitalWrite(motorUppNedPin2, bitRead(lookup[out], 1));
  digitalWrite(motorUppNedPin3, bitRead(lookup[out], 2));
  digitalWrite(motorUppNedPin4, bitRead(lookup[out], 3));
}
