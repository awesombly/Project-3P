#pragma once
#include <string>

static std::string operator ""_s( const char* _str, size_t _len )
{
	return ( std::string( _str, _len ) );
}

template<class Type>
static void SafeDelete( Type*& _pointer )
{
	if ( _pointer != nullptr )
	{
		delete _pointer;
		_pointer = nullptr;
	}
}

template<class Type>
static void SafeDeleteArray( Type*& _pointer )
{
	if ( _pointer != nullptr )
	{
		delete[] _pointer;
		_pointer = nullptr;
	}
}

struct LoginInfo
{
	std::string ID;
	std::string PW;
};

static std::string SimpleJsonToString( std::string::const_iterator& _iter )
{
	std::string result;
	while ( *( ++_iter ) != ':' )
	{
		continue;
	}

	while ( *( ++_iter ) != ',' )
	{
		if ( *_iter == '}' )
		{
			break;
		}

		if ( *_iter == '\"' || *_iter == ' ' )
		{
			continue;
		}

		result.push_back( *_iter );
	}
	return result;
}

static LoginInfo SimpleJsonToLoginInfo( const char* _data )
{
	LoginInfo result;
	std::string data = _data;
	std::string::const_iterator iter = std::begin( data );
	if ( *iter == '{' )
	{
		result.ID = SimpleJsonToString( iter );
		result.PW = SimpleJsonToString( iter );
	}

	return result;
}

static std::string ToUFT8( const char* _string )
{
	int lengthUnicode = 0;
	int lengthUTF = 0;
	wchar_t* UnicodeBuffer = nullptr;
	char* multibyteBuffer = nullptr;

	if ( ( lengthUnicode = ::MultiByteToWideChar( CP_ACP, 0, _string, ( int )::strlen( _string ), NULL, 0 ) ) < 0 )
	{
		return 0;
	}
	UnicodeBuffer = new wchar_t[lengthUnicode + 1];
	::memset( UnicodeBuffer, 0x00, sizeof( wchar_t ) * ( lengthUnicode + 1 ) );

	// Ansi -> Unicode
	lengthUnicode = ::MultiByteToWideChar( CP_ACP, 0, _string, ( int )::strlen( _string ), UnicodeBuffer, lengthUnicode );
	if ( ( lengthUTF = ::WideCharToMultiByte( CP_UTF8, 0, UnicodeBuffer, lengthUnicode, NULL, 0, NULL, NULL ) ) < 0 )
	{
		delete[] UnicodeBuffer;
		return 0;
	}
	multibyteBuffer = new char[lengthUTF + 1];
	::memset( multibyteBuffer, 0x00, sizeof( char ) * ( lengthUTF + 1 ) );

	// Unicode -> UTF-8
	lengthUTF = ::WideCharToMultiByte( CP_UTF8, 0, UnicodeBuffer, lengthUnicode, multibyteBuffer, lengthUTF, NULL, NULL );
	multibyteBuffer[lengthUTF] = 0;
	std::string result( multibyteBuffer );

	delete[] UnicodeBuffer;
	delete[] multibyteBuffer;

	return result;
}

static std::string ToAnsi( const char* _string )
{
	int lengthUnicode = 0;
	int lengthUTF = 0;
	wchar_t* UnicodeBuffer = nullptr;
	char* multibyteBuffer = nullptr;

	if ( ( lengthUnicode = ::MultiByteToWideChar( CP_UTF8, 0, _string, ( int )::strlen( _string ), NULL, 0 ) ) < 0 )
	{
		return 0;
	}
	UnicodeBuffer = new wchar_t[lengthUnicode + 1];
	::memset( UnicodeBuffer, 0x00, sizeof( wchar_t )*( lengthUnicode + 1 ) );

	// UTF-8 -> Unicode
	lengthUnicode = ::MultiByteToWideChar( CP_UTF8, 0, _string, ( int )::strlen( _string ), UnicodeBuffer, lengthUnicode );
	if ( ( lengthUTF = ::WideCharToMultiByte( CP_ACP, 0, UnicodeBuffer, lengthUnicode, NULL, 0, NULL, NULL ) ) < 0 )
	{
		delete[] UnicodeBuffer;
		return 0;
	}
	multibyteBuffer = new char[lengthUTF + 1];
	::memset( multibyteBuffer, 0x00, sizeof( char )*( lengthUTF + 1 ) );

	// Unicode -> Ansi
	lengthUTF = ::WideCharToMultiByte( CP_ACP, 0, UnicodeBuffer, lengthUnicode, multibyteBuffer, lengthUTF, NULL, NULL );
	multibyteBuffer[lengthUTF] = 0;
	std::string result( multibyteBuffer );

	delete[] UnicodeBuffer;
	delete[] multibyteBuffer;

	return result;
}