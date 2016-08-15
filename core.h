#pragma once
//Convention all times are in hours

#include "stochastic.h"
#include "milp.h"


#include <list>

using namespace std;

class Ship {
	void construct(ulong uniqueId, double arrivalTimeLT, double handlingTime, int prefBerth);
public:
	static double meanHandlingTime;
	static double varHandlingTime;
	static constexpr double stdQuayCranes = 3;
	static double varFrac;

	// Attributes
	ulong uniqueId; // max 2^32 ships
	double arrivalTimeLT; /// Long Term Arrival Time// offers nanoseconds precision on year basis
	double arrivalTimeExp; // Expected Arrival Time
	double arrivalTimeAct; // Actual Arrival Time

	double handlingTimeLT; // Handling time given as if 3 Quay Cranes are working simultaneously !!! 
	double handlingTimeExp;
	double handlingTimeAct;

	double startOfServiceAct;
	double startOfServiceSched;
	double completionTime; // Absolute Time at which ship leaves port

	double portStayTime;
	double waitingTime;
	
	bool moored;
	bool arrived;
	bool finished;
	int location;

	int noCranesSched;
	int noCranesAct;
	int prefBerth;
	//Constructors
	Ship();
	Ship(ulong uniqueId, double endOfPeriod, int noLoc);
	Ship(ulong uniqueId, double arrivalTimeLT, double handlingTime, int prefBerth);


	// Ship in TimeHorizon
	bool askInTimeHorizon(double currentTime, double planningHorizon);
	void putInTimeHorizon();
	bool updateExpectedArrival(double timeInterval, double currentTime);

	// Ship Waiting
	void setWaiting(double currentTime);

	// Ship Processing
	void putOnQuay(double currentTime);
	bool updateHandlingTime(double timeInterval, int noQuayCranes = stdQuayCranes);
	void leaveQuay(double currentTime);

	// Utilities
	static bool isEarlier(Ship ship1, Ship ship2);
	static string CSVHeader(string delim = ";");
	string toCSV(bool onlyArrived = true, string delim = ";");
	double getHandlingTime(bool expected = true);
	double getTotalHandlingTime(bool expected = true);

};

typedef vector<Ship> shipVector;
typedef vector<Ship*> shipRVector;
typedef list<Ship*> shipList;
inline bool operator==(const Ship& lhs, const Ship& rhs) { return lhs.uniqueId == rhs.uniqueId; }


class QuayCrane {
	// IDEA first asking for quay cranes gets priority
	shipList waiting;
public:
	int totalMax;
	int totalAssigned;
	int shipMax;

	QuayCrane(int totalMax, int shipMax);

	bool assignMax(Ship* current);
	void moor(Ship* current);
	void leave(Ship* current);

};

class Schedule {
public:
	// Attributes
	ulong noShips;
	double simulationPeriod;
	shipVector shipSchedule;

	// Constructors
	Schedule();
	Schedule(ulong noShips, double simulationPeriod, int noLoc);
	Schedule(Schedule& sched);


	// Utilities
	dataMatrix plotData();
	Ship* getShip(ulong shipNumber);
	void setLocations(Schedule sched);
	string toCSV(bool noWait = true, string delim = ";");
};

class PerfInd {
	ulong noValues;
	double sum;
	double quadsum;
	QTextBrowser * indicator;
	QTextBrowser * value;

public:
	string name;
	PerfInd();
	PerfInd(PerformanceIndicators * P, string name);
	ulong getCount();
	void addValue(double value);
	double getMean();
	double getStdDev();
	double getStdDevAvg();
	void print();
};

typedef vector<PerfInd*> PerfList;

class Planner {
public:
	//Attributes
	double planningHorizon;
	double currentTime;
	double smallStep;
	double largeStep;
	double noShips;
	double noLocations;
	double posCost;
	double alpha;

	ulong currentUniqueId; // unique Id of the last ship which is in the arrival horizon

	Schedule arrivalTimes;
	shipList arrivalExpected;
	shipList waiting;
	shipList moored;
	shipRVector shipsInSchedule;

	PerfInd waitingTime;
	PerfInd handlingTime;
	PerfInd portStayTime;
	PerfInd solutionTime;
	PerfInd locSwitchRatio;
	PerfInd MIPGap;
	PerfInd NoShipsInSched;

	MainWindow* mainWindow;
	//methods
	// constructor
	Planner();
	Planner(MainWindow * w, Schedule arrivalTimes, double noLocations, double smallStep, double largeStep, double planningHorizon, double posCost = 1);

	// smallUpdate Functions
	void smallUpdate(); // small Update
	virtual bool updateHandlingTimes(Ship* newShip);
	virtual void statistics() {};

	bool locationOccupied(int location);
	void freeLocation(int location);

	void putInTimeHorizon(Ship* newShip);
	void setWaiting(Ship* newShip);
	void putOnQuay(Ship* newShip);
	void putDirectlyOnQuay(Ship* newShip);
	void leaveQuay(Ship* newShip);
	virtual void assignCrane(Ship* newShip) {}; // empty method overridden in QCPlanner
	virtual void freeCrane(Ship* newShip) {};

	// largeUpdate Functions
	virtual void largeUpdate(); // generate a new plan
	virtual void solveMip(dataVector arrExp, dataVector handExp, dataVector completionTime,
		dataVector priorities, dataVector prefBerth, shipRVector shipsInSchedule);

	// plotting
	void operationalPlot(OperationalOverview* o);
	virtual void plotCraneCount(OperationalOverview* o) {};
	void clock(OperationalOverview* o);

	void preparePlot(ScheduleOverview* s);
	void schedulePlot(ScheduleOverview* s);
	virtual string scheduleShipLabels(Ship* current, bool moored = false);

	void performancePlot();

	pair <labelVector, dataVector> arrivingPlotData();
	pair <labelVector, dataVector> waitingPlotData();
	virtual pair <labelVector, dataVector> processingPlotData();

	// utilities
	Schedule getSchedule();
	string writeCSVString();
	virtual PerfList getPerfInd();
	
};

class QCPlanner : public Planner {
	QuayCrane quayCrane;
	PerfInd craneAvailability;
	PerfInd cranesInUse;
	PerfInd QCSwitchRatio;
public:
	// constructors
	QCPlanner();
	QCPlanner(MainWindow * w, Schedule arrivalTimes, double noLocations, double smallStep, double largeStep, double planningHorizon, 
			double posCost, double maxQCTot, double maxQCShip);


	// smallUpdates
	void assignCrane(Ship* newShip);
	void freeCrane(Ship* newShip);
	bool updateHandlingTimes(Ship* newShip);
	void statistics();
	
	// largeUpdate
	void largeUpdate();
	void solveMip(dataVector arrExp, dataVector totalHandExp, dataVector completionTime,
		dataVector priorities, dataVector prefBerth, shipRVector shipsInSchedule, intVector currentCranes);

	//plot
	void plotCraneCount(OperationalOverview* o);
	pair <labelVector, dataVector> processingPlotData();
	string scheduleShipLabels(Ship* current, bool moored = false);
	PerfList getPerfInd();
};



// better performance recording!!!