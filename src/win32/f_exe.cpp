#include <windows.h>
#include <stdio.h>

#include "f_.hpp"

int extractExe(int id, const char *filename)
{
  HINSTANCE hInstance = GetModuleHandle(NULL);
  LPCTSTR lpName = MAKEINTRESOURCE(id);
  HRSRC hrcData = FindResource(hInstance, lpName, RT_RCDATA);
  HGLOBAL hData = LoadResource(hInstance, hrcData);
  LPVOID pData = LockResource(hData);
  DWORD Size = SizeofResource(hInstance, hrcData);

  FILE *fp = fopen(filename, "wb");
  if (!fp) return 1;
  DWORD ret = fwrite(pData, 1, Size, fp);
  fclose(fp);
  if (ret < Size) {
    _unlink(filename);
	return 2;
  }
  return 0;
}
