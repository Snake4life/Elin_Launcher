#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "globale.h"

typedef struct t_list{
  char *filename;
  void *next;
  void *prev;
} t_list;

t_list *List = NULL;
char sTat = 0;

long long TOTAL_SIZE = 0;
t_list *new_list(t_list *l_List, char *name, size_t size)
{
  t_list *tmpList = (t_list*)malloc(sizeof(t_list));
  if (l_List) l_List->next = tmpList;
  tmpList->next = NULL;
  tmpList->prev = l_List;
  tmpList->filename = (char*)malloc(strlen(name)+1);
  strcpy(tmpList->filename, name);
  TOTAL_SIZE += size;
  return tmpList;
}

void update()
{
  sTat = 1;
  int NAME_DIR_SIZE = 0;
  const char *Q = URL_SRC;
  while ((Q = strchr(&Q[1], '/')))
	if (strlen(Q) > 1) NAME_DIR_SIZE = strlen(&Q[1]);
  t_list *l_List = NULL;
  char *MD5_List = (char*)malloc(1);
  char URL_MD5[strlen(URL_SRC)+9];
  sprintf(URL_MD5, "%s%s", URL_SRC, "MD5.TXT");
  if (cURL_to_(URL_MD5, &MD5_List, NULL)) {
    Info_Message(L"No MD5.TXT found on update server !", 1);
    free(MD5_List);
	sTat = 0;
    set_B_size_reset();
	return;
  }
  set_B_size_reset();
  char *E=NULL, *P = MD5_List;
  long actChar=0, totalChar = 0;
  while (P) {
    if (E) P++;
    E = strstr(P, "\n");
	if (E) totalChar++;
    P = E;
  }
  E=NULL;
  P = MD5_List;
  sTat = 2;
  while (P) {
    if (E) P++;
    E = strstr(P, "\n");
    if (E && strlen(E)) {
	  E[0] = 0;
	  P[32] = 0;
	  char *T = strstr(P+33, "\t");
	  T[0] = 0;
      char *p = strchr(T+1+NAME_DIR_SIZE, '\r');
	  if (p) p[0] = 0;
	  size_t S_s = atol(P+33);
	  if (file_exist(T+1+NAME_DIR_SIZE)) {
	    size_t L_s = file_size(T+1+NAME_DIR_SIZE);
	    char *md5 = l_md5(T+1+NAME_DIR_SIZE);
		if (strcmp(md5, P) || L_s != S_s) l_List = new_list(l_List, T+1, S_s);
		free(md5);
	  } else l_List = new_list(l_List, T+1, S_s);
	  if (l_List && !List) List = l_List;
	}
	actChar++;
//	printf("Checking : %ld%%\r", (long)(actChar*100/totalChar));
	Info_Progress((double)actChar, totalChar);
    P = E;
  }
  free(MD5_List);
//  printf("\n");
  set_B_size(TOTAL_SIZE);
//  printf("Download size: %ld mo.\n", (long)(TOTAL_SIZE/1024/1024));
  sTat = 0;
  t_list *r_List = List;
  while (r_List) {
	char *p = strchr(r_List->filename, '\r');
	if (p) p[0] = 0;
    char URL_file[strlen(URL_SRC)+strlen(r_List->filename+NAME_DIR_SIZE)+1];
	sprintf(URL_file, "%s%s", URL_SRC, r_List->filename+NAME_DIR_SIZE);
    short ret, i = 5;
    do {
	  ret = cURL_to_(URL_file, NULL, r_List->filename+NAME_DIR_SIZE);
	  short ret2 = ret;
	  while (ret2) {
	    ret2 = cURL_ping(URL_SRC);
		sleep(1);
	  }
	  i--;
	} while (i && ret);
	if (ret) {
	  wchar_t TMP_c[(strlen(URL_file)+20+1)*sizeof(wchar_t)];
	  wchar_t *TMP_w = to_WCHAR(URL_file);
	  swprintf(TMP_c, L"Can't download:\n%s", TMP_w);
	  free(TMP_w);
	  Info_Message_t(TMP_c, 1);
	}
	r_List = (t_list*)r_List->next;
  }
  while (l_List) {
    free(l_List->filename);
	l_List = (t_list*)l_List->prev;
    if (l_List && l_List->next) free(l_List->next);
  }
  if (List) free(List);
//  printf("Check Finished !\n");
  Info_Message(L"Check Finished !", 0);
}

int update_launcher()
{
  sTat = 3;
  char *ver = (char*)malloc(strlen(Update_Link)+1);
  strcpy(ver, Update_Link);
  strcpy(&ver[strlen(ver)-3], "ver");
  char *ram = (char*)malloc(+1);
  ram[0] = 0;
  if (cURL_to_(ver, &ram, NULL)) {
	free(ram);
	sTat = 0;
	return 0;
  }
  char *p = strchr(ram, '\n');
  if (p) p[0] = 0;
  p = strchr(ram, '\r');
  if (p) p[0] = 0;
  if (!strcmp(L_VERSION, ram)) {
	free(ram);
	sTat = 0;
	return 0;
  }
  free(ram);
  unlink(strrchr(Update_Link, '/')+1);
  if (cURL_to_(Update_Link, NULL, strrchr(Update_Link, '/')+1)) {
	unlink(strrchr(Update_Link, '/')+1);
	sTat = 0;
	return 0;
  }
  char tmp1[strlen(Exe_Name)+5+1], tmp2[strlen(Exe_Name)+5+1];
  sprintf(tmp1, "%s.del1", Exe_Name);
  sprintf(tmp2, "%s.del2", Exe_Name);
  rename(Exe_Name, tmp1);
  unlink("Elin_Launcher.exe");
  l_unzip(strrchr(Update_Link, '/')+1);
  unlink(strrchr(Update_Link, '/')+1);
  if (!file_exist("Elin_Launcher.exe")) {
	rename(tmp1, Exe_Name);
	return 0;
  }
  rename("Elin_Launcher.exe", tmp2);
  const char *ExeTmp[3] = {tmp2, "--upd", Exe_Name};
  sTat = 0;
  return Set_URL_Param(ExeTmp);
}
