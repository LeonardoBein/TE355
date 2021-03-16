#if !defined(_MY_CRYPT_)
#define _MY_CRYPT_

#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <stdio.h>
#include <string.h>


int publicEncrypt(unsigned char * data,int data_len,unsigned char * key, unsigned char *encrypted);
int privateDecrypt(unsigned char * enc_data,int data_len,unsigned char * key, unsigned char *decrypted);
int privateEncrypt(unsigned char * data,int data_len,unsigned char * key, unsigned char *encrypted);
int publicDecrypt(unsigned char * enc_data,int data_len,unsigned char * key, unsigned char *decrypted);



#endif // _MY_CRYPT_

