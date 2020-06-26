//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### LCD_UTILS.C #############################
//---------------------------------------------

/* Includes ------------------------------------------------------------------*/
#include "lcd_utils.h"
#include "hard.h"
#include "stm32f0xx.h"
#include "tim.h"
#include "lcd.h"

#include <string.h>
#include <stdio.h>


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------
#ifdef LINE_LENGTH_8
const char s_blank [] = {"        "};
const unsigned char s_sel_up_down [] = { 0x02, 0x05, 0x07 };
#endif

#ifdef LINE_LENGTH_16
const char s_blank [] = {"                "};
const unsigned char s_sel_up_down [] = { 0x02, 0x08, 0x0f };
#endif

//funcion blinking
unsigned char blinking_state = 0;
unsigned char blinking_how_many = 0;

//funcion show select
unsigned char show_select_state = 0;

//funcion options
unsigned char options_state = 0;
unsigned char options_curr_sel = 0;
unsigned char options_mark = 0;


//funcion options y options_decimals
unsigned char change_state = 0;
unsigned short change_current_val = 0;
unsigned char change_last_option = 0;

#define change_current_val_int change_current_val
unsigned short change_current_val_dec = 0;

//funcion scroll renglon 1
#ifdef USE_SCROLL_FIRST_LINE
unsigned char scroll1_state = 0;
unsigned char scroll1_last_window = 0;
unsigned char scroll1_current_window = 0;

volatile unsigned short scroll1_timer = 0;
#endif

//funcion scroll renglon 2
#ifdef USE_SCROLL_SECOND_LINE
unsigned char scroll2_state = 0;
unsigned char scroll2_last_window = 0;
unsigned char scroll2_current_window = 0;

volatile unsigned short scroll2_timer = 0;
#endif

volatile unsigned short show_select_timer = 0;





//-- Private Defines -----------------

        

// Module Functions ------------------------------------------------------------
///////////////////////////////////////////////////////
// We dont need Switches for the following functions //
///////////////////////////////////////////////////////
void LCD_UtilsInit (void)
{
    LCDInit();

    Lcd_Command(CLEAR);
    Wait_ms(100);
    Lcd_Command(CURSOR_OFF);
    Wait_ms(100);
    Lcd_Command(BLINK_OFF);
    Wait_ms(100);    
    CTRL_BKL_ON;
}


void LCD_UpdateTimer (void)
{
    if (show_select_timer)
        show_select_timer--;

#ifdef USE_SCROLL_FIRST_LINE
    if (scroll1_timer)
        scroll1_timer--;
#endif

#ifdef USE_SCROLL_SECOND_LINE
    if (scroll2_timer)
        scroll2_timer--;
#endif
}


void LCD_ClearScreen (void)
{
    LCD_1ER_RENGLON;
    Lcd_TransmitStr((const char *)s_blank);
    LCD_2DO_RENGLON;
    Lcd_TransmitStr((const char *)s_blank);
}


void LCD_Writel1 (char * l1)
{
    LCD_1ER_RENGLON;
    Lcd_TransmitStr(l1);
}


void LCD_Writel2 (char * l2)
{
    LCD_2DO_RENGLON;
    Lcd_TransmitStr(l2);
}



//funcion que muestra los string de renglon 1 y renglon 2
//recibe puntero a primer renglon
//puntero a segundo renglon
//how many cantidad de pantallas en blinking
//modo BLINK_DIRECT o BLINK_CROSS
//si modo es BLINK_NO en how_many me pasan los segundos antes de terminar
resp_t LCD_ShowBlink (const char * p_text1, const char * p_text2, unsigned char how_many, unsigned char mode)
{
    resp_t resp = resp_continue;

    switch (blinking_state)
    {
    case BLINKING_INIT:
        if (mode == BLINK_DIRECT)
            blinking_state = BLINKING_MARK_D;
        else if (mode == BLINK_CROSS)
            blinking_state = BLINKING_MARK_C;
        else
            blinking_state = BLINKING_MARK_N;

        blinking_how_many = how_many;
        show_select_timer = 0;
        break;

    case BLINKING_MARK_D:
        if (!show_select_timer)
        {
            if (blinking_how_many)
            {
                blinking_how_many--;
                LCD_1ER_RENGLON;
                Lcd_TransmitStr(p_text1);
                LCD_2DO_RENGLON;
                Lcd_TransmitStr(p_text2);
                show_select_timer = TT_BLINKING_IN_ON;
                blinking_state = BLINKING_SPACE_D;
            }
            else
            {
                blinking_state = BLINKING_INIT;
                resp = resp_finish;
            }
        }
        break;

    case BLINKING_SPACE_D:
        if (!show_select_timer)
        {
            LCD_1ER_RENGLON;
            Lcd_TransmitStr((const char *) s_blank);
            LCD_2DO_RENGLON;
            Lcd_TransmitStr((const char *) s_blank);
            show_select_timer = TT_BLINKING_IN_OFF;
            blinking_state = BLINKING_MARK_D;
        }
        break;

    case BLINKING_MARK_C:
        if (!show_select_timer)
        {
            if (blinking_how_many)
            {
                blinking_how_many--;
                LCD_1ER_RENGLON;
                Lcd_TransmitStr(p_text1);
                LCD_2DO_RENGLON;
                Lcd_TransmitStr((const char *) s_blank);
                show_select_timer = TT_BLINKING_IN_ON;
                blinking_state = BLINKING_SPACE_C;
            }
            else
            {
                LCD_1ER_RENGLON;
                Lcd_TransmitStr((const char *) s_blank);
                blinking_state = BLINKING_INIT;
                resp = resp_finish;
            }
        }
        break;

    case BLINKING_SPACE_C:
        if (!show_select_timer)
        {
            LCD_1ER_RENGLON;
            Lcd_TransmitStr((const char *) s_blank);
            LCD_2DO_RENGLON;
            Lcd_TransmitStr(p_text2);
            show_select_timer = TT_BLINKING_IN_ON;
            blinking_state = BLINKING_MARK_C;
        }
        break;

    case BLINKING_MARK_N:
        LCD_1ER_RENGLON;
        Lcd_TransmitStr(p_text1);
        LCD_2DO_RENGLON;
        Lcd_TransmitStr(p_text2);
        show_select_timer = TT_BLINKING_IN_ON * how_many;
        blinking_state = BLINKING_SPACE_N;
        break;

    case BLINKING_SPACE_N:
        if (!show_select_timer)
        {
            blinking_state = BLINKING_INIT;
            resp = resp_finish;
        }
        break;

    default:
        blinking_state = BLINKING_INIT;
        resp = resp_finish;
        break;
    }
    return resp;
}


