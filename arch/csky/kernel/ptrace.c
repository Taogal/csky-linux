// SPDX-License-Identifier: GPL-2.0
// Copyright (C) 2018 Hangzhou C-SKY Microsystems co.,ltd.
#include <linux/elf.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/ptrace.h>
#include <linux/regset.h>
#include <linux/sched.h>
#include <linux/signal.h>
#include <linux/smp.h>
#include <linux/uaccess.h>
#include <linux/user.h>

#include <asm/thread_info.h>
#include <asm/page.h>
#include <asm/pgtable.h>
#include <asm/processor.h>
#include <asm/asm-offsets.h>

#include <abi/regdef.h>

/* sets the trace bits. */
#define TRACE_MODE_SI      1 << 14
#define TRACE_MODE_RUN     0
#define TRACE_MODE_JMP     0x3 << 14
#define TRACE_MODE_MASK    ~(0x3 << 14)

/*
 * Translate table used by gdbserver, convert pt_regs into user_regs_struct
 *
 * PT_xxx is the stack offset at which the register is saved.
 * Notice that usp has no stack-slot and needs to be treated
 * specially (see get_reg/put_reg below).
 *
 * FIXME: let gdbserver use pt_regs directly, and add sp into pt_regs.
 */
static int regoff[] = PTRACE_REGOFF_ABI;

/*
 * Get contents of register REGNO in task TASK.
 */
static long get_reg(struct task_struct *task, int regno)
{
	unsigned long *addr;

	if ((regno < sizeof(regoff)/sizeof(regoff[0])) && (regoff[regno] != -1)) {
		addr = (unsigned long *)(task->thread.esp0 + regoff[regno]);
		return *addr;
	} else
		return 0;
}

static int put_reg(struct task_struct *task, int regno,
		unsigned long data)
{
	unsigned long *addr;

	if ((regno < sizeof(regoff) / sizeof(regoff[0])) && (regoff[regno] != -1)) {
		addr = (unsigned long *) (task->thread.esp0 + regoff[regno]);
		*addr = data;
		return 0;
	} else
		return -1;
}
/*
 * Make sure the single step bit is not set.
 */
static void singlestep_disable(struct task_struct *child)
{
	unsigned long tmp;
	tmp = (get_reg(child, REGNO_SR) & TRACE_MODE_MASK) | TRACE_MODE_RUN;
	put_reg(child, REGNO_SR, tmp);
}


static void singlestep_enable(struct task_struct *child)
{
	unsigned long tmp;
	tmp = (get_reg(child, REGNO_SR) & TRACE_MODE_MASK) | TRACE_MODE_SI;
	put_reg(child, REGNO_SR, tmp);
}

/*
 * Make sure the single step bit is set.
 */
void user_enable_single_step(struct task_struct *child)
{
	if (child->thread.esp0 == 0) return;
	singlestep_enable(child);
}

void user_disable_single_step(struct task_struct *child)
{
	if (child->thread.esp0 == 0) return;
	singlestep_disable(child);
}

int ptrace_getfpregs(struct task_struct *child, void __user *data)
{

	if (!access_ok(VERIFY_WRITE, data, sizeof(struct user_cskyfp_struct)))
		return -EIO;

	if(raw_copy_to_user(data, &child->thread.fcr,
				sizeof(struct user_cskyfp_struct)))
		return -EFAULT;

	return 0;
}

int ptrace_setfpregs(struct task_struct *child, void __user *data)
{
	if (!access_ok(VERIFY_READ, data, sizeof(struct user_cskyfp_struct)))
		return -EIO;

	if(raw_copy_from_user(&child->thread.fcr, data,
				sizeof(struct user_cskyfp_struct)))
		return -EFAULT;
	return 0;
}

enum csky_regset {
	REGSET_GPR,
};

static int gpr_get(struct task_struct *target,
		   const struct user_regset *regset,
		   unsigned int pos, unsigned int count,
		   void *kbuf, void __user *ubuf)
{
	struct pt_regs *regs;

	regs = task_pt_regs(target);
	return user_regset_copyout(&pos, &count, &kbuf, &ubuf, regs, 0, -1);
}

