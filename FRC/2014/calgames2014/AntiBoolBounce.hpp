#ifndef ANTI_BOOL_BOUNCE_HPP
#define ANTI_BOOL_BOUNCE_HPP

class AntiBoolBounce
{
private:
	bool antiBounceBool;
	int sinceLastCommand;
public:
	bool get();
	void set(bool value);
	
	AntiBoolBounce();
	void update();
};

#endif
