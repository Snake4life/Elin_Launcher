#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <curl/curl.h>

#ifdef WIN32 
#include <io.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#endif

#include "globale.h"

const int Time_UP = 10;

long long B_size, B_pos;


int dir_exist(const char *dirname)
{
  struct stat   buffer;   
  if( stat( dirname, &buffer ) != 0 )
    return 0;
  else if( buffer.st_mode & S_IFDIR )  // S_ISDIR() doesn't exist on my windows 
    return 1;
  return 0;
}
int file_exist(const char *filename)
{
  struct stat   buffer;   
  return (stat (filename, &buffer) == 0);
}
size_t file_size(const char *filename)
{
  struct stat   buffer;   
  stat (filename, &buffer);
  return buffer.st_size;
}
void l_mkdir(const char* dirname)
{
#ifdef WIN32
  mkdir(dirname);
#else
  mkdir(dirname, 0777);
#endif

}

void set_B_size_reset()
{
  B_pos = 0;
  B_size = 0;
}

void set_add_B_pos(long long l_tmp)
{
  B_pos += l_tmp;
}

void set_B_size(long long l_tmp)
{
  B_size = l_tmp;
}

typedef struct t_c_curl {
  long TimeOut;
  long long pos;
  FILE *fp;
  char **ram;
} t_c_curl;


int cURL_init()
{
  curl_global_init(CURL_GLOBAL_ALL);
#ifdef HAVE_CURL_SSL
  curl_global_init(CURL_GLOBAL_SSL);
#endif
#ifdef WIN32 
  curl_global_init(CURL_GLOBAL_WIN32);
#endif
  return 1;
}

int cURL_cleanup()
{
  curl_global_cleanup();
  return 1;
}

void display_progress(long pos_count, double sp_pos_tmp)
{
  printf("   %i%% (%.02f/%.02f) ~ %i ko/s\r", (int)((B_pos + pos_count) * 100 / B_size), ((double)B_pos + pos_count) / 1048576, (double)B_size / 1048576, (int)sp_pos_tmp/1024);
  Info_Progress(((double)B_pos + pos_count) / 1048576, (double)B_size / 1048576);
}

static size_t cURL_write_callback(void *ptr, size_t size, size_t nmemb, void **stream)
{
  CURL *curl = stream[0];
  t_c_curl *c_curl = (t_c_curl*)stream[1];

  long long init_pos = c_curl->pos;
  c_curl->pos += size*nmemb;
    double l_size;
    curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &l_size);
    long long m_size = l_size;
  double sp_pos_tmp;
  curl_easy_getinfo(curl, CURLINFO_SPEED_DOWNLOAD, &sp_pos_tmp);
  if (!B_size) {
	if (m_size > 0) {
	  printf("Downloading : %ld%% --- %.00f ko/s\r", (long)(c_curl->pos * 100 / m_size), sp_pos_tmp/1024);
	  Info_Progress((double)c_curl->pos / 1048576, (double)m_size / 1048576);
	}
  } else display_progress(c_curl->pos, sp_pos_tmp);
  fflush(stdout);

  if (c_curl->ram) {
    *c_curl->ram = (char*)realloc(*c_curl->ram, init_pos + size*nmemb + 1);
    memcpy(*c_curl->ram + init_pos, ptr, size*nmemb);

    curl_easy_setopt(curl, CURLOPT_TIMEOUT, (long)time(NULL)-c_curl->TimeOut+Time_UP);
    return size*nmemb;
  }

  curl_easy_setopt(curl, CURLOPT_TIMEOUT, (long)time(NULL)-c_curl->TimeOut+Time_UP);
  return fwrite(ptr, size, nmemb, (FILE*)c_curl->fp);
}

int cURL_to_(const char *URL, char **ram, const char *outFile)
{
  CURL *curl = curl_easy_init();
  if (!curl) return -1;
  t_c_curl *c_curl = (t_c_curl*)malloc(sizeof(t_c_curl));

//  DEBUG("\tcURL: \"%s\"\n", URL);

  c_curl->pos = 0;
  c_curl->ram = ram;
  c_curl->fp = NULL;
  if (c_curl->ram == NULL) {
    char tmpDir[strlen(outFile)];
	strcpy(tmpDir, outFile);
	char *P=tmpDir;
    while (P) {
	  P = strstr(P+1, "/");
	  if (P) {
	    char tmpDD[P-tmpDir+1];
		memset(tmpDD, 0, P-tmpDir+1);
	    strncpy(tmpDD, tmpDir, P-tmpDir);
		if (!dir_exist(tmpDD)) l_mkdir(tmpDD);
	  }
	}
    c_curl->fp = fopen(outFile, "wb");
	if (!c_curl->fp) {
		wchar_t msgTMP[2048];
		wchar_t *WmsgTMP = to_WCHAR(outFile);
#ifdef WIN32
		swprintf(msgTMP, L"Can't open output file... %s", WmsgTMP);
#else
		swprintf(msgTMP, 2048, L"Can't open output file... %s", WmsgTMP);
#endif
		free(WmsgTMP);
		if (file_exist(outFile)) {
			unlink(outFile);
		if (file_exist(outFile))
			c_curl->fp = fopen(outFile, "wb");
			if (!c_curl->fp) {
				Info_Message(msgTMP, 1);
				wprintf(L"%s\n", msgTMP);
				return -1;
			}
		} else {
			Info_Message(msgTMP, 1);
			wprintf(L"%s\n", msgTMP);
			return -1;
		}
	}
	printf("Downloading:: %s          \n", outFile);
  }

  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, (void *)cURL_write_callback);
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
  curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, Time_UP);

  c_curl->TimeOut = time(NULL);
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, (long)time(NULL)-c_curl->TimeOut+Time_UP);

#ifndef WIN32 
  curl_easy_setopt(curl, CURLOPT_NOSIGNAL , 1);
#endif

  void *Contener[2] = {curl, c_curl};
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)Contener);
  curl_easy_setopt(curl, CURLOPT_URL, URL);

  int ret = curl_easy_perform(curl);

  if (!B_size) printf("\n");
  if (c_curl->fp) fclose(c_curl->fp);
  if (c_curl->ram) {
	*c_curl->ram = (char*)realloc(*c_curl->ram, c_curl->pos + sizeof(char));
	memset(*c_curl->ram + c_curl->pos, 0, 1);
	c_curl->pos += sizeof(char);
  }

  set_add_B_pos(c_curl->pos);

  if (!ret) {
    int val = ret;
	curl_easy_getinfo(curl, CURLINFO_HTTP_CODE, &val);
	if (val != 200) ret = -1;
  }

  free(c_curl);
  curl_easy_cleanup(curl);

  return ret;
}

int cURL_ping(const char *URL)
{
  CURL *curl = curl_easy_init();
  if (!curl) return -1;

//  DEBUG("\tcURL ping: \"%s\"\n", URL);

  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
  curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, Time_UP);

  curl_easy_setopt(curl, CURLOPT_TIMEOUT, (long)time(NULL)+Time_UP);

#ifndef WIN32 
  curl_easy_setopt(curl, CURLOPT_NOSIGNAL , 1);
#endif

  curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
  curl_easy_setopt(curl, CURLOPT_URL, URL);

  int ret = curl_easy_perform(curl);
  if (!ret) {
	curl_easy_getinfo(curl, CURLINFO_HTTP_CODE, &ret);
	if (ret != 200) ret = 1;
	else ret = 0;
  } else ret = 1;

  curl_easy_cleanup(curl);

  return ret;
}
