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


char* l_md5(const char* filename)
{
  char *ret = (char*)malloc(33 * sizeof(char));
  memset(ret, 0, 33);
#ifdef WIN32
    WCHAR wTMP[1024];
    DWORD dwStatus = 0;
    BOOL bResult = FALSE;
    HCRYPTPROV hProv = 0;
    HCRYPTHASH hHash = 0;
    HANDLE hFile = NULL;
    BYTE rgbFile[BUFSIZE];
    DWORD cbRead = 0;
    BYTE rgbHash[MD5LEN];
    DWORD cbHash = 0;
    CHAR rgbDigits[] = "0123456789abcdef";
    // Logic to check usage goes here.

	WCHAR Wfilename[strlen(filename)+1];
	CPY_CHAR_to_WCHAR(filename, Wfilename);
    hFile = CreateFile(Wfilename,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_SEQUENTIAL_SCAN,
        NULL);

    if (INVALID_HANDLE_VALUE == hFile)
    {
        dwStatus = GetLastError();
/*		swprintf(wTMP, L"Error opening file %s\nError: %ld\n", to_WCHAR(filename), dwStatus);
		Info_Message(wTMP, 0);	*/
        return ret;
    }

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
        CloseHandle(hFile);
        return ret;
    }

    if (!CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash))
    {
        dwStatus = GetLastError();
		swprintf(wTMP, L"CryptCreateHash failed: %ld\n", dwStatus);
		Info_Message(wTMP, 0);
        CloseHandle(hFile);
        CryptReleaseContext(hProv, 0);
        return ret;
    }

    while ((bResult = ReadFile(hFile, rgbFile, BUFSIZE, 
        &cbRead, NULL)))
    {
        if (0 == cbRead)
        {
            break;
        }

        if (!CryptHashData(hHash, rgbFile, cbRead, 0))
        {
            dwStatus = GetLastError();
			swprintf(wTMP, L"CryptHashData failed: %ld\n", dwStatus);
			Info_Message(wTMP, 0);
            CryptReleaseContext(hProv, 0);
            CryptDestroyHash(hHash);
            CloseHandle(hFile);
            return ret;
        }
    }

    if (!bResult)
    {
        dwStatus = GetLastError();
		swprintf(wTMP, L"ReadFile failed: %ld\n", dwStatus);
		Info_Message(wTMP, 0);
        CryptReleaseContext(hProv, 0);
        CryptDestroyHash(hHash);
        CloseHandle(hFile);
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
    CloseHandle(hFile);
#else
    unsigned char digest[MD5LEN];
    MD5_CTX ctx;
    MD5_Init(&ctx);

    FILE *fp = fopen(filename, "rb");
    char buff[BUFSIZE];
    int buffSize;
    while ((buffSize = fread (buff, 1, BUFSIZE, fp)) != 0) MD5_Update(&ctx, buff, buffSize);
    fclose(fp);

    MD5_Final(digest, &ctx);
    for (int i = 0; i < MD5LEN; i++) sprintf(&ret[i*2], "%02x", digest[i]);
#endif
    return ret;
}
