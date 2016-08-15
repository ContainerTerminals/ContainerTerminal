#include "core.h"
using namespace plot;


#pragma region Ship
double Ship::meanHandlingTime = 8;
double Ship::varFrac = 0.1;
double Ship::varHandlingTime = 4;

#pragma region Constructors
void Ship::construct(ulong uniqueId, double arrivalTimeLT, double handlingTimeLT, int prefBerth) {
	this->uniqueId = uniqueId;
	this->arrivalTimeLT = arrivalTimeLT;
	this->handlingTimeLT = handlingTimeLT;
	this->prefBerth = prefBerth;
	this->moored = false;
	this->arrived = false;
	this->finished = true;
	this->location = -1;
	this->noCranesSched = -1;
	this->noCranesAct = 0;
}

Ship::Ship()
{
	uniqueId = -1;
}

Ship::Ship(ulong uniqueId, double endOfPeriod, int noLoc)
{
	double arrivalTimeLT = Stoch::generateUniformRand(0, endOfPeriod);
	double handlingTime = Stoch::generateLogNormRand(meanHandlingTime, varHandlingTime);
	int prefBerth = int(Stoch::generateUniformRand(0, noLoc));
	construct(uniqueId, arrivalTimeLT, handlingTime, prefBerth);
}

Ship::Ship(ulong uniqueId, double arrivalTimeLT, double handlingTime, int prefBerth) {
	construct(uniqueId, arrivalTimeLT, handlingTime, prefBerth);
}
#pragma endregion
#pragma region Ship in TimeHorizon
bool Ship::askInTimeHorizon(double currentTime, double planningHorizon) {
	return arrivalTimeLT < (currentTime + planningHorizon);
}

void Ship::putInTimeHorizon()
{
	startOfServiceSched = numeric_limits<double>::max();
	arrivalTimeExp = arrivalTimeLT;
	//IMPROVE more precise (maybe)
}

bool Ship::updateExpectedArrival(double timeInterval, double currentTime)
{
	bool justArrived = false;
	arrivalTimeExp += Stoch::generateTimeDeviation(timeInterval, varFrac);
	if (arrivalTimeExp < currentTime) {
		setWaiting(currentTime);
		justArrived = true;
	}
	return justArrived;
}
#pragma endregion
#pragma region Ship Waiting
void Ship::setWaiting(double currentTime) {
	arrivalTimeAct = arrivalTimeExp;
	arrived = true;
	arrivalTimeAct = currentTime;
}
#pragma endregion
#pragma region shipProcessing

void Ship::putOnQuay(double currentTime) {
	arrived = true;
	startOfServiceAct = currentTime;
	waitingTime = currentTime - arrivalTimeAct;
	handlingTimeExp = handlingTimeLT;
}

bool Ship::updateHandlingTime(double timeInterval, int noQuayCranes)
{
	bool justFinished = false;
	if (noQuayCranes > 0)
		handlingTimeExp = handlingTimeExp - Stoch::generateWorkPerformed(timeInterval * noQuayCranes / stdQuayCranes, varFrac);

	if (handlingTimeExp < 0) {
		justFinished = true;
	}
	return justFinished;
}

void Ship::leaveQuay(double currentTime)
{

	handlingTimeAct = currentTime - startOfServiceAct;
	portStayTime = currentTime - arrivalTimeAct;
	completionTime = currentTime;
}
#pragma endregion
#pragma region utilities
bool Ship::isEarlier(Ship ship1, Ship ship2)
{
	return ship1.arrivalTimeLT < ship2.arrivalTimeLT;
}

double Ship::getHandlingTime(bool expected)
{
	double time = handlingTimeLT;
	if (expected)
		time = handlingTimeExp;
	if (noCranesSched == -1)
		return time;
	else return time * noCranesSched / stdQuayCranes;
}

double Ship::getTotalHandlingTime(bool expected)
{
	if (expected)
		return handlingTimeExp *stdQuayCranes;
	else return handlingTimeLT *stdQuayCranes;
}

