#include "milp.h"


void testProblem(MainWindow* w, bool cranes) {
	double planningHorizon = 24;
	dataVector expCompletionTimes = { 2,4 };
	dataVector expArrivalTimes = { 1,6,10 };
	dataVector expHandlingTimesTOT = { 24, 24, 24 };
	dataVector expHandlingTimes = { 12, 12, 12 };
	dataVector priorities = { 1,1, 1 };
	dataVector prefBerth = { 0,0, 0 };
	double posCost = 0;
	double maxQTot = 6;
	double maxQship = 4;
	intVector currentNoQC = { 3,3 };

	// add also QC constraints
	// Why not automatically max no cranes??

	if (cranes)
	{
		QCBAP prob = QCBAP(planningHorizon, expCompletionTimes, expArrivalTimes, expHandlingTimesTOT, priorities, prefBerth, posCost, maxQTot, maxQship, currentNoQC);
		prob.run();
		prob.writeSolution(w);
		prob.plotSolution(w->getScheduleOverview());
	}

	else
	{
		BAP prob = BAP(planningHorizon, expCompletionTimes, expArrivalTimes, expHandlingTimes, priorities, prefBerth, posCost, 0);
		prob.run();
		prob.writeSolution(w);
		prob.plotSolution(w->getScheduleOverview());
	}

}


// CLASS MILP
#pragma region MILP
	// constructor
		MILP::MILP():env(GRBEnv()), model(GRBModel(env)){};

	// run
		void MILP::run() {
			addVariables();
			addAllConstraints();
			model.getEnv().set(GRB_DoubleParam_MIPGap, 0.01);
			model.getEnv().set(GRB_DoubleParam_TimeLimit, 5);
			model.optimize();
			runTime = model.get(GRB_DoubleAttr_Runtime);
			if (getMIPGAP()> 0.02)
			{
				model.getEnv().set(GRB_DoubleParam_MIPGap, 0.02);
				model.getEnv().set(GRB_DoubleParam_TimeLimit, 10);
				model.optimize();
				runTime += model.get(GRB_DoubleAttr_Runtime);
				if (getMIPGAP() > 0.05)
				{
					model.getEnv().set(GRB_DoubleParam_MIPGap, 0.05);
					model.getEnv().set(GRB_DoubleParam_TimeLimit, 15);
					runTime += model.get(GRB_DoubleAttr_Runtime);
				}
			}
			feasible = (model.get(GRB_IntAttr_SolCount) > 0);
		}


	// utilities
		GRBLinExpr MILP::sum(varVector variables, double coeff)
		{
			GRBLinExpr expr;
			iterList(var, variables) {
				expr += coeff * (*var);
			}
			return expr;
		}

		dataVector MILP::toDataVector(varVector variables) {
			int size = variables.size();
			dataVector data(size);
			iterLin(i, size) {
				data[i] = variables[i].get(GRB_DoubleAttr_X);
			}
			return data;
		}

		intVector MILP::toIntVector(varVector variables)
		{
			int size = variables.size();
			intVector data(size);
			iterLin(i, size) {
				data[i] = variables[i].get(GRB_DoubleAttr_X);
			}
			return data;

		}

		intVector MILP::toIntVector(varMatrix variables)
		{
			int size = variables.size();
			int size2 = 0;
			if (size > 0)
				size2 = variables[0].size();
			intVector data(size);
			iterLin(i, size) iterLin(j, size2) {
				if (variables[i][j].get(GRB_DoubleAttr_X) == 1)
					data[i] = j;
			}
			return data;
		}

		dataMatrix MILP::toDataMatrix(varMatrix variables)
		{
			dataMatrix data =  dataMatrix(variables.size() , dataVector());
			iterLin(i, variables.size()){
				data[i] = dataVector(variables[i].size());
				iterLin(j, variables[i].size())
					data[i][j] = variables[i][j].get(GRB_DoubleAttr_X);
			}
			return data;
		}

	// variables
		varMatrix MILP::createTriag(int size, double obj, double max, char GRBtype) {

			varMatrix aux = varMatrix(size, varVector());
			iterLin(i, size) {
				aux[i] = varVector(i);
				iterLin(j, i) {
					aux[i][j] = model.addVar(0, max, obj, GRBtype, "");
				}
			}

			return aux;
		}

		varMatrix MILP::createRect(int size1, int size2, double obj, double max, char GRBtype) {

			varMatrix aux = varMatrix(size1, varVector());
			iterLin(i, size1) {
				aux[i] = varVector(size2);
				iterLin(j, size2) {
					aux[i][j] = model.addVar(0, max, obj, GRBtype, "");
				}
			}

			return aux;
		}

		varVector MILP::createLin(int size, double obj, double max, char GRBtype, double min) {
			varVector aux = varVector(size);
			iterLin(i, size) {
				aux[i] = model.addVar(min, max, obj, GRBtype, "");
			}
			return aux;
		}

		varVector MILP::createLin(int size, dataVector obj, double max, char GRBtype, double min) {
			varVector aux = varVector(size);
			iterLin(i, size) {
				aux[i] = model.addVar(min, max, obj[i], GRBtype, "");
			}
			return aux;
		}

		varVector MILP::createLin(int size, double obj, double max, char GRBtype, int branchPrior, double min)
		{
			varVector aux = createLin(size, obj, max, GRBtype, min);
			model.update();
			iterLin(i, size)
				aux[i].set(GRB_IntAttr_BranchPriority, branchPrior);
			return aux;
		}

		varMatrix MILP::createRect(int size1, int size2, double obj, double max, char GRBtype, int branchPrior)
		{
			varMatrix aux = createRect(size1, size2, obj, max, GRBtype);
			model.update();
			iterLin(i, size1) {
				iterLin(j, size2) {
					aux[i][j].set(GRB_IntAttr_BranchPriority, branchPrior);
				}
			}

			return aux;

		}


	// output
		double MILP::getSolutionTime()
		{
			return runTime;
		}

		bool MILP::isFeasible()
		{
			return feasible;
		}

		double MILP::getMIPGAP() {
			if (!nonEmptyModel())
				return 0;
			try {
				return model.get(GRB_DoubleAttr_MIPGap);
			}
			catch (...) {
				return GRB_INFINITY;
			}
		}



