/*
FFT for 8x8 max7219 with adafruit electretmicrophone module 
 code adapted from Andy Doro FFT for LoL shield code (http://andydoro.com)
 adafruit recomends using 3.3v rail for mic 
 
 
 */

//#include <LedControl.h> //library fot the 8x8 matrix
#include <fix_fft.h>    //library for fft 
#include <Adafruit_NeoPixel.h>
#define AUDIOPIN 5      //define mic to A5
#define N_PIXELS  140  // Number of pixels in strand
#define N_STRIPS  14
#define PIN 6
Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_PIXELS, PIN, NEO_GRB + NEO_KHZ800);

/*
 Now we need a LedControl to work with.
 ***** These pin numbers will probably not work with your hardware *****
 pin 11 is connected to the DataIn 
 pin 13 is connected to the CLK 
 pin 10 is connected to LOAD 
 We have only a single MAX72XX.
 */
//LedControl lc=LedControl(11,13,10,1); //DIN, CLK, LD pins and number of displays 1

/* we always wait a bit between updates of the display */
unsigned long delaytime=20;

int audioPin = A0;
char im[128], data[128], data_avgs[10]; //FFt variables
int i, j;
int maxExpectedAudio = 15; //25
int jcol[14][10] = {
{0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
{19, 18, 17, 16, 15, 14, 13, 12, 11, 10},
{20, 21, 22, 23, 24, 25, 26, 27, 28, 29},
{39, 38, 37, 36, 35, 34, 33, 32, 31, 30},
{40, 41, 42, 43, 44, 45, 46, 47, 48, 49},
{59, 58, 57, 56, 55, 54, 53, 52, 51, 50},
{60, 61, 62, 63, 64, 65, 66, 67, 68, 69},
{79, 78, 77, 76, 75, 74, 73, 72, 71, 70},
{80, 81, 82, 83, 84, 85, 86, 87, 88, 89},
{99, 98, 97, 96, 95, 94, 93, 92, 91, 90},
{100, 101, 102, 103, 104, 105, 106, 107, 108, 109},
{119, 118, 117, 116, 115, 114, 113, 112, 111, 110},
{120, 121, 122, 123, 124, 125, 126, 127, 128, 129},
{139, 138, 137, 136, 135, 134, 133, 132, 131, 130}
};

void setup(){
  /*
   The MAX72XX is in power-saving mode on startup,
   we have to do a wakeup call
   */
  //lc.shutdown(0,false);
  //lc.setIntensity(0,8); //set the brightness 0 darkest 15 brightest
  //lc.clearDisplay(0); //clear the display ready to begin 
  Serial.begin(9600); // hardware serial
  strip.begin();
}


void loop() {
  int sensorValue = analogRead(A1);
  int sensorValue2 = analogRead(A2);
  int sensitivity = map(sensorValue, 0, 1023, 5, 100);
  int brightness = map(sensorValue2, 0, 1023, 0, 100);
  Serial.print(sensitivity);
  // Build table of audio samples
  for (i = 0; i < 128; i++) {
    data[i] = analogRead(audioPin);
    im[i] = 0;
  }

  fix_fft(data, im, 7, 0);  // FFT processing

  for (i = 0; i < 64; i++)
    data[i] = sqrt(data[i] * data[i] + im[i] * im[i]); // Make values positive

  for (i = 0; i < 10; i++) {
    // Average values
    j = i << 3; //5?
    data_avgs[i] = data[j] + data[j + 1] + data[j + 2] + data[j + 3]
      + data[j + 4] + data[j + 5] + data[j + 6] + data[j + 7] + data[j + 8] + data[j + 9] ;
    if (i == 0);
    data_avgs[i] >>= 1;  // KK: De-emphasize first audio band (too sensitive)
    data_avgs[i] = map(data_avgs[i], 0, sensitivity, 0, 9); // Map for output to 8x8 display
  }
    for(int i = 0; i < 10; i++) //rows
    {
      for(int e = 0; e < 7; e++) //cols
      {
        int z = e * 2;
        if(i <= data_avgs[e] && data_avgs[e] < 10)
        {

          //lc.setLed(0, 7-e, 7-i, true);
          //Serial.print(i);
          //Serial.println(data_avgs[e]);
          
          if(i < 3) //green
          {
            strip.setPixelColor(jcol[14-z][i-10], 0, 255 * brightness / 100, 0);
            strip.setPixelColor(jcol[13-z][i-10], 0, 255 * brightness / 100, 0);
          }
          else if(i < 5) //lavender
          {
            strip.setPixelColor(jcol[14-z][i-10], 255 * brightness / 100, 230 * brightness / 100, 250 * brightness / 100);
            strip.setPixelColor(jcol[13-z][i-10], 255 * brightness / 100, 230 * brightness / 100, 250 * brightness / 100);            
          }
          else if(i < 8) //pink
          {
            strip.setPixelColor(jcol[14-z][i-10], 255 * brightness / 100, 105 * brightness / 100, 180 * brightness / 100);
            strip.setPixelColor(jcol[13-z][i-10], 255 * brightness / 100, 105 * brightness / 100, 180 * brightness / 100);            
          }
          else //red
          {
            strip.setPixelColor(jcol[14-z][i-10], 255 * brightness / 100, 0, 0);
            strip.setPixelColor(jcol[13-z][i-10], 255 * brightness / 100, 0, 0);            
          }
        }
        else
        {
          //lc.setLed(0, 7-e, 7-i, false);
          //Serial.print("-");
          //Serial.print(" ");
          strip.setPixelColor(jcol[14-z][i-10], 0, 0, 0);
          strip.setPixelColor(jcol[13-z][i-10], 0, 0, 0);          
        }
      }
    }
    
    strip.show();
  //Serial.println("");
  
}
