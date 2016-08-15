#pragma once
#include "core.h"
#include "save.h"
// CHECK What happens at location change ?? -> bad


class Terminal { 
	Schedule schedule;

	public:
	double simulationTime;
	int noLoc;
	int noShips;

	Terminal(double simulationTime, int noLoc, int noShips);
	Terminal(double simulationTime, int noLoc, double congestion);
	Schedule getSchedule();
	virtual Planner* generatePlanner(MainWindow* w, double smallStep, double largeStep, double planningHorizon, double posCost);
	};

class QCTerminal : public Terminal{
	public:
	int maxTotQC;
	int maxShipQC;
	
	QCTerminal(double simulationTime, int noLoc, int noShips, int maxTotQC, int maxShipQC);
	QCTerminal(double simulationTime, int noLoc, double congestion, int maxTotQC, int maxShipQC);
	QCTerminal(Terminal term, int maxTotQC, int maxShipQC);

	Planner* generatePlanner(MainWindow* w, double smallStep, double largeStep, double planningHorizon, double posCost);

};


class Experiment {
	Terminal* terminal;
	Saver saver;

	double smallStep;
	double largeStep;
	int stepRatio;
	int noSteps;
	double planningHorizon;
	double posCost;

	Schedule sched;

	OperationalOverview* operOverview;
	ScheduleOverview* schedOverview;


	// constructor die schedule genereert & expliciet opgeven
	// getSchedule
public:
	Planner* plan;
	Experiment(MainWindow* w, Terminal* terminal, double smallStep, double largeStep, double planningHorizon,string name, double posCost = 1);
	Experiment(MainWindow* w, Terminal* terminal, double smallStep, double largeStep, double planningHorizon, Saver saver, double posCost = 1);
	void setAlpha(double alpha);

	void save(string filename);
	void run(bool visual = true);
	~Experiment();


	};