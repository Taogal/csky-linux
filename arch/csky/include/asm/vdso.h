// SPDX-License-Identifier: GPL-2.0
// Copyright (C) 2018 Hangzhou C-SKY Microsystems co.,ltd.
#ifndef __ASM_CSKY_VDSO_H
#define __ASM_CSKY_VDSO_H

#include <abi/vdso.h>

struct csky_vdso {
	unsigned short rt_signal_retcode[4];
};

#endif /* __ASM_CSKY_VDSO_H */
