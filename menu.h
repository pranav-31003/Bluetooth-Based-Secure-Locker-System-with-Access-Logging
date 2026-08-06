/*=============================================================================
 * File        : menu.h
 * Project     : Bluetooth-Based Secure Locker with Access Logging
 * Description : Public API for the admin menu, entered via an external
 *               push-button interrupt (EINT2). Provides a CLK Setting
 *               (Time/Date/Day) sub-menu, an Exit option, and the
 *               Password-change function, all built on SecureLocker's
 *               own lcd/keypad/buzzer/rtc/eeprom drivers.
 *===========================================================================*/
#ifndef MENU_H
#define MENU_H

void admin_int_init(void);   /* Configure P0.7 as EINT2 and enable the admin-button interrupt */
void admin_menu(void);       /* Run the interactive admin menu (blocking) until the user exits */

/* Set to 1 by the EINT2 ISR when the admin push-button is pressed; the
 * main loop checks this flag and calls admin_menu() when it is set. */
extern volatile unsigned char admin_flag;

#endif
