#pragma once

#include "../global.h"

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



// ==== Interface Structures ====

typedef enum {
    FILE_TYPE_NONE = 0,
    FILE_TYPE_FILE,
    FILE_TYPE_DIR,
    FILE_TYPE_ANY
} fileType;

typedef struct {
    bool exists;
    fileType type;
    uint64_t mtime; // modification time, standardized to unix-time
} fileStat;



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



// ==== Interface ====

//* Get the current working directory in standard form.
const char* get_cwd(void);
//* Set the current working directory in standard form.
bool set_cwd(const char* path);

//* Get path information. File can be file or directory. Returns 0 on success.
bool stat_path(const char* path, fileStat* out);

//* Create desired path. Returns 0 on success.
bool create_dir(const char* path);
//* file creation is already handeled by the C fopen

/*
 * Concatenates `target` to `source`, adding a separator if necessary.
 * 
 * @param source The original path, will be modified in-place.
 * @param target The path to append.
 * @param max_source_size The total size of the `source` buffer.
 * @return true if concatenation succeeded, false if buffer would overflow.
 */
bool join_path(char* source, const char* target, const size_t max_source_size);

//* Get system time.
//* list files and directories