#pragma endregion
// CLASS BAP
#pragma region BAP
	// constructor
		BAP::BAP(double planningHorizon, dataVector expCompletionTimes, dataVector expArrivalTimes, dataVector expHandlingTimes,
				dataVector priorities, dataVector prefBerth, double posCost, double alpha)
		{
			this->noLocations = expCompletionTimes.size();
			this->noShips = expArrivalTimes.size();
			this->priorities = priorities;
			this->bigMWaiting = 100 * planningHorizon;
			this->expCompletionTimes = expCompletionTimes;
			this->expArrivalTimes = expArrivalTimes;
			this->expHandlingTimes = expHandlingTimes;
			this->prefBerth = prefBerth;
			this->posCost = posCost;
			this->alpha = alpha;
		}

	// run

		void BAP::addVariables()
		{
			location = varMatrix(noShips, varVector()); // add location variables
			iterLin(i, noShips) {
				location[i] = varVector(noLocations);
				iterLin(j, noLocations) {
					double obj = posCost * abs(prefBerth[i] - j) * priorities[i]; // cost of berthing ship i at berth j
					location[i][j] = model.addVar(0, 1, obj, GRB_BINARY, "");
				}
			}
			scheduleStart = createLin(noShips, priorities, DOUBLE_MAX, GRB_CONTINUOUS);
			aux1 = createRect(noShips, noShips, 0, 1, GRB_BINARY);
			aux2 = createTriag(noShips, 0, 1, GRB_BINARY);
			model.update();
		}

		void BAP::addAllConstraints()
		{
			addArrivalTimeConstr();
			addUniqeLocationConstr();
			addNoOverlapConstr();
			addRespectCurrentShips();
			model.update();
		}

	// constraints
		void BAP::addArrivalTimeConstr()
		{
			iterLin(i, noShips) {
				model.addConstr(scheduleStart[i] >= expArrivalTimes[i] + alpha);
			}
		}

		void BAP::addUniqeLocationConstr()
		{
			iterLin(i, noShips)
				model.addConstr(sum(location[i], 1) == 1);
		}

		void BAP::addNoOverlapConstr()
		{ // AUX1 & 2
			iterRectNoEq(i, j, noShips)
				model.addConstr(scheduleStart[i] - scheduleStart[j] - expHandlingTimes[j] - 3 * alpha >= -bigMWaiting *aux1[i][j]);

			iterLin(j, noShips)
				model.addConstr(aux1[j][j] == 0);

			iterTriangle(i, j, noShips) {
				iterLin(k, noLocations)
					model.addConstr(location[i][k] + location[j][k] - 1 <= aux2[i][j]);

				// Assure that always one condition is broken
				model.addConstr(aux1[i][j] + aux1[j][i] + aux2[i][j] <= 2);
			}
		}

		void BAP::addRespectCurrentShips()
		{
			iterLin(i, noShips) iterLin(k, noLocations) {
				model.addConstr(scheduleStart[i] - expCompletionTimes[k] >= (-1 + location[i][k]) * bigMWaiting);
			}
		}

	// write
		void BAP::writeSolution(MainWindow * w) {
			w->text("location");
			w->text(text::toString(toDataMatrix(location)));

			w->text("schedule start");
			w->text(text::toString(toDataVector(scheduleStart)));

			w->text("aux1");
			w->text(text::toString(toDataMatrix(aux1)));
			
			w->text("aux2");
			w->text(text::toString(toDataMatrix(aux2)));
		}

		dataVector BAP::getBerthTimes()
		{ 
		return toDataVector(scheduleStart);
		}

		intVector BAP::getLocation()
		{
			return toIntVector(location);
		}

		bool BAP::infeasible()
		{
			return  model.get(GRB_IntAttr_Status) != 2 ;
		}

		void BAP::plotSolution(ScheduleOverview * s, double planningHorizon)
		{	
			s->addTextBox(30);
			s->initialSettings(noLocations, planningHorizon, 4);
			s->setGrid(noLocations, planningHorizon);
		
			auto loc = toIntVector(location);
			auto start = toDataVector(scheduleStart);
			int noSchedShips = start.size();

			dataVector end(noSchedShips);
			labelVector labels(noSchedShips);
			iterLin(i, noSchedShips)
			{
				end[i] = start[i] + expHandlingTimes[i];
				labels[i] = "no:" + to_string(i) + "\n from " + Times::toHour(start[i]) + "to " +
							Times::toHour(end[i]);
			}
			s->setData(loc, start, end, labels);
			auto colors = plot::toColor(start, loc, 2);
			s->setColor(colors);

			dataVector endCurrent = expCompletionTimes;
			intVector locCurrent = intVector(endCurrent.size());
			labelVector labelsCurrent = labelVector(endCurrent.size());

			iterLin(i, endCurrent.size()) 
			{
				labelsCurrent[i] = "\n till " + Times::toHour(endCurrent[i]);
				locCurrent[i] = i;
			}
	

			s->addData(locCurrent, endCurrent, labelsCurrent, QColor(0, 0, 0));

			

			s->saveSchedule("BAPtestMILP.png");
		}

	// utils
		bool BAP::nonEmptyModel() 
		{
		return (noShips > 0);
		}

