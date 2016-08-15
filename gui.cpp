#include "gui.h"
#include "math.h"

// text namespace
	void text::setName(QTextBrowser* textBrowser, const QString &name)
	{
		textBrowser->setText(name);
	}

	void text::setNameString(QTextBrowser* textBrowser, string name) {
		setName(textBrowser, QString::fromStdString(name));
	}

	void text::appendText(QTextBrowser* textBrowser,string name) {
		textBrowser->append(QString::fromStdString(name));
	}

	string text::toString(dataVector data)
	{	string s = "";
		iterList(number, data)
			s += to_string(*number) + "\t";
		return s;
	}

	string text::toString(dataMatrix data)
	{	string s = "\n";
		iterList(vector, data)
			s += toString(*vector) + '\n';
		return s;
	}


	// plot namespace
		int plot::plt(QCustomPlot* customPlot, dataMatrix plotData, string name) {
			//Add plot using same axis
			int graphNum = customPlot->graphCount();
			if (graphNum == 0) {
				customPlot->legend->setVisible(true);
				setAxisLabels(customPlot, "x", "y");
			}

			assignPair(dataMatrix2QVector(plotData), xDataQ, yDataQ);
			customPlot->addGraph();
			customPlot->graph(graphNum)->setData(xDataQ, yDataQ);
			customPlot->graph(graphNum)->rescaleAxes();
			customPlot->graph(graphNum)->setName(QString::fromStdString(name));

			return graphNum;
		}

		int plot::plt(QCustomPlot* customPlot, dataMatrix plotData, string xLabel, string yLabel, string name)
		{	int graphNum = plt(customPlot, plotData, name);
			setAxisLabels(customPlot, xLabel, yLabel);
			return graphNum;
		}

		void plot::setAxisLabels(QCustomPlot* customPlot, string xLabel, string yLabel){
			customPlot->xAxis->setLabel(to_QString(xLabel));
			customPlot->yAxis->setLabel(to_QString(yLabel));
		}

		void plot::setPlotData(QCustomPlot* customPlot, dataMatrix plotData, int graphNum, bool rescale) {
			assignPair(dataMatrix2QVector(plotData), xDataQ, yDataQ);
			customPlot->graph(graphNum)->setData(xDataQ, yDataQ);
			if (rescale)
				customPlot->graph(graphNum)->rescaleAxes();
		}

		void plot::setLimits(QCustomPlot* customPlot,double xmin, double xmax, double ymin, double ymax) {
			customPlot->xAxis->setRange(xmin, xmax);
			customPlot->yAxis->setRange(ymin, ymax);
		}

		int plot::drawLine(QCustomPlot* customPlot, double xmin, double xmax, double ymin, double ymax) {
			int noPoints = 10;
			QVector<double> xDataQ(noPoints), yDataQ(noPoints);
			double xInc = (xmax - xmin) / (noPoints - 1);
			double yInc = (ymax - ymin) / (noPoints - 1);
			double x = xmin;
			double y = ymin;
			for (int i = 0; i < noPoints - 1; i++) {
				xDataQ[i] = x; x += xInc;
				yDataQ[i] = y; y += yInc;
			}

			xDataQ[noPoints - 1] = x;
			yDataQ[noPoints - 1] = y;
			int graphNum = customPlot->graphCount();
			customPlot->addGraph();
			customPlot->graph(graphNum)->setData(xDataQ, yDataQ);
			return graphNum;
		}

		int plot::scatterPlot(QCustomPlot * customPlot, dataMatrix plotData, string name)
		{
			int graphNum = plt(customPlot, plotData, name);
			auto currentGraph = customPlot->graph(graphNum);
			currentGraph->setLineStyle(QCPGraph::lsNone);
			currentGraph->setScatterStyle(QCPScatterStyle::ssCross);
			return graphNum;
		}

		int plot::scatterPlot(QCustomPlot* customPlot, dataMatrix plotData, string xLabel, string yLabel, string name) {
			int graphNum = scatterPlot(customPlot, plotData, name);
			setAxisLabels(customPlot, xLabel, yLabel);
			return graphNum;
		}

		int plot::barPlot(QCustomPlot* customPlot, dataVector plotVector, labelVector labelVector, string name){
			// create empty bar chart objects:

		return barPlot(customPlot, plotVector, generateSpacing(plotVector, 1, 1), labelVector, name);
		}

		int plot::barPlot(QCustomPlot * customPlot, dataVector plotVector, dataVector ticVector, labelVector labelVector, string name, double xmax)
		{
			customPlot->clearPlottables();
			if (plotVector.size() > 0) {
				QCPBars *barData = new QCPBars(customPlot->xAxis, customPlot->yAxis);
				customPlot->addPlottable(barData);
				// set names and colors:
				QPen pen;
				pen.setWidthF(1.2);
				barData->setName(to_QString(name));
				pen.setColor(QColor(1, 92, 191));
				barData->setPen(pen);
				barData->setBrush(QColor(1, 92, 191, 100));

				// prepare x axis with country labels:
				QVector<double> ticks = dataVector2QVector(ticVector);
				QVector<QString> labels = labelVector2QVector(labelVector);

				customPlot->xAxis->setAutoTicks(false);
				customPlot->xAxis->setAutoTickLabels(false);
				customPlot->xAxis->setTickVector(ticks);
				customPlot->xAxis->setTickVectorLabels(labels);
				//customPlot->xAxis->setTickLabelRotation(60);
				customPlot->xAxis->setSubTickCount(0);
				customPlot->xAxis->setTickLength(0, 4);
				customPlot->xAxis->grid()->setVisible(true);
				if (xmax <= 0)
					customPlot->xAxis->setRange(0, plotVector.size() + 2);
				else
					customPlot->xAxis->setRange(0, xmax);
				// prepare y axis:
				customPlot->yAxis->setRange(0, *(max_element(plotVector.begin(), plotVector.end())));
				customPlot->yAxis->setPadding(5); // a bit more space to the left border
				customPlot->yAxis->setLabel("");
				customPlot->yAxis->grid()->setSubGridVisible(true);
				QPen gridPen;
				gridPen.setStyle(Qt::SolidLine);
				gridPen.setColor(QColor(0, 0, 0, 25));
				customPlot->yAxis->grid()->setPen(gridPen);
				gridPen.setStyle(Qt::DotLine);
				customPlot->yAxis->grid()->setSubGridPen(gridPen);

				// Add data:
				QVector<double> qBarData = dataVector2QVector(plotVector);
				barData->setData(ticks, qBarData);

				// setup legend:
				customPlot->legend->setVisible(true);
				customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop | Qt::AlignRight);
				customPlot->legend->setBrush(QColor(255, 255, 255, 200));
				QPen legendPen;
				legendPen.setColor(QColor(130, 130, 130, 200));
				customPlot->legend->setBorderPen(legendPen);
				customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
			}
			else {
				QVector<double> ticks = dataVector2QVector(ticVector);
				QVector<QString> labels = labelVector2QVector(labelVector);
				customPlot->xAxis->setTickVector(ticks);
				customPlot->xAxis->setTickVectorLabels(labels);
			}
			return 0;
		}

		void plot::addBar(QCustomPlot * customPlot, double min, double max, double location, string name){
				QCPBars *barData = new QCPBars(customPlot->xAxis, customPlot->yAxis);
				customPlot->addPlottable(barData);
				// set names and colors:
				QPen pen;
				pen.setWidthF(1.2);
				barData->setName(to_QString(name));
				pen.setColor(QColor(1, 92, 191));
				barData->setPen(pen);
				barData->setBrush(QColor(1, 92, 191, 100));

				QVector<double> ticks = {location};
				QVector<QString> labels = {to_QString(name)};

				// Add data:
				QVector<double> qBarData = {max - min};
				barData->setData(ticks, qBarData);
				barData->setBaseValue(min);
		}

		void plot::rectPlot(QCustomPlot * customPlot, dataVector min, dataVector max, dataVector location, labelVector name)
		{ // draw rectangles at location
			customPlot->clearPlottables();
			for (int i = 0; i < min.size(); i++)
			addBar(customPlot, min[i], max[i], location[i], name[i]);

			QVector<double> ticks = dataVector2QVector(location);

			customPlot->xAxis->setAutoTicks(false);
			customPlot->xAxis->setAutoTickLabels(false);
			customPlot->xAxis->setTickVector(ticks);
			QVector<QString> labels (min.size());
			customPlot->xAxis->setTickVectorLabels(labels); //location
			customPlot->xAxis->setSubTickCount(0);
			customPlot->xAxis->setTickLength(0, 4);
			customPlot->xAxis->grid()->setVisible(true);
			customPlot->xAxis->setRange(0, 10); //TEMP

			// prepare y axis:
			customPlot->yAxis->setRange(0, 24); //TEMP
			customPlot->yAxis->setPadding(5); // a bit more space to the left border
			customPlot->yAxis->setLabel("");
			customPlot->yAxis->grid()->setSubGridVisible(true);
			QPen gridPen;
			gridPen.setStyle(Qt::SolidLine);
			gridPen.setColor(QColor(0, 0, 0, 25));
			customPlot->yAxis->grid()->setPen(gridPen);
			gridPen.setStyle(Qt::DotLine);
			customPlot->yAxis->grid()->setSubGridPen(gridPen);

			// setup legend:
			customPlot->legend->setVisible(true);
			customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop | Qt::AlignRight);
			customPlot->legend->setBrush(QColor(255, 255, 255, 200));
			QPen legendPen;
			legendPen.setColor(QColor(130, 130, 130, 200));
			customPlot->legend->setBorderPen(legendPen);
		}

		void plot::setMaxbar(QCustomPlot* customPlot, double ymax)
		{
			customPlot->yAxis->setRange(0, ymax);
		}

		void plot::draw(QCustomPlot* customPlot) {
			customPlot->replot();
			}

		colorVector plot::toColor(dataVector sched, intVector locations, int noColors){
			// generate alternating color scheme for shedule
			colorVector defaultColors = {QColor(255,0,0), QColor(0,0,255), QColor(0,100,0), QColor(0,0,0), QColor(160,0,160) };
			int noShips = sched.size();
			colorVector colorOutput(noShips);
			if (noShips >0)
			{	int noLocations = *max_element(locations.begin(),locations.end()) + 1;
				vector<intVector> counter(noLocations);
				iterLin(i, noShips)
					if (locations[i] >= 0)
					counter[locations[i]].push_back(i);
				iterLin(i, noLocations)
				{	sort(counter[i].begin(), counter[i].end(), [sched](float a, float b) {return (sched[a] < sched[b]);});
					iterLin(j,counter[i].size()){
						colorOutput[counter[i][j]] = defaultColors[j%noColors];}
				}
			}
			return colorOutput;
		}

