#pragma once

#include "../global.h"

#include <stddef.h>
#include <stdint.h>

// Platform independent inclusions
#if defined(_WIN32)
    #include <windows.h>
    #include <direct.h>
    #define PATH_MAX 260
#else
    #include <unistd.h>
    #include <limits.h>
#endif


// ==== PLATFORM ENUMS ====

typedef enum {
    PLATFORM_ARCH_UNKNOWN = 0,
    
    PLATFORM_ARCH_X86,
    PLATFORM_ARCH_X64,
    PLATFORM_ARCH_ARM,
    PLATFORM_ARCH_ARM64,
    PLATFORM_ARCH_RISCV64,
    PLATFORM_ARCH_PPC,
    PLATFORM_ARCH_PPC64,
    PLATFORM_ARCH_MIPS,
    PLATFORM_ARCH_MIPS64,
    
    PLATFORM_ARCH_COUNT
} hostArch;

typedef enum {
    PLATFORM_VENDOR_UNKNOWN = 0,

    PLATFORM_VENDOR_NONE,     // "none"
    PLATFORM_VENDOR_PC,       // "pc"
    PLATFORM_VENDOR_APPLE,    // "apple"
    PLATFORM_VENDOR_W64,      // "w64" (MinGW-w64)
    PLATFORM_VENDOR_IBM,      // "ibm"
    PLATFORM_VENDOR_ORACLE,   // "oracle" (Solaris)
    PLATFORM_VENDOR_HP,       // "hp" (HP-UX)

    PLATFORM_VENDOR_COUNT
} hostVendor;

typedef enum {
    PLATFORM_ABI_UNKNOWN = 0,
    PLATFORM_ABI_GLIBC,
    PLATFORM_ABI_MUSL,
    PLATFORM_ABI_MSVC,
    PLATFORM_ABI_BSD,
    PLATFORM_ABI_SOLARIS,
    PLATFORM_ABI_AIX,
    PLATFORM_ABI_HPUX,
    
    PLATFORM_ABI_COUNT
} hostAbi;
    
typedef enum {
    PLATFORM_OS_UNKNOWN = 0,

    PLATFORM_OS_WINDOWS,
    PLATFORM_OS_LINUX,
    PLATFORM_OS_MACOS,
    PLATFORM_OS_FREEBSD,
    PLATFORM_OS_NETBSD,
    PLATFORM_OS_OPENBSD,
    PLATFORM_OS_DRAGONFLYBSD,
    PLATFORM_OS_SOLARIS,
    PLATFORM_OS_AIX,
    PLATFORM_OS_HPUX,
    PLATFORM_OS_UNIX_GENERIC,

    PLATFORM_OS_COUNT
} hostOS;



// ==== Host System Reflection Functions ====
hostArch get_host_arch(void);
hostVendor get_host_vendor(void);
hostAbi get_host_abi(void);
hostOS get_host_os(void);

const char* get_host_group_name(void);
const char* get_host_arch_name(void);
const char* get_host_vendor_name(void);
const char* get_host_abi_name(void);
const char* get_host_os_name(void);

bool host_is_windows(void);
bool host_is_posix(void);
bool host_is_bsd(void);



// ==== System control ====

/*const char* platform_env_get(const char* var);
int platform_env_set(const char* var, const char* value);
int platform_env_exists(const char* var);

char* platform_file_read(const char* path, size_t* out_size);
int platform_file_write(const char* path, const char* data, size_t size);
int platform_file_exists(const char* path);
int platform_file_delete(const char* path);

int platform_mkdir(const char* path);
int platform_dir_exists(const char* path);

bool paltform_get_cwd(void);
bool platform_set_cwd(const char* path);

int platform_cache_init(const char* cache_dir);
int platform_cache_write(const char* key, const char* value);
char* platform_cache_read(const char* key);

uint64_t platform_timestamp_ms(void);
char* platform_join_path(const char* a, const char* b);
void platform_free(void* p);
*/

// ==== Interface ====

bool create_dir(const char* path);
// stat()

const char* get_cwd(void);
bool set_cwd(const char* path);
