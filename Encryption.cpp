

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <string>
#include <fstream>
#include <vector>



char codebook[] = {
'a','A','b','B','c','C','d','D','e','E','f','F','g','G','h','H','i','I' ,
'j','J','k','K','l','L','m','M','n','N','o','O','p','P','q','Q','r','R' ,
's','S','t','T','u','U','v','V','w','W','x','X','y','Y','z','Z','0','1' ,
'2','3','4','5','6','7','8','9','.',' '
};

typedef unsigned char uchar;
void encrypt(std::string&, int);
void decrypt(std::string&, int);
void copy_string(std::string&, char*);
void read_ppm_img(uchar*& image, std::string filename, int& w, int& h);
void write_ppm_img(uchar* image, std::string filename, int w, int h);
int get_index(char c);
void write_stego_img(uchar* img, int k, int w, int h, std::string msg);
void read_stego_img(uchar* img, int& k, int w, int h);

int k = 0;
const int CBL = 64;


int main(int argc, char* argv[])
{
	bool encrypt1 = false; bool decrypt1 = false;
	std::string plaintext;
	std::string ciphertext;
	char* index_of_plaintext = 0;
	char* index_of_ciphertext = 0;
	std::string f;


	for (int i = 1; i < argc; i++)
	{
		if (std::strcmp(argv[i], "-p") == 0)
			index_of_plaintext = argv[i + 1];
		if (std::strcmp(argv[i], "-k") == 0)
			k = std::atoi(argv[++i]);
		if (std::strcmp(argv[i], "-E") == 0)
			encrypt1 = true;
		if (std::strcmp(argv[i], "-D") == 0)
			decrypt1 = true;
		if (std::strcmp(argv[i], "-C") == 0)
			index_of_ciphertext = argv[i + 1];
		if (std::strcmp(argv[i], "-f") == 0)
			f = argv[i + 1];

	}

	
	uchar *img = nullptr;
	int w = 0, h = 0; 
	read_ppm_img(img, f, w, h);
	
	
	
	




	if (encrypt1 == true)
	{
		copy_string(plaintext, index_of_plaintext);
		encrypt(plaintext, k);
		std::cout << std::endl << plaintext << std::endl;


		write_stego_img(img, k, h, w, plaintext);
		std::string filename_crypto = "crypto_" + f;
		write_ppm_img(img, filename_crypto, w, h);
		std::cout << std::endl << "Image saved to: " << filename_crypto <<  std::endl;
		
	}

	if (decrypt1 == true)
	{
		read_stego_img(img, k, w, h);
	
		int index_codebook;

		for (int i = 6; i < w * h * 3; i+=3)
		{
			uchar mask1 = 3;
			uchar e_message = 0;
			e_message = img[i] & mask1;
			e_message = ((img[i+1] & mask1) << 2) | e_message;
			e_message = ((img[i+2] & mask1) << 4) | e_message;
			index_codebook = e_message;
			std::cout << index_codebook;
		}
		
		//copy_string(ciphertext, index_of_ciphertext);
		decrypt(ciphertext, k);
	}
	
	
	//for (int i = 0; i < w * h * 3; i++)
		//std::cout << (int) img[i] << " ";
	
	if (img)
		delete[] img;
	img = nullptr;


	
	return 0;
}


void encrypt(std::string& plaintext, int k)
{
	std::cout << "The plaintext was: " << plaintext << std::endl;
	std::cout << "The ciphertext is: ";
	for (int i = 0; i < plaintext.length(); i++)
	{
		std::string c = plaintext;
		for (int j = 0; j < CBL; j++)
		{
			if (c[i] == codebook[j])
			{
				int shift = (j + k) % CBL;
				plaintext[i] = codebook[shift];


			}

		}
	}
}

void decrypt(std::string& ciphertext, int k)
{
	std::cout << "The ciphertext was: " << ciphertext << std::endl;
	std::cout << "The plaintext is: ";
	for (int i = 0; i < ciphertext.length(); i++)
	{
		for (int j = 0; j < CBL; j++)
		{
			if (ciphertext[i] == codebook[j])
			{
				if ((j - k) >= 0)
				{
					std::cout << codebook[(j - k) % CBL];
				}
				else
					std::cout << codebook[((((j - k) % CBL) + CBL) % CBL)];
			}
		}
	}
}



void copy_string(std::string& s, char* text)
{
	s = s.append(text);
}


