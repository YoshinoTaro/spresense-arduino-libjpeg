/*
 *  spresense_libjpeg_sample.ino  
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

char filename[16] = "/mnt/sd0/in.jpg";
char outfile[17] = "/mnt/sd0/out.rgb";

void setup() {
  static struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr; 
  FILE* infile;                 /* source file */
  FILE* out;                    /* output file */
  JSAMPARRAY buffer;            /* Output row buffer */
  int row_stride;               /* physical row width in output buffer */

  struct stat buf;
  for (;;) {
    sleep(1);
    int ret = stat("/mnt/sd0", &buf);
    if (ret) printf("Please insert formatted SD Card.\n");
    else break;
  }

  if ((infile = fopen(filename, "rb")) == NULL) {
    fprintf(stderr, "can't open %s\n", filename);
    return;
  }
  printf("open %s\n", filename);

  if ((out = fopen(outfile, "wb")) == NULL) {
    fprintf(stderr, "can't open %s\n", outfile);
    return;    
  }
  printf("create %s\n", outfile);

  /* Step 1: allocate and initialize JPEG decompression object */
  /* We set up the normal JPEG error routines, then override error_exit. */
  cinfo.err = jpeg_std_error(&jerr);
  /* Now we can initialize the JPEG decompression object. */
  jpeg_create_decompress(&cinfo);
  printf("Step1: create decompress object\n");

  /* Step 2: specify data source (eg, a file) */
  jpeg_stdio_src(&cinfo, infile);
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
  /* JSAMPLEs per row in output buffer */
  /* row_stride = cinfo.output_width * cinfo.output_components; */
  //row_stride = cinfo.output_width * 2; // in case of YCbCr
  row_stride = cinfo.output_width * 3; // in case of RGB
  /* Make a one-row-high sample array that will go away when done with image */
  buffer = (*cinfo.mem->alloc_sarray)
                ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

  /* Step 6: while (scan lines remain to be read) */
  printf("Step6: read scanlines\n");
  int n = 0; /* for debug */
  while (cinfo.output_scanline < cinfo.output_height) {
    /* jpeg_read_scanlines expects an array of pointers to scanlines.
     * Here the array is only one element long, but you could ask for
     * more than one scanline at a time if that's more convenient.
     */
    (void) jpeg_read_scanlines(&cinfo, buffer, 1);
    if (row_stride != fwrite(buffer[0], 1, row_stride, out)) {
      printf("fwrite error : %d\n", errno);
    }
    ++n;
  }
  printf("read lines: %d\n", n);  /* for debug */
  printf("close output & input file\n");
  fclose(out); fclose(infile);
  printf("please rename the filename to out.data to visualize by GIMP\n");
  printf("The image size is %d x %d\n"
       , cinfo.output_width, cinfo.output_height);
  
  /* Step 7: Finish decompression */
  printf("Step7: finish decompression\n");  
  (void) jpeg_finish_decompress(&cinfo);

  /* Step 8: Release JPEG decompression object */
  printf("Step8: destroy decompress object\n");  
  jpeg_destroy_decompress(&cinfo);
 
}

void loop() {}
