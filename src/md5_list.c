#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#ifdef WIN32
  #include <windows.h>
  #include <windowsx.h>
  #include <Winreg.h>
#else
  #include <unistd.h>
#endif

#include "globale.h"

void sub_md5_list(const char *sc_dir, FILE *fp)
{
  DIR *pdir;
  struct dirent *dirp;

  if ((pdir = opendir(sc_dir)) != NULL) {
	while ((dirp = readdir(pdir)) != NULL) {
	  if ((strcmp(dirp->d_name, ".")) && (strcmp(dirp->d_name, ".."))) {
		char File1[500];
		sprintf(File1, "%s/%s", sc_dir, dirp->d_name);
		if (dir_exist(File1)) sub_md5_list(File1, fp);
		else {
		  char *t_md5 = l_md5(File1);
		  size_t t_size = file_size(File1);
		  fprintf(fp, "%s\t%i\t%s/%s\n", t_md5, t_size, sc_dir, dirp->d_name);
		  if (t_md5) free(t_md5);
		}
	  }
	}
	closedir(pdir);
  }
}

void md5_list()
{
  FILE *fp = fopen("MD5.TXT", "w");
  if (!fp) return;

  sub_md5_list(".", fp);

  fclose(fp);
}