string Ship::CSVHeader(string delim)
{
	ostringstream stream;
	stream << "uniqueId" << delim << "arrivalTimeLT" << delim << "arrivalTimeAct" << delim << "handlingTimeLT" << delim << "handlingTimeAct"
		<< delim << "startOfServiceAct" << delim << "completionTime" << delim << "location" << delim << "prefBerth" << delim << "noCranesAct" << delim << "noCranesSched";
	return stream.str();

}

string Ship::toCSV(bool onlyArrived, string delim) {
	ostringstream stream;
	if (!onlyArrived || finished)
	{
		stream << uniqueId << delim << arrivalTimeLT << delim << arrivalTimeAct << delim << handlingTimeLT << delim << handlingTimeAct
			<< delim << startOfServiceAct << delim << completionTime << delim << location << delim << prefBerth << delim << noCranesAct << delim << noCranesSched;
		return stream.str();
	}
	else return "";

}
#pragma endregion
#pragma endregion


#pragma region QuayCrane
QuayCrane::QuayCrane(int totalMax, int shipMax) : totalMax(totalMax), totalAssigned(0), shipMax(shipMax)
{}

bool QuayCrane::assignMax(Ship* current) {
	int diff = current->noCranesSched - current->noCranesAct;
	bool overload = diff + totalAssigned > totalMax;
	if (overload)
		diff = totalMax - totalAssigned;
	current->noCranesAct += diff;
	totalAssigned += diff;
	return overload;
}

void QuayCrane::moor(Ship* current)
{
	if (assignMax(current))
		waiting.push_back(current);
}

void QuayCrane::leave(Ship* current)
{
	totalAssigned -= current->noCranesAct;
	bool capacityReached = false;
	auto ship = waiting.begin();
	while (!capacityReached &&  ship != waiting.end()) 
	{
		capacityReached = assignMax(*ship);
		if (!capacityReached) 
			waiting.erase(ship++);
	}

}

#pragma endregion


#pragma region Schedule
#pragma region constructors
Schedule::Schedule()
{		}

Schedule::Schedule(ulong noShips, double simulationPeriod, int noLoc)
{
	this->noShips = noShips;
	this->simulationPeriod = simulationPeriod;
	this->shipSchedule = shipVector(noShips);

	for (ulong i = 0; i < noShips; i++) {
		shipSchedule[i] = Ship(i, simulationPeriod, noLoc);
	}

	sort(shipSchedule.begin(), shipSchedule.end(), Ship::isEarlier);

	// reset uniqueId according to Schedule
	for (ulong i = 0; i < noShips; i++) {
		shipSchedule[i].uniqueId = i;
	}
}

Schedule::Schedule(Schedule & sched)
{
	this->noShips = sched.noShips;
	this->simulationPeriod = sched.simulationPeriod;
	shipSchedule = shipVector(this->noShips);
	for (ulong i = 0; i < noShips; i++) {
		shipSchedule[i] = Ship(*(sched.getShip(i)));
	}
}
#pragma endregion
#pragma region Utilities
dataMatrix Schedule::plotData() {
	dataMatrix arrivalData(2, dataVector(noShips));
	for (ulong i = 0; i < noShips; i++) {
		arrivalData[0][i] = i;
		arrivalData[1][i] = shipSchedule[i].arrivalTimeLT;
	}
	return arrivalData;
}

Ship* Schedule::getShip(ulong shipNumber)
{
	assert(shipNumber < noShips);
	return &shipSchedule[shipNumber];
}

void Schedule::setLocations(Schedule sched)
{	iterLin(i, noShips) {
		getShip(i)->prefBerth = sched.getShip(i)->location;
	}
}

string Schedule::toCSV(bool noWait, string delim) {
	string message = Ship::CSVHeader();
	iterLin(i, noShips) {
		string line = shipSchedule[i].toCSV(noWait, delim);
		if (!line.empty())
			message = message + "\n" + line;
	}
	return message;
}
#pragma endregion
#pragma endregion


#pragma region PerfInd
PerfInd::PerfInd()
{
}

PerfInd::PerfInd(PerformanceIndicators * P, string name)
{
	sum = 0;
	quadsum = 0;
	noValues = 0;
	indicator = P->addIndicatorBox();
	value = P->addValueBox();
	this->name = name;
}

