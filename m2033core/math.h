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