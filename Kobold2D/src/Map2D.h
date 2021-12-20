#pragma once
#include <vector>

template<typename T>
class Map2D
{
private:
	std::vector<T> m_map;
public:
	int m_width;
	int m_height;

	Map2D(int width, int height) : 
		m_map(width * height, T()),
		m_width(width), 
		m_height(height)
	{
	}

	const T& operator () (unsigned x, unsigned y) const
	{
		return m_map[y * m_width + x];
	}

	T& operator () (unsigned x, unsigned y)
	{
		return m_map[y * m_width + x];
	}
};

class BMap
{
private:
	std::vector<bool> m_map;
public:
	int m_width;
	int m_height;

	BMap(int width, int height) :
		m_map(width* height, false),
		m_width(width),
		m_height(height)
	{
	}

	void Set(unsigned x, unsigned y, bool value)
	{
		m_map[y * m_width + x] = value;
	}

	bool Get(unsigned x, unsigned y) const
	{
		return m_map[y * m_width + x];
	}
};