ulong PerfInd::getCount()
{
	return noValues;
}

void PerfInd::addValue(double value)
{
	sum += value;
	quadsum += value * value;
	noValues++;
}

double PerfInd::getMean()
{
	if (noValues > 0)
		return sum / noValues;
	else return 0;
}

double PerfInd::getStdDevAvg() {
	return getStdDev() / sqrt(noValues);
}

double PerfInd::getStdDev() {
	if (noValues > 1)
		return sqrt( (quadsum + (sum * sum /  noValues) )/( noValues - 1));
	else return 0;

}
void PerfInd::print()
{
	indicator->setText(to_QString(name));
	value->setText(to_QString(to_string(getMean())));
}
# pragma endregion


#pragma region Planner	
#pragma region constructors
Planner::Planner() {
}

Planner::Planner(MainWindow * w, Schedule arrivalTimes, double noLocations, double smallStep, double largeStep, double planningHorizon, double posCost) :
	waitingTime(PerfInd(w->getPI(), "waiting time")), handlingTime(PerfInd(w->getPI(), "handling time")),
	portStayTime(PerfInd(w->getPI(), "port stay time")), solutionTime(PerfInd(w->getPI(), "MILP solution time")),
	MIPGap(PerfInd(w->getPI(), "MIP Gap")), locSwitchRatio(PerfInd(w->getPI(), "berth switch ratio")),
	NoShipsInSched(PerfInd(w->getPI(), "Ships in Schedule"))
{
	this->mainWindow = w;
	this->arrivalTimes = arrivalTimes;
	this->smallStep = smallStep;
	this->largeStep = largeStep;
	this->noLocations = noLocations;
	this->currentUniqueId = 0;
	this->currentTime = 0;
	this->planningHorizon = planningHorizon;
	this->noShips = arrivalTimes.noShips;
	this->arrivalExpected = list<Ship*>(0);
	this->waiting = list<Ship*>(0);
	this->moored = list<Ship*>(0);
	this->posCost = posCost;
	this->alpha = 0;
}

#pragma endregion
#pragma region smallUpdate
void Planner::smallUpdate()
{
	currentTime += smallStep;
	statistics();
	// look which ships are going to arrive witihin the TimeHorizon
	for (; currentUniqueId < noShips &&
		arrivalTimes.getShip(currentUniqueId)->askInTimeHorizon(currentTime, planningHorizon);
		currentUniqueId++) {
		putInTimeHorizon(arrivalTimes.getShip(currentUniqueId));
	}

	// determine arrival times and put arrived ships in waiting
	for (auto num = arrivalExpected.begin(); num != arrivalExpected.end();) {
		Ship* current = *num; // As above
		++num;
		if (current->updateExpectedArrival(smallStep, currentTime)) {
			setWaiting(current);
		}
	}

	// estimate handling time and remove finished ships from the quay
	for (auto num = moored.begin(); num != moored.end();) {
		Ship* current = *num; // As above
		++num;
		if (updateHandlingTimes(current)) {
			leaveQuay(current);
			freeLocation(current->location); // puts ship on Quay at free location
		}
	}

}

bool Planner::updateHandlingTimes(Ship* newShip)
{
	return newShip->updateHandlingTime(smallStep);
}

bool Planner::locationOccupied(int location) {
	if (moored.size() > 0) {
		for (auto num = moored.begin(); num != moored.end() || location < 0; ++num) {
			Ship * current = *num; // As above
			if (current->location == location) {
				return true;
			}
		}
	}
	return false;
}

void Planner::freeLocation(int location) {
	// set ship on free location
	double minTime = numeric_limits<double>::max();
	Ship* bestFitship = &Ship();
	for (auto num = waiting.begin(); num != waiting.end(); ++num) {
		Ship * current = *num; // As above
		if (current->location == location && current->startOfServiceSched < minTime) {
			bestFitship = current;
			minTime = current->startOfServiceSched;
		}
	}
	if (minTime != numeric_limits<double>::max()) {
		putOnQuay(bestFitship);
	}
}

void Planner::putInTimeHorizon(Ship * newShip) {
	newShip->putInTimeHorizon();
	arrivalExpected.push_back(newShip);
}

