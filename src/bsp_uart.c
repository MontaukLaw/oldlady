#include "user_comm.h"

// ���ڳ�ʼ��
void UartInit(void)
{
  // dbg�ô�������
  UART1_DeInit();

  // ��ʼ������1
  UART1_Init((uint32_t)115200,
             UART1_WORDLENGTH_8D,
             UART1_STOPBITS_1,
             UART1_PARITY_NO,
             UART1_SYNCMODE_CLOCK_DISABLE,
             UART1_MODE_TXRX_ENABLE);

  // ���ô���1
  UART1_Cmd(ENABLE);

}

