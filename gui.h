#pragma once
#include <QMainWindow>
#include <QTimer>
#include "qcustomplot.h"
#include "types.h"


namespace text {
	void setName(QTextBrowser* textBrowser, const QString &name);
	void setNameString(QTextBrowser* textBrowser, string name);
	void appendText(QTextBrowser* textBrowser, string name);
	string toString(dataVector data);
	string toString(dataMatrix data);
}

namespace plot{
	int plt(QCustomPlot * customPlot, dataMatrix plotData, string name);
	int plt(QCustomPlot* customPlot, dataMatrix plotData, string xLabel, string yLabel, string name);
		
	void setAxisLabels(QCustomPlot* customPlot, string xLabel, string yLabel);
	void setPlotData(QCustomPlot* customPlot, dataMatrix plotData, int graphNum, bool rescale);
	void setLimits(QCustomPlot* customPlot, double xmin, double xmax, double ymin, double ymax);

	int scatterPlot(QCustomPlot* customPlot, dataMatrix plotData, string name);
	int scatterPlot(QCustomPlot* customPlot, dataMatrix plotData, string xLabel, string yLabel, string name);
	int barPlot(QCustomPlot * customPlot, dataVector plotVector, labelVector labelVector, string name);
	int barPlot(QCustomPlot * customPlot, dataVector plotVector, dataVector ticVector, labelVector labelVector, string name, double xmax = -1);
	void addBar(QCustomPlot * customPlot, double min, double max, double location, string name);
	void rectPlot(QCustomPlot * customPlot, dataVector min, dataVector max, dataVector location, labelVector name);
	void setMaxbar(QCustomPlot* customPlot, double ymax);
	void draw(QCustomPlot * customPlot);
	int drawLine(QCustomPlot * customPlot, double xmin, double xmax, double ymin, double ymax);
	
	colorVector toColor(dataVector sched, intVector locations, int noColors);
}



pair<QVector<double>, QVector<double>> dataMatrix2QVector(dataMatrix plotData);
labelVector dataVector2LabelVector(dataVector data);
QVector<double> dataVector2QVector(dataVector data);
QVector<QString> labelVector2QVector(labelVector labels);
dataVector generateSpacing(dataVector data, double space, double start);
QString to_QString(string s);