void Planner::setWaiting(Ship * newShip) {
	int loc = newShip->location;
	if (loc >= 0 && !locationOccupied(loc)) {
		putDirectlyOnQuay(newShip);
	}

	else {
		// Else Put Ship In Waiting Line
		newShip->setWaiting(currentTime);
		arrivalExpected.remove(newShip);
		waiting.push_back(newShip);
	}
}

void Planner::putOnQuay(Ship * newShip) {
	// Ship from waiting to Quay
	newShip->putOnQuay(currentTime);
	assignCrane(newShip);
	waiting.remove(newShip);
	moored.push_back(newShip);
}

void Planner::putDirectlyOnQuay(Ship * newShip) {
	// ship from arrival to quay
	newShip->putOnQuay(currentTime);
	assignCrane(newShip);
	arrivalExpected.remove(newShip);
	moored.push_back(newShip);
}

void Planner::leaveQuay(Ship * newShip) {
	newShip->leaveQuay(currentTime);
	freeCrane(newShip);
	moored.remove(newShip);
	waitingTime.addValue(newShip->waitingTime);
	portStayTime.addValue(newShip->portStayTime);
	handlingTime.addValue(newShip->handlingTimeAct);

}

#pragma endregion
#pragma region LargeUpdate
void Planner::largeUpdate()
{
	dataVector arrExp;// expected Arrival Times (relative to currentTime
	dataVector handExp; // expected Handling Times
	dataVector priorities; // importance of delay
	dataVector prefBerth;
	shipsInSchedule = shipRVector();
	dataVector completionTime(noLocations);

	iterList(ref, arrivalExpected) {
		Ship* current = *ref;
		shipsInSchedule.push_back(current);
		arrExp.push_back(current->arrivalTimeExp - currentTime);
		handExp.push_back(current->getHandlingTime(false)); // in QCBAP totalHandling
		priorities.push_back(planningHorizon - (current->arrivalTimeExp - currentTime));
		prefBerth.push_back(current->prefBerth);
	}

	iterList(ref, waiting) {
		Ship* current = *ref;
		shipsInSchedule.push_back(current);
		arrExp.push_back(0);
		handExp.push_back(current->getHandlingTime(false)); // in QCBAP totalHandling
		priorities.push_back(planningHorizon);
		prefBerth.push_back(current->prefBerth);
	}

	iterList(ref, moored) {
		Ship* current = *ref;
		completionTime[current->location] = current->getHandlingTime(true);
	}

	solveMip(arrExp, handExp, completionTime, priorities, prefBerth, shipsInSchedule);
	iterLin(i, noLocations)
		if(!locationOccupied(i))
			freeLocation(i);
}

void Planner::solveMip(dataVector arrExp, dataVector handExp, dataVector completionTime,
	dataVector priorities, dataVector prefBerth, shipRVector shipsInSchedule)
{
	BAP problem = BAP(planningHorizon, completionTime, arrExp, handExp, priorities, prefBerth, posCost, alpha);
	problem.run();
	if (problem.isFeasible())
	{
		auto loc = problem.getLocation();
		auto sched = problem.getBerthTimes();
		iterLin(i, shipsInSchedule.size())
		{
			if (shipsInSchedule[i]->location >= 0)
				locSwitchRatio.addValue(shipsInSchedule[i]->location != loc[i]);
			shipsInSchedule[i]->location = loc[i];
			shipsInSchedule[i]->startOfServiceSched = sched[i] + currentTime;
		}
		solutionTime.addValue(problem.getSolutionTime());
		NoShipsInSched.addValue(shipsInSchedule.size());
		double currentMIPGap = problem.getMIPGAP();
		if (currentMIPGap < GRB_INFINITY)
			MIPGap.addValue(currentMIPGap);
	}
	else 
		mainWindow->text("Infeasible model encountered!!!");
}

#pragma endregion
#pragma region plot

