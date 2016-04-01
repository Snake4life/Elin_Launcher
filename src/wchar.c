#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>


wchar_t *to_WCHAR(const char *c)
{
  const size_t cSize = strlen(c)+1;
  wchar_t *wc = new wchar_t[cSize];
  mbstowcs (wc, c, cSize);
  return wc;
}

char *to_CHAR(const wchar_t *wc)
{
  const size_t wcSize = wcslen(wc)+1;
  char *c = new char[wcSize];
  wcstombs (c, wc, wcSize);
  return c;
}

void CPY_WCHAR_to_CHAR(const wchar_t *wc, char *c)
{
  const size_t wcSize = wcslen(wc)+1;
  wcstombs (c, wc, wcSize);
}

void CPY_CHAR_to_WCHAR(const char *c, wchar_t *wc)
{
  const size_t cSize = strlen(c)+1;
  mbstowcs (wc, c, cSize);
}
