/* 
 * a very simple base64 encoder, licensed as public domain. original source:
 * https://en.wikibooks.org/wiki/Algorithm_Implementation/Miscellaneous/Base64
*/

#ifndef _BASE64_H_
#define _BASE64_H_

#include <stdint.h>
#include <vector>
#include <string>
 
std::string base64( uint8_t* buffer, size_t size );

#endif // _BASE64_H_
