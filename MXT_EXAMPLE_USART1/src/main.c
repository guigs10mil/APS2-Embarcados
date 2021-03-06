/**
 * \file
 *
 * \brief Example of usage of the maXTouch component with USART
 *
 * This example shows how to receive touch data from a maXTouch device
 * using the maXTouch component, and display them in a terminal window by using
 * the USART driver.
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */

/**
 * \mainpage
 *
 * \section intro Introduction
 * This simple example reads data from the maXTouch device and sends it over
 * USART as ASCII formatted text.
 *
 * \section files Main files:
 * - example_usart.c: maXTouch component USART example file
 * - conf_mxt.h: configuration of the maXTouch component
 * - conf_board.h: configuration of board
 * - conf_clock.h: configuration of system clock
 * - conf_example.h: configuration of example
 * - conf_sleepmgr.h: configuration of sleep manager
 * - conf_twim.h: configuration of TWI driver
 * - conf_usart_serial.h: configuration of USART driver
 *
 * \section apiinfo maXTouch low level component API
 * The maXTouch component API can be found \ref mxt_group "here".
 *
 * \section deviceinfo Device Info
 * All UC3 and Xmega devices with a TWI module can be used with this component
 *
 * \section exampledescription Description of the example
 * This example will read data from the connected maXTouch explained board
 * over TWI. This data is then processed and sent over a USART data line
 * to the board controller. The board controller will create a USB CDC class
 * object on the host computer and repeat the incoming USART data from the
 * main controller to the host. On the host this object should appear as a
 * serial port object (COMx on windows, /dev/ttyxxx on your chosen Linux flavour).
 *
 * Connect a terminal application to the serial port object with the settings
 * Baud: 57600
 * Data bits: 8-bit
 * Stop bits: 1 bit
 * Parity: None
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC and IAR for AVR.
 * Other compilers may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/">Atmel</A>.\n
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include <asf.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "conf_board.h"
#include "conf_example.h"
#include "conf_uart_serial.h"
#include "tfont.h"
#include "sourcecodepro_28.h"
#include "calibri_36.h"
#include "arial_72.h"
#include "math.h"

#include "Icons/WashH/time.h"
#include "Icons/WashH/vortex.h"
#include "Icons/WashH/water.h"
#include "Icons/WashH/zanvil.h"
#include "Icons/WashH/gear.h"
#include "Icons/WashH/day.h"
#include "Icons/WashH/rep.h"

#include "Icons/WashH/lockedGray.h"
#include "Icons/WashH/LockedGreen.h"
#include "Icons/WashH/lockedRed.h"
#include "Icons/WashH/unlocked.h"



#include "Icons/buttonH/lavar.h"
#include "Icons/buttonH/modo.h"
#include "Icons/buttonH/configurar.h"
#include "Icons/buttonH/voltar.h"
#include "Icons/buttonH/cancelar.h"



#include "Anima/D0.h"
#include "Anima/D1.h"
#include "Anima/D2.h"
#include "Anima/D3.h"
#include "Anima/D4.h"
#include "Anima/D5.h"
#include "Anima/D6.h"
#include "Anima/D7.h"
#include "Anima/D8.h"
#include "Anima/D9.h"
#include "Anima/D10.h"
#include "Anima/D11.h"
#include "Anima/D12.h"
#include "Anima/D13.h"
#include "Anima/D14.h"
#include "Anima/D15.h"
#include "Anima/D16.h"
#include "Anima/D17.h"
#include "Anima/D18.h"
#include "Anima/D19.h"


#include "maquina1.h"


#define MAX_ENTRIES        3
#define STRING_LENGTH     70

#define USART_TX_MAX_LENGTH     0xff

#define TOUCH_PRESS_STATUS    192
#define TOUCH_RELEASE_STATUS  32

#define config_text_group_height  95
#define source_font_height        15
#define calibri_height            40
#define config_spacing            15

#define LED_PIO           PIOC
#define LED_PIO_ID        12
#define LED_PIO_IDX       8u
#define LED_PIO_IDX_MASK  (1u << LED_PIO_IDX)

#define BUT_PIO           PIOA
#define BUT_PIO_ID        10
#define BUT_PIO_IDX       11u
#define BUT_PIO_IDX_MASK  (1u << BUT_PIO_IDX)

#define UNLOCK_PRESS_SECONDS   3

struct ili9488_opt_t g_ili9488_display_opt;

int f_modo = 0;
int f_lock = 0;
int f_config = 0;
volatile int f_start = 0;
volatile int f_draw_config = 0;
volatile int f_draw_start = 0;
volatile int f_draw_menu = 0;

volatile int f_pressing_lock = 0;
volatile int lock_counter = 0;
volatile int f_door_is_open = 0;
volatile int f_draw_door_is_open = 0;
volatile int f_draw_ready_unlock = 0;

volatile int tempo_sec = 0;

volatile int security_flag=0;

const char *enxague_tempos[] = {"0", "15", "30", "45"};
const char *enxague_vezes[] = {"0", "1", "2", "3"};
const char *centrifuga_tempos[] = {"0", "5", "10", "15"};
const char *centrifuga_RPM[] = {"600", "800", "900", "1200"};
const int enxague_tempos_int[] = {0, 15, 30, 45};
const int enxague_vezes_int[] = {0, 1, 2, 3};
const int centrifuga_tempos_int[] = {0, 5, 10, 15};
const int centrifuga_RPM_int[] = {600, 800, 900, 1200};

int enx_t_i = 0;
int enx_v_i = 0;
int cen_t_i = 0;
int cen_r_i = 0;
int pesado = 0;
int bolhas = 0;

volatile int anim_counter = 0;
volatile int f_draw_anim = 0;

const tImage* AnimaList[18]= {&D0,&D1,&D2,&D3,&D4,&D5,&D6,&D7,&D8,&D9,&D10,&D11,&D12,&D13,&D14,&D15,&D16,&D17};

/**
 * Inicializa ordem do menu
 * retorna o primeiro ciclo que
 * deve ser exibido.
 */
