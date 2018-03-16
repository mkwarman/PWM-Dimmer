#include <avr/io.h>
#include <avr/interrupt.h>

// Pin identification
const int LIGHT_DIMMER_PWM=5;
const int MAT_DIMMER_PWM=6;
const int ZERO_CROSS_SIGNAL=2;

// For serial read target value detection
const char LIGHT_DUTY_CYCLE_IDENTIFIER='L';
const char MAT_DUTY_CYCLE_IDENTIFIER='M';
const char ZERO_CROSS_DELAY_IDENTIFIER='Z';

// Compensate for early Zero Cross detection
int zero_cross_delay = 450;

// Zero Cross Detection
void zeroCrossInterrupt()
{
  // Compensate for early zero cross detection
  delayMicroseconds(zero_cross_delay);

  // turn on TRIAC gates
  digitalWrite(LIGHT_DIMMER_PWM, HIGH);
  digitalWrite(MAT_DIMMER_PWM, HIGH);

  // Start timer with divide by 256 input
  TCCR1B=0x04;

  // Reset timer - count from zero
  TCNT1 = 0;
}

void setup()
{
  // Initialize zero cross signal detection
  pinMode(ZERO_CROSS_SIGNAL, INPUT_PULLUP);

  // Initialze PWM signal output
  pinMode(LIGHT_DIMMER_PWM, OUTPUT);
  pinMode(MAT_DIMMER_PWM, OUTPUT);

  // Initialze serial channel
  Serial.begin(9600);
  
  // --- Timer Setup ---
  // Initialize the comparators
  OCR1A = 50;
  OCR1B = 50;

  // Enable comparator A and B and overflow interrupts
  TIMSK1 |= (1 << OCIE1A);
  TIMSK1 |= (1 << OCIE1B);

  // Timer control registers set for normal operation, timer disabled
  TCCR1A = 0x00;
  TCCR1B = 0x00;
  
  // Attach interrupt to zero cross detection pin, target zeroCrossInterrupt()
  attachInterrupt(digitalPinToInterrupt(ZERO_CROSS_SIGNAL), zeroCrossInterrupt, RISING);
}

// Comparator match A - light dimmer
ISR(TIMER1_COMPA_vect){
  // Set TRIAC gate to low
   digitalWrite(LIGHT_DIMMER_PWM, LOW);
}

// Comparator match B - mat dimmer
ISR(TIMER1_COMPB_vect){
  // Set TRIAC gate to low
  digitalWrite(MAT_DIMMER_PWM, LOW); 
}

// Read three digit serial signal, return parsed int
int read_three_digits(){
  // Get 100s place
  int dc100 = (Serial.read() - '0') * 100;

  // Get 10s place
  int dc10 = (Serial.read() - '0') * 10;

  // Get 1s place
  int dc1 = Serial.read() - '0';

  // Add respective multiples and return
  return (dc100 + dc10 + dc1);
}


void loop(){
  // If four digits of serial available (ex. A400)
  if(Serial.available() >= 4){
    // Get the first character
    char identifier = Serial.read();

    // Read serial data and save value to respective variable
    if(identifier == LIGHT_DUTY_CYCLE_IDENTIFIER){
      OCR1A = read_three_digits();
    }
    else if(identifier == MAT_DUTY_CYCLE_IDENTIFIER){
      OCR1B = read_three_digits();
    }
    else if(identifier == ZERO_CROSS_DELAY_IDENTIFIER){
      zero_cross_delay = read_three_digits();
    }
  }
}
