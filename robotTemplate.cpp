#include <WPILib.h>  //includes all additional documents that we take functions from (defining victors, mecanum drive, etc.)
#include <Victor.h>
#include <Timer.h>
#include <cmath>
#include <NetworkCommunication/UsageReporting.h>

//front of robot is cRIO, back with battery

void RotateVector(double&,double&,double);
const int32_t kMaxNumberOfMotors=4;
void Normalize(double*);

/**
 * This is a demo program showing the use of the RobotBase class.
 * The IterativeRobot class is the base of a robot application that will automatically call your
 * Periodic methods for each packet based on the mode.
 */ 
class RobotDemo : public IterativeRobot
{
	DriverStation *ds;
	Victor *victorFL, *victorFR, *victorBL, *victorBR;
	Talon *spinnerL1, *spinnerL2, *spinnerR1, *spinnerR2;
	Joystick *stickDrive, *stickOther;
	RobotDrive myRobot; // robot drive system
	DoubleSolenoid *s1,*s2,*s3;
	Compressor *compressor;
	//velocities in ft/s, motor_power and spinner_speed indicates power level
	const float ROBOT_VELOCITY, ROBOT_VELOCITY_HORIZ, MOTOR_POWER, SPINNER_SPEED;
	bool spinnerStatusLeft, spinnerStatusRight, 
		s1Status,s2Status,s3Status, 
		actionLeftPerformed,actionRightPerformed;
	int periodicCounter;
	const int SOLENOID_WAIT_TICKS;
	
public:
	RobotDemo();
	~RobotDemo();
	
	void retract(DoubleSolenoid*, bool&);
	void RobotInit();
	void DisabledInit();
	void DisabledPeriodic();
	void AutonomousInit();
	void AutonomousPeriodic();
	void TeleopPeriodic();
	void TeleopInit();
	void move_horizontal(float);
	void spinnerLeft(float);
	void stopSpinnersLeft();
	void stop();
	void move(float);
	void spinnerRight(float);
	void stopSpinnersRight();
	void toggleCompressor();
	void extend(DoubleSolenoid*, bool&);
	void toggle(DoubleSolenoid*, bool&);
	void toggleLeftDrop(float);
	void toggleLeftPick(float);
	void toggleRightDrop(float);
	void toggleRightPick(float);
	void MecanumDrive(float,float,float,float);
	