t_ciclo *initMenuOrder() {
    c_rapido.previous = &c_config;
    c_rapido.next = &c_diario;

    c_diario.previous = &c_rapido;
    c_diario.next = &c_pesado;

    c_pesado.previous = &c_diario;
    c_pesado.next = &c_enxague;

    c_enxague.previous = &c_pesado;
    c_enxague.next = &c_centrifuga;

    c_centrifuga.previous = &c_enxague;
    c_centrifuga.next = &c_config;

    c_config.previous = &c_centrifuga;
    c_config.next = &c_rapido;

    return(&c_diario);
}

static void configure_lcd(void) {
    /* Initialize display parameter */
    g_ili9488_display_opt.ul_width = ILI9488_LCD_WIDTH;
    g_ili9488_display_opt.ul_height = ILI9488_LCD_HEIGHT;
    g_ili9488_display_opt.foreground_color = COLOR_CONVERT(COLOR_WHITE);
    g_ili9488_display_opt.background_color = COLOR_CONVERT(COLOR_WHITE);

    /* Initialize LCD */
    ili9488_init(&g_ili9488_display_opt);
}

void font_draw_text(tFont *font, const char *text, int x, int y, int spacing) {
    char *p = text;
    while(*p != NULL) {
        char letter = *p;
        int letter_offset = letter - font->start_char;
        if(letter <= font->end_char) {
            tChar *current_char = font->chars + letter_offset;
            ili9488_draw_pixmap(x, y, current_char->image->width, current_char->image->height, current_char->image->data);
            x += current_char->image->width + spacing;
        }
        p++;
    }
}

/**
 * \brief Set maXTouch configuration
 *
 * This function writes a set of predefined, optimal maXTouch configuration data
 * to the maXTouch Xplained Pro.
 *
 * \param device Pointer to mxt_device struct
 */
