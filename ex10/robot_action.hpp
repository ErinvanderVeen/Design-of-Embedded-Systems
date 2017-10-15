#ifndef ROBOT_ACTION_H
#define ROBOT_ACTION_H

class RobotAction {
	public:
		// Be careful with PASS! Sensor Data is not updated for the next action
		enum Control { SKIP, BLOCK, PASS };
		virtual Control takeControl(SensorData&) = 0;
		virtual void action(SensorData&) = 0;
};

#endif
