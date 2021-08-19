#include <iostream>
#include <unistd.h>
#include <fstream>
#include <algorithm>
#include <math.h>


using std::cout;
using std::endl;
using std::ofstream;
using std::ifstream;

#pragma pack(1)
#pragma once

typedef int LONG;
typedef unsigned short WORD;
typedef unsigned int DWORD;

typedef struct tagBITMAPFILEHEADER {
    WORD bfType;
    DWORD bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfOffBits;
} BITMAPFILEHEADER, *PBITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER {
    DWORD biSize;
    LONG biWidth;
    LONG biHeight;
    WORD biPlanes;
    WORD biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG biXPelsPerMeter;
    LONG biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;



unsigned char** pixelGray;
unsigned char** pixelOverlay;
unsigned char** pixelOut1;
//unsigned char** colorTable;
//unsigned char** colorTableO;
unsigned char** pixelHE;

int rows;
int cols;
int sizeofImg;

int rowsOverlay;
int colsOverlay;



struct colorPalette
{
    unsigned char table[1024];
};


void GRAY_Allocate(unsigned char**& data)
{
    data = new unsigned char*[rows];
    for (int i = 0; i < rows; i++)
    {
        data[i] = new unsigned char[cols];
    }
}

struct histogram
{
    int freq;
    int maxi;
    int hist[256];
    int chist[256];
    int cfreq[256];



    void initializeHist()
    {
        for(int i = 0; i < 256; i++)
        {
            hist[i] = 0;
        }
    }

    void cumulate()
    {
        chist[0] = hist[0];
        for(int i = 1; i < 256; i++)
        {
            chist[i] = hist[i] + chist[i-1];
        }
    }

    void frequency()
    {

        freq = chist[255]/256;

        freq = freq + 1;

    }

   void getMaxI()
   {
       maxi = hist[0];
       for(int i = 0; i < 256; i++)
       {
           if(maxi < hist[i])
           {
               maxi = hist[i];
           }
       }
    }

    void freqarray(){

        int i = freq;

        for(int j =0; j < 256; j++)
        {

            cfreq[j]= 0;
        }

        cfreq[0] = i;
        for(int m =1; m < 256; m++)
        {

            cfreq[m]= cfreq[m-1] +i;
        }

    }

};




bool FillAndAllocate(char*& buffer, const char* BMPfile, int& rows, int& cols, int& BufferSize) //Returns 1 if executed sucessfully, 0 if not sucessfull
{
    std::ifstream file(BMPfile);

    if (file)
    {
        file.seekg(0, std::ios::end);
        std::streampos length = file.tellg();
        file.seekg(0, std::ios::beg);

        buffer = new char[length];
        file.read(&buffer[0], length);

        PBITMAPFILEHEADER file_header;
        PBITMAPINFOHEADER info_header;

        file_header = (PBITMAPFILEHEADER) (&buffer[0]);
        info_header = (PBITMAPINFOHEADER) (&buffer[0] + sizeof(BITMAPFILEHEADER));
        rows = info_header->biHeight;
        cols = info_header->biWidth;
        BufferSize = file_header->bfSize;
        return 1;
    }
    else
    {
        cout << "File" << BMPfile << " does not Exist!" << endl;
        return 0;
    }
}


void GetPixlesFromBMP8(unsigned char** pixelGray, int end, int rows, int cols, char* FileReadBuffer, struct histogram h, unsigned char** pixelHE) // end is BufferSize (total size of file)
{
    colorPalette cp;
    h.initializeHist();


    int count = 1;
    int extra = cols % 4; // The nubmer of bytes in a row (cols) will be a multiple of 4.
    /* "The data is stored row by row with padding on the end of each row.
    The padding ensures the image rows are multiples of four. The four, just like in the color table,
    makes it easier to read blocks and keep track of addresses."*/
    int index = 0;

    for (int i = 0; i < rows; i++)
    {
        count += extra;
        for (int j = cols - 1; j >= 0; j--)
        {
            index = end - count++;
            pixelGray[i][j] = FileReadBuffer[index];
            h.hist[(int)pixelGray[i][j]]++;
        }
    }

    //h.getMaxI();

    //int maxint = h.maxi;

    //cout << maxint << endl;

    h.cumulate();

    //h.frequency();

    //h.freqarray();

    double temp;

    for (int i = 0; i < rows; i++)
    {
        for (int j = cols - 1; j >= 0; j--)
        {
            temp =  (h.chist[(int)pixelGray[i][j]] - h.chist[0]);

            pixelHE[i][j] = round ((temp/(sizeofImg - h.chist[0]))* 255);       //option1

            //pixelHE[i][j] = (h.cfreq[(int)pixelGray[i][j]] * h.maxi) *255/ sizeofImg;   //option2

            int testHE = pixelHE[i][j];
            int testPx = pixelGray[i][j];
            //cout << "old: " << testPx << " VS new: " << testHE << endl;
        }
    }

}


void GetOvPixlesFromBMP8(unsigned char** pixelOverlay, int end, int rows, int cols, char* FileReadBuffer) // end is BufferSize (total size of file)
{
    int count = 1;
    int extra = cols % 4; // The nubmer of bytes in a row (cols) will be a multiple of 4.
    /* "The data is stored row by row with padding on the end of each row.
    The padding ensures the image rows are multiples of four. The four, just like in the color table,
    makes it easier to read blocks and keep track of addresses."*/
    int index = 0;

    for (int i = 0; i < rows; i++)
    {
        count += extra;
        for (int j = cols - 1; j >= 0; j--)
        {
            index = end - count++;
            pixelOverlay[i][j] = FileReadBuffer[index];
        }
    }
}


void WriteOutBMP8(char* FileBuffer, const char* outFile, int BufferSize)
{


    colorPalette clr;


    std::ofstream write(outFile, std::ios::binary);
    if (!write)
    {
        cout << "Failed to write " << outFile << endl;
        return;
    }



    write.write(FileBuffer, BufferSize);



    // write out color table

    write.write((char*)&clr, sizeof(colorPalette));


    // write out pixels
    int count = 1;
    int extra = cols % 4; // The number of bytes in a row (cols) will be a multiple of 4.
    int index = 0;
    char ch;
    int intChar;
    for (int i = 0; i < rows; i++)
    {
        count += extra;
        for (int j = cols - 1; j >= 0; j--)
        {
            index = BufferSize - count;
            //FileBuffer[index] = pixelGray[i][j];
            FileBuffer[index] = 0;
            ch = pixelGray[i][j];
            intChar = ch;
            count++;
        }
    }
}

void OutBMP8(char* FileBuffer, const char* outFile, int BufferSize, unsigned char** pixelHE)
{

    int count = 1;
    int extra = cols % 4; // The nubmer of bytes in a row (cols) will be a multiple of 4.
    int index = 0;

    for (int i = 0; i < rows; i++)
    {
        count += extra;
        for (int j = cols - 1; j >= 0; j--)
        {
            index = BufferSize - count;
            FileBuffer[index] = pixelHE[i][j];
            count++;
        }
    }

    std::ofstream write(outFile, std::ios::binary);
    if (!write)
    {
        cout << "Failed to write " << outFile << endl;
        return;
    }

    colorPalette clr;

    write.write(FileBuffer, BufferSize);
    write.write((char*)&clr, sizeof(colorPalette));
}


void overlayBMP(unsigned char** pixelGray,unsigned char** pixelOverlay, char* FileBuffer, const char* outFile, int BufferSize)
{
    if(rows != rowsOverlay || cols != colsOverlay)
    {
        cout << "Original image and overlay image do not have the same dimensions!" << endl;
        return;
    }
    char ch1;
    char ch2;
    for (int i = 0; i < rows; i++)
    {
        for (int j = cols - 1; j >= 0; j--)
        {
            ch1 = pixelOverlay[i][j];
            if(pixelOverlay[i][j] != 0)
            {

                pixelOut1[i][j] = pixelGray[i][j];
                ch1 = pixelOut1[i][j];
            }
            else
            {
                ch2 = pixelGray[i][j];
                pixelOut1[i][j] = (unsigned char)255;
                ch1 = pixelOut1[i][j];
            }
        }
    }

    int count = 1;
    int extra = cols % 4; // The nubmer of bytes in a row (cols) will be a multiple of 4.
    int index = 0;

    for (int i = 0; i < rows; i++)
    {
        count += extra;
        for (int j = cols - 1; j >= 0; j--)
        {
            index = BufferSize - count;
            FileBuffer[index] = pixelOut1[i][j];

            count++;
        }
    }



    std::ofstream write(outFile, std::ios::binary);
    if (!write)
    {
        cout << "Failed to write " << outFile << endl;
        return;
    }

    colorPalette clr;

    write.write(FileBuffer, BufferSize);
    write.write((char*)&clr, sizeof(colorPalette));
}




int main(int args, char** cat)
{
    char* FileBuffer; int BufferSize;
    char* OverlayBuffer; int OverlaySize;

    histogram h;
    h.initializeHist();



    # define BMPfile "test1.bmp"         //change to test2
    # define overlay "overlay1.bmp"     //change to overlay2


    if (!FillAndAllocate(FileBuffer, BMPfile, rows, cols, BufferSize))
    {
        cout << "File read error" << endl;
        return 0;
    }
    cout << "Rows: " << rows << " Cols: " << cols << endl;

    sizeofImg = rows*cols;

    cout << "Original image size: " << sizeofImg << "\n" << endl;


    if (!FillAndAllocate(OverlayBuffer, overlay, rowsOverlay, colsOverlay, OverlaySize))
    {
        cout << "File read error" << endl;
        return 0;
    }
    cout << "Overlay Rows: " << rowsOverlay << " Overlay Cols: " << colsOverlay << endl;

    GRAY_Allocate(pixelGray);       //allocate 2D array for pixel data
    GRAY_Allocate(pixelHE);         //allocate 2D array for HE result
    GRAY_Allocate(pixelOverlay);    //allocate 2D array for overlay pixels
    GRAY_Allocate(pixelOut1);       //allocate 2D array for overlay result

    GetPixlesFromBMP8(pixelGray,BufferSize, rows, cols, FileBuffer, h, pixelHE);

    #define WriteOutFile "BMPout.bmp"


    //WriteOutBMP8(FileBuffer, WriteOutFile,BufferSize);


    //getoColorTable(overlay, OverlaySize, OverlayBuffer);


    //GRAY_Allocate(pixelOverlay);

    GetOvPixlesFromBMP8(pixelOverlay,OverlaySize, rowsOverlay, colsOverlay, OverlayBuffer); //get overly pixels and store them in pixelOverlay

    //GRAY_Allocate(pixelOut1);


    #define BMPout1 "out1.bmp"
    overlayBMP(pixelGray,pixelOverlay,FileBuffer, BMPout1,BufferSize);


    //GRAY_Allocate(pixelHE);
    //histEqu(pixelGray, pixelHE, h);



    #define HEout "out2.bmp"
    OutBMP8(FileBuffer, HEout, BufferSize, pixelHE);


    //#define BMPout1 "out1.bmp"
    //OutBMP8(FileBuffer, BMPout1,BufferSize);


    return 1;
}
