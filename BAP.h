#pragma once
#include "experiment.h"

void simpleTest(MainWindow * w);

void congestion(MainWindow* w);

void noBerths(MainWindow * w); // PURPOSE??

void variance(MainWindow * w);

void posCost(MainWindow * w);

void planHorizon(MainWindow * w, double fracVar = 0.1, double congestion = 0.75);

void reschedulePeriod(MainWindow * w, double fracVar = 0.1, double congestion = 0.75);

void alpha(MainWindow * w);

void posCostRobust(MainWindow * w);