static void mxt_init(struct mxt_device *device)
{
    enum status_code status;

    /* T8 configuration object data */
    uint8_t t8_object[] = {
        0x0d, 0x00, 0x05, 0x0a, 0x4b, 0x00, 0x00,
        0x00, 0x32, 0x19
    };

    /* T9 configuration object data */
    uint8_t t9_object[] = {
        0x8B, 0x00, 0x00, 0x0E, 0x08, 0x00, 0x80,
        0x32, 0x05, 0x02, 0x0A, 0x03, 0x03, 0x20,
        0x02, 0x0F, 0x0F, 0x0A, 0x00, 0x00, 0x00,
        0x00, 0x18, 0x18, 0x20, 0x20, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x02,
        0x02
    };

    /* T46 configuration object data */
    uint8_t t46_object[] = {
        0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x03,
        0x00, 0x00
    };

    /* T56 configuration object data */
    uint8_t t56_object[] = {
        0x02, 0x00, 0x01, 0x18, 0x1E, 0x1E, 0x1E,
        0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E,
        0x1E, 0x1E, 0x1E, 0x1E, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00
    };

    /* TWI configuration */
    twihs_master_options_t twi_opt = {
        .speed = MXT_TWI_SPEED,
        .chip  = MAXTOUCH_TWI_ADDRESS,
    };

    status = (enum status_code)twihs_master_setup(MAXTOUCH_TWI_INTERFACE, &twi_opt);
    Assert(status == STATUS_OK);

    /* Initialize the maXTouch device */
    status = mxt_init_device(device, MAXTOUCH_TWI_INTERFACE,
                             MAXTOUCH_TWI_ADDRESS, MAXTOUCH_XPRO_CHG_PIO);
    Assert(status == STATUS_OK);

    /* Issue soft reset of maXTouch device by writing a non-zero value to
     * the reset register */
    mxt_write_config_reg(device, mxt_get_object_address(device,
                         MXT_GEN_COMMANDPROCESSOR_T6, 0)
                         + MXT_GEN_COMMANDPROCESSOR_RESET, 0x01);

    /* Wait for the reset of the device to complete */
    delay_ms(MXT_RESET_TIME);

    /* Write data to configuration registers in T7 configuration object */
    mxt_write_config_reg(device, mxt_get_object_address(device,
                         MXT_GEN_POWERCONFIG_T7, 0) + 0, 0x20);
    mxt_write_config_reg(device, mxt_get_object_address(device,
                         MXT_GEN_POWERCONFIG_T7, 0) + 1, 0x10);
    mxt_write_config_reg(device, mxt_get_object_address(device,
                         MXT_GEN_POWERCONFIG_T7, 0) + 2, 0x4b);
    mxt_write_config_reg(device, mxt_get_object_address(device,
                         MXT_GEN_POWERCONFIG_T7, 0) + 3, 0x84);

    /* Write predefined configuration data to configuration objects */
    mxt_write_config_object(device, mxt_get_object_address(device,
                            MXT_GEN_ACQUISITIONCONFIG_T8, 0), &t8_object);
    mxt_write_config_object(device, mxt_get_object_address(device,
                            MXT_TOUCH_MULTITOUCHSCREEN_T9, 0), &t9_object);
    mxt_write_config_object(device, mxt_get_object_address(device,
                            MXT_SPT_CTE_CONFIGURATION_T46, 0), &t46_object);
    mxt_write_config_object(device, mxt_get_object_address(device,
                            MXT_PROCI_SHIELDLESS_T56, 0), &t56_object);

    /* Issue recalibration command to maXTouch device by writing a non-zero
     * value to the calibrate register */
    mxt_write_config_reg(device, mxt_get_object_address(device,
                         MXT_GEN_COMMANDPROCESSOR_T6, 0)
                         + MXT_GEN_COMMANDPROCESSOR_CALIBRATE, 0x01);
}

void draw_screen(void) {
    ili9488_set_foreground_color(COLOR_CONVERT(COLOR_WHITE));
    ili9488_draw_filled_rectangle(0, 0, ILI9488_LCD_WIDTH-1, ILI9488_LCD_HEIGHT-1);
}

uint32_t convert_axis_system_x(uint32_t touch_y) {
    // entrada: 4096 - 0 (sistema de coordenadas atual)
    // saida: 0 - 320
    return ILI9488_LCD_WIDTH - ILI9488_LCD_WIDTH*touch_y/4096;
}

uint32_t convert_axis_system_y(uint32_t touch_x) {
    // entrada: 0 - 4096 (sistema de coordenadas atual)
    // saida: 0 - 320
    return ILI9488_LCD_HEIGHT*touch_x/4096;
}

int get_next_from_list(int list_of_ints[4], int current_index) {

    int size = sizeof(list_of_ints);

    if (current_index + 1 < size) {
        return current_index + 1;
    } else {
        return 0;
    }
}

char *bool_to_string(int booly) {
    if (booly) return "Sim";
    else return "Nao";
}

void open_door(void) {
    pio_clear(LED_PIO, LED_PIO_IDX_MASK); // acende
    f_door_is_open = 1;
    if(f_start) {
        security_flag=1;

    }
}

void close_door(void) {
    pio_set(LED_PIO, LED_PIO_IDX_MASK); // apaga
    f_door_is_open = 0;
}

void but_callback(void)
{
    if (f_door_is_open) {
        close_door();
    } else {
        open_door();
    }
}

void fill_config_struct() {
    c_config.bubblesOn = bolhas;
    c_config.centrifugacaoRPM = centrifuga_RPM_int[cen_r_i];
    c_config.centrifugacaoTempo = centrifuga_tempos_int[cen_t_i];
    c_config.enxagueQnt = enxague_vezes_int[enx_v_i];
    c_config.enxagueTempo = enxague_tempos_int[enx_t_i];
    c_config.heavy = pesado;
}

void draw_config_enxague(void) {
    font_draw_text(&sourcecodepro_28, "ENXAGUE", 20, 30+calibri_height+config_spacing, 1);
    font_draw_text(&calibri_36, "Tempo:", 20, 30+calibri_height+config_spacing+source_font_height, 1);
    font_draw_text(&calibri_36, "Vezes:", 20, 30+calibri_height*2+config_spacing+source_font_height, 1);
    font_draw_text(&calibri_36, enxague_tempos[enx_t_i], ILI9488_LCD_WIDTH/2, 30+calibri_height+config_spacing+source_font_height, 1);
    font_draw_text(&calibri_36, enxague_vezes[enx_v_i], ILI9488_LCD_WIDTH/2, 30+calibri_height*2+config_spacing+source_font_height, 1);
    ili9488_set_foreground_color(COLOR_CONVERT(COLOR_GRAY));
    ili9488_draw_pixmap(ILI9488_LCD_WIDTH-70,30+calibri_height+config_spacing+source_font_height
                        ,rep.width,rep.height,rep.data);
    ili9488_draw_pixmap(ILI9488_LCD_WIDTH-70,30+calibri_height*2+config_spacing+source_font_height
                        ,rep.width,rep.height,rep.data);
}