void LCD_Scroll1Reset (void)
{
    scroll1_state = SCROLL_INIT;
}


//hace un scroll en el primer renglon del lcd
//recibe un puntero al string
//devuelve resp_continue o resp_finish
resp_t LCD_Scroll1 (const char * p_text)
{
    resp_t resp = resp_continue;
    unsigned char last_window;
    unsigned char i;

    switch (scroll1_state)
    {
    case SCROLL_INIT:
        scroll1_last_window = strlen(p_text) + (2 * LINE_LENGTH);
        scroll1_current_window = 1;
        scroll1_state++;
        break;

    case SCROLL_SENDING:
        if (!scroll1_timer)
        {
            last_window = scroll1_current_window + LINE_LENGTH;
            LCD_1ER_RENGLON;

            for (i = scroll1_current_window; i < last_window; i++)
            {
                if (i < LINE_LENGTH)
                    Lcd_senddata(' ');
                else if (i < (scroll1_last_window - LINE_LENGTH))
                    Lcd_senddata(*(p_text + (i - LINE_LENGTH)));
                else if (i < scroll1_last_window)
                    Lcd_senddata(' ');
                else
                {
                    //termine de enviar
                    i = last_window;
                    scroll1_state++;
                }
            }
            scroll1_current_window++;
            scroll1_timer = TT_SCROLL;
        }
        break;

    case SCROLL_FINISH:
        resp = resp_finish;
        scroll1_state = SCROLL_INIT;
        break;

    default:
        resp = resp_finish;
        scroll1_state = SCROLL_INIT;
        break;
    }

    return resp;
}


void LCD_Scroll2Reset (void)
{
    scroll2_state = SCROLL_INIT;
}


//hace un scroll en el segundo renglon del lcd
//recibe un puntero al string
//devuelve resp_continue o resp_finish
resp_t LCD_Scroll2 (const char * p_text)
{
    resp_t resp = resp_continue;
    unsigned char last_window;
    unsigned char i;

    switch (scroll2_state)
    {
    case SCROLL_INIT:
        scroll2_last_window = strlen(p_text) + (2 * LINE_LENGTH);
        scroll2_current_window = 1;
        scroll2_state++;
        break;

    case SCROLL_SENDING:
        if (!scroll2_timer)
        {
            last_window = scroll2_current_window + LINE_LENGTH;
            LCD_2DO_RENGLON;

            for (i = scroll2_current_window; i < last_window; i++)
            {
                if (i < LINE_LENGTH)
                    Lcd_senddata(' ');
                else if (i < (scroll2_last_window - LINE_LENGTH))
                    Lcd_senddata(*(p_text + (i - LINE_LENGTH)));
                else if (i < scroll2_last_window)
                    Lcd_senddata(' ');
                else
                {
                    //termine de enviar
                    i = last_window;
                    scroll2_state++;
                }
            }
            scroll2_current_window++;
            scroll2_timer = TT_SCROLL;
        }
        break;

    case SCROLL_FINISH:
        resp = resp_finish;
        scroll2_state = SCROLL_INIT;
        break;

    default:
        resp = resp_finish;
        scroll2_state = SCROLL_INIT;
        break;
    }

    return resp;
}

#define CGRAM_POS0    0x00
#define CGRAM_POS1    0x08
#define CGRAM_POS2    0x10
#define CGRAM_POS3    0x18
#define CGRAM_POS4    0x20
#define CGRAM_POS5    0x28
#define CGRAM_POS6    0x30
#define CGRAM_POS7    0x38