static int gpr_set(struct task_struct *target,
		    const struct user_regset *regset,
		    unsigned int pos, unsigned int count,
		    const void *kbuf, const void __user *ubuf)
{
	int ret;
	struct pt_regs *regs;

	regs = task_pt_regs(target);
	ret = user_regset_copyin(&pos, &count, &kbuf, &ubuf, &regs, 0, -1);
	return ret;
}

static const struct user_regset csky_regsets[] = {
	[REGSET_GPR] = {
		.core_note_type = NT_PRSTATUS,
		.n = ELF_NGREG,
		.size = sizeof(elf_greg_t),
		.align = sizeof(elf_greg_t),
		.get = &gpr_get,
		.set = &gpr_set,
	},
};

static const struct user_regset_view user_csky_view = {
	.name = "csky",
	.e_machine = ELF_ARCH,
	.regsets = csky_regsets,
	.n = ARRAY_SIZE(csky_regsets),
};

const struct user_regset_view *task_user_regset_view(struct task_struct *task)
{
	return &user_csky_view;
}

void ptrace_disable(struct task_struct *child)
{
	singlestep_disable(child);
}

long arch_ptrace(struct task_struct *child, long request, unsigned long addr,
		unsigned long data)
{
	unsigned long tmp = 0, ret = 0;
	int i;

	switch (request) {
	/*
	 * FIXME: remove the PTRACE_GETREGS/SETREGS
	 * Let gdbserver use PTRACE_GETREGSET/SETREGSET
	 */
	case PTRACE_GETREGS:    /* Get all gp regs from the child. */
		for (i = 0; i <= CSKY_GREG_NUM; i++) {
			tmp = get_reg(child, i);
			ret = put_user(tmp, (unsigned long *)data);
			if (ret)
				break;
			data += sizeof(long);
		}
		break;
	case PTRACE_SETREGS:    /* Set all gp regs in the child. */
		for (i = 0; i <= CSKY_GREG_NUM; i++) {
			ret = get_user(tmp, (unsigned long *)data);
			if (ret)
				break;
			put_reg(child, i, tmp);
			data += sizeof(long);
		}
		break;

	case PTRACE_GETFPREGS:
		ret = ptrace_getfpregs(child, (void  __user *) data);
		break;

	case PTRACE_SETFPREGS:
		ret = ptrace_setfpregs(child, (void __user *) data);
		break;

	case PTRACE_GET_THREAD_AREA:
		ret = put_user(task_thread_info(child)->tp_value,
				(long unsigned int *) data);
		break;
	default:
		ret = ptrace_request(child, request, addr, data);
		break;
	}

	return ret;
}

/*
 * If process's system calls is traces, do some corresponding handles in this
 * fuction before entering system call function and after exiting system call
 * fuction.
 */
asmlinkage void syscall_trace(int why, struct pt_regs * regs)
{
	long saved_why;
	/*
	 * Save saved_why, why is used to denote syscall entry/exit;
	 * why = 0:entry, why = 1: exit
	 */
	saved_why = regs->regs[SYSTRACE_SAVENUM];
	regs->regs[SYSTRACE_SAVENUM] = why;

	/* the 0x80 provides a way for the tracing parent to distinguish
	   between a syscall stop and SIGTRAP delivery */
	ptrace_notify(SIGTRAP | ((current->ptrace & PT_TRACESYSGOOD)
				? 0x80 : 0));
	/*
	 * this isn't the same as continuing with a signal, but it will do
	 * for normal use.  strace only continues with a signal if the
	 * stopping signal is not SIGTRAP.  -brl
	 */
	if (current->exit_code) {
		send_sig(current->exit_code, current, 1);
		current->exit_code = 0;
	}

	regs->regs[SYSTRACE_SAVENUM] = saved_why;
	return;
}

