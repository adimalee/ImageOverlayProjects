#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QFileDialog"
#include "QPixmap"
#include "QGraphicsItem"
#include "QGraphicsPixmapItem"
#include <fstream>
#include <algorithm>
#include <math.h>

using namespace std;

//int oldValue = 1;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    int slider1, slider2;
    ui -> horizontalSlider -> setRange(1,100);
    ui -> horizontalSlider_2 -> setRange(1,100);
    ui -> horizontalSlider -> setSliderPosition(50);
    ui -> horizontalSlider_2 -> setSliderPosition(50);
    slider1 = ui -> horizontalSlider -> value();
    slider2 = ui -> horizontalSlider_2 -> value();
    UpdateLabel(QString::number(slider1), QString::number(slider2));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
        QFileDialog dialog(this);
        dialog.setNameFilter(tr("Images (*.bmp)"));
        dialog.setViewMode(QFileDialog::Detail);
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                        "C:/",
                                                        tr("Images (*.bmp)"));
        ui -> label_2 -> setText(fileName);
        QPixmap image(fileName);
        ui -> label_4 -> setPixmap(image);
}

void MainWindow::on_horizontalSlider_sliderReleased()
{
    int slider1, slider2;
    ui -> horizontalSlider -> setRange(1,100);
    ui -> horizontalSlider_2 -> setRange(1,100);
    slider1 = ui -> horizontalSlider -> value();
    slider2 = ui -> horizontalSlider_2 -> value();
    UpdateLabel(QString::number(slider1), QString::number(slider2));
    QString Qfilename = ui -> label_2 -> text();
    std::string filename = Qfilename.toLocal8Bit().constData();
    std::ifstream inFile(filename.c_str(), ios::binary);
    //                          Bitmap File Header
    char* temp = new char[sizeof(BMPFH)];
    // using "read" since binary; input from file placed into temp for each member of structure
    inFile.read(temp, sizeof(BMPFH));
    BMPFH* fh = (BMPFH*)(temp);


    //                          Bitmap Information Header
    temp = new char[sizeof(DIBH)];
    inFile.read(temp, sizeof(DIBH));
    DIBH* ih = (DIBH*)(temp);

    //                          Color Table
    inFile.seekg(54, ios::beg);  //offset to beginning of color table
    ColorTable ct;
    inFile.read((char*)&ct, sizeof(ColorTable));


    //                          Pixel Data
    inFile.seekg(fh->StartAddress, ios::beg); //offset to beginning of pixel array
    Pixel px;
    inFile.read((char*)&px, sizeof(Pixel));
    inFile.close();

    char HF[2];
    HF[1] = fh->HeaderField[0];
    HF[2] = fh->HeaderField[1];
    unsigned int SoB = fh->Size_of_BMP;
    unsigned short R1 = fh->Reserved1;
    unsigned short R2 = fh->Reserved2;
    unsigned int SA = fh->StartAddress;

    unsigned int SoH = ih->Size_of_Header;
    signed int  W = ih->Width;
    signed int H = ih->Height;
    unsigned short NoP = ih->Num_of_Planes;
    unsigned short NoB = ih->Num_of_Bits;
    unsigned int CM = ih->CompMethod;
    unsigned int SoR = ih->Size_of_Raw;
    signed int HR = ih->HRes;
    signed int VR = ih->VRes;
    unsigned int NoC = ih->Num_of_Col;
    unsigned int NoIC = ih->Num_of_ICol;

    std::string outFile_name = "E:/Fall 2016 Resources/ECE484W-20160906T232551Z/ECE484W/Milestone 2/Program 2/Program 2/Program 2/new.bmp";

    //write header information to new file
    std::ofstream outFile(outFile_name.c_str(), ios::binary);
    outFile.write(reinterpret_cast<char*>(&HF[1]), sizeof(HF[1]));
    outFile.write(reinterpret_cast<char*>(&HF[2]), sizeof(HF[2]));
    outFile.write(reinterpret_cast<char*>(&SoB), sizeof(SoB));
    outFile.write(reinterpret_cast<char*>(&R1), sizeof(R1));
    outFile.write(reinterpret_cast<char*>(&R2), sizeof(R2));
    outFile.write(reinterpret_cast<char*>(&SA), sizeof(SA));
    outFile.write(reinterpret_cast<char*>(&SoH), sizeof(SoH));
    outFile.write(reinterpret_cast<char*>(&W), sizeof(W));
    outFile.write(reinterpret_cast<char*>(&H), sizeof(H));
    outFile.write(reinterpret_cast<char*>(&NoP), sizeof(NoP));
    outFile.write(reinterpret_cast<char*>(&NoB), sizeof(NoB));
    outFile.write(reinterpret_cast<char*>(&CM), sizeof(CM));
    outFile.write(reinterpret_cast<char*>(&SoR), sizeof(SoR));
    outFile.write(reinterpret_cast<char*>(&HR), sizeof(HR));
    outFile.write(reinterpret_cast<char*>(&VR), sizeof(VR));
    outFile.write(reinterpret_cast<char*>(&NoC), sizeof(NoC));
    outFile.write(reinterpret_cast<char*>(&NoIC), sizeof(NoIC));

    //write color table to new file
    unsigned char d;
    for (int i = 0; i < 1024; i++)
    {
        if (i < 3) d = 0;
        else d = ct.data[i];
        outFile.write(reinterpret_cast<char*>(&d), sizeof(d));
    }

    int brightness_change;
    if(slider1 == 50)
    {
        brightness_change = 0;
    }
    else
    {
        brightness_change = slider1 - 50;
    }

    //write new pixel data to new file
    unsigned char newP;
    unsigned char oldP;
    int check;
    for (int j = 0; j < H; j++)
    {
        for (int i = 0; i < W; i++)
        {
            oldP = px.pix[j*W + i];
            if (slider2 == 50)
            {
                check = oldP + brightness_change;
            }
            else
            {
                check = round(((oldP-127)*(2.5*(slider2))/100)+127+brightness_change);
            }
            if (check < 0)
                newP = 0;
            else if (check > 255)
                newP = 255;
            else
                newP = check;
            outFile.write(reinterpret_cast<char*>(&newP), sizeof(newP));
        }
    }
    outFile.close();
    QPixmap image(QString::fromStdString(outFile_name));
    //ui -> label_2 -> setText(QString::fromStdString(outFile_name));
    ui -> label_4 -> setPixmap(image);
    //oldValue = slider1;
}

