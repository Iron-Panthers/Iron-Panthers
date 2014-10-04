#ifndef INTAKE_HPP
#define INTAKE_HPP

#include "WPILib.h"

class Intake
{
private:
	const DoubleSolenoid::Value droppedState;
	const DoubleSolenoid::Value liftedState;
	
	const float motorOutSpeed;
	const float motorInSpeed;
	
	DoubleSolenoid *intakeSolenoid;
	SpeedController *spinner;
	
	int spinTime;
public:
	Intake(DoubleSolenoid* sol);
	
	bool isLifted() const;
	bool isDropped() const;
	
	void dropArm();
	void liftArm();
	
	void spinIn();
	void spinOut();

	void stopSpinner();
	
	void update();
};

#endif
