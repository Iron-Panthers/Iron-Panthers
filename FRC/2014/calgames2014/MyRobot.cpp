#include "WPILib.h"
#include "ThreeMotorGroup.hpp"
#include "FreeFunctions.hpp"
#include "Thrower.hpp"
#include "Intake.hpp"
#include "AntiBoolBounce.hpp"
#include "TimedAction.hpp"
#include <ctime>
#include <stdlib.h>
/**
 * This is a demo program showing the use of the RobotBase class.
 * The IterativeRobot class is the base of a robot application that will automatically call your
 * Periodic methods for each packet based on the mode.
 */
class RobotDemo : public IterativeRobot
{
	Joystick rightStick;
	Joystick leftStick;
	ThreeMotorGroup *leftMotorGroup, *rightMotorGroup; //controls the three motors for each side
	RobotDrive *myRobot;
	DoubleSolenoid* shiftingSolenoid;
	Intake intake;
	Thrower thrower;
	
	Compressor *compressor;
		
	const float ROBOT_SPEED;
	
	AntiBoolBounce throwBall;
	AntiBoolBounce throwBallToggle;
	AntiBoolBounce throwBallState;
	AntiBoolBounce pancakeShifterFast;
	AntiBoolBounce pancakeShifterSlow;
	AntiBoolBounce intakeArmDown;
	AntiBoolBounce intakeArmUp;
	AntiBoolBounce toggleCompressor;
	AntiBoolBounce enabledShifter;
	
