#pragma once
#include <QWidget>
#include "types.h"
#include <QTextBrowser>
namespace Ui {class PerformanceIndicators;}

class PerformanceIndicators : public QWidget {
	Q_OBJECT
	// attributes
	vector<QTextBrowser*> indicators;
	vector<QTextBrowser*> values;

public:
	//basic
	PerformanceIndicators(QWidget * parent = Q_NULLPTR);
	~PerformanceIndicators();

	// addWidgets
	QTextBrowser* addIndicatorBox();
	QTextBrowser* addValueBox();
	

private:
	Ui::PerformanceIndicators *ui;
};
