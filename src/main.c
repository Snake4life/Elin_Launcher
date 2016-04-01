#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "globale.h"
#include "resource.h"
#include "unzip.h"

#define CMD_1 "--upd"
#define CMD_2 "--new"
#define CMD_3 "--set"
#define CMD_4 "--out"
#define CMD_5 "--prt"

#define URL_HEAD_t "URL Paths :"
#define URL_HEAD_t_s sizeof(URL_HEAD_t)

const char URL_HEAD[URL_HEAD_t_s+129*3] = URL_HEAD_t;
const char *URL_SRC; //	http://127.0.0.1:14750/Tera/
const char *URL_WEB; //	http://127.0.0.1:14750/Tera/
const char *URL_LST; //	server/serverlist.
const char *Update_Link = "http://37.187.109.105:8080/tera/Elin_Launcher.zip";

const wchar_t *BUTTON_PLAY = L"PLAY";
const wchar_t *BUTTON_CHECK = L"Check";
const wchar_t *BUTTON_SITE = L"Site";

const char *Exe_Name;

char *loginName, *loginPass, loginSave;
const char *loginLang;
const char * LANG_ARRAY[][2] = {
	{(const char*)L"EN", "en"},
	{(const char*)L"FR", "fr"},
	{(const char*)L"DE", "de"},
	{NULL},
};

#ifdef WIN32
extern HWND hWnd;
#endif

int write_test(const char *dirName)
{
  char tstname[strlen(dirName)+8+1];
  sprintf(tstname, "%s/tst.txt", dirName);
  FILE *tst = fopen(tstname, "wb");
  if (!tst) return 1;
  int ret = fwrite("tst", 1, 3, tst);
  fclose(tst);
  unlink(tstname);
  if (ret <= 0) return 1;
  return 0;
}

void loop_deleter(const char* filename)
{
  do {
    usleep(100);
    unlink(filename);
  } while (file_exist(filename));
}

int Set_URL_Param(const char**Exe_Argv)
{
  if (strcmp(Exe_Argv[1], CMD_1) && strcmp(Exe_Argv[1], CMD_2) &&
		strcmp(Exe_Argv[1], CMD_3) && strcmp(Exe_Argv[1], CMD_4) && strcmp(Exe_Argv[1], CMD_5)) return 0;
  char tmp[1024];
  FILE *fp = fopen(Exe_Argv[0], "rb");
  fseek(fp, 0, SEEK_END);
  size_t size = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  char buffer[size];
  size = fread(buffer, 1, size, fp);
  fclose(fp);
  fp = NULL;
  if (!strcmp(Exe_Argv[1], CMD_3)) {
	sprintf(tmp, "%s.del2", Exe_Argv[0]);
	rename(Exe_Argv[0], tmp);
  }
  size_t pos = 0;
  while (pos < size && memcmp(&buffer[pos], URL_HEAD, URL_HEAD_t_s)) pos++;
  if (pos < size) {
    pos += URL_HEAD_t_s;
	memset(buffer+pos, 0, 129);
	strncpy(buffer+pos, !strcmp(Exe_Argv[1], CMD_1) ? URL_SRC: Exe_Argv[2], 128);
	buffer[pos+128] = 0;
    pos += 129;
	memset(buffer+pos, 0, 129);
	strncpy(buffer+pos, !strcmp(Exe_Argv[1], CMD_1) ? URL_WEB: Exe_Argv[3], 128);
	buffer[pos+128] = 0;
    pos += 129;
	memset(buffer+pos, 0, 129);
	strncpy(buffer+pos, !strcmp(Exe_Argv[1], CMD_1) ? URL_LST: Exe_Argv[4], 128);
	buffer[pos+128] = 0;
  }
  memset(tmp, 0, 1024);
  if (!strcmp(Exe_Argv[1], CMD_2)) {
	strncpy(tmp, Exe_Argv[0], strlen(Exe_Argv[0])-3);
	char tmp2[10];
	int i=0;
	do {
		sprintf(tmp2, "new%i.exe", i);
		strcpy(&tmp[strlen(Exe_Argv[0])-3], tmp2);
		i++;
	} while (file_exist(tmp));
  } else if (!strcmp(Exe_Argv[1], CMD_5)) {
	strncpy(tmp, Exe_Argv[0], strlen(Exe_Argv[0])-4);
	strcpy(&tmp[strlen(tmp)], " ");
	strcpy(&tmp[strlen(tmp)], Exe_Argv[5]);
	strcpy(&tmp[strlen(tmp)], ".exe");
  } else if (!strcmp(Exe_Argv[1], CMD_1)) strcpy(tmp, Exe_Argv[2]);
  else if (!strcmp(Exe_Argv[1], CMD_1)) strcpy(tmp, Exe_Argv[5]);
  else strcpy(tmp, Exe_Argv[0]);
  fp = fopen(tmp, "wb");
  fwrite(buffer, 1, size, fp);
  fclose(fp);
  if (!strcmp(Exe_Argv[1], CMD_1)) {
	wchar_t *wTmp = to_WCHAR(Exe_Argv[2]);
	ShellExecute(NULL, NULL, wTmp, NULL, NULL, SW_SHOWDEFAULT);
	free(wTmp);
  }
  if (!strcmp(Exe_Argv[1], CMD_3)) {
	wchar_t *wTmp = to_WCHAR(Exe_Argv[0]);
	ShellExecute(NULL, NULL, wTmp, L"--clean", NULL, SW_SHOWDEFAULT);
	free(wTmp);
  }
  return 1;
}

