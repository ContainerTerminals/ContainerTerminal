#include "QCBAP.h"

void simpleTestQC(MainWindow * w)
{
	double planningHorizon = Times::day;
	double simulationTime = Times::week;
	double shortStep = 5 * Times::minute;
	double longStep = 2 * Times::hour;
	int maxTotQC = 11;
	int maxShipQC = 3;

	int noLoc = 4;
	QCTerminal term = QCTerminal(simulationTime, noLoc, 0.8, maxTotQC, maxShipQC);
	Experiment exp = Experiment(w, &term, shortStep, longStep, planningHorizon, "test", 1);
	exp.run(true);
	exp.save("test.csv");
}


void noCranes(MainWindow *w)
{
	double planningHorizon = Times::day;
	double simulationTime = 4 * Times::week;
	double shortStep = 5 * Times::minute;
	double longStep = 6 * Times::hour;
	int noLoc = 4;
	double congestion = 0.65;
	dataVector noCranes = { 9, 11, 13, 15, 16}; //
	Saver saver = Saver("noQC");
	double maxShipQC = 4;
	Terminal term = Terminal(simulationTime, noLoc, congestion);
	iterList(qc,noCranes)
	{
		try {
			w->text("started: noQC:=  " + to_string(*qc) + "\n");
			QCTerminal qcTerm = QCTerminal(term, *qc, maxShipQC);
			Experiment exp = Experiment(w, &qcTerm, shortStep, longStep, planningHorizon, saver, 1);
			exp.run(false);
			exp.save("Afterwards$" + to_string(*qc) + "$");
		}
		catch (const std::exception &e) {
			w->text(e.what());
		}
		catch (...) {
			w->text("other error");
		}
	}
	w->text("calculation finished \n");

}