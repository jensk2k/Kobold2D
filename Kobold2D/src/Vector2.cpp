#include "Vector2.h"

//Vec2f Vec2f::operator+(const Vec2f& other) const
//{
//	return Vec2(x + other.x, y + other.y);
//}
//
//Vec2f& Vec2f::operator+=(const Vec2f& other)
//{
//	x += other.x;
//	y += other.y;
//	return *this;
//}
//
//Vec2f Vec2f::operator-(const Vec2f& other) const
//{
//	return Vec2f(x - other.x, y - other.y);
//}
//
//Vec2f& Vec2f::operator-=(const Vec2f& other)
//{
//	x -= other.x;
//	y -= other.y;
//	return *this;
//}
//
//Vec2f Vec2f::operator*(float scalar) const
//{
//	return Vec2f(x * scalar, y * scalar);
//}
//
//Vec2f& Vec2f::operator*=(float f)
//{
//	x *= f;
//	y *= f;
//	return *this;
//}
//
//Vec2f Vec2f::operator*(const Vec2f& other) const
//{
//	return Vec2f(x * other.x, y * other.y);
//}
//
//Vec2f& Vec2f::operator*=(const Vec2f& other)
//{
//	x *= other.x;
//	y *= other.y;
//	return *this;
//}
//
//Vec2f Vec2f::operator/(float scalar) const
//{
//	return Vec2f(x / scalar, y / scalar);
//}
//
//float Vec2f::Length() const
//{ 
//	float x2y2 = x * x + y * y;
//
//	return x2y2 > 0.f ? sqrt(x2y2) : 0.f; 
//}
//
//float Vec2f::Length2() const
//{
//	return x * x + y * y;
//}
//
//bool Vec2f::IsZero() const
//{
//	return Length2() < 0.0000000001f;
//}
//
//void Vec2f::Normalize()
//{
//	float l = Length();
//	if (l > 0.f)
//	{
//		x /= l;
//		y /= l;
//	}
//	else
//	{
//		x = 0.f;
//		y = 0.f;
//	}
//}
//
//Vec2f Vec2f::Normalized()
//{
//	float l = Length();
//	return Vec2f(x / l, y / l);
//}
//
//void Vec2f::Rotate(float angle)
//{
//	Vec2f v = Rotate(*this, angle);
//	x = v.x;
//	y = v.y;
//}
//
//void Vec2f::RotateAroundPoint(const Vec2f& p, float angle) 
//{
//	Vec2f v = RotateAroundPoint(*this, p, angle);
//	x = v.x;
//	y = v.y;
//}
//
//void Vec2f::RotateAroundPoint(const Vec2f& p, float cosAngle, float sinAngle)
//{
//	Vec2f v = RotateAroundPoint(*this, p, cosAngle, sinAngle);
//	x = v.x;
//	y = v.y;
//}
//
//float Vec2f::Dot(const Vec2f& v1, const Vec2f& v2)
//{
//	return v1.x * v2.x + v1.y * v2.y;
//}
//
//float Vec2f::Angle(const Vec2f& v1, const Vec2f& v2)
//{
//	float dot = Dot(v1, v2);
//	float l1 = v1.Length();
//	float l2 = v2.Length();
//	float angle = acos(dot / (l1 * l2));
//
//	if (angle != angle)
//	{
//		angle = 0.f;
//	}
//	return angle;
//}
//
//float Vec2f::AngleSigned(const Vec2f& v1, const Vec2f& v2)
//{
//	return atan2(v1.x * v2.y - v1.y * v2.x, v1.x * v2.x + v1.y * v2.y);
//}
//
//Vec2f Vec2f::Rotate(const Vec2f& v1, float angle)
//{
//	float sinA = sin(-angle);
//	float cosA = cos(-angle);
//
//	float x = v1.x * cosA - v1.y * sinA;
//	float y = v1.x * sinA + v1.y * cosA;
//
//	return Vec2f(x, y);
//}
//
//Vec2f Vec2f::RotateAroundPoint(const Vec2f& v, const Vec2f& p, float angle)
//{
//	float cosAngle = cos(angle);
//	float sinAngle = sin(angle);
//	return RotateAroundPoint(v, p, cosAngle, sinAngle);
//}
//
//Vec2f Vec2f::RotateAroundPoint(const Vec2f& v, const Vec2f& p, float cosAngle, float sinAngle)
//{	
//	Vec2f t = v - p;
//	float x = t.x * cosAngle - t.y * sinAngle;
//	float y = t.x * sinAngle + t.y * cosAngle;
//	return Vec2f(x, y) + p;
//}

std::ostream& operator<<(std::ostream& os, const Vec2f& vec)
{
	os << "(" << vec.x << "," << vec.y << ")";
	return os;
}
