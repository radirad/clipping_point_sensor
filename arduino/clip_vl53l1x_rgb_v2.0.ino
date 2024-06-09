#include <Wire.h>
#include <VL53L1X.h>
#include <Adafruit_NeoPixel.h>
#include "classes.h"

using namespace std;
#define XSHUT_pin1 13

#define LED_PIN 2
#define LEDS 8
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

ColourRGB CR[4]={ ColourRGB("Red",255,0,0),
                  ColourRGB("Yellow",253,166,0),
                  ColourRGB("Green",0,255,0),
                  ColourRGB("Blue",0,0,255)
                };

//Actual colour
int actColour=0;

// Counters //
int count=20,count_tmp=0,DIST;
unsigned long tmp_DIST,tmp_DIST_MAX=3000;

// Enable debugging mode //
int debug=1;

// Set range where sensor will operate //
int min_range=90;
int max_range=260;
int mid_range=(max_range-min_range)/4;

// Reset counters
int xreset=0; 
int xnotreset=0;

// Initialise sensor //
VL53L1X sensor;

void reset() { 
  sensor.stopContinuous();
  delay(10);
  asm volatile ("jmp 0"); 
}

void setup(){
  pixels.begin();
 
  while (!Serial) {}
  if(debug){ Serial.begin(115200); }
  pinMode(XSHUT_pin1, OUTPUT);
  Wire.begin();
  Wire.setClock(400000); // use 400 kHz I2C

  pinMode(XSHUT_pin1, INPUT);

  sensor.setTimeout(300);
  if (!sensor.init()){
    if(debug){ Serial.println("Failed to detect and initialize sensor!");}
    while (1);
  }

  // Use long distance mode and allow up to 50000 us (50 ms) for a measurement.
  // You can change these settings to adjust the performance of the sensor, but
  // the minimum timing budget is 20 ms for short distance mode and 33 ms for
  // medium and long distance modes. See the VL53L1X datasheet for more
  // information on range and timing limits.
  sensor.setDistanceMode(VL53L1X::Long);
  sensor.setMeasurementTimingBudget(30000);

  // Start continuous readings at a rate of one measurement every 50 ms (the
  // inter-measurement period). This period should be at least as long as the
  // timing budget.
  sensor.startContinuous(12);

}

void loop(){
	
	tmp_DIST = sensor.read();

// Ccheck for sensor not operating
if(tmp_DIST==-1){
  xreset++;
  xnotreset=1;
  if(xreset>20)reset();
}else if(xnotreset!=0){
  xnotreset++;
  if(xnotreset>100){
    xnotreset=0;
    xreset=0;
  }
}

// Check for no object in range //
	if(tmp_DIST>max_range){
		count_tmp++;
	}

	if(tmp_DIST<tmp_DIST_MAX){
		tmp_DIST_MAX=tmp_DIST;
		if ( tmp_DIST < min_range ) { DIST=1;}
		else if ( tmp_DIST < (min_range+mid_range)) { DIST=2;}
		else if ( tmp_DIST < (max_range-mid_range) ) { DIST=3;} 
		else if ( tmp_DIST < max_range ) { DIST=4;}
		else { DIST=0;}
			
		if(DIST<=4 && DIST>0){
      actColour=3;
      pixels.setPixelColor(0, CR[actColour].getRed(), CR[actColour].getGreen(), CR[actColour].getBlue());
      pixels.setPixelColor(1, CR[actColour].getRed(), CR[actColour].getGreen(), CR[actColour].getBlue());
      pixels.show();
		}
		if(DIST<=3 && DIST>0){
      actColour=2;
      pixels.setPixelColor(2, CR[actColour].getRed(), CR[actColour].getGreen(), CR[actColour].getBlue());
      pixels.setPixelColor(3, CR[actColour].getRed(), CR[actColour].getGreen(), CR[actColour].getBlue());
      pixels.show();
		}
		if(DIST<=2 && DIST>0){
      actColour=1;
      pixels.setPixelColor(4, CR[actColour].getRed(), CR[actColour].getGreen(), CR[actColour].getBlue());
      pixels.setPixelColor(5, CR[actColour].getRed(), CR[actColour].getGreen(), CR[actColour].getBlue());
      pixels.show();
  	}
		if(DIST==1){
      actColour=0;
      pixels.setPixelColor(6, CR[actColour].getRed(), CR[actColour].getGreen(), CR[actColour].getBlue());
      pixels.setPixelColor(7, CR[actColour].getRed(), CR[actColour].getGreen(), CR[actColour].getBlue());
      pixels.show();
		}				
	}

  if(debug){
    Serial.print(tmp_DIST);
    Serial.print(" - ");
    Serial.print(DIST);
    Serial.print(" - ");
    Serial.print(tmp_DIST_MAX);
    if (sensor.timeoutOccurred()) { Serial.print(" TIMEOUT"); }
    Serial.println();
  }

// No object in range, clear LEDs and counters //
	if(count_tmp>count){
		if(DIST>0){
			_delay_ms(500);
		}else{
			_delay_ms(50);
		}
		count_tmp=0;
		tmp_DIST_MAX=3000;
		DIST=0;
    pixels.clear();
    pixels.show();
	}	

}