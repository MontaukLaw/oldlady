#include "stm8s.h"

#ifdef _RAISONANCE_
#define PUTCHAR_PROTOTYPE int putchar(char c)

#elif defined(_COSMIC_)
#define PUTCHAR_PROTOTYPE char putchar(char c)

#else /* _IAR_ */
#define PUTCHAR_PROTOTYPE int putchar(int c)

#endif

void main(void)
{

  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);

  UART1_DeInit();

  UART1_Init((uint32_t)115200, UART1_WORDLENGTH_8D, 
             UART1_STOPBITS_1, UART1_PARITY_NO,
             UART1_SYNCMODE_CLOCK_DISABLE, UART1_MODE_TXRX_ENABLE);
  
  printf("aa");
  //printf("\n\rUART1 Example :retarget the C library printf()/getchar() functions to the UART\n\r");
  //printf("\n\rEnter Text\n\r");
  while (1)
  {  
     printf("abcdef"); 
  }
}

PUTCHAR_PROTOTYPE
{
  /* Write a character to the UART1 */
  UART1_SendData8(c);
  /* Loop until the end of transmission */
  while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET)
    ;

  return (c);
}

/**
  * @brief Retargets the C library scanf function to the USART.
  * @param None
  * @retval char Character to Read
  */


void assert_failed(uint8_t *file, uint32_t line)
{
}