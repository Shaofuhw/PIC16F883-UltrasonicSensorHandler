/*	Driver for HD44780 Compatible LCD Modules
	Copyright (C) 2014 Jesus Ruben Santa Anna Zamudio.

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.

	Author website: http://www.geekfactory.mx
	Author e-mail: ruben at geekfactory dot mx
 */
#ifndef LCD_H
#define LCD_H
/*-------------------------------------------------------------*/
/*		Includes and dependencies			*/
/*-------------------------------------------------------------*/
#include <stdint.h>

/*-------------------------------------------------------------*/
/*		Macros and definitions				*/
/*-------------------------------------------------------------*/
// Entry Mode Set Control Bits
#define BIT_S_AUTOSCROLL_ON	(1<<0)		//!< Enable autoscroll. For use with Entry Mode Set command
#define BIT_S_AUTOSCROLL_OFF	0		//!< Disable autoscroll. For use with Entry Mode Set command
#define BIT_ID_INCREMENT_CURSOR	(1<<1)		//!< Increment cursor position after each char. For use with Entry Mode Set command
#define BIT_ID_DECREMENT_CURSOR	0		//!< Decrement cursor position after each char. For use with Entry Mode Set command
// Display On/Off Control Bits
#define BIT_B_CURSOR_BLINK	(1<<0)
#define BIT_B_CURSOR_NO_BLINK	0
#define BIT_C_CURSOR_ON		(1<<1)
#define BIT_C_CURSOR_OFF	0
#define BIT_D_DISPLAY_ON	(1<<2)
#define BIT_D_DISPLAY_OFF	0
// Cursor / Display Shift Control Bits
#define BIT_RL_SHIFT_RIGHT	(1<<2)
#define BIT_RL_SHIFT_LEFT	0
#define BIT_SC_SHIFT_DISPLAY	(1<<3)		//!< Seting this bit causes a display scroll
#define BIT_SC_SHIFT_CURSOR	0		//!< Clearing this bits causes a cursor move
// Function set Control Bits
#define BIT_F_FONT_5_10		(1<<2)
#define BIT_F_FONT_5_8		0
#define BIT_N_DISP_LINES_2	(1<<3)
#define BIT_N_DISP_LINES_1	0
#define BIT_DL_DATALENGTH_8	(1<<4)
#define BIT_DL_DATALENGTH_4	0
// Define "boolean" values
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

/*-------------------------------------------------------------*/
/*		Typedefs enums & structs			*/
/*-------------------------------------------------------------*/

/**
 * Enumeration defining the HD44780 commands
 */
enum enLcdCommands {
	E_CLEAR_DISPLAY = 0x01,
	E_RETURN_HOME = 0x02,
	E_ENTRY_MODE_SET = 0x04,
	E_DISPLAY_ON_OFF_CTRL = 0x08,
	E_CURSOR_DISPLAY_SHIFT = 0x10,
	E_FUNCTION_SET = 0x20,
	E_SET_CGRAM_ADDR = 0x40,
	E_SET_DDRAM_ADDR = 0x80,
};

/**
 * This enumeration defines the available cursor modes
 */
enum enLCDCursorModes {
	E_LCD_CURSOR_OFF = 0x00,
	E_LCD_CURSOR_ON = 0x02,
	E_LCD_CURSOR_ON_BLINK = 0x03,
};

/**
 * Represents the IO pins used to control the LCD
 */
enum enLCDControlPins {
	E_D0_PIN = 0,
	E_D1_PIN,
	E_D2_PIN,
	E_D3_PIN,
	E_D4_PIN,
	E_D5_PIN,
	E_D6_PIN,
	E_D7_PIN,
	E_EN_PIN,
	E_RS_PIN,
	E_RW_PIN,
};

/*-------------------------------------------------------------*/
/*		Function prototypes				*/
/*-------------------------------------------------------------*/

/**
 * @brief Initializes the LCD driver
 *
 * This function prepares the resources needed to operate an HD44780 compatible
 * display. The IO Ports initialization and the LCD controller chip preparation
 * sequences are performed here.
 * 
 * The Initialization routine leaves the display disabled. To see the written
 * text you should enable it by calling lcd_enable() function.
 *
 * @param iores Pointer data to be used with the IO port initialization function
 * @param cols The number of columns present on the display (number of
 * horizontal characters)
 * @param rows The number of rows that the display can show (text lines)
 *
 * @return Returns true if the LCD was succesfully initialized, false otherwise
 */
uint8_t lcd_init(void * iodata, uint8_t cols, uint8_t rows);

/**
 * @brief  Clears the entire LCD display
 *
 * Clears the LCD and returns the cursor to the home position. All data on the
 * display controller�s RAM memory is cleared.
 */
void lcd_clear();

/**
 * @brief Returns the cursor to home position
 *
 * This function returns the cursor to the begining of the DDRAM memory without
 * affecting the it�s contents.
 */
void lcd_home();

/**
 * @brief Turns on the display and shows the DDRAM contents
 *
 * This function sends a command to turn the display ON and shows the contents of
 * the DDRAM on the screen. After the Initialization of the LCD, the display is
 * turned Off, so you need to call this function before you see anything on
 * screen.
 */
void lcd_on();

/**
 * @brief Turns off the display
 * 
 * This function turns off the display and hides the content of the DDRAM,
 * however the data on the DDRAM (display memory) is NOT cleared.
 */
void lcd_off();

/**
 * @brief Set the cursor display mode
 *
 * Turns on the cursor and sets the cursor display mode to one of the following:
 *
 * E_LCD_CURSOR_OFF - Don�t display the cursor (Default Setting).
 * E_LCD_CURSOR_ON - Show cursor as a small line under the letters
 * E_LCD_CURSOR_ON_BLINK - Shows cursor as a blinking square
 * 
 * As defined in enum enLCDCursorModes.
 *
 * @param emode The display mode of the cursor
 *
 */
