#pragma once
#include <vector>
#include <QColor>
#include <sstream> // for StringStream
#include <iomanip> // to Set precision
# include <tuple>


#define assignPair(result,one,two) auto one = result.first; auto two = result.second;

#define iterTriangle(i,j,max) for(int i = 0; i < max; i++) for(int j = 0; j < i; j++)
#define iterRectNoEq(i,j,max) for(int i = 0; i < max; i++) for(int j = 0; j < max; j++) if ( i != j)
#define iterLin(i,max) for(int i = 0; i < max; i++)
#define iterList(ref, list) for(auto ref = list.begin(); ref != list.end(); ref++)

using namespace std;

typedef vector <vector<double>> dataMatrix;
typedef vector<double> dataVector;
typedef vector<int> intVector;
typedef vector<string> labelVector;
typedef vector<QColor> colorVector;

namespace Times{
	// attributes
		constexpr double hour = 1;
		constexpr double day = 24 * hour;
		constexpr double week = day * 7;
		constexpr double year = day * 3600;

		constexpr double minute = hour / 60;
		constexpr double second = minute / 60;
	//methods
		string toDate(double time);
		string toHour(double time);
		string full(int num);
};

// method
string toString(double num);
