#pragma once
using namespace std;
//include <boost/filesystem.hpp>

void write();

string getTime();

class Saver {

	string defaultLoc = "./data";
	bool date;
	string folder;
	string name;

	string getTime();

	string getDate();

public:
	Saver(string folder, bool date = true);
	const string getPath(string fileName);
	bool save(string message,string filename);

	static string doubleNumber(int num);


};