// utilities
	pair <QVector<double>, QVector<double> > dataMatrix2QVector(dataMatrix plotData)
	{
		dataVector xData = plotData[0];
		dataVector yData = plotData[1];
		int n = xData.size();
		QVector<double> xDataQ(n), yDataQ(n);

		for (int i = 0; i < n; i++) {
			xDataQ[i] = xData[i];
			yDataQ[i] = yData[i];
		}

		return make_pair(xDataQ, yDataQ);
	}

	labelVector dataVector2LabelVector(dataVector data){
		int n = data.size();
		labelVector labels(n);

		for (int i = 0; i < n; i++) {
			labels[i] = to_string(data[i]);
			}
		return labels; 
		}

	QVector<double> dataVector2QVector(dataVector data) {
		int n = data.size();
		QVector<double> dataQ(n);

		for (int i = 0; i < n; i++) {
			dataQ[i] = data[i];}
		return dataQ;}

	QVector<QString> labelVector2QVector(labelVector labels) {
		int n = labels.size();
		QVector<QString> labelsQ(n);

		for (int i = 0; i < n; i++) {
			labelsQ[i] = to_QString(labels[i]);
		}
		return labelsQ;
	}

	dataVector generateSpacing(dataVector list, double space, double start) {
		int n = list.size();
		dataVector data(n);
		double x = start;

		for (int i = 0; i < n; i++) {
			data[i] = x;
			x += space;
		}
		return data;
	}

	QString to_QString(string s){
		return QString::fromStdString(s);
		}



