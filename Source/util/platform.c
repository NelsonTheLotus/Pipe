#include "platform.h"

#include "log.h"


#include <string.h>
#include <stdlib.h>



// ==== Detection functions ====

targetArch get_target_arch(void) 
{
#if defined(__x86_64__) || defined(_M_X64)
    return PLATFORM_ARCH_X64;

#elif defined(__i386__) || defined(_M_IX86)
    return PLATFORM_ARCH_X86;

#elif defined(__aarch64__) || defined(_M_ARM64)
    return PLATFORM_ARCH_ARM64;

#elif defined(__arm__) || defined(_M_ARM)
    return PLATFORM_ARCH_ARM;

#elif defined(__riscv) && __riscv_xlen == 64
    return PLATFORM_ARCH_RISCV64;

#elif defined(__powerpc64__) || defined(__ppc64__)
    return PLATFORM_ARCH_PPC64;

#elif defined(__powerpc__) || defined(__ppc__)
    return PLATFORM_ARCH_PPC;

#elif defined(__mips64)
    return PLATFORM_ARCH_MIPS64;

#elif defined(__mips__)
    return PLATFORM_ARCH_MIPS;

#else
    return PLATFORM_ARCH_UNKNOWN;
#endif
}

const char* get_target_arch_name(void) 
{
    switch (get_target_arch()) {
        case PLATFORM_ARCH_X86:     return "x86";
        case PLATFORM_ARCH_X64:     return "x86_64";
        case PLATFORM_ARCH_ARM:     return "arm";
        case PLATFORM_ARCH_ARM64:   return "aarch64";
        case PLATFORM_ARCH_RISCV64: return "riscv64";
        case PLATFORM_ARCH_PPC:     return "ppc";
        case PLATFORM_ARCH_PPC64:   return "ppc64";
        case PLATFORM_ARCH_MIPS:    return "mips";
        case PLATFORM_ARCH_MIPS64:  return "mips64";
        default:                    return "unknown";
    }
}


targetVendor get_target_vendor(void) 
{
#if defined(__APPLE__)
    return PLATFORM_VENDOR_APPLE;

#elif defined(_WIN32)
    return PLATFORM_VENDOR_W64; /* MinGW/MSVC target uses w64 */

#elif defined(__linux__)
    return PLATFORM_VENDOR_PC;

#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)
    return PLATFORM_VENDOR_PC;

#elif defined(__sun)
    return PLATFORM_VENDOR_ORACLE;

#elif defined(_AIX)
    return PLATFORM_VENDOR_IBM;

#elif defined(__hpux)
    return PLATFORM_VENDOR_HP;

#else
    return PLATFORM_VENDOR_UNKNOWN;
#endif
}

const char* get_target_vendor_name(void) 
{
    switch (get_target_vendor()) {
        case PLATFORM_VENDOR_NONE:   return "none";
        case PLATFORM_VENDOR_PC:     return "pc";
        case PLATFORM_VENDOR_APPLE:  return "apple";
        case PLATFORM_VENDOR_W64:    return "w64";
        case PLATFORM_VENDOR_IBM:    return "ibm";
        case PLATFORM_VENDOR_ORACLE: return "oracle";
        case PLATFORM_VENDOR_HP:     return "hp";
        default:                     return "unknown";
    }
}


targetAbi get_target_abi(void) 
{
#if defined(_WIN32)
    return PLATFORM_ABI_MSVC;

#elif defined(__GLIBC__)
    return PLATFORM_ABI_GLIBC;

#elif defined(__MUSL__)
    return PLATFORM_ABI_MUSL;

/* BSD libcs */
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)
    return PLATFORM_ABI_BSD;

#elif defined(__sun)
    return PLATFORM_ABI_SOLARIS;

#elif defined(_AIX)
    return PLATFORM_ABI_AIX;

#elif defined(__hpux)
    return PLATFORM_ABI_HPUX;

