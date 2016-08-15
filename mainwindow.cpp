#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QDesktopWidget>
#include <QScreen>
#include <QMessageBox>
#include <QMetaEnum>
#include <map>

using namespace std;
// basic
	MainWindow::MainWindow(QWidget *parent) :
	  QMainWindow(parent),
	  ui(new Ui::MainWindow)
	{
	  ui->setupUi(this);
	  setWindowTitle("ContainerTerminals");

	}

	MainWindow::~MainWindow()
	{
	  delete ui;
	}

// getWidgets
	OperationalOverview * MainWindow::getOperationalOverview()
	{
		return ui->Operations;
	}

	ScheduleOverview  * MainWindow::getScheduleOverview() {
		return ui->Schedule;
	}

	PerformanceIndicators * MainWindow::getPI()
	{
		return ui->Performance;
	}

	void MainWindow::text(string output)
	{	auto o = getOperationalOverview();
		auto browser = o->getText();
		text::appendText(browser, output);
	}




