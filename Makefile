# This Makefile will build the MinGW Win32 application.

L_VERSION = 1.2.0

HEADERS = res/resource.h
OBJS =	obj/main.o obj/curl.o obj/md5.o obj/md5pass.o obj/wchar.o obj/update.o obj/crypt.o obj/unzip.o obj/md5_list.o
OBJS_L = ${OBJS} obj/console.o
OBJS_W = ${OBJS}
OBJS_S = ${OBJS_W} obj/resource.o obj/f_exe.o obj/f_image.o obj/p_click.o obj/winmain.o obj/p_winmain.o obj/f_winmain.o obj/f_info.o
INCLUDE_DIRS = -I. -I./lib/include -I./res
EXT = 
WORK = 

#MACHDEP = -m64

CC = g++
#gcc
C+ = g++
RAR = rar
RC = windres
BIN2H = bin2h
WARNS = -Wall
CFLAGS = -g -O3 ${WARNS} $(MACHDEP) -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -DSILENT -DL_VERSION=\"$(L_VERSION)\"
LDFLAGS_C = $(MACHDEP) -lcurl -lunzip -lz
ifeq ($(OS), Windows_NT)
	BIN2H := tools/${BIN2H}.exe
	RAR := tools/rar.exe
	CFLAGS += -DCURL_STATICLIB -D _WIN32_IE=0x0501 -D WINVER=0x600 -D _WIN32_WINNT=0x600 -D UNICODE -D _UNICODE
	LDFLAGS_C += -s -L./lib/win32 -L/usr/lib -lws2_32 -static-libgcc -static-libstdc++ -lcomctl32 -lgdiplus -lole32 -lmsimg32 -lpsapi
	LDFLAGS += $(LDFLAGS_C) -mwindows -Wl,--subsystem,windows
	EXT = .exe
	DLL = .dll
	BIN_PATH = /bin/
	WORK = Elin_Launcher
	OBJS_S := $(patsubst obj%, obj/win32%, $(OBJS_S))
	OBJS_D := $(patsubst obj%, obj/win32%, $(OBJS_D))
else
	BIN2H := tools/${BIN2H}
	CFLAGS += -fPIC
	LDFLAGS += $(LDFLAGS_C) -s -L./lib/unix -lssl -lcrypto -lldap -lrt -lgnutls -pthread
	OBJS_L := $(patsubst obj%, obj/unix%, $(OBJS_L))
	DLL = .so
	WORK = lPSO2_Launcher
	
endif

Virg = ,
R_VERSION = $(subst .,$(Virg),$(L_VERSION))
RAW_LST = $(wildcard raw/*.dll) $(wildcard raw/*.txt) $(wildcard raw/*.pso2) $(wildcard raw/*.exe) $(wildcard raw/*.ttf)
HEADERS := $(RAW_LST)
HEADERS := $(HEADERS:.dll=.h)
HEADERS := $(HEADERS:.txt=.h)
HEADERS := $(HEADERS:.pso2=.h)
HEADERS := $(HEADERS:.exe=.h)
HEADERS := $(HEADERS:.ttf=.h)
Tools_LST = $(wildcard tools/*)
Tools_LST := $(Tools_LST:tools/ftp.txt=)
Tools_LST := $(Tools_LST:tools/ftp_login.txt=)

all: ${WORK}



lElin_Launcher: ${OBJS_L}
	@echo $(notdir $@)
	@rm -f PSO2_Launcher_src.tar.bz2
	@${C+} -o "$@"${EXT} ${OBJS_L} ${LDFLAGS}

Elin_Launcher:  ${OBJS_S}
	@echo $(notdir $@)
	@rm -f PSO2_Launcher_src.tar.bz2
	@${C+} -o "$@"${EXT} ${OBJS_S} ${LDFLAGS}

clean:
#	@make -s -C src/win32/plugins clean
	@rm -f src/{win32,unix}/raw.c obj/{win32,unix}/*.o obj/{win32,unix}/lang/*.o obj/{win32,unix}/*.d raw/*.h include/raw.h Elin_Launcher.exe

obj/win32/%.o: src/%.c
	@echo $(notdir $@)
	@${CC} ${CFLAGS} ${INCLUDE_DIRS} -c $< -o $@
obj/unix/%.o: src/%.c
	@echo $(notdir $@)
	@${CC} ${CFLAGS} ${INCLUDE_DIRS} -c $< -o $@

obj/win32/%.o: src/%.cpp
	@echo $(notdir $@)
	@${C+} ${CFLAGS} ${INCLUDE_DIRS} -c $< -o $@
obj/unix/%.o: src/%.cpp
	@echo $(notdir $@)
	@${C+} ${CFLAGS} ${INCLUDE_DIRS} -c $< -o $@

obj/win32/%.o: src/sys/%.c
	@echo $(notdir $@)
	@${CC} -MMD -MP -MF obj/win32/$*.d ${CFLAGS} ${INCLUDE_DIRS} -c $< -o $@

obj/unix/%.o: src/sys/%.c
	@echo $(notdir $@)
	@${CC} -MMD -MP -MF obj/unix/$*.d ${CFLAGS} ${INCLUDE_DIRS} -c $< -o $@

obj/win32/%.o: src/sys/%.cpp
	@echo $(notdir $@)
	@${C+} -MMD -MP -MF obj/win32/$*.d ${CFLAGS} ${INCLUDE_DIRS} -c $< -o $@

obj/unix/%.o: src/sys/%.cpp
	@echo $(notdir $@)
	@${C+} -MMD -MP -MF obj/unix/$*.d ${CFLAGS} ${INCLUDE_DIRS} -c $< -o $@

obj/win32/%.o: src/win32/%.c
	@echo $(notdir $@)
	@${CC} ${CFLAGS} ${INCLUDE_DIRS} -c $< -o $@

obj/win32/%.o: src/win32/%.cpp
	@echo $(notdir $@)
	@${C+} ${CFLAGS} ${INCLUDE_DIRS} -c $< -o $@

obj/win32/resource.o: res/resource.rc res/Application.manifest res/resource.h
	@echo $(notdir $@)
	@${RC} -I./include -I./res -DR_VERSION=$(R_VERSION) -DL_VERSION=\\\"$(L_VERSION)\\\" -DRX_VERSION=$(R_VERSION) -DLX_VERSION=\\\"$(L_VERSION)\\\" -i $< -o $@
