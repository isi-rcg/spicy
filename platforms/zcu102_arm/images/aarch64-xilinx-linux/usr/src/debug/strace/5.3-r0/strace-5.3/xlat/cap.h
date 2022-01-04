/* Generated by ./xlat/gen.sh from ./xlat/cap.in; do not edit. */

#include "gcc_compat.h"
#include "static_assert.h"


#ifndef XLAT_MACROS_ONLY

# ifdef IN_MPERS

#  error static const struct xlat cap in mpers mode

# else

static const struct xlat_data cap_xdata[] = {
 XLAT(CAP_CHOWN),
 XLAT(CAP_DAC_OVERRIDE),
 XLAT(CAP_DAC_READ_SEARCH),
 XLAT(CAP_FOWNER),
 XLAT(CAP_FSETID),
 XLAT(CAP_KILL),
 XLAT(CAP_SETGID),
 XLAT(CAP_SETUID),
 XLAT(CAP_SETPCAP),
 XLAT(CAP_LINUX_IMMUTABLE),
 XLAT(CAP_NET_BIND_SERVICE),
 XLAT(CAP_NET_BROADCAST),
 XLAT(CAP_NET_ADMIN),
 XLAT(CAP_NET_RAW),
 XLAT(CAP_IPC_LOCK),
 XLAT(CAP_IPC_OWNER),
 XLAT(CAP_SYS_MODULE),
 XLAT(CAP_SYS_RAWIO),
 XLAT(CAP_SYS_CHROOT),
 XLAT(CAP_SYS_PTRACE),
 XLAT(CAP_SYS_PACCT),
 XLAT(CAP_SYS_ADMIN),
 XLAT(CAP_SYS_BOOT),
 XLAT(CAP_SYS_NICE),
 XLAT(CAP_SYS_RESOURCE),
 XLAT(CAP_SYS_TIME),
 XLAT(CAP_SYS_TTY_CONFIG),
 XLAT(CAP_MKNOD),
 XLAT(CAP_LEASE),
 XLAT(CAP_AUDIT_WRITE),
 XLAT(CAP_AUDIT_CONTROL),
 XLAT(CAP_SETFCAP),
 XLAT(CAP_MAC_OVERRIDE),
 XLAT(CAP_MAC_ADMIN),
 XLAT(CAP_SYSLOG),
 XLAT(CAP_WAKE_ALARM),
 XLAT(CAP_BLOCK_SUSPEND),
 XLAT(CAP_AUDIT_READ),
};
static
const struct xlat cap[1] = { {
 .data = cap_xdata,
 .size = ARRAY_SIZE(cap_xdata),
 .type = XT_NORMAL,
} };

# endif /* !IN_MPERS */

#endif /* !XLAT_MACROS_ONLY */