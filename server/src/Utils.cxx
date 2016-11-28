#include "Utils.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <openssl/aes.h>
#include <sstream>

#include "Log.h"

char* Utils::encrypt(char* data, int dataLen, char* key, int keyLen, int& outLen)
{
    char* out = new char[AES_BLOCK_SIZE + dataLen];
    size_t blockLen = 0;
    size_t encMsgLen = 0;

    EVP_CIPHER_CTX aesCtx;
    #define AES_KEYLEN  256 //whatever
    char iv[32];
    for(int i = 0 ; i < 32; i++)
        iv[i] = '\0';
    //memset(iv,'\0',AES_KEYLEN/8); // all 0 IV;

    EVP_CIPHER_CTX_init(&aesCtx);

    if(!EVP_EncryptInit_ex(&aesCtx,EVP_aes_256_cbc(), NULL, (const unsigned char*)key,(const unsigned char*)iv))
    {
        logError << "Failed to initiaze AES context";
        return NULL;
    }

    if(!EVP_EncryptUpdate(&aesCtx,(unsigned char*)out,(int*)&blockLen,(unsigned char*)data,dataLen))
    {
        logError << "Failed to encrypt";
        return NULL;
    }
    encMsgLen += blockLen;

    if(!EVP_EncryptFinal_ex(&aesCtx,(unsigned char*)(out + encMsgLen), (int*)&blockLen))
    {
        logError << "Failed to encrypt";
    }

    EVP_CIPHER_CTX_cleanup(&aesCtx);

    outLen = encMsgLen + blockLen;
    return out;
}
char* Utils::decrypt(char* data, int dataLen, char* key, int keyLen, int& outLen)
{
    char* out = new char[dataLen];
    size_t blockLen = 0;
    size_t encMsgLen = 0;

    EVP_CIPHER_CTX aesCtx;
    char iv[AES_KEYLEN/8];
    memset(iv,'\0',AES_KEYLEN/8); // all 0 IV;

    EVP_CIPHER_CTX_init(&aesCtx);

    if(!EVP_DecryptInit_ex(&aesCtx,EVP_aes_256_cbc(), NULL, (const unsigned char*)key,(const unsigned char*)iv))
    {
        logError << "Failed to initiaze AES context";
        return NULL;
    }
    if(!EVP_DecryptUpdate(&aesCtx,(unsigned char*)out,(int*)&blockLen,(unsigned char*)data,dataLen))
    {
        logError << "Failed to decrypt";
        return NULL;
    }
    encMsgLen += blockLen;

    if(!EVP_DecryptFinal_ex(&aesCtx,(unsigned char*)(out + encMsgLen), (int*)&blockLen))
    {
        logError << "Failed to decrypt2";
    }

    EVP_CIPHER_CTX_cleanup(&aesCtx);

    outLen = encMsgLen + blockLen;
    return out;
}
std::string Utils::hex(char* data,int len)
{
    char buff[(len * 2) +1];
    memset(buff,(len*2) +1,'\0');
    for(int i = 0; i < len; i++)
    {
        sprintf(buff + (i * 2),"%02X",(unsigned char)data[i]);
    }
    buff[len*2] = '\0';
    return std::string(buff);
}

char* Utils::generateRandom(int size)
{
    static bool initOnce = false;
    if(initOnce)
    {
        srand(time(NULL));
        initOnce = true;
    }

    char* out = new char[size];
    for(int i = 0; i < size; i++)
    {
        out[i] = rand() % 256;
    }
    return out;
}
