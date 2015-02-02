#ifndef _SHA1_H_
#define _SHA1_H_

#include <stdint.h>
#include <string.h>
#include <vector>

#define SHA1_HASH_SIZE (160/8)

void sha1( uint8_t digest[SHA1_HASH_SIZE], const uint8_t* inbuf, size_t length );

#endif //_SHA1_H_

