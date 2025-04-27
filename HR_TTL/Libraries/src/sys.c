#include "sys.h"

void WFI_SET(void) { __ASM volatile("wfi"); }
// 关闭所有中断
void INTX_DISABLE(void) { __ASM volatile("cpsid i"); }
// 开启所有中断
void INTX_ENABLE(void) { __ASM volatile("cpsie i"); }
// 设置栈顶地址

// this for arm-cc
// addr:栈顶地址
//__asm void MSR_MSP(u32 addr) {
// MSR MSP, r0 // set Main Stack value
//             BX r14
//}

// this for arm-none-eabi-gcc
void MSR_MSP(uint32_t addr) {
  __asm volatile("MSR MSP, %0   \n" /* 将函数参数 addr 写入 MSP 寄存器 */
                 "BX LR         \n"
                 :
                 : "r"(addr));
}
