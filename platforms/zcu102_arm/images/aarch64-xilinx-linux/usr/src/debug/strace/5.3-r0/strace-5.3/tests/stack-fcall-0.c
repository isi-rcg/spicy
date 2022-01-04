/*
 * Copyright (c) 2014-2019 The strace developers.
 * All rights reserved.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "stack-fcall.h"

int f0(int i, unsigned long f)
{
	return f1(i, f ^ (unsigned long) (void *) f0) - i;
}
