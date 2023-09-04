#include "globalFunction.h"

void saveWaveFile(std::string wavPathToFile, PCMWavData obj)
{
}

PCMWavData loadWaveFile(std::string wavPath) {
    
   	char chunk_id[5];
	char chunk_wav[5];
	char chunk_fmt[5];
	char chunk_data[5];
    unsigned long size;
	int format_tag;
    short channels;
	short audio_format;
	short block_align;
	int byte_rate;
	int sample_rate;
	short bits_per_sample;
	int subchunk_size;	// number of frames in audio * 2
    	
	std::ifstream fin(wavPath, std::ios::in | std::ios::binary);
	if(!fin)
	{
		//std::cout<<"blad pliku"<<std::endl;		
	}
	fin.read((char*) &chunk_id[0], sizeof(char));
	fin.read((char*) &chunk_id[1], sizeof(char));
	fin.read((char*) &chunk_id[2], sizeof(char));
	fin.read((char*) &chunk_id[3], sizeof(char));
	chunk_id[4]='\0';
	
	fin.read((char*) &size, sizeof(unsigned long));

	fin.read((char*) &chunk_wav[0], sizeof(char));
	fin.read((char*) &chunk_wav[1], sizeof(char));
	fin.read((char*) &chunk_wav[2], sizeof(char));
	fin.read((char*) &chunk_wav[3], sizeof(char));	
	chunk_wav[4]='\0';

	fin.read((char*) &chunk_fmt[0], sizeof(char));
	fin.read((char*) &chunk_fmt[1], sizeof(char));
	fin.read((char*) &chunk_fmt[2], sizeof(char));
	fin.read((char*) &chunk_fmt[3], sizeof(char));
	chunk_fmt[4]='\0';
	
	fin.read((char*) &format_tag, sizeof(int));	
	fin.read((char*) &audio_format, sizeof(short));
	fin.read((char*) &channels, sizeof(short));
	fin.read((char*) &sample_rate, sizeof(int));
	fin.read((char*) &byte_rate, sizeof(int));
	fin.read((char*) &block_align, sizeof(short));
	fin.read((char*) &bits_per_sample, sizeof(short));

	fin.read((char*) &chunk_data[0], sizeof(char));
	fin.read((char*) &chunk_data[1], sizeof(char));
	fin.read((char*) &chunk_data[2], sizeof(char));
	fin.read((char*) &chunk_data[3], sizeof(char));
	chunk_data[4]='\0';

	fin.read((char*) &subchunk_size, sizeof(int));

	int ilosc_probek = subchunk_size/2;	
	short tmp;
	std::vector<short> muzyka(ilosc_probek);

	int index=0;
	for(int jj=0; jj<ilosc_probek; jj++)
	{
		fin.read((char*) &tmp, sizeof(short));
		muzyka[index] = tmp;
		index++;
	}
	fin.close();

	PCMWavData objWav(chunk_id, chunk_wav, chunk_fmt, chunk_data, size, format_tag, channels, audio_format, block_align, byte_rate, sample_rate, bits_per_sample, subchunk_size, muzyka);
	
	return objWav;
    }

/// <summary>
/// Framing and windowing function.
/// </summary>
/// <param name="window_size">Window size - milisekunds</param>
/// <param name="over_lap">Overlap window - milisekunds</param>
/// <param name="data">PCM data</param>
/// <param name="size_data">number of samples</param>
/// <param name="sampleRate">sampling frequency</param>
/// <returns>std::vector< std::vector<short> > return vector with shape: number of frames in window x number of windows</returns>

