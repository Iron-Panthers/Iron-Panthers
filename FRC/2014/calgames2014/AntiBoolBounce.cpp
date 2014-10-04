#include "AntiBoolBounce.hpp"
#include <time.h>

AntiBoolBounce::AntiBoolBounce()
	: sinceLastCommand(0)
{
}

bool AntiBoolBounce::get()
{
	return antiBounceBool;
}
void AntiBoolBounce::set(bool value)
{
	if (sinceLastCommand > 8)
	{
		sinceLastCommand = 0;
		antiBounceBool = value;
	}
}

void AntiBoolBounce::update()
{
	sinceLastCommand++;
}
