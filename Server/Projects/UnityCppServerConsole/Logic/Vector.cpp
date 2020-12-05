#include "Vector.h"

const Vector3 Vector3::One = { 1.0f,  1.0f,  1.0f };
const Vector3 Vector3::Zero = { 0.0f,  0.0f,  0.0f };
const Vector3 Vector3::Left = { -1.0f, 0.0f,  0.0f };
const Vector3 Vector3::Right = { 1.0f,  0.0f,  0.0f };
const Vector3 Vector3::Up = { 0.0f,  1.0f,  0.0f };
const Vector3 Vector3::Down = { 0.0f, -1.0f,  0.0f };
const Vector3 Vector3::Forward = { 0.0f,  0.0f,  1.0f };
const Vector3 Vector3::Backward = { 0.0f,  0.0f, -1.0f };

Vector3::Vector3()
	: x( 0.0f ), y( 0.0f ), z( 0.0f )
{
}

Vector3::Vector3( float _x, float _y, float _z )
	: x( _x ), y( _y ), z( _z )
{
}

Vector3 Vector3::GetNormalize() const
{
	const float length = std::sqrtf( x * x + y * y + z * z );
	if ( length < std::numeric_limits<float>::epsilon() )
	{
		return Vector3::Zero;
	}

	return { x / length, y / length, z / length };
}

float Vector3::GetMagnitude() const
{
	return std::sqrtf( GetSqrMagnitude() );
}

float Vector3::GetSqrMagnitude() const
{
	return std::powf( x, 2 ) + std::powf( y, 2 ) + std::powf( z, 2 );
}

Vector3 Vector3::Lerp( const Vector3& _start, const Vector3& _end, const float& _time )
{
	const float newTime = std::min( _time, 1.0f );
	return _start * ( 1.0f - newTime ) + ( _end * newTime );
}

Vector3 Vector3::Normalize( const Vector3& _vector )
{
	return _vector.GetNormalize();
}

Vector3 Vector3::Cross( const Vector3& _lhs, const Vector3& _rhs )
{
	return { ( _lhs.y * _rhs.z ) - ( _rhs.y * _lhs.z ),
			 ( _rhs.x * _lhs.z ) - ( _lhs.x * _rhs.z ),
			 ( _lhs.x * _rhs.y ) - ( _rhs.x * _lhs.y ) };
}

float Vector3::Dot( const Vector3& _lhs, const Vector3& _rhs )
{
	return ( _lhs.x * _rhs.x ) + ( _lhs.y * _rhs.y ) + ( _lhs.z * _rhs.z );
}

float Vector3::Magnitude( const Vector3& _vector )
{
	return _vector.GetMagnitude();
}

float Vector3::SqrMagnitude( const Vector3& _vector )
{
	return _vector.GetSqrMagnitude();
}

Vector3 Vector3::operator +( const Vector3& _vector ) const
{
	return { x + _vector.x, y + _vector.y, + - _vector.z };
}

Vector3 Vector3::operator -( const Vector3& _vector ) const
{
	return { x - _vector.x, y - _vector.y, z - _vector.z };
}

Vector3 Vector3::operator *( const Vector3& _vector ) const
{
	return { x * _vector.x, y * _vector.y, z * _vector.z };
}

Vector3 Vector3::operator /( const Vector3& _vector ) const
{
	return { x / _vector.x, y / _vector.y, z / _vector.z };
}

Vector3 Vector3::operator *( float _value ) const
{
	return { x * _value, y * _value, z * _value };
}

Vector3 Vector3::operator /( float _value ) const
{
	return { x / _value, y / _value, z / _value };
}
