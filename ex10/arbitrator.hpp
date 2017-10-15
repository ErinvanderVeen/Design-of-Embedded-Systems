#ifndef ARBITRATOR_H
#define ARBITRATOR_H

#include <string>
#include <iterator>
#include <list>

#include "robot_action.hpp"
#include "sensors.hpp"
#include "shared_memory.hpp"

class Arbitrator {
	public:
		Arbitrator(SharedMemory&, Sensors&);
		void start();
		void apply(RobotAction*);

	private:
		std::list<RobotAction*> actions;
		SensorData sensor_data;
		Sensors sensors;
};

#endif
