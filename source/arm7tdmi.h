#include "include.h"

char* int_memory[32 * 1024]; // 32kb 
char* ext_memory[256 * 1024]; // 256kb
char* vRAM_memory[96 * 1024]; // 96kb

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

enum SIGNAL {
    ON,
    OFF
};

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
    MODE1
};

void setFlag(FLAGS flag);
// Register set arm

u32 R0, R1, R2, R3, R4, R5, R6, R7, R8, R9, R10, R11, R12  = 0x00000000; // General purpose registers
u32 R13                                                    = 0x00000000; // This register is used as Stack Pointer (SP) in THUMB state. While in ARM state the user may decided to use R13 and/or other register(s) as stack pointer(s), or as general purpose register.
u32 R14                                                    = 0x00000000; // This register is used as Link Register (LR). That is, when calling to a sub-routine by a Branch with Link (BL) instruction, then the return address (ie. old value of PC) is saved in this register.
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

class ARMCPU {
public:    
    
    void setFlag(FLAGS flag);
    void forceStateChange(STATE state);
    
};