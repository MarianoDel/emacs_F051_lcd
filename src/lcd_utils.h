//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### LCD_UTILS.H #############################
//---------------------------------------------

#ifndef _LCD_UTILS_H_
#define _LCD_UTILS_H_

// Included for the Types ------------------------------------------------------
#include "hard.h"

// Modules needs Switches Functions for menus interaction ----------------------
// #define LCD_USE_SWITCHES_FOR_MENUES_FUNCTIONS
//necesita dos funciones de teclas para los menues
//checkS1()
//checkS2()

//-- Config Defines for the LCD used ----------
#define LINE_LENGTH    16
// #define LINE_LENGTH    8
#define USE_SCROLL_SECOND_LINE
#define USE_SCROLL_FIRST_LINE

#if (LINE_LENGTH == 8)
#define LINE_LENGTH_8
#endif

#if (LINE_LENGTH == 16)
#define LINE_LENGTH_16
#endif


//-- Config Defines for the functions used ----------
//for blinking in ms
#define TT_BLINKING_IN_ON    1000
#define TT_BLINKING_IN_OFF   600

//for show select in ms
#define TT_SHOW_SELECT_IN_ON    1000
#define TT_SHOW_SELECT_IN_OFF   500

//for func scroll in ms
#define TT_SCROLL    250


// Module Exported Types & Macros ----------------------------------------------
#define LCD_1ER_RENGLON Lcd_SetDDRAM(0x00)
#define LCD_2DO_RENGLON Lcd_SetDDRAM(0x40)
#define LCD_3ER_RENGLON Lcd_SetDDRAM(0x14)
#define LCD_4TO_RENGLON Lcd_SetDDRAM(0x54)

//estados de la funcion BLINKING
#define BLINKING_INIT       0
#define BLINKING_MARK_D     1
#define BLINKING_SPACE_D    2
#define BLINKING_MARK_C     3
#define BLINKING_SPACE_C    4
#define BLINKING_MARK_N     5
#define BLINKING_SPACE_N    6

//modos de blinking
#define BLINK_DIRECT    0
#define BLINK_CROSS     1
#define BLINK_NO        2

//respuestas de las funciones
#define RESP_CONTINUE		0
#define RESP_SELECTED		1
#define RESP_CHANGE			2
#define RESP_CHANGE_ALL_UP	3
#define RESP_WORKING		4

#define RESP_FINISH			10
#define RESP_YES			11
#define RESP_NO				12
#define RESP_NO_CHANGE		13

#define RESP_OK				20
#define RESP_NOK			21
#define RESP_NO_ANSWER		22
#define RESP_TIMEOUT		23
#define RESP_READY			24



//estados de la funcion SHOW SELECT
#define SHOW_SELECT_INIT				0
#define SHOW_SELECT_1					1
#define SHOW_SELECT_2					2
#define SHOW_SELECT_3					3
#define SHOW_SELECT_SELECTED			4
#define SHOW_SELECT_SELECTED_1			5
#define SHOW_SELECT_CHANGE				6
#define SHOW_SELECT_CHANGE_1			7

//estados de la funcion OPTIONS
#define OPTIONS_INIT					0
#define OPTIONS_WAIT_SELECT			1
#define OPTIONS_WAIT_SELECT_1			2
#define OPTIONS_WAIT_SELECT_2			3
#define OPTIONS_WAIT_SELECT_3			4
#define OPTIONS_WAIT_SELECT_TIMEOUT        5

#define OPTIONS_CHANGE_SELECT	10

//estados de la funcion OPTIONS ON OFF
#define OPTIONS_ONOFF_INIT          0
#define OPTIONS_ONOFF_REDRAW        1
#define OPTIONS_ONOFF_WAIT_IN_ON    2
#define OPTIONS_ONOFF_WAIT_IN_OFF   3
#define OPTIONS_ONOFF_SELECT_OPTION    4
#define OPTIONS_ONOFF_WAIT_FREE_S1    5
#define OPTIONS_ONOFF_CHANGE_OPTION    6


//estados de la funcion SCROLL
#define SCROLL_INIT			0
#define SCROLL_SENDING			1
#define SCROLL_FINISH			2

//estados de la funcion CHANGE
#define CHANGE_INIT					0
#define CHANGE_WAIT_SELECT			1


//modos de change
#define CHANGE_PERCENT    0
#define CHANGE_SECS		  1
#define CHANGE_CHANNELS	  2
#define CHANGE_PROGRAMS    3
#define CHANGE_VOLTAGE_MAINS    4
#define CHANGE_VOLTAGE_1        5
#define CHANGE_VOLTAGE_2        6
#define CHANGE_VOLTAGE_3        7
#define CHANGE_VOLTAGE_4        8
#define CHANGE_VOLTAGE_5        9
#define CHANGE_VOLTAGE_6        10

#define CHANGE_RESET	  0x80

//wrapers de la funcion FuncChange
#define FuncChangePercent(X)	FuncChange(X, CHANGE_PERCENT, 0, 100)
#define FuncChangeSecs(X)		FuncChange(X, CHANGE_SECS, 0, 10)
#define FuncChangeSecsMove(X)		FuncChange(X, CHANGE_SECS, 30, 120)
#define FuncChangeChannels(X)	FuncChange(X, CHANGE_CHANNELS, 1, 511)
#define FuncChangeChannelsQuantity(X)	FuncChange(X, CHANGE_CHANNELS, 1, 6)
#define FuncChangePercentReset()	FuncChangeReset()
#define FuncChangeSecsReset()	FuncChangeReset()
#define FuncChangeChannelsReset()	FuncChangeReset()
#define FuncChangePrograms(X)	FuncChange(X, CHANGE_PROGRAMS, 1, 9)
#define FuncChangeProgramsSequence(X)	FuncChange(X, CHANGE_PROGRAMS, 0, 9)


// Module Exported Functions ---------------------------------------------------
void LCD_UtilsInit (void);
void LCD_UpdateTimer (void);
void LCD_ClearScreen (void);
resp_t LCD_ShowBlink (const char * , const char * , unsigned char, unsigned char);
resp_t LCD_Scroll1 (const char *);
resp_t LCD_Scroll2 (const char *);
void LCD_Writel1 (char * l1);
void LCD_Writel2 (char * l2);
    

resp_t FuncShowSelect (const char *);
resp_t FuncShowSelectv2 (const char *);
resp_t FuncOptions (const char *, const char *, unsigned char *, unsigned char, unsigned char);
resp_t FuncOptionsOnOff (unsigned char *);

//unsigned char FuncChange (unsigned char *);
unsigned char FuncChange (unsigned short *, unsigned char, unsigned short, unsigned short);
unsigned char FuncChangeOnOff (unsigned char *);

void FuncOptionsReset (void);
void FuncShowSelectv2Reset (void);
void FuncChangeReset (void);



unsigned char FuncChangeDecimals (unsigned char *, unsigned char *, 
                                  unsigned char , unsigned char ,    
                                  unsigned char , unsigned char);    


#endif    /* MAIN_MENU_H_ */