void MainWindow::on_horizontalSlider_2_sliderReleased()
{
    int slider1, slider2;
    ui -> horizontalSlider -> setRange(1,100);
    ui -> horizontalSlider_2 -> setRange(1,100);
    slider1 = ui -> horizontalSlider -> value();
    slider2 = ui -> horizontalSlider_2 -> value();
    UpdateLabel(QString::number(slider1), QString::number(slider2));
    QString Qfilename = ui -> label_2 -> text();
    std::string filename = Qfilename.toLocal8Bit().constData();
    std::ifstream inFile(filename.c_str(), ios::binary);
    //                          Bitmap File Header
    char* temp = new char[sizeof(BMPFH)];
    // using "read" since binary; input from file placed into temp for each member of structure
    inFile.read(temp, sizeof(BMPFH));
    BMPFH* fh = (BMPFH*)(temp);


    //                          Bitmap Information Header
    temp = new char[sizeof(DIBH)];
    inFile.read(temp, sizeof(DIBH));
    DIBH* ih = (DIBH*)(temp);

    //                          Color Table
    inFile.seekg(54, ios::beg);  //offset to beginning of color table
    ColorTable ct;
    inFile.read((char*)&ct, sizeof(ColorTable));


    //                          Pixel Data
    inFile.seekg(fh->StartAddress, ios::beg); //offset to beginning of pixel array
    Pixel px;
    inFile.read((char*)&px, sizeof(Pixel));
    inFile.close();

    char HF[2];
    HF[1] = fh->HeaderField[0];
    HF[2] = fh->HeaderField[1];
    unsigned int SoB = fh->Size_of_BMP;
    unsigned short R1 = fh->Reserved1;
    unsigned short R2 = fh->Reserved2;
    unsigned int SA = fh->StartAddress;

    unsigned int SoH = ih->Size_of_Header;
    signed int  W = ih->Width;
    signed int H = ih->Height;
    unsigned short NoP = ih->Num_of_Planes;
    unsigned short NoB = ih->Num_of_Bits;
    unsigned int CM = ih->CompMethod;
    unsigned int SoR = ih->Size_of_Raw;
    signed int HR = ih->HRes;
    signed int VR = ih->VRes;
    unsigned int NoC = ih->Num_of_Col;
    unsigned int NoIC = ih->Num_of_ICol;

    std::string outFile_name = "E:/Fall 2016 Resources/ECE484W-20160906T232551Z/ECE484W/Milestone 2/Program 2/Program 2/Program 2/new.bmp";

    //write header information to new file
    std::ofstream outFile(outFile_name.c_str(), ios::binary);
    outFile.write(reinterpret_cast<char*>(&HF[1]), sizeof(HF[1]));
    outFile.write(reinterpret_cast<char*>(&HF[2]), sizeof(HF[2]));
    outFile.write(reinterpret_cast<char*>(&SoB), sizeof(SoB));
    outFile.write(reinterpret_cast<char*>(&R1), sizeof(R1));
    outFile.write(reinterpret_cast<char*>(&R2), sizeof(R2));
    outFile.write(reinterpret_cast<char*>(&SA), sizeof(SA));
    outFile.write(reinterpret_cast<char*>(&SoH), sizeof(SoH));
    outFile.write(reinterpret_cast<char*>(&W), sizeof(W));
    outFile.write(reinterpret_cast<char*>(&H), sizeof(H));
    outFile.write(reinterpret_cast<char*>(&NoP), sizeof(NoP));
    outFile.write(reinterpret_cast<char*>(&NoB), sizeof(NoB));
    outFile.write(reinterpret_cast<char*>(&CM), sizeof(CM));
    outFile.write(reinterpret_cast<char*>(&SoR), sizeof(SoR));
    outFile.write(reinterpret_cast<char*>(&HR), sizeof(HR));
    outFile.write(reinterpret_cast<char*>(&VR), sizeof(VR));
    outFile.write(reinterpret_cast<char*>(&NoC), sizeof(NoC));
    outFile.write(reinterpret_cast<char*>(&NoIC), sizeof(NoIC));

    //write color table to new file
    unsigned char d;
    for (int i = 0; i < 1024; i++)
    {
        if (i < 3) d = 0;
        else d = ct.data[i];
        outFile.write(reinterpret_cast<char*>(&d), sizeof(d));
    }

    int brightness_change;
    if(slider1 == 50)
    {
        brightness_change = 0;
    }
    else
    {
        brightness_change = slider1 - 50;
    }

    //write new pixel data to new file
    unsigned char newP;
    unsigned char oldP;
    int check;
    for (int j = 0; j < H; j++)
    {
        for (int i = 0; i < W; i++)
        {
            oldP = px.pix[j*W + i];
            if (slider2 == 50)
            {
                check = oldP + brightness_change;
            }
            else
            {
                check = round(((oldP-127)*(2.5*(slider2))/100)+127+brightness_change);
            }
            if (check < 0)
                newP = 0;
            else if (check > 255)
                newP = 255;
            else
                newP = check;
            outFile.write(reinterpret_cast<char*>(&newP), sizeof(newP));
        }
    }
    outFile.close();
    QPixmap image(QString::fromStdString(outFile_name));
    //ui -> label_2 -> setText(QString::fromStdString(outFile_name));
    ui -> label_4 -> setPixmap(image);
}

