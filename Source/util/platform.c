#include "platform.h"

/*
 * DISCLAIMER:
 * This file provides an endpoint to access cross-platform filesystem calls.
 * Given the care that I take in making cross platform code (basically none),
 * the majority of the file was generated using ChatGPT. As such, the code 
 * provided here is semi-temporary. What may (and probably will) change:
 *  - Function docstrings
 *  - Function signatures (behavior should stay mostly identical)
 *  - Error logging and errno setting
 * 
 * Basically everything after system reflection functions is volatile :)
 */



#include "log.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <stdio.h>  // for snprintf

// --- Platform dependent inclusions ---
#if defined(_WIN32)
    #include <windows.h>
    #include <direct.h>
    #include <tchar.h>

    #define mkdir_win(p) _mkdir(p)
    #define PATH_SEPARATOR '\\'

    #ifndef MAX_PATH_SIZE
        #define MAX_PATH_SIZE 4096
    #endif

#else
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <unistd.h>
    #include <limits.h>   // PATH_MAX (if available)
    #include <dirent.h>

    #define mkdir_posix(p) mkdir(p, 0755)
    #define PATH_SEPARATOR '/'

    #ifndef MAX_PATH_SIZE
        #if defined(PATH_MAX)
            #define MAX_PATH_SIZE PATH_MAX
        #else
            #define MAX_PATH_SIZE 4096
        #endif
    #endif
#endif



// ==== Compile-time evaluation ====
// --- Arch ---
#if defined(__x86_64__) || defined(_M_X64)
    #define HOST_ARCH_NAME "x86_64"
    #define HOST_ARCH_ID   PLATFORM_ARCH_X64
#elif defined(__i386__) || defined(_M_IX86)
    #define HOST_ARCH_NAME "x86"
    #define HOST_ARCH_ID   PLATFORM_ARCH_X86
#elif defined(__aarch64__) || defined(_M_ARM64)
    #define HOST_ARCH_NAME "aarch64"
    #define HOST_ARCH_ID   PLATFORM_ARCH_ARM64
#elif defined(__arm__) || defined(_M_ARM)
    #define HOST_ARCH_NAME "arm"
    #define HOST_ARCH_ID   PLATFORM_ARCH_ARM
#elif defined(__riscv) && __riscv_xlen == 64
    #define HOST_ARCH_NAME "riscv64"
    #define HOST_ARCH_ID   PLATFORM_ARCH_RISCV64
#elif defined(__powerpc64__) || defined(__ppc64__)
    #define HOST_ARCH_NAME "ppc64"
    #define HOST_ARCH_ID   PLATFORM_ARCH_PPC64
#elif defined(__powerpc__) || defined(__ppc__)
    #define HOST_ARCH_NAME "ppc"
    #define HOST_ARCH_ID   PLATFORM_ARCH_PPC
#elif defined(__mips64)
    #define HOST_ARCH_NAME "mips64"
    #define HOST_ARCH_ID   PLATFORM_ARCH_MIPS64
#elif defined(__mips__)
    #define HOST_ARCH_NAME "mips"
    #define HOST_ARCH_ID   PLATFORM_ARCH_MIPS
#else
    #define HOST_ARCH_NAME "unknown"
    #define HOST_ARCH_ID   PLATFORM_ARCH_UNKNOWN
#endif


// --- Vendor ---
#if defined(__APPLE__)
    #define HOST_VENDOR_NAME "apple"
    #define HOST_VENDOR_ID   PLATFORM_VENDOR_APPLE
#elif defined(_WIN32)
    #define HOST_VENDOR_NAME "w64"
    #define HOST_VENDOR_ID   PLATFORM_VENDOR_W64
#elif defined(__linux__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)
    #define HOST_VENDOR_NAME "pc"
    #define HOST_VENDOR_ID   PLATFORM_VENDOR_PC
#elif defined(__sun)
    #define HOST_VENDOR_NAME "oracle"
    #define HOST_VENDOR_ID   PLATFORM_VENDOR_ORACLE
