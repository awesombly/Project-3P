#pragma once
#include <cereal/cereal.hpp>

struct Vector3
{
public:
	float x;
	float y;
	float z;

public:
	Vector3();
	Vector3( float x, float y, float z );

	Vector3 GetNormalize() const;
	float GetMagnitude() const;
	float GetSqrMagnitude() const;

public:
	static const Vector3 One;
	static const Vector3 Zero;
	static const Vector3 Left;
	static const Vector3 Right;
	static const Vector3 Up;
	static const Vector3 Down;
	static const Vector3 Forward;
	static const Vector3 Backward;

public:
	static Vector3 Lerp( const Vector3& _start, const Vector3& _end, const float& _time );
	static Vector3 Normalize( const Vector3& _vector );
	static Vector3 Cross( const Vector3& _lhs, const Vector3& _rhs );
	static float Dot( const Vector3& _lhs, const Vector3& _rhs );
	static float Magnitude( const Vector3& _vector );
	static float SqrMagnitude( const Vector3& _vector );

public:
	Vector3 operator +( const Vector3& _vector ) const;
	Vector3 operator -( const Vector3& _vector ) const;
	Vector3 operator *( const Vector3& _vector ) const;
	Vector3 operator /( const Vector3& _vector ) const;
	Vector3 operator *( float _value ) const;
	Vector3 operator /( float _value ) const;

public:
	template <class Archive>
	void serialize( Archive& ar )
	{
		ar( CEREAL_NVP( x ) );
		ar( CEREAL_NVP( y ) );
		ar( CEREAL_NVP( z ) );
	}
};
