const int buttonPin = 2;

const int redLedPin = 9;
const int greenLedPin = 10;
const int blueLedPin = 11;

const int lightSensorPin = A0;

int moodLevel = 50;
const int maxMoodLevel = 100;
const int minMoodLevel = 0;

const int moodIncrease = 10;

const unsigned long inactivityDelay = 3000;
const unsigned long moodDecreaseInterval = 700;

const int darknessThreshold = 700;

unsigned long lastTouchTime = 0;
unsigned long lastMoodDecreaseTime = 0;

int lastButtonReading = HIGH;
int stableButtonReading = HIGH;

unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 40;

unsigned long buttonPressStartTime = 0;
const unsigned long longPressDelay = 600;

bool isSleeping = false;
bool isLongPress = false;


// Sets the RGB LED color (0-255).

void setColor(int red, int green, int blue) {
  analogWrite(redLedPin, 255 - red);
  analogWrite(greenLedPin, 255 - green);
  analogWrite(blueLedPin, 255 - blue);
}

void turnOffLED() {
  setColor(0, 0, 0);
}


// Displays the current mood using LED colors.

void showMood() {
  if (isLongPress) {
    setColor(204, 160, 0);
    return;
  }

  if (isSleeping) {
    turnOffLED();
    return;
  }

  if (moodLevel <= 30) {
    setColor(255, 0, 0);
  } else if (moodLevel <= 65) {
    setColor(0, 0, 255);
  } else {
    setColor(0, 255, 0);
  }
}


// Initializes pins, timers, and serial communication.

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);

  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(blueLedPin, OUTPUT);

  turnOffLED();

  unsigned long currentTime = millis();

  lastTouchTime = currentTime;
  lastMoodDecreaseTime = currentTime;

  showMood();

  Serial.begin(9600);
}


// Main loop

void loop() {
  unsigned long currentTime = millis();

  int lightLevel = analogRead(lightSensorPin);

  Serial.print("Light level: ");
  Serial.print(lightLevel);
  Serial.print(" | Mood: ");
  Serial.println(moodLevel);


  // Determines sleep based on light.

  if (lightLevel >= darknessThreshold) {
    isSleeping = true;
  } else {
    isSleeping = false;
  }


  Serial.print(" | Sleeping: ");
  Serial.print(isSleeping);
  Serial.print(" | Mood: ");
  Serial.println(moodLevel);


  // Reads and debounces the button.

  int currentButtonReading = digitalRead(buttonPin);

  if (currentButtonReading != lastButtonReading) {
    lastDebounceTime = currentTime;
  }

  if (currentTime - lastDebounceTime >= debounceDelay) {
    if (currentButtonReading != stableButtonReading) {
      stableButtonReading = currentButtonReading;

      if (stableButtonReading == LOW) {
        moodLevel = moodLevel + moodIncrease;

        if (moodLevel > maxMoodLevel) {
          moodLevel = maxMoodLevel;
        }

        lastTouchTime = currentTime;
        lastMoodDecreaseTime = currentTime;
        buttonPressStartTime = currentTime;
      }
    }
  }

  lastButtonReading = currentButtonReading;


  // Activates mustard yellow while the button is held.

  isLongPress = (
    stableButtonReading == LOW &&
    currentTime - buttonPressStartTime >= longPressDelay
  );


  // Reduces mood when the toy is not touched.

  if (currentTime - lastTouchTime >= inactivityDelay) {
    if (currentTime - lastMoodDecreaseTime >= moodDecreaseInterval) {
      if (moodLevel > minMoodLevel) {
        moodLevel = moodLevel - 1;
      }

      lastMoodDecreaseTime = currentTime;
    }
  }


  // Updates the LED to display the current state.

  showMood();
}