unsigned char v_cgram0 [8] = {0x07, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x07 };
unsigned char v_cgram1 [8] = {0x1f, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
unsigned char v_cgram2 [8] = {0x1c, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1c };
unsigned char v_cgram3 [8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x1f };
unsigned char v_cgram4 [8] = {0x1f, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x1f };
unsigned char v_cgram5 [8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x07 };
unsigned char v_cgram6 [8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x1c };

void LCD_BigNumbersInit (void)
{
    unsigned char i;
    //inicializo el LCD para manejar numeros grandes
    Lcd_SetCGRAM(CGRAM_POS0);
    for (i = 0; i < 8; i++)
        Lcd_senddata(v_cgram0[i]);

    Lcd_SetCGRAM(CGRAM_POS1);
    for (i = 0; i < 8; i++)
        Lcd_senddata(v_cgram1[i]);

    Lcd_SetCGRAM(CGRAM_POS2);
    for (i = 0; i < 8; i++)
        Lcd_senddata(v_cgram2[i]);

    Lcd_SetCGRAM(CGRAM_POS3);
    for (i = 0; i < 8; i++)
        Lcd_senddata(v_cgram3[i]);

    Lcd_SetCGRAM(CGRAM_POS4);
    for (i = 0; i < 8; i++)
        Lcd_senddata(v_cgram4[i]);

    Lcd_SetCGRAM(CGRAM_POS5);
    for (i = 0; i < 8; i++)
        Lcd_senddata(v_cgram5[i]);

    Lcd_SetCGRAM(CGRAM_POS6);
    for (i = 0; i < 8; i++)
        Lcd_senddata(v_cgram6[i]);
    
}


#define SEG_LAT_IZQ    0x00
#define SEG_BARRA_ALTA    0x01
#define SEG_LAT_DER    0x02
#define SEG_BARRA_BAJA    0x03
#define SEG_BARRA_ALTA_BAJA    0x04
#define SEG_CORTE_BAJO_IZQ    0x05
#define SEG_CORTE_BAJO_DER    0x06

#define SEG_EMPTY    ' '


void LCD_BigNumbers (unsigned char line1_position, unsigned char number)
{
    unsigned char line1 = line1_position;
    unsigned char line2 = 0x40 + line1_position;

    switch (number)
    {
    case 0:
        //usar despues TransmitArray
        Lcd_SetDDRAM(line1);
        Lcd_senddata(SEG_LAT_IZQ);
        Lcd_senddata(SEG_BARRA_ALTA);
        Lcd_senddata(SEG_LAT_DER);
        
        Lcd_SetDDRAM(line2);
        Lcd_senddata(SEG_LAT_IZQ);
        Lcd_senddata(SEG_BARRA_BAJA);        
        Lcd_senddata(SEG_LAT_DER);
        break;

    case 1:
        Lcd_SetDDRAM(line1);
        Lcd_senddata(SEG_EMPTY);
        Lcd_senddata(SEG_EMPTY);
        Lcd_senddata(SEG_LAT_DER);

        Lcd_SetDDRAM(line2);
        Lcd_senddata(SEG_EMPTY);
        Lcd_senddata(SEG_EMPTY);
        Lcd_senddata(SEG_LAT_DER);
        break;

    case 2:
        Lcd_SetDDRAM(line1);
        Lcd_senddata(SEG_CORTE_BAJO_IZQ);
        Lcd_senddata(SEG_BARRA_ALTA_BAJA);
        Lcd_senddata(SEG_LAT_DER);

        Lcd_SetDDRAM(line2);
        Lcd_senddata(SEG_LAT_IZQ);
        Lcd_senddata(SEG_BARRA_BAJA);
        Lcd_senddata(SEG_EMPTY);
        break;
        
    case 3:
        Lcd_SetDDRAM(line1);
        Lcd_senddata(SEG_EMPTY);
        Lcd_senddata(SEG_BARRA_ALTA_BAJA);
        Lcd_senddata(SEG_LAT_DER);
        Lcd_SetDDRAM(line2);
        Lcd_senddata(SEG_EMPTY);
        Lcd_senddata(SEG_BARRA_BAJA);
        Lcd_senddata(SEG_LAT_DER);                
        break;

    case 4:
        Lcd_SetDDRAM(line1);
        Lcd_senddata(SEG_LAT_IZQ);
        Lcd_senddata(SEG_BARRA_BAJA);
        Lcd_senddata(SEG_LAT_DER);
        Lcd_SetDDRAM(line2);
        Lcd_senddata(SEG_EMPTY);
        Lcd_senddata(SEG_EMPTY);
        Lcd_senddata(SEG_LAT_DER);
        break;

    case 5:
        Lcd_SetDDRAM(line1);
        Lcd_senddata(SEG_LAT_IZQ);
        Lcd_senddata(SEG_BARRA_ALTA_BAJA);
        Lcd_senddata(SEG_CORTE_BAJO_DER);
        Lcd_SetDDRAM(line2);
        Lcd_senddata(SEG_EMPTY);
        Lcd_senddata(SEG_BARRA_BAJA);
        Lcd_senddata(SEG_LAT_DER);                
        break;

    case 6:
        Lcd_SetDDRAM(line1);
        Lcd_senddata(SEG_LAT_IZQ);
        Lcd_senddata(SEG_BARRA_ALTA_BAJA);
        Lcd_senddata(SEG_CORTE_BAJO_DER);
        Lcd_SetDDRAM(line2);
        Lcd_senddata(SEG_LAT_IZQ);
        Lcd_senddata(SEG_BARRA_BAJA);
        Lcd_senddata(SEG_LAT_DER);
        break;

    case 7:
        Lcd_SetDDRAM(line1);
        Lcd_senddata(SEG_EMPTY);
        Lcd_senddata(SEG_BARRA_ALTA);
        Lcd_senddata(SEG_LAT_DER);
        Lcd_SetDDRAM(line2);
        Lcd_senddata(SEG_EMPTY);
        Lcd_senddata(SEG_EMPTY);
        Lcd_senddata(SEG_LAT_DER);                
        break;

    case 8:
        Lcd_SetDDRAM(line1);
        Lcd_senddata(SEG_LAT_IZQ);
        Lcd_senddata(SEG_BARRA_ALTA_BAJA);
        Lcd_senddata(SEG_LAT_DER);
        Lcd_SetDDRAM(line2);
        Lcd_senddata(SEG_LAT_IZQ);
        Lcd_senddata(SEG_BARRA_BAJA);
        Lcd_senddata(SEG_LAT_DER);
        break;

    case 9:
        Lcd_SetDDRAM(line1);
        Lcd_senddata(SEG_LAT_IZQ);
        Lcd_senddata(SEG_BARRA_ALTA_BAJA);
        Lcd_senddata(SEG_LAT_DER);
        Lcd_SetDDRAM(line2);
        Lcd_senddata(SEG_EMPTY);
        Lcd_senddata(SEG_EMPTY);
        Lcd_senddata(SEG_LAT_DER);
        break;
    }
}

//////////////////////////////////////////////////////////////////////////////////
// We need Switches Functions for the following functions CheckS1() & CheckS2() //
//////////////////////////////////////////////////////////////////////////////////

//funcion que muestra el string enviado en formato de menu
//ademas v2 agrega el cont o select en el segundo renglon
//respuestas
//resp_continue
//resp_selected
//resp_change
//resp_change_all_up
resp_t LCD_ShowSelectv2 (const char * p_text, sw_actions_t sw_action)
{
    resp_t resp = resp_continue;

    switch (show_select_state)
    {
    case SHOW_SELECT_INIT:
        LCD_2DO_RENGLON;
#ifdef LINE_LENGTH_8
        Lcd_TransmitStr((const char *) "Cnt Slct");
#endif
#ifdef LINE_LENGTH_16        
        // Lcd_TransmitStr((const char *) "Cont.     Select");
        Lcd_TransmitStr((const char *) "SET    or    < >");        
#endif        
        show_select_state++;
        break;

    case SHOW_SELECT_1:
        LCD_1ER_RENGLON;
        Lcd_TransmitStr(p_text);
        show_select_timer = TT_SHOW_SELECT_IN_ON;
        show_select_state++;
        break;

    case SHOW_SELECT_2:
        if (!show_select_timer)
        {
            LCD_1ER_RENGLON;
            Lcd_TransmitStr((const char *) s_blank);
            show_select_timer = TT_SHOW_SELECT_IN_OFF;
            show_select_state++;
        }

        // check switches actions
        if ((sw_action == selection_up) || (sw_action == selection_dwn))
            show_select_state = SHOW_SELECT_CHANGE;

        if (sw_action == selection_enter)
            show_select_state = SHOW_SELECT_SELECTED;

        break;

    case SHOW_SELECT_3:
        if (!show_select_timer)
        {
            show_select_state = SHOW_SELECT_1;
        }

        // check switches actions
        if ((sw_action == selection_up) || (sw_action == selection_dwn))
            show_select_state = SHOW_SELECT_CHANGE;

        if (sw_action == selection_enter)
            show_select_state = SHOW_SELECT_SELECTED;

        break;

    case SHOW_SELECT_SELECTED:
        LCD_1ER_RENGLON;
        Lcd_TransmitStr(p_text);
        LCD_2DO_RENGLON;
#ifdef LINE_LENGTH_8
        Lcd_TransmitStr((const char *) "Selected");
#endif
#ifdef LINE_LENGTH_16
        Lcd_TransmitStr((const char *) "Selected...     ");
#endif        
        show_select_state++;
        break;

    case SHOW_SELECT_SELECTED_1:
        if (sw_action == selection_none)
        {
            resp = resp_selected;
            show_select_state = SHOW_SELECT_INIT;
        }
        break;

    case SHOW_SELECT_CHANGE:
        LCD_1ER_RENGLON;
        Lcd_TransmitStr(p_text);
        LCD_2DO_RENGLON;
#ifdef LINE_LENGTH_8
        Lcd_TransmitStr((const char *) "Changing");
#endif
#ifdef LINE_LENGTH_16
        Lcd_TransmitStr((const char *) "Changing...     ");
#endif        
        show_select_state++;
        break;

    case SHOW_SELECT_CHANGE_1:
        if (sw_action == selection_none)
        {
            resp = resp_change;
            show_select_state = SHOW_SELECT_INIT;
        }
        break;

    default:
        show_select_state = SHOW_SELECT_INIT;
        break;
    }

    return resp;
}


//recibe:
// puntero al primer renglon de seleccion solo 12 caracteres
// puntero al valor OFF->0 ON->1
// acciones a realizar
//devuelve:
// resp_continue o resp_finish
// el valor de seleccion final en el puntero al valor
resp_t LCD_EncoderOptionsOnOff (char * primer_renglon,
                                unsigned char * bool_value,
                                sw_actions_t actions)
{
    resp_t resp = resp_continue;

    switch (options_state)
    {
    case OPTIONS_ONOFF_INIT:
        LCD_1ER_RENGLON;
        Lcd_TransmitStr(primer_renglon);
        LCD_2DO_RENGLON;
        Lcd_TransmitStr((const char *) "SET    or    < >");        

        options_curr_sel = *bool_value;
        show_select_timer = 0;        
        options_state = OPTIONS_ONOFF_REDRAW;
        break;

    case OPTIONS_ONOFF_REDRAW:
        Lcd_SetDDRAM(12);
        if (options_curr_sel)
            Lcd_TransmitStr(" ON ");
        else
            Lcd_TransmitStr(" OFF");

        //espero que liberen switch para avanzar
        if (actions == selection_none)    
        {
            show_select_timer = TT_SHOW_SELECT_IN_ON;
            options_state = OPTIONS_ONOFF_WAIT_IN_ON;
        }
        break;
        
    case OPTIONS_ONOFF_WAIT_IN_ON:
        if ((actions == selection_up) || (actions == selection_dwn))
        {
            options_state = OPTIONS_ONOFF_REDRAW;
            if (options_curr_sel)
                options_curr_sel = 0;
            else
                options_curr_sel = 1;
        }

        if (actions == selection_enter)
        {
            LCD_2DO_RENGLON;
            Lcd_TransmitStr((const char *) "Selected...     ");
            options_state = OPTIONS_ONOFF_SELECT_OPTION;
        }

        if (!show_select_timer)
        {
            Lcd_SetDDRAM(12);
            Lcd_TransmitStr("    ");
            
            show_select_timer = TT_SHOW_SELECT_IN_OFF;
            options_state = OPTIONS_ONOFF_WAIT_IN_OFF;
        }
        break;

    case OPTIONS_ONOFF_WAIT_IN_OFF:
        // si hay alguna seleccion en off paso a ON
        if ((actions == selection_up) ||
            (actions == selection_dwn) ||
            (actions == selection_enter))
        {
            options_state = OPTIONS_ONOFF_WAIT_IN_ON;
        }
        else if (!show_select_timer)
        {
            options_state = OPTIONS_ONOFF_REDRAW;
        }
        break;
        
    case OPTIONS_ONOFF_SELECT_OPTION:
        if (actions == selection_none)
        {
            *bool_value = options_curr_sel;
            options_state = OPTIONS_INIT;                
            resp = resp_finish;
        }
        break;

    default:
        options_state = OPTIONS_ONOFF_INIT;
        break;
    }

    return resp;
}



//recibe:
// el primer renglon
// el valor de default, minimo y maximo permitido
// la accion del switch
//devuelve:
// en el valor de default ultima seleccion
// resp_continue o resp_finish si termino la seleccion
resp_t LCD_EncoderChange (char * primer_renglon,
                          unsigned short * orig_value,
                          unsigned short min_val,
                          unsigned short max_val,
                          sw_actions_t actions)
{
    resp_t resp = resp_continue;
    char s_current [20];

    switch (change_state)
    {
    case CHANGE_INIT:
        LCD_1ER_RENGLON;
        Lcd_TransmitStr(primer_renglon);
        LCD_2DO_RENGLON;
        Lcd_TransmitStr((const char *) "SET    or    < >");        

        change_current_val = *orig_value;
        show_select_timer = 0;        
        change_state = CHANGE_WAIT_SELECT_IN_OFF;
        break;

    case CHANGE_WAIT_SELECT_IN_ON:
        if (actions == selection_up) 
        {
            if (change_current_val < max_val)
            {
                change_current_val++;
                change_state = CHANGE_SHOW_AGAIN;                
            }

            show_select_timer = TT_SHOW_SELECT_IN_ON;
            LCD_2DO_RENGLON;
            Lcd_TransmitStr((const char *) "SET    or    ++>");                    
        }

        if (actions == selection_dwn) 
        {
            if (change_current_val > min_val)
            {
                change_current_val--;
                change_state = CHANGE_SHOW_AGAIN;
            }

            show_select_timer = TT_SHOW_SELECT_IN_ON;
            LCD_2DO_RENGLON;
            Lcd_TransmitStr((const char *) "SET    or    <--");
        }

        if (actions == selection_enter)
        {
            LCD_2DO_RENGLON;
            Lcd_TransmitStr((const char *) "Selected...     ");
            change_state = CHANGE_SELECT_DONE;
        }
        
        if (!show_select_timer)
        {
            Lcd_SetDDRAM(14);    //TODO: ver esto despues, pasarlo como info en estructura
            Lcd_TransmitStr("  ");
            LCD_2DO_RENGLON;
            Lcd_TransmitStr((const char *) "SET    or    < >");        
            show_select_timer = TT_SHOW_SELECT_IN_OFF;
            change_state = CHANGE_WAIT_SELECT_IN_OFF;
        }
        break;

    case CHANGE_WAIT_SELECT_IN_OFF:
        //no hago cambios en apagado, agoto el timer para tener cambios siempre en ON
        if ((actions == selection_up) ||
            (actions == selection_dwn) ||
            (actions == selection_enter))
        {
            show_select_timer = 0;
        }
        
        if (!show_select_timer)
        {
            show_select_timer = TT_SHOW_SELECT_IN_ON;
            change_state = CHANGE_SHOW_AGAIN;
        }        
        break;

    case CHANGE_SHOW_AGAIN:
        Lcd_SetDDRAM(14);    //TODO: ver esto despues, pasarlo como info en estructura
        sprintf(s_current, "%02d", change_current_val);
        Lcd_TransmitStr(s_current);
        change_state = CHANGE_WAIT_SELECT_IN_ON;
        break;

    case CHANGE_SELECT_DONE:
        if (actions == selection_none)
        {
            *orig_value = change_current_val;
            change_state = CHANGE_INIT;
            resp = resp_finish;
        }
        break;
        
    default:
        change_state = CHANGE_INIT;
        break;
    }

    return resp;
}


void FuncShowSelectv2Reset (void)
{
    show_select_state = SHOW_SELECT_INIT;
}

#ifdef LCD_USE_SWITCHES_FOR_MENUES_FUNCTIONS
//recibe el primer renglon y el segundo
//recibe un puntero a las posiciones de memoria de los asteriscos
//recibe una variable de cantidad de opciones y otra variable con la primera opcion a la que apunta
//devuelve resp_continue o (RESP_SELECTED | opcion_elegida << 4)
unsigned char FuncOptions (const char * p_text1, const char * p_text2,
                           unsigned char * p_sel, unsigned char opts, unsigned char first_option)
{
    unsigned char resp = resp_continue;

    if (first_option & 0x80)	//me piden que elija una opcion especial
    {
        if ((first_option & 0x7F) < opts)	//no esta fuera de escala
        {
            if (options_state == OPTIONS_INIT)
                options_curr_sel = (first_option & 0x7F);
            else
            {
                if (options_curr_sel != (first_option & 0x7F)) //fuerzo el cambio, ya estaba mostrando la pantalla
                    options_state = OPTIONS_CHANGE_SELECT;
            }
        }
        else
            options_curr_sel = 0;
    }

    switch (options_state)
    {
    case OPTIONS_INIT:
        LCD_1ER_RENGLON;
        Lcd_TransmitStr(p_text1);
        LCD_2DO_RENGLON;
        Lcd_TransmitStr(p_text2);
        show_select_timer = 0;
        options_state++;
        break;

    case OPTIONS_WAIT_SELECT:
        if (!show_select_timer)
        {
            Lcd_SetDDRAM(*(p_sel + options_curr_sel));
            Lcd_TransmitStr("*");
            show_select_timer = TT_SHOW_SELECT_IN_ON;
            options_state++;
        }
        break;

    case OPTIONS_WAIT_SELECT_1:
        if (CheckS1() > S_NO)
        {
            options_state = OPTIONS_CHANGE_SELECT;
        }

        if (CheckS2() > S_NO)
        {
            options_state = OPTIONS_WAIT_SELECT_TIMEOUT;
            show_select_timer = 200;
        }

        if (!show_select_timer)
        {
            Lcd_SetDDRAM(*(p_sel + options_curr_sel));
            Lcd_TransmitStr(" ");
            show_select_timer = TT_SHOW_SELECT_IN_OFF;
            options_state = OPTIONS_WAIT_SELECT_3;
        }
        break;

    case OPTIONS_WAIT_SELECT_2:
        if (CheckS1() == S_NO)
        {
            options_state = OPTIONS_WAIT_SELECT_1;
        }
        break;

    case OPTIONS_WAIT_SELECT_3:
        if (CheckS1() > S_NO)
        {
            options_state = OPTIONS_CHANGE_SELECT;
        }

        if (CheckS2() > S_NO)
        {
            options_state = OPTIONS_WAIT_SELECT_TIMEOUT;
            show_select_timer = 200;
        }

        if (!show_select_timer)
        {
            Lcd_SetDDRAM(*(p_sel + options_curr_sel));
            Lcd_TransmitStr("*");
            show_select_timer = TT_SHOW_SELECT_IN_ON;
            options_state = OPTIONS_WAIT_SELECT_1;
        }
        break;

    case OPTIONS_WAIT_SELECT_TIMEOUT:
        if (!show_select_timer)
        {
            resp = (resp_selected | (options_curr_sel << 4));
            options_state = OPTIONS_INIT;
        }
        break;

    case OPTIONS_CHANGE_SELECT:
        Lcd_SetDDRAM(*(p_sel + options_curr_sel));
        Lcd_TransmitStr(" ");

        if (first_option & 0x80)	//me piden que elija una opcion especial
            options_curr_sel = (first_option & 0x7F);
        else
        {
            if (options_curr_sel < (opts - 1))
                options_curr_sel++;
            else
                options_curr_sel = 0;
        }

        Lcd_SetDDRAM(*(p_sel + options_curr_sel));
        Lcd_TransmitStr("*");

        options_state = OPTIONS_WAIT_SELECT_2;
        break;

    default:
        options_state = OPTIONS_INIT;
        break;
    }

    return resp;
}




//recibe un puntero a la variable de seleccion original
//devuelve resp_continue o resp_finish y el valor lo devuelve por el puntero
unsigned char FuncOptionsOnOff (unsigned char * bool_value)
{
    unsigned char resp = resp_continue;

    switch (options_state)
    {
    case OPTIONS_ONOFF_INIT:
#ifdef LINE_LENGTH_8
        LCD_1ER_RENGLON;        
        Lcd_TransmitStr((const char *) "on of e ");
        LCD_2DO_RENGLON;
        Lcd_TransmitStr((const char*)"chg  sel");
#endif
#ifdef LINE_LENGTH_16
        LCD_1ER_RENGLON;
        Lcd_TransmitStr((const char *) "on  off   done ");
        LCD_2DO_RENGLON;
        Lcd_TransmitStr((const char *) "change    select");
#endif

        options_mark = *bool_value;
        
        if (options_mark)
            options_curr_sel = 0;
        else
            options_curr_sel = 1;
                    
        show_select_timer = 0;
        options_state++;
        break;

    case OPTIONS_ONOFF_REDRAW:
        //borro asteriscos
        Lcd_SetDDRAM(*(s_sel_up_down));
        Lcd_TransmitStr(" ");
        Lcd_SetDDRAM(*(s_sel_up_down + 1));
        Lcd_TransmitStr(" ");
        Lcd_SetDDRAM(*(s_sel_up_down + 2));
        Lcd_TransmitStr(" ");

        //redibujo seleccion y current
        if (options_mark)
            Lcd_SetDDRAM(*(s_sel_up_down));
        else
            Lcd_SetDDRAM(*(s_sel_up_down + 1));

        Lcd_TransmitStr("*");        
        Lcd_SetDDRAM(*(s_sel_up_down + options_curr_sel));
        Lcd_TransmitStr("*");

        show_select_timer = TT_SHOW_SELECT_IN_ON;
        options_state = OPTIONS_ONOFF_WAIT_IN_ON;

        break;

    case OPTIONS_ONOFF_WAIT_IN_ON:    //en este momento tengo el asterisco
        if (CheckS1() > S_NO)
        {
            options_state = OPTIONS_ONOFF_CHANGE_OPTION;
        }

        if (CheckS2() > S_NO)
        {
            options_state = OPTIONS_ONOFF_SELECT_OPTION;
            show_select_timer = 200;
        }

        if (!show_select_timer)
        {
            Lcd_SetDDRAM(*(s_sel_up_down + options_curr_sel));
            Lcd_TransmitStr(" ");
            show_select_timer = TT_SHOW_SELECT_IN_OFF;
            options_state = OPTIONS_ONOFF_WAIT_IN_OFF;
        }
        break;

    case OPTIONS_ONOFF_WAIT_IN_OFF:    //en este momento no tengo asterisco
        if (CheckS1() > S_NO)
        {
            options_state = OPTIONS_ONOFF_CHANGE_OPTION;
        }

        if (CheckS2() > S_NO)
        {            
            options_state = OPTIONS_ONOFF_SELECT_OPTION;
            show_select_timer = 200;
        }

        if (!show_select_timer)
        {
            Lcd_SetDDRAM(*(s_sel_up_down + options_curr_sel));
            Lcd_TransmitStr("*");
            show_select_timer = TT_SHOW_SELECT_IN_ON;
            options_state = OPTIONS_ONOFF_WAIT_IN_ON;
        }
        break;
        

    case OPTIONS_ONOFF_SELECT_OPTION:
        if (!show_select_timer)
        {
            if (options_curr_sel == 2)    //esto es un end, en bool_value ya esta lo elegido
            {
                *bool_value = options_mark;
                options_state = OPTIONS_INIT;                
                resp = resp_finish;
            }
            else
            {
                if (options_curr_sel)
                    options_mark = 0;
                else
                    options_mark = 1;

                options_state = OPTIONS_ONOFF_REDRAW;
            }
        }
        break;

    case OPTIONS_ONOFF_CHANGE_OPTION:
        
        if (options_curr_sel < 2)
            options_curr_sel++;
        else
            options_curr_sel = 0;
    
        options_state = OPTIONS_ONOFF_WAIT_FREE_S1;
        break;

    case OPTIONS_ONOFF_WAIT_FREE_S1:
        if (CheckS1() == S_NO)
        {
            options_state = OPTIONS_ONOFF_REDRAW;
        }
        break;

    default:
        options_state = OPTIONS_ONOFF_INIT;
        break;
    }

    return resp;
}


void FuncOptionsReset (void)
{
    options_state = OPTIONS_INIT;
}


//recibe el valor original para arrancar seleccion
//recibe el modo CHANGE_PERCENT, CHANGE_SECS o CHANGE_CHANNELS Ademas puede tener |CHANGE_RESET
//recibe min val permitido, MAX val permitido
//devuelve resp_continue o resp_finish si termino la seleccion
unsigned char FuncChange (unsigned short * p_orig_value, unsigned char mode,
                          unsigned short min_val, unsigned short max_val)
{
    unsigned char resp = resp_continue;
    char s_current [20];

    switch (change_state)
    {
    case CHANGE_INIT:
        change_current_val = *p_orig_value;
        change_last_option = 0;
        FuncOptionsReset();
        change_state++;
        break;

    case CHANGE_WAIT_SELECT:
#ifdef LINE_LENGTH_8
        if (mode == CHANGE_PERCENT)
        {
            sprintf(s_current, "%3d", change_current_val);
            strcat(s_current, (const char*)"%   sel");
        }
        else if (mode == CHANGE_SECS)
        {
            sprintf(s_current, "%2d", change_current_val);
            strcat(s_current, (const char*)" secs sel");
        }
        else if (mode == CHANGE_CHANNELS)
        {
            sprintf(s_current, "%3d ", change_current_val);
            strcat(s_current, (const char*)"ch sel");
        }
        else if (mode == CHANGE_PROGRAMS)
        {
            sprintf(s_current, "%1d ", change_current_val);
            strcat(s_current, (const char*)"ch sel");
        }
        else if (mode == CHANGE_VOLTAGE_MAINS)
        {
            sprintf(s_current, "%2dV main", change_current_val);
        }
        else if (mode == CHANGE_VOLTAGE_1)
        {
            sprintf(s_current, "%2dV ch:1", change_current_val);
        }
        else if (mode == CHANGE_VOLTAGE_2)
        {
            sprintf(s_current, "%2dV ch:2", change_current_val);
        }                
        else if (mode == CHANGE_VOLTAGE_3)
        {
            sprintf(s_current, "%2dV ch:3", change_current_val);
        }                
        else if (mode == CHANGE_VOLTAGE_4)
        {
            sprintf(s_current, "%2dV ch:4", change_current_val);
        }                
        else if (mode == CHANGE_VOLTAGE_5)
        {
            sprintf(s_current, "%2dV ch:5", change_current_val);
        }                
        else if (mode == CHANGE_VOLTAGE_6)
        {
            sprintf(s_current, "%2dV ch:6", change_current_val);
        }                        
        else 
            return resp_finish;

        resp = FuncOptions ((const char *) "up dn e ",
                            s_current,(unsigned char *) s_sel_up_down,
                            3, change_last_option);
#endif
        
#ifdef LINE_LENGTH_16
        memset(s_current, ' ', sizeof(s_current));
        if (mode == CHANGE_PERCENT)
        {
            sprintf(s_current, "chg  %3d", change_current_val);
            strcat(s_current, (const char*)"%   sel");
        }
        else if (mode == CHANGE_SECS)
        {
            sprintf(s_current, "chg %2d", change_current_val);
            strcat(s_current, (const char*)" secs sel");
        }
        else	//debe ser CHANNELS
        {
            sprintf(s_current, "chg   %3d", change_current_val);
            strcat(s_current, (const char*)"ch  sel");
        }

        resp = FuncOptions ((const char *) "up  down   done ",
                            s_current,(unsigned char *) s_sel_up_down,
                            3, change_last_option);
#endif
        change_last_option = 0;

        if ((resp & 0x0f) == resp_selected)
        {
            resp = resp & 0xf0;
            resp >>= 4;
            if (resp == 0)
            {
                if (change_current_val < max_val)
                    change_current_val++;

                resp = resp_working;
            }

            if (resp == 1)
            {
                if (change_current_val > min_val)
                    change_current_val--;

                change_last_option = (1 | 0x80);	//fuerzo update de la opcion
                resp = resp_working;
            }

            if (resp == 2)
            {
                change_state = CHANGE_INIT;
                resp = resp_finish;
                *p_orig_value = change_current_val;
            }
        }
        break;

    default:
        change_state = CHANGE_INIT;
        break;
    }

    return resp;
}

//recibe el valor original para arrancar seleccion
//recibe min val permitido, MAX val permitido
//devuelve resp_continue o resp_finish si termino la seleccion
unsigned char FuncChangeDecimals (unsigned char * p_orig_int, unsigned char * p_orig_dec,
                                  unsigned char min_val_int, unsigned char min_val_dec,    //ej 1.1
                                  unsigned char max_val_int, unsigned char max_val_dec)    //ej 2.4
{
    unsigned char resp = resp_continue;
    char s_current [20];

    switch (change_state)
    {
    case CHANGE_INIT:
        change_current_val_int = *p_orig_int;
        change_current_val_dec = *p_orig_dec;     
        change_last_option = 0;
        FuncOptionsReset();
        change_state++;
        break;

    case CHANGE_WAIT_SELECT:
#ifdef LINE_LENGTH_8

        sprintf(s_current, "%1d.%1dA", change_current_val_int, change_current_val_dec);
        strcat(s_current, (const char*)" c s");

        resp = FuncOptions ((const char *) "up dn e ",
                            s_current,(unsigned char *) s_sel_up_down,
                            3, change_last_option);
#endif
        
#ifdef LINE_LENGTH_16
        memset(s_current, ' ', sizeof(s_current));
        sprintf(s_current, "current: %1d.%1dA", change_current_val_int, change_current_val_dec);
        strcat(s_current, (const char*)"%   sel");

        resp = FuncOptions ((const char *) "up  down   done ",
                            s_current,(unsigned char *) s_sel_up_down,
                            3, change_last_option);
#endif
        change_last_option = 0;

        if ((resp & 0x0f) == resp_selected)
        {
            resp = resp & 0xf0;
            resp >>= 4;
            if (resp == 0)
            {
                //esto es UP, no dejo mas de max_val_int.max_val_dec
                if (change_current_val_int < max_val_int)
                {
                    if (change_current_val_dec < 9)
                        change_current_val_dec++;
                    else
                    {
                        change_current_val_dec = 0;
                        change_current_val_int++;
                    }
                }
                else if (change_current_val_dec < max_val_dec)
                    change_current_val_dec++;
                    
                resp = resp_working;
            }

            if (resp == 1)
            {
                //esto es DWN, no dejo menos de min_val_int.min_val_dec
                if (change_current_val_int > min_val_int)
                {
                    if (change_current_val_dec > 0)
                        change_current_val_dec--;
                    else
                    {
                        change_current_val_dec = 9;
                        change_current_val_int--;
                    }
                }
                else if (change_current_val_dec > min_val_dec)
                    change_current_val_dec--;

                change_last_option = (1 | 0x80);	//fuerzo update de la opcion
                resp = resp_working;
            }

            if (resp == 2)
            {
                change_state = CHANGE_INIT;
                *p_orig_int = change_current_val_int;
                *p_orig_dec = change_current_val_dec;
                resp = resp_finish;                
            }
        }
        break;

    default:
        change_state = CHANGE_INIT;
        break;
    }

    return resp;
}

#endif    //LCD_USE_SWITCHES_FOR_MENUES_FUNCTIONS



// //--- end of file ---//
