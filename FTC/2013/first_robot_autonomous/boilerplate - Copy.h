#ifndef BOILERPLATE_H
#define BOILERPLATE_H

#include "JoystickDriver.c"
#include "move_turns_vars.h"

const float CIRCUMFERENCE = 23.495;
const float SPEED        = 18;
const float TURNING_CIRCUMFERENCE = 212.458;
const float VELOCITY = 52;
const float EPSILON_SLAVE = 1.27;

const float EPSILON_TURN = 1.1;
const float EPSILON_MOVE = 1.135;

/*
Secret task, do not pay attention to this.
shhhh
*/
task playSparrow()
{
	PlaySoundFile("sparrow2.rso");
	wait1Msec(30370);
	ClearSounds();
}

void stop();

/*
Not working due to the way that field control keeps track of nPgmTime.
As of 12/2/13, we are not using this function due to the problems that arise.
*/
float getTimeLeft()
{
    return 30 - (nPgmTime * 1000);
}


/*
Stop both wheels.
*/
void stop()
{
        motor[rightWheel] = 0;
        motor[leftWheel] = 0;

        wait1Msec(250);
}


/*
The primary function used during the autonomous period for forward and backward movement.
Ustilizes various epsilon constants for calibration. Takes a float distance to figure out the number
of cm of movement.
*/
void move(float distance)
{
    /*
    nSyncedMotors = synchBC;
    nSyncedTurnRatio = 100;

    nMotorEncoder[rightWheel] = 0;*/
    if (distance > 0){
        motor[leftWheel] = -SPEED;
        wait10Msec(1);
        motor[rightWheel] = EPSILON_SLAVE * SPEED;
        wait1Msec(distance/(VELOCITY * EPSILON_MOVE)*1000); // the amount of time to keep both of the
                                                            // wheels moving at a specified speed.
    }
    else
    {
        motor[leftWheel] = SPEED;
        motor[rightWheel] = -EPSILON_SLAVE * SPEED;
        wait1Msec(distance/(VELOCITY * EPSILON_MOVE)/1000); // the amount of time to keep both of the
                                                            // wheels moving at a specified speed.
    }
    stop();
}



/*
Function used in the autonomous period for turning. Very hacky implementation, but these are the values
that work for our turing when pivoting on a single back wheel.
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
        motor[leftWheel] = -SPEED;

        float deg = (float)degrees;
        float time = abs(deg)/360*TURNING_CIRCUMFERENCE/(VELOCITY*EPSILON_TURN)*1000;
        wait1Msec(time);
    }

    stop();
}


void turnReverse(int degrees)
{
	degrees %= 360;

    if (degrees < 0 && degrees >= 15)
    	degrees += 9;
  	else if (degrees < 15 && degrees >= 35)
  		degrees += 22;
  	else if (degrees < 35 && degrees >= 52)
  		degrees += 10;
  	else if (degrees < 52 && degrees >= 70)
  		degrees += 12;
  	else if (degrees < 70 && degrees >= 120)
  		degrees += 5;

  	if (degrees > 0 && degrees <= -15)
  		degrees -= 5;
  	else if (degrees > -15 && degrees <= -35)
  		degrees -= 14;
  	else if (degrees > -35 && degrees <= -52)
  		degrees -= 15;
  	else if (degrees > -52 && degrees <= -70)
  		degrees -= 14;
  	else if (degrees > -70 && degrees <= -120)
  		degrees -= 15;
  	else if (degrees > -120 && degrees <= -180)
  		degrees -= 12;

    if (degrees < 0)
    {
        motor[rightWheel] = 0;
        motor[leftWheel] = SPEED;

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
A routine for opening up the swiffer, kicking out the block, and moving everything back.
*/
void gotoBox(){ //TEST First
    servo[swiffer] = 60;
    wait1Msec(250);
    servo[kicker] = 60;
    wait1Msec(250);
    servo[kicker] = 10;
    wait1Msec(250);
    servo[swiffer] = 190;
}

/*
function to get the value of the sensor. 1-9 (not used, no IR)
*/
int getDegreesToIRBeacon(tSensors ir)
{
    wait1Msec(1);
    int ir_value = SensorValue[ir];
    wait1Msec(1);
    return ir_value;
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
DON'T USE
*/
int getIr(){
	int mode = getDegreesToIRBeacon(S2);
	int cntr = 0;

	while(cntr <= 25){
		if((mode != getDegreesToIRBeacon(S2) && getDegreesToIRBeacon(S2) != 0)){
	      mode = getDegreesToIRBeacon(S2);
	      cntr = 0;
		}
		else{
			cntr++;
		}
	}
	return(mode);

}
/*
cubic function used to model a gradual increase in speed when using joystick
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


/**
Forward/backward left stick
Left/right right stick
*/
void joyMethodTwo()
{
		if (abs(joystick.joy1_y1) > 5)
		{
			motor[leftWheel] = -motorAdjustedSpeed(joystick.joy1_y1);
		}
		else
		{
			motor[leftWheel] = 0;
		}

		if (abs(joystick.joy1_y2) > 5)
		{
			motor[rightWheel] = motorAdjustedSpeed(joystick.joy1_y2);
		}
		else
		{
			motor[rightWheel] = 0;
		}
}

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
    else if (abs(joystick.joy1_y1) < 5 && abs(joystick.joy1_x2) < 5)
    {
        motor[leftWheel] = 0;
        motor[rightWheel] = 0;
    }
}

