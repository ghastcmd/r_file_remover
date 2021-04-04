#include "pch.h"

#ifdef _WIN32
typedef HANDLE files_dir_t;
typedef WIN32_FIND_DATA files_folder_t;

#else
typedef DIR* files_dir_t;
typedef struct dirent* files_folder_t;

#endif

void files_opendir(const char* path, files_dir_t* hnd)
{
#ifdef _WIN32
    char s_buff[MAX_PATH];
    strcpy(s_buff, path);
    strcat(s_buff, "\\*");

    files_folder_t ffd;
    *hnd = FindFirstFileA(s_buff, &ffd);
    FindNextFileA(*hnd, &ffd);
#else
    *hnd = opendir(path);
    readdir(*hnd);
    readdir(*hnd);  
#endif
}

bool files_nextfile(files_dir_t hnd, files_folder_t* ffd)
{
#ifdef _WIN32
    return FindNextFileA(hnd, ffd);
#else
    *ffd = readdir(hnd);
    return (bool)*ffd;
#endif
}

void files_close(files_dir_t dir)
{
#ifdef _WIN32
    FindClose(dir);
#else
    closedir(dir);
#endif
}

char* files_getname(files_folder_t* ffd)
{
#ifdef _WIN32
    return ffd->cFileName;
#else
    return (*ffd)->d_name;
#endif
}

bool files_find_file(const char* path)
{
#ifdef _WIN32
    files_folder_t ffd;
    HANDLE ret = FindFirstFileA(path, &ffd);
    return ret == INVALID_HANDLE_VALUE ? 0 : 1;
#else
    return (bool)opendir(path);
#endif
}

void files_list_names(char* path)
{
    files_dir_t dir;
    files_opendir(".", &dir);
    files_folder_t ffd;
    while(files_nextfile(dir, &ffd))
    {
        puts(files_getname(&ffd));
    }
    files_close(dir);
}

void files_list_current()
{
    files_list_names(".");
}

#ifdef _WIN32
bool files_is_folder(files_folder_t* ffd)
#else
bool files_is_folder(const char* path)
#endif
{
#ifdef _WIN32
    return ffd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
#else
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISDIR(path_stat.st_mode);
#endif
}

void files_reset_permissions(char* path)
{
#ifdef _WIN32
    SetFileAttributesA(path, FILE_ATTRIBUTE_NORMAL);
#else
    chmod(path, 0777);
#endif
}

void files_rmdir_r(char* path);

#ifdef _WIN32
static void files_rm_obj(char* path, files_folder_t* ffd)
#define _ARG_FILES_RM_OBJ_ ffd
#else
static void files_rm_obj(char* path)
#define files_rm_obj(path, ffd) files_rm_obj(path)
#define _ARG_FILES_RM_OBJ_ path
#endif
{
    if (files_is_folder(_ARG_FILES_RM_OBJ_))
    {
        files_rmdir_r(path);
    }
    else
    {
        if (remove(path) != 0)
        {
            files_reset_permissions(path);
            remove(path);
        }
    }
}

static inline int path_name_idx(char* path)
{
    int i;
    for (i = strlen(path); path[i] != '\\' && path[i] != '/' && i >= 0; i--);
    return i;
}

char* path_name(char* path)
{
    int i;
    for (i = strlen(path); path[i] != '\\' && path[i] != '/' && i >= 0; i--);
    return &path[i + 1];
}

static char* path_split(char** path)
{
    int i = path_name_idx(*path);
    (*path)[i] = '\0';
    return &((*path)[i+1]);
}

void files_rm(char* path)
{
    char* buff = path_split(&path);
    if (path != buff)
    {
        chdir(path);
    }
    path = buff;
#ifdef _WIN32
    files_folder_t ffd;
    FindFirstFileA(path, &ffd);
    files_rm_obj(path, &ffd);
#else
    files_rm_obj(path, NULL);
#endif
}

void files_rmdir_r(char* path)
{    
    files_dir_t dir;
    files_opendir(path, &dir);
    
    chdir(path);

    files_folder_t ffd;
    while (files_nextfile(dir, &ffd))
    {
        register char* ffd_name = files_getname(&ffd);
        files_rm_obj(ffd_name, &ffd);
    }

    files_close(dir);
    chdir("..");
    rmdir(path);
}

void usage(char** argv);

int main(int argc, char* argv[])
{
    if (argc <= 1)
    {
        fprintf(stderr, "Insuficient arguments.\n");
        usage(argv);
        exit(0);
    }

    for (int i = 0; i < argc; i++)
    {
        puts(argv[i]);
    }

    return 0;

    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            continue;
        }
        // takes pointer to last char in current argv
        char* loc = &argv[i][strlen(argv[i]) - 1];
        if (*loc == '\\' || *loc == '/')
        {
            *loc = '\0';
        }

        files_rm(argv[i]);
    }
}

void usage(char** argv)
{
    printf("Usage:\n\t%s [file to remove]\n", path_name(argv[0]));
    puts("\tArguments starting with '-' will be ignored");
    puts("\tAll the exclusions are forced/recursive");
}