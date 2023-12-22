#include <stdio.h>
#include <stdlib.h>

#define uchar unsigned char
void pixelate_row(uchar *pixels_start, int image_width, int channels, int window_width);
void pixelate_column(uchar *pixels_start, int image_height, int channels, int window_height);
void pixelate_diag(uchar *pixels_start, int image_width, int image_height, int channels, int window_width, int window_height);
void pixelate_rectangle(uchar *filtered, uchar *pixels_start, int image_width, int image_height, int channels, int window_width, int window_height, int start_x, int start_y, int rect_w, int rect_h);
void pixelate_whole(uchar *pixels, int image_width, int image_height, int channels, int window_width, int window_height);
void pixelate_simple(uchar *pixels, int image_width, int image_height, int channels, int window_width, int window_height, int rank);
void pixelate_kernel(uchar* pixels, int channels, int window_width, int window_height, int rank);