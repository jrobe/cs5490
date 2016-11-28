#ifndef __Utils_h__
#define __Utils_h__

#include <iostream>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/x509.h>

struct Utils
{

    static char* encrypt(char* data, int dataLen, char* key, int keyLen, int& outLen);
    static char* decrypt(char* data, int dataLen, char* key, int keyLen, int& outLen);
    static std::string hex(char* data,int len);
    static char* generateRandom(int size);
};

#endif
