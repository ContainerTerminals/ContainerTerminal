#include "containerLocation.h"
#include "mainwindow.h"
#include "gui.h"

dataMatrix generatePositions(double yardDepth, double shipPlace, double std, int noContainers)
{
	dataMatrix positions(2, dataVector(noContainers));
	for (int i = 0; i< noContainers;i++){
	positions[0][i] = Stoch::generateNormRand(shipPlace, std);
	positions[1][i] = Stoch::generateUniformRand(0,yardDepth);
	}
	return positions;
}

dataMatrix generateDistance(dataMatrix containerPositions, double quaymin, double quaymax, int noPoints)
{	double Y = 0;
	double X = quaymin;
	double Xinc = (quaymax - quaymin)/(noPoints -1);
	dataMatrix distance(2, dataVector(noPoints));
	for (int i = 0; i < noPoints; i++) {
		double sum = 0;
		
		for (int j = 0; j < containerPositions[0].size(); j++) {
			sum += calcDistance(X,Y, containerPositions[0][j], containerPositions[1][j]);
		}
		distance[0][i] = X;
		distance[1][i] = sum;
		X += Xinc;
	}
	return distance;
}

double calcDistance(double X1,double Y1,double X2, double Y2){
	return abs(X1-X2) + abs(Y1-Y2);}

void distancePlot(MainWindow* w, MainWindow* g){
	double quayDepth = 10;
	double quayLength = 100;
	double std = 20;
	double noContainers = 30; //100;
	double noPoints = 1000;
	dataMatrix positions  = generatePositions(quayDepth, quayLength /2, std, noContainers);
	dataMatrix distance = generateDistance(positions, 0, quayLength , noPoints);

/*	auto plotW = w->getQCustomPlot();
	plot::scatterPlot(plotW, positions, "quay length", "quay Depth", "positions");
	plot::setLimits(plotW, 0, quayLength, 0, quayDepth);
	
	
	auto plotG = g ->getQCustomPlot();
	plot::plt(plotG, distance,"quay length","Distance","distance");
	plot::setLimits(plotG, 0,quayLength,0, quayLength * noContainers /2);
	*/}
	