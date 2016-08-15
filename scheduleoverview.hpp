#pragma once
#include <QWidget>
#include "ui_scheduleoverview.h"
#include <QColor>
#include <QFrame>
#include <QTextBrowser>
#include <QtWidgets/QPushButton>
#include "types.h"

namespace Ui {class ScheduleOverview;}
 using namespace std;
class ScheduleOverview : public QWidget {
	Q_OBJECT

public:
	//attributes
		const double locationWidth = 90;
		const double scheduleWidth = 1700;
		const double backgroundHeight = 100;
		double scheduleHeight = 800;
		int num;
		double rowHeight;
		double rowSpace; // width + margin
		int halfMargin;
		double pxPerHour;
		vector<QPushButton*>boxVector;
		vector<QFrame*>  grid; 
		vector<QTextBrowser*>hourIndicators;
		vector<QTextBrowser*>locationIndicators;

	//methods

		//basic
			ScheduleOverview(QWidget * parent = Q_NULLPTR);
			~ScheduleOverview();

		// initialize
			void initialSettings(int noLocations, double planningHorizon, double margin);
			void setGrid(int noLocations, int noTimes);
			void addTextBox(int numBox);
			void addTextBox();

		// change Boxes & Grid

			void setData(intVector xpos, intVector ypos, intVector width, intVector height, labelVector text);
			void setData(intVector loc, dataVector start, dataVector end, labelVector text);
			void addData(intVector loc, dataVector end, labelVector text, QColor color);
			void addData(intVector xpos, intVector ypos, intVector width, intVector height, labelVector text, QColor color);

			void setColor(QPushButton* pushButton, QColor color);
			void setColor(colorVector colors);

			void updateGrid(double currentTime);

		// save
			void saveSchedule(string filename);
private:
	Ui::ScheduleOverview *ui;

};
