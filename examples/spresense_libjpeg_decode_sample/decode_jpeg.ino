/*
 *  decode_jpeg.ino  
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

bool decode_jpeg(uint8_t* imgbuf, uint32_t imgsize
     , uint8_t* outbuf, int width, int height) {

  static struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;  

  /* Step 1: allocate and initialize JPEG decompression object */
  /* We set up the normal JPEG error routines */
  cinfo.err = jpeg_std_error(&jerr);
  /* Now we can initialize the JPEG compression object. */
  jpeg_create_decompress(&cinfo);
  printf("Step1: create decompress object\n");

  jpeg_mem_src(&cinfo, imgbuf, imgsize);
  printf("Step2: set the data source\n");    

  /* Step 3: read file parameters with jpeg_read_header() */
  (void)jpeg_read_header(&cinfo, TRUE);
  printf("Step3: read the data header\n");

  /* We can ignore the return value from jpeg_read_header since
   *   (a) suspension is not possible with the stdio data source, and
   *   (b) we passed TRUE to reject a tables-only JPEG file as an error.
   * See libjpeg.txt for more info.
   */

  /* Step 4: set parameters for decompression */
  cinfo.out_color_space = JCS_RGB;
  printf("Step4: set the output parameter as JCS_RGB\n");

  /* Step 5: Start decompressor */
  jpeg_start_decompress(&cinfo);
  printf("Step5: start decompression\n");
  printf("cinfo.output_width:  %d\n", cinfo.output_width);  
  printf("cinfo.output_height: %d\n", cinfo.output_height);  
  if (cinfo.output_width != width 
   || cinfo.output_height != height) {
    printf("[Error] size mismatch\n");
    printf("  width = %d\n", width);
    printf(" height = %d\n", height);
    return false;
  }

  /* JSAMPLEs per row in output buffer */
  JSAMPARRAY buffer; /* Output row buffer */
  int row_stride;    /* physical row width in output buffer */
  /* row_stride = cinfo.output_width * cinfo.output_components; */
  //row_stride = cinfo.output_width * 2; // in case of YCbCr
  row_stride = cinfo.output_width * 3; // in case of RGB
  /* Make a one-row-high sample array that will go away when done with image */
  buffer = (*cinfo.mem->alloc_sarray)
                ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

  /* Step 6: while (scan lines remain to be read) */
  printf("Step6: read scanlines\n");
  int n = 0;  /* for debug */
  uint8_t* tmp = outbuf;
  while (cinfo.output_scanline < height) {
    /* jpeg_read_scanlines expects an array of pointers to scanlines.
     * Here the array is only one element long, but you could ask for
     * more than one scanline at a time if that's more convenient.
     */
    jpeg_read_scanlines(&cinfo, buffer, 1);
    memcpy(tmp, buffer[0], width*3);
    tmp += width*3; 
    ++n;
  }
  printf("read lines: %d\n", n); /* for debug */

  /* Step 7: Finish decompression */
  jpeg_finish_decompress(&cinfo);
  printf("Step7: finish decompression\n");  

  /* Step 8: Release JPEG decompression object */
  jpeg_destroy_decompress(&cinfo);
  printf("Step8: destroy decompress object\n");  

  return true;
}
