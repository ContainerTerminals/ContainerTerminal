#include "scheduleoverview.hpp"
#include "gui.h"


//basic
	ScheduleOverview::ScheduleOverview(QWidget *parent):
		QWidget(parent),
		ui(new Ui::ScheduleOverview)
		{
			ui->setupUi(this);
		}

	ScheduleOverview::~ScheduleOverview() {
		delete ui;
	}


// initial setup
	void ScheduleOverview::initialSettings(int noLocations, double planningHorizon, double margin)
	{	num = 0;
		if (noLocations < 5)
			scheduleHeight = 160 * noLocations;
		ui->Schedule->setFixedHeight(scheduleHeight);
		ui->Location->setFixedHeight(scheduleHeight);
		ui->BackGround->setFixedHeight(scheduleHeight + backgroundHeight);

		rowSpace = scheduleHeight /noLocations;
		rowHeight = rowSpace - margin;
		this->halfMargin = margin / 2;
		pxPerHour = scheduleWidth / planningHorizon;
	}

	void ScheduleOverview::setGrid(int noLocations, int noTimes)
	{	int lineWidth = halfMargin * 2 + 2;

			for (int i = 0; i < noLocations; i++) {

				grid.push_back(new QFrame(ui->BackGround));
				auto line = grid.back();
				double height = rowSpace * i - halfMargin - 1;
				line->setGeometry(QRect(0, height, scheduleWidth, lineWidth));
				line->setFrameShape(QFrame::HLine);
				line->setStyleSheet(QLatin1String("background-color: rgba(0,0, 0, 0);"));
				line->setLineWidth(lineWidth);
				line->raise();
				line->setVisible(true);
			}

			for (int i = 0; i <= noTimes; i++) {
				grid.push_back(new QFrame(ui->BackGround));
				auto line = grid.back();
				double width = i * pxPerHour - lineWidth/2;
				line->setGeometry(QRect(width, 0, lineWidth, scheduleHeight));
				line->setLineWidth(lineWidth);
				line->setFrameShape(QFrame::VLine);
				line->setStyleSheet(QLatin1String("background-color: rgba(0,0, 0, 0);"));
				line->raise();
				line->setVisible(true);
			}

			for (int i = 1; i <= noTimes-1; i++) {
				hourIndicators.push_back(new QTextBrowser(ui->BackGround));
				auto text = hourIndicators.back();
				double width = i * pxPerHour - 30;
				text->setGeometry(QRect(width, scheduleHeight, 100, 100));
				text->setText(to_QString( Times::toHour(i) ));
				text->setStyleSheet(QLatin1String("font: 12pt \"MS Shell Dlg 2\";\n"
					"background-color: rgba(255, 255, 255, 0);"));
				text->setFrameShape(QFrame::NoFrame);
				text->setVisible(true);
			}

			for (int i = 0; i <= noLocations; i++) {
				locationIndicators.push_back(new QTextBrowser(ui->Location));
				auto text = locationIndicators.back();
				double height = (i + 0.5 ) * rowSpace - 30;
				text->setGeometry(QRect(60,height, 50, 50));
				text->setText(to_QString(to_string(i)));
				text->setStyleSheet(QLatin1String("font: 15pt \"MS Shell Dlg 2\";\n"
					"background-color: rgba(255, 255, 255, 0);"));
				text->setFrameShape(QFrame::NoFrame);
				text->setVisible(true);
			}


			//ui->Schedule->raise();
		qApp->processEvents();
		}

	void ScheduleOverview::addTextBox() {
		boxVector.push_back(new QPushButton(ui->Schedule));
		auto pushButton = boxVector.back();
	
		pushButton->setObjectName(to_QString(""));
		pushButton->setGeometry(QRect(0, 0, 0, 0));
		pushButton->setEnabled(false);
		pushButton->setStyleSheet(QLatin1String("background-color: rgba(255, 0, 255, 160);\n"
			"color: rgb(255, 255, 255);\n"
			"font: 14pt \"MS Shell Dlg 2\";\n"
			"border: none;\n"
			"margin: 0px;\n"
			"padding: 0px;"));
		pushButton->setVisible(false);
		qApp->processEvents();
	}

	void ScheduleOverview::addTextBox(int numBox)
	{
		for (int i = 0; i < numBox; i++) {
			addTextBox();
		}
	}

// change Boxes & Grid
	void ScheduleOverview::setData(intVector xpos, intVector ypos, intVector width, intVector height, labelVector text)
	{
		for (int i = 0; i < boxVector.size(); i++) {
			boxVector[i]->setVisible(false);
		}
	
		for (int i = 0; i < xpos.size(); i++) {
			auto pushButton = boxVector[i];
			pushButton->setGeometry(QRect(xpos[i], ypos[i] +halfMargin, width[i], height[i]));
			pushButton->setVisible(true);
			pushButton->setText(to_QString(text[i]));
			pushButton->raise();
		}
		ui->Schedule->raise();
	}

	void ScheduleOverview::setData(intVector loc, dataVector start, dataVector end, labelVector text)
	{ num = loc.size();
	intVector xpos(num);
	intVector ypos(num);
	intVector width(num);
	intVector height(num);

	for (int i = 0; i < num; i++) {
		xpos[i] = (int) (start[i] * pxPerHour - 1);
		ypos[i] = (int) (loc[i] * rowSpace);
		width[i] = (int)((end[i] - start[i] )* pxPerHour )+ 2;
		height[i] = (int) rowHeight;}

		setData(xpos, ypos, width ,height, text);
	}

	void ScheduleOverview::addData(intVector loc, dataVector end, labelVector text, QColor color)
	{
		int add = loc.size();
		intVector xpos(add);
		intVector ypos(add);
		intVector width(add);
		intVector height(add);

		for (int i = 0; i < add; i++) {
			xpos[i] = (int)(0);
			ypos[i] = (int)(loc[i] * rowSpace);
			width[i] = (int)(end[i] * pxPerHour) + 2;
			height[i] = (int)rowHeight;
		}

		addData(xpos, ypos, width, height, text, color);

	}

	void ScheduleOverview::addData(intVector xpos, intVector ypos, intVector width, intVector height, labelVector text, QColor color)
	{ 	for (int i = 0; i < xpos.size(); i++) {
			auto pushButton = boxVector[num];
			pushButton->setGeometry(QRect(xpos[i], ypos[i] + halfMargin, width[i], height[i]));
			pushButton->setText(to_QString(text[i]));
			setColor(pushButton, color);
			pushButton->setVisible(true);
			num++;
		}
		ui->Schedule->raise();
	}

	void ScheduleOverview::setColor(QPushButton * pushButton, QColor color) {
		string colorString = "background-color: rgba(" + to_string(color.red()) + ", " + to_string(color.green())
			+ ", " + to_string(color.blue()) + ", 170);\n";
		pushButton->setStyleSheet("color: rgb(255, 255, 255);\n"
			+ to_QString(colorString) +
			"font: 14pt \"MS Shell Dlg 2\";\n"
			"border: none;\n"
			"margin: 0px;\n"
			"padding: 0px;");
		pushButton->repaint();
	}

	void ScheduleOverview::setColor(colorVector colors) {
		iterLin(i, colors.size()) {
			setColor(boxVector[i], colors[i]);
		}

	}

	void ScheduleOverview::updateGrid(double currentTime)
	{
		iterList(ref, hourIndicators) {
			currentTime++;
			(*ref)->setText(to_QString(Times::toHour(currentTime)));
		}
	}

// make Image
	void ScheduleOverview::saveSchedule(string filename)
	{
		ui->BackGround->grab().save(to_QString(filename));
	}
