/*
FFT for 14 x 10 neopixels snaked up and down
 code adapted from Andy Doro FFT for LoL shield code (http://andydoro.com)
 adafruit recomends using 3.3v rail for mic 
 */

#include <fix_fft.h>    //library for fft 
#include <Adafruit_NeoPixel.h>
#define AUDIOPIN 5      //define mic to A5
#define N_PIXELS  140  // Number of pixels in strand
#define PIN 6
Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_PIXELS, PIN, NEO_GRB + NEO_KHZ800);

int audioPin = A0;
char im[128], data[128], data_avgs[14]; //FFt variables
int i, j;
int jcol[14][10] = { //Keep track of snaking neopixels up and down by reversing every other column.
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
  //Serial.begin(9600); // hardware serial
  strip.begin();
}


void loop() {
  int sensorValue = analogRead(A1); //read pot 1
  int sensorValue2 = analogRead(A2); //read pot 2
  int sensitivity = map(sensorValue, 0, 1023, 5, 100); //map pot 1 to sensitivity between 5 and 100
  int brightness = map(sensorValue2, 0, 1023, 0, 100); //map pot 2 to brightness from 0 to 100
  // Build table of audio samples
  for (i = 0; i < 128; i++) {
    data[i] = analogRead(audioPin);
    im[i] = 0;
  }

  fix_fft(data, im, 7, 0);  // FFT processing

  for (i = 0; i < 64; i++)
    data[i] = sqrt(data[i] * data[i] + im[i] * im[i]); // Make values positive

  for (i = 0; i < 14; i++) {
    // Average values
    data_avgs[i] = (data[i*4] + data[i*4 + 1] + data[i*4 + 2] + data[i*4 + 3]);   // average together
    if (i == 0)
      data_avgs[i] >>= 1;  // KK: De-emphasize first audio band (too sensitive)
    data_avgs[i] = map(data_avgs[i], 0, sensitivity, 0, 10);
  }
    for(int x = 0; x < 14; x++) //rows
    {
      for(int y = 0; y < 10; y++) //cols
      {
        //int z = x * 2;
        if(y < data_avgs[13-x])
        {          
          if(y < 3) //green
          {
            strip.setPixelColor(jcol[x][y], 0, 255 * brightness / 100, 0);
            //strip.setPixelColor(jcol[z-1][y], 0, 255 * brightness / 100, 0);
          }
          else if(y < 5) //lavender
          {
            strip.setPixelColor(jcol[x][y], 132 * brightness / 100, 20 * brightness / 100, 120 * brightness / 100);
            //strip.setPixelColor(jcol[z-1][y], 255 * brightness / 100, 230 * brightness / 100, 250 * brightness / 100);            
          }
          else if(y < 8) //pink
          {
            strip.setPixelColor(jcol[x][y], 255 * brightness / 100, 200 * brightness / 100, 230 * brightness / 100);
            //strip.setPixelColor(jcol[z-1][y], 255 * brightness / 100, 105 * brightness / 100, 180 * brightness / 100);            
          }
          else //red
          {
            strip.setPixelColor(jcol[x][y], 255 * brightness / 100, 0, 0);
            //strip.setPixelColor(jcol[z-1][y], 255 * brightness / 100, 0, 0);            
          }
        }
        else
        {
          strip.setPixelColor(jcol[x][y], 0, 0, 0);
          //strip.setPixelColor(jcol[z-1][y], 0, 0, 0);          
        }
      }
    }
    
    strip.show();
    //delay(20);
}
