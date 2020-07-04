// CONFIGURATION
//////////////////////////////////////////////////////////////////////////////////
//You can add a different tubes name here and then add it's factor in the setup
enum tube {SBM20, SI29BG, SBM19, STS5, SI22G, SI3BG, SBM21, LND712, SBT9, SI1G};

int installed_tube = tube(SBM20); // Change with the Tube used in your Project
//////////////////////////////////////////////////////////////////////////////////

/* By youtube.com/xcibe95x */
/* github.com/xcibe95x */

/* PINS SETUP
 *  
 * Voltmeter - A1
 * USB Voltmeter - A6
 * SDA - A4
 * SCL - A5
 * PWM - D9
 * BUZZER - D7
 * 
*/

//Libraries
#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define XPOS 0 // LCD X Position
#define YPOS 1 // LCD Y Position
#define GEIGER 3 // Geiger-Muller Tube Pulse input

// LCD Offsets, Remove or Replace with your LCD offsets/code aswell as libraries n stuff
LiquidCrystal_I2C lcd(0x27, 16, 2);

unsigned long previousMillis = 0;
unsigned long previousMillis1 = 0;

const int led =  2;
const int buzzer =  7;

int impulse = 0;
int measuringUnit = 0;
int buttonState = 0;
float cps = 0.0; // Clicks Per Second
unsigned long int cpm = 0; // Click Per Minute
float conversionFactor = 0.0;
float microSievert = 0.0;
float milliRoentgen = 0.0;


/////////////////////////////////


void setup() {
  
 SPI.begin();
 Serial.begin(9600); // Open Serial Port for Debug or External Tools
 
 // Initialize LCD
 lcd.init();
 lcd.clear();
 lcd.backlight();

 // LCD Prints
 lcd.setCursor(0,0);  
 lcd.print("CHB95");
 lcd.setCursor(11,0);  
 lcd.print("Rev1");
 lcd.setCursor(0,1);  
 lcd.print("Geiger Counter");
 delay(1500);
 lcd.clear();

 // Pin Setups
  pinMode(buzzer, OUTPUT);
  pinMode(led, OUTPUT);

//////////////////////////////////////////////////////////////////////////////////////////////////////

 // TUBE CONFIGURATION
 
 // Calculate or find the factor value and add it here if your tube is different.
 // Doing your own calculations and calibrations may make your geiger counter more accurate
 // I will only correct values for SBM-20 and SI3BG, feel free to push values for other tubes.
 switch (installed_tube) {
    case 0:   conversionFactor = 0.006315; break; //SBM20
    case 1:  conversionFactor = 0.010000; break; //SI29BG
    case 2:   conversionFactor = 0.001500; break; //SBM19
    case 3:    conversionFactor = 0.006666; break; //STS5
    case 4:   conversionFactor = 0.001714; break; //SI22G
    case 5: conversionFactor = 0.194; break; //SI3BG Use this for now as a factor, i will update after i compare with SBM-20.
    //case 5: conversionFactor = 0.044444; break; //SI3BG Attempt to correct it too low uSv/hr?
    //case 5:   conversionFactor = 0.631578; break; //SI3BG that one give crazy uSv/hr
    case 6:   conversionFactor = 0.048000; break; //SBM21
    case 7:  conversionFactor = 0.005940; break; //LND712
    case 8:    conversionFactor = 0.010900; break; //SBT9
    case 9:    conversionFactor = 0.006000; break; //SI1G
    default: break;
 }

 attachInterrupt(digitalPinToInterrupt(GEIGER), triggerGeiger, FALLING);

/////////////////////////////////////////////////////////////////////////////////////////////////////

 // PWM = 10% Output(9) Stuff
  digitalWrite(13, 1); // PWM
  TCCR1A = TCCR1A & 0xe0 | 2;
  TCCR1B = TCCR1B & 0xe0 | 0x09; 
  analogWrite(9,22);
  
  // More LCD Prints
  lcd.setCursor(0,0);  
  lcd.print("Software Updates:");
  lcd.setCursor(0,1);  
  lcd.print("github/xcibe95x");
  delay(1000);
  lcd.clear();
  
}

