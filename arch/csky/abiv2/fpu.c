// SPDX-License-Identifier: GPL-2.0
// Copyright (C) 2018 Hangzhou C-SKY Microsystems co.,ltd.
#include <linux/ptrace.h>
#include <linux/uaccess.h>
#include <abi/reg_ops.h>

#define MTCR_MASK	0xFC00FFE0
#define MFCR_MASK	0xFC00FFE0
#define MTCR_DIST	0xC0006420
#define MFCR_DIST	0xC0006020

void __init init_fpu(void)
{
	mtcr("cr<1, 2>", 0);
}

/*
 * fpu_libc_helper() is to help libc to excute:
 *  - mfcr %a, cr<1, 2>
 *  - mfcr %a, cr<2, 2>
 *  - mtcr %a, cr<1, 2>
 *  - mtcr %a, cr<2, 2>
 */
int fpu_libc_helper(struct pt_regs * regs)
{
	int fault;
	unsigned long instrptr, regx = 0;
	unsigned long index = 0, tmp = 0;
	unsigned long tinstr = 0;
	u16 instr_hi, instr_low;

	instrptr = instruction_pointer(regs);
	if (instrptr & 1) return 0;

	fault = __get_user(instr_low, (u16 *)instrptr);
	if (fault) return 0;

	fault = __get_user(instr_hi, (u16 *)(instrptr + 2));
	if (fault) return 0;

	tinstr = instr_hi | ((unsigned long)instr_low << 16);

	if (((tinstr >> 21) & 0x1F) != 2) return 0;

	if ((tinstr & MTCR_MASK) == MTCR_DIST)
	{
		index = (tinstr >> 16) & 0x1F;
		if(index > 13) return 0;

		tmp = tinstr & 0x1F;
		if (tmp > 2) return 0;

		regx =  *(&regs->a0 + index);

		if(tmp == 1)
			mtcr("cr<1, 2>", regx);
		else if (tmp == 2)
			mtcr("cr<2, 2>", regx);
		else
			return 0;

		regs->pc +=4;
		return 1;
	}

	if ((tinstr & MFCR_MASK) == MFCR_DIST) {
		index = tinstr & 0x1F;
		if(index > 13) return 0;

		tmp = ((tinstr >> 16) & 0x1F);
		if (tmp > 2) return 0;

		if (tmp == 1)
			regx = mfcr("cr<1, 2>");
		else if (tmp == 2)
			regx = mfcr("cr<2, 2>");
		else
			return 0;

		*(&regs->a0 + index) = regx;

		regs->pc +=4;
		return 1;
	}

	return 0;
}

void fpu_fpe(struct pt_regs * regs)
{
	int sig;
	unsigned int fesr;
	siginfo_t info;
	asm volatile("mfcr %0, cr<2, 2>":"=r"(fesr));

	if(fesr & FPE_ILLE){
		info.si_code = ILL_ILLOPC;
		sig = SIGILL;
	}
	else if(fesr & FPE_IDC){
		info.si_code = ILL_ILLOPN;
		sig = SIGILL;
	}
	else if(fesr & FPE_FEC){
		sig = SIGFPE;
		if(fesr & FPE_IOC){
			info.si_code = FPE_FLTINV;
		}
		else if(fesr & FPE_DZC){
			info.si_code = FPE_FLTDIV;
		}
		else if(fesr & FPE_UFC){
			info.si_code = FPE_FLTUND;
		}
		else if(fesr & FPE_OFC){
			info.si_code = FPE_FLTOVF;
		}
		else if(fesr & FPE_IXC){
			info.si_code = FPE_FLTRES;
		}
		else {
			info.si_code = NSIGFPE;
		}
	}
	else {
		info.si_code = NSIGFPE;
		sig = SIGFPE;
	}
	info.si_signo = SIGFPE;
	info.si_errno = 0;
	info.si_addr = (void *)regs->pc;
	force_sig_info(sig, &info, current);
}

#define FMFVR_FPU_REGS(vrx, vry)	\
	"fmfvrl %0, "#vrx" \n"		\
	"fmfvrh %1, "#vrx" \n"		\
	"fmfvrl %2, "#vry" \n"		\
	"fmfvrh %3, "#vry" \n"

