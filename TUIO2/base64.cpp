/* 
 * a very simple base64 encoder, licensed as public domain. original source:
 * https://en.wikibooks.org/wiki/Algorithm_Implementation/Miscellaneous/Base64
*/

#include "base64.h"
 
const static unsigned char encodeLookup[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
const static unsigned char padCharacter = '=';

std::string base64( uint8_t* cursor, size_t size ) {

	std::string encodedString;
	uint32_t temp;

	encodedString.reserve(((size/3) + (size % 3 > 0)) * 4);

	for (size_t idx = 0; idx < size/3; idx++) {

		temp  = (*cursor++) << 16;
		temp += (*cursor++) << 8;
		temp += (*cursor++);

		encodedString.append( 1, encodeLookup[(temp & 0x00FC0000) >> 18] );
		encodedString.append( 1, encodeLookup[(temp & 0x0003F000) >> 12] );
		encodedString.append( 1, encodeLookup[(temp & 0x00000FC0) >> 6 ] );
		encodedString.append( 1, encodeLookup[(temp & 0x0000003F)      ] );
	}

	switch (size % 3) {
		case 1:
			temp = (*cursor++) << 16;
			encodedString.append( 1, encodeLookup[(temp & 0x00FC0000) >> 18] );
			encodedString.append( 1, encodeLookup[(temp & 0x0003F000) >> 12] );
			encodedString.append( 2, padCharacter );
			break;
		case 2:
			temp  = (*cursor++) << 16;
			temp += (*cursor++) << 8;
			encodedString.append( 1, encodeLookup[(temp & 0x00FC0000) >> 18] );
			encodedString.append( 1, encodeLookup[(temp & 0x0003F000) >> 12] );
			encodedString.append( 1, encodeLookup[(temp & 0x00000FC0) >> 6 ] );
			encodedString.append( 1, padCharacter);
			break;
	}

	return encodedString;
}

