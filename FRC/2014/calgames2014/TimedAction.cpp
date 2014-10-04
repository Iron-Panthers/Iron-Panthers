#include "TimedAction.hpp"

TimedAction::TimedAction(float forSeconds) : forSeconds(forSeconds) {}

template <class T>
TimedDrive<T>::TimedDrive(T* object
					    , void (T::*driveFunc)(float, float, bool)
					    , float forSeconds)
					    : TimedAction(forSeconds)
					    , robotDriveObject(object)
					    , arcade(driveFunc)
					    , calledTimes(0)
{
	
}

template <class T>
void TimedDrive<T>::call(float speed, float rot, bool squared)
{
	int numTimesToCall = forSeconds * 100;
	if (calledTimes < numTimesToCall)
	{
		(*robotDriveObject.*arcade)(speed, rot, squared);
		calledTimes++;
	}
}

template <class T>
bool TimedDrive<T>::isExhausted()
{
	int numTimesToCall = forSeconds * 100;
	return calledTimes >= numTimesToCall;
}
