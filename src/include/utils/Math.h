#pragma once

class Math
{
public:
	// 包括边界！！！只支持int，float等基本数据类型，前两个，谁大谁小无所谓。
	template<typename T>
	static T Clamp(const T value,const T& rangeOne, const T& rangeTwo)
	{
		T maxRange = rangeOne > rangeTwo ? rangeOne : rangeTwo;
		T minRange = rangeOne < rangeTwo ? rangeOne : rangeTwo;

		if (value > maxRange) return maxRange;
		else if (value < minRange) return minRange;

		return value;
	}
};