void Planner::operationalPlot(OperationalOverview* o)
{
	auto pArriving = o->getArriving();
	auto pWaiting = o->getWaiting();
	auto pProcessing = o->getProcessing();

	assignPair(arrivingPlotData(), lArriving, dArriving);
	assignPair(waitingPlotData(), lWaiting, dWaiting);
	assignPair(processingPlotData(), lProcessing, dProcessing);

	dataVector locations(moored.size());
	int i = 0;
	iterList(ref, moored) {
		locations[i] = 1 + (*ref)->location;
		i++;
	}

	barPlot(pArriving, dArriving, lArriving, "arriving");
	setMaxbar(pArriving, planningHorizon);
	draw(pArriving);

	barPlot(pWaiting, dWaiting, lWaiting, "waiting");
	setMaxbar(pWaiting, planningHorizon / 2);
	draw(pWaiting);

	barPlot(pProcessing, dProcessing, locations, lProcessing, "processing", noLocations + 1);
	setMaxbar(pProcessing, Ship::meanHandlingTime * 1.5);
	draw(pProcessing);

	text::setNameString(o->getClock(), Times::toDate(currentTime));
	plotCraneCount(o);
}

void Planner::clock(OperationalOverview * o) {
	text::setNameString(o->getClock(), Times::toDate(currentTime));
}

void Planner::preparePlot(ScheduleOverview * s) {
	s->addTextBox(30);
	s->initialSettings(noLocations, planningHorizon, 4);
	s->setGrid(noLocations, planningHorizon);
}

void Planner::performancePlot()
{ 
	PerfList perfInd = getPerfInd();
	iterList(ref, perfInd) {
		(*ref)->print();
	}

}

void Planner::schedulePlot(ScheduleOverview * s)
{
	int noSchedShips = shipsInSchedule.size();
	intVector loc(noSchedShips);
	dataVector start(noSchedShips);
	dataVector end(noSchedShips);
	labelVector labels(noSchedShips);
	iterLin(i, noSchedShips)
	{
		Ship * current = shipsInSchedule[i];
		start[i] = current->startOfServiceSched - currentTime;
		end[i] = current->startOfServiceSched + current->getHandlingTime(false) - currentTime;
		loc[i] = current->location;
		labels[i] = scheduleShipLabels(current, false);
	}
	s->setData(loc, start, end, labels);
	auto colors = plot::toColor(start, loc, 2);
	s->setColor(colors);

	int noCurrent = moored.size();
	intVector locCurrent(noCurrent);
	dataVector endCurrent(noCurrent);
	labelVector labelsCurrent(noCurrent);

	auto ref = moored.begin();
	iterLin(i, noCurrent) {
		Ship* current = *ref;
		locCurrent[i] = current->location;
		endCurrent[i] = current->getHandlingTime(true);
		labelsCurrent[i] = scheduleShipLabels(current, true);
		ref++;
	}

	s->addData(locCurrent, endCurrent, labelsCurrent, QColor(0, 0, 0));

	s->updateGrid(currentTime);
}

string Planner::scheduleShipLabels(Ship * current, bool moored)
{
	if (moored)
	{
		double complTime = current->getHandlingTime(true);
		return "no:" + to_string(current->uniqueId) + "\ntill" + Times::toHour(complTime + currentTime);
	}

	else
	{
		double start = current->startOfServiceSched;
		double end = start + current->getHandlingTime(false);
		return "no:" + to_string(current->uniqueId) + "\nfrom " + Times::toHour(start) + "to " +
			Times::toHour(end);
	}

}

pair<labelVector, dataVector> Planner::arrivingPlotData()
{
	int num = arrivalExpected.size();
	labelVector labels(num);
	dataVector data(num);
	int i = 0;
	for (auto it = arrivalExpected.begin(); it != arrivalExpected.end(); it++) {
		data[i] = (*it)->arrivalTimeExp - currentTime;
		labels[i] = to_string((*it)->uniqueId) + "\nloc: " + to_string((*it)->location) +
			"\neta: " + Times::toHour((*it)->arrivalTimeExp);
		i++;
	}
	return make_pair(labels, data);
}

