#include "user_comm.h"

// 串口初始化
void UartInit(void)
{
  // dbg用串口设置
  UART1_DeInit();

  // 初始化串口1
  UART1_Init((uint32_t)115200,
             UART1_WORDLENGTH_8D,
             UART1_STOPBITS_1,
             UART1_PARITY_NO,
             UART1_SYNCMODE_CLOCK_DISABLE,
             UART1_MODE_TXRX_ENABLE);

  // 启用串口1
  UART1_Cmd(ENABLE);

}

