/******************************************************************************
This source file is part of metro2033-tools
For the latest info, see http://code.google.com/p/metro2033-tools/

Copyright (c) 2010-2011 Ivan Shishkin <codingdude@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
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