pair<labelVector, dataVector> Planner::waitingPlotData()
{
	int num = waiting.size();
	labelVector labels(num);
	dataVector data(num);
	int i = 0;
	for (auto it = waiting.begin(); it != waiting.end(); it++) {
		data[i] = currentTime - (*it)->arrivalTimeAct;
		labels[i] = to_string((*it)->uniqueId) + "\nloc: " + to_string((*it)->location); // evt location
		i++;
	}
	return make_pair(labels, data);
}

pair<labelVector, dataVector> Planner::processingPlotData()
{
	int num = moored.size();
	labelVector labels(num);
	dataVector data(num);
	int i = 0;
	for (auto it = moored.begin(); it != moored.end(); it++) {
		data[i] = (*it)->getHandlingTime(true);
		labels[i] = to_string((*it)->uniqueId); // evt location
		i++;
	}
	return make_pair(labels, data);
}
#pragma endregion
#pragma region utilities
Schedule Planner::getSchedule()
{
	return arrivalTimes;
}
string Planner::writeCSVString()
{ 
	string l1 = "";
	string l2 = "";
	string l3 = "";
	string l4 = "";
	string l5 = "";
	PerfList perfList = getPerfInd();
	iterList(ref, perfList){
		string name = (*ref)->name;
		name.erase(remove_if(name.begin(), name.end(), isspace), name.end());
		l1 += name + "; ";
		l2 += to_string((*ref)->getMean()) + "; ";
		l3 += to_string((*ref)->getStdDev()) + "; ";
		l4 += to_string((*ref)->getStdDevAvg()) + "; ";
		l5 += to_string((*ref)->getCount()) + "; ";
		}
	return l1 + '\n' + l2 + '\n' + l3 + '\n' + l4 + '\n' + l5;
}

PerfList Planner::getPerfInd()
{ 
	PerfList perfList(0);
	perfList.push_back(&waitingTime);
	perfList.push_back(&portStayTime);
	perfList.push_back(&handlingTime);
	perfList.push_back(&solutionTime);
	perfList.push_back(&MIPGap);
	perfList.push_back(&locSwitchRatio);
	perfList.push_back(&NoShipsInSched);
	return perfList;
}
#pragma endregion
#pragma endregion


#pragma region QCPlanner
QCPlanner::QCPlanner():
	quayCrane(QuayCrane(0,0)){}

QCPlanner::QCPlanner(MainWindow * w, Schedule arrivalTimes, double noLocations, double smallStep, double largeStep, double planningHorizon,
		 double posCost, double maxQCTot, double maxQCShip):
	quayCrane(QuayCrane(maxQCTot, maxQCShip)), craneAvailability(PerfInd(w->getPI(), "crane availability")), 
	cranesInUse(PerfInd(w->getPI(), "cranes in use")), QCSwitchRatio(PerfInd(w->getPI(), "crane switch ratio")),
	Planner(w, arrivalTimes, noLocations, smallStep, largeStep, planningHorizon, posCost)
{}


void QCPlanner::assignCrane(Ship * newShip)
{
	quayCrane.moor(newShip);
}

void QCPlanner::freeCrane(Ship * newShip)
{
	quayCrane.leave(newShip);
}

bool QCPlanner::updateHandlingTimes(Ship * newShip)
{	craneAvailability.addValue((double) newShip->noCranesAct / newShip->noCranesSched);
	return newShip->updateHandlingTime(smallStep, newShip->noCranesAct);

}

void QCPlanner::statistics() {
	cranesInUse.addValue(quayCrane.totalAssigned);
}

void QCPlanner::largeUpdate()
{
	dataVector arrExp;// expected Arrival Times
	dataVector totalHandExp; // expected Handling Times
	dataVector priorities; // importance of delay
	dataVector prefBerth;
	shipsInSchedule = shipRVector();
	dataVector completionTime(noLocations);
	intVector currentCranes(noLocations);

	iterList(ref, arrivalExpected) {
		Ship* current = *ref;
		shipsInSchedule.push_back(current);
		arrExp.push_back(current->arrivalTimeExp - currentTime);
		totalHandExp.push_back(current->getTotalHandlingTime(false));
		priorities.push_back(planningHorizon - (current->arrivalTimeExp - currentTime));
		prefBerth.push_back(current->prefBerth);

	}

	iterList(ref, waiting) {
		Ship* current = *ref;
		shipsInSchedule.push_back(current);
		arrExp.push_back(0);
		totalHandExp.push_back(current->getTotalHandlingTime(false));
		priorities.push_back(planningHorizon);
		prefBerth.push_back(current->prefBerth);
	}

	iterList(ref, moored) {
		Ship* current = *ref;
		completionTime[current->location] = current->handlingTimeExp;
		currentCranes[current->location] = current->noCranesSched;
	}

	solveMip(arrExp, totalHandExp, completionTime, priorities, prefBerth, shipsInSchedule, currentCranes);
	iterLin(i, noLocations)
		if (!locationOccupied(i))
			freeLocation(i);
}

