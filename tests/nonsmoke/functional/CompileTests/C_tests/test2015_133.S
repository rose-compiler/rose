 .macro disable_irq_notrace
 msr cpsr_c, #PSR_I_BIT | SVC_MODE
 .endm
 .macro enable_irq_notrace
 msr cpsr_c, #SVC_MODE
 .endm
 .macro asm_trace_hardirqs_off
 .endm
 .macro asm_trace_hardirqs_on_cond, cond
 .endm
 .macro asm_trace_hardirqs_on
 asm_trace_hardirqs_on_cond al
 .endm
 .macro disable_irq
 disable_irq_notrace
 asm_trace_hardirqs_off
 .endm
 .macro enable_irq
 asm_trace_hardirqs_on
 enable_irq_notrace
 .endm
 .macro save_and_disable_irqs, oldcpsr
 mrs \oldcpsr, cpsr
 disable_irq
 .endm
 .macro restore_irqs_notrace, oldcpsr
 msr cpsr_c, \oldcpsr
 .endm
 .macro restore_irqs, oldcpsr
 tst \oldcpsr, #PSR_I_BIT
 asm_trace_hardirqs_on_cond eq
 restore_irqs_notrace \oldcpsr
 .endm
 .macro instr_sync
 .endm
 .macro smp_dmb mode
 .endm
 .macro setmode, mode, reg
 msr cpsr_c, #\mode
 .endm
 .macro usracc, instr, reg, ptr, inc, cond, rept, abort, t=T()
 .rept \rept
9999:
 .if \inc == 1
 \instr\cond\()b\()\t \reg, [\ptr], #\inc
 .elseif \inc == 4
 \instr\cond\()\t \reg, [\ptr], #\inc
 .else
 .error "Unsupported inc macro argument"
 .endif
 .pushsection __ex_table,"a"
 .align 3
 .long 9999b, \abort
 .popsection
 .endr
 .endm
 .macro strusr, reg, ptr, inc, cond=al, rept=1, abort=9001f
 usracc str, \reg, \ptr, \inc, \cond, \rept, \abort
 .endm
 .macro ldrusr, reg, ptr, inc, cond=al, rept=1, abort=9001f
 usracc ldr, \reg, \ptr, \inc, \cond, \rept, \abort
 .endm
 .macro string name:req, string
 .type \name , #object
\name:
 .asciz "\string"
 .size \name , . - \name
 .endm
 .macro bitop, name, instr
.globl \name; .align 16,0x90; \name:

 ands ip, r1, #3
 strneb r1, [ip] @ assert word-aligned
 and r2, r0, #31
 mov r0, r0, lsr #5
 mov r3, #1
 mov r3, r3, lsl r2
 save_and_disable_irqs ip
 ldr r2, [r1, r0, lsl #2]
 \instr r2, r2, r3
 str r2, [r1, r0, lsl #2]
 restore_irqs ip
 mov pc, lr

ENDPROC(\name )
 .endm
 .macro testop, name, instr, store
.globl \name; .align 16,0x90; \name:

 ands ip, r1, #3
 strneb r1, [ip] @ assert word-aligned
 and r3, r0, #31
 mov r0, r0, lsr #5
 save_and_disable_irqs ip
 ldr r2, [r1, r0, lsl #2]!
 mov r0, #1
 tst r2, r0, lsl r3
 \instr r2, r2, r0, lsl r3
 \store r2, [r1]
 moveq r0, #0
 restore_irqs ip
 mov pc, lr

ENDPROC(\name )
 .endm
