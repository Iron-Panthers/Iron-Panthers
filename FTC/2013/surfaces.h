//#ifndef SURFACES_H
#define SURFACES_H

#include "JoystickDriver.c"
#include "move_turns_vars.h"

/*
Check if joysticks are being used - therefore we intend to move fast
For use in listenJoystickSurfaces, where we are not moving incrementally
*/
bool checkJoysticks()
{
	bool y1 = abs(joystick.joy1_y1) > 15;
	bool y2 = abs(joystick.joy1_y2) > 15;
	bool x1 = abs(joystick.joy1_x1) > 15;
	bool x2 = abs(joystick.joy1_x2) > 15;
	return y1 || y2 || x1 || x2;
}

/*
Without this task, the robot will fail.
Secret task, do not pay attention to this.
shhhh
*/
task playSparrow()
{
	PlaySoundFile("sparrow2.rso");
	wait1Msec(4500);
	ClearSounds();
}

/*
Listen for controls from joystick and run linear slide, lever, etc.
*/
task listenJoystickSurfaces()
{
		while (true)
		{
				getJoystickSettings(joystick);

				/*
					Flag turner
					Right trigger - turn fast/turn slow--upper trigger/lower trigger
				*/
        if (joy1Btn(6)) // up fast
        {
        	motor[flagRaiser] = 95;
        }
        else if (joy1Btn(8)) // up slow
      	{
      		motor[flagRaiser] = 30;
      	}
        else if (!joy1Btn(5) && !joy1Btn(7))
        {
        	motor[flagRaiser] = 0;
				}

				//deploy pullup bar
				if (joy1Btn(1))
				{
					servo[deployArm] = 0; //up
				}
				else if (joy1Btn(3))
				{
					servo[deployArm] = 255;	//down
				}
				else if (!joy1Btn(1) && !joy1Btn(3))
				{
					servo[deployArm] = 128;
				}

				/*
					Pullup Robot
					Left trigger - raise/lower--upper trigger/lower trigger
				*/
				if (joy1Btn(5)) // pull robot up with string
				{
					motor[pullupRaise] = 80;
				}
				else if (joy1Btn(7)) // lower robot // unnecessary because end round on pullup bar
				{
					motor[pullupRaise] = -90;
				}
				else if (!joy1Btn(5) && !joy1Btn(7))
				{
					motor[pullupRaise] = 0;
				}

       	//slow movement
				switch (joystick.joy1_TopHat)
				{
				case 0:
					motor[leftWheel] = SLOW_SPEED;
					motor[rightWheel] = SLOW_SPEED;
					break;
				case 4:
					motor[leftWheel] = -SLOW_SPEED;
					motor[rightWheel] = -SLOW_SPEED;
					break;
				case 2:
					motor[leftWheel] = SLOW_SPEED;
					motor[rightWheel] = 0;
					break;
				case 6:
					motor[leftWheel] = 0;
					motor[rightWheel] = SLOW_SPEED;
					break;
				default:
					//Our main joysticks are not being used, so we are safe to zero the motors
					if (!checkJoysticks())
					{
						motor[leftWheel] = 0;
						motor[rightWheel] = 0;
					}
					break;
				}

				if (joy1Btn(4))
				{
					servo[dropBlockArm] = BLOCK_ARM_FULL - 20;
				}
				else if (joy1Btn(2))
				{
					servo[dropBlockArm] = BLOCK_ARM_DOWN;
				}

				//most important part of the robot
     		if (joy1Btn(10))
     		{
     			StartTask(playSparrow, 50);
     		}
		}
}

/*
Deposit block into closest basket during autonomous.
*/
void dropBlock()
{
	servo[dropBlockArm] = BLOCK_ARM_FULL;
	wait1Msec(1000);                      //need to measure time
	servo[dropBlockArm] = BLOCK_ARM_DOWN; //reset
}

/*
Initialize the motors and servos to their starting positions.
*/
void init()
{
	motor[flagRaiser] = 0;
	motor[rightWheel] = 0;
	motor[leftWheel] = 0;
	motor[pullupRaise] = 0;

	servo[deployArm] = 128;
  servo[dropBlockArm] = BLOCK_ARM_DOWN;
	wait1Msec(1000);
}

#endif
