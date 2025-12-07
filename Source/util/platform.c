#include "platform.h"



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

const char* normalize_path(const char* path)
{
    if (!path) return NULL;

    size_t len = strlen(path);
    char* out = (char*)malloc(len + 1);
    if (!out) return NULL;

    char sep = host_is_windows() ? '\\' : '/';
    char prev = 0;
    size_t j = 0;

    for (size_t i = 0; i < len; ++i) {
        char c = path[i];

        // Convert '/' to platform separator on Windows
        if (host_is_windows() && c == '/') c = '\\';

        // Collapse repeated separators
        if (c == sep && prev == sep) continue;

        out[j++] = c;
        prev = c;
    }

    // Remove trailing separator (except for root)
    if (j > 1 && out[j - 1] == sep) j--;

    out[j] = '\0';
    return out;
}

const char* resolve_full_path(const char* relative_path)
{
    return "";
}


// ==== Interface ====

bool create_dir(const char* path);

const char* get_cwd(void);
bool set_cwd(const char* path);
