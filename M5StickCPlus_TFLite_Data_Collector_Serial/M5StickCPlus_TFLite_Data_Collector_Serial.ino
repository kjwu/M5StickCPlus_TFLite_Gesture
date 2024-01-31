/*
  IMU Capture

  This example uses the on-board IMU to start reading acceleration and gyroscope
  data from on-board IMU and prints it to the Serial Monitor for one second
  when the significant motion is detected.

  You can also use the Serial Plotter to graph the data.

  The circuit:
  - M5StickCPlus.

  Created by Don Coleman, Sandeep Mistry
  Modified by Dominic Pajak, Sandeep Mistry
  Modified for M5StickCPlus by Kuan-Ju Wu

  This example code is in the public domain.
*/

#include <M5StickCPlus.h>
#include "time.h"

float accX = 0.0F;
float accY = 0.0F;
float accZ = 0.0F;
float gyroX = 0.0F;
float gyroY = 0.0F;
float gyroZ = 0.0F;
const float accelerationThreshold = 2.5; // threshold of significant in G's
const int numSamples = 119;
int samplesRead = numSamples;

void setup() {

  Serial.begin(115200);

  //Comment the following if not using M5Stick, for other ESP32, will need to get different library for the IMU
  M5.begin();
  M5.Lcd.setRotation(3);
  M5.Lcd.fillScreen(BLACK);
  M5.IMU.Init();
  M5.Lcd.setTextSize(1);
  M5.Lcd.setCursor(1, 0, 2);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 1, 2);
}

void loop() {
  // put your main code here, to run repeatedly:
  M5.Lcd.setCursor(0, 50);

  // Check stillness of the IMU, only start collection if there is movement.
  while (samplesRead == numSamples) {
    // read the acceleration data
    M5.IMU.getAccelData(&accX, &accY, &accZ);

    // sum up the absolutes
    float aSum = fabs(accX) + fabs(accY) + fabs(accZ);

    // check if it's above the threshold
    if (aSum >= accelerationThreshold) {
      // reset the sample read count
      samplesRead = 0;
      break;
    }
  }

  //collect the acceleration data
  while (samplesRead < numSamples) {
    // check if both new acceleration and gyroscope data is
    // available
      // read the acceleration and gyroscope data
      M5.IMU.getAccelData(&accX, &accY, &accZ);
      M5.IMU.getGyroData(&gyroX,&gyroY,&gyroZ);

      samplesRead++;
      M5.Lcd.printf("%3.3f, %3.3f, %3.3f\n, %3.3f, %3.3f, %3.3f\n", accX, accY, accZ, gyroX, gyroY, gyroZ);


      // print the data in CSV format
      Serial.print(accX, 3);
      Serial.print(',');
      Serial.print(accY, 3);
      Serial.print(',');
      Serial.print(accZ, 3);
      Serial.print(',');
      Serial.print(gyroX, 3);
      Serial.print(',');
      Serial.print(gyroY, 3);
      Serial.print(',');
      Serial.print(gyroZ, 3);
      Serial.println();

      if (samplesRead == numSamples) {
        // add an empty line if it's the last sample
        Serial.println();
      }
    }

}
