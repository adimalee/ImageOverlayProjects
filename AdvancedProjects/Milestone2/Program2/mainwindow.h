#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#pragma pack(2)                    // Using pragma to force structure format
struct BMPFH                       // Bitmap file header
{
  char HeaderField[2];             // Used to identify the BMP and DIB file is 0x42 0x4D in hexadecimal, same as BM in ASCII
  unsigned int Size_of_BMP;        // size of the BMP file in bytes
  unsigned short Reserved1;        // Reserved; actual value depends on the application that creates the image
  unsigned short Reserved2;        // "                                                                       "
  unsigned int StartAddress;       // offset, i.e. starting address, of the byte where the bitmap image data (pixel array) can be found
};

#pragma pack()
struct DIBH                        // Bitmap information header
{
  unsigned int Size_of_Header;     // Size of this header (40 bytes)
  signed int Width;                // bitmap width in pixels (signed integer)
  signed int Height;               // bitmap height in pixels (signed integer)
  unsigned short Num_of_Planes;    // number of color planes (must be 1)
  unsigned short Num_of_Bits;      // number of bits per pixel, which is the color depth (1, 4, 8, 16, 24, 32)
  unsigned int CompMethod;         // compression method being used (0, 1, 2, 3)
  unsigned int Size_of_Raw;        // size of the raw bitmap data
  signed int HRes;                 // horizontal resolution of the image. (pixel per meter, signed integer)
  signed int VRes;                 // vertical resolution of the image. (pixel per meter, signed integer)
  unsigned int Num_of_Col;         // number of colors in the color palette, or 0 to default to 2^n
  unsigned int Num_of_ICol;        // number of important colors used, or 0 when every color is important; generally ignored
};

struct ColorTable
{
    unsigned char data[1024];
};

struct Pixel
{
    unsigned char pix[307200];
};


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
    void equalizeBMP(std::string, BMPFH*, DIBH*, ColorTable, Pixel);
    void overlayBMP(std::string, BMPFH*, DIBH*, ColorTable, Pixel, Pixel);
    void UpdateLabel(const QString, const QString);
    void on_horizontalSlider_sliderReleased();
    void on_horizontalSlider_2_sliderReleased();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    std::string getOverlayText();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