void QCPlanner::solveMip(dataVector arrExp, dataVector totalHandExp, dataVector completionTime,
	dataVector priorities, dataVector prefBerth, shipRVector shipsInSchedule, intVector currentCranes)
{
	QCBAP problem = QCBAP(planningHorizon, completionTime, arrExp, totalHandExp, priorities, prefBerth, posCost, quayCrane.totalMax, quayCrane.shipMax, currentCranes);
	problem.run();
	if (problem.isFeasible())
	{
		intVector loc = problem.getLocation();
		dataVector sched = problem.getBerthTimes();
		intVector qc = problem.getQC();

		iterLin(i, shipsInSchedule.size()) {
			if (shipsInSchedule[i]->location >= 0) {
				locSwitchRatio.addValue(shipsInSchedule[i]->location != loc[i]);
				QCSwitchRatio.addValue(shipsInSchedule[i]->noCranesSched != qc[i]);
			}
			shipsInSchedule[i]->location = loc[i];
			shipsInSchedule[i]->startOfServiceSched = sched[i] + currentTime;
			shipsInSchedule[i]->noCranesSched = qc[i];
		}

		solutionTime.addValue(problem.getSolutionTime());
		NoShipsInSched.addValue(shipsInSchedule.size());
		double currentMIPGap = problem.getMIPGAP();
		if (currentMIPGap < GRB_INFINITY) 
			MIPGap.addValue(currentMIPGap);
	}
	else
		mainWindow->text("Infeasible model encountered!!!");
	

}

void QCPlanner::plotCraneCount(OperationalOverview* o)
{	iterList(ref, moored)
		craneAvailability.addValue( (*ref)->noCranesAct * 1.0 / (*ref)->noCranesSched);
	cranesInUse.addValue(quayCrane.totalAssigned);
	text::setNameString(o->getQCCount(), to_string(quayCrane.totalAssigned) + " QCs used");
}

pair<labelVector, dataVector> QCPlanner::processingPlotData()
{
	int num = moored.size();
	labelVector labels(num);
	dataVector data(num);
	int i = 0;
	for (auto it = moored.begin(); it != moored.end(); it++) {
		data[i] = (*it)->handlingTimeExp * Ship::stdQuayCranes;
		labels[i] = to_string((*it)->uniqueId) + "\nQC" + to_string((*it)->noCranesAct) + '/' + to_string((*it)->noCranesSched);
		i++;
	}
	return make_pair(labels, data);
}

string QCPlanner::scheduleShipLabels(Ship * current, bool moored)
{
	if (moored)
	{
		double complTime = current->handlingTimeExp;
		return "no:" + to_string(current->uniqueId) + "\ntill" + Times::toHour(complTime + currentTime) + "\nQC " +
			to_string(current->noCranesAct) + '/' + to_string(current->noCranesSched);
	}

	else
	{
		double start = current->startOfServiceSched;
		double end = start + current->getHandlingTime(false);
		return "no:" + to_string(current->uniqueId) + "\nfrom " + Times::toHour(start) + "to " +
			Times::toHour(end) + "\nQC " + to_string(current->noCranesSched);
	}

}

PerfList QCPlanner::getPerfInd()
{ PerfList perfList = Planner::getPerfInd();
	perfList.push_back(&cranesInUse);
	perfList.push_back(&craneAvailability);
	perfList.push_back(&QCSwitchRatio);
	return perfList;
	
}


#pragma endregion



