#pragma once
#include <windows.h>
#include <string>

static std::string ToUFT8( const char* _string )
{
	int lengthUnicode = 0, lengthUTF = 0;
	wchar_t* UnicodeBuffer = NULL;
	char* multibyteBuffer = NULL;

	if ( ( lengthUnicode = ::MultiByteToWideChar( CP_ACP, 0, _string, ::strlen( _string ), NULL, 0 ) ) < 0 )
	{
		return 0;
	}

	UnicodeBuffer = new wchar_t[lengthUnicode + 1];
	::memset( UnicodeBuffer, 0x00, sizeof( wchar_t ) * ( lengthUnicode + 1 ) );

	// Ansi -> Unicode
	lengthUnicode = ::MultiByteToWideChar( CP_ACP, 0, _string, ::strlen( _string ), UnicodeBuffer, lengthUnicode );

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
	int lengthUnicode = 0, lengthUTF = 0;
	wchar_t* UnicodeBuffer = NULL;
	char* multibyteBuffer = NULL;

	if ( ( lengthUnicode = ::MultiByteToWideChar( CP_UTF8, 0, _string, ::strlen( _string ), NULL, 0 ) ) < 0 )
	{
		return 0;
	}

	UnicodeBuffer = new wchar_t[lengthUnicode + 1];
	::memset( UnicodeBuffer, 0x00, sizeof( wchar_t )*( lengthUnicode + 1 ) );

	// UTF-8 -> Unicode
	lengthUnicode = ::MultiByteToWideChar( CP_UTF8, 0, _string, ::strlen( _string ), UnicodeBuffer, lengthUnicode );

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