void lcd_cursor(enum enLCDCursorModes emode);

/**
 * @brief Moves the cursor one position to the left
 *
 * Sends a command to move the cursor one position to the left
 */
void lcd_cursor_left();

/**
 * @brief Moves the cursor one position to the right
 *
 * Sends a command to move the cursor one position to the right
 */
void lcd_cursor_right();

/**
 * @brief Scrolls the display viewport to the left
 *
 * Scrolls the viewport one position to the left, cursor position is also
 * affected.
 */
void lcd_scroll_left();

/**
 * @brief Scrolls the display viewport to the right
 *
 * Scrolls the viewport one position to the right, cursor position is also
 * affected.
 */
void lcd_scroll_right();

/**
 * @brief Enables LCD autoscroll mode
 *
 * Enables autoscroll function when new characters are written to the LCD module.
 */
void lcd_autoscroll_on();

/**
 * @brief Disables LCD autoscroll mode
 *
 * Disables the autoscroll function.
 *
 */
void lcd_autoscroll_off();

/**
 * @brief Moves the cursor to the given position
 *
 * This functions sets the cursor position on the DDRAM. If cursor display is 
 * enabled (using lcd_cursor()), cursor will also be shown on the display.
 *
 * @param col The column of the LCD to place the cursor, with 0 being the
 * leftmost position on the display
 * @param row The row on the LCD to place the cursor, where the top row is 0
 */
void lcd_goto(uint8_t col, uint8_t row);

/**
 * @brief Writes data or a command to the LCD display
 *
 * This function writes a single character to the current cursor position OR
 * sends a command to the LCD display controller.
 * The second parameter "rs", indicates if the data is command or char,
 * and should be set to TRUE if the data presented to this function is a
 * character or FALSE if the data is a command to the LCD controller.
 *
 * @param data The data or command byte to send to the LCD
 * @param rs A boolean value indicating if the previous parameter was a
 * character to display (TRUE) or a command (FALSE) to the LCD controller
 *
 */
void lcd_send(uint8_t data, uint8_t rs);

/**
 * @brief Writes a character to DDRAM
 *
 * This is a macro that calls lcd_send() function to write a character to the
 * current DDRAM position.
 */
#define lcd_write( x ) lcd_send( x, TRUE )

/**
 * @brief Writes a command to the LCD controller
 *
 * This is a macro that calls lcd_send() function to write a command to the LCD
 * controller.
 */
#define lcd_command( x ) lcd_send( x, FALSE )

/**
 * @brief Writes a string to the current LCD position
 *
 * Writes a null terminated string to the current position on DDRAM
 *
 * @brief string Pointer to the string to write to the screen
 */
void lcd_puts(const uint8_t * string);

/**
 * @brief Writes a custom character to CGRAM
 *
 * This function allows the user to define up to 8 personalized characters that
 * can be displayed by sending the appropiate code using the vLCDWrite()
 * Function/Macro.
 * 
 * The custom character number and the char bitmap are passed to this function
 * and it writes to the proper CGRAM address.
 *
 * @param charnum The number for the custom character to define
 * @param chardata The character bitmap. Each custom character is composed of
 * 8 bytes which are read from the provided data buffer
 */
void lcd_create_char(uint8_t charnum, const uint8_t * chardata);

/*-------------------------------------------------------------*/
/*	External functions (library needs to link against this)	*/
/*-------------------------------------------------------------*/

/**
 * @brief Prepares the IO pins to be used with the LCD
 *
 * THIS FUNCTION SHOULD BE PROVIDED BY THE USER/PROGRAMMER.
 *
 * The function should prepare the IO pins, configure them as outputs and
 * default all the pins to low logic level.
 * 
 * The function should return 4 if the LCD will be operated on 4 bit mode and
 * return 8 if operating in 8 bit mode. If the function returs 0 or any other
 * value, it means it cannot properly initialize the IO pins.
 *
 * @param iodata A pointer to a structure containing data to operate on IO ports
 *
 * @return TRUE if LCD was initialized succesfully or FALSE otherwise
 */
uint8_t lcd_ioinit(void * iodata);

/**
 * @brief Sets the logic level of a control line or data bus line
 *
 * THIS FUNCTION SHOULD BE PROVIDED BY THE USER/PROGRAMMER.
 *
 * This function should output the indicated state on the selected pin. The pin
 * can be a control line or a data bus line.
 * 
 * @param pin The pin where the operation is performed
 * @param level	The new logic level for the indicated pin
 */
void lcd_ioset(enum enLCDControlPins pin, uint8_t level);

#define lcd_iohigh(x) lcd_ioset(x,TRUE)
#define lcd_iolow(x) lcd_ioset(x,FALSE)

/**
 * @brief Writes data in 4 bit mode
 *
 * THIS FUNCTION SHOULD BE PROVIDED BY THE USER/PROGRAMMER.
 *
 * This function writes to the data bus when operating in 4 bit mode. The
 * function should send a pulse on the E line of the LCD controller after
 * writting the data to the bus.
 *
 * @param data The data to write to the bus, writes only the lower 4 bits so 2
 * write operations are needed to write a full byte
 */
void lcd_iowrite4(uint8_t data);

/**
 * @brief Writes data in 8 bit mode
 *
 * THIS FUNCTION SHOULD BE PROVIDED BY THE USER/PROGRAMMER.
 *
 * This function writes to the data bus when operating in 8 bit mode. The
 * function should send a pulse on the E line of the LCD controller after
 * writting the data to the bus.
 *
 * @param data The data to write to the bus, writes the whole byte
 */
void lcd_iowrite8(uint8_t data);
#endif
// End of Header file
