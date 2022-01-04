/*
 * Copyright © 2000 SuSE, Inc.
 * Copyright © 2007 Red Hat, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of SuSE not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  SuSE makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * SuSE DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL SuSE
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "pixman-private.h"

typedef enum
{
    ARM_V7		= (1 << 0),
    ARM_V6		= (1 << 1),
    ARM_VFP		= (1 << 2),
    ARM_NEON		= (1 << 3),
    ARM_IWMMXT		= (1 << 4)
} arm_cpu_features_t;

#if defined(USE_ARM_SIMD) || defined(USE_ARM_NEON) || defined(USE_ARM_IWMMXT)

#if defined(_MSC_VER)

/* Needed for EXCEPTION_ILLEGAL_INSTRUCTION */
#include <windows.h>

extern int pixman_msvc_try_arm_neon_op ();
extern int pixman_msvc_try_arm_simd_op ();

static arm_cpu_features_t
detect_cpu_features (void)
{
    arm_cpu_features_t features = 0;

    __try
    {
	pixman_msvc_try_arm_simd_op ();
	features |= ARM_V6;
    }
    __except (GetExceptionCode () == EXCEPTION_ILLEGAL_INSTRUCTION)
    {
    }

    __try
    {
	pixman_msvc_try_arm_neon_op ();
	features |= ARM_NEON;
    }
    __except (GetExceptionCode () == EXCEPTION_ILLEGAL_INSTRUCTION)
    {
    }

    return features;
}

#elif defined(__APPLE__) && defined(TARGET_OS_IPHONE) /* iOS */

#include "TargetConditionals.h"

static arm_cpu_features_t
detect_cpu_features (void)
{
    arm_cpu_features_t features = 0;

    features |= ARM_V6;

    /* Detection of ARM NEON on iOS is fairly simple because iOS binaries
     * contain separate executable images for each processor architecture.
     * So all we have to do is detect the armv7 architecture build. The
     * operating system automatically runs the armv7 binary for armv7 devices
     * and the armv6 binary for armv6 devices.
     */
#if defined(__ARM_NEON__)
    features |= ARM_NEON;
#endif

    return features;
}

#elif defined(__ANDROID__) || defined(ANDROID) /* Android */

#include <cpu-features.h>

static arm_cpu_features_t
detect_cpu_features (void)
{
    arm_cpu_features_t features = 0;
    AndroidCpuFamily cpu_family;
    uint64_t cpu_features;

    cpu_family = android_getCpuFamily();
    cpu_features = android_getCpuFeatures();

    if (cpu_family == ANDROID_CPU_FAMILY_ARM)
    {
	if (cpu_features & ANDROID_CPU_ARM_FEATURE_ARMv7)
	    features |= ARM_V7;

	if (cpu_features & ANDROID_CPU_ARM_FEATURE_VFPv3)
	    features |= ARM_VFP;

	if (cpu_features & ANDROID_CPU_ARM_FEATURE_NEON)
	    features |= ARM_NEON;
    }

    return features;
}

#elif defined (__linux__) /* linux ELF */

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/utsname.h>
#include <fcntl.h>
#include <string.h>
#include <elf.h>

/*
 * The whole CPU capabilities detection is a bit ugly: when running in
 * userspace qemu, we see /proc/self/auxv from the host system. To make
 * everything even worse, the size of each value is 64-bit when running
 * on a 64-bit host system. So the data is totally bogus because we expect
 * 32-bit values. As AT_PLATFORM value is used as a pointer, it may cause
 * segfault (null pointer dereference on x86-64 host). So in order to be
 * on a safe side, we require that AT_PLATFORM value is found only once,
 * and it has non-zero value (this is still not totally reliable for a big
 * endian 64-bit host system running qemu and may theoretically fail).
 */
#define ARM_HWCAP_VFP 64
#define ARM_HWCAP_IWMMXT 512
#define ARM_HWCAP_NEON 4096