void draw_config_centrifuga(void) {
    font_draw_text(&sourcecodepro_28, "CENTRIFUGA", 20, 30+calibri_height+config_spacing+(config_text_group_height+config_spacing), 1);
    font_draw_text(&calibri_36, "RPM:", 20, 30+calibri_height+config_spacing+source_font_height+(config_text_group_height+config_spacing), 1);
    font_draw_text(&calibri_36, "Tempo:", 20, 30+calibri_height*2+config_spacing+source_font_height+(config_text_group_height+config_spacing), 1);
    font_draw_text(&calibri_36, centrifuga_RPM[cen_r_i], ILI9488_LCD_WIDTH/2, 30+calibri_height+config_spacing+source_font_height+(config_text_group_height+config_spacing), 1);
    font_draw_text(&calibri_36, centrifuga_tempos[cen_t_i], ILI9488_LCD_WIDTH/2, 30+calibri_height*2+config_spacing+source_font_height+(config_text_group_height+config_spacing), 1);
    ili9488_set_foreground_color(COLOR_CONVERT(COLOR_GRAY));
    ili9488_draw_pixmap(ILI9488_LCD_WIDTH-70,30+calibri_height+config_spacing+source_font_height+(config_text_group_height+config_spacing)
                        ,rep.width,rep.height,rep.data);
    ili9488_draw_pixmap(ILI9488_LCD_WIDTH-70,30+calibri_height*2+config_spacing+source_font_height+(config_text_group_height+config_spacing)
                        ,rep.width,rep.height,rep.data);
}

void draw_config_adicionais(void) {
    font_draw_text(&sourcecodepro_28, "ADICIONAIS", 20, 30+calibri_height+config_spacing+2*(config_text_group_height+config_spacing), 1);
    font_draw_text(&calibri_36, "Pesado:", 20, 30+calibri_height+config_spacing+source_font_height+2*(config_text_group_height+config_spacing), 1);
    font_draw_text(&calibri_36, "Bolhas:", 20, 30+calibri_height*2+config_spacing+source_font_height+2*(config_text_group_height+config_spacing), 1);
    font_draw_text(&calibri_36, bool_to_string(pesado), ILI9488_LCD_WIDTH/2, 30+calibri_height+config_spacing+source_font_height+2*(config_text_group_height+config_spacing), 1);
    font_draw_text(&calibri_36, bool_to_string(bolhas), ILI9488_LCD_WIDTH/2, 30+calibri_height*2+config_spacing+source_font_height+2*(config_text_group_height+config_spacing), 1);
    ili9488_draw_pixmap(ILI9488_LCD_WIDTH-70,30+calibri_height+config_spacing+source_font_height+2*(config_text_group_height+config_spacing)
                        ,rep.width,rep.height,rep.data);
    ili9488_draw_pixmap(ILI9488_LCD_WIDTH-70,30+calibri_height*2+config_spacing+source_font_height+2*(config_text_group_height+config_spacing)
                        ,rep.width,rep.height,rep.data);
}

void TC1_Handler(void) {
    volatile uint32_t ul_dummy;

    ul_dummy = tc_get_status(TC0, 1);

    UNUSED(ul_dummy);

    if (f_pressing_lock) {
        lock_counter += 1;
    }

    if (lock_counter == UNLOCK_PRESS_SECONDS) {
        f_draw_ready_unlock = 1;
    }
}

void TC0_Handler(void) {
    volatile uint32_t ul_dummy;

    ul_dummy = tc_get_status(TC0, 0);

    /* Avoid compiler warning */
    UNUSED(ul_dummy);
    if (anim_counter>17)
    {
        anim_counter = 0;
    }

    f_draw_anim = 1;

}