#elif defined(_AIX)
    #define HOST_VENDOR_NAME "ibm"
    #define HOST_VENDOR_ID   PLATFORM_VENDOR_IBM
#elif defined(__hpux)
    #define HOST_VENDOR_NAME "hp"
    #define HOST_VENDOR_ID   PLATFORM_VENDOR_HP
#else
    #define HOST_VENDOR_NAME "unknown"
    #define HOST_VENDOR_ID   PLATFORM_VENDOR_UNKNOWN
#endif


// --- OS ---
#if defined(_WIN32) || defined(_WIN64)
    #define HOST_OS_NAME "windows"
    #define HOST_OS_ID   PLATFORM_OS_WINDOWS
#elif defined(__APPLE__) && defined(__MACH__)
    #define HOST_OS_NAME "darwin"
    #define HOST_OS_ID   PLATFORM_OS_MACOS
#elif defined(__linux__)
    #define HOST_OS_NAME "linux"
    #define HOST_OS_ID   PLATFORM_OS_LINUX
#elif defined(__FreeBSD__)
    #define HOST_OS_NAME "freebsd"
    #define HOST_OS_ID   PLATFORM_OS_FREEBSD
#elif defined(__NetBSD__)
    #define HOST_OS_NAME "netbsd"
    #define HOST_OS_ID   PLATFORM_OS_NETBSD
#elif defined(__OpenBSD__)
    #define HOST_OS_NAME "openbsd"
    #define HOST_OS_ID   PLATFORM_OS_OPENBSD
#elif defined(__DragonFly__)
    #define HOST_OS_NAME "dragonfly"
    #define HOST_OS_ID   PLATFORM_OS_DRAGONFLYBSD
#elif defined(__sun)
    #define HOST_OS_NAME "solaris"
    #define HOST_OS_ID   PLATFORM_OS_SOLARIS
#elif defined(_AIX)
    #define HOST_OS_NAME "aix"
    #define HOST_OS_ID   PLATFORM_OS_AIX
#elif defined(__hpux)
    #define HOST_OS_NAME "hpux"
    #define HOST_OS_ID   PLATFORM_OS_HPUX
#elif defined(__unix__) || defined(__unix)
    #define HOST_OS_NAME "unix"
    #define HOST_OS_ID   PLATFORM_OS_UNIX_GENERIC
#else
    #define HOST_OS_NAME "unknown"
    #define HOST_OS_ID   PLATFORM_OS_UNKNOWN
#endif


// --- ABI ---
#if defined(_WIN32)
    #define HOST_ABI_NAME "msvc"
    #define HOST_ABI_ID   PLATFORM_ABI_MSVC
#elif defined(__GLIBC__)
    #define HOST_ABI_NAME "gnu"
    #define HOST_ABI_ID   PLATFORM_ABI_GLIBC
#elif defined(__MUSL__)
    #define HOST_ABI_NAME "musl"
    #define HOST_ABI_ID   PLATFORM_ABI_MUSL
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)
    #define HOST_ABI_NAME "bsd"
    #define HOST_ABI_ID   PLATFORM_ABI_BSD
#elif defined(__sun)
    #define HOST_ABI_NAME "solaris"
    #define HOST_ABI_ID   PLATFORM_ABI_SOLARIS
#elif defined(_AIX)
    #define HOST_ABI_NAME "aix"
    #define HOST_ABI_ID   PLATFORM_ABI_AIX
#elif defined(__hpux)
    #define HOST_ABI_NAME "hpux"
    #define HOST_ABI_ID   PLATFORM_ABI_HPUX
#else
    #define HOST_ABI_NAME "unknown"
    #define HOST_ABI_ID   PLATFORM_ABI_UNKNOWN
#endif


#define HOST_GROUP_NAME \
    HOST_ARCH_NAME "-" HOST_VENDOR_NAME "-" HOST_OS_NAME "-" HOST_ABI_NAME


// ==== System reflection functions ====