static arm_cpu_features_t
detect_cpu_features (void)
{
    arm_cpu_features_t features = 0;
    Elf32_auxv_t aux;
    int fd;
    uint32_t hwcap = 0;
    const char *plat = NULL;
    int plat_cnt = 0;

    fd = open ("/proc/self/auxv", O_RDONLY);
    if (fd >= 0)
    {
	while (read (fd, &aux, sizeof(Elf32_auxv_t)) == sizeof(Elf32_auxv_t))
	{
	    if (aux.a_type == AT_HWCAP)
	    {
		hwcap = aux.a_un.a_val;
	    }
	    else if (aux.a_type == AT_PLATFORM)
	    {
		plat = (const char*) aux.a_un.a_val;
		plat_cnt++;
	    }
	}
	close (fd);
	if (plat == NULL || plat_cnt != 1 || *plat != 'v')
	{
	    /*
	     * Something seems to be really wrong, most likely we are
	     * running under qemu. Let's use machine type from "uname" for
	     * CPU capabilities detection:
	     * http://www.mail-archive.com/qemu-devel at nongnu.org/msg22212.html
	     */
	    struct utsname u;
	    hwcap = 0; /* clear hwcap, because it is bogus */
	    if (uname (&u) == 0)
	    {
		if (strcmp (u.machine, "armv7l") == 0)
		{
		    features |= (ARM_V7 | ARM_V6);
		    hwcap |= ARM_HWCAP_VFP;  /* qemu is supposed to emulate vfp */
		    hwcap |= ARM_HWCAP_NEON; /* qemu is supposed to emulate neon */
		}
		else if (strcmp (u.machine, "armv6l") == 0)
		{
		    features |= ARM_V6;
		    hwcap |= ARM_HWCAP_VFP;  /* qemu is supposed to emulate vfp */
		}
	    }
	}
	else if (strncmp (plat, "v7l", 3) == 0)
	{
	    features |= (ARM_V7 | ARM_V6);
	}
	else if (strncmp (plat, "v6l", 3) == 0)
	{
	    features |= ARM_V6;
	}
    }

    /* hardcode these values to avoid depending on specific
     * versions of the hwcap header, e.g. HWCAP_NEON
     */
    if ((hwcap & ARM_HWCAP_VFP) != 0)
        features |= ARM_VFP;
    if ((hwcap & ARM_HWCAP_IWMMXT) != 0)
        features |= ARM_IWMMXT;
    /* this flag is only present on kernel 2.6.29 */
    if ((hwcap & ARM_HWCAP_NEON) != 0)
        features |= ARM_NEON;

    return features;
}

#else /* Unknown */

static arm_cpu_features_t
detect_cpu_features (void)
{
    return 0;
}

#endif /* Linux elf */

static pixman_bool_t
have_feature (arm_cpu_features_t feature)
{
    static pixman_bool_t initialized;
    static arm_cpu_features_t features;

    if (!initialized)
    {
	features = detect_cpu_features();
	initialized = TRUE;
    }

    return (features & feature) == feature;
}

#endif /* USE_ARM_SIMD || USE_ARM_NEON || USE_ARM_IWMMXT */

pixman_implementation_t *
_pixman_arm_get_implementations (pixman_implementation_t *imp)
{
#ifdef USE_ARM_SIMD
    if (!_pixman_disabled ("arm-simd") && have_feature (ARM_V6))
	imp = _pixman_implementation_create_arm_simd (imp);
#endif

#ifdef USE_ARM_IWMMXT
    if (!_pixman_disabled ("arm-iwmmxt") && have_feature (ARM_IWMMXT))
	imp = _pixman_implementation_create_mmx (imp);
#endif

#ifdef USE_ARM_NEON
    if (!_pixman_disabled ("arm-neon") && have_feature (ARM_NEON))
	imp = _pixman_implementation_create_arm_neon (imp);
#endif

    return imp;
}
