/*
 *  spresense_libjpeg_decode_sample.ino  
 *  Copyright 2022 Yoshino Taro
 *
 *  This is a sample sketch for the Spresense libjpeg board package.  
 *  You need Spresense Main Board, Camera Board, and Extenstion board.
 *  And it would better have ILI9341 LCD to check the output.
 *  
 *  This example code is under LGPL v2.1 
 *  (because Arduino library of Spresense is under LGPL v2.1)
 *  And the mp3 files using this program are under CC-BY-SA
 */

#include <Camera.h>
#include <SDHCI.h>
SDClass SD;

#define HAVE_LCD
#ifdef HAVE_LCD
#include "Adafruit_ILI9341.h"
#define TFT_DC  9
#define TFT_CS  10
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
#endif /* HAVE_LCD */

const int width  = CAM_IMGSIZE_QVGA_H;
const int height = CAM_IMGSIZE_QVGA_V;
uint8_t* outbuf = NULL;

extern "C" bool decode_jpeg(uint8_t* imgbuf, uint32_t imgsize
  , uint8_t* outbuf, int width, int height);


void setup() {  
  Serial.begin(115200);
  while (!SD.begin()) printf("insert SD card\n");
  theCamera.begin();
  theCamera.setStillPictureImageFormat(
    width, height, CAM_IMAGE_PIX_FMT_JPG);
  CamImage img = theCamera.takePicture();
  if (!img.isAvailable()) {
    Serial.println("Cannot take a picture");
    return;
  }

  outbuf = (uint8_t*)malloc(sizeof(uint8_t)*width*height*3);
  if (outbuf == NULL) {
    Serial.println("Out of memory!");
    Serial.println("need to consider a smaller size picture");
    Serial.println("or expand the memory size for main-core");
    return;
  } else {
    Serial.println("memory allocated");
    memset(outbuf, NULL, width*height*3);
  }

  if (!decode_jpeg(img.getImgBuff(), img.getImgSize()
                 , outbuf, width, height)) {
    Serial.println("decode error");
    return;
  }

  if (SD.exists("out.rgb")) SD.remove("out.rgb");
  File myFile = SD.open("out.rgb", FILE_WRITE);
  myFile.write(outbuf, width*height*3);
  myFile.close();
  free(outbuf);
  Serial.println("the clipped image saved as out.rgb");
  Serial.println("Please change the filename to out.data to visualize by GIMP");

  // end the camera to reset the format and free the image buffer
  theCamera.end(); 

#ifdef HAVE_LCD  
  Serial.println("Preview the picture for check");
  tft.begin();
  tft.setRotation(3);
  theCamera.begin();
  theCamera.setStillPictureImageFormat(
    width, height, CAM_IMAGE_PIX_FMT_RGB565);
  CamImage tmp = theCamera.takePicture();
  tft.drawRGBBitmap(0, 0, (uint16_t*)tmp.getImgBuff(), width, height);
#endif /* HAVE_LCD */

}

void loop() {}
