#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#ifdef _WIN32
#include <Windows.h>
#include <conio.h>

#include <direct.h>
#include <AccCtrl.h>
#include <AclAPI.h>

#else
#include <sys/ioctl.h>
#include <sys/stat.h>

#include <unistd.h>
#include <pthread.h>
#include <termios.h>
#include <dirent.h>

#endif