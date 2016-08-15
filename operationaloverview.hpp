#pragma once
#include <QMainWindow>
#include "ui_operationaloverview.h"
namespace Ui {
	class OperationalOverview;

}


class OperationalOverview : public QWidget {
	Q_OBJECT

public:
	//basic
		OperationalOverview(QWidget * parent = Q_NULLPTR);
		~OperationalOverview();

	//getWidgets
		QCustomPlot* getArriving();
		QCustomPlot * getWaiting();
		QCustomPlot * getProcessing();
		QTextBrowser * getText();
		QTextBrowser * getClock();
		QTextBrowser * getQCCount();


private:
	Ui::OperationalOverview *ui;
};
