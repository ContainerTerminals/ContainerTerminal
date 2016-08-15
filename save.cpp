#include <iostream>
#include <fstream>
#include<direct.h>
#include <ctime>
#include <sstream>
#include <save.h>



using namespace std;


// first folder creation
// save in folder
// date generation -> folder
// csv implement
// convenient library



string Saver::getTime() {
	time_t t = time(0);   // get time now
	struct tm * now = localtime(&t);
	ostringstream stream;
	stream << doubleNumber(now->tm_hour) << doubleNumber(now->tm_min) << doubleNumber(now->tm_sec);
	return stream.str();

	}

string Saver::getDate() {
	time_t t = time(0);   // get time now
	struct tm * now = localtime(&t);
	ostringstream stream;
	stream << doubleNumber(now->tm_mon + 1) << doubleNumber(now->tm_mday) ;
	return stream.str();
}

 Saver::Saver(string folder, bool useDate)
	{ this->date = useDate;
	this->folder = folder;
	string date  = getDate();
	string time = getTime();
	if (useDate)
		this->name = this-> defaultLoc + "/" + folder + "/" + date + "/" + time + "/" ;
	else this->name = this->defaultLoc + "/" + folder;
	
	mkdir(defaultLoc.c_str());
	mkdir((defaultLoc + "/" + folder ).c_str());
	
	if (useDate)
		{mkdir((defaultLoc + "/" + folder + "/" + date).c_str());
		mkdir( (defaultLoc + "/" + folder + "/" + date + "/" + time).c_str());}

}

const string Saver::getPath(string fileName){
	return (name + "/" + fileName).c_str();}

bool Saver::save(string message, string filename)
{
	
	ofstream writer;
	writer.open(getPath(filename));
	writer << message ;
	writer.close();

	return true;
}

string Saver::doubleNumber(int num){
	string s = to_string(num);
	return string(2 - s.length(), '0') + s;}

void write() {
	string s = "tester";
	Saver saver = Saver(s);
	saver.save("hey this is a test", "testfile.txt");
}


