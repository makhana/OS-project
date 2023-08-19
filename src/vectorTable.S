.extern _ZN5Riscv20handleSupervisorTrapEiPv

.align 4
.global _ZN5Riscv11vectorTableEv
.type _ZN5Riscv11vectorTableEv, @function
_ZN5Riscv11vectorTableEv:
    j _ZN5Riscv14supervisorTrapEv
    j _ZN5Riscv12softwareTrapEv
    mv zero, zero
    mv zero, zero
    mv zero, zero
    mv zero, zero
    mv zero, zero
    mv zero, zero
    mv zero, zero
    j _ZN5Riscv12externalTrapEv

