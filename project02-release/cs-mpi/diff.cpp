/*
 * bmp_diff.cpp
 *
 * Reads two BMP files of the same dimensions, compares them pixel-by-pixel,
 * and writes an output BMP where mismatched pixels are marked red and
 * matching pixels are black. Includes debug prints and mismatch count.
 *
 * Usage:
 *   ./bmp_diff image1.bmp image2.bmp diff.bmp
 */

#include "app.h"    // ReadBitmapFile, WriteBitmapFile
#include "matrix.h" // New2dMatrix, Delete2dMatrix
#include <iostream>
#include <cstring>

using namespace std;
typedef unsigned char uchar;

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cout << "Usage: " << argv[0]
             << " <image1.bmp> <image2.bmp> <output_diff.bmp>" << endl;
        return 1;
    }

    char* fileA   = argv[1];
    char* fileB   = argv[2];
    char* outFile = argv[3];

    BITMAPFILEHEADER headerA, headerB;
    BITMAPINFOHEADER infoA, infoB;
    int rowsA, colsA, rowsB, colsB;

    // Read first image
    cout << "[DEBUG] Reading " << fileA << endl;
    uchar** imgA = ReadBitmapFile(fileA, headerA, infoA, rowsA, colsA);
    if (!imgA) {
        cerr << "Error: could not read " << fileA << endl;
        return 1;
    }
    cout << "[DEBUG] Loaded " << fileA << ": " << rowsA << " x " << colsA << endl;

    // Read second image
    cout << "[DEBUG] Reading " << fileB << endl;
    uchar** imgB = ReadBitmapFile(fileB, headerB, infoB, rowsB, colsB);
    if (!imgB) {
        cerr << "Error: could not read " << fileB << endl;
        Delete2dMatrix(imgA);
        return 1;
    }
    cout << "[DEBUG] Loaded " << fileB << ": " << rowsB << " x " << colsB << endl;

    // Ensure dimensions match
    if (rowsA != rowsB || colsA != colsB) {
        cerr << "Error: dimensions differ ("
             << rowsA << "x" << colsA << " vs "
             << rowsB << "x" << colsB << ")" << endl;
        Delete2dMatrix(imgA);
        Delete2dMatrix(imgB);
        return 1;
    }

    int rows = rowsA;
    int cols = colsA;
    cout << "[DEBUG] Allocating diff buffer: "
         << rows << " x " << (cols * 3) << " channels" << endl;

    // Allocate diff image
    uchar** diff = New2dMatrix<uchar>(rows, cols * 3);
    if (!diff) {
        cerr << "Error: could not allocate diff buffer." << endl;
        Delete2dMatrix(imgA);
        Delete2dMatrix(imgB);
        return 1;
    }

    // Compare pixel-by-pixel
    for (int r = 0; r < rows; ++r) {
        int base = 0;
        for (int c = 0; c < cols; ++c) {
            bool mismatch = false;
            for (int k = 0; k < 3; ++k) {
                if (imgA[r][base + k] != imgB[r][base + k]) {
                    mismatch = true;
                    break;
                }
            }
            diff[r][base    ] = 0;              // B
            diff[r][base + 1] = 0;              // G
            diff[r][base + 2] = mismatch ? 255 : 0; // R
            base += 3;
        }
    }

    // Count mismatches before writing-out
    int mismatch_count = 0;
    for (int r = 0; r < rows; ++r) {
        int base = 0;
        for (int c = 0; c < cols; ++c, base += 3) {
            if (diff[r][base + 2] == 255)
                ++mismatch_count;
        }
    }
    cout << "Total mismatched pixels: " << mismatch_count << endl;

    // Write diff BMP using first image's header/info
    mismatch_count = 0;
    for (int r = 0; r < rows; ++r) {
        int base = 0;
        for (int c = 0; c < cols; ++c, base += 3) {
            if (diff[r][base + 2] == 255)
                ++mismatch_count;
        }
    }
    cout << "Total mismatched pixels: " << mismatch_count << endl;

    // Clean up
    cout << "[DEBUG] Deleting imgA" << endl;
    Delete2dMatrix(imgA);
    cout << "[DEBUG] Deleting imgB" << endl;
    Delete2dMatrix(imgB);
    cout << "[DEBUG] Done." << endl;

    return 0;
}
