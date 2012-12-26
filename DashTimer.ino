//Sample using LiquidCrystal library
#include <LiquidCrystal.h>

/*******************************************************

This program will test the LCD panel and the buttons
Mark Bramwell, July 2010

********************************************************/

// select the pins used on the LCD panel
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// define some values used by the panel and buttons
int lcd_key     = 0;
int adc_key_in  = 0;
const int startPin = 2;
const int finishPin = 3;

unsigned int timeElapsed = 0;
int startStop   = 0; //digital pin 2
int finish       = 1; //digital pin 3

volatile int startStopState = LOW;
volatile int finishState = LOW;

volatile boolean timing = false;
volatile boolean finished = false;

#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

// read the buttons
int read_LCD_buttons()
{
 adc_key_in = analogRead(0);      // read the value from the sensor 
 // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
 // we add approx 50 to those values and check to see if we are close
 if (adc_key_in > 1000) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
 if (adc_key_in < 50)   return btnRIGHT;  
 if (adc_key_in < 195)  return btnUP; 
 if (adc_key_in < 380)  return btnDOWN; 
 if (adc_key_in < 555)  return btnLEFT; 
 if (adc_key_in < 790)  return btnSELECT;   
 return btnNONE;  // when all others fail, return this...
}

void timerSetup()
{
cli();//stop interrupts

//set timer0 interrupt at 1kHz (this is the actual timer clock)
  TCCR0A = 0;// set entire TCCR0A register to 0
  TCCR0B = 0;// same for TCCR0B
  TCNT0  = 0;//initialize counter value to 0
  // set compare match register for 2khz increments
  //OCR0A = 124;// = (16*10^6) / (2000*64) - 1 (must be <256)
  // set compare match reg for 1Khz increments
  OCR0A = 249;// = (16*10^6) / (1000*64) - 1 (must be <256)
  
  // turn on CTC mode
  TCCR0A |= (1 << WGM01);
  // Set CS11 and CS10 bits for 64 prescaler
  TCCR0B |= (1 << CS11) | (1 << CS10);   
  // enable timer compare interrupt (LATER)
  //TIMSK0 |= (1 << OCIE0A);


//set timer1 interrupt at 5Hz (this is to update display and other housekeeping)
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0;
  // set timer count for 10hz increments
  OCR1A = 49999;// = (16*10^6) / (5*64) - 1
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS11 bit for 8 prescaler
  TCCR1B |= (1 << CS11);   
  // enable timer compare interrupt 
  TIMSK1 |= (1 << OCIE1A);

sei();//allow interrupts
  
}

void buttonSetup()
{
   pinMode(startPin, INPUT);
   pinMode(finishPin, INPUT);
  pinMode(A2, INPUT);
  
  attachInterrupt(startStop, startISR, FALLING);
  attachInterrupt(finish, finishISR, RISING);
}

void setup()
{
 lcd.begin(16, 2);              // start the library
 
 initMessage();
 buttonSetup();
 timerSetup();

 //digitalWrite(1, HIGH);
}

void initMessage()
{
 lcd.setCursor(0,0);
 lcd.print("Welcome to Dash Timer"); // print a simple message
}

ISR(TIMER0_COMPA_vect){//timer1 interrupt 1KHz
//this is the actual internal running millisecond stopwatch
  if (timing)
  {
    timeElapsed++;
  }
}
 
ISR(TIMER1_COMPA_vect){//timer1 interrupt 5Hz
//update the display every 5 seconds with changing info
  lcd.setCursor(10,1);
  lcd.print("----");
  lcd.setCursor(10,1);
 lcd.print(timeElapsed);
 lcd.setCursor(1,1);
 if (startStopState == HIGH)
 {
   lcd.print("ON      ");
 }
 else
 {
   lcd.print("OFF     ");
 }
 
 if (finishState == LOW)
 {
   lcd.setCursor(1,1);
   lcd.print("FINISH!");
 }
}

void enableStopWatch(boolean on)
{
  cli();
  if (on)
  {
    // enable timer compare interrupt
    TCNT0  = 0; //reset the counter
    TIMSK0 |= (1 << OCIE0A);
  }
  else
  {
    // disable timer compare interrupt
    TIMSK0 = 0;
  }
  sei();
}

void startISR()
{
  
    startStopState = digitalRead(startPin);
    if (startStopState == LOW)
    {
      startStopFunction();
    }
  
}

void finishISR()
{
  cli();
  finishState = digitalRead(finishPin);
  //check finished value so timer can't get accidentally restarted at finish line
  if (finishState == HIGH)
  {
    if (timing)
    {
      startStopFunction();
      finished = true;
    }
  }
  sei();
}

void startStopFunction()
{
  timing =~ timing;
  enableStopWatch(timing);
}
 
 
void loop()
{
 //nothing to do here
 lcd_key = read_LCD_buttons();  // read the buttons
 
 switch (lcd_key)               // depending on which button was pushed, we perform an action
 {
   case btnRIGHT:
     {
     if (!timing)
     {
       timeElapsed = 0;
     }
     break;
     }
   case btnLEFT:
     {
     startStopFunction();
     break;
     }
  }
}