std::vector< std::vector<double> > framing(int window_size, PCMWavData objWav)
{
	/// przelicz window_size na liczbe probek
	int getSampleRate = objWav.getSampleRate();
	int liczbaProbekWindow_size = (getSampleRate * window_size * 0.001);
	long subChunk = objWav.getSubchunkSize();
	int liczbaOkienek = subChunk/liczbaProbekWindow_size; 

	//std::vector< std::vector<double> > data_framed(liczbaProbekWindow_size, std::vector<double>(liczbaOkienek));
	std::vector< std::vector<double> > data_framed(liczbaOkienek, std::vector<double>(liczbaProbekWindow_size));
	std::vector<short> dane = objWav.getVectorDane(); 

	int okienko = 0;
	int licznik = 0;

	for(int i=liczbaProbekWindow_size; i<subChunk; i=i+liczbaProbekWindow_size)
	{
		for(int j=0; j<liczbaProbekWindow_size; j++)
		{
			data_framed[okienko][j] = dane[licznik];
			licznik++;
		}
		okienko++;
	}

	int uuu= 0 ;

	return data_framed;
}

/// <summary>
/// Function implements number Gilbert Model
/// </summary>
/// <param name="PLR">probability of loss %</param>
/// <param name="MBLL">Mean Burst Lost Length</param>
/// <param name="liczbaOkien">number of frames in audio</param>
/// <returns> vector 0 - packet delivered,
///					 1 - packet loss</returns>

std::vector<short> gilbertModel(int PLR, int MBLL, int liczbaOkien)
{
	double b = 1.0 -double (1.0/MBLL);
	double Pz = (double)PLR/100.0;
	double Pdz = 1.0 - ((1.0 - (double)b)/(1.0 - (double)Pz)) * (double)Pz;
	//double Pdz = (double)Pz/(double)((double)MBLL*(1.0-(double)Pz));

	std::vector<short> wynik(liczbaOkien);

	// start model with true state (packet delivered)
	wynik[0] = 0;

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(0,1);

	for(int i=1; i<liczbaOkien; i++)
	{
		if(wynik[i-1]==0)
		{
			if(dis(gen) <= Pdz)
			{
				wynik[i] = 0;
			}
			else
			{
				wynik[i] = 1;
			}
		}
		else
		{
			if(dis(gen) <= b)
			{
				wynik[i] = 1;
			}
			else
			{
				wynik[i] = 0;
			}
		}
	}	

	
	return wynik;
}

std::vector<double> walidacjaGilbertModel(std::vector<short> w)
{
		std::vector<short> burst;

	/// validation

	int j = 0;
	int l = 0;

	for(int i=0; i<w.size(); i++)
	{
		if(w[i] == 1)
		{
			j = i + 1;
			l=1;
			while(j<w.size() && w[j] == 1)
			{
				l = l + 1;
				j = j + 1;
			}
			if(l>1)
			{
				burst.push_back(l);
			}
			i = j - 1;
		}
	
		i = i + 1;	
	}
	/// Mean Burst Lost Length estimation
	
	int sumaBurst = 0;	

	for(int j=0; j<burst.size(); j++)
	{
		sumaBurst = sumaBurst + burst[j];
	}
	double srMBLL = (double) sumaBurst/(double) burst.size();
	/// number of loss packets
	
	int suma = 0;

	for(int i=0; i<w.size(); i++)
	{
		suma = suma + w[i];
	}
	
	double PLR = ((double)suma/(double)w.size())*100;

	std::vector<double> wyniki;
	wyniki.push_back(PLR);
	wyniki.push_back(srMBLL);

	return wyniki;
}

std::vector< std::vector<double> > convolutionModeldWavFile(std::vector<short> gilbert, std::vector< std::vector<double> > dane, int rozmiar_okna, PCMWavData obj)
{
	int liczbaProbekWindow_size = (obj.getSampleRate() * rozmiar_okna * 0.001);

	for(int i=0; i<gilbert.size(); i++)
	{
		if(gilbert[i] == 1)
		{
			for(int j=0; j<liczbaProbekWindow_size; j++)
			{
					dane[i][j] = 0;
			}							
		}
	}
	return dane;
}

std::string toPosixNotation(std::string path)
{
	for(int i=0; i<path.size(); i++)
	{
		if(path[i] == '\\')
		{
			path[i] = '\\\\';
		}			
	}		
	return path;
}
