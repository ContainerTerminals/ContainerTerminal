#include "operationaloverview.hpp"

// basic
	OperationalOverview::OperationalOverview(QWidget *parent) :
		QWidget(parent),
		ui(new Ui::OperationalOverview)
	{
		ui->setupUi(this);
	}


	OperationalOverview::~OperationalOverview() {
		delete ui;
	}

// getWidgets
	QCustomPlot * OperationalOverview::getArriving()
	{
		return ui->arriving;
	}

	QCustomPlot * OperationalOverview::getWaiting()
	{
		return ui->waiting;
	}

	QCustomPlot * OperationalOverview::getProcessing()
	{
		return ui->processing;
	}

	QTextBrowser * OperationalOverview::getText() {
		return ui->printScreen;}

	QTextBrowser * OperationalOverview::getClock()
	{
		return ui->clock;
	}

	QTextBrowser * OperationalOverview::getQCCount()
	{
		return ui->QCCount;
	}
