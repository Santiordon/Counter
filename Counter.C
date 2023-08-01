int latchPin = 4;           // Latch pin of 74HC595 is connected to Digital pin 5
int clockPin = 5;           // Clock pin of 74HC595 is connected to Digital pin 6
int dataPin = 3;            // Data pin of 74HC595 is connected to Digital pin 4

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

    // Extract the digit
    digit = (number / divisor) % 10;

    return digit;
}

void loop()
{
  int total = (millis() / 1000);
  int notSeconds = (millis() / 10000);
  int tensDigit = getDigit(total, 1);
  int onesDigit = getDigit(total, 0); 

  tensLeds = 0;
  leds = 0;

  updateTensShiftRegister();
  updateShiftRegister();

  if(onesDigit != 0){
    bitSet(leds, onesDigit - 1);
  }

  if(tensDigit != 0){
    bitSet(tensLeds, tensDigit - 1);
  }

  updateTensShiftRegister();
  updateShiftRegister();

  digitalWrite(ninePin, LOW);
  digitalWrite(tenPin, LOW);
  digitalWrite(secondNinePin, LOW);
  digitalWrite(secondTenPin, LOW);

  if(onesDigit == 9){
    digitalWrite(ninePin, HIGH);
  } else if(onesDigit == 0) {
    digitalWrite(tenPin, HIGH);
  }

  if(tensDigit == 9){
    digitalWrite(secondNinePin, HIGH);
  } else if(tensDigit == 0){
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