void MainWindow::UpdateLabel(const QString text1, const QString text2)
{
    ui->label_5->setText(text1);
    ui->label_6->setText(text2);
}



void MainWindow::on_pushButton_2_clicked()
{
    QString Qfilename = ui -> label_2 -> text();
    std::string filename = Qfilename.toLocal8Bit().constData();
    std::ifstream inFile(filename.c_str(), ios::binary);

    //                          Bitmap File Header
    char* temp = new char[sizeof(BMPFH)];
    // using "read" since binary; input from file placed into temp for each member of structure
    inFile.read(temp, sizeof(BMPFH));
    BMPFH* fh = (BMPFH*)(temp);


    //                          Bitmap Information Header
    temp = new char[sizeof(DIBH)];
    inFile.read(temp, sizeof(DIBH));
    DIBH* ih = (DIBH*)(temp);

    //                          Color Table
    inFile.seekg(54, ios::beg);  //offset to beginning of color table
    ColorTable ct;
    inFile.read((char*)&ct, sizeof(ColorTable));


    //                          Pixel Data
    inFile.seekg(fh->StartAddress, ios::beg); //offset to beginning of pixel array
    Pixel px;
    inFile.read((char*)&px, sizeof(Pixel));
    inFile.close();

    equalizeBMP("E:/Fall 2016 Resources/ECE484W-20160906T232551Z/ECE484W/Milestone 2/Program 2/Program 2/Program 2/equalized.bmp", fh, ih, ct, px);  //create equalized file
}

