#ifndef ARM7TDMI_INCLUDE
#define ARM7TDMI_INCLUDE

#include "include.h"

uint cycles;

char* memory[288 * 1024]; 

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

enum STATE {
    ARM,
    THUMB
};

enum FLAGS {
    // Condition flags
    SIGN_FLAG, // (0=Not Signed, 1=Signed) 
    ZERO_FLAG, // (0=Not Zero, 1=Zero)     
    CARRY_FLAG, // (0=Borrow/No Carry, 1=Carry/No Borrow)
    OVERFLOW_FLAG, // (0=No Overflow, 1=Overflow)
    // Controls bits
    STICKY_FLAG, // (1=Sticky Overflow, ARMv5TE and up only)
    IRQ_DISABLE, // (0=Enable, 1=Disable)
    FIQ_DISABLE, // (0=Enable, 1=Disable
    STATE_BIT, // (0=ARM, 1=THUMB)
    // Mode bits
    MODE4,
    MODE3,
    MODE2,
    MODE1,
    MODE0
};

enum EXCEPTIONS {

    RESET,
    UND_INSTRUCTION,
    SOFTWARE_INT,
    PREFECTH_ABORT,
    DATA_ABORT,
    ADDR_EXC26,
    NORMAL_INT,
    FAST_INT
};

/*

Address  Prio  Exception                  Mode on Entry      Interrupt Flags
  
  BASE+00h 1     Reset                      Supervisor (_svc)  I=1, F=1
  BASE+04h 7     Undefined Instruction      Undefined  (_und)  I=1, F=unchanged
  BASE+08h 6     Software Interrupt (SWI)   Supervisor (_svc)  I=1, F=unchanged
  BASE+0Ch 5     Prefetch Abort             Abort      (_abt)  I=1, F=unchanged
  BASE+10h 2     Data Abort                 Abort      (_abt)  I=1, F=unchanged
  BASE+14h ??    Address Exceeds 26bit      Supervisor (_svc)  I=1, F=unchanged
  BASE+18h 4     Normal Interrupt (IRQ)     IRQ        (_irq)  I=1, F=unchanged
  BASE+1Ch 3     Fast Interrupt (FIQ)       FIQ        (_fiq)  I=1, F=1

*/

// Register set arm

u32 R0, R1, R2, R3, R4, R5, R6, R7, R8, R9, R10, R11, R12  = 0x00000000; // General purpose registers
u32 R8_FIQ, R9_FIQ, R10_FIQ, R11_FIQ, R12_FIQ              = 0x00000000; // FIQ registers
u32 R13, R13_FIQ, R13_SVC, R13_ABT, R13_IRQ, R13_UND       = 0x00000000; // This register is used as Stack Pointer (SP) in THUMB state. While in ARM state the user may decided to use R13 and/or other register(s) as stack pointer(s), or as general purpose register.
u32 R14, R14_FIQ, R14_SVC, R14_ABT, R14_IRQ, R14_UND       = 0x00000000; // This register is used as Link Register (LR). That is, when calling to a sub-routine by a Branch with Link (BL) instruction, then the return address (ie. old value of PC) is saved in this register.
u32 R15_PC                                                 = 0x00000000; // R15 is always used as program counter (PC). Note that when reading R15, this will usually return a value of PC+nn because of read-ahead (pipelining), whereas 'nn' depends on the instruction and on the CPU state (ARM or THUMB).
u32 CPSR                                                   = 0x00000000; // The current condition codes (flags) and CPU control bits are stored in the CPSR register.

/* Current Program Status Register (CPSR)

Bit   Expl.
31    N - Sign Flag       (0=Not Signed, 1=Signed)                  ;\
30    Z - Zero Flag       (0=Not Zero, 1=Zero)                      ; Condition
29    C - Carry Flag      (0=Borrow/No Carry, 1=Carry/No Borrow)    ; Code Flags
28    V - Overflow Flag   (0=No Overflow, 1=Overflow)               ;/
27    Q - Sticky Overflow (1=Sticky Overflow, ARMv5TE and up only)
26-8  Reserved            (For future use) - Do not change manually!
7     I - IRQ disable     (0=Enable, 1=Disable)                         ;\
6     F - FIQ disable     (0=Enable, 1=Disable)                         ; Control
5     T - State Bit       (0=ARM, 1=THUMB) - Do not change manually!    ; Bits
4-0   M4-M0 - Mode Bits   (See below)                                   ;/

*/

u32 SPSR_FIQ = 0x00000000; // Fast Interrupt request
u32 SPSR_SVC = 0x00000000; // Supervisor
u32 SPSR_ABT = 0x00000000; // Abort
u32 SPSR_IRQ = 0x00000000; // Interrupt request
u32 SPSR_UND = 0x00000000; // Undefined 

/* Saved Program Status Registers (SPSR_<mode>)

Whenever the CPU enters an exception, the current status register (CPSR) is copied to the respective 
SPSR_<mode> register. Note that there is only one SPSR for each mode, so nested exceptions inside of 
the same mode are allowed only if the exception handler saves the content of SPSR in memory.

For example, for an IRQ exception: IRQ-mode is entered, and CPSR is copied to SPSR_irq. 
If the interrupt handler wants to enable nested IRQs, then it must first push SPSR_irq before doing so.

*/ 

void ADC();     // Add with carry 
void ADD();     // Add
void AND();     // AND
void B();       // Branch
void BIC();     // Bit Clear
void BL();      // Branch with Link 
void BX();      // Branch and Exchange
void CDP();     // Coprocesor Data Processing
void CMN();     // Compare Negative 
void CMP();     // Compare
void EOR();     // Exclusive OR
void LDC();     // Load coprocessor from memory
void LDM();     // Load multiple registers
void LDR();     // Load register from memory
void MCR();     // Move CPU register to coprocessor registe
void MLA();     // Multiply Accumulate 
void MOV();     // Move register or constant 
void MRC();     // Move from coprocessor register to CPU register
void MRS();     // Move PSR status/flags to register
void MSR();     // Move register to PSR status/flags
void MUL();     // Multiply 
void MVN();     // Move negative register
void ORR();     // OR
void RSB();     // Reverse Subtract 
void RSC();     // Reverse Subtract with Carry
void SBC();     // Subtract with Carry
void STC();     // Store coprocessor register to memory
void STM();     // Store Multiple
void STR();     // Store register to memory 
void SUB();     // Subtract 
void SWI();     // Software Interrupt
void SWP();     // Swap register with memory
void TEQ();     // Test bitwise equality
void TST();     // Test bits



class ARMCPU {
public:    
    
    void setFlag(FLAGS flag, int flag_state);
    void forceStateChange(STATE state);
    
    // Memory management
    void getDataforMem(u32 addr);
    void putDatainMem(u32 addr, u32 data);

    void reset();
    void irq();
    void fiq();
    void abort();
    void changeMode(u32 Mode);
    void ariseException(EXCEPTIONS exc);
};

#endif