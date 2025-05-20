/* main.cpp */

//
// Performs a contrast stretch over a Windows bitmap (.bmp) file, making lighter pixels
// lighter and darker pixels darker.
//
// Usage: cs infile.bmp outfile.bmp steps
//
// Sree Dhyuti - JBU7511
//
// Initial author:
//   Prof. Joe Hummel
//   Northwestern University
//


// Imports
#include "app.h"      
#include "matrix.h"    
#include <mpi.h>       
#include <algorithm>    
#include <cstring>     
#include <iostream>     
#include <chrono>       

using namespace std;
typedef unsigned char uchar;

// Helper functions from original cs.cpp:
//  copy_boundary: copy outer border pixels to preserve image edges
void copy_boundary(uchar** dest, uchar** src, int rows, int cols);

//  stretch_one_pixel: apply 3x3 contrast-stretch to a single pixel
void stretch_one_pixel(uchar** dest, uchar** src, int baserow, int basecol);

// Serial and MPI contrast-stretch entrypoints
using Img = uchar**;
Img ContrastStretchSerial(Img img, int rows, int cols, int steps);
Img ContrastStretchMPI   (Img img, int rows, int cols, int steps);

int main(int argc, char* argv[])
{
    // Initialize MPI
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Parse arguments on rank 0
    if (argc != 4) {
        if (rank == 0) cout << "Usage: cs infile.bmp outfile.bmp steps" << endl;
        MPI_Finalize();
        return 0;
    }
    char* infile  = argv[1];       
    char* outfile = argv[2];     
    int   steps   = atoi(argv[3]);     

    if (rank == 0) {
        cout << "** Starting Contrast Stretch **" << endl
             << "   Input:  " << infile  << endl
             << "   Output: " << outfile << endl
             << "   Steps:  " << steps  << endl;
    }

    // Read bitmap on rank 0 (populate header, info, rows, cols)
    BITMAPFILEHEADER header;
    BITMAPINFOHEADER info;
    Img image;
    int rows, cols;
    if (rank == 0) cout << "** Reading bitmap..." << endl;
    image = ReadBitmapFile(infile, header, info, rows, cols);
    if (!image) {
        if (rank == 0) cerr << "** Failed reading file" << endl;
        MPI_Finalize();
        return 1;
    }

    // Synchronize and start timing
    MPI_Barrier(MPI_COMM_WORLD);
    auto t0 = chrono::high_resolution_clock::now();

    // Dispatch to serial or MPI version
    if (size > 1) {
        if (rank == 0) cout << "** MPI Contrast Stretch **" << endl;
        image = ContrastStretchMPI(image, rows, cols, steps);
    } else {
        image = ContrastStretchSerial(image, rows, cols, steps);
    }

    // Synchronize and stop timing
    MPI_Barrier(MPI_COMM_WORLD);
    auto t1 = chrono::high_resolution_clock::now();
    if (rank == 0) {
        double dur = chrono::duration<double>(t1 - t0).count();
        cout << "** Processing time: " << dur << " s" << endl;
    }

    //Write output bitmap on rank 0
    if (rank == 0) {
        cout << "** Writing bitmap..." << endl;
        WriteBitmapFile(outfile, header, info, image);
        cout << "** Done." << endl;
    }

    //Finalize MPI
    MPI_Finalize();
    return 0;
}


// Serial implementation
Img ContrastStretchSerial(Img img, int rows, int cols, int steps)
{
    // Allocate a temporary image
    Img tmp = New2dMatrix<uchar>(rows, cols * 3);
    // Copy the outer boundary so edges remain fixed
    copy_boundary(tmp, img, rows, cols);
   
    for (int s = 1; s <= steps; ++s) {
        // Loop over non-boundary rows
        for (int r = 1; r < rows - 1; ++r) {
            int bc = 3;  
     
            for (int c = 1; c < cols - 1; ++c, bc += 3) {
                stretch_one_pixel(tmp, img, r, bc);
            }
        }
        // Swap pointers 
        swap(img, tmp);
    }

    Delete2dMatrix(tmp);
    return img;
}


