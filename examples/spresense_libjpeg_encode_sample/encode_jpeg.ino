/*
 *  encode_jpeg.ino  
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

#define HAVE_BOOLEAN

#include <nuttx/config.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libjpeg/jpeglib.h>
extern "C" {
#include <setjmp.h>
}

uint8_t* encode_jpeg(uint16_t* imgbuf, uint16_t width, uint16_t height, uint32_t* outlen) {

  static struct jpeg_compress_struct cinfo;
  static struct jpeg_error_mgr jerr;
  const int quality = 98;
  
  /* Step 1: allocate and initialize JPEG compression object */
  /* We set up the normal JPEG error routines */
  cinfo.err = jpeg_std_error(&jerr);
  /* Now we can initialize the JPEG compression object. */
  jpeg_create_compress(&cinfo);
  printf("Step1: create compress object\n");

  unsigned char* out; unsigned long len;
  jpeg_mem_dest(&cinfo, &out, &len);
  printf("Step2: set the data source\n");

  /* Step 3: set file parameters */
  cinfo.image_width  = width;
  cinfo.image_height = height;
  cinfo.input_components = 3;
  cinfo.in_color_space = JCS_RGB;
  jpeg_set_quality(&cinfo, quality, TRUE);
  jpeg_set_defaults(&cinfo); 
  printf("Step3: set file parameters\n");
  
  /* Step 4: set data source parameters */
  jpeg_start_compress(&cinfo, TRUE);
  printf("Step4: start compress\n");
  
  /* Step 5: Start compressor */
  printf("Step5: write scanlines\n");
  uint8_t linebuf[width*3];
  JSAMPROW row_pointer[1];
  int n = 0;  /* for debug */
  while (cinfo.next_scanline < height) {
    uint16_t i, j;
    uint32_t offset = cinfo.next_scanline * width;
    for (i = 0, j = 0; i < width; ++i, j+=3) {
      uint16_t value = imgbuf[offset + i];
      linebuf[j + 0] = (value & 0xf800) >> 8; // red
      linebuf[j + 1] = (value & 0x07e0) >> 3; // green
      linebuf[j + 2] = (value & 0x001f) << 3; // blue
    }
    row_pointer[0] = linebuf;    
    jpeg_write_scanlines(&cinfo, row_pointer, 1);
    ++n;
  }
  printf("write lines: %d\n", n); /* for debug */

  /* Step 6: Finish decompression */
  jpeg_finish_compress(&cinfo);
  printf("Step6: finish compression\n");  

  /* Step 7: Release JPEG compression object */
  jpeg_destroy_compress(&cinfo);
  printf("Step7: destroy compress object\n");  

  *outlen = (uint32_t)len; 
  return out;
}
