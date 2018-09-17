#include <iostream>
#include <dirent.h>
#include <string>
#include <locale>
#include <sstream>
#include <vector>
#include <map>
#include <fstream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <cstdlib>

using namespace std;

typedef vector<string> Tokens;

typedef struct _Yolo {
	int cls;
	float x;
	float y;
	float w;
	float h;	
} Yolo;

DIR *dir;
struct dirent *ent;

vector<string> txt_files;
map<string, vector<string>> txt_Content; 
multimap<string, pair<int, vector<float>>> converted_Content;
multimap<string, Yolo> yolo_label;

string def_png_dir = "/home/vadas/data_object_image_2/training/image_2/";

void get_labels(string dirName) {
	if ((dir = opendir(dirName.c_str())) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			string str = ent->d_name;
			if(str.find(".txt", 0) != string::npos) { 
				txt_files.push_back(str);
			}
		}
		closedir(dir);
	} else {
		cout << "not found";
	}
}

void read_Txt(string txtName) {

	ifstream txtFile;
	
	txtFile.open(txtName.c_str());

	if (!txtFile.is_open()) {
		cerr << txtName << " file not opend" << endl;
		return ;
	}

	vector<string> content;

	while (!txtFile.eof()) {
		string line;
		getline(txtFile, line);

		content.push_back(line);
	}
		
	txt_Content.insert(make_pair(txtName, content));

}

int cls_to_num(string str_cls) {
	if(!str_cls.compare("Car")) {
		return 0;
	}
	else if(!str_cls.compare("Pedestrian")) {
		return 1;
	}
	else if(!str_cls.compare("Cyclist")) {
		return 2;
	}
	else if(!str_cls.compare("Tram")) {
		return 3;
	}
	
	return -1;
}

void convert_Coord() {
	for (auto x : txt_Content) {
		for (auto iter = x.second.begin(); iter != x.second.end(); iter++) {
			vector<float> tokens;
			istringstream iss(*iter);
			string tmp;
			int cls = -1;
			while (iss >> tmp) {
				try {
					tokens.push_back(stof(tmp));
				}
				catch (const exception& e)
				{
					cls = cls_to_num(tmp);
				}
			}	

			if (cls != -1)
				converted_Content.insert(make_pair(x.first, make_pair(cls, tokens)));	
		}
	}

	int count = 0;
	for (auto x : converted_Content) {
		int idx = x.first.find("txt", 0);
		string png_dir = x.first;		
		png_dir.replace(idx, 3, "png");		
		png_dir = def_png_dir + png_dir;

		cv::Mat img = cv::imread(png_dir, 1);
		Yolo coords;
		coords.cls = (x.second).first;
		coords.x = ((x.second).second[3] + (x.second).second[5]) / 2.0;
		coords.x /= img.cols; 
		coords.y = ((x.second).second[4] + (x.second).second[6]) / 2.0;
		coords.y /= img.rows; 
		coords.w = ((x.second).second[5] - (x.second).second[3]) / img.cols;
		coords.h = ((x.second).second[6] - (x.second).second[4]) / img.rows;

		yolo_label.insert(make_pair(x.first, coords));
		if(count % 5000 == 0)
			cout << count << endl;
		count++;
	}
	
	const int dir_err = system("mkdir -p ./converted");	

	for (auto x : yolo_label) {
		ofstream outfile;
		outfile.open("./converted/"+x.first, ios_base::app);
		outfile << (x.second).cls << " " << (x.second).x << " " << (x.second).y << " " << (x.second).w << " " << (x.second).h << endl;
	}
}

int main(int argc, char **argv) {

	get_labels(argv[1]);

	for(auto file : txt_files) {
		read_Txt(file);
	}

	for (auto &x : txt_Content) {
		for(auto iter = x.second.begin(); iter != x.second.end();) {
			if((*iter).find("DontCare",0) != string::npos) {
				x.second.erase(iter);
			}	
			else if((*iter).find("Sitters", 0) != string::npos) {
				x.second.erase(iter);
			}
			else if((*iter).find("Misc", 0) != string::npos) {
				x.second.erase(iter);
			}
			else if(int idx = (*iter).find("Van", 0) != string::npos) {
				(*iter).replace(idx-1, 3, "Car");
			}
			else if(int idx = (*iter).find("Truck", 0) != string::npos) {
				(*iter).replace(idx-1, 5, "Car");
			}
			else {
				iter++;
			}
		}
	}

	convert_Coord();
	
	return 0;
}
