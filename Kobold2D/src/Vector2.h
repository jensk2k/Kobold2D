#pragma once
#include <math.h>
#include <iostream>

template <typename T>
struct Vec2
{	T x{};
	T y{};
	//Vec2() : x(0.f), y(0.f) {}
	Vec2(){}
	Vec2(float x, float y) : x(x), y(y) {}
	Vec2(const Vec2<T>& other) : x(other.x), y(other.y) {}

	Vec2<T> operator+(const Vec2<T>& other) const
	{
		return Vec2(x + other.x, y + other.y);
	}
	Vec2<T>& operator+=(const Vec2<T>& other)
	{
		x += other.x;
		y += other.y;
		return *this;
	}
	Vec2<T> operator-(const Vec2<T>& other) const
	{
		return Vec2<T>(x - other.x, y - other.y);
	}
	Vec2<T>& operator-=(const Vec2<T>& other)
	{
		x -= other.x;
		y -= other.y;
		return *this;
	}
	Vec2<T> operator*(T scalar) const
	{
		return Vec2<T>(x * scalar, y * scalar);
	}
	Vec2<T>& operator*=(T f)
	{
		x *= f;
		y *= f;
		return *this;
	}
	Vec2<T> operator*(const Vec2<T>& other) const
	{
		return Vec2<T>(x * other.x, y * other.y);
	}
	Vec2<T>& operator*=(const Vec2<T>& other)
	{
		x *= other.x;
		y *= other.y;
		return *this;
	}
	Vec2<T> operator/(T scalar) const
	{
		return Vec2<T>(x / scalar, y / scalar);
	}
	float Length() const
	{
		float x2y2 = x * x + y * y;
		return x2y2 > 0.f ? sqrt(x2y2) : T{};
	}
	T Length2() const { return x * x + y * y; }
	bool IsZero() const { return Length2() < 0.0000000001f; }
	void Normalize() {
		T l = Length();
		if (l > T{})
		{
			x /= l;
			y /= l;
		}
		else
		{
			x = T{};
			y = T{};
		}
	}
	Vec2<T> Normalized()
	{
		float l = Length();
		return Vec2<T>(x / l, y / l);
	}
	void Rotate(float angle)
	{
		Vec2<T> v = Rotate(*this, angle);
		x = v.x;
		y = v.y;
	}
	void RotateAroundPoint(const Vec2<T>& p, float angle)
	{
		Vec2<T> v = RotateAroundPoint(*this, p, angle);
		x = v.x;
		y = v.y;
	}
	void RotateAroundPoint(const Vec2<T>& p, float cosAngle, float sinAngle)
	{
		Vec2<T> v = RotateAroundPoint(*this, p, cosAngle, sinAngle);
		x = v.x;
		y = v.y;
	}
	static float Dot(const Vec2<T>& v1, const Vec2<T>& v2) { return v1.x * v2.x + v1.y * v2.y; }
	static float Angle(const Vec2<T>& v1, const Vec2<T>& v2)
	{
		T dot = Dot(v1, v2);
		T l1 = v1.Length();
		T l2 = v2.Length();
		T angle = acos(dot / (l1 * l2));

		if (angle != angle)
		{
			angle = T{};
		}
		return angle;
	}
	static float AngleSigned(const Vec2<T>& v1, const Vec2<T>& v2)
	{
		return atan2(v1.x * v2.y - v1.y * v2.x, v1.x * v2.x + v1.y * v2.y);
	}
	static Vec2<T> Rotate(const Vec2<T>& v1, float angle)
	{
		float sinA = sin(-angle);
		float cosA = cos(-angle);

		float x = v1.x * cosA - v1.y * sinA;
		float y = v1.x * sinA + v1.y * cosA;

		return Vec2<T>(x, y);
	}
	static Vec2<T> RotateAroundPoint(const Vec2<T>& v, const Vec2<T>& p, float angle)
	{
		float cosAngle = cos(angle);
		float sinAngle = sin(angle);
		return RotateAroundPoint(v, p, cosAngle, sinAngle);
	}
	static Vec2<T> RotateAroundPoint(const Vec2<T>& v, const Vec2<T>& p, float cosAngle, float sinAngle)
	{
		Vec2<T> t = v - p;
		float x = t.x * cosAngle - t.y * sinAngle;
		float y = t.x * sinAngle + t.y * cosAngle;
		return Vec2<T>(x, y) + p;
	}
};

using Vec2f = Vec2<float>;

std::ostream& operator<<(std::ostream& os, const Vec2f& vec);