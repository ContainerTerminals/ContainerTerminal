#include "BAP.h"

void congestion(MainWindow * w)
{
	double planningHorizon = Times::day;
	double simulationTime = 4 * Times::week;
	double shortStep = 5 * Times::minute;
	double longStep = 6 * Times::hour;
	int noLoc = 4;
	//double congestion = 0.5;
	dataVector congestion = {0.1, 0.2, 0.4, 0.6 , 0.3, 0.5, 0.7,0.8, 0.9}; //
	Saver saver = Saver("congestion");
	iterList(con, congestion)
	{
		try {
			w->text("started: congestion:=  " + to_string(*con) + "\n");
			Terminal term = Terminal(simulationTime, noLoc, *con);
			Experiment exp = Experiment(w, &term, shortStep, longStep, planningHorizon, saver, 1);
			exp.run(false);
			exp.save("Afterwards$" + to_string(*con) + "$");
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

void simpleTest(MainWindow *w) 
{	Ship::meanHandlingTime = 6;
	double planningHorizon = Times::day;
	double simulationTime = Times::day;
	double shortStep = 5 * Times::minute;
	double longStep = 6 * Times::hour;
	int noLoc = 4;
	Terminal term = Terminal(simulationTime, noLoc, 0.8);
	Experiment exp = Experiment(w, &term, shortStep, longStep, planningHorizon, "test", 1);
	exp.run(true);
	exp.save("test.csv");
}

void noBerths(MainWindow * w){}

void variance(MainWindow * w) {
	double planningHorizon = Times::day;
	double simulationTime = 4 * Times::week;
	double shortStep = 5 * Times::minute;
	double longStep = 6 * Times::hour;
	int noLoc = 4;
	dataVector varFrac= { 0.2, 1e-3, 0.01, 0.02, 0.5, 0.1, 0.05};
	double congestion = 0.85;
	Terminal term = Terminal(simulationTime, noLoc, congestion);
	Saver saver = Saver("variance");
	iterList(var, varFrac)
	{
		try {
			Ship::varFrac = *var;
			w->text("started: variance:=  " + to_string(*var) + "\n");
			Experiment exp = Experiment(w, &term, shortStep, longStep, planningHorizon, saver, 1);
			exp.run(false);
			exp.save("Afterwards$" + to_string(*var) + "$");
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

void posCost(MainWindow * w) {
	double planningHorizon = Times::day;
	double simulationTime = 4 * Times::week;
	double shortStep = 5 * Times::minute;
	double longStep = 6 * Times::hour;
	int noLoc = 4;
	dataVector posCost = {10, 0.1, 0.3, 100, 3, 0, 1};
	double congestion = 0.85;
	Terminal term = Terminal(simulationTime, noLoc, congestion);
	Saver saver = Saver("posCost");
	iterList(pos, posCost)
	{
		try {
			w->text("Position Cost:=  " + to_string(*pos) + "\n");
			Experiment exp = Experiment(w, &term, shortStep, longStep, planningHorizon, saver, *pos);
			exp.run(false);
			exp.save("Afterwards$" + to_string(*pos) + "$");
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

void posCostRobust(MainWindow * w) {
	double planningHorizon = Times::day;
	double simulationTime = 4 * Times::week;
	double shortStep = 5 * Times::minute;
	double longStep = 6 * Times::hour;
	int noLoc = 4;
	dataVector posCost = { 10, 0.1, 0.3, 100, 3, 0, 1 };
	double congestion = 0.85;
	Terminal term = Terminal(simulationTime, noLoc, congestion);
	Saver saver1 = Saver("posCost1ST");
	Saver saver2 = Saver("posCostRobust");
	Experiment exp1 = Experiment(w, &term, shortStep, longStep, planningHorizon, saver1, 0);
	Ship::varFrac = 0.001;
	exp1.run(false);
	exp1.save("Afterwards$" + to_string(0) + "$");
	iterList(pos, posCost)
	{
		try {
			Ship::varFrac = 0.1;
			w->text("Position Cost:=  " + to_string(*pos) + "\n");
			Experiment exp2 = Experiment(w, &term, shortStep, longStep, planningHorizon, saver2, *pos);
			exp2.plan->arrivalTimes.setLocations(exp1.plan->arrivalTimes);
			exp2.run(false);
			exp2.save("Afterwards$" + to_string(*pos) + "$");
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

void planHorizon(MainWindow * w, double fracVar, double congestion){
	Ship::varFrac = fracVar;
	dataVector planningHorizonList = {0.25 * Times::day, 0.5 * Times::day, Times::day,  2 * Times::day, 3 * Times::day};
	double simulationTime = 4 * Times::week;
	double shortStep = 5 * Times::minute;
	double longStep = 6 * Times::hour;
	int noLoc = 4;
	Terminal term = Terminal(simulationTime, noLoc, congestion);
	string name = "planning Horizon";
	if (fracVar != 0.01)
		name+= toString(fracVar) + "&" + toString(congestion);
	Saver saver = Saver(name);
	iterList(planHor, planningHorizonList)
	{
		try {
			w->text("started: planning Horizon:=  " + to_string(*planHor) + "\n");
			Experiment exp = Experiment(w, &term, shortStep, longStep, *planHor, saver, 1);
			exp.run(false);
			exp.save("Afterwards$" + to_string(*planHor) + "$");
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

void alpha(MainWindow * w) {
	double planningHorizon = Times::day;
	double simulationTime = 4 * Times::week;
	double shortStep = 5 * Times::minute;
	double longStep = 6 * Times::hour;
	int noLoc = 4;
	dataVector alphaList = {0, 0.25, 0.5, 1, 2, 4};
	double congestion = 0.85;
	Terminal term = Terminal(simulationTime, noLoc, congestion);
	Saver saver = Saver("alpha");
	iterList(alpha, alphaList)
	{
		try {
			w->text("started: alpha:=  " + to_string(*alpha) + "\n");
			Experiment exp = Experiment(w, &term, shortStep, longStep, planningHorizon, saver, 1);
			exp.setAlpha(*alpha);
			exp.run(false);
			exp.save("Afterwards$" + to_string(*alpha) + "$");
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

void reschedulePeriod(MainWindow * w, double fracVar, double congestion){
	double planningHorizon = Times::day;
	Ship::varFrac = fracVar;
	dataVector reschedulePeriodList= { 1, 2, 4, 8, 12};
	double simulationTime = 4 * Times::week;
	double shortStep = 5 * Times::minute;
	int noLoc = 4;
	Terminal term = Terminal(simulationTime, noLoc, congestion);
	string name = "rescheduling";
	if (fracVar != 0.01)
		name += toString(fracVar) + "&" + toString(congestion);
	Saver saver = Saver(name);
	iterList(resPer, reschedulePeriodList)
	{
		try {
			w->text("started: Reschedule Period:=  " + to_string(*resPer) + "\n");
			Experiment exp = Experiment(w, &term, shortStep, *resPer, planningHorizon, saver, 1);
			exp.run(false);
			exp.save("Afterwards$" + to_string(*resPer) + "$");
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