void read_ppm_img(uchar*& image, std::string filename, int& w, int& h)
{
	std::ifstream inFile;
	inFile.open(filename);
	std::string version;
	int maxvalue;
	inFile >> version >> w >> h >> maxvalue;
	int size = h * w * 3;
	image = new uchar[size];

	int val = 0;
	for (int i = 0; i < size; i++) {
		val = 0;
		inFile >> val;
		image[i] = val;
	}

	

	inFile.close();
}


void write_ppm_img(uchar* image, std::string filename, int w, int h)
{
	std::ofstream outFile;
	outFile.open(filename);
	outFile << "P3" << std::endl;
	outFile << w << " ";
	outFile << h << std::endl;;
	outFile << "255" << std::endl;
	for (int i = 0; i < h * w * 3; i++)
		outFile << (int) image[i] << " ";
}


int get_index(char c)
{
	int index;
	for (int i = 0; i < CBL; i++)\
		if (c == codebook[i])
			index = i;
	return index;
}


void write_stego_img(uchar* img, int k, int w, int h, std::string msg)
{
	
	uchar mask1 = 3;
	uchar length_char = (unsigned char) msg.length();
	uchar mask2 = ~mask1;
	
	img[0] = (img[0] & mask2) | (length_char & mask1);// red
	uchar length_char_shift2 = length_char >> 2;
	img[1] = (img[1] & mask1) | (length_char_shift2 & mask1); // green
	uchar length_char_shift4 = length_char >> 4;
	img[2] = (img[2] & mask1) | (length_char_shift4 & mask1); // blue
	
	unsigned char k_char = (unsigned char)k;
	img[3] = (img[3] & mask2) | (k_char & mask1); // red
	unsigned char k_char_shift2 = k_char >> 2;
	img[4] = (img[4] & mask2) | (k_char_shift2 & mask1);
	unsigned char k_char_shift4 = k_char >> 4;
	img[5] = (img[5] & mask2) | (k_char_shift4 & mask1);


		for (int i = 0; i < msg.length(); i++)
		{
		
			uchar c= (unsigned char)get_index(msg[i]);

			for (int j = 6; j < w * h * 3; j += 3)
			{
				
				img[j] = (img[j] & mask2) | (c & mask1);
			}
			for (int j = 7; j < w * h * 3; j += 3)
			{
				uchar c_shift2 = c >> 2;
				img[j] = (img[j] & mask2) | (c_shift2 & mask1);
				
			}
			for (int j = 8; j < w * h * 3; j += 3)
			{
				uchar c_shift4 = c >> 4;
				img[j] = (img[j] & mask2) | (c_shift4 & mask1);
			}
		}

}


void read_stego_img(uchar* img, int& k, int w, int h)
{
	uchar mask1 = 0x03;
	uchar length_char = 0;
	length_char = img[0] & mask1;
	length_char = ((img[1] & mask1) << 2) | length_char;
	length_char = ((img[2] & mask1) << 4) | length_char;
	int length = (int) length_char;

	uchar k_char = 0;
	k_char = img[3] & mask1;
	k_char = ((img[4] & mask1) << 2) | k_char;
	k_char = ((img[5] & mask1) << 2) | k_char;
	k = k_char;


	for (int i = 6; i < w * h * 3; i += 3)
	{
		int index_codebook;
		uchar mask1 = 3;
		uchar e_message = 0;
		e_message = img[i] & mask1;
		e_message = ((img[i + 1] & mask1) << 2) | e_message;
		e_message = ((img[i + 2] & mask1) << 4) | e_message;
		index_codebook = (int) e_message;

		std::cout << index_codebook;
	}

	//for (int i = 0; i < length; i++)
	//	for (int j = 6; j < w * h * 3; j += 3)
	//	{
	//		uchar c = 0;
	//		c = img[j] & mask1;
	//		c = ((img[j + 1] & mask1) << 2) | c;
	//		c = ((img[j + 2] & mask1) << 4) | c;
	//		img[j] = c;

	//	}
		

	/*for (int i = 6; i < w * h * 3; i += 3)
	{
		
		uchar e_message = 0;
		img [i]= img[i] & mask1;
		img[i] = ((img[i + 1] & mask1) << 2) | img[i];
		img [i] = ((img[i + 2] & mask1) << 4) | img[i];*/
		
		
		
	}
	
	
	