void TC_init(Tc * TC, int ID_TC, int TC_CHANNEL, int freq) {
    uint32_t ul_div;
    uint32_t ul_tcclks;
    uint32_t ul_sysclk = sysclk_get_cpu_hz();

    uint32_t channel = 1;

    /* Configura o PMC */
    /* O TimerCounter ? meio confuso
    o uC possui 3 TCs, cada TC possui 3 canais
    TC0 : ID_TC0, ID_TC1, ID_TC2
    TC1 : ID_TC3, ID_TC4, ID_TC5
    TC2 : ID_TC6, ID_TC7, ID_TC8
    */
    pmc_enable_periph_clk(ID_TC);

    /** Configura o TC para operar em  4Mhz e interrup?c?o no RC compare */
    tc_find_mck_divisor(freq, ul_sysclk, &ul_div, &ul_tcclks, ul_sysclk);
    tc_init(TC, TC_CHANNEL, ul_tcclks | TC_CMR_CPCTRG);
    tc_write_rc(TC, TC_CHANNEL, (ul_sysclk / ul_div) / freq);

    /* Configura e ativa interrup?c?o no TC canal 0 */
    /* Interrup??o no C */
    NVIC_EnableIRQ((IRQn_Type) ID_TC);
    tc_enable_interrupt(TC, TC_CHANNEL, TC_IER_CPCS);

    /* Inicializa o canal 0 do TC */
    tc_start(TC, TC_CHANNEL);
}

void check_lock(uint32_t tx, uint32_t ty,uint32_t status) {
    if (tx >= 10 && tx <= 70) {
        if (ty >= 398+10 && ty <= 398+10+60) {
            if(!f_lock && status==TOUCH_PRESS_STATUS) {
                f_lock = 1;
                ili9488_draw_pixmap(10,398+10,lockedRed.width,lockedRed.height,lockedGray.data);
            }
            if(f_lock) {
                if (status == TOUCH_PRESS_STATUS) {
                    f_pressing_lock = 1;
                    TC_init(TC0, ID_TC1, 1, 1);
                    ili9488_draw_pixmap(10,398+10,lockedRed.width,lockedRed.height,lockedRed.data);
                }
                else {
                    if (f_pressing_lock && lock_counter > 2) {
                        tc_stop(TC0, 1);
                        anim_counter = 0;
                        f_lock = 0;
                        f_pressing_lock = 0;
                        lock_counter = 0;
                        ili9488_draw_pixmap(10,398+10,unlocked.width,unlocked.height,unlocked.data);
                    }
                }
            }

        }
    }

    if (status == TOUCH_RELEASE_STATUS) {
        if (f_lock) {
            if (f_pressing_lock) {
                f_pressing_lock = 0;
                lock_counter = 0;
                ili9488_draw_pixmap(10,398+10,lockedGray.width,lockedGray.height,lockedGray.data);
            }
        }
    }
}


void update_screen (uint32_t tx, uint32_t ty, uint32_t status) {



    check_lock(tx,ty,status);

    if (!f_lock && status == TOUCH_PRESS_STATUS) {
        if (tx >= ILI9488_LCD_WIDTH/2-80 && tx <= ILI9488_LCD_WIDTH/2+80 && !f_start && !f_config) {
            if (ty >= 198+50 && ty <= 198+50+80) {
                f_draw_start = 1;
                if (f_door_is_open) {
                    f_draw_door_is_open = 1;
                }
            } else if (ty > 328+10 && ty < 328+10+60) {
                f_modo = 1;
            } else if (ty > 398+10 && ty < 398+10+60) {
                f_draw_config = 1;
                f_config = 1;
            }
        }

        else if (tx >= ILI9488_LCD_WIDTH/2-80 && tx <= ILI9488_LCD_WIDTH/2+80 && f_start) {
            if (ty > 398+10 && ty < 398+10+60) {
                f_draw_menu = 1;
                f_start = 0;
            }
        }

        else if (f_config) {
            if (tx >= ILI9488_LCD_WIDTH/2-80 && tx <= ILI9488_LCD_WIDTH/2+80) {
                if (ty > 398+10 && ty < 398+10+60) {
                    f_draw_menu = 1;
                    f_config = 0;

                    fill_config_struct();
                }
            }

            else if (tx >= ILI9488_LCD_WIDTH-70 && tx <= ILI9488_LCD_WIDTH-10) {

                int y_base = 30+calibri_height+config_spacing+source_font_height;

                if (ty >= y_base && ty <= y_base+36) {
                    enx_t_i = get_next_from_list(enxague_tempos_int, enx_t_i);
                    f_draw_config = 1;
                }
                else if (ty >= y_base+calibri_height && ty <= y_base+calibri_height+36) {
                    enx_v_i = get_next_from_list(enxague_vezes_int, enx_v_i);
                    f_draw_config = 1;
                }

                else if (ty >= y_base+(config_text_group_height+config_spacing) && ty <= y_base+(config_text_group_height+config_spacing)+36) {
                    cen_r_i = get_next_from_list(centrifuga_RPM_int, cen_r_i);
                    f_draw_config = 1;
                }
                else if (ty >= y_base+calibri_height+(config_text_group_height+config_spacing) && ty <= y_base+calibri_height+(config_text_group_height+config_spacing)+36) {
                    cen_t_i = get_next_from_list(centrifuga_tempos_int, cen_t_i);
                    f_draw_config = 1;
                }

                else if (ty >= y_base+2*(config_text_group_height+config_spacing) && ty <= y_base+2*(config_text_group_height+config_spacing)+36) {
                    pesado = !pesado;
                    f_draw_config = 1;
                }
                else if (ty >= y_base+calibri_height+2*(config_text_group_height+config_spacing) && ty <= y_base+calibri_height+2*(config_text_group_height+config_spacing)+36) {
                    bolhas = !bolhas;
                    f_draw_config = 1;
                }

            }
        }

    }

}

