/*
 * Copyright (c) 2015 Etienne Gemsa <etienne.gemsa@lse.epita.fr>
 * Copyright (c) 2015-2016 Dmitry V. Levin <ldv@altlinux.org>
 * Copyright (c) 2015-2018 The strace developers.
 * All rights reserved.
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "defs.h"

#ifdef HAVE_LINUX_INPUT_H

# include DEF_MPERS_TYPE(struct_ff_effect)

# include <linux/ioctl.h>
# include <linux/input.h>

typedef struct ff_effect struct_ff_effect;

#endif /* HAVE_LINUX_INPUT_H */

#include MPERS_DEFS

#ifdef HAVE_LINUX_INPUT_H

static void
decode_envelope(void *const data)
{
	const struct ff_envelope *const envelope = data;

	tprintf(", envelope={attack_length=%" PRIu16
		", attack_level=%" PRIu16
		", fade_length=%" PRIu16
		", fade_level=%#x}",
		envelope->attack_length,
		envelope->attack_level,
		envelope->fade_length,
		envelope->fade_level);
}

static int
ff_effect_ioctl(struct tcb *const tcp, const kernel_ulong_t arg)
{
	tprints(", ");

	struct_ff_effect ffe;

	if (umove_or_printaddr(tcp, arg, &ffe))
		return RVAL_IOCTL_DECODED;

	tprints("{type=");
	print_evdev_ff_type(ffe.type);
	tprintf(", id=%" PRIu16
		", direction=%" PRIu16 ", ",
		ffe.id,
		ffe.direction);

	if (abbrev(tcp)) {
		tprints("...}");
		return RVAL_IOCTL_DECODED;
	}

	tprintf("trigger={button=%" PRIu16
		", interval=%" PRIu16 "}"
		", replay={length=%" PRIu16
		", delay=%" PRIu16 "}",
		ffe.trigger.button,
		ffe.trigger.interval,
		ffe.replay.length,
		ffe.replay.delay);

	switch (ffe.type) {
		case FF_CONSTANT:
			tprintf(", constant={level=%" PRId16,
				ffe.u.constant.level);
			decode_envelope(&ffe.u.constant.envelope);
			tprints("}");
			break;
		case FF_RAMP:
			tprintf(", ramp={start_level=%" PRId16
				", end_level=%" PRId16,
				ffe.u.ramp.start_level,
				ffe.u.ramp.end_level);
			decode_envelope(&ffe.u.ramp.envelope);
			tprints("}");
			break;
		case FF_PERIODIC:
			tprintf(", periodic={waveform=%" PRIu16
				", period=%" PRIu16
				", magnitude=%" PRId16
				", offset=%" PRId16
				", phase=%" PRIu16,
				ffe.u.periodic.waveform,
				ffe.u.periodic.period,
				ffe.u.periodic.magnitude,
				ffe.u.periodic.offset,
				ffe.u.periodic.phase);
			decode_envelope(&ffe.u.periodic.envelope);
			tprintf(", custom_len=%u, custom_data=",
				ffe.u.periodic.custom_len);
			printaddr(ptr_to_kulong(ffe.u.periodic.custom_data));
			tprints("}");
			break;
		case FF_RUMBLE:
			tprintf(", rumble={strong_magnitude=%" PRIu16
				", weak_magnitude=%" PRIu16 "}",
				ffe.u.rumble.strong_magnitude,
				ffe.u.rumble.weak_magnitude);
			break;
		default:
			break;
	}

	tprints("}");

	return RVAL_IOCTL_DECODED;
}

MPERS_PRINTER_DECL(int, evdev_write_ioctl_mpers, struct tcb *const tcp,
		   const unsigned int code, const kernel_ulong_t arg)
{
	switch (code) {
		case EVIOCSFF:
			return ff_effect_ioctl(tcp, arg);
		default:
			return RVAL_DECODED;
	}
}

#endif /* HAVE_LINUX_INPUT_H */