void MainWindow::on_pushButton_3_clicked()
{
    QString Qfilename = ui -> label_2 -> text();
    std::string filename = Qfilename.toLocal8Bit().constData();
    std::ifstream inFile(filename.c_str(), ios::binary);

    //                          Bitmap File Header
    char* temp = new char[sizeof(BMPFH)];
    // using "read" since binary; input from file placed into temp for each member of structure
    inFile.read(temp, sizeof(BMPFH));
    BMPFH* fh = (BMPFH*)(temp);


    //                          Bitmap Information Header
    temp = new char[sizeof(DIBH)];
    inFile.read(temp, sizeof(DIBH));
    DIBH* ih = (DIBH*)(temp);

    //                          Color Table
    inFile.seekg(54, ios::beg);  //offset to beginning of color table
    ColorTable ct;
    inFile.read((char*)&ct, sizeof(ColorTable));


    //                          Pixel Data
    inFile.seekg(fh->StartAddress, ios::beg); //offset to beginning of pixel array
    Pixel px;
    inFile.read((char*)&px, sizeof(Pixel));
    inFile.close();

    std::string filename2 = getOverlayText();
    std::ifstream inFile2(filename2.c_str(), ios::binary);
    inFile2.seekg(fh->StartAddress, ios::beg); //offset to beginning of pixel array
    Pixel px2;
    inFile2.read((char*)&px2, sizeof(Pixel));
    inFile2.close();
    overlayBMP("E:/Fall 2016 Resources/ECE484W-20160906T232551Z/ECE484W/Milestone 2/Program 2/Program 2/Program 2/overlay.bmp", fh, ih, ct, px, px2);
}

std::string MainWindow::getOverlayText()
{
    QFileDialog dialog(this);
    dialog.setNameFilter(tr("Images (*.bmp)"));
    dialog.setViewMode(QFileDialog::Detail);
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    "C:/",
                                                    tr("Images (*.bmp)"));
    ui -> label_2 -> setText(fileName);
    QPixmap image(fileName);
    std::string file = fileName.toLocal8Bit().constData();
    return file;
}

void MainWindow::equalizeBMP(std::string outFile_name, BMPFH* ofh, DIBH* oih, ColorTable oct, Pixel opx)    // performs histogram equalization
{
    char HF[2];
    HF[1] = ofh->HeaderField[0];
    HF[2] = ofh->HeaderField[1];
    unsigned int SoB = ofh->Size_of_BMP;
    unsigned short R1 = ofh->Reserved1;
    unsigned short R2 = ofh->Reserved2;
    unsigned int SA = ofh->StartAddress;

    unsigned int SoH = oih->Size_of_Header;
    signed int  W = oih->Width;
    signed int H = oih->Height;
    unsigned short NoP = oih->Num_of_Planes;
    unsigned short NoB = oih->Num_of_Bits;
    unsigned int CM = oih->CompMethod;
    unsigned int SoR = oih->Size_of_Raw;
    signed int HR = oih->HRes;
    signed int VR = oih->VRes;
    unsigned int NoC = oih->Num_of_Col;
    unsigned int NoIC = oih->Num_of_ICol;

    //write header information to new file
    std::ofstream outFile(outFile_name.c_str(), ios::binary);
    outFile.write(reinterpret_cast<char*>(&HF[1]), sizeof(HF[1]));
    outFile.write(reinterpret_cast<char*>(&HF[2]), sizeof(HF[2]));
    outFile.write(reinterpret_cast<char*>(&SoB), sizeof(SoB));
    outFile.write(reinterpret_cast<char*>(&R1), sizeof(R1));
    outFile.write(reinterpret_cast<char*>(&R2), sizeof(R2));
    outFile.write(reinterpret_cast<char*>(&SA), sizeof(SA));
    outFile.write(reinterpret_cast<char*>(&SoH), sizeof(SoH));
    outFile.write(reinterpret_cast<char*>(&W), sizeof(W));
    outFile.write(reinterpret_cast<char*>(&H), sizeof(H));
    outFile.write(reinterpret_cast<char*>(&NoP), sizeof(NoP));
    outFile.write(reinterpret_cast<char*>(&NoB), sizeof(NoB));
    outFile.write(reinterpret_cast<char*>(&CM), sizeof(CM));
    outFile.write(reinterpret_cast<char*>(&SoR), sizeof(SoR));
    outFile.write(reinterpret_cast<char*>(&HR), sizeof(HR));
    outFile.write(reinterpret_cast<char*>(&VR), sizeof(VR));
    outFile.write(reinterpret_cast<char*>(&NoC), sizeof(NoC));
    outFile.write(reinterpret_cast<char*>(&NoIC), sizeof(NoIC));

    //count number of pixels with intensity of i
    double countArray[256];
    for (int i = 0; i < 256; i++)
    {
        countArray[i] = std::count(opx.pix, opx.pix+262144, i);
    }

    //calculate probability and cumalative
    double probability[256];
    double cumprob[256];
    for (int i = 0; i < 256; i++)
    {
        probability[i] = countArray[i]/262144.0;
        if (i == 0) cumprob[i] = probability[i];
        else
            cumprob[i] = probability[i] + cumprob[i-1];
    }

    //write color table to new file
    unsigned char d;
    for (int i = 0; i < 1024; i++)
    {
        if (i < 3) d = 0;
        else d = oct.data[i];
        outFile.write(reinterpret_cast<char*>(&d), sizeof(d));
    }

    //write new pixel data to new file
    unsigned char newP;
    unsigned char oldP;
    for (int j = 0; j < H; j++)
    {
        for (int i = 0; i < W; i++)
        {
            oldP = opx.pix[j*W + i];
            if (oldP == 0)
            {
                newP = 0;
            }
            else
            {
                newP = round(cumprob[oldP]*255);
            }
            outFile.write(reinterpret_cast<char*>(&newP), sizeof(newP));
        }
    }
    outFile.close();
    QPixmap image(QString::fromStdString(outFile_name));
    ui -> label_2 -> setText(QString::fromStdString(outFile_name));
    ui -> label_4 -> setPixmap(image);
}

