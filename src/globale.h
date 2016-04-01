#ifndef GLOBALE_H
#define GLOBALE_H

#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#ifdef WIN32
	#include <windows.h>
	#define usleep(a) Sleep(a / 100)
	#define sleep(a) Sleep(a * 1000)
#endif

//MAIN.C
extern const char *URL_SRC, *URL_WEB, *Update_Link;
extern const char * LANG_ARRAY[][2];
extern char sTat;
extern char *loginName, *loginPass, loginSave;
extern const char *loginLang, *Exe_Name;
int Set_URL_Param(const char**);
void play();
void md5_list();

// UPDATE.C
void update();
int update_launcher();

// CURL.C
int dir_exist(const char *);
int file_exist(const char *);
size_t file_size(const char *);
void l_mkdir(const char*);
void set_B_size_reset();
void set_add_B_pos(long long);
void set_B_size(long long);
int cURL_init();
int cURL_cleanup();
int cURL_to_(const char *URL, char **ram, const char *outFile);
int cURL_ping(const char *URL);

// VERIF
char* l_md5(const char*);
char* l_md5pass(const char*);

// WCHAR
wchar_t *to_WCHAR(const char *c);
char *to_CHAR(const wchar_t *wc);
void CPY_WCHAR_to_CHAR(const wchar_t *wc, char *c);
void CPY_CHAR_to_WCHAR(const char *c, wchar_t *wc);

// CRYPT
char * app_Crypt(char *dataIn);
char * app_DeCrypt(char *dataIn, int len);

// UNZIP
int l_unzip(const char *);

// GUI
int MainWin();
int extractExe(int id, const char *filename);
void Info_Progress(double pos, double size);
void Info_Message(const wchar_t *mess, int etat);
void Info_Message_t(const wchar_t *mess, int etat);
	// etat :: 0 = info , 1 = error

#ifndef unlink
#define unlink remove
#endif
#endif