#define FMTVR_FPU_REGS(vrx, vry)	\
	"fmtvrl "#vrx", %0 \n"		\
	"fmtvrh "#vrx", %1 \n"		\
	"fmtvrl "#vry", %2 \n"		\
	"fmtvrh "#vry", %3 \n"

#define STW_FPU_REGS(a, b, c, d)	\
	"stw    %0, (%4, "#a") \n"	\
	"stw    %1, (%4, "#b") \n"	\
	"stw    %2, (%4, "#c") \n"	\
	"stw    %3, (%4, "#d") \n"

#define LDW_FPU_REGS(a, b, c, d)	\
	"ldw    %0, (%4, "#a") \n"	\
	"ldw    %1, (%4, "#b") \n"	\
	"ldw    %2, (%4, "#c") \n"	\
	"ldw    %3, (%4, "#d") \n"

void save_to_user_fp(struct user_fp *user_fp)
{
	unsigned long flg;
	unsigned long tmp1, tmp2, tmp3, tmp4;
	unsigned long *fpregs;

	local_save_flags(flg);


	asm volatile(
		"mfcr    %0, cr<1, 2> \n"
		"mfcr    %1, cr<2, 2> \n"
		:"=r"(tmp1),"=r"(tmp2));

	user_fp->fcr = tmp1;
	user_fp->fesr = tmp2;

	fpregs = &user_fp->vr[0];
	asm volatile(
		FMFVR_FPU_REGS(vr0, vr1)
		STW_FPU_REGS(0, 4, 16, 20)
		FMFVR_FPU_REGS(vr2, vr3)
		STW_FPU_REGS(32, 36, 48, 52)
		FMFVR_FPU_REGS(vr4, vr5)
		STW_FPU_REGS(64, 68, 80, 84)
		FMFVR_FPU_REGS(vr6, vr7)
		STW_FPU_REGS(96, 100, 112, 116)
		"addi    %4, 128\n"
		FMFVR_FPU_REGS(vr8, vr9)
		STW_FPU_REGS(0, 4, 16, 20)
		FMFVR_FPU_REGS(vr10, vr11)
		STW_FPU_REGS(32, 36, 48, 52)
		FMFVR_FPU_REGS(vr12, vr13)
		STW_FPU_REGS(64, 68, 80, 84)
		FMFVR_FPU_REGS(vr14, vr15)
		STW_FPU_REGS(96, 100, 112, 116)
		:"=a"(tmp1),"=a"(tmp2),"=a"(tmp3),
		"=a"(tmp4),"+a"(fpregs));

	local_irq_restore(flg);
}

void restore_from_user_fp(struct user_fp *user_fp)
{
	unsigned long flg;
	unsigned long tmp1, tmp2, tmp3, tmp4;
	unsigned long *fpregs;

	local_irq_save(flg);

	tmp1 = user_fp->fcr;
	tmp2 = user_fp->fesr;

	asm volatile(
		"mtcr   %0, cr<1, 2>\n"
		"mtcr   %1, cr<2, 2>\n"
		::"r"(tmp1), "r"(tmp2));

	fpregs = &user_fp->vr[0];
	asm volatile(
		LDW_FPU_REGS(0, 4, 16, 20)
		FMTVR_FPU_REGS(vr0, vr1)
		LDW_FPU_REGS(32, 36, 48, 52)
		FMTVR_FPU_REGS(vr2, vr3)
		LDW_FPU_REGS(64, 68, 80, 84)
		FMTVR_FPU_REGS(vr4, vr5)
		LDW_FPU_REGS(96, 100, 112, 116)
		FMTVR_FPU_REGS(vr6, vr7)
		"addi   %4, 128\n"
		LDW_FPU_REGS(0, 4, 16, 20)
		FMTVR_FPU_REGS(vr8, vr9)
		LDW_FPU_REGS(32, 36, 48, 52)
		FMTVR_FPU_REGS(vr10, vr11)
		LDW_FPU_REGS(64, 68, 80, 84)
		FMTVR_FPU_REGS(vr12, vr13)
		LDW_FPU_REGS(96, 100, 112, 116)
		FMTVR_FPU_REGS(vr14, vr15)
		:"=a"(tmp1),"=a"(tmp2),"=a"(tmp3),
		"=a"(tmp4),"+a"(fpregs));

	local_irq_restore(flg);
}


