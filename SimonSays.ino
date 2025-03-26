//*****************************************************************************
// SimonSays Project for UCIEEE workshop
//
// DISCLAIMER: THIS PROGRAM SHOULD NOT BE USED AS AN EXAMPLE OF BEST PROGRAMMING
//             PRACTICES.
//
//             Ideally an Arduino program like this would use tasks and modules
//             which are both covered later in ENCE260. However, this code was 
//             made to be as similar to what you might've made in COSC131 as 
//             possible for your understanding.
//
//             Because this program does not use interupts or time triggered tasks,
//             the game occasionally does not behave as expected. If this happens,
//             just press the reset button on your Arduino. 
//*****************************************************************************

// Pin definitions
#define GREEN_LED A0
#define RED_LED   8
#define YELLOW_LED A3
#define BLUE_LED  5
#define GREEN_BUT  A1
#define RED_BUT 7
#define YELLOW_BUT A2
#define BLUE_BUT 6
#define BUZZER  9

// LEDs, Buttons, and Notes
int LEDS[] =  {GREEN_LED, RED_LED, YELLOW_LED, BLUE_LED};
int BUTTONS[] = {GREEN_BUT, RED_BUT, YELLOW_BUT, BLUE_BUT};
int NOTES[] = {262, 330, 523, 392}; // GREEN_NOTE, RED_NOTE, YELLOW_NOTE, BLUE_NOTE (given in Hz)
#define WRONG_TONE 233  // Tone that plays when the wrong button is pressed (given in Hz)

// Game parameters 
#define MAX_LEVEL 100
#define NUM_BUTTONS 4 
int LEVEL = 1;
int GAMESPEED = 500; // This is the duration which the notes are played for in the game sequence
                    // The smaller this number is, the harder the game is to play! 
#define DELAY 250 // This is the time between the blinking LEDs that play when the user gets the whole
                 // sequence right or when then lose the game.  

// Arrays to store the correct sequence, sound, and input sequence the user enters.
int sequence[MAX_LEVEL];
int sound[MAX_LEVEL];
int inputSequence[MAX_LEVEL];


//*****************************************************************************
// Gameplay functions         
//*****************************************************************************
// Function to start game when any button is pressed.
// This loops over and checks all the buttons in the BUTTONS array to see if any have been pressed.
bool startGame() {
  for (int i = 0; i < NUM_BUTTONS; i++) { 
    if (digitalRead(BUTTONS[i]) == HIGH) { 
      return true; 
    }
  }
  return false;
}

// Function to generate the game sequence
// This generates a random sequence of LEDs that is MAX_LEVEL long. Each coloured LED in the sequence is 
// represented in the sequence[i] array by either 0, 1, 2, 3. These numbers are used to index into the NOTES[]
// array to assign the notes to the LEDs. 
void generateSequence() {
  for (int i = 0; i < MAX_LEVEL; i++) {
    sequence[i] = random(0, NUM_BUTTONS); 
    sound[i] = NOTES[sequence[i]];
  }
}

// Function to display LED and sound sequence
// This function shows the player a portion of the sequence that matches what level they are on. This level number, i, 
// is used to index into the sequence[] and sound[] arrays to show the player each of the leds and corresponding sounds
// in the correct order. 
void showSequence() {
  ledsOff();
  for (int i = 0; i < LEVEL; i++) {
    digitalWrite(LEDS[sequence[i]], HIGH);
    tone(BUZZER, sound[i]);
    delay(GAMESPEED);
    digitalWrite(LEDS[sequence[i]], LOW);
    noTone(BUZZER);
    delay(GAMESPEED);
  }
}

// Function to check if the user has entered the right buttons
// This function uses the inputSequence[] array to store each of the users button presses. After every
// button push, this function checks if the button push was correct. 
void getSequence() {
  for (int i = 0; i < LEVEL; i++) {
    bool buttonPushed = false;
    while (!buttonPushed) {
      int button = handleButtonPress(); //button is an integer corresponding to the button that was pushed. 
      if (button != -1) {
        inputSequence[i] = button;
        buttonPushed = true;
        // Check if button push was incorrect. Exit the function if the button push was incorrect. 
        if (inputSequence[i] != sequence[i]) {
          wrongSequence();
          restartGame();
          return;
        }
      }
    }
  }
  rightSequence(); 
}


//*****************************************************************************
// Helper functions        
//*****************************************************************************
// Function to restart the game
void restartGame(){
  LEVEL = 1;
}

// Function to turn all LEDs off
void ledsOff() {
  for (int i = 0; i < NUM_BUTTONS; i++) {
    digitalWrite(LEDS[i], LOW);
  }
}

// Function to turn all LEDs on
void ledsOn() {
  for (int i = 0; i < NUM_BUTTONS; i++) {
    digitalWrite(LEDS[i], HIGH);
  }
}

// Read and handle button press
// This function loops through all buttons and checks if they have been pushed. It then 
// lights up the corresponding LED and sounds the note while the button is pushed. 
int handleButtonPress() {
  // Check all buttons for button presses
  for (int j = 0; j < NUM_BUTTONS; j++) {
    if (digitalRead(BUTTONS[j]) == HIGH) {
      // If a button is pressed, turn on the corresponding LED and play the note while the button is pressed
      while(digitalRead(BUTTONS[j]) == HIGH){
        digitalWrite(LEDS[j], HIGH);
        tone(BUZZER, NOTES[j]);
      }
      // Turn off the LED and note when button is released
      noTone(BUZZER);
      digitalWrite(LEDS[j], LOW);
      return j; // return index of button pushed
    }
  }
  return -1; // return -1 if no button is pressed
}

// Function to confirm correct sequence and increase game level 
void rightSequence() {
  // Blink all LEDs once to show the player that they have entered the right sequence
  ledsOff();
  delay(DELAY);
  ledsOn();
  delay(DELAY);
  ledsOff();
  delay(DELAY);

  // Increase the level the user is on
  if (LEVEL < MAX_LEVEL) {
    LEVEL++;
  }
  else if (LEVEL == MAX_LEVEL) {
    restartGame(); // Restart the game 
  }
}

// Function to show player that they have lost the game by buzzing an error tone and blinking all
// LEDs 3 times. 
void wrongSequence() {
  for (int i = 0; i < 3; i++) {
    tone(BUZZER, WRONG_TONE);
    ledsOn();
    delay(DELAY);
    noTone(BUZZER);
    ledsOff();
    delay(DELAY);
  }
}


//*****************************************************************************
// Setup function     
//*****************************************************************************
void setup() {
  Serial.begin(9600);
  for (int i = 0; i < NUM_BUTTONS; i++) {
    pinMode(LEDS[i], OUTPUT); // Initialises all the LEDs as outputs 
    pinMode(BUTTONS[i], INPUT); // Initialises all the buttons as inputs 
  }
  // Initialize the Buzzer as an output
  pinMode(BUZZER, OUTPUT);
}

//*****************************************************************************
// Main loop        
//*****************************************************************************
// This is the 'main' function. This is the game play loop which will keep running. 
void loop() {
  if (LEVEL == 1) {
    generateSequence();
  }
  if (startGame() || LEVEL != 1) {
    showSequence(); // This function displays the sequence for the user to enter
    getSequence(); // This function gets the sequence the user enters and checks if each button press is right. 
  }
}