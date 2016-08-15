#include "performanceindicators.hpp"
#include "ui_performanceindicators.h"

// basic
	PerformanceIndicators::PerformanceIndicators(QWidget * parent) : QWidget(parent) {
		ui = new Ui::PerformanceIndicators();
		ui->setupUi(this); 
	}

	PerformanceIndicators::~PerformanceIndicators() {
		delete ui;
	}

// addWidgets
	QTextBrowser * PerformanceIndicators::addIndicatorBox()
	{	int num = indicators.size();
		indicators.push_back(new QTextBrowser( ui->indList ) );
		auto textBrowser = indicators.back();
		textBrowser->setObjectName(QStringLiteral("textBrowser"));
		textBrowser->setGeometry(QRect(0, 75 * num , 250, 50));
		textBrowser->setStyleSheet(QStringLiteral("font: 14pt \"MS Shell Dlg 2\";"));
		textBrowser->setFrameShape(QFrame::NoFrame);
		textBrowser->setVisible(true);
		qApp->processEvents();
		return textBrowser;
	}

	QTextBrowser * PerformanceIndicators::addValueBox()
	{
		int num = values.size();
		values.push_back(new QTextBrowser(ui->valueList));
		auto textBrowser = values.back();
		textBrowser->setObjectName(QStringLiteral("textBrowser"));
		textBrowser->setGeometry(QRect(0, 75 * num, 250, 50));
		textBrowser->setStyleSheet(QStringLiteral("font: 14pt \"MS Shell Dlg 2\";"));
		textBrowser->setFrameShape(QFrame::NoFrame);
		textBrowser->setVisible(true);
		qApp->processEvents();
		return textBrowser;
	}
