#include "stochastic.h"

using namespace std;

// variables
		default_random_engine generator = Stoch::initGenerator();

// methods
	double Stoch::generateUniformRand(double min, double max)
	{
		uniform_real_distribution<double> distribution(min, max);
		return distribution(generator);
	}

	double Stoch::generateLogNormRand(double mean, double var){
	double m = log(mean / sqrt(1+ var / (mean * mean) ));
	double s = sqrt( log(1 + var / (mean * mean) ));

	 lognormal_distribution<double> distribution(m, s);
		return distribution(generator);
	}

	double Stoch::generateNormRand(double mean, double std)
	{	normal_distribution<double> distribution(mean,std);
		return distribution(generator);
	}

	double Stoch::generateGammaRand(double mean, double variance){
		double scale = variance / mean; // often called theta or (beta on c++)
		double shape = mean / scale; // often called k or alpha

		gamma_distribution<double> distribution(shape, scale);
		return distribution(generator);
	}

	double Stoch::generateWorkPerformed(double timeInterval, double cv)
	{	double mean = timeInterval;
		double variance = cv * mean;
		return generateGammaRand(mean, variance);
	}

	double Stoch::generateTimeDeviation(double timeInterval, double cv)
	{
		return generateWorkPerformed(timeInterval, cv) - timeInterval;
	}

	dataMatrix Stoch::cumulativeDistributionFunction(dataVector values){
		sort(values.begin(), values.end(), less<double>());
		long noPoints = values.size();
		dataVector y(noPoints);
		for (int i = 0; i < noPoints; i++){
			 y[i] = (1.0 * i)/ noPoints;
		}
		return {values,y};
	}

	default_random_engine Stoch::initGenerator()
		{long systime = clock();
		int seed = systime % 100000;
		default_random_engine generator(seed);
		 return generator;
	}

// old
		dataMatrix Stoch::calculate(double frequency) {
		int noPoints = 100000;
		dataVector x(noPoints);
		dataVector y(noPoints);
		// seed instellen!!!
		double sum = 0;
		double quadsum = 0;
		for (int i = 0; i < noPoints; i++)
		{
			x[i] = i;
			//y[i] = timeOfArrival(20, frequency);
			//y[i] = generateUniformRand(0, 10);
			y[i] = generateLogNormRand(10, 3);
			sum += y[i];
			quadsum += y[i] * y[i];
		}
		dataMatrix plotPoints = { x,y };
		double average = sum / noPoints;
		double variance = (quadsum - sum*sum / noPoints) / noPoints;
		//print("mean:=" + to_string(average));
		//print("variance:=" + to_string(variance));

		return(plotPoints);
	}

	// time planner
		double Stoch::timeOfArrival(double waitingTime, double frequency) {
		double period = 1.0 / frequency;
		double mean = period; // frequency of asking
		double variance = 0.1 * mean;

		double scale = variance / mean; // often called theta or (beta on c++)
		double shape = mean / scale; // often called k or alpha

		gamma_distribution<double> distribution(shape, scale);
		double timePassed = 0;
		while (waitingTime > 0) {
			double workAccomplished = distribution(generator);
			if (waitingTime > workAccomplished) {
				waitingTime -= workAccomplished;
				timePassed += period;
			}
			else {
				timePassed += waitingTime * period / workAccomplished;
				waitingTime = 0; //Approximation
			}
		}
		return timePassed;
	}

		dataMatrix Stoch::processOfArrival(double waitingTime, double frequency) {
		double period = 1.0 / frequency;
		double mean = period; // frequency of asking
		double variance = 0.1 * mean;

		double scale = variance / mean; // often called theta or (beta on c++)
		double shape = mean / scale; // often called k or alpha
		dataVector timing(0);
		dataVector arrival(0);

		timing.push_back(0);
		arrival.push_back(waitingTime);

		gamma_distribution<double> distribution(shape, scale);
		double timePassed = 0;
		while (waitingTime > 0) {
			double workAccomplished = distribution(generator);
			if (waitingTime > workAccomplished) {
				waitingTime -= workAccomplished;
				timePassed += period;
				timing.push_back(timePassed);
				arrival.push_back(waitingTime + timePassed);

			}
			else {
				timePassed += waitingTime * period / workAccomplished;
				waitingTime = 0; //Approximation
				timing.push_back(timePassed);
				arrival.push_back(waitingTime + timePassed);
			}
		}
		dataMatrix plotData = { timing, arrival };
		return plotData;
	}