void show_regs(struct pt_regs *fp)
{
	unsigned long   *sp;
	unsigned char   *tp;
	int	i;

	pr_info("\nCURRENT PROCESS:\n\n");
	pr_info("COMM=%s PID=%d\n", current->comm, current->pid);

	if (current->mm) {
		pr_info("TEXT=%08x-%08x DATA=%08x-%08x BSS=%08x-%08x\n",
		       (int) current->mm->start_code,
		       (int) current->mm->end_code,
		       (int) current->mm->start_data,
		       (int) current->mm->end_data,
		       (int) current->mm->end_data,
		       (int) current->mm->brk);
		pr_info("USER-STACK=%08x  KERNEL-STACK=%08x\n\n",
		       (int) current->mm->start_stack,
		       (int) (((unsigned long) current) + 2 * PAGE_SIZE));
	}

	pr_info("PC: 0x%08lx\n", (long)fp->pc);
	pr_info("orig_a0: 0x%08lx\n", fp->orig_a0);
	pr_info("PSR: 0x%08lx\n", (long)fp->sr);

	pr_info("a0: 0x%08lx  a1: 0x%08lx  a2: 0x%08lx  a3: 0x%08lx\n",
	       fp->a0, fp->a1, fp->a2, fp->a3);
#if defined(__CSKYABIV2__)
	pr_info("r4: 0x%08lx  r5: 0x%08lx    r6: 0x%08lx    r7: 0x%08lx\n",
	       fp->regs[0], fp->regs[1], fp->regs[2], fp->regs[3]);
	pr_info("r8: 0x%08lx  r9: 0x%08lx   r10: 0x%08lx   r11: 0x%08lx\n",
	       fp->regs[4], fp->regs[5], fp->regs[6], fp->regs[7]);
	pr_info("r12 0x%08lx  r13: 0x%08lx   r15: 0x%08lx\n",
	       fp->regs[8], fp->regs[9], fp->r15);
	pr_info("r16:0x%08lx   r17: 0x%08lx   r18: 0x%08lx    r19: 0x%08lx\n",
	       fp->exregs[0], fp->exregs[1], fp->exregs[2], fp->exregs[3]);
	pr_info("r20 0x%08lx   r21: 0x%08lx   r22: 0x%08lx    r23: 0x%08lx\n",
	       fp->exregs[4], fp->exregs[5], fp->exregs[6], fp->exregs[7]);
	pr_info("r24 0x%08lx   r25: 0x%08lx   r26: 0x%08lx    r27: 0x%08lx\n",
	       fp->exregs[8], fp->exregs[9], fp->exregs[10], fp->exregs[11]);
	pr_info("r28 0x%08lx   r29: 0x%08lx   r30: 0x%08lx    r31: 0x%08lx\n",
	       fp->exregs[12], fp->exregs[13], fp->exregs[14], fp->exregs[15]);
	pr_info("hi 0x%08lx     lo: 0x%08lx \n",
	       fp->rhi, fp->rlo);
#else
	pr_info("r6: 0x%08lx   r7: 0x%08lx   r8: 0x%08lx   r9: 0x%08lx\n",
	       fp->regs[0], fp->regs[1], fp->regs[2], fp->regs[3]);
	pr_info("r10: 0x%08lx   r11: 0x%08lx   r12: 0x%08lx   r13: 0x%08lx\n",
	       fp->regs[4], fp->regs[5], fp->regs[6], fp->regs[7]);
	pr_info("r14 0x%08lx   r1: 0x%08lx   r15: 0x%08lx\n",
	       fp->regs[8], fp->regs[9], fp->r15);
#endif

	pr_info("\nCODE:");
	tp = ((unsigned char *) fp->pc) - 0x20;
	tp += ((int)tp % 4) ? 2 : 0;
	for (sp = (unsigned long *) tp, i = 0; (i < 0x40);  i += 4) {
		if ((i % 0x10) == 0)
			pr_cont("\n%08x: ", (int) (tp + i));
		pr_cont("%08x ", (int) *sp++);
	}
	pr_cont("\n");

	pr_info("\nKERNEL STACK:");
	tp = ((unsigned char *) fp) - 0x40;
	for (sp = (unsigned long *) tp, i = 0; (i < 0xc0); i += 4) {
		if ((i % 0x10) == 0)
			pr_cont("\n%08x: ", (int) (tp + i));
		pr_cont("%08x ", (int) *sp++);
	}
	pr_cont("\n");

	return;
}