void mxt_handler(struct mxt_device *device)
{
    /* USART tx buffer initialized to 0 */
    char tx_buf[STRING_LENGTH * MAX_ENTRIES] = {0};
    uint8_t i = 0; /* Iterator */

    /* Temporary touch event data struct */
    struct mxt_touch_event touch_event;

    /* Collect touch events and put the data in a string,
     * maximum 2 events at the time */
    do {
        /* Temporary buffer for each new touch event line */
        char buf[STRING_LENGTH];

        /* Read next next touch event in the queue, discard if read fails */
        if (mxt_read_touch_event(device, &touch_event) != STATUS_OK) {
            continue;
        }

        // eixos trocados (quando na vertical LCD)
        uint32_t conv_x = convert_axis_system_x(touch_event.y);
        uint32_t conv_y = convert_axis_system_y(touch_event.x);

        /* Format a new entry in the data string that will be sent over USART */
        sprintf(buf, "Nr: %1d, X:%4d, Y:%4d, Status:0x%2x conv X:%3d Y:%3d\n\r",
                touch_event.id, touch_event.x, touch_event.y,
                touch_event.status, conv_x, conv_y);

        /*printf("%s: %d", "Stuff", touch_event.status);*/
        if (touch_event.status == TOUCH_RELEASE_STATUS) {
            update_screen(conv_x, conv_y, TOUCH_RELEASE_STATUS);
        } else if (touch_event.status == TOUCH_PRESS_STATUS) {
            update_screen(conv_x, conv_y, TOUCH_PRESS_STATUS);
        }

        /* Add the new string to the string buffer */
        /*strcat(tx_buf, buf);*/
        i++;

        /* Check if there is still messages in the queue and
         * if we have reached the maximum numbers of events */
    } while ((mxt_is_message_pending(device)) & (i < MAX_ENTRIES));

    /* If there is any entries in the buffer, send them over USART */
    if (i > 0) {
        usart_serial_write_packet(USART_SERIAL_EXAMPLE, (uint8_t *)tx_buf, strlen(tx_buf));
    }
}

void RTC_init() {
    /* Configura o PMC */
    pmc_enable_periph_clk(ID_RTC);

    /* Default RTC configuration, 24-hour mode */
    rtc_set_hour_mode(RTC, 0);

    /* Configura data e hora manualmente */
    rtc_set_date(RTC, 0, 0, 0, 0);
    rtc_set_time(RTC, 0, 0, 0);

    /* Configure RTC interrupts */
    NVIC_DisableIRQ(RTC_IRQn);
    NVIC_ClearPendingIRQ(RTC_IRQn);
    NVIC_SetPriority(RTC_IRQn, 5);
    NVIC_EnableIRQ(RTC_IRQn);

    /* Ativa interrupcao via alarme */
    rtc_enable_interrupt(RTC, RTC_IER_ALREN);

}

void RTC_Handler(void)
{
    uint32_t ul_status = rtc_get_status(RTC);

    /*
    *  Verifica por qual motivo entrou
    *  na interrupcao, se foi por segundo
    *  ou Alarm
    */
    if ((ul_status & RTC_SR_SEC) == RTC_SR_SEC) {
        rtc_clear_status(RTC, RTC_SCCR_SECCLR);
    }

    /* Time or date alarm */
    if ((ul_status & RTC_SR_ALARM) == RTC_SR_ALARM) {
        rtc_clear_status(RTC, RTC_SCCR_ALRCLR);

        static int seconds_passed = 0;

        if (f_start) {
            rtc_set_date_alarm(RTC, 1, 0, 1, 0);
            int hora, min, sec;
            rtc_get_time(RTC, &hora, &min, &sec);
            if (sec >= 59) {
                if (min >= 59) {
                    rtc_set_time_alarm(RTC, 1, 0, hora+1, 0, 1, 0);
                } else {
                    rtc_set_time_alarm(RTC, 1, hora, 1, min+1, 1, 0);
                }
                seconds_passed = 0;
            } else {
                rtc_set_time_alarm(RTC, 1, hora, 1, min, 1, sec+1);
                seconds_passed += 1;
            }

            if (tempo_sec - seconds_passed >= 0) {
                char string[32];
                tempo_sec -= 1;
                sprintf(string, "%02d:%02d:%02d", tempo_sec/3600, tempo_sec%3600/60, tempo_sec%3600%60);
                font_draw_text(&calibri_36, string, 20, 128+60+40, 1);
            } else {
                f_draw_menu = 1;
                f_start = 0;

                seconds_passed = 0;
            }

        }

    }

    rtc_clear_status(RTC, RTC_SCCR_ACKCLR);
    rtc_clear_status(RTC, RTC_SCCR_TIMCLR);
    rtc_clear_status(RTC, RTC_SCCR_CALCLR);
    rtc_clear_status(RTC, RTC_SCCR_TDERRCLR);

}

