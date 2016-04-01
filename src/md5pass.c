#include <stdio.h>
#include <string.h>

#ifdef WIN32
	#include <windows.h>
	#include <Wincrypt.h>
#else
	#include <openssl/md5.h>
#endif

#define BUFSIZE 1024
#define MD5LEN  16

#include "globale.h"


char* l_md5pass(const char* Pass)
{
  char *ret = (char*)malloc(33 * sizeof(char));
  memset(ret, 0, 33);
#ifdef WIN32
    WCHAR wTMP[1024];
    DWORD dwStatus = 0;
    HCRYPTPROV hProv = 0;
    HCRYPTHASH hHash = 0;
    BYTE rgbHash[MD5LEN];
    DWORD cbHash = 0;
    CHAR rgbDigits[] = "0123456789abcdef";
    // Logic to check usage goes here.

    // Get handle to the crypto provider
    if (!CryptAcquireContext(&hProv,
        NULL,
        NULL,
        PROV_RSA_FULL,
        CRYPT_VERIFYCONTEXT))
    {
        dwStatus = GetLastError();
		swprintf(wTMP, L"CryptAcquireContext failed: %ld\n", dwStatus);
		Info_Message(wTMP, 0);
        return ret;
    }

    if (!CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash))
    {
        dwStatus = GetLastError();
		swprintf(wTMP, L"CryptCreateHash failed: %ld\n", dwStatus);
		Info_Message(wTMP, 0);
        CryptReleaseContext(hProv, 0);
        return ret;
    }

    if (!CryptHashData(hHash, (PBYTE)Pass, strlen(Pass), 0))
    {
        dwStatus = GetLastError();
		swprintf(wTMP, L"CryptHashData failed: %ld\n", dwStatus);
		Info_Message(wTMP, 0);
        CryptReleaseContext(hProv, 0);
        CryptDestroyHash(hHash);
        return ret;
    }

    cbHash = MD5LEN;
    if (CryptGetHashParam(hHash, HP_HASHVAL, rgbHash, &cbHash, 0))
    {
		DWORD i = 0;
        while (i < cbHash)
        {
            sprintf(&ret[i*2], "%c%c", rgbDigits[rgbHash[i] >> 4],
                rgbDigits[rgbHash[i] & 0xf]);
			i++;
        }
    }
    else
    {
        dwStatus = GetLastError();
		swprintf(wTMP, L"CryptGetHashParam failed: %ld\n", dwStatus);
		Info_Message(wTMP, 0);
    }

    CryptDestroyHash(hHash);
    CryptReleaseContext(hProv, 0);
#else
    unsigned char digest[MD5LEN];
    MD5_CTX ctx;
    MD5_Init(&ctx);

    MD5_Update(&ctx, Pass, strlen(Pass));

    MD5_Final(digest, &ctx);
    for (int i = 0; i < MD5LEN; i++) sprintf(&ret[i*2], "%02x", digest[i]);
#endif
    return ret;
}
