#ifndef _GLOBALFUNCTION_H_
#define _GLOBALFUNCTION_H_

#include "PCMWavData.h"
#include <iostream>
#include <complex>
#include <valarray>
#include <vector>
#include <fstream>
#include <vector>
#include <math.h>
#include <conio.h>
#include <time.h>
#include <random>

const double PI = 3.141592653589793238460;
 
typedef std::complex<double> Complex;
typedef std::valarray<Complex> CArray;
typedef std::valarray<std::valarray<double> > va2d;

void saveWaveFile(std::string wavPathToFile, PCMWavData obj);
PCMWavData loadWaveFile(std::string wavPath);
void fft_funkcja(CArray& x);
void ifft(CArray& x);
//int windowing(int window_size, int over_lap, unsigned long size_data, long sampleRate);
std::vector< std::vector<double> > framing(int window_size, PCMWavData objWav);
std::vector<short> gilbertModel(int PLR, int MBLL, int liczbaOkien);
std::vector<double> walidacjaGilbertModel(std::vector<short> model);
std::vector< std::vector<double> > convolutionModeldWavFile(std::vector<short> gilbert, std::vector< std::vector<double> > dane, int, PCMWavData);
std::string toPosixNotation(std::string);

#endif