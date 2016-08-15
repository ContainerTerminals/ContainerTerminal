#pragma once

#include "QGlobal.h"
#include <cmath> 
#include "gurobi_c++.h"

#include "mainwindow.h"


typedef vector<GRBVar> varVector;
typedef vector<varVector> varMatrix;

void testProblem(MainWindow* w, bool cranes = false);

class MILP {
protected:
	//attributes
	// basic attributes
	static constexpr double DOUBLE_MAX = numeric_limits<double>::max();
	static constexpr double EPSILON = 0.1;
	GRBEnv env;
	GRBModel model;
	double runTime;
	bool feasible;

	// constructor
	MILP();

	// run
	virtual void addVariables() = 0;
	virtual void addAllConstraints() = 0;

	// utilities
	GRBLinExpr sum(varVector, double coeff = 1);

	intVector toIntVector(varVector variables);
	intVector toIntVector(varMatrix variables);
	dataVector toDataVector(varVector variables);
	dataMatrix toDataMatrix(varMatrix variables);

	virtual bool nonEmptyModel() = 0;

	// Variables
	varMatrix createTriag(int size, double obj, double max, char GRBtype);
	varMatrix createRect(int size1, int size2, double obj, double max, char GRBtype);
	varMatrix createRect(int size1, int size2, double obj, double max, char GRBtype, int branchPrior);
	varVector createLin(int size, double obj, double max, char GRBtype, double min = 0);
	varVector createLin(int size, double obj, double max, char GRBtype, int branchPrior, double min = 0);
	varVector createLin(int size, dataVector obj, double max, char GRBtype, double min = 0);

public:
	// run
	void run();

	// output
	double getSolutionTime();
	bool isFeasible();
	double getMIPGAP();
};

class BAP : public MILP {
protected:
	// decision variables
	varMatrix location;
	varVector scheduleStart;
	varMatrix aux1;		// 1 if schip i is starts earlier than ship j is finished
	varMatrix aux2;		// 1 if both ships do share the same location

	// parameters
	int noShips; // number of ships in time horizon not yet processed
	int noLocations; // number of locations

	double bigMWaiting;

	dataVector expCompletionTimes;			// current times of ships finished per location (currentTime for empty locations)
	dataVector expArrivalTimes;				// size noShips; relative to currentTime
	dataVector expHandlingTimes;			// size noShips
	dataVector prefBerth;
	dataVector priorities;

	double posCost;
	double alpha;

//Methods
	//constructors
public:
	BAP(double planningHorizon, dataVector expCompletionTimes, dataVector expArrivalTimes, dataVector expHandlingTimes,
		 dataVector priorities, dataVector prefBerth, double posCost, double alpha);

	//run
	void addVariables();
	void addAllConstraints();
	
	// utilities
	bool nonEmptyModel();

	//constraints
	void addArrivalTimeConstr();
	void addUniqeLocationConstr();
	virtual void addNoOverlapConstr();
	void addRespectCurrentShips();

	//output
	void writeSolution(MainWindow * w) ;
	void plotSolution(ScheduleOverview * s, double planningHorizon = 24);
	dataVector getBerthTimes();
	intVector getLocation();
	bool infeasible();


};

class QCBAP : public BAP {
protected:
	varVector handlingTime;
	varVector noQuayCranes;
	varMatrix aux3;
	varMatrix aux1loc;
	varMatrix auxK;
	varMatrix  auxKloc;

	double bigMCranes;
	int maxQTot;
	int maxQship;

	intVector currentNoQC;

public:
	// Methods
	// constructors
	QCBAP(double planningHorizon, dataVector expCompletionTimes, dataVector expArrivalTimes,
		dataVector expHandlingTimes, dataVector priorities, dataVector prefBerth, double posCost,
		double maxQTot, double maxQship, intVector currentNoQC);

	// run
	void addVariables();
	void addAllConstraints();

	// constraints
	virtual void addNoOverlapConstr();
	void addNumberOfQC();
	void addHandlingTime();

	//output
	void writeSolution(MainWindow * w);
	void plotSolution(ScheduleOverview * s, double planningHorizon = 24);
	intVector getQC();
};
