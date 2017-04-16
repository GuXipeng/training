/*
 *  TOPPERS/SafeG Dual-OS monitor
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Safety Gate Dual-OS monitor
 *
 *  Copyright (C) 2009-2011 by Embedded and Real-Time Systems Laboratory
 *     Graduate School of Information Science, Nagoya University, JAPAN
 *
 *  上記著作権者は，以下の(1)〜(4)の条件を満たす場合に限り，本ソフトウェ
 *  ア（本ソフトウェアを改変したものを含む．以下同じ）を使用・複製・改
 *  変・再配布（以下，利用と呼ぶ）することを無償で許諾する．
 *  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
 *      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
 *      スコード中に含まれていること．
 *  (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
 *      用できる形で再配布する場合には，再配布に伴うドキュメント（利用
 *      者マニュアルなど）に，上記の著作権表示，この利用条件および下記
 *      の無保証規定を掲載すること．
 *  (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
 *      用できない形で再配布する場合には，次のいずれかの条件を満たすこ
 *      と．
 *    (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
 *        作権表示，この利用条件および下記の無保証規定を掲載すること．
 *    (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
 *        報告すること．
 *  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
 *      害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
 *      また，本ソフトウェアのユーザまたはエンドユーザからのいかなる理
 *      由に基づく請求からも，上記著作権者およびTOPPERSプロジェクトを
 *      免責すること．
 *
 *  本ソフトウェアは，無保証で提供されているものである．上記著作権者お
 *  よびTOPPERSプロジェクトは，本ソフトウェアに関して，特定の使用目的
 *  に対する適合性も含めて，いかなる保証も行わない．また，本ソフトウェ
 *  アの利用により直接的または間接的に生じたいかなる損害に関しても，そ
 *  の責任を負わない．
 *
 */
#ifndef _T_SAFEG_SYSCALLS_H_
#define _T_SAFEG_SYSCALLS_H_

#include "safeg_syscalls.h"

#ifndef __ASSEMBLY__

struct safeg_smc_args {
    void *arg0;
    void *arg1;
    void *arg2;
    void *arg3;
    void *arg4;
};

static inline
void safeg_switch_to_nt(volatile struct safeg_smc_args *ret_args)
{
    ret_args->arg0 = T_SMC_SWITCH_OK;
    while (1) {
        Asm("mov r0, %0\n\t"
            "mov r1, %1\n\t"
            "smc 0\n\t"
            : /* no output */
            : "I" (T_SMC_SWITCH), "r" (ret_args)
            : "r0", "r1", "memory");
        if (ret_args->arg0 != T_SMC_SWITCH_OK) return;
    }
}

struct safeg_fifo_msg {
    unsigned int id;        /* FIFO ID */
    unsigned char *buffer;  /* data buffer address */
    unsigned int bytes;     /* number of bytes to write/read */
    int rw_bytes;           /* result: written/read bytes */
};

static inline
void safeg_fifo_write(struct safeg_fifo_msg *msg)
{
    register unsigned long r0 asm("r0") = T_SMC_FIFO_WRITE;
    register unsigned long r1 asm("r1") = (unsigned long)msg;

    Asm("smc #0"
        : /* no output */
        : "r" (r0), "r" (r1)
        : "memory");
}

static inline
void safeg_fifo_read(struct safeg_fifo_msg *msg)
{
    register unsigned long r0 asm("r0") = T_SMC_FIFO_READ;
    register unsigned long r1 asm("r1") = (unsigned long)msg;

    Asm("smc #0"
        : /* no output */
        : "r" (r0), "r" (r1)
        : "memory");
}

static inline void safeg_reboot_nt(void)
{
    Asm("mov r0, %0\n\t"
        "smc 0\n\t"
        : /* no output */
        : "I" (T_SMC_REBOOT)
        : "r0", "memory");
}

Inline void
safeg_prof_init(void)
{
    Asm("mov r0, %0\n\t"
        "smc 0\n\t"
        : /* no output */
        : "I" (T_SMC_PROF_INIT)
        : "r0", "memory");
}

Inline void
safeg_prof_get(int cpuid,
                uint32_t *p_trust_int_cnt, uint32_t *p_nontrust_int_cnt, 
                uint32_t *p_trust_exc_cnt,  uint32_t *p_nontrust_exc_cnt)
{
    struct safeg_smc_args args;

    args.arg0 = cpuid;
    args.arg1 = p_trust_int_cnt;
    args.arg2 = p_nontrust_int_cnt;
    args.arg3 = p_trust_exc_cnt;
    args.arg4 = p_nontrust_exc_cnt;

    Asm("mov r0, %0\n\t"
        "mov r1, %1\n\t"
        "smc 0\n\t"
        : /* no output */
        : "I" (T_SMC_PROF_GET), "r" (&args)
        : "r0", "r1", "memory");
}

Inline void
safeg_prof_get_ptr(int cpuid,
                uint32_t *p_trust_int_cnt, uint32_t *p_nontrust_int_cnt, 
                uint32_t *p_trust_exc_cnt,  uint32_t *p_nontrust_exc_cnt)
{
    struct safeg_smc_args args;

    args.arg0 = cpuid;

    Asm("mov r0, %0\n\t"
        "mov r1, %1\n\t"
        "smc 0\n\t"
        : /* no output */
        : "I" (T_SMC_PROF_GET_PTR), "r" (&args)
        : "r0", "r1", "memory");

    p_trust_int_cnt = args.arg1;
    p_nontrust_int_cnt = args.arg2;
    p_trust_exc_cnt = args.arg3;
    p_nontrust_exc_cnt = args.arg4;
}

#endif /* __ASSEMBLY__ */

#endif /* _T_SAFEG_SYSCALLS_H_ */
