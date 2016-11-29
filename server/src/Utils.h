#ifndef __Utils_h__
#define __Utils_h__

#include <iostream>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/x509.h>

typedef unsigned char byte;

struct Utils
{

    static byte* encrypt(byte* data, int dataLen, byte* key, int keyLen, int& outLen);
    static byte* decrypt(byte* data, int dataLen, byte* key, int keyLen, int& outLen);
    static std::string hex(byte* data,int len);
    static byte* generateRandom(int size);
    static byte* fromHex(const char* data,int len, int& outLen);

    static std::string fromNumber(int64_t num);
};

#endif
