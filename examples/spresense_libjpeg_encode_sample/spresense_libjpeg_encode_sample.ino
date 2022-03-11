/*
 *  spresense_libjpeg_encode_sample.ino  
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

#define HAVE_LCD
#ifdef HAVE_LCD
#include "Adafruit_ILI9341.h"
#define TFT_DC  9
#define TFT_CS  10
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
#endif /* HAVE_LCD */

SDClass SD;
File myFile;

const int width  = CAM_IMGSIZE_QVGA_H;
const int height = CAM_IMGSIZE_QVGA_V;

extern "C" uint8_t* encode_jpeg(uint16_t* imgbuf
 , uint16_t width, uint16_t height, uint32_t* outlen);

void setup() { 
  Serial.begin(115200);
  while (!SD.begin()) printf("insert SD card\n");
  theCamera.begin();
  Serial.println("take picture: " + String(width) + "x" + String(height));
  theCamera.setStillPictureImageFormat(width, height, CAM_IMAGE_PIX_FMT_RGB565);
  CamImage img = theCamera.takePicture();
  if (!img.isAvailable()) {
    Serial.println("Cannot take a picture");
    return;
  }
  
  uint32_t outlen; uint8_t* outbuf;
  outbuf = encode_jpeg((uint16_t*)img.getImgBuff(), width, height, &outlen);
  if (outbuf == NULL) printf("outbuf is null\n");
  Serial.println("outlen = " + String(outlen));
  if (SD.exists("out.jpg")) SD.remove("out.jpg");
  myFile = SD.open("out.jpg", FILE_WRITE);
  myFile.write(outbuf, outlen);
  myFile.close();
  Serial.println("the clipped image saved as out.jpg");

#ifdef HAVE_LCD  
  tft.begin();
  tft.setRotation(3);   
  tft.drawRGBBitmap(0, 0, (uint16_t*)img.getImgBuff() 
            , CAM_IMGSIZE_QVGA_H, CAM_IMGSIZE_QVGA_V); 
#endif /* HAVE_LCD */

  theCamera.end();  
}

void loop() {}
