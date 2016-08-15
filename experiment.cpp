#include "experiment.h"

#pragma region Terminal
	Terminal::Terminal(double simulationTime, int noLoc, int noShips) : simulationTime(simulationTime), noLoc(noLoc), noShips(noShips)
{
	schedule = Schedule(noShips, simulationTime, noLoc);
}

	Terminal::Terminal(double simulationTime, int noLoc, double congestion): 
		simulationTime(simulationTime), noLoc(noLoc)
{		this->noShips = (congestion * simulationTime * noLoc) / Ship::meanHandlingTime;
		schedule = Schedule(noShips, simulationTime, noLoc);
}

	Schedule Terminal::getSchedule()
{
	return Schedule(schedule);
}

	Planner* Terminal::generatePlanner(MainWindow * w, double smallStep, double largeStep, double planningHorizon, double posCost)
	{
		return new Planner(w, getSchedule(), noLoc, smallStep, largeStep, planningHorizon, posCost);
	}
#pragma endregion


#pragma region QCTerminal

QCTerminal::QCTerminal(double simulationTime, int noLoc, int noShips, int maxTotQC, int maxShipQC):
	maxTotQC(maxTotQC), maxShipQC(maxShipQC), Terminal(simulationTime, noLoc, noShips)
	{}


QCTerminal::QCTerminal(double simulationTime, int noLoc, double congestion, int maxTotQC, int maxShipQC):
	maxTotQC(maxTotQC), maxShipQC(maxShipQC), Terminal(simulationTime, noLoc, congestion)
	{}

QCTerminal::QCTerminal(Terminal term, int maxTotQC, int maxShipQC):
	maxTotQC(maxTotQC), maxShipQC(maxShipQC), Terminal(term){}

Planner* QCTerminal::generatePlanner(MainWindow * w, double smallStep, double largeStep, double planningHorizon, double posCost)
{
	return new QCPlanner(w, getSchedule(), noLoc, smallStep, largeStep, planningHorizon, posCost, maxTotQC, maxShipQC); 
}
#pragma endregion


#pragma region Experiment
Experiment::Experiment(MainWindow* w, Terminal* terminal, double smallStep, double largeStep, double planningHorizon, string name, double posCost):
terminal(terminal), planningHorizon(planningHorizon), smallStep(smallStep), largeStep(largeStep), saver(Saver(name)), posCost(posCost)
{ 
	this->stepRatio = largeStep / smallStep;
	this->noSteps = terminal->simulationTime / largeStep ;
	this->sched =terminal->getSchedule();
	this->operOverview = w->getOperationalOverview();
	this->schedOverview = w->getScheduleOverview();
	this->plan = terminal->generatePlanner(w, smallStep, largeStep, planningHorizon, posCost);
}

Experiment::Experiment(MainWindow * w, Terminal* terminal, double smallStep, double largeStep, double planningHorizon, Saver saver, double posCost):
	terminal(terminal), planningHorizon(planningHorizon), smallStep(smallStep), largeStep(largeStep), saver(saver), posCost(posCost)
{
	this->stepRatio = largeStep / smallStep;
	this->noSteps = terminal->simulationTime / largeStep;
	this->sched = terminal->getSchedule();
	this->operOverview = w->getOperationalOverview();
	this->schedOverview = w->getScheduleOverview();
	this->plan = terminal->generatePlanner(w,smallStep, largeStep, planningHorizon, posCost);
}

void Experiment::setAlpha(double alpha)
{ plan->alpha = alpha;
}

void Experiment::save(string filename)
{ 
	saver.save(plan->writeCSVString() , filename + "global.csv");
	saver.save(plan->getSchedule().toCSV(), filename + "ship.csv");
}

Experiment::~Experiment()
{
	delete plan;
}

void Experiment::run(bool visual)
{	
	plan->smallUpdate();
	plan->currentTime = 0;
	plan->largeUpdate();
		
	if (visual)
	{	plan->preparePlot(schedOverview);
		plan->schedulePlot(schedOverview);
		plan->operationalPlot(operOverview);
		plan->performancePlot();
		plan->schedulePlot(schedOverview);
	}


	iterLin(i, noSteps) {
		iterLin(j, stepRatio) {
			plan->smallUpdate();
			if (visual){
				plan->operationalPlot(operOverview);
				plan->performancePlot();}
			qApp->processEvents();
			Sleep(50);
		}
		plan->largeUpdate();
		if (visual)
			plan->schedulePlot(schedOverview);
		else plan->clock(operOverview);
	}

	qApp->processEvents();
}
#pragma endregion


