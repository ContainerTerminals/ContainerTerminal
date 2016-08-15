#pragma once

#include <QMainWindow>
#include <QApplication>
#include <ctime>
#include <random>
#include <string>
#include <functional>
#include <algorithm>
#include <vector>
#include "mainwindow.h"
using namespace std;

namespace Stoch{
// new
	double generateUniformRand(double min, double max);
	double generateLogNormRand(double mean, double std);
	double generateNormRand(double mean, double std);
	double generateGammaRand(double mean, double variance);

	double generateWorkPerformed(double timeInterval, double cv);
	double generateTimeDeviation(double timeInterval, double cv);
	dataMatrix cumulativeDistributionFunction(dataVector values);

	default_random_engine initGenerator();
	//default_random_engine generator;
// old (must be updated & moved to new class)
	dataMatrix calculate(double frequency);
	double timeOfArrival(double waitingTime, double frequency);
	dataMatrix processOfArrival(double waitingTime, double frequency);


};