// --- Build System reflection

hostArch get_host_arch(void) 
{
    return HOST_ARCH_ID;
}
const char* get_host_arch_name(void) 
{
    return HOST_ARCH_NAME;
}

hostVendor get_host_vendor(void) 
{
    return HOST_VENDOR_ID;
}
const char* get_host_vendor_name(void) 
{
    return HOST_VENDOR_NAME;
}

hostAbi get_host_abi(void) 
{
    return HOST_ABI_ID;
}
const char* get_host_abi_name(void) 
{
    return HOST_ABI_NAME;
}

hostOS get_host_os(void)
{
    return HOST_OS_ID;
}
const char* get_host_os_name(void) 
{
    return HOST_OS_NAME;
}

/*
 * Return a group string with format:
 *  <arch>-<vendor>-<os>-<abi>
*/
const char* get_host_group_name(void) 
{
    return HOST_GROUP_NAME;
}


bool host_is_windows(void)
{
#if defined(_WIN32) || defined(_WIN64)
    return true;
#else
    return false;
#endif
}

bool host_is_posix(void)
{
#if defined(__unix__) || defined(__unix) || \
    defined(__APPLE__) || defined(__MACH__) || \
    defined(__linux__) || defined(__FreeBSD__) || defined(__NetBSD__) || \
    defined(__OpenBSD__) || defined(__DragonFly__) || defined(__sun) || \
    defined(_AIX) || defined(__hpux)
    return true;
#else
    return false;
#endif
}

bool host_is_bsd(void)
{
#if defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)
    return true;
#else
    return false;
#endif
}



// ==== Internal helpers ====

//* Normalize path by modifying string.
static bool normalize_path(char* path)
{
    size_t norm_index = 0;

    char prevChar = '\0';

    for(size_t char_index = 0; path[char_index]; char_index++) 
    {
        char c = path[char_index];
        if(c == '/')
        {
            if(prevChar==PATH_SEPARATOR) continue; // collapse duplicate seperators
            if(host_is_windows()) c = PATH_SEPARATOR;
        } 

        path[norm_index++] = c;
        prevChar = c;
    }

    // Remove trailing separator (except for root)
    if(norm_index>1 && prevChar==PATH_SEPARATOR) norm_index--;
    path[norm_index] =  '\0';
    return true;
}

//* Standardize path by modifiying string
static bool standardize_path(char* path)
{
    size_t std_index = 0;

    // const char sep = host_is_windows() ? '\\' : '/';
    char prevChar = '\0';

    for(size_t char_index = 0; path[char_index]; char_index++) 
    {
        char c = path[char_index];
        if(prevChar=='/' && c==PATH_SEPARATOR) continue;
        if(c == '\\') c = '/';  // only standardize the non-standard '\\'

        path[std_index++] = c;
        prevChar = c;
    }

    // Remove trailing separator (except for root)
    if(std_index>1 && prevChar==PATH_SEPARATOR) std_index--;
    path[std_index] =  '\0';
    return true;
}

// static const char* resolve_full_path(const char* relative_path, char* absolute_path, size_t max_size)
// {
//     return "";
// }

#if defined(_WIN32)
static uint64_t get_std_filetime(const FILETIME* ft)
{
    ULARGE_INTEGER ull;
    ull.LowPart = ft->dwLowDateTime;
    ull.HighPart = ft->dwHighDateTime;
    return (ull.QuadPart / 10000000ULL) - 11644473600ULL; // Windows -> Unix epoch
}
#else
static uint64_t get_std_filetime(time_t t)
{
    return (uint64_t)t; // POSIX already in seconds
}
#endif

// ==== Interface ====

const char *get_cwd(void)
{
    static char buf[MAX_PATH_SIZE];

#if defined(_WIN32)
    // _getcwd sets errno on failure
    if (_getcwd(buf, (int)sizeof(buf)) == NULL) return NULL;
#else
    // getcwd sets errno on failure (e.g. ERANGE if buffer too small)
    if (getcwd(buf, sizeof(buf)) == NULL) return NULL;
#endif

    standardize_path(buf);
    return buf;
}

