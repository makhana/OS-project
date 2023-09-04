.extern _ZN5Riscv18handleSoftwareTrapEv

.align 4
.global _ZN5Riscv12softwareTrapEv
.type _ZN5Riscv12softwareTrapEv, @function
_ZN5Riscv12softwareTrapEv:

    // switch to SYSTEM stack
    sd t0, -1 * 8(sp)
    csrw sscratch, a0

    ld a0, _ZN3TCB7runningE
    ld t0, _ZN3TCB14systemSPOffsetE
    add a0, a0, t0

    mv t0, sp

    ld sp, 0(a0)
    sd t0, 0(a0)

    csrr a0, sscratch
    ld t0, -1 * 8(t0)

    # push all registers to stack
    addi sp, sp, -256
    .irp index, 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    sd x\index, \index * 8(sp)
    .endr

    call _ZN5Riscv18handleSoftwareTrapEv

    # pop all registers from stack
    .irp index, 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    ld x\index, \index * 8(sp)
    .endr
    addi sp, sp, 256

    // return to USER stack
    sd t0, -1 * 8(sp)
    csrw sscratch, a0

    ld a0, _ZN3TCB7runningE
    ld t0, _ZN3TCB14systemSPOffsetE
    add a0, a0, t0

    mv t0, sp

    ld sp, 0(a0)
    sd t0, 0(a0)

    csrr a0, sscratch
    ld t0, -1 * 8(t0)

    sret