void MainWindow::overlayBMP(std::string outFile_name, BMPFH* ofh, DIBH* oih, ColorTable oct, Pixel opx, Pixel opx2)    // makes overlay
{
    char HF[2];
    HF[1] = ofh->HeaderField[0];
    HF[2] = ofh->HeaderField[1];
    unsigned int SoB = ofh->Size_of_BMP;
    unsigned short R1 = ofh->Reserved1;
    unsigned short R2 = ofh->Reserved2;
    unsigned int SA = ofh->StartAddress;

    unsigned int SoH = oih->Size_of_Header;
    signed int  W = oih->Width;
    signed int H = oih->Height;
    unsigned short NoP = oih->Num_of_Planes;
    unsigned short NoB = oih->Num_of_Bits;
    unsigned int CM = oih->CompMethod;
    unsigned int SoR = oih->Size_of_Raw;
    signed int HR = oih->HRes;
    signed int VR = oih->VRes;
    unsigned int NoC = oih->Num_of_Col;
    unsigned int NoIC = oih->Num_of_ICol;

    //write header information to new file
    std::ofstream outFile(outFile_name.c_str(), ios::binary);
    outFile.write(reinterpret_cast<char*>(&HF[1]), sizeof(HF[1]));
    outFile.write(reinterpret_cast<char*>(&HF[2]), sizeof(HF[2]));
    outFile.write(reinterpret_cast<char*>(&SoB), sizeof(SoB));
    outFile.write(reinterpret_cast<char*>(&R1), sizeof(R1));
    outFile.write(reinterpret_cast<char*>(&R2), sizeof(R2));
    outFile.write(reinterpret_cast<char*>(&SA), sizeof(SA));
    outFile.write(reinterpret_cast<char*>(&SoH), sizeof(SoH));
    outFile.write(reinterpret_cast<char*>(&W), sizeof(W));
    outFile.write(reinterpret_cast<char*>(&H), sizeof(H));
    outFile.write(reinterpret_cast<char*>(&NoP), sizeof(NoP));
    outFile.write(reinterpret_cast<char*>(&NoB), sizeof(NoB));
    outFile.write(reinterpret_cast<char*>(&CM), sizeof(CM));
    outFile.write(reinterpret_cast<char*>(&SoR), sizeof(SoR));
    outFile.write(reinterpret_cast<char*>(&HR), sizeof(HR));
    outFile.write(reinterpret_cast<char*>(&VR), sizeof(VR));
    outFile.write(reinterpret_cast<char*>(&NoC), sizeof(NoC));
    outFile.write(reinterpret_cast<char*>(&NoIC), sizeof(NoIC));


    //write color table to new file
    unsigned char d;
    for (int i = 0; i < 1024; i++)
    {
        if (i < 3) d = 0;
        else d = oct.data[i];
        outFile.write(reinterpret_cast<char*>(&d), sizeof(d));
    }


    //write pixel data to new file
    unsigned char oldP;
    unsigned char oldP_overlay;
    unsigned char newP;
    for (int j = 0; j < H; j++)
    {
        for (int i = 0; i < W; i++)
        {
            oldP = opx.pix[j*W + i];
            oldP_overlay = opx2.pix[j*W + i];
            if (oldP_overlay < 150)
            {
                newP = 255;
            }
            else
            {
                newP = oldP;
            }
            outFile.write(reinterpret_cast<char*>(&newP), sizeof(newP));
        }
    }
    outFile.close();
    QPixmap image(QString::fromStdString(outFile_name));
    ui -> label_2 -> setText(QString::fromStdString(outFile_name));
    ui -> label_4 -> setPixmap(image);
}