int main(int argc, const char **argv)
{
  Exe_Name = argv[0];
  URL_SRC = &URL_HEAD[URL_HEAD_t_s+129*0];
  URL_WEB = &URL_HEAD[URL_HEAD_t_s+129*1];
  URL_LST = &URL_HEAD[URL_HEAD_t_s+129*2];
  if (write_test(".")) {
    Info_Message(L"Can't write in my directory !", 1);
    return 1;
  }
  char EXEtmp[strlen(Exe_Name+5+1)];
  sprintf(EXEtmp, "%s.del1", Exe_Name);
  loop_deleter(EXEtmp);
  sprintf(EXEtmp, "%s.del2", Exe_Name);
  loop_deleter(EXEtmp);
  if (argc > 4 && Set_URL_Param(argv)) return 0;
//  if (argc-1 && !strcmp(argv[1], "--param") && GUI_Param()) return 0;
  if (argc-1 && !strcmp(argv[1], "--md5")) {
    md5_list();
	Info_Message(L"MD5 Files list was created !", 0);
	return 0;
  }
  if (argc-1 && !strcmp(argv[1], "--clean")) return 0;
  if (argc-1 && !strcmp(argv[1], "--help")) {
	Info_Message(L"ARGs Lists for URL parameter:\n\t--md5 : create md5 liste for check files\n\tNew exe : \"--new URL_DIR_GAME_FILE URL_WEBSITE PATH_SERVERLIST_IN_WEBSITE\"\n\tSet : \"--set URL_DIR_GAME_FILE URL_WEBSITE PATH_SERVERLIST_IN_WEBSITE\"\n\tOut file : \"--out URL_DIR_GAME_FILE URL_WEBSITE PATH_SERVERLIST_IN_WEBSITE FILENAME\"\nAdded in end of filename : \"--prt URL_DIR_GAME_FILE URL_WEBSITE PATH_SERVERLIST_IN_WEBSITE PART_FILENAME\"", 0);
	return 0;
  }
  if (!dir_exist("Client")) l_mkdir("Client");
  if (write_test("Client")) {
    Info_Message(L"Can't write in \"Client\" directory !", 1);
    return 1;
  }
  loginName = loginPass = NULL;
  loginLang = LANG_ARRAY[0][1];
  loginSave = 0;
  if (file_exist("Elin_Launcher.sav")) {
    FILE *fp = fopen("Elin_Launcher.sav", "rb");
	char tmpLng[3];
	memset(tmpLng, 0, 3);
	fread(tmpLng, 1, 2, fp);
	for (int i=0; LANG_ARRAY[i][0]; i++) {
	    if (!strcmp(tmpLng, LANG_ARRAY[i][1])) loginLang = LANG_ARRAY[i][1];;
	}
	fseek(fp, 0, SEEK_END);
	size_t mx = ftell(fp);
	fseek(fp, 3, SEEK_SET);
	char tmpRst[mx-3+1];
	memset(tmpRst, 0, mx-3+1);
	fread(tmpRst, 1, mx-3, fp);
	char *c = strstr(tmpRst,"\n");
	loginName = (char*)malloc(c-tmpRst+1);
	memset(loginName, 0, c-tmpRst+1);
	strncpy(loginName, tmpRst, c-tmpRst);
	c++;
	loginPass = app_DeCrypt(c, c-tmpRst);
	loginSave = 1;
	fclose(fp);
  }
  cURL_init();
  MainWin();
  cURL_cleanup();
  return 0;
}

