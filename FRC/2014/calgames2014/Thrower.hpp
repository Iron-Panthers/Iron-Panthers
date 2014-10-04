#ifndef THROWER_HPP
#define THROWER_HPP

#include "WPILib.h"
#include "FreeFunctions.hpp"
#include "Intake.hpp"
#include <time.h>

class Thrower
{
private:
	const DoubleSolenoid::Value thrownState;
	const DoubleSolenoid::Value retractedState;
	
	DoubleSolenoid* const one;
	DoubleSolenoid* const two;
	
	Intake *const pIntake;	
	int sinceLastCommand;
	
	bool canThrow;
	int throwTries;
	
public:
	Thrower(DoubleSolenoid *one, DoubleSolenoid *two, Intake* pIntake);
	bool throwBall();
	void retract();

	void update();
	
	bool isRetracted();
	
};

#endif
