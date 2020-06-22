//--------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### UART.C ###################
//--------------------------------

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UART_H_
#define __UART_H_

//--- Exported types ---//
//--- Exported constants ---//
//--- Exported macro ---//
#define USART1_CLK (RCC->APB2ENR & 0x00004000)
#define USART1_CLK_ON RCC->APB2ENR |= 0x00004000
#define USART1_CLK_OFF RCC->APB2ENR &= ~0x00004000

#define USART2_CLK (RCC->APB1ENR & 0x00020000)
#define USART2_CLK_ON RCC->APB1ENR |= 0x00020000
#define USART2_CLK_OFF RCC->APB1ENR &= ~0x00020000

#define USART_9600		5000
#define USART_115200		416
#define USART_250000		192

#define USARTx_RX_DISA	USARTx->CR1 &= 0xfffffffb
#define USARTx_RX_ENA	USARTx->CR1 |= 0x04

#define TT_GSM_MINI		5

#define SIZEOF_DATA 256
// #define SIZEOF_TXDATA 128

//--- Exported functions ---//
unsigned char ReadUsart1Buffer (unsigned char *, unsigned short);
unsigned char ReadUsart2Buffer (unsigned char *, unsigned short);

void USART1Config(void);
void USART1_IRQHandler(void);
void Usart1Send (char *);
void Usart1SendUnsigned(unsigned char *, unsigned char);

void USART2Config(void);
void USART2_IRQHandler(void);
void Usart2Send (char *);
void Usart2SendUnsigned(unsigned char *, unsigned char);

#endif //--- End ---//


//--- END OF FILE ---//