void play()
{
  if (!loginName || !strlen(loginName) || !loginPass || !strlen(loginPass) || !loginLang || !strlen(loginLang)) {
    Info_Message(L"Please, enter your :\n\tAccount\n\tPassword\n\tLanguage", 0);
	return;
  }
  if (loginSave) {
    FILE *fp = fopen("Elin_Launcher.sav", "wb");
	fprintf(fp, "%s\n%s\n", loginLang, loginName);
	char *tmp = app_Crypt(loginPass);
	fwrite(tmp, 1, strlen(loginPass), fp);
	free(tmp);
	fclose(fp);
  } else {
	if (file_size("Elin_Launcher.sav")) unlink("Elin_Launcher.sav");
  }
  char *md5Pass = l_md5pass(loginPass);
#ifdef WIN32
  char EArg[strlen("-token: -serverlist: -accountBits:0x7ff09f0001 -masterAccount: -permissions:0 -accessLevel:1 -lastServerInfo:")+strlen(md5Pass)+strlen(URL_WEB)+strlen(URL_LST)+strlen(loginLang)+strlen(loginName)+3+1];
  sprintf(EArg, "-token:%s -serverlist:%s%s%s -accountBits:0x7ff09f0001 -masterAccount:%s -permissions:0 -accessLevel:1 -lastServerInfo:%i", md5Pass, URL_WEB, URL_LST, loginLang, loginName, 0);
#else
  char EArg2[strlen("cd Client && wine NS.exe -token:  -serverlist: -accountBits:0x7ff09f0001 -masterAccount: -permissions:0 -accessLevel:1 -lastServerInfo:")+strlen(md5Pass)+strlen(URL_WEB)+strlen(URL_LST)+strlen(loginLang)+strlen(loginName)+3+1];
  sprintf(EArg2, "cd Client && wine NS.exe -token:%s  -serverlist:%s%s%s -accountBits:0x7ff09f0001 -masterAccount:%s -permissions:0 -accessLevel:1 -lastServerInfo:%i", md5Pass, URL_WEB, URL_LST, loginLang, loginName, 0);
#endif
  free(md5Pass);
#ifdef WIN32
  if (extractExe(IDB_BTN_Exe, "Client\\NS.exe")) {
	Info_Message(L"Can't extract starter in \"Client\" directory !\nCanceling...", 1);
	return;
  }
  wchar_t *wEArg = to_WCHAR(EArg);
  SHELLEXECUTEINFO ShExecInfo = {0};
  ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
  ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
  ShExecInfo.hwnd = hWnd;
  ShExecInfo.lpVerb = NULL;
  ShExecInfo.lpFile = L"NS.exe";        
  ShExecInfo.lpParameters = wEArg;   
  ShExecInfo.lpDirectory = L"Client";
  ShExecInfo.nShow = SW_SHOW;
  ShExecInfo.hInstApp = NULL; 
  ShellExecuteEx(&ShExecInfo);
  ShowWindow(hWnd, SW_HIDE);
  WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
  ShowWindow(hWnd, SW_SHOW);
  free(wEArg);
  loop_deleter("Client\\NS.exe");
#else
  system(Exec_Arg);
#endif
}
