#include <StopWatch.h>
#include <LiquidCrystal.h>

/*******************************************************

This program will test the LCD panel and the buttons
Mark Bramwell, July 2010

********************************************************/

// select the pins used on the LCD panel
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

//StopWatch
StopWatch sw;

// define some values used by the panel and buttons
int lcd_key     = 0;
int adc_key_in  = 0;
const int startPin = 2;
const int finishPin = 3;

int startStop   = 0; //digital pin 2
int finish       = 1; //digital pin 3

volatile int startStopState = LOW;
volatile int finishState = LOW;


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

void buttonSetup()
{
   pinMode(startPin, INPUT);
   pinMode(finishPin, INPUT);
  
  attachInterrupt(startStop, startISR, FALLING);
  attachInterrupt(finish, finishISR, RISING);
}

void setup()
{
  Serial.begin(9600);
 lcd.begin(16, 2);              // start the library
 
 initMessage();
 buttonSetup();

 //digitalWrite(1, HIGH);
}

void initMessage()
{
 lcd.setCursor(0,0);
 lcd.print("Welcome to Dash Timer"); // print a simple message
}


void updateDisplay()
{
lcd.setCursor(10,1);
lcd.print("----");
lcd.setCursor(10,1);
 lcd.print(sw.elapsed());
 lcd.setCursor(1,1);
 
 switch (sw.state())
 {
   case StopWatch::RUNNING:
   {
     lcd.print("TIMING ");
     break;
   }
   case StopWatch::STOPPED:
   {
     lcd.print("STOPPED");
     break;
   }
   case StopWatch::RESET:
   {
     lcd.print("RESET  ");
     break;
   }
 }
   
}


void startISR()
{
    startStopState = digitalRead(startPin);
    if (startStopState == LOW)
    {
      sw.start();
    }
}

void finishISR()
{
  finishState = digitalRead(finishPin);
  //check finished value so timer can't get accidentally restarted at finish line
  if (finishState == HIGH)
  {
    sw.stop();
  }
}

 
void loop()
{
  Serial.print("Timer is  ");
  if (sw.isRunning())
  {
    Serial.println(" ON");
  }
  else
  {
    Serial.println(" OFF");
  }
  
  updateDisplay();
  delay(250);
 //nothing to do here
 lcd_key = read_LCD_buttons();  // read the buttons
 
 switch (lcd_key)               // depending on which button was pushed, we perform an action
 {
   case btnRIGHT:
     {
       Serial.println("RESET");
       sw.reset();
       
     break;
     }
   case btnLEFT:
     {
     sw.stop();
     break;
     }
     case btnSELECT:
     {
       sw.start();
       break;
     }
  }
  
}

