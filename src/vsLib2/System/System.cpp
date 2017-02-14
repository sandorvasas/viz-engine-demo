#include "System.h"


/*
===================================
	CompressByteArray
===================================
*/
void vsSystem::CompressByteArray( byte* byteArray, uint len, int compressionSign, byte* outArray, uint& outLen )
{
	uint i=0, j, k;
	byte* compressed = new byte[len];
	while( i < len )
	{
		j = i;
		while( byteArray[i] == byteArray[j] ) i++;
	}
}

/*
===================================
	DecompressByteArray
===================================
*/
void vsSystem::DecompressByteArray( byte* byteArray, uint len, int compressionSign, byte* outArray, uint& outLen )
{
}