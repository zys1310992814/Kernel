/*
 * Testcae main entry
 *
 * (C) BiscuitOS 2017.10 <buddy.zhang@aliyun.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/kernel.h>
#include <test/debug.h>

#ifdef CONFIG_DEBUG_KERNEL_LATER
/*
 * debug on kernel last before userland launch.
 */
void debug_on_kernel_later(void)
{
#ifdef CONFIG_DEBUG_INTERRUPT
    debug_interrupt_common();
#endif

#ifdef CONFIG_DEBUG_SCHED
    debug_task_scheduler_common();
#endif

#ifdef CONFIG_DEBUG_MMU
    debug_mmu_common();
#endif

#ifdef CONFIG_DEBUG_SYSCALL
    debug_syscall_common();
#endif

#ifdef CONFIG_DEBUG_VFS
    debug_vfs_common();
#endif

#ifdef CONFIG_DEBUG_BLOCK_DEV
    debug_block_common();
#endif

}
#endif

#ifdef CONFIG_DEBUG_KERNEL_EARLY
/*
 * debug on kernel early
 */
void debug_on_kernel_early(void)
{

}
#endif

#ifdef CONFIG_DEBUG_USERLAND_SHELL
/*
 * debug on shell stage
 */
void debug_on_shell_stage(void)
{

}
#endif

#if defined (CONFIG_DEBUG_USERLAND_EARLY) || \
    defined (CONFIG_DEBUG_USERLAND_SHELL)
/* Debug kernel on Userland Stage */
void debug_kernel_on_userland_stage(void)
{
#ifdef CONFIG_TESTCASE_SCHED
    debug_scheduler_kernel_on_userland();
#endif
}
#endif
