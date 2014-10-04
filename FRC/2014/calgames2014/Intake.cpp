#include "Intake.hpp"
#include "WPILib.h"
#include <time.h>

Intake::Intake(DoubleSolenoid* sol)
	: droppedState(DoubleSolenoid::kReverse)
	, liftedState (DoubleSolenoid::kForward)
	, motorOutSpeed(0.75f)
	, motorInSpeed(-0.75f)
	, intakeSolenoid(sol)
//	, spinner(motor)
{
}

bool Intake::isLifted() const { return intakeSolenoid->Get() && liftedState; }
bool Intake::isDropped() const { return intakeSolenoid->Get() && droppedState; }

void Intake::spinIn()
{
	spinTime = 0;
	spinner->Set(motorInSpeed);
}

void Intake::spinOut()
{
	spinTime = 0;
	spinner->Set(motorOutSpeed);
}

void Intake::stopSpinner()
{
	spinner->Set(0.0f);
}

void Intake::update()
{
	
	//if (spinTime > 8)
	//{
	//	stopSpinner();
	//}
	spinTime++;
}

void Intake::dropArm()
{
	intakeSolenoid->Set(droppedState);
}

void Intake::liftArm()
{
	intakeSolenoid->Set(liftedState);
}
