/******************************************************************************
            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE 
                    Version 2, December 2004 

 Copyright (C) 2004 Sam Hocevar <sam@hocevar.net> 

 Everyone is permitted to copy and distribute verbatim or modified 
 copies of this license document, and changing it is allowed as long 
 as the name is changed. 

            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE 
   TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION 

  0. You just DO WHAT THE FUCK YOU WANT TO.
******************************************************************************/

#include "math.h"

using namespace m2033;

vec2::vec2() : x(0), y(0) {}

vec2::vec2( float _x, float _y ) : x(_x), y(_y) {}

vec2::~vec2() {}

vec2 vec2::operator + ( const vec2& vector ) const
{
	return vec2( x + vector.x,
				y + vector.y );
}

vec2 vec2::operator - ( const vec2& vector ) const
{
	return vec2( x - vector.x,
				y - vector.y );
}

vec2 vec2::operator * ( const vec2& vector ) const
{
	return vec2( x * vector.x,
				y * vector.y );
}

vec2 vec2::operator * ( const float val ) const
{
	return vec2( x * val,
				y * val );
}

vec2 vec2::operator / ( const vec2& vector ) const
{
	return vec2( x / vector.x,
				y / vector.y );
}

vec2 vec2::operator / ( const float val ) const
{
	return vec2( x / val,
				y / val );
}

vec2& vec2::operator += ( const vec2& vector )
{
	x += vector.x;
	y += vector.y;

	return *this;
}

vec2& vec2::operator -= ( const vec2& vector )
{
	x -= vector.x;
	y -= vector.y;

	return *this;
}

vec2& vec2::operator *= ( const vec2& vector )
{
	x *= vector.x;
	y *= vector.y;

	return *this;
}

vec2& vec2::operator *= ( const float val )
{
	x *= val;
	y *= val;

	return *this;
}

vec2& vec2::operator /= ( const vec2& vector )
{
	x /= vector.x;
	y /= vector.y;

	return *this;
}

vec2& vec2::operator /= ( const float val )
{
	x /= val;
	y /= val;

	return *this;
}

bool vec2::operator == ( const vec2& vector ) const
{
	return ( x == vector.x && y == vector.y );
}

bool vec2::operator != ( const vec2& vector ) const
{
	return ( x != vector.x || y != vector.y );
}

float vec2::dot( const vec2& vector )
{
	return x * vector.x + y * vector.y;
}

float vec2::dotabs( const vec2& vector )
{
	return abs(x * vector.x) + abs(y * vector.y);
}

vec2& vec2::operator = ( const vec2& vector )
{
	 x = vector.x;
	y = vector.y;

	return *this;
}

float vec2::operator [] ( const unsigned id )
{
	assert( id < 3 );
	return *(&x + id);
}

bool vec2::operator > ( const vec2& vector )
{
	return ( ( x > vector.x ) && ( y > vector.y ) );
}

bool vec2::operator < ( const vec2& vector )
{
	return ( ( x < vector.x ) && ( y < vector.y ) );
}

float vec2::length()
{
	return sqrt( x * x + y * y );
}

///////////////////////////////////////////////////////////////////////////////

vec3::vec3() : x(0), y(0), z(0) {}

vec3::vec3( float _x, float _y, float _z ) : x(_x), y(_y), z(_z) {}

vec3::~vec3() {}

vec3 vec3::operator + ( const vec3& vector ) const
{
        return vec3( x + vector.x,
                                        y + vector.y,
                                        z + vector.x );
}

vec3 vec3::operator - ( const vec3& vector ) const
{
        return vec3( x - vector.x,
                                        y - vector.y,
                                        z - vector.z );
}

vec3 vec3::operator * ( const vec3& vector ) const
{
        return vec3( x * vector.x,
                                        y * vector.y,
                                        z * vector.z );
}

vec3 vec3::operator * ( const float val ) const
{
                return vec3( x * val,
                                                y * val,
                                                z * val );
}

vec3 vec3::operator / ( const vec3& vector ) const
{
        return vec3( x / vector.x,
                                        y / vector.y,
                                        z / vector.z );
}

vec3 vec3::operator / ( const float val ) const
{
        return vec3( x / val,
                                        y / val,
                                        z / val );
}

vec3& vec3::operator += ( const vec3& vector )
{
        x += vector.x;
        y += vector.y;
        z += vector.x;

        return *this;
}

vec3& vec3::operator -= ( const vec3& vector )
{
        x -= vector.x;
        y -= vector.y;
        z -= vector.x;

        return *this;
}

vec3& vec3::operator *= ( const vec3& vector )
{
        x *= vector.x;
        y *= vector.y;
        z *= vector.z;

        return *this;
}

vec3& vec3::operator *= ( const float val )
{
        x *= val;
        y *= val;
        z *= val;

        return *this;
}

vec3& vec3::operator /= ( const vec3& vector )
{
        x /= vector.x;
        y /= vector.y;
        z /= vector.z;

        return *this;
}

vec3& vec3::operator /= ( const float val )
{
        x /= val;
        y /= val;
        z /= val;

        return *this;
}

bool vec3::operator == ( const vec3& vector ) const
{
        return ( x == vector.x && y == vector.y && z == vector.z );
}

bool vec3::operator != ( const vec3& vector ) const
{
        return ( x != vector.x || y != vector.y || z != vector.z );
}

float vec3::dot( const vec3& vector )
{
        return x * vector.x + y * vector.y + z * vector.z;
}

float vec3::dotabs( const vec3& vector )
{
        return abs(x * vector.x) + abs(y * vector.y) + abs(z * vector.z);
}

vec3& vec3::operator = ( const vec3& vector )
{
        x = vector.x;
        y = vector.y;
        z = vector.z;

        return *this;
}

float vec3::operator [] ( const unsigned id )
{
        assert( id < 3 );
        return *(&x + id);
}

bool vec3::operator > ( const vec3& vector )
{
        return ( ( x > vector.x ) && ( y > vector.y ) && ( z > vector.z ) );
}

bool vec3::operator < ( const vec3& vector )
{
        return ( ( x < vector.x ) && ( y < vector.y ) && ( z < vector.z ) );
}

float vec3::length()
{
        return sqrt( x * x + y * y + z * z );
}