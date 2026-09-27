#include <ezButton.h>

#define STRIKE_INIT_PIN 5
#define PLAYER_BUTTON1_PIN 0
#define PLAYER_BUTTON2_PIN 1
#define PLAYER_BUTTON3_PIN 2
#define PLAYER_BUTTON4_PIN 3

const int stripLights[4] = {4, 9, 13, 17};

// Define player lights pins (3 strikes per player)
const int PLAYER_LIGHTS[4][3] = {
  {6, 7, 8},     // playerButton1
  {10, 11, 12},  // playerButton2
  {14, 15, 16},  //playerButton3
  {18, 19, 20}   // playerButton4
};

ezButton strikeInit(STRIKE_INIT_PIN);
ezButton playerButtons[4] = {ezButton(PLAYER_BUTTON1_PIN), ezButton(PLAYER_BUTTON2_PIN), ezButton(PLAYER_BUTTON3_PIN), ezButton(PLAYER_BUTTON4_PIN)};

bool gameActive = false;
int lightsOn[4] = {0, 0, 0, 0};
int ledState = LOW;
unsigned long previousMillis = 0;
unsigned long currentMillis;
const long interval = 100;


void setup() {
  Serial.begin(9600);
  strikeInit.setDebounceTime(50);

  for (int i = 0; i < 4; i++) {
    playerButtons[i].setDebounceTime(50);
  }


  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 3; j++) {
      pinMode(PLAYER_LIGHTS[i][j], OUTPUT);
      digitalWrite(PLAYER_LIGHTS[i][j], LOW);
    }
    pinMode(stripLights[i], OUTPUT);
    digitalWrite(stripLights[i], HIGH);
  }
}

void loop() {
  strikeInit.loop();

  for (int i = 0; i < 4; i++) {
    playerButtons[i].loop();
  }
 
  // strikeInit button press
  if (strikeInit.isPressed()) {
    if (gameActive){
      for (int i = 0; i < 4; i++) {

          if (lightsOn[i] < 3) {
            digitalWrite(stripLights[i], HIGH);
          }else{
            digitalWrite(stripLights[i], LOW);
          }  
      }
    }
    gameActive = !gameActive;
    Serial.print("Strike incoming... ");
  }


  if (gameActive) {
    currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      if (ledState == LOW) {
        ledState = HIGH;
      } else {
        ledState = LOW;
      }
      previousMillis = currentMillis;

      for (int i = 0; i < 4; i++) {
        if (lightsOn[i] < 3) {


          digitalWrite(stripLights[i], ledState);
        }
      }
    }    
    for (int i = 0; i < 4; i++) {
      if (playerButtons[i].isPressed() && lightsOn[i] < 3) {
        for (int j = 0; j < 4; j++) {

          if (lightsOn[j] < 3) {
            digitalWrite(stripLights[j], HIGH);
          }else{
            digitalWrite(stripLights[j], LOW);
          }  
        }   
        digitalWrite(PLAYER_LIGHTS[i][lightsOn[i]], HIGH);
        lightsOn[i]++;
        Serial.print("Player ");
        Serial.print(i + 1);
        Serial.print(" has ");
        Serial.print(lightsOn[i]);
        Serial.println(" strikes");

        // Check if player is dead
        if (lightsOn[i] == 3) {
          Serial.print("Player ");
          Serial.print(i + 1);
          Serial.println(" is a bad judge!");
          digitalWrite(stripLights[i], LOW);
        }

        gameActive = false;
        break;
      }
    }
  }
}