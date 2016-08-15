// all plotting, printing files etc

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "operationaloverview.hpp"
#include "scheduleoverview.hpp"
#include "performanceindicators.hpp"
#include "gui.h"

using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT
  
public:
	// basic
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

  // getWidgets
  OperationalOverview * getOperationalOverview();
  ScheduleOverview  * getScheduleOverview();
  PerformanceIndicators * getPI();

  // write Text
  void text(string output);
  

private:
  Ui::MainWindow *ui;

};





#endif // MAINWINDOW_H
