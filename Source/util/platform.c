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



// ==== Platform functions ====

const char* get_cwd(char* buf, int size)   // don't even check if on linux yet
{
    return getcwd(buf, size);
}


fileStat stat_path(const char* path)
{
    fileStat returnStat = (fileStat){false, FILE_TYPE_NONE, 0, false, false};

    if(path == NULL) return returnStat;
    struct stat path_stat;

    if(stat(path, &path_stat) == -1) return returnStat;
    
    returnStat.exists = true;
    if(S_ISREG(path_stat.st_mode)) returnStat.type = FILE_TYPE_FILE;
    else if(S_ISDIR(path_stat.st_mode)) returnStat.type = FILE_TYPE_DIR;
    else returnStat.type = FILE_TYPE_ANY;
    returnStat.mtime = path_stat.st_mtime;
    if(access(path, R_OK) == 0) returnStat.readAllow = true;
    if(access(path, W_OK) == 0) returnStat.writeAllow = true;

    return returnStat;
}

bool create_dir(const char* path)
{
    if(path == NULL) return false;
    if(mkdir(path, 0777) == -1) return false;
    return true;
}