void loop() {

 // Keep track of Arduino internal timer
 unsigned long currentMillis = millis();
 unsigned long currentMillis1 = millis();

 // Conversions
 milliRoentgen = microSievert/10; // uSv/hr -> mR/hr (10uSv/hr == 1mR/hr)

if (impulse == 1) {
    digitalWrite(led, 1);
    delay(1);
    impulse = 0;
} else {
        digitalWrite(led, 0);
}
 
 // Each Second Print & Reset the CPS 
  if (currentMillis - previousMillis >= 1000) {
    previousMillis = currentMillis;
    cpm = cps*60.0;
    microSievert = cpm*conversionFactor;

   // Print CPM to Serial for External Tools
   Serial.print("CPM:");
   Serial.println(cpm);
    
 if (measuringUnit == 0) {
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print(cpm);
   lcd.print(" CPM");
   lcd.setCursor(8,0);
   lcd.print(cps);
   lcd.print(" CPS");
   lcd.setCursor(0,1);
   lcd.print(microSievert, 2);
   lcd.print(" uSv/hr");
 } else {
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print(cpm);
   lcd.print(" CPM");
   lcd.setCursor(0,1);
   lcd.print(milliRoentgen);
   lcd.print(" mR/hr");
 }
  cps = 0;  
}
    
/////////////////////////////////////////////////INDICATORS////////////////////////////////////////////

  batterylevel(15,0);
  usbplug(14,0);
  
///////////////////////////////////////////////////////////////////////////////////////////////////////


  // Push Buttons Functions
  if (digitalRead(12) == 1) {
    if (measuringUnit == 0) {
      measuringUnit = 1;
      lcd.clear();
      delay(50);
    } else { 
      measuringUnit = 0;
      lcd.clear();
      delay(50);
    }}


  
}

 void triggerGeiger() {
   if(previousMillis1 != millis()){
        cps++;
        digitalWrite(buzzer, 1);
        impulse=1;
        delay(1);
        digitalWrite(buzzer, 0);
    previousMillis1 = millis();
   }
 }

// Draw battery level in position x,y
void batterylevel(int xpos,int ypos)
{
   double analog_value = analogRead(A1);

  //read the voltage and convert it to volt
   float curvolt = ( analog_value * 5.0) / 1024;
  // check if voltge is bigger than 4.2 volt so this is a power source
  if(curvolt > 4.0)
  {
    byte batlevel[8] = {
    B01110,
    B11111,
    B11111,
    B10101,
    B10001,
    B11011,
    B11011,
    B11111,
    };
    lcd.createChar(0 , batlevel);
    lcd.setCursor(xpos,ypos);
    lcd.write(byte(0));
  }
  if(curvolt <= 4.0 && curvolt > 3.8)
  {
    byte batlevel[8] = {
    B01110,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    };
    lcd.createChar(0 , batlevel);
    lcd.setCursor(xpos,ypos);
    lcd.write(byte(0));
  }
  if(curvolt <= 3.8 && curvolt > 3.7)
  {
    byte batlevel[8] = {
    B01110,
    B10001,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    };
    lcd.createChar(0 , batlevel);
    lcd.setCursor(xpos,ypos);
    lcd.write(byte(0));
  }
  if(curvolt <= 3.7 && curvolt > 3.6)
  {
    byte batlevel[8] = {
    B01110,
    B10001,
    B10001,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    };
    lcd.createChar(0 , batlevel);
    lcd.setCursor(xpos,ypos);
    lcd.write(byte(0));
  }
  if(curvolt <= 3.6 && curvolt > 3.4)
  {
    byte batlevel[8] = {
    B01110,
    B10001,
    B10001,
    B10001,
    B11111,
    B11111,
    B11111,
    B11111,
    };
    lcd.createChar(0 , batlevel);
    lcd.setCursor(xpos,ypos);
    lcd.write(byte(0));
  }
  if(curvolt <= 3.4 && curvolt > 3.2)
  {
    byte batlevel[8] = {
    B01110,
    B10001,
    B10001,
    B10001,
    B10001,
    B11111,
    B11111,
    B11111,
    };
    lcd.createChar(0 , batlevel);
    lcd.setCursor(xpos,ypos);
    lcd.write(byte(0));
  }
  if(curvolt <= 3.2 && curvolt > 3.0)
  {
    byte batlevel[8] = {
    B01110,
    B10001,
    B10001,
    B10001,
    B10001,
    B10001,
    B11111,
    B11111,
    };
    lcd.createChar(0 , batlevel);
    lcd.setCursor(xpos,ypos);
    lcd.write(byte(0));
  }
  if(curvolt < 3.0)
  {
    byte batlevel[8] = {
    B01110,
    B10001,
    B10001,
    B10001,
    B10001,
    B10001,
    B10001,
    B11111,
    };
    lcd.createChar(0 , batlevel);
    lcd.setCursor(xpos,ypos);
    lcd.write(byte(0));
  }
}

// Draw USB icon in position x,y
void usbplug(int xpos,int ypos)
{
  
   double USBvalue = analogRead(A6);
   float USB = ( USBvalue * 5.0) / 1024;


// lcd.setCursor(10,1);
// lcd.print(USB);
  
  if(USB > 4.0)
    {
    byte usbicon[8] = {
  B00100,
  B00101,
  B10101,
  B10111,
  B11100,
  B00100,
  B01110,
  B00100
    };
    lcd.createChar(1 , usbicon);
    lcd.setCursor(xpos,ypos);
    lcd.write(byte(1));
  }
   if(USB < 4.0)
  {
    byte usbicon[8] = {
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    };
    lcd.createChar(1 , usbicon);
    lcd.setCursor(xpos,ypos);
    lcd.write(byte(1));
  }
}
                        
