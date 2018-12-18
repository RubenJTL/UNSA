#include <iostream>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include "CImg.h"
#include "Utils/Utils.h"

using namespace std;
using namespace cimg_library;

int getTam(string fileNames){
	string command = "cat " + fileNames + " | wc -l > tam";
	system(command.c_str());
	ifstream archivo("tam");
	int res = 0;
	archivo>>res;
	return res;
}

string getRandomImg(string fileNames){
	int max = getTam(fileNames);
	ifstream names(fileNames.c_str());
	int random = rand() % max;
	string file = "";
	for(int i = 0; i < random; i++){
		names>>file;
	}
	return file;
}

void copyImg(string img){
	string command = "cp " + img + " .";
	system (command.c_str());
}

void deleteFile(string file){
	string command = "rm " + file;
	system(command.c_str());
}

int main(int argc, char ** argv){
	srand(time(NULL));
	
	string imgSecret = "";
	string imgStego1 = "";
	string imgStego2 = "";
	string command = "";
	string out = "";
	string extension = "";
	CImg<int> stegoImg1;
	CImg<int> stegoImg2;
	CImg<int> outstegoImg1;
	CImg<int> outstegoImg2;
	CImg<int> h11;
	CImg<int> h12;
	CImg<int> h21;
	CImg<int> h22;
	
	/*(stegoImg1).display();
	(stegoImg2).display();
	(h1,h2).get_append('c').display_graph();
	h1.save("h1");
	h2.save("h2");
	h1 = CImg<int>("h1");
	h2 = CImg<int>("h2");
	(h1,h2).get_append('c').display_graph();
	//main_dis.display(h1);
	*/


	stegoImg1 = CImg<int>("mono.jpg");
	stegoImg2 = CImg<int>("paisaje.jpg");
	h11 = stegoImg1.get_histogram(256);
	h21 = stegoImg2.get_histogram(256);
	outstegoImg1 = CImg<int>("outmono.bmp");
	outstegoImg2 = CImg<int>("outpaisaje.bmp");
	h12 = outstegoImg1.get_histogram(256);
	h22 = outstegoImg2.get_histogram(256);
	(h11,h12).get_append('c').display_graph();
	(h21,h22).get_append('c').display_graph();

/*
	
	for(int i = 0; i < iteraciones; i++){
		imgSecret = getRandomImg(SECRET_NAMES);
		imgStego1 = getRandomImg(STEGO_NAMES);
		imgStego2 = getRandomImg(STEGO_NAMES);
		copyImg(SECRET_DIR + imgSecret);
		copyImg(STEGO_DIR + imgStego1);
		copyImg(STEGO_DIR + imgStego2);
		command = "./run -c " + imgSecret + " " + imgStego1 + " " + imgStego2 + " key" + to_string(i);
		system(command.c_str());
		tie(imgStego1,extension) = getExtension(imgStego1);
		tie(imgStego2,extension) = getExtension(imgStego2);
		imgStego1 += ".bmp";
		imgStego2 += ".bmp";
		stegoImg1 = CImg<int>(imgStego1.c_str());
		stegoImg2 = CImg<int>(imgStego2.c_str());
		h11 = stegoImg1.get_histogram(256);
		h21 = stegoImg2.get_histogram(256);
		out = HISTOGRAM_DIR + to_string(i) + "_" + imgStego1 + ".hy";
		//h11.save(out.c_str());
		out = HISTOGRAM_DIR + to_string(i) + "_" + imgStego2 + ".hy";
		//h21.save(out.c_str());
		imgStego1 = "out" + imgStego1;
		imgStego2 = "out" + imgStego2;
		outstegoImg1 = CImg<int>(imgStego1.c_str());
		outstegoImg2 = CImg<int>(imgStego2.c_str());
		h12 = outstegoImg1.get_histogram(256);
		h22 = outstegoImg2.get_histogram(256);
		(h11,h12).get_append('c').display_graph();
		(h21,h22).get_append('c').display_graph();
		out = HISTOGRAM_DIR + to_string(i) + "_" + imgStego1 + ".hy";
		//h12.save(out.c_str());
		out = HISTOGRAM_DIR + to_string(i) + "_" + imgStego2 + ".hy";
		//h22.save(out.c_str());
	}
*/
}