	void TestInit();
	void TestPeriodic();
	float ramp(float);
};
	RobotDemo::RobotDemo() :
		ds(DriverStation::GetInstance()),
		victorFL(new Victor(4)), //front: 	left
		victorFR(new Victor(1)), //			right
		victorBL(new Victor(3)), //back:	left
		victorBR(new Victor(2)), //			right
		
		spinnerL1(new Talon(8)), //left spinners
		spinnerL2(new Talon(7)),
		spinnerR1(new Talon(5)), //right spinners
		spinnerR2(new Talon(6)),
		
		stickDrive(new Joystick(1)),
		stickOther(new Joystick(2)),
		
		myRobot(victorFL, victorBL, victorFR, victorBR),  //order is for mecanum drive later
		
		s1(new DoubleSolenoid(1,2)), //forward channel, reverse channel
		s2(new DoubleSolenoid(4,5)),
		s3(new DoubleSolenoid(7,8)),
		compressor(new Compressor(1,5)), //pressureSwitchChannel, compressorRelayChannel
		
		ROBOT_VELOCITY(2.00), //define constants (these values must be calibrated)
		ROBOT_VELOCITY_HORIZ(4.00), //no srsly
		MOTOR_POWER(0.75),
		SPINNER_SPEED(0.75),
		
		periodicCounter(0),
		SOLENOID_WAIT_TICKS(15)
	// initialized in same order as they are declared above.
	{
		SmartDashboard::init();
		myRobot.SetInvertedMotor(RobotDrive::kRearRightMotor, true);
		myRobot.SetInvertedMotor(RobotDrive::kFrontRightMotor, true);
		this->SetPeriod(.02); 	//Set update period to sync with robot control packets (20ms nominal)
		myRobot.SetSafetyEnabled(false);
	}
	RobotDemo::~RobotDemo()
	{
		retract(s2,s2Status); //catcher arms come in first
		Wait(.5);
		retract(s3,s3Status);
		retract(s1,s1Status);		
		//get rid of pointers
		delete compressor;
		delete s3;
		delete s2;
		delete s1;
		delete stickOther;
		delete stickDrive;
		delete spinnerR2;
		delete spinnerL2;
		delete spinnerR1;
		delete spinnerL1;
		delete victorBR;
		delete victorBL;
		delete victorFR;		
		delete victorFL;
		delete ds;
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
void RobotDemo::AutonomousInit() {
	compressor->Start(); //starting compressor to ready for teleop
	
	SmartDashboard::PutBoolean("Left-Spinner", spinnerStatusLeft);
	SmartDashboard::PutBoolean("Right-Spinner", spinnerStatusLeft);
	SmartDashboard::PutBoolean("S1-Status", s1Status);
	SmartDashboard::PutBoolean("S2-Status", s2Status);
	SmartDashboard::PutBoolean("S3-Status", s3Status);
	SmartDashboard::PutNumber("Compressor-Pressure", compressor->GetPressureSwitchValue());
	
	
	//based on switch input on I/O tab of Driver Station--tells robot whether it
	//bool sideStart = true;
	//bool centerStartToLeft = false;
	//bool centerStartToRight = false;
	
	retract(s2,s2Status); //catcher arms come in first
	Wait(.2);
	retract(s3,s3Status);
	retract(s1,s1Status);
	
	//test this module,
	/*bool sideStart = ds->GetDigitalIn(1);  //is starting in the middle.
	bool centerStartToLeft = ds->GetDigitalIn(2); //center -> left low goal
	bool centerStartToRight = ds->GetDigitalIn(3); //center -> right low goal
	*/
	
	//if(sideStart){  //either side--face right
		move_horizontal(-15.5);  //MUST make sure robot is facing right direction!!!
		spinnerLeft(SPINNER_SPEED);
		spinnerRight(-SPINNER_SPEED);
		Wait(3.0); //arbitrary time -- test
		stopSpinnersLeft();
		stopSpinnersRight();
		stop();
	//}
		
}



/**
 * Periodic code for autonomous mode should go here.
 *
 * Use this method for code which will be called periodically at a regular
 * rate while the robot is in autonomous mode.
 */
void RobotDemo::AutonomousPeriodic() {
}

/**
 * Initialization code for teleop mode should go here.
 * 
 * Use this method for initialization code which will be called each time
 * the robot enters teleop mode.
 */
void RobotDemo::TeleopInit() {
	s1Status=true;
	s2Status=true;
	s3Status=true;
	retract(s3,s3Status);
	retract(s1,s1Status);				
	spinnerStatusLeft=true;
	spinnerStatusRight=true;
	compressor->Start();
	return;
}

/**
 * Periodic code for teleop mode should go here.
 *
 * Use this method for code which will be called periodically at a regular
 * rate while the robot is in teleop mode.
 */
void RobotDemo::TeleopPeriodic() {
	/*ALTERNATE OPTION: use constants at the top of the code to pass in buttons associated
	 * with various actions.
	 * 
	 *FOR ALTERNATE LAYOUTS: 
	 *have one set of layouts commented out? 
	 *take input from Driver Station I/O tab to determine which set is used? 
	 * */
	
	
	//driver joystick: Extreme 3D
	bool compressor = stickDrive->GetRawButton(10);
	//drive-related
	bool stopDrive = stickDrive->GetRawButton(1);
	bool slowSpeed = stickDrive->GetRawButton(2);
	//arm controls--solenoids involved
	bool extendAll_driver = stickDrive->GetRawButton(9);
	bool retractAll_driver = stickDrive->GetRawButton(11);
	
	//spinner controls
	bool leftDrop_driver = stickDrive->GetRawButton(3);
	bool leftPick_driver = stickDrive->GetRawButton(5);
	bool rightDrop_driver = stickDrive->GetRawButton(4);
	bool rightPick_driver = stickDrive->GetRawButton(6);

	//other joystick: Attack 3
	//arm controls--solenoids involved
	bool extendAll_other = stickOther->GetRawButton(3);
	bool retractAll_other = stickOther->GetRawButton(2);
	bool leftArm_other = (stickOther->GetRawButton(6) || stickOther->GetRawButton(10));
	bool rightArm_other = (stickOther->GetRawButton(7) || stickOther->GetRawButton(11));
	//spinner controls
	bool leftDrop_other = (stickOther->GetRawButton(4) && !stickOther->GetRawButton(1));
	bool leftPick_other = (stickOther->GetRawButton(4) && stickOther->GetRawButton(1));
	bool rightDrop_other = (stickOther->GetRawButton(5) && !stickOther->GetRawButton(1));
	bool rightPick_other = (stickOther->GetRawButton(5) && stickOther->GetRawButton(1));
	
	SmartDashboard::PutBoolean("Left-Spinner", spinnerStatusLeft);
	SmartDashboard::PutBoolean("Right-Spinner", spinnerStatusLeft);
	SmartDashboard::PutBoolean("S1-Status", s1Status);
	SmartDashboard::PutBoolean("S2-Status", s2Status);
	SmartDashboard::PutBoolean("S3-Status", s3Status);
	SmartDashboard::PutNumber("Compressor-Pressure", this->compressor->GetPressureSwitchValue());
	
	
	if(compressor) {
		toggleCompressor();
	}

	++periodicCounter;
	
	float x = stickDrive->GetX();
	float y = stickDrive->GetY();
	float z = (stickDrive->GetRawAxis(3)) / 1.5; //actually twist--capped at 2/3 for speed control
	
	if(abs(x) < .125) x =0;
	if(abs(y) < .125) y =0;
	if(abs(z) < .125) z =0;
	
	if(!slowSpeed) {
		//myRobot.MecanumDrive_Cartesian(x, y, z, 0.0);
		MecanumDrive(x, y, z, 0.0);
	} 
	
	else if(slowSpeed) { 
		if (x < 0) x = x/3; 
		else x = x/3; 
		
		if (y < 0) y = y/3;
		else y = y/3;
		
		if (z < 0) z = z/3;
		else z = z/3;
		
		//myRobot.MecanumDrive_Cartesian(x, y, z, 0.0);
		MecanumDrive(x, y, z, 0.0);
	}
	
	//stop robot 
	if (stopDrive){
		stop();
		return;
	}
	
	//extend all arms
	if(extendAll_driver){ 
		extend(s1,s1Status); //spinner arms go out first
		extend(s3,s3Status);
		Wait(.2);
		extend(s2,s2Status);
		return;
	}
	
	//retract all arms
	if(retractAll_driver){
		retract(s2,s2Status); //catcher arms come in first
		Wait(.2);
		retract(s3,s3Status);
		retract(s1,s1Status);
		return;		
	}
	
	//toggle left set of spinners
	if(leftDrop_driver){
		toggleLeftDrop(SPINNER_SPEED);
		return;
	}
	if(leftPick_driver){
		toggleLeftPick(SPINNER_SPEED);
		return;
	}

	//toggle right set of spinners
	if(rightDrop_driver){
		toggleRightDrop(SPINNER_SPEED);
		return;
	}
	if(rightPick_driver){
		toggleRightPick(SPINNER_SPEED);
		return;
	}
	
	//attack3 joystick code below
	
	//extend all arms
	if(extendAll_other){
		extend(s1,s1Status); //spinner arms go out first
		extend(s3,s3Status);
		Wait(.2);
		extend(s2,s2Status);
		Wait(0.5);
		return;
	}
	
	//retract all arms
	if(retractAll_other){
		retract(s2,s2Status); //catcher arms come in first
		Wait(.2);
		retract(s3,s3Status);
		retract(s1,s1Status);
		Wait(0.5);
		return;
	}
	
	if(leftDrop_other){
		spinnerL1->Set(SPINNER_SPEED); //may need to
		spinnerL2->Set(SPINNER_SPEED); //reverse these
		return;
	}
	if(leftPick_other){
		spinnerL1->Set(-SPINNER_SPEED); //may need to
		spinnerL2->Set(-SPINNER_SPEED); //reverse these
		return;
	}

	
	//toggle right set of spinners
	if(rightDrop_other){
		spinnerR1->Set(SPINNER_SPEED);
		spinnerR2->Set(SPINNER_SPEED);
		return;
	}
	if(rightPick_other){
		spinnerR1->Set(-SPINNER_SPEED);
		spinnerR2->Set(-SPINNER_SPEED);
		return;
	}
	
	if (!(leftDrop_driver || leftPick_driver || leftDrop_other || leftPick_other))
	{
		stopSpinnersLeft();
	}
	if (!(rightDrop_driver || rightPick_driver || rightDrop_other || rightPick_other))
	{
		stopSpinnersRight();
	}

	//toggle left spinner arm
	if(leftArm_other) {  
		toggle(s1,s1Status);
		return;
	}

	//toggle right spinner arm
	if(rightArm_other) {
		toggle(s3,s3Status);
		return;
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

}

float RobotDemo::ramp(float sec){
	float currentMotorPower = .083 * sec * sec;
	return currentMotorPower;
}

void RobotDemo::move(float distance) { //distance in feet
	/*if(distance>0){  //calls to motors directly
		victorFL->Set(MOTOR_POWER);
		victorFR->Set(MOTOR_POWER);
		victorBL->Set(MOTOR_POWER);
		victorBR->Set(MOTOR_POWER);
		Wait(distance/ROBOT_VELOCITY);
	}
	else{
		victorFL->Set(-1*MOTOR_POWER);
		victorFR->Set(-1*MOTOR_POWER);
		victorBL->Set(-1*MOTOR_POWER);
		victorBR->Set(-1*MOTOR_POWER);
		Wait(-1*distance/ROBOT_VELOCITY);
	}
	stop();*/
	float sec = 0.0;
	float currentMotorPower = ramp(sec);
			
	if(distance>0){ //forward		
		while(currentMotorPower<MOTOR_POWER){
			//myRobot.MecanumDrive_Cartesian(0,currentMotorPower,0.0,0.0);
			MecanumDrive(0,currentMotorPower,0.0,0.0);
			sec += 0.1;
			currentMotorPower = ramp(sec);
		}
		//myRobot.MecanumDrive_Cartesian(0,MOTOR_POWER,0.0,0.0);
		MecanumDrive(0,MOTOR_POWER,0.0,0.0);
		Wait(distance/ROBOT_VELOCITY-3);
	}
	else{ //backward
		while(currentMotorPower<MOTOR_POWER){
			//myRobot.MecanumDrive_Cartesian(0,-currentMotorPower,0.0,0.0);
			MecanumDrive(0,-currentMotorPower,0.0,0.0);
			sec += 0.1;
			currentMotorPower = ramp(sec);
		}
		//myRobot.MecanumDrive_Cartesian(0,-1*MOTOR_POWER,0.0,0.0);
		MecanumDrive(0,-1*MOTOR_POWER,0.0,0.0);
		Wait(-1*distance/ROBOT_VELOCITY-3);
	}
	stop();
}

void RobotDemo::move_horizontal(float distance){ //distance in feet
	/*if(distance>0){  //to right
		victorFL->Set(MOTOR_POWER);
		victorFR->Set(-1*MOTOR_POWER);
		victorBL->Set(-1*MOTOR_POWER);
		victorBR->Set(MOTOR_POWER);
		Wait(distance/ROBOT_VELOCITY);
	}
	else{ //to left
		victorFL->Set(-1*MOTOR_POWER);
		victorFR->Set(MOTOR_POWER);
		victorBL->Set(MOTOR_POWER);
		victorBR->Set(-1*MOTOR_POWER);
		Wait(-1*distance/ROBOT_VELOCITY);
	}*/	
	if(distance>0){ //to the right
		//myRobot.MecanumDrive_Cartesian(MOTOR_POWER,0.f,0.0,0.0);
		MecanumDrive(MOTOR_POWER,0.f,0.0,0.0);
		Wait(distance/ROBOT_VELOCITY_HORIZ);
	}
	else{ //to the left
		//myRobot.MecanumDrive_Cartesian(-1*MOTOR_POWER,0.f,0.0,0.0);
		MecanumDrive(-1*MOTOR_POWER,0.f,0.f,0.f);
		Wait(-1*distance/ROBOT_VELOCITY_HORIZ);
	}
	stop();
}

void RobotDemo::stop(){
	victorFL->Set(0.0);
	victorFR->Set(0.0);
	victorBL->Set(0.0);
	victorBR->Set(0.0);	
}

void RobotDemo::toggleCompressor(){
	if(compressor->Enabled()){
		compressor->Stop();
	}
	else{
		compressor->Start();
	}
}

void RobotDemo::spinnerLeft(float speed) {
	float sec = 0.0;
	float currentMotorPower = ramp(sec);
		
	if(speed<0.05 && speed>-0.05){
		stopSpinnersLeft();
	}
	else if(speed>0){
		while(currentMotorPower<speed){
			spinnerL1->Set(currentMotorPower);
			spinnerL2->Set(currentMotorPower);
			sec += 0.1;
			currentMotorPower = ramp(sec);
		}
		spinnerL1->Set(speed); //may need to
		spinnerL2->Set(speed); //reverse these
	}
	else{
		while(currentMotorPower<abs(speed)){
			spinnerL1->Set(-currentMotorPower);
			spinnerL2->Set(-currentMotorPower);
			sec += 0.3;
			currentMotorPower = ramp(sec);
		}
		spinnerL1->Set(speed); //may need to
		spinnerL2->Set(speed); //reverse these
	}
}

void RobotDemo::spinnerRight(float speed) {
	float sec = 0.0;
	float currentMotorPower = 0.0;
	
	if(speed<0.05 && speed>-0.05){
		stopSpinnersRight();
	}
	else if(speed>0){
		while(currentMotorPower<speed){
			spinnerR1->Set(currentMotorPower);
			spinnerR2->Set(currentMotorPower);
			sec += 0.1;
			currentMotorPower = ramp(sec);
		}
		spinnerR1->Set(speed); //may need to
		spinnerR2->Set(speed); //reverse these
	}
	else{
		while(currentMotorPower<abs(speed)){
			spinnerR1->Set(-currentMotorPower);
			spinnerR2->Set(-currentMotorPower);
			sec += 0.3;
			currentMotorPower = ramp(sec);
		}
		spinnerR1->Set(speed); //may need to
		spinnerR2->Set(speed); //reverse these
	}
	
}

void RobotDemo::stopSpinnersLeft(){
	spinnerL1->Disable();
	spinnerL2->Disable();
}

void RobotDemo::stopSpinnersRight(){
	spinnerR1->Disable();
	spinnerR2->Disable();
}

void RobotDemo::toggleLeftDrop(float speed){
	if(spinnerStatusLeft){
		spinnerLeft(speed);
		spinnerStatusLeft=false;
	}
	else{
		stopSpinnersLeft();
		spinnerStatusLeft=true;
	}
}	
void RobotDemo::toggleLeftPick(float speed){
	if(spinnerStatusLeft){
		spinnerLeft(-speed);
		spinnerStatusLeft=false;
	}
	else{
		stopSpinnersLeft();
		spinnerStatusLeft=true;
	}
}
	
void RobotDemo::toggleRightDrop(float speed){
	if(spinnerStatusRight){
		spinnerRight(speed);
		spinnerStatusRight=false;
	}
	else{
		stopSpinnersRight();
		spinnerStatusRight=true;
	}
}
	
void RobotDemo::toggleRightPick(float speed){
	if(spinnerStatusRight){
		spinnerRight(-speed);
		spinnerStatusRight=false;
	}
	else{
		stopSpinnersRight();
		spinnerStatusRight=true;
	}
}

void RobotDemo::extend(DoubleSolenoid *solenoid, bool &status){
	//if (periodicCounter < SOLENOID_WAIT_TICKS)
	//{
	//	return;
	//}
		
	solenoid->Set(DoubleSolenoid::kForward);
	status = false;
	
	periodicCounter = 0;

}

void RobotDemo::retract(DoubleSolenoid *solenoid, bool &status){
	//if (periodicCounter < SOLENOID_WAIT_TICKS)
	//{
	//	return;
	//}
		
	solenoid->Set(DoubleSolenoid::kReverse);
	status = true;
	
	periodicCounter = 0;
}

void RobotDemo::toggle(DoubleSolenoid *solenoid, bool &status)
{
	if (periodicCounter < SOLENOID_WAIT_TICKS)
	{
		return;
	}
	
	if(status) {
		solenoid->Set(DoubleSolenoid::kForward);
		status=false;
	}
	else {
		solenoid->Set(DoubleSolenoid::kReverse);
		status=true;
	}
	
	periodicCounter = 0;
}

void RobotDemo::MecanumDrive(float x, float y, float rotation, float gyroAngle)
{
	const float EPSILONFL=1.f, EPSILONFR=1.f, EPSILONBL=1.f, EPSILONBR=1.f;
		
	double xIn = x;
	double yIn = y;
	// Negate y for the joystick.
	yIn = -yIn;
	// Compenstate for gyro angle.
	RotateVector(xIn, yIn, gyroAngle);

	double wheelSpeeds[kMaxNumberOfMotors];
	wheelSpeeds[0] = xIn + yIn + rotation;
	wheelSpeeds[1] = -xIn + yIn - rotation;
	wheelSpeeds[2] = -xIn + yIn + rotation;
	wheelSpeeds[3] = xIn + yIn - rotation;

	Normalize(wheelSpeeds);

	uint8_t syncGroup = 0x80;

	victorFL->Set(EPSILONFL*wheelSpeeds[0] * 1, syncGroup);
	victorFR->Set(EPSILONFR*wheelSpeeds[1] * -1, syncGroup);
	victorBL->Set(EPSILONBL*wheelSpeeds[2] * 1, syncGroup);
	victorBR->Set(EPSILONBR*wheelSpeeds[3] * -1, syncGroup);

	CANJaguar::UpdateSyncGroup(syncGroup);
}

void RotateVector(double &x, double &y, double angle)
{
	double cosA = cos(angle * (3.14159 / 180.0));
	double sinA = sin(angle * (3.14159 / 180.0));
	double xOut = x * cosA - y * sinA;
	double yOut = x * sinA + y * cosA;
	x = xOut;
	y = yOut;
}
void Normalize(double *wheelSpeeds)
{
	double maxMagnitude = fabs(wheelSpeeds[0]);
	int32_t i;
	for (i=1; i<kMaxNumberOfMotors; i++)
	{
		double temp = fabs(wheelSpeeds[i]);
		if (maxMagnitude < temp) maxMagnitude = temp;
	}
	if (maxMagnitude > 1.0)
	{
		for (i=0; i<kMaxNumberOfMotors; i++)
		{
			wheelSpeeds[i] = wheelSpeeds[i] / maxMagnitude;
		}
	}
}

START_ROBOT_CLASS(RobotDemo);
