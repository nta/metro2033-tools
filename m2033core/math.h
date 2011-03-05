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

#ifndef __M2033_MATH_H__
#define __M2033_MATH_H__

#include "prerequisites.h"

namespace m2033
{
	class vec2
	{
	public:
		float x, y;

		vec2();
		vec2( float _x, float _y );
		~vec2();

		vec2 operator + ( const vec2& vector ) const;
		vec2 operator - ( const vec2& vector ) const;
		vec2 operator * ( const vec2& vector ) const;
		vec2 operator * ( const float val ) const;
		 vec2 operator / ( const vec2& vector ) const;
		vec2 operator / ( const float val ) const;

		vec2& operator += ( const vec2& vector );
		vec2& operator -= ( const vec2& vector );
		vec2& operator *= ( const vec2& vector );
		vec2& operator *= ( const float val );
		vec2& operator /= ( const vec2& vector );
		vec2& operator /= ( const float val );

		bool operator == ( const vec2& vector ) const;
		bool operator != ( const vec2& vector ) const;

		float dot( const vec2& vector );
		float dotabs( const vec2& vector );

		vec2& operator = ( const vec2& vector );

		float operator [] ( const unsigned id );

		bool operator > ( const vec2& vector );
		bool operator < ( const vec2& vector );

		float length();
	};

	class vec3
	{
	public:
		float x, y, z;

		vec3();
		vec3( float _x, float _y, float _z );
		~vec3();

		vec3 operator + ( const vec3& vector ) const;
		vec3 operator - ( const vec3& vector ) const;
		vec3 operator * ( const vec3& vector ) const;
		vec3 operator * ( const float val ) const;
		 vec3 operator / ( const vec3& vector ) const;
		vec3 operator / ( const float val ) const;

		vec3& operator += ( const vec3& vector );
		vec3& operator -= ( const vec3& vector );
		vec3& operator *= ( const vec3& vector );
		vec3& operator *= ( const float val );
		vec3& operator /= ( const vec3& vector );
		vec3& operator /= ( const float val );

		bool operator == ( const vec3& vector ) const;
		bool operator != ( const vec3& vector ) const;

		float dot( const vec3& vector );
		float dotabs( const vec3& vector );

		vec3& operator = ( const vec3& vector );

		float operator [] ( const unsigned id );

		bool operator > ( const vec3& vector );
		bool operator < ( const vec3& vector );

		float length();
	};
}

#endif // __M2033_MATH_H__