#pragma once

inline float RandF(float min, float max)
{
	float random = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	float diff = max - min;
	float r = random * diff;
	return min + r;
}

inline int RandI(int min, int max)
{
	return min + rand() % (max - min + 1);
}
template<typename T>
inline void Clamp(T& value, T hi, T lo)
{
	if (value > hi)
		value = hi;

	else if (value < lo)
		value = lo;
}

template<typename T>
inline void ClampMin(T& value, T lo)
{
	if (value < lo)
		value = lo;
}

template<typename T>
inline void ClampMax(T& value, T hi)
{
	if (value > hi)
		value = hi;
}

template<typename T>
inline void Clamp01(T& value)
{
	if (value > 1)
		value = 1;
	else if (value < 0)
		value = 0;
}

inline float RadToDeg(float rad)
{
	return rad * 57.2957795131f;
}

inline float DegToRad(float deg)
{
	return deg * 0.01745329251f;
}

inline float Lerp(float a, float b, float t)
{
	return a + t * (b - a);
}

inline float InverseLerp(float a, float b, float value)
{
	return (value - a) / (b - a);
}

inline std::string ToBinary(unsigned int number)
{
	std::stringstream nr;
	while (number != 0)
	{ 
		nr << number % 2;
		number /= 2;
	}
	nr << "b0";
	string out = nr.str();
	reverse(out.begin(), out.end());
	return out;
}

inline std::string ToHex(unsigned int number)
{
	std::stringstream nr;
	while (number != 0)
	{
		nr << number % 16;
		number /= 16;
	}
	nr << "x0";
	string out = nr.str();
	reverse(out.begin(), out.end());
	return out;
}