#ifndef MOVEMENT_H
#define MOVEMENT_H

#include "JoystickDriver.c"
#include "move_turns_vars.h"
#include "surfaces.h"

/// This file contains all the forward definitions for the movement of the
///		robot --- controlled and autonomous.

//Controlled joystick functions
/*
Listen for controls from joystick and run controlled robot movement.
*/
void joyMethodOne();
void joyMethodTwo();
void stop();

task listenJoystickMovement()
{
    while (true)
    {
        getJoystickSettings(joystick);

        joyMethodTwo();
				//joyMethodTwo();
    }
}

/*
returns the sign of the function
*/
int sign(int num)
{
    if (num > 0)
        return 1;
    else if (num < 0)
        return -1;
    else
        return 0;
}

/*
Cubic function for mapping joystick values to motor speeds.
Optimized for low speed precision.
*/
int motorAdjustedSpeed(int joystick)
{
    //cubic regression
    int joystickAbs = abs(joystick);

    const float a = 0.000130295;
    const float b = -0.016406;
    const float c = 0.73254;
    const float d = 1.72279;

    int eq = a * joystickAbs * joystickAbs * joystickAbs +
             b * joystickAbs * joystickAbs +
             c * joystickAbs +
             d;
    return sign(joystick) > 0 ? eq : -eq;
}

/*
Check if POV HAT is being used - therefore we intend to move slowly
For use in joyMethodOne or joyMethodTwo. Opposite of checkJoystick().
*/
bool checkPOV()
{
	bool pov = joystick.joy1_TopHat != -1;
	return pov;
}

/*
Left joystick = forward/backward
Right joystick = left/right
*/
void joyMethodOne()
{
    if (joystick.joy1_x2 > 5)
    {
        motor[leftWheel] = -motorAdjustedSpeed(joystick.joy1_x2);
        motor[rightWheel] = 0;
    }
    else if (joystick.joy1_x2 < -5)
    {
 				motor[rightWheel] = -motorAdjustedSpeed(joystick.joy1_x2);
 				motor[leftWheel] = 0;
    }

    if (joystick.joy1_y1 > 5 || joystick.joy1_y1 < -5)
    {
        motor[leftWheel] = -motorAdjustedSpeed(joystick.joy1_y1);
        motor[rightWheel] = motorAdjustedSpeed(joystick.joy1_y1);
    }
    else if (abs(joystick.joy1_y1) < 5 && abs(joystick.joy1_x2) < 5 && !checkPOV())
    {
        motor[leftWheel] = 0;
        motor[rightWheel] = 0;
    }
}

/*
Left joystick = speed of left wheel
Right joystick = speed of right wheel;
*/
void joyMethodTwo()
{
		if (abs(joystick.joy1_y1) > 5)
		{
			motor[leftWheel] = motorAdjustedSpeed(joystick.joy1_y1);
		}
		else
		{
			if (!checkPOV())
				motor[leftWheel] = 0;
		}

		if (abs(joystick.joy1_y2) > 5)
		{
			motor[rightWheel] = motorAdjustedSpeed(joystick.joy1_y2);
		}
		else
		{
			if (!checkPOV())
				motor[rightWheel] = 0;
		}
}

/*
Pivot on one wheel, turning backwards.
*/
void turnReverse(int degrees)
{
		degrees %= 360;

		//Calibration
    if (degrees < 0 && degrees >= -15)
    	degrees -= 9;
  	else if (degrees < -15 && degrees >= -35)
  		degrees -= 22;
  	else if (degrees < -35 && degrees >= -52)
  		degrees -= 0;
  	else if (degrees < -52 && degrees >= -70)
  		degrees -= 12;
  	else if (degrees < -70 && degrees >= -120)
  		degrees -= 5;

  	if (degrees > 0 && degrees <= 15)
  		degrees += 5;
  	else if (degrees > 15 && degrees <= 35)
  		degrees += 14;
  	else if (degrees > 35 && degrees <= 52)
  		degrees += 15;
  	else if (degrees > 52 && degrees <= 70)
  		degrees += 14;
  	else if (degrees > 70 && degrees <= 120)
  		degrees += 15;
  	else if (degrees > 120 && degrees <= 180)
  		degrees += 12;

    if (degrees < 0)
    {
        motor[rightWheel] = 0;
        motor[leftWheel] = -SPEED;

        float deg = (float)degrees;
        float time = 0.0;

        time = abs(deg)/360.0*TURNING_CIRCUMFERENCE/(VELOCITY)*1000;


        wait1Msec(time);
    }
    else
    {
        motor[leftWheel] = 0;
        motor[rightWheel] = -SPEED;

        float deg = (float)degrees;
        float time = abs(deg)/360*TURNING_CIRCUMFERENCE/(VELOCITY*EPSILON_TURN)*1000;
        wait1Msec(time);
    }
    stop();
}

/*
Pivot on one wheel, turning forwards.
*/
void turn(int degrees)
{
    degrees %= 360;

    if (degrees < 0 && degrees >= -15)
    	degrees -= 9;
  	else if (degrees < -15 && degrees >= -35)
  		degrees -= 22;
  	else if (degrees < -35 && degrees >= -52)
  		degrees -= 10;
  	else if (degrees < -52 && degrees >= -70)
  		degrees -= 12;
  	else if (degrees < -70 && degrees >= -120)
  		degrees -= 5;

  	if (degrees > 0 && degrees <= 15)
  		degrees += 5;
  	else if (degrees > 15 && degrees <= 35)
  		degrees += 14;
  	else if (degrees > 35 && degrees <= 52)
  		degrees += 15;
  	else if (degrees > 52 && degrees <= 70)
  		degrees += 14;
  	else if (degrees > 70 && degrees <= 120)
  		degrees += 15;
  	else if (degrees > 120 && degrees <= 180)
  		degrees += 12;

    if (degrees < 0)
    {
        motor[leftWheel] = 0;
        motor[rightWheel] = SPEED;

        float deg = (float)degrees;
        float time = 0.0;

        time = abs(deg)/360.0*TURNING_CIRCUMFERENCE/(VELOCITY)*1000;


        wait1Msec(time);
    }
    else
    {
        motor[rightWheel] = 0;
        motor[leftWheel] = SPEED;

        float deg = (float)degrees;
        float time = abs(deg)/360*TURNING_CIRCUMFERENCE/(VELOCITY*EPSILON_TURN)*1000;
        wait1Msec(time);
    }

    stop();
}

/*
Move for distance, based on time. Will be at the mercy of the battery power.
The primary function used during the autonomous period for forward and backward movement.
Ustilizes various epsilon constants for calibration. Takes a float distance to figure out the number
of cm of movement.

*/
void move(float distance)
{
    if (distance > 0){
        motor[leftWheel] = SPEED;
        wait10Msec(1);
        motor[rightWheel] = SPEED;
        wait1Msec(distance/(VELOCITY * EPSILON_MOVE)*1000); // the amount of time to keep both of the
                                                            // wheels moving at a specified speed.
    }
    else
    {
        motor[leftWheel]= -1 * SPEED;
        wait10Msec(1);
        motor[rightWheel]= -1 * SPEED;
        //wait1Msec(1000);
        wait1Msec(abs(distance)/(VELOCITY * EPSILON_MOVE)*1000); // the amount of time to keep both of the
                                                            // wheels moving at a specified speed.
    }
    stop();
}

/*
Stops both wheels.
*/
void stop()
{
        motor[rightWheel] = 0;
        motor[leftWheel] = 0;

        wait1Msec(250);
}
