#include "Thrower.hpp"

Thrower::Thrower(DoubleSolenoid* one, DoubleSolenoid* two, Intake *pointerIntake)
	: thrownState(DoubleSolenoid::kReverse)
	, retractedState(DoubleSolenoid::kForward) 
	, one(one), two(two)
	, pIntake(pointerIntake)
	, sinceLastCommand(0)
	, canThrow(false)
	, throwTries(0)
{
}

bool Thrower::throwBall()
{
	if (canThrow)
	{
		sinceLastCommand++;
		one->Set(thrownState);
		two->Set(thrownState);
		throwTries = 0;
		return true;
	}
	
	pIntake->dropArm();
	
	throwTries++;
	if (throwTries > 10)
	{
		throwTries = 0;
		return false;
	}
		
	
	return throwBall();
	
	return false;
}

void Thrower::retract()
{
	sinceLastCommand = 0;
	one->Set(retractedState);
	two->Set(retractedState);
}

void Thrower::update()
{
	sinceLastCommand++;
	 
	//if (pIntake->isDropped() && sinceLastCommand > 8)
	if (sinceLastCommand > 8)
		canThrow = true;
	else
		canThrow = false;
}

bool Thrower::isRetracted()
{
	return one->Get() == retractedState;
}
