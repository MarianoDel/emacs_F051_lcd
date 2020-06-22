//----------------------------------------------------------
// #### PLACA RGBW CON MICRO STM32F051C8 - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### HARD.H #############################################
//----------------------------------------------------------

// Define to prevent recursive inclusion ---------------------------------------
#ifndef _HARD_H_
#define _HARD_H_


//----------- Defines For Configuration -------------
//----------- Hardware Board Version -------------
#define VER_1_0		//Planolux llc rgbw ver 1.0

//-------- Type of Program ----------------

//-------- End Of Defines For Configuration ------


//GPIOA pin0
//GPIOA pin1
//GPIOA pin2
//GPIOA pin3    LCD_D7 - LCD_D4

//GPIOA pin4
#define LCD_E    ((GPIOA->ODR & 0x0010) != 0)
#define LCD_E_ON    (GPIOA->BSRR = 0x00000010)
#define LCD_E_OFF    (GPIOA->BSRR = 0x00100000)

//GPIOA pin5
//GPIOA pin6
//GPIOA pin7

//GPIOB pin0 
//GPIOB pin1

//GPIOB pin2
#define LCD_RS    ((GPIOB->ODR & 0x0004) != 0)
#define LCD_RS_ON    (GPIOB->BSRR = 0x00000004)
#define LCD_RS_OFF    (GPIOB->BSRR = 0x00040000)

//GPIOB pin10
//GPIOB pin11
//GPIOB pin12
//GPIOB pin13

//GPIOB pin14
#define LED    ((GPIOB->ODR & 0x4000) != 0)
#define LED_ON    (GPIOB->BSRR = 0x00004000)
#define LED_OFF    (GPIOB->BSRR = 0x40000000)

//GPIOB pin15

//GPIOA pin8
//GPIOA pin9
//GPIOA pin10
//GPIOA pin11
//GPIOA pin12
//GPIOA pin13

//GPIOF pin6
//GPIOF pin7

//GPIOA pin14
//GPIOA pin15

//GPIOB pin3

//GPIOB pin4
#define CTRL_BKL    ((GPIOB->ODR & 0x0010) != 0)
#define CTRL_BKL_ON    (GPIOB->BSRR = 0x00000010)
#define CTRL_BKL_OFF    (GPIOB->BSRR = 0x00100000)

//GPIOB pin5
//GPIOB pin6
//GPIOB pin7
//GPIOB pin8
//GPIOB pin9



// Exported Constants  ---------------------------------------------------------
//Estados Externos de LED BLINKING
#define LED_NO_BLINKING    0
#define LED_BLINK_1    1
#define LED_BLINK_2    2
#define LED_BLINK_3    3
#define LED_BLINK_4    4


// Exported Types --------------------------------------------------------------
// typedef enum
// {
//     main_init = 0,
//     main_wait_for_gsm_network,
//     main_ready,
//     main_enable_output

// } main_state_t;

typedef enum {
    resp_ok = 0,
    resp_continue,
    resp_selected,
    resp_change,
    resp_change_all_up,
    resp_working,
    resp_error,
    resp_need_to_save,
    resp_finish,
    resp_nok,
    resp_timeout,
    resp_ready,
    resp_no_answer,
    resp_save

} resp_t;


//--- Stringtify Utils -----------------------
#define str_macro(s) #s


// Module Exported Functions ---------------------------------------------------
void ChangeLed (unsigned char);
void UpdateLed (void);


#endif /* _HARD_H_ */

//--- end of file ---//

