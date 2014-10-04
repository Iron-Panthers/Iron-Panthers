#ifndef THREE_MOTOR_GROUP_HPP
#define THREE_MOTOR_GROUP_HPP

class ThreeMotorGroup : public SpeedController
{
private:
	SpeedController *one, *two, *three;
public:
	ThreeMotorGroup(SpeedController *one, SpeedController *two, SpeedController *three);
	
	virtual void Set(float speed, uint8_t syncGroup);
	virtual float Get();
	virtual void Disable();
	virtual void PIDWrite(float output);
	
	virtual ~ThreeMotorGroup();
};

#endif
