/*
  IMU Classifier

  This example uses the on-board IMU to start reading acceleration and gyroscope
  data from on-board IMU, once enough samples are read, it then uses a
  TensorFlow Lite (Micro) model to try to classify the movement as a known gesture.

  Note: The direct use of C/C++ pointers, namespaces, and dynamic memory is generally
        discouraged in Arduino examples, and in the future the TensorFlowLite library
        might change to make the sketch simpler.

  The circuit:
  - M5StickCPlus

  Created by Don Coleman, Sandeep Mistry
  Modified by Dominic Pajak, Sandeep Mistry
  Adapted for M5StickCPlus by Kuan-Ju Wu

  This example code is in the public domain.
*/


#include <TensorFlowLite_ESP32.h>
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
// #include "tensorflow/lite/version.h"

#include "model.h"
#include <M5StickCPlus.h>


// global variables used for TensorFlow Lite (Micro)
tflite::ErrorReporter* error_reporter = nullptr;

// pull in all the TFLM ops, you can remove this line and
// only pull in the TFLM ops you need, if would like to reduce
// the compiled size of the sketch.
tflite::AllOpsResolver tflOpsResolver;

const tflite::Model* tflModel = nullptr;
tflite::MicroInterpreter* tflInterpreter = nullptr;
TfLiteTensor* tflInputTensor = nullptr;
TfLiteTensor* tflOutputTensor = nullptr;

// Create a static memory buffer for TFLM, the size may need to
// be adjusted based on the model you are using
constexpr int tensorArenaSize = 8 * 1024;
byte tensorArena[tensorArenaSize] __attribute__((aligned(16)));

// array to map gesture index to a name
const char* GESTURES[] = {
  "t",
  "d",
  "l",
  "r"
  // "tr",
  // "tl",
  // "br",
  // "bl"
};

#define NUM_GESTURES (sizeof(GESTURES) / sizeof(GESTURES[0]))

void setup() {

  static tflite::MicroErrorReporter micro_error_reporter;
  error_reporter = &micro_error_reporter;

  M5.begin();
  M5.Lcd.setRotation(3);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(3);  // Set font size.  设置字体大小

  M5.IMU.Init();

  Serial.begin(115200);

  // get the TFL representation of the model byte array
  tflModel = tflite::GetModel(model);
  if (tflModel->version() != TFLITE_SCHEMA_VERSION) {
    Serial.println("Model schema mismatch!");
    while (1);
  }

  // Create an interpreter to run the model
  tflInterpreter = new tflite::MicroInterpreter(tflModel, tflOpsResolver, tensorArena, tensorArenaSize, error_reporter);

  // Allocate memory for the model's input and output tensors
  tflInterpreter->AllocateTensors();

  // Get pointers for the model's input and output tensors
  tflInputTensor = tflInterpreter->input(0);
  tflOutputTensor = tflInterpreter->output(0);
}

void loop() {
  float aX, aY, aZ, gX, gY, gZ;
  // check if new acceleration AND gyroscope data is available
  // read the acceleration and gyroscope data
  M5.IMU.getAccelData(&aX, &aY, &aZ);
  M5.IMU.getGyroData(&gX,&gY,&gZ);


  // normalize the IMU data between 0 to 1 and store in the model's
  // input tensor
  tflInputTensor->data.f[0] = (aX + 4.0) / 8.0;
  tflInputTensor->data.f[1] = (aY + 4.0) / 8.0;
  tflInputTensor->data.f[2] = (aZ + 4.0) / 8.0;
  tflInputTensor->data.f[3] = (gX + 20.0) / 40.0;
  tflInputTensor->data.f[4] = (gY + 20.0) / 40.0;
  tflInputTensor->data.f[5] = (gZ + 20.0) / 40.0;


    // Run inferencing
    TfLiteStatus invokeStatus = tflInterpreter->Invoke();
    if (invokeStatus != kTfLiteOk) {
      Serial.println("Invoke failed!");
      while (1);
      return;
    }

    // Loop through the output tensor values from the model
    for (int i = 0; i < NUM_GESTURES; i++) {
      Serial.print(GESTURES[i]);
      Serial.print(": ");
      Serial.println(tflOutputTensor->data.f[i], 6);
        M5.Lcd.setCursor(30, 40);

      if(tflOutputTensor->data.f[i]>0.99){
        if(i == 0){
          M5.Lcd.fillScreen(RED);
          // M5.Lcd.print("UP       ");
        }else if(i == 1){
          M5.Lcd.fillScreen(GREEN);
          // M5.Lcd.print("DOWN     ");
        }else if(i == 2){
          M5.Lcd.fillScreen(YELLOW);
          // M5.Lcd.print("LEFT     ");
        }else if(i == 3){
          M5.Lcd.fillScreen(BLUE);
          // M5.Lcd.print("RIGHT    ");
        }
        // else if(i == 4){
        //   M5.Lcd.fillScreen(PINK);
        //   M5.Lcd.print("TOPRIGHT ");
        // }else if(i == 5){
        //   M5.Lcd.fillScreen(YELLOW);
        //   M5.Lcd.print("TOPLEFT  ");
        // }else if(i == 6){
        //   M5.Lcd.fillScreen(NAVY);
        //   M5.Lcd.print("BOTTOMRIGHT");
        // }else if(i == 7){
        //   M5.Lcd.fillScreen(PURPLE);
        //   M5.Lcd.print("BOTTOMLEFT");
        // }
      }
    }
    Serial.println();
M5.Lcd.setCursor(30, 60);
}