#else
    return PLATFORM_ABI_UNKNOWN;
#endif
}

const char* get_target_abi_name(void) 
{
    switch (get_target_abi()) {
        case PLATFORM_ABI_GLIBC:   return "gnu";     /* convention in triplets */
        case PLATFORM_ABI_MUSL:    return "musl";
        case PLATFORM_ABI_MSVC:    return "msvc";
        case PLATFORM_ABI_BSD:     return "bsd";
        case PLATFORM_ABI_SOLARIS: return "solaris";
        case PLATFORM_ABI_AIX:     return "aix";
        case PLATFORM_ABI_HPUX:    return "hpux";
        default:                   return "unknown";
    }
}


targetOS get_target_os(void)
{
#if defined(_WIN32) || defined(_WIN64)
    return PLATFORM_OS_WINDOWS;

#elif defined(__APPLE__) && defined(__MACH__)
    return PLATFORM_OS_MACOS;

#elif defined(__linux__)
    return PLATFORM_OS_LINUX;

#elif defined(__FreeBSD__)
    return PLATFORM_OS_FREEBSD;

#elif defined(__NetBSD__)
    return PLATFORM_OS_NETBSD;

#elif defined(__OpenBSD__)
    return PLATFORM_OS_OPENBSD;

#elif defined(__DragonFly__)
    return PLATFORM_OS_DRAGONFLYBSD;

#elif defined(__sun)
    return PLATFORM_OS_SOLARIS;

#elif defined(_AIX)
    return PLATFORM_OS_AIX;

#elif defined(__hpux)
    return PLATFORM_OS_HPUX;

#elif defined(__unix__) || defined(__unix)
    return PLATFORM_OS_UNIX_GENERIC;

#else
    return PLATFORM_OS_UNKNOWN;
#endif
}

const char* get_target_os_name(void) 
{
    switch (get_target_os()) {
        case PLATFORM_OS_WINDOWS:      return "windows";
        case PLATFORM_OS_LINUX:        return "linux";
        case PLATFORM_OS_MACOS:        return "darwin";
        case PLATFORM_OS_FREEBSD:      return "freebsd";
        case PLATFORM_OS_NETBSD:       return "netbsd";
        case PLATFORM_OS_OPENBSD:      return "openbsd";
        case PLATFORM_OS_DRAGONFLYBSD: return "dragonfly";
        case PLATFORM_OS_SOLARIS:      return "solaris";
        case PLATFORM_OS_AIX:          return "aix";
        case PLATFORM_OS_HPUX:         return "hpux";
        case PLATFORM_OS_UNIX_GENERIC: return "unix";
        default:                       return "unknown";
    }
}



// ==== Target group generation: arch-vendor-os-abi ==== 

/*
 * Return a group string with format:
 *  <arch>-<vendor>-<os>-<abi>
 * 
 * **Returned string must be freed**
*/
char* get_target_group_name(void) 
{
    const char* arch   = get_target_arch_name();
    const char* vendor = get_target_vendor_name();
    const char* os     = get_target_os_name();
    const char* abi    = get_target_abi_name();

    /* triplet string length */
    size_t len = strlen(arch) + strlen(vendor) + strlen(os) + strlen(abi) + 4 + 1;

    char* out = (char*)malloc(len);
    if (!out)
    {
        log_l("Unable to allocate target group string.", CRITICAL);
        return NULL;    // don't return a static string against failure
    }

    strcat(out, arch);
    strcat(out, "-");
    strcat(out, vendor);
    strcat(out, "-");
    strcat(out, os);
    strcat(out, "-");
    strcat(out, abi);

    // snprintf(out, len, "%s-%s-%s-%s", arch, vendor, os, abi);
    return out;
    // malloced string is then owned by caller**
}




// ==== Interface ====

bool make_dir(const char* path);

const char* get_cwd(void);
bool set_cwd(const char* path);
