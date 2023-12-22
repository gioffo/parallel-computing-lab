#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "pix_kernel.h"
#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv){
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);

    char* filename;
    int width = 1000;
    int height = 1000;
    int channels = 3;
    int window_width = 3;
    int window_height = 3;
    double start_time, end_time;

    start_time = MPI_Wtime();
    // Read arguments on all processes
    if (argc > 2)
    {
        filename = argv[1];
        width = atoi(argv[2]);
        height = atoi(argv[3]);
        switch (argc)
        {
        case 7:
            channels = atoi(argv[4]);
            window_width = atoi(argv[5]);
            window_height = atoi(argv[6]);
            break;
        case 6:
            channels = atoi(argv[4]);
            window_width = atoi(argv[5]);
            window_height = window_width;
            break;
        case 5:
            channels = atoi(argv[4]);
            break;
        
        default:
            break;
        }
    }
    else if (argc > 1)
    {
        filename = argv[0];
        width = atoi(argv[1]);
        height = width;
    }
    else
    {
        filename = "foto.raw";
    }
    if(rank==0){
        printf("P%d - Command Line Arguments:\n", rank);
        for(int i=0;i<argc;i++){
            printf("\targ[%d]: %s\n",i,argv[i]);
        }
        printf("\t Parsed: argc %d, image %s, width %d, height %d, channels %d, window_width %d, window_height %d\n",argc,filename,width,height,channels,window_width,window_height);
    }

    int num_pixels = width*height*channels;
    uchar* image = NULL;
    // Initialize image only on root process
    if(rank==0){
        image = (uchar*)malloc(num_pixels*sizeof(uchar));
        FILE* fp = fopen(filename, "rb");
        fread(image, sizeof(uchar), num_pixels, fp);
        fclose(fp);
        printf("P%d - IMAGE READ: image %s, width %d, height %d, channels %d\n", rank,filename,width,height,channels);
    }

    // Initialize blocks information and memory
    int block_width = width;
    int block_height = height/size;
    // Boolean calculation for cases when block_height is smaller than kernel size
    int block_height_smaller = block_height < window_height;
    // Tentative fix for bug for uneven cases
    int num_kernels = ceil(height/window_height);
    int blocks_p = num_kernels/size;
    block_height = blocks_p * window_height;
    int* sendcounts = (int*)malloc(size*sizeof(int));
    int* displacements = (int*)malloc(size*sizeof(int));
    for (int i = 0; i < size-1; i++){
        sendcounts[i] = block_width*block_height*channels;
        displacements[i] = i*block_width*block_height*channels;
    }
    sendcounts[size-1] = (height - (size-1)*block_height)*block_width*channels;
    displacements[size-1] = (size-1)*block_width*block_height*channels;
    if(0 && rank==0){
        printf("Calculated sendcounts and displacements:\n");
        printf("Shared block_width %d, block_height %d\n",block_width,block_height);
        printf("Last block_width %d, block_height %d\n",block_width,height - (size-1)*block_height);
    }
    if (rank==size-1){
        block_height = height - (size-1)*block_height;
    }
    // printf("P%d: block_width %d, block_height %d\n", rank,block_width,block_height);
    uchar* block = (uchar*)malloc(block_width*block_height*channels*sizeof(uchar));
    printf("P%d: block allocated\n", rank);
    // Scatter blocks to all processes
    end_time = MPI_Wtime(); 
    printf("P%d: BLOCK INIT:    \t\t%f\n", rank, end_time-start_time);
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Scatterv(image, sendcounts, displacements, MPI_UNSIGNED_CHAR, block, sendcounts[rank], MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);
    // MPI_Scatter(image, block_width*block_height*channels, MPI_UNSIGNED_CHAR, block, block_width*block_height*channels, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);
    end_time = MPI_Wtime();
    printf("P%d: BLOCK SCATTER: \t\t%f\n", rank, end_time-start_time);

    // Apply kernel to each block
    pixelate_simple(block, block_width, block_height, channels, window_width, window_height,rank);
    end_time = MPI_Wtime();
    printf("P%d: KERNEL APPLIED:\t\t%f\n", rank, end_time-start_time);
    // Gather blocks to root process
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Gatherv(block, sendcounts[rank], MPI_UNSIGNED_CHAR, image, sendcounts, displacements, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);
    // MPI_Gather(block, block_width*block_height*channels, MPI_UNSIGNED_CHAR, image, block_width*block_height*channels, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);
    end_time = MPI_Wtime();
    printf("P%d: BLOCK GATHERED:\t\t%f\n", rank, end_time-start_time);
    free(block);
    free(sendcounts);
    free(displacements);
    // Write image to file and free memory
    if(rank==0){
        char out_name[50];
        int mbytes = num_pixels/1000000;
        sprintf(out_name,"out%dMB_%dpx_p%d.raw",mbytes,window_width,size);
        FILE* fp = fopen("output.raw", "wb");
        fwrite(image, sizeof(uchar), num_pixels, fp);
        fclose(fp);
        free(image);
        end_time = MPI_Wtime();
        printf("P%d: WRITE TO DISK: \t\t%f\n", rank, end_time-start_time);
    }
    // Finalize MPI and return
    MPI_Finalize();
    return 0;    
}