#pragma endregion
// CLASS QCBAP
#pragma region QCBAP
	// constructor

		QCBAP::QCBAP(double planningHorizon, dataVector expCompletionTimes, dataVector expArrivalTimes, dataVector expHandlingTimes,
			dataVector priorities, dataVector prefBerth, double posCost, double maxQTot, double maxQship, intVector currentNoQC) :
			maxQTot(maxQTot), maxQship(maxQship), currentNoQC  (currentNoQC), bigMCranes(maxQship),
			BAP(planningHorizon, expCompletionTimes, expArrivalTimes, expHandlingTimes, priorities, prefBerth, posCost,0)
		{}

	// run
		void QCBAP::addVariables() {
			BAP::addVariables();
			handlingTime = createLin(noShips, priorities, DOUBLE_MAX, GRB_CONTINUOUS);
			noQuayCranes = createLin(noShips, 0, maxQship, GRB_INTEGER, 5, 1);
			aux3 = createRect(noShips, noShips, 0, 1, GRB_BINARY);
			aux1loc = createRect(noShips, noLocations, 0, 1, GRB_BINARY);
			auxK = createRect(noShips, noShips, 0, maxQship, GRB_INTEGER);
			auxKloc = createRect(noShips, noLocations, 0, maxQship, GRB_INTEGER);
			model.update();
		}

		void QCBAP::addAllConstraints() {
			BAP::addAllConstraints();
			addNumberOfQC();
			addHandlingTime();
		}


	// constraints
		void QCBAP::addNoOverlapConstr()
		{ // AUX1 & 2
			iterRectNoEq(i, j, noShips)
				model.addConstr(scheduleStart[i] - scheduleStart[j] - handlingTime[j] >= -bigMWaiting *aux1[i][j]);
			iterLin(j, noShips)
				model.addConstr(aux1[j][j] == 0);
			iterTriangle(i, j, noShips) {
				iterLin(k, noLocations)
					model.addConstr(location[i][k] + location[j][k] - 1 <= aux2[i][j]);

				// Assure that always one condition is broken
				model.addConstr(aux1[i][j] + aux1[j][i] + aux2[i][j] <= 2);
			}
		}

		void QCBAP::addNumberOfQC()
		{	// future ships
			iterRectNoEq(j1, j2, noShips) {
				model.addConstr(scheduleStart[j2] - scheduleStart[j1] -EPSILON >= - bigMWaiting * aux3[j1][j2]);
				iterLin(j, noShips)
					model.addConstr(aux3[j][j] == 0);
				model.addConstr(auxK[j1][j2] >= - bigMCranes * (2 - aux1[j1][j2] - aux3[j1][j2] ) + noQuayCranes[j2]);
			}
			// current ships
			iterLin(j, noShips)
				iterLin(i, noLocations) {
				model.addConstr(scheduleStart[j] - expCompletionTimes[i] >= -bigMWaiting * aux1loc[j][i]);
				model.addConstr(auxKloc[j][i] >= -bigMCranes * (1 - aux1loc[j][i]) + currentNoQC[i]);
			}

			// check number of Quay Cranes
			iterLin(j, noShips) {
				model.addConstr(noQuayCranes[j] + sum(auxK[j]) + sum(auxKloc[j]) <= maxQTot);
			}
		}

		void QCBAP::addHandlingTime()
		{
			iterLin(j, noShips)
				for (double n = 1; n <= maxQship; n++)
					model.addConstr(handlingTime[j] >= expHandlingTimes[j] * (2 / n - noQuayCranes[j] / (n * n)));
		}

	// write
		void QCBAP::writeSolution(MainWindow* w)
		{	w->text("status:=" + to_string(model.get(GRB_IntAttr_Status)) +"\n");
			if (model.get(GRB_IntAttr_Status) == 2)
			{
				w->text("location");
				w->text(text::toString(toDataMatrix(location)));

				w->text("schedule start");
				w->text(text::toString(toDataVector(scheduleStart)));

				w->text("no Quay Cranes");
				w->text(text::toString(toDataVector(noQuayCranes)));

				w->text("handling times");
				w->text(text::toString(toDataVector(handlingTime)));

				w->text("aux1");
				w->text(text::toString(toDataMatrix(aux1)));

				w->text("aux1loc");
				w->text(text::toString(toDataMatrix(aux1loc)));

				w->text("aux2");
				w->text(text::toString(toDataMatrix(aux2)));
		
				w->text("aux3");
				w->text(text::toString(toDataMatrix(aux3)));

				w->text("auxk");
				w->text(text::toString(toDataMatrix(auxK)));

				w->text("auxkloc");
				w->text(text::toString(toDataMatrix(auxKloc)));
			}

		}

		intVector QCBAP::getQC()
		{
			return toIntVector(noQuayCranes);
		}

		void QCBAP::plotSolution(ScheduleOverview * s, double planningHorizon)
		{
			s->addTextBox(30);
			s->initialSettings(noLocations, planningHorizon, 4);
			s->setGrid(noLocations, planningHorizon);

			auto loc = toIntVector(location);
			auto start = toDataVector(scheduleStart);
			auto hand = toDataVector(handlingTime);
			auto QC = toIntVector(noQuayCranes);
			int noSchedShips = start.size();
			

			dataVector end(noSchedShips);
			labelVector labels(noSchedShips);
			iterLin(i, noSchedShips)
			{
				end[i] = start[i] + hand[i];
				labels[i] = "no:" + to_string(i) + "\n from " + Times::toHour(start[i]) + "to " +
					Times::toHour(end[i]) + "\n" + to_string(QC[i]) + " QC";
			}
			s->setData(loc, start, end, labels);
			auto colors = plot::toColor(start, loc, 2);
			s->setColor(colors);

			dataVector endCurrent = expCompletionTimes;
			intVector locCurrent = intVector(endCurrent.size());
			labelVector labelsCurrent = labelVector(endCurrent.size());

			iterLin(i, endCurrent.size())
			{
				labelsCurrent[i] = "\n till " + Times::toHour(endCurrent[i]) + "\n" + to_string(QC[i]) + " QC";
				locCurrent[i] = i;
			}

			s->addData(locCurrent, endCurrent, labelsCurrent, QColor(0, 0, 0));

			s->updateGrid(0);

			s->saveSchedule("QCBAPtestMILP.png");
		}

#pragma endregion

 
