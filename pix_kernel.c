#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "pix_kernel.h"
#include <mpi.h>
#include <stdio.h>
#ifndef uint8_t
#define uint8_t unsigned char
#endif

void pixelate_simple(uchar *pixels, int image_width, int image_height, int channels, int window_width, int window_height, int rank){
    // int column_index = start_x;
    // int row_index = start_y;
    int filterSize = window_width == window_height ? window_width : -1;
    if (filterSize<0){
        printf("P%d: Error: please use square filters\n", rank);
        return;;
    }
    printf("P%d: Received block of size %dx%d\n", rank,image_width,image_height);
    int tot_pixels = image_width*image_height;
    uchar *pixels_copy =
    malloc(tot_pixels*channels*sizeof(uchar));
    // printf("P%d: Copying pixels, initialized array\n", rank);
    for (int i = 0; i < tot_pixels * channels; i++)
    {
        pixels_copy[i] = (uint8_t)pixels[i];
    }
    // printf("P%d: Copy done\n", rank);
    for(int i = 0; i<tot_pixels; i++){
        int column_index = i%image_width;
        int row_index = i/image_width;
        int target_row_index = row_index + (filterSize - row_index % filterSize) - 1;
        int target_col_index = column_index + (filterSize - column_index % filterSize) - 1;
        if (target_col_index >= image_width)
        {
            target_col_index = image_width - 1;
        }
        if (target_row_index >= image_height)
        {
            target_row_index = image_height - 1;
        }
        if (target_row_index < 0 || target_col_index < 0)
        {
            printf("Error: negative index\n");
            return;
        }
        int index = target_row_index * image_width*channels + target_col_index * channels;
        int img_index = row_index * image_width*channels + column_index * channels;
        if(img_index!=index){
            pixels[img_index] = (int)pixels_copy[index];
            pixels[img_index + 1] = (int)pixels_copy[index + 1];
            pixels[img_index + 2] = (int)pixels_copy[index + 2];
        }
    }
    free(pixels_copy);
}

