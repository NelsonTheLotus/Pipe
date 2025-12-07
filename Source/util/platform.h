#pragma once

#include "../global.h"

#include <stddef.h>
#include <stdint.h>

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
} targetArch;

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
} targetVendor;

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
} targetAbi;
    
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
} targetOS;



// ==== System Reflection Functions ====

targetArch get_target_arch(void);
targetVendor get_target_vendor(void);
targetAbi get_target_abi(void);
targetOS get_target_os(void);

char* get_target_group_name(void);
const char* get_target_arch_name(void);
const char* get_target_vendor_name(void);
const char* get_target_abi_name(void);
const char* get_target_os_name(void);

int platform_is_posix(void);
int platform_is_bsd(void);

/* -----------------------------------------------------------
   FILE / ENV / CACHE API (same as before)
   ----------------------------------------------------------- */

const char* platform_env_get(const char* var);
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