	float driveForTime;
	int autonomousDriveCounter;
	//std::vector<TimedAction> autonomousActions;
public:
	RobotDemo():	// as they are declared above.
		rightStick(1),
		leftStick(2),
		leftMotorGroup(new ThreeMotorGroup(new Talon(4), new Talon(5), new Talon(6))),
		rightMotorGroup(new ThreeMotorGroup(new Victor(1), new Victor(2), new Talon(3))),
		myRobot(new RobotDrive(leftMotorGroup, rightMotorGroup)),
		intake(new DoubleSolenoid(5, 6)),//, new Victor(7)),
		thrower(new DoubleSolenoid(1, 2), new DoubleSolenoid(3, 4), &intake),
		compressor(new Compressor(10, 5)),
		ROBOT_SPEED(9.0f)
	{		
		shiftingSolenoid = new DoubleSolenoid(7, 8);
		
		myRobot->SetExpiration(0.1);
		this->SetPeriod(0); 	//Set update period to sync with robot control packets (20ms nominal)
	}
	
/**
 * Robot-wide initialization code should go here.
 * 
 * Use this method for default Robot-wide initialization which will
 * be called when the robot is first powered on.  It will be called exactly 1 time.
 */
void RobotDemo::RobotInit() {
	
}

/**
 * Initialization code for disabled mode should go here.
 * 
 * Use this method for initialization code which will be called each time
 * the robot enters disabled mode. 
 */
void RobotDemo::DisabledInit() {
}

/**
 * Periodic code for disabled mode should go here.
 *
 * Use this method for code which will be called periodically at a regular
 * rate while the robot is in disabled mode.
 */
void RobotDemo::DisabledPeriodic() {
}

/**
 * Initialization code for autonomous mode should go here.
 * 
 * Use this method for initialization code which will be called each time
 * the robot enters autonomous mode.
 */
void RobotDemo::AutonomousInit() 
{
	compressor->Start();
	//autonomousActions.push_back(TimedDrive<RobotDrive>(myRobot, &RobotDrive::ArcadeDrive, 2.0f));
	driveForTime = 2.0f;
	/*leftMotorGroup->Set(1.0f, 0);
	rightMotorGroup->Set(1.0f, 0);
	Wait(3000);
	leftMotorGroup->Set(0.0f, 0);
	rightMotorGroup->Set(0.0f, 0);*/
}

/**
 * Periodic code for autonomous mode should go here.
 *
 * Use this method for code which will be called periodically at a regular
 * rate while the robot is in autonomous mode.
 */
void RobotDemo::AutonomousPeriodic() {
	//std::vector<TimedAction>::iterator it = autonomousActions.begin();
	//static_cast<TimedDrive<RobotDrive&> >(*it).call(1.0f, 0.0f, true);
	//need generic base call function, so no cast needed
	/*if (autonomousDriveCounter < driveForTime * 100)
	{
		myRobot->ArcadeDrive(1.0f, 0.0f, true);
		autonomousDriveCounter++;
	}
	else
		myRobot->ArcadeDrive(0.0f, 0.0f, true);*/
}

/**
 * Initialization code for teleop mode should go here.
 * 
 * 
 * Use this method for initialization code which will be called each time
 * the robot enters teleop mode.
 */
void RobotDemo::TeleopInit() {
	compressor->Start();
}

/**
 * Periodic code for teleop mode should go here.
 *
 * Use this method for code which will be called periodically at a regular
 * rate while the robot is in teleop mode.
 */ 
///
/////
////
////
//
///
////
/*
 * 
 * 
 * 
 * 
 * 
 * hello
 * ;;;;./
 */
void RobotDemo::TeleopPeriodic() {
	// drive with arcade style
//	const float EPSILON = 0.01f;
//	float rightY = rightStick.GetY();
//	float rightX = rightStick.GetX();
//	if ((rightY > EPSILON || rightY < -EPSILON) || (rightX > EPSILON || rightX || -EPSILON))
		myRobot->ArcadeDrive(rightStick, true);
//	else
//		myRobot->ArcadeDrive(-leftStick.GetY(), leftStick.GetX(), true);
//	
	throwBall.set(rightStick.GetRawButton(1));// || leftStick.GetRawButton(1));
	throwBallToggle.set(rightStick.GetRawButton(2));// || leftStick.GetRawButton(2));
	pancakeShifterSlow.set(rightStick.GetRawButton(5));// || leftStick.GetRawButton(5));
	pancakeShifterFast.set(rightStick.GetRawButton(6));// || leftStick.GetRawButton(6));
	intakeArmDown.set(rightStick.GetRawButton(3)); //|| leftStick.GetRawButton(3));
	intakeArmUp.set(rightStick.GetRawButton(4));// || leftStick.GetRawButton(4));

	throwBall.update();
	throwBallToggle.update();
	pancakeShifterFast.update();
	pancakeShifterSlow.update();

	thrower.update();
	
	if (compressor->GetPressureSwitchValue() == 1)
		compressor->Stop();
	else if ((!compressor->Enabled()) && compressor->GetPressureSwitchValue() == 0)
		compressor->Start();
	
	if (throwBall.get())
		thrower.throwBall();
	else
		thrower.retract();
	
	if (throwBallToggle.get())
	{
		throwBallState.update();
		throwBallState.set(thrower.isRetracted());
		if (throwBallState.get())
			thrower.throwBall();
		else
			thrower.retract();
	}
	
	const DoubleSolenoid::Value fast = DoubleSolenoid::kForward;
	const DoubleSolenoid::Value slow = DoubleSolenoid::kReverse;
	if (pancakeShifterFast.get()) 
	{
		shiftingSolenoid->Set(fast);	
	}
	if (pancakeShifterSlow.get())
	{
		shiftingSolenoid->Set(slow);
	}

}

/**
 * Initialization code for test mode should go here.
 * 
 * Use this method for initialization code which will be called each time
 * the robot enters test mode.
 */
void RobotDemo::TestInit() {
}

/**
 * Periodic code for test mode should go here.
 *
 * Use this method for code which will be called periodically at a regular
 * rate while the robot is in test mode.
 */
void RobotDemo::TestPeriodic() {
	toggleCompressor.set(rightStick.GetRawButton(1));
	if (toggleCompressor.get())
	{
		bool enabled = compressor->Enabled();
		if (enabled)
			compressor->Stop();
		else
			compressor->Start();
	}
	
	if (compressor->GetPressureSwitchValue() == 1)
		compressor->Stop();
}

void RobotDemo::move(float distance)
{
	//Distance is in meters
	//SPEED = 22m/s
	int waitTime = (int)((distance / ROBOT_SPEED) * 1000);
	
	int sig = sign(distance);
	
	myRobot->TankDrive(1.0f * sig, 1.0f * sig, true);
	sleep(waitTime);
}

};



START_ROBOT_CLASS(RobotDemo);