// MPI implementatin 
Img ContrastStretchMPI(Img img, int rows, int cols, int steps)
{
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    //Determine local chunk size (rows per rank + remainder)
    int base = rows / size;
    int rem  = rows % size;
    int local_rows = base + (rank < rem ? 1 : 0);

    // Allocate local buffers including two halo rows
    Img loc  = New2dMatrix<uchar>(local_rows + 2, cols * 3);
    Img loc2 = New2dMatrix<uchar>(local_rows + 2, cols * 3);

    // Scatter: rank 0 sends each stripe to other ranks
    if (rank == 0) {
        for (int r = 0; r < local_rows; ++r)
            memcpy(loc[r+1], img[r], cols*3);
        int pos = local_rows;
        for (int p = 1; p < size; ++p) {
            int pr = base + (p < rem ? 1 : 0);
            for (int r = 0; r < pr; ++r)
                MPI_Send(img[pos + r], cols*3, MPI_UNSIGNED_CHAR, p, 0, MPI_COMM_WORLD);
            pos += pr;
        }
    } else {
        for (int r = 0; r < local_rows; ++r)
            MPI_Recv(loc[r+1], cols*3, MPI_UNSIGNED_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    for (int s = 1; s <= steps; ++s) {
        if (rank == 0) cout << "** MPI Step " << s << "..." << endl;
        if (rank > 0)
            MPI_Sendrecv(loc[1],           cols*3, MPI_UNSIGNED_CHAR, rank-1, 1,
                         loc[0],           cols*3, MPI_UNSIGNED_CHAR, rank-1, 2,
                         MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        if (rank < size-1) 
            MPI_Sendrecv(loc[local_rows], cols*3, MPI_UNSIGNED_CHAR, rank+1, 2,
                         loc[local_rows+1],cols*3, MPI_UNSIGNED_CHAR, rank+1, 1,
                         MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        if (rank == 0)
            memcpy(loc2[1], loc[1], cols*3);    
        else
            memcpy(loc2[1], loc[0], cols*3);    
        if (rank == size-1)
            memcpy(loc2[local_rows], loc[local_rows], cols*3);
        else
            memcpy(loc2[local_rows], loc[local_rows+1], cols*3);
        for (int r = 1; r <= local_rows; ++r) {
            loc2[r][0] = loc[r][0];
            loc2[r][1] = loc[r][1];
            loc2[r][2] = loc[r][2];
            int off = (cols-1)*3;
            loc2[r][off]   = loc[r][off];
            loc2[r][off+1] = loc[r][off+1];
            loc2[r][off+2] = loc[r][off+2];
        }

        //Contrast-stretch interior rows only
        int rs = (rank==0       ? 2 : 1);
        int re = (rank==size-1 ? local_rows-1 : local_rows);
        for (int r = rs; r <= re; ++r) {
            int bc = 3;
            for (int c = 1; c < cols-1; ++c, bc+=3) {
                stretch_one_pixel(loc2, loc, r, bc);
            }
        }

        //Swap buffers for next iteration
        swap(loc, loc2);
    }

    //Gather processed stripes back to rank 0
    if (rank == 0) {
        for (int r = 0; r < local_rows; ++r)
            memcpy(img[r], loc[r+1], cols*3);
        int pos = local_rows;
        for (int p = 1; p < size; ++p) {
            int pr = base + (p<rem ?1:0);
            for (int r = 0; r < pr; ++r)
                MPI_Recv(img[pos+r], cols*3, MPI_UNSIGNED_CHAR, p,3,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            pos += pr;
        }
    } else {
        // send stripe to rank 0
        for (int r = 0; r < local_rows; ++r)
            MPI_Send(loc[r+1], cols*3, MPI_UNSIGNED_CHAR, 0,3,MPI_COMM_WORLD);
    }

    //Cleanup buffers
    Delete2dMatrix(loc2);
    Delete2dMatrix(loc);
    return (rank==0 ? img : nullptr);
}
