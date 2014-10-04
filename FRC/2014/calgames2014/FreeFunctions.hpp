#ifndef FREE_FUNCTIONS_HPP
#define FREE_FUNCTIONS_HPP

#include "WPILib.h"

template <typename T>
int sign(T num)
{
	return num >= 0 ? 1 : -1;
}

#endif
