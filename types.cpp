#include "types.h"
	//conversion
	string Times::toDate(double time)
	{
		double epsilon = 0.00000000001;
		int days = (int)(time / day + epsilon);
		int hours = (int)(fmod(time + epsilon, day) / hour);
		int minutes = (int)(fmod(time + epsilon, hour) / minute);
		return "Day " + to_string(days) + ", " + full(hours) + ":" + full(minutes);
	}

	string Times::toHour(double time) {
		double epsilon = 0.00000000001;
		int hours = (int)(fmod(time + epsilon, day) / hour);
		int minutes = (int)(fmod(time + epsilon, hour) / minute);
		return full(hours) + ":" + full(minutes);
	}

	string Times::full(int num)
	{	string s = to_string(num);
		if (s.length() < 2)
			s = "0"+s;
		return s;
	}

	string toString(double num) {
		stringstream stream;
		stream << fixed << setprecision(2) << num;
		return stream.str();
	}