void io_init(void)
{

    // Configura led
    pmc_enable_periph_clk(LED_PIO_ID);
    pio_configure(LED_PIO, PIO_OUTPUT_0, LED_PIO_IDX_MASK, PIO_DEFAULT);

    // Inicializa clock do periférico PIO responsavel pelo botao
    pmc_enable_periph_clk(BUT_PIO_ID);

    // Configura PIO para lidar com o pino do botão como entrada
    // com pull-up
    pio_configure(BUT_PIO, PIO_INPUT, BUT_PIO_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);

    // Configura interrupção no pino referente ao botao e associa
    // função de callback caso uma interrupção for gerada
    // a função de callback é a: but_callback()
    pio_handler_set(BUT_PIO,
                    BUT_PIO_ID,
                    BUT_PIO_IDX_MASK,
                    PIO_IT_RISE_EDGE,
                    but_callback);

    // Ativa interrupção
    pio_enable_interrupt(BUT_PIO, BUT_PIO_IDX_MASK);

    // Configura NVIC para receber interrupcoes do PIO do botao
    // com prioridade 4 (quanto mais próximo de 0 maior)
    NVIC_EnableIRQ(BUT_PIO_ID);
    NVIC_SetPriority(BUT_PIO_ID, 4);
}

int main(void)
{

    t_ciclo *p_current = initMenuOrder();

    struct mxt_device device;

    /* Initialize the USART configuration struct */
    const usart_serial_options_t usart_serial_options = {
        .baudrate     = USART_SERIAL_EXAMPLE_BAUDRATE,
        .charlength   = USART_SERIAL_CHAR_LENGTH,
        .paritytype   = USART_SERIAL_PARITY,
        .stopbits     = USART_SERIAL_STOP_BIT
    };

    sysclk_init(); /* Initialize system clocks */
    board_init();  /* Initialize board */

    io_init();

    configure_lcd();
    draw_screen();

    /* Initialize the mXT touch device */
    mxt_init(&device);

    /* Initialize stdio on USART */
    stdio_serial_init(USART_SERIAL_EXAMPLE, &usart_serial_options);

    RTC_init();

    close_door();

    ili9488_draw_pixmap(ILI9488_LCD_WIDTH/2-64, 20, 128, 128, p_current->icon->data);

    char string[32];
    sprintf(string, "Modo: %s", p_current->nome);
    font_draw_text(&calibri_36, string, 20, 128+30, 1);
    int tempo_min = p_current->enxagueTempo * p_current->enxagueQnt + p_current->centrifugacaoTempo;
    sprintf(string, "%d horas e %02d mins", tempo_min/60, tempo_min%60);
    font_draw_text(&calibri_36, string, 20, 128+30+40, 1);

    ili9488_draw_pixmap(ILI9488_LCD_WIDTH/2-80, 198+50, lavar.width, lavar.height, lavar.data);

    ili9488_draw_pixmap(ILI9488_LCD_WIDTH/2-80, 328+10, modo.width, modo.height, modo.data);

    ili9488_draw_pixmap(ILI9488_LCD_WIDTH/2-80, 398+10, configurar.width, configurar.height, configurar.data);

    ili9488_draw_pixmap(10,398+10,unlocked.width,unlocked.height,unlocked.data);

    while(1) {

        if (mxt_is_message_pending(&device)) {
            mxt_handler(&device);
        }

        if (f_draw_ready_unlock) {
            ili9488_draw_pixmap(10,398+10, LockedGreen.width, LockedGreen.height, LockedGreen.data);

            f_draw_ready_unlock = 0;
        }

        if (f_modo) {
            p_current = p_current->next;

            ili9488_draw_pixmap(ILI9488_LCD_WIDTH/2-64, 20, 128, 128, p_current->icon->data);

            ili9488_set_foreground_color(COLOR_CONVERT(COLOR_WHITE));
            ili9488_draw_filled_rectangle(0, 128+30, ILI9488_LCD_WIDTH, 128+30+40+38);

            sprintf(string, "Modo: %s", p_current->nome);
            font_draw_text(&calibri_36, string, 20, 128+30, 1);
            int tempo_min = p_current->enxagueTempo * p_current->enxagueQnt + p_current->centrifugacaoTempo;
            sprintf(string, "%d horas e %02d mins", tempo_min/60, tempo_min%60);
            font_draw_text(&calibri_36, string, 20, 128+30+40, 1);

            if (f_door_is_open && f_draw_start) {
                f_draw_door_is_open = 1;
            }

            f_modo = 0;
        }

        if(security_flag) {
            f_start=0;
            f_draw_menu=1;
            security_flag=0;
        }
        if (f_draw_config) {

            draw_screen();

            font_draw_text(&calibri_36, "Configuracao", 20, 30, 1);

            draw_config_enxague();
            draw_config_centrifuga();
            draw_config_adicionais();

            ili9488_set_foreground_color(COLOR_CONVERT(COLOR_GRAY));
            ili9488_draw_filled_rectangle(10, 398+10, 70, 398+10+60);
            ili9488_draw_pixmap(10,398+10,unlocked.width,unlocked.height,unlocked.data);

            ili9488_draw_pixmap(ILI9488_LCD_WIDTH/2-80, 398+10,voltar.width,voltar.height,voltar.data);

            f_draw_config = 0;
        }

        if (f_draw_start && !f_door_is_open) {

            draw_screen();

            ili9488_draw_pixmap(ILI9488_LCD_WIDTH/2-63,20,AnimaList[0]->width,AnimaList[0]->height,AnimaList[0]->data);
            sprintf(string, "%s", p_current->nome);
            font_draw_text(&calibri_36, string, 20, 128+60, 1);
            tempo_sec = (p_current->enxagueTempo * p_current->enxagueQnt + p_current->centrifugacaoTempo) * 60;
            sprintf(string, "%02d:%02d:%02d", tempo_sec/3600, tempo_sec%3600/60, tempo_sec%3600%60);
            font_draw_text(&calibri_36, string, 20, 128+60+40, 1);

            ili9488_draw_pixmap(ILI9488_LCD_WIDTH/2-80,398+10,cancelar.width,cancelar.height,cancelar.data);

            ili9488_set_foreground_color(COLOR_CONVERT(COLOR_GRAY));
            ili9488_draw_filled_rectangle(10, 398+10, 70, 398+10+60);
            ili9488_draw_pixmap(10,398+10,unlocked.width,unlocked.height,unlocked.data);

            rtc_set_date(RTC, 0, 0, 0, 0);
            rtc_set_time(RTC, 0, 0, 0);

            rtc_set_date_alarm(RTC, 0, 0, 0, 0);
            rtc_set_time_alarm(RTC, 0, 0, 0, 0, 1, 1);
            TC_init(TC0, ID_TC0, 0, 10);

            f_start = 1;

            f_draw_start = 0;
        }

        if (f_draw_menu) {

            tc_stop(TC0,0);
            draw_screen();

            ili9488_draw_pixmap(ILI9488_LCD_WIDTH/2-64, 20, 128, 128, p_current->icon->data);

            sprintf(string, "Modo: %s", p_current->nome);
            font_draw_text(&calibri_36, string, 20, 128+30, 1);
            int tempo_min = p_current->enxagueTempo * p_current->enxagueQnt + p_current->centrifugacaoTempo;
            sprintf(string, "%d horas e %02d mins", tempo_min/60, tempo_min%60);
            font_draw_text(&calibri_36, string, 20, 128+30+40, 1);

            ili9488_draw_pixmap(ILI9488_LCD_WIDTH/2-80, 198+50, lavar.width, lavar.height, lavar.data);

            ili9488_draw_pixmap(ILI9488_LCD_WIDTH/2-80, 328+10, modo.width, modo.height, modo.data);

            ili9488_draw_pixmap(ILI9488_LCD_WIDTH/2-80, 398+10, configurar.width, configurar.height, configurar.data);

            ili9488_draw_pixmap(10,398+10,unlocked.width,unlocked.height,unlocked.data);

            if (f_door_is_open && f_draw_start) {
                f_draw_door_is_open = 1;
            }

            f_draw_menu = 0;
        }

        if (f_draw_anim) {

            tc_disable_interrupt(TC0, 0, TC_IER_CPCS);
            tc_disable_interrupt(TC0, 1, TC_IER_CPCS);
            rtc_disable_interrupt(RTC, RTC_IER_ALREN);

            ili9488_draw_pixmap(ILI9488_LCD_WIDTH/2-63,20,AnimaList[anim_counter]->width,AnimaList[anim_counter]->height,AnimaList[anim_counter]->data);

            tc_enable_interrupt(TC0, 0, TC_IER_CPCS);
            tc_enable_interrupt(TC0, 1, TC_IER_CPCS);
            rtc_enable_interrupt(RTC, RTC_IER_ALREN);

            anim_counter = anim_counter + 1;

            f_draw_anim = 0;
        }

        if (f_draw_door_is_open) {
            font_draw_text(&calibri_36, "PORTA ABERTA!", 20, 30, 1);
            f_draw_door_is_open = 0;
        }

    }

    return 0;
}
