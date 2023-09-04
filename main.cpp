#include <iostream>
#include <exception>
#include <fstream>
#include "PCMWavData.h"
#include "globalFunction.h"

#define _CRT_SECURE_NO_WARNINGS

/*
1. czas trwania okna (ms)
3. wartosc PLR (procenty)
4. MBLL (pakiety)
5. sciezka pliku wejsciowego
6. sciezka pliku wyjsciowego
*/
/*
	framing(int window_size, PCMWavData objWav)
	gilbertModel(double PLR, int MBLL, int liczbaOkien)
	convolutionModeldWavFile(szererg_gilberta, dane_framing, window_size_in_ms, obj);
	void zapiszWav(std::vector< std::vector<double> > dane, int okno_size, string pathToOutFile)	
*/
int main(int argc, char* argv[])
{
	int window_size_in_ms = 0;
	int PLR = 0;
	int MBLL = 0;
	
	std::string in_file;
	std::string out_file;

	if(argc != 6)
	{
	
		std::cerr<<"Zle argumenty"<<std::endl;
		//return 0;
	}

		window_size_in_ms = atoi(argv[1]);
	//window_size_in_ms = 20;
		PLR = atoi(argv[2]);
	//PLR = 10;
		MBLL = atoi(argv[3]);
	//MBLL = 5;
		in_file = toPosixNotation((std::string)(argv[4]));
	//in_file = "\\1_M1_target30s_A.wav";
		out_file = toPosixNotation((std::string)(argv[5]));
	//out_file = "1_M1_target30s_A_20_10_5.wav";	

	//std::cout<< window_size_in_ms <<std::endl;
	//std::cout<< PLR <<std::endl;
	//std::cout<< MBLL <<std::endl;
	//std::cout<< in_file <<std::endl;
	//std::cout<< out_file <<std::endl;	

	PCMWavData obj = loadWaveFile(in_file);

	std::vector< std::vector<double> > dane_framing = framing(window_size_in_ms, obj);
	std::vector<short> szererg_gilberta= gilbertModel(PLR, MBLL, dane_framing.size());
	std::vector<double> y = walidacjaGilbertModel(szererg_gilberta);
	std::vector< std::vector<double> > convolvedDane = convolutionModeldWavFile(szererg_gilberta, dane_framing, window_size_in_ms, obj);
	obj.zapiszWav(convolvedDane, window_size_in_ms, out_file); 
	int i = 0;

	std::cout<<" PLR "<<y[0]<<std::endl;
	std::cout<<" srMBLL " <<y[1]<<std::endl;

	//system("PAUSE");
	return 0;
}