/*
moves the elevator up to the highest extend. DON'T USE
*/
task moveElevatorUpFull()
{
	servo[elevator] = 0;
	servo[elevator2] = 255;
	wait1Msec(time_elevator2_up);
	servo[elevator2] = 128;
	wait1Msec(time_elevator1_up - time_elevator2_up);
	servo[elevator] = 128;
}
/*
moves the elevator down to the lowest point. DON'T USE
*/
task moveElevatorDownFull()
{
	servo[elevator] = 255;
	servo[elevator2] = 0;
	wait1Msec(time_elevator2_down);
	servo[elevator2] = 128;
	wait1Msec(time_elevator1_down - time_elevator2_down);
	servo[elevator] = 128;
}

/*
moves the elevator up 80%. DON'T USE
*/
task moveElevatorUp80()
{
	servo[elevator] = 0;
	servo[elevator2] = 255;
	wait1Msec(time_elevator2_up * 0.8);
	servo[elevator2] = 128;
	wait1Msec(time_elevator1_up * 0.8 - time_elevator2_up * 0.8);
	servo[elevator] = 128;
}

/*
moves the elevator up 80%. DON'T USE
*/
task moveElevatorDown80()
{

	servo[elevator] = 255;
	servo[elevator2] = 0;
	wait1Msec(time_elevator2_down * 0.8);
	servo[elevator2] = 128;
	wait1Msec(time_elevator1_down * 0.8 - time_elevator2_down * 0.8);
	servo[elevator] = 128;
}

/*
use joystick driver
*/
task listenJoystick()
{
	int ticks = 0;
    while (true)
    {
        getJoystickSettings(joystick);

        //joyMethodOne();
				joyMethodTwo();

        //ELEVATOR
        /*
        Any number higher than 128 will make the elevator go down
        Any number less than 128 will make the elevator go up
        128 is the magic number at which the servo stops

        Speed of the servo is directly related with the distance between 128 and the speed set.
            servoChangeRate does not at all affect speed on continuous rotation servos
        */
        if (joy1Btn(7)) //GOIN DOWN
        {
            servo[elevator] = 255;
            ticks = 0;
        }
        else if (joy1Btn(5)) // GOIN UP
        {
            servo[elevator] = 0;
            ticks = 0;
        }
        else if (!joy1Btn(5) && !joy1Btn(7) && ticks == 50)
        {
            servo[elevator] = 128;
        }
        if (joy1Btn(6)) //GOIN DOWN
        {
            servo[elevator2] = 255;
            ticks = 0;
        }
        else if (joy1Btn(8)) // GOIN UP
        {
            servo[elevator2] = 0;
            ticks = 0;
        }
        else if (!joy1Btn(6) && !joy1Btn(8) && ticks == 50)
        {
            servo[elevator2] = 128;
        }
        ticks++;

        //box capturer
      	if (joy1Btn(1)) //go down fully swiffer
      	{
      		if (ServoValue[kicker] == KICKER_UP)
      		{
      			servo[kicker] = KICKER_DOWN;
      			wait1Msec(100);
      		}
      			servo[swiffer] = SWIFFER_DOWN;
      	}
       	else if (joy1Btn(3))//go up
       	{
        	servo[swiffer] = SWIFFER_UP;
      	}

       	if (joy1Btn(4))	//kick out kicker
       	{
       		if (ServoValue[swiffer] == SWIFFER_DOWN)
       		{
       			servo[swiffer] = SWIFFER_UP;
       			wait1Msec(250);
       		}
       		servo[kicker] = KICKER_UP;
       	}
       	else if (joy1Btn(2)) //go down
       		servo[kicker] = KICKER_DOWN;

       	//raise elevator up automatically
       	/*if (joystick.joy1_TopHat == 2)
    			StartTask(moveElevatorUp80);
    		else if (joystick.joy1_TopHat == 6)
    			StartTask(moveElevatorDown80);
    		else if (joy1Btn(9) || joy1Btn(5) || joy1Btn(7) || joy1Btn(6) || joy1Btn(8))
    		{
    			StopTask(moveElevatorUp80);
    			StopTask(moveElevatorDown80);
    		}*/

       	//flag raiser
       	//7, 0, 1 go up
       	//5, 4, 3 go down
       	if (joystick.joy1_TopHat == 7 || joystick.joy1_TopHat == 0 || joystick.joy1_TopHat == 1)
       	{
       		motor[flagRaiser] = 80;
      	}
      	else if (joystick.joy1_TopHat == 5 || joystick.joy1_TopHat == 4 || joystick.joy1_TopHat == 3)
      	{
      		motor[flagRaiser] = -80;
      	}
      	if (joystick.joy1_TopHat == -1)
     		{
     			motor[flagRaiser] = 0;
     		}

     		//most important part of the robot
     		if (joy1Btn(10))
     		{
     			StartTask(playSparrow);
     		}

    }
}

/*
sets the values of the servos in the bucket to the right position to kick
*/
void init()
{
	servo[swiffer] = SWIFFER_DOWN;
	servo[kicker] = KICKER_DOWN;
}

#endif
