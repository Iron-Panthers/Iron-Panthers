#include "WPILib.h"
#include "ThreeMotorGroup.hpp"
	
ThreeMotorGroup::ThreeMotorGroup(SpeedController *one
							   , SpeedController *two
							   , SpeedController *three)
	: one(one), two(two), three(three)
{
	
}

void ThreeMotorGroup::Set(float speed, uint8_t syncGroup = 0)
{
	one->Set(speed);
	two->Set(speed);
	three->Set(speed);
}

float ThreeMotorGroup::Get()
{		
	//Returns the average speed of all three motors
	return (one->Get() + two->Get() + three->Get()) / 3;
}
void ThreeMotorGroup::Disable()
{
	one->Disable();
	two->Disable();
	three->Disable();
}

void ThreeMotorGroup::PIDWrite(float output) 
{
	Set(output);
}

ThreeMotorGroup::~ThreeMotorGroup()
{
	delete one;
	delete two;
	delete three;
}
