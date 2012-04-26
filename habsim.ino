// GPS Generator and Emulator
// Based on the UKHAS GPSgen Code: http://ukhas.org.uk/code:emulator
// Ported to arduino by Jim Blackhurst


// todo
// 1. Documentation, Comments, format
// 2. remove unused DEFINEs and Vars
// 3. implement Cutdown
// DONE 4. review descent code to use pressure func
// DONE 5. accurate timing 
// 6. simple button control for SIM rate
// 7. bug with rendering '10' m/s descent rate
// 8. rescale glyph graph on descent
// 9. tune max altitude
// 10. fix wind variability


#include <PString.h>
#include <Time.h> 
#include <LiquidCrystal.h>
#include <math.h>

// radians to degrees
#define DEGREES(x) ((x) * 57.295779513082320877)
// degrees to radians
#define RADIANS(x) ((x) / 57.295779513082320877)

#define LOG_BASE 1.4142135623730950488
#define LOG_POWER 5300.0

//#define PI 3.141592653589793

#define WIND_TURBULENCE 10 // chance of the wind changing the bearing by up to 1 degree per cycle of the simulation

//#define LAUNCH_LAT 52.213389
//#define LAUNCH_LON 0.096834
#define LAUNCH_LAT 52.1658
#define LAUNCH_LON -1.4250
#define LAUNCH_ALT 203

#define TERMINAL_VELOCITY 40 // meters per second
#define ASCENT_RATE 5 // meters per second
#define DESCENT_RATE 5 // meters per second

//should take 20secs to do 100m
//currently takes 50sec to do 100m

//http://www.engineeringtoolbox.com/air-altitude-pressure-d_462.html
//http://en.wikipedia.org/wiki/Atmospheric_pressure
#define LAUNCH_KPA 100
#define BURST_KPA 0.02
#define GPS_HZ 1

// SIM_HZ - the internal speed of the simulation
// if you are going to change this then you will need to scale simAccel acordingly
// Faster updates result in smaller distances per step being fed into the horizontal speed 
// Equation.  Due to the ATMEGA's 8bit arcitecture and 4 byte floats, it looses precision
// and Lon stops updating. 20Hz is the reccomneded value.
#define SIM_HZ 20


// DEBUG status - output either the intended NMEA sentances, or debug data
// 1 = debug, 0 = NMEA
#define DEBUG 1

// Define the pins used by the Liquid Crystal Display
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// Status hold the current state of the sim.
// 0 = Ascent, 1 = Descent, 2 = Landed
int Status =0;

// simAccel - an artifical acceleration factor for speeds.
// This value can be used to maintain a acurate simulation of both the vertical and 
// horizontal speeds by compensating for the time it takes to process the calculations
// It can also be used to run the simulation at a fast-forward speed 
// With the sim running at 20Hz, a simAccel value of 1.4 will give accurate speeds 
float simAccel = 1.4;

time_t Now;
char buf[128];

float CurLon,CurLat,CurAlt,CurKPA,CurSpeed; 
unsigned long update_counter;
unsigned long output_counter;
float ascent_rate_mod = 0;
float tropo_wind_rate_mod = 0;
float strat_wind_rate_mod = 0;
//float lat_wind = 0;
//float lon_wind = 0;
float windBearing = 0.0;

float windSpeed = 10.0;

float distancePerStep = 0.0;
float Drag;


long speedTest =0;
float msTest =0;
float speedTestResult = 0;


byte balloon[8] = {B01110, B11111, B11111, B11111, B01110, B00100, B01110, B01110};
byte chute[8] =   {B11111, B11111, B10001, B10001, B01010, B00100, B01110, B01110};
byte land[8] =    {B00000, B00000, B00000, B11111, B10101, B00100, B01110, B01110};
byte scale_0[8] = {B00100, B00000, B00100, B00000, B00100, B00000, B00100, B00000};
byte scale_1[8] = {B00100, B00000, B00100, B00000, B00100, B00000, B11111, B00000};
byte scale_2[8] = {B00100, B00000, B00100, B00000, B11111, B00000, B00100, B00000};
byte scale_3[8] = {B00100, B00000, B11111, B00000, B00100, B00000, B00100, B00000};
byte scale_4[8] = {B11111, B00000, B00100, B00000, B00100, B00000, B00100, B00000};


void setup()
{
 // open the serial port at 9600 bps:
 Serial.begin(115200); 

 // set up the LCD's number of columns and rows: 
 lcd.begin(16, 2);
 lcd.createChar(0, balloon);
 lcd.createChar(1, chute);
 lcd.createChar(2, land);
 lcd.createChar(3, scale_0);
 lcd.createChar(4, scale_1);
 lcd.createChar(5, scale_2);
 lcd.createChar(6, scale_3);
 lcd.createChar(7, scale_4);

long tempBits = 0;                               // create a long of random bits to use as seed
 for (int i=1; i<=32 ; i++)
 {                    
   tempBits = ( tempBits | ( analogRead( 0 ) & 1 ) ) << 1;
 }
 randomSeed(tempBits);                           // seed

 windBearing = random(359);

  
 setTime(14,14,14,1,3,2012); //Hardcode the datetime, this is effectivly the 'launch' time
 Now = now(); //assign the current time to the variable 'Now'
 
 // get first waypoint co-ordinate as launch position
 CurLon = LAUNCH_LON;
 CurLat = LAUNCH_LAT; 
 CurAlt = LAUNCH_ALT;
 CurKPA = LAUNCH_KPA;

 update_counter =  millis();
 output_counter =  millis();

 lcd.clear();

 speedTest = millis();
 msTest = CurAlt;
}


void loop()
{
  Now = now();
  float windOffset = random(10);
  //if (random(WIND_TURBULENCE)==1) windBearing += (windOffset-5)/10;
  if (windBearing>359) windBearing = 0;
  if (windBearing<0) windBearing = 359;
  windBearing+=0.1;
  //windBearing=0;
  
  LCDFlight();
    
  if (Status != 2)
  {
    if (millis() > (update_counter + (1000/SIM_HZ)))
    {
      update_alt();
      
      
      distancePerStep = (windSpeed/SIM_HZ)*simAccel;
      
      updateWindWalk(CurLat, CurLon, windBearing, distancePerStep); 
  
      if(DEBUG) Serial.print("[t");
      if(DEBUG) Serial.print(millis()-update_counter);
      if(DEBUG) Serial.print(" :VsA ");
      if(DEBUG) Serial.print(speedTestResult);
      if(DEBUG) Serial.print("]   ");
      
      update_counter =  millis();
      
      if(DEBUG) Serial.println();
    }

    if (millis() > (output_counter + (1000/GPS_HZ)))
    {
      if (!DEBUG) Output_NEMA(Now,CurLat,CurLon,CurAlt,windBearing,CurSpeed);
      output_counter =  millis();
    }
  }




  if (millis()>=(speedTest+10000))
  {
    speedTestResult = (CurAlt-msTest)/10;
    msTest=CurAlt;   
    speedTest = millis();
  }
}