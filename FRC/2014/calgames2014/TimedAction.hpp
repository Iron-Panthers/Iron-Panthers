#ifndef TIMED_ACTION_HPP
#define TIMED_ACTION_HPP

#include "WPILib.h"

class TimedAction
{
protected:
	float forSeconds;
public:
	TimedAction(float forSeconds);
};

//This is meant to be used for ArcadeDrive within RobotDrive class
template <class T>
class TimedDrive : public TimedAction
{
private:
	void (T::*arcade)(float, float, bool);	//pointer to member function Arcade Drive
	T *robotDriveObject;						//pointer to RobotDrive object
	
	int calledTimes;
public:
	TimedDrive(T* object, void (T::*driveFunc)(float, float, bool), float forSeconds);
	void call(float speed, float rot, bool squared);
	
	bool isExhausted();
};

#endif
