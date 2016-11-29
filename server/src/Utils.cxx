#include "Utils.h"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <openssl/aes.h>
#include <sstream>

#include "Log.h"

byte* Utils::encrypt(byte* data, int dataLen, byte* key, int keyLen, int& outLen)
{
    byte* out = new byte[AES_BLOCK_SIZE + dataLen];
    size_t blockLen = 0;
    size_t encMsgLen = 0;

    EVP_CIPHER_CTX aesCtx;
    #define AES_KEYLEN  256 //whatever 
    byte iv[32];
    for(int i = 0 ; i < 32; i++)
        iv[i] = '\0';
    //memset(iv,'\0',AES_KEYLEN/8); // all 0 IV;

    EVP_CIPHER_CTX_init(&aesCtx);

    if(!EVP_EncryptInit_ex(&aesCtx,EVP_aes_256_cbc(), NULL, (const unsigned char*)key,(const unsigned char*)iv))
    {
        logError << "Failed to initiaze AES context";
        return NULL;
    }

    if(!EVP_EncryptUpdate(&aesCtx,(byte*)out,(int*)&blockLen,(byte*)data,dataLen))
    {
        logError << "Failed to encrypt";
        return NULL;
    }
    encMsgLen += blockLen;

    if(!EVP_EncryptFinal_ex(&aesCtx,(byte*)(out + encMsgLen), (int*)&blockLen))
    {
        logError << "Failed to encrypt";
    }

    EVP_CIPHER_CTX_cleanup(&aesCtx);

    outLen = encMsgLen + blockLen;
    return out;
}
byte* Utils::decrypt(byte* data, int dataLen, byte* key, int keyLen, int& outLen)
{
    byte* out = new byte[dataLen];
    size_t blockLen = 0;
    size_t encMsgLen = 0;

    EVP_CIPHER_CTX aesCtx;
    byte iv[AES_KEYLEN/8];
    memset(iv,'\0',AES_KEYLEN/8); // all 0 IV;

    EVP_CIPHER_CTX_init(&aesCtx);

    if(!EVP_DecryptInit_ex(&aesCtx,EVP_aes_256_cbc(), NULL, (const byte*)key,(const byte*)iv))
    {
        logError << "Failed to initiaze AES context";
        return NULL;
    }
    if(!EVP_DecryptUpdate(&aesCtx,(byte*)out,(int*)&blockLen,(byte*)data,dataLen))
    {
        logError << "Failed to decrypt";
        return NULL;
    }
    encMsgLen += blockLen;

    if(!EVP_DecryptFinal_ex(&aesCtx,(byte*)(out + encMsgLen), (int*)&blockLen))
    {
        logError << "Failed to decrypt2";
    }

    EVP_CIPHER_CTX_cleanup(&aesCtx);

    outLen = encMsgLen + blockLen;
    return out;
}
std::string Utils::hex(byte* data,int len)
{
    char buff[(len * 2) +1];
    memset(buff,(len*2) +1,'\0');
    for(int i = 0; i < len; i++)
    {
        sprintf(buff + (i * 2),"%02X",data[i]);
    }
    buff[len*2] = '\0';
    return std::string(buff);
}

byte* Utils::fromHex(const char* data,int len, int& outLen)
{
    byte* out = new byte[len/2];
    memset(out,len/2,'\0');
    assert(len % 2 == 0); //\0 at the end makes it uneven
	for (int i = 0; i < (len / 2); i++) 
	{
        sscanf(data+ 2*i, "%02x", &out[i]);
    }
    outLen = len / 2;


    return (byte*)out;
}

byte* Utils::generateRandom(int size)
{
    static bool initOnce = false;
    if(!initOnce)
    {
        srand(time(NULL));
        initOnce = true;
    }

    byte* out = new byte[size];
    for(int i = 0; i < size; i++)
    {
        out[i] = rand() % 256;
    }
    return out;
}

std::string Utils::fromNumber(int64_t num)
{
    std::stringstream ss;
    ss << num;
    return ss.str();
}