bool set_cwd(const char *path)
{
    if (!path || !*path) return false;

    char* norm_path = (char*)malloc((strlen(path)+1)*sizeof(char));
    if(norm_path) log_l("set_cwd: Could not allocate required memory. Attempting without path normalization.", CRITICAL);
    else
    {
        strncpy(norm_path, path, strlen(path)+1);
        normalize_path(norm_path);
        path = norm_path;
    }

    int chdir_res = false;
#if defined(_WIN32)
    chdir_res = !_chdir(path);   // _chdir sets errno
#else
    chdir_res = !chdir(path);    // chdir sets errno
#endif

    if(norm_path) free(norm_path);
    return chdir_res;
}

/*
 * Modifications to be made: if the file or directory does not exist, stat returns false.
 * This was not originally intended, but maybe it should be.
 */
bool stat_path(const char* path, fileStat* out)
{
    if (!path || !out) return false;

#if defined(_WIN32)
    WIN32_FILE_ATTRIBUTE_DATA data;
    if (!GetFileAttributesExA(path, GetFileExInfoStandard, &data)) {
        out->exists = false;
        out->type = FILE_TYPE_NONE;
        out->mtime = 0;
        return false;
    }

    out->exists = true;
    out->type = (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? FILE_TYPE_DIR : FILE_TYPE_FILE;
    out->mtime = get_std_filetime(&data.ftLastWriteTime);

#else
    struct stat st, target;

    if (lstat(path, &st) != 0) {
        out->exists = false;
        out->type = FILE_TYPE_NONE;
        out->mtime = 0;
        return false;
    }

    out->exists = true;

    if (S_ISLNK(st.st_mode)) {
        // symlink: follow target
        if (stat(path, &target) == 0) {
            if (S_ISDIR(target.st_mode))
                out->type = FILE_TYPE_DIR;
            else if (S_ISREG(target.st_mode))
                out->type = FILE_TYPE_FILE;
            else
                out->type = FILE_TYPE_NONE;
            out->mtime = get_std_filetime(target.st_mtime);
        } else {
            // dangling symlink
            out->type = FILE_TYPE_NONE;
            out->mtime = 0;
        }
    } else {
        if (S_ISDIR(st.st_mode))
            out->type = FILE_TYPE_DIR;
        else if (S_ISREG(st.st_mode))
            out->type = FILE_TYPE_FILE;
        else
            out->type = FILE_TYPE_NONE;
        out->mtime = get_std_filetime(st.st_mtime);
    }
#endif

    return true;
}

bool create_dir(const char* path)
{
    if (!path || !*path) {
        errno = EINVAL;
        return false;
    }

    fileStat st;
    if (stat_path(path, &st)) {
        if (st.type == FILE_TYPE_DIR) return true; // already exists
        if (st.exists) {
            errno = ENOTDIR;
            return false; // exists but not a directory
        }
    }

    // Recursively create parent
    char parent[MAX_PATH_SIZE];
    strncpy(parent, path, sizeof(parent));
    parent[sizeof(parent)-1] = '\0';

    // Remove trailing slash
    size_t len = strlen(parent);
    while (len > 0 && (parent[len-1] == '/' || parent[len-1] == '\\')) {
        parent[len-1] = '\0';
        len--;
    }

    // Find last path separator
    char* last_sep = strrchr(parent, '/');
#if defined(_WIN32)
    char* last_sep_win = strrchr(parent, '\\');
    if (!last_sep || (last_sep_win && last_sep_win > last_sep)) last_sep = last_sep_win;
#endif

    if (last_sep) {
        *last_sep = '\0';
        if (strlen(parent) > 0 && !create_dir(parent))
            return false; // failed to create parent
    }

    // Create directory itself
#if defined(_WIN32)
    if (_mkdir(path) != 0 && errno != EEXIST) return false;
#else
    if (mkdir(path, 0755) != 0 && errno != EEXIST) return false;
#endif

    return true;
}

bool join_path(char* source, const char* target, const size_t max_source_size)
{
    if (!source || !target || max_source_size == 0) return false;

    size_t len_source = strlen(source);
    size_t len_target = strlen(target);

    if (len_source + len_target + 2 > max_source_size) {
        // +2: one for possible separator, one for null terminator
        return false;
    }

    // Add separator if needed
    if (len_source > 0 && source[len_source - 1] != '/' && source[len_source - 1] != '\\') {
        source[len_source] = PATH_SEPARATOR;
        len_source++;
        source[len_source] = '\0';
    }

    // Skip leading separator in target to avoid duplicate
    if (target[0] == '/' || target[0] == '\\') target++;

    // Concatenate
    strncat(source, target, max_source_size - len_source - 1);
    return true;
}

bool list_path(const char* path, fileType filter, char*** out_entries, size_t* out_count)
{
    if (!path || !out_entries || !out_count) return false;
    *out_entries = NULL;
    *out_count = 0;

#if defined(_WIN32)
    char search_path[MAX_PATH_SIZE];
    snprintf(search_path, sizeof(search_path), "%s\\*", path);

    WIN32_FIND_DATAA fd;
    HANDLE h = FindFirstFileA(search_path, &fd);
    if (h == INVALID_HANDLE_VALUE) return false;

    size_t capacity = 16;
    size_t count = 0;
    char** entries = (char**)malloc(capacity * sizeof(char*));
    if (!entries) { FindClose(h); return false; }

    do {
        const char* name = fd.cFileName;
        if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) continue;

        fileType type = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? FILE_TYPE_DIR : FILE_TYPE_FILE;
        if (filter != FILE_TYPE_NONE && type != filter) continue;

        if (count >= capacity) {
            capacity *= 2;
            char** tmp = (char**)realloc(entries, capacity * sizeof(char*));
            if (!tmp) { // cleanup
                for (size_t i = 0; i < count; i++) free(entries[i]);
                free(entries);
                FindClose(h);
                return false;
            }
            entries = tmp;
        }

        entries[count] = _strdup(name);
        if (!entries[count]) continue;
        count++;
    } while (FindNextFileA(h, &fd));

    FindClose(h);
    *out_entries = entries;
    *out_count = count;
    return true;

#else
    DIR* dir = opendir(path);
    if (!dir) return false;

    size_t capacity = 16;
    size_t count = 0;
    char** entries = (char**)malloc(capacity * sizeof(char*));
    if (!entries) { closedir(dir); return false; }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        const char* name = entry->d_name;
        if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) continue;

        // Determine type
        fileType type = FILE_TYPE_NONE;
#ifdef DT_DIR
        if (entry->d_type == DT_DIR) type = FILE_TYPE_DIR;
        else if (entry->d_type == DT_REG) type = FILE_TYPE_FILE;
        else {
            char tmp[MAX_PATH_SIZE];
            snprintf(tmp, sizeof(tmp), "%s/%s", path, name);
            fileStat st;
            if (stat_path(tmp, &st)) type = st.type;
        }
#else
        char tmp[MAX_PATH_SIZE];
        snprintf(tmp, sizeof(tmp), "%s/%s", path, name);
        fileStat st;
        if (stat_path(tmp, &st)) type = st.type;
#endif

        if (filter != FILE_TYPE_NONE && type != filter) continue;

        if (count >= capacity) {
            capacity *= 2;
            char** tmp2 = (char**)realloc(entries, capacity * sizeof(char*));
            if (!tmp2) {
                for (size_t i = 0; i < count; i++) free(entries[i]);
                free(entries);
                closedir(dir);
                return false;
            }
            entries = tmp2;
        }

        entries[count] = strdup(name);
        if (!entries[count]) continue;
        count++;
    }

    closedir(dir);
    *out_entries = entries;
    *out_count = count;
    return true;
#endif
}

