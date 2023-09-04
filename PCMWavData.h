#ifndef _PCMWAVDATA_H_
#define _PCMWAVDATA_H_ 

#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>

class PCMWavData {

private:
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
	int subchunk_size;
	std::vector<short> dane;

public:        
        PCMWavData() {
        }

		PCMWavData(char* _chunk_id,
			char* _chunk_wav,
			char* _chunk_fmt,
			char* _chunk_data,
			unsigned long _size,
			int _format_tag,
			short _channel,
			short _audio_format,
			short _block_align,
			int _byte_rate,
			int _sample_rate,
			short _bits_per_sample,
			int _subchunk_size,
			std::vector<short> _dane)
		{	
				std::strcpy(this->chunk_id, _chunk_id);
				std::strcpy(this->chunk_wav, _chunk_wav);	
				std::strcpy(this->chunk_fmt, _chunk_fmt);
				std::strcpy(this->chunk_data, _chunk_data);
				this->size = _size;
				this->format_tag = _format_tag;
				this->channels = _channel;
				this->audio_format = _audio_format;
				this->block_align = _block_align;
				this->byte_rate = _byte_rate;
				this->sample_rate = _sample_rate;
				this->bits_per_sample = _bits_per_sample;
				this->subchunk_size = _subchunk_size;
				this->dane = _dane;
        }

		~PCMWavData() {
        }

		int getSampleRate()
		{
			return this->sample_rate;
		}

		int getSubchunkSize()
		{
			return this->subchunk_size/2;
		}
		std::vector<short> getVectorDane()
		{
			return this->dane;
		}
		
		/// <summary>
		/// Funkcja zapisuj¹ca dane do pliku wav 
		/// </summary>
		/// <param name="dane">dwuwymiarowy wektor</param>
		/// <param name="okno_size">Okno w liczbie próbek</param>

		void zapiszWav(std::vector< std::vector<double> > dane, int okno_size, std::string path_to_out_file)
		{
			int liczbaProbekWindow_size = (sample_rate * okno_size * 0.001);
		
			std::ofstream fout(path_to_out_file, std::ios::out | std::ios::binary);

			fout.write((const char *) &chunk_id[0], sizeof(char));
			fout.write((const char *) &chunk_id[1], sizeof(char));
			fout.write((const char *) &chunk_id[2], sizeof(char));
			fout.write((const char *) &chunk_id[3], sizeof(char));
	
			fout.write((const char *) &size, sizeof(unsigned long));

			fout.write((const char *) &chunk_wav[0], sizeof(char));
			fout.write((const char *) &chunk_wav[1], sizeof(char));
			fout.write((const char *) &chunk_wav[2], sizeof(char));
			fout.write((const char *) &chunk_wav[3], sizeof(char));

			fout.write((const char *) &chunk_fmt[0], sizeof(char));
			fout.write((const char *) &chunk_fmt[1], sizeof(char));
			fout.write((const char *) &chunk_fmt[2], sizeof(char));
			fout.write((const char *) &chunk_fmt[3], sizeof(char));
	
			fout.write((const char *) &format_tag, sizeof(int));
			fout.write((const char *) &audio_format, sizeof(short));
			fout.write((const char *) &channels, sizeof(short));
			fout.write((const char *) &sample_rate, sizeof(int));
			fout.write((const char *) &byte_rate, sizeof(int));	
			fout.write((const char *) &block_align, sizeof(short));	
			fout.write((const char *) &bits_per_sample, sizeof(short));	
	
			fout.write((const char *) &chunk_data[0], sizeof(char));
			fout.write((const char *) &chunk_data[1], sizeof(char));
			fout.write((const char *) &chunk_data[2], sizeof(char));
			fout.write((const char *) &chunk_data[3], sizeof(char));

			fout.write((const char *) &subchunk_size, sizeof(int));
			
			int ilosc_probek_w_nagraniu=dane.size();
			int okno = 0;
			short tmp=0;
						
			for(int okno=0; okno<dane.size(); okno++)
			{
				for(int j=0; j<liczbaProbekWindow_size; j++)
				{
					tmp = dane[okno][j];
					fout.write((const char*) &tmp, sizeof(short));
				}
			}
			fout.close();
		}
};

#endif