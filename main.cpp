#include <QApplication>
#include <string>
#include "QCBAP.h"
#include "BAP.h"
#include <exception>
#include <typeinfo>
#include <stdexcept>
using namespace std;


int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
  QApplication::setGraphicsSystem("raster");
#endif
  QApplication a(argc, argv);

	MainWindow w;
	w.show();
	posCostRobust(&w);

  return a.exec();


}





