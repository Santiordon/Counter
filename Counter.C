int latchPin = 4;           // Latch pin of 74HC595 is connected to Digital pin 5
int clockPin = 5;           // Clock pin of 74HC595 is connected to Digital pin 6
int dataPin = 3;            // Data pin of 74HC595 is connected to Digital pin 4

int buttonPin = 1;
int buzzerPin = 0;

int tensLatchPin = 10;
int tensClockPin = 11;
int tensDataPin = 9;

int ninePin = 6;
int tenPin = 7;

int secondNinePin = 12;
int secondTenPin = 13;

int outputEnablePin = 2;    // OE pin of 74HC595 is connected to PWM pin 3
int tensOutputEnablePin = 8;

byte leds = 0;              // Variable to hold the pattern of which LEDs are currently turned on or off
byte tensLeds = 0;

int buttonState = 0;
int fastButtonState = 0;
int maxDisplayState = 3;

int displayState = 0;

int singleInt = 0;
int doubleInt = 0;

int timeReduction = 1000; // 1000 is standard.

bool sonicSpeed = false;

long lastChange = millis();

/*
 * setup() - this function runs once when you turn your Arduino on
 * We initialize the serial connection with the computer
 */
void setup() 
{
  Serial.begin(9600);
  // Set all the pins of 74HC595 as OUTPUT
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);  
  pinMode(clockPin, OUTPUT);
  pinMode(ninePin, OUTPUT);
  pinMode(tenPin, OUTPUT);
  pinMode(outputEnablePin, OUTPUT);

  pinMode(buttonPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(A5, INPUT);
  
  pinMode(tensOutputEnablePin, OUTPUT);
  pinMode(tensLatchPin, OUTPUT);
  pinMode(tensClockPin, OUTPUT);  
  pinMode(tensDataPin, OUTPUT);
}

/*
 * loop() - this function runs over and over again
 */

int getDigit(int number, int place) {
    int digit = 0;
    int divisor = 1;

    for (int i = 0; i < place; i++) {
        divisor *= 10;
    }

    digit = (number / divisor) % 10;

    return digit;
}

int getMinute(int time) {
  int minutes = time / 60;
  singleInt = getDigit(minutes, 0);
  doubleInt = getDigit(minutes, 1);
}

int getHour(int time) {
  int hours = (time / 60) / 60;
  singleInt = getDigit(hours, 0);
  doubleInt = getDigit(hours, 1);
}

int getDay(int time) {
  int days = ((time / 60) / 60) / 24;
  singleInt = getDigit(days, 0);
  doubleInt = getDigit(days, 1);
}

void loop()
{
  int total = (millis() / timeReduction);
  buttonState = digitalRead(A0);
  fastButtonState = digitalRead(A5);

  if(buttonState == LOW && millis() - lastChange >= 1000){
    displayState++;
    if(displayState > maxDisplayState) {
      displayState = 0;
    }
    lastChange = millis();
    tone(A1, 20, 50);
  }

  if(fastButtonState == LOW && millis() - lastChange >= 1000){
    if(sonicSpeed){
      timeReduction = 1000;
      sonicSpeed = false;
    } else {
      timeReduction = 10;
      sonicSpeed = true;
    }
    lastChange = millis();
    tone(A1, 20, 50);
  }

  int notSeconds = (millis() / timeReduction * 10);
  doubleInt = getDigit(total, 1) % 6;
  singleInt = getDigit(total, 0);

  digitalWrite(A2, LOW);
  digitalWrite(A3, LOW);
  digitalWrite(A4, LOW);

  if(displayState == 1){
    getMinute(total);
    digitalWrite(A2, HIGH);
  } else if(displayState == 2){
    getHour(total);
    digitalWrite(A3, HIGH);
  } else if(displayState == 3){
    getDay(total);
    digitalWrite(A4, HIGH);
  }
  tensLeds = 0;
  leds = 0;

  updateTensShiftRegister();
  updateShiftRegister();

  if(singleInt != 0){
    bitSet(leds, singleInt - 1);
  }

  if(doubleInt != 0){
    bitSet(tensLeds, doubleInt - 1);
  }

  updateTensShiftRegister();
  updateShiftRegister();

  digitalWrite(ninePin, LOW);
  digitalWrite(tenPin, LOW);
  digitalWrite(secondNinePin, LOW);
  digitalWrite(secondTenPin, LOW);

  if(singleInt == 9){
    digitalWrite(ninePin, HIGH);
  } else if(singleInt == 0) {
    digitalWrite(tenPin, HIGH);
  }

  if(doubleInt == 9){
    digitalWrite(secondNinePin, HIGH);
  } else if(doubleInt == 0){
    digitalWrite(secondTenPin, HIGH);
  }
  

  delay(5);
}

/*
 * updateShiftRegister() - This function sets the latchPin to low, then calls the Arduino function 'shiftOut' to shift out contents of variable 'leds' in the shift register before putting the 'latchPin' high again.
 */
void updateShiftRegister()
{
   digitalWrite(latchPin, LOW);
   shiftOut(dataPin, clockPin, LSBFIRST, leds);
   digitalWrite(latchPin, HIGH);
}

void updateTensShiftRegister(){
  digitalWrite(tensLatchPin, LOW);
  shiftOut(tensDataPin, tensClockPin, LSBFIRST, tensLeds);
  digitalWrite(tensLatchPin, HIGH);
}

/*
 * setBrightness() - Generates PWM signal and writes it to OE pin.
 */

void fadeIn(){
  for(byte i = 0; i < 256; i++){
    setBrightness(i);
    delay(1);
  }
}

void fadeOut(){
  for(byte i = 255; i > 0; i--){
    setBrightness(i);
    delay(1);
  }
}
void setBrightness(byte brightness) // 0 to 255
{
  analogWrite(outputEnablePin, 255-brightness);
}