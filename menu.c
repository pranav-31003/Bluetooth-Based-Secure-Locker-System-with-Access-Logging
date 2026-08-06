/*=============================================================================
 * File        : menu.c
 * Project     : Bluetooth-Based Secure Locker with Access Logging
 * Description : Admin configuration menu, entered by pressing an external
 *               push-button wired to trigger EINT2 on P0.7.
 *
 *               MERGE NOTE: the CLK Setting feature below was brought in
 *               from the separate "EnviroTime" project, and rewritten
 *               here to call SecureLocker's own drivers (lcd_*, keypad_*,
 *               buzzer_*, rtc registers, eeprom_*) instead of EnviroTime's
 *               driver API. An Alarm feature was previously merged in
 *               alongside it; it has since been removed (not required for
 *               this project) and option 2 is now a plain Exit instead.
 *
 * MENU STRUCTURE (after the admin button / EINT2 is pressed):
 *   Line1: "1=CLK  2=Exit"      Line2: "3=Pwd  4=Set"
 *     1 = CLK Setting -> 1=Time (HH:MM:SS) 2=Date (DD/MM/YYYY) 3=Day (0=SUN..6=SAT)
 *     2 = Exit         -> leave the admin menu immediately, return to normal operation
 *     3 = Password     -> choose L1 (Bluetooth) or L2 (keypad), then
 *                          old/new/confirm, same as the original menu
 *     4 = Set          -> same as Exit (saves/confirms and returns to normal operation)
 *   'D' at any point   -> cancel / go back one level (also exits at the top level)
 *   '*'                -> backspace while entering a value
 *   '#'                -> confirm the value being entered
 *
 *   If no key is pressed for 15 s while the top-level menu is showing,
 *   the menu automatically exits back to normal operation (ATM-style
 *   idle timeout).
 *
 *   Every admin action (entering CLK Setting, entering Password, and
 *   exiting the menu) is written to the UART0 audit log via log_event(),
 *   which automatically stamps it with the current RTC date/time - see
 *   security.c.
 *
 * Wiring:
 *   Admin push-button -> P0.7 (configured as EINT2, falling edge trigger)
 *===========================================================================*/
#include <lpc214x.h>
#include <string.h>
#include "delay.h"
#include "menu.h"
#include "defines.h"
#include "lcd.h"
#include "keypad.h"
#include "rtc.h"
#include "eeprom.h"
#include "buzzer.h"
#include "security.h"

/* Flag raised by the EINT2 ISR; polled by the main loop in
 * projectmain.c to decide when to invoke admin_menu(). */
volatile unsigned char admin_flag = 0;

/*------------------------------------------------------------
 * Private helper prototypes
 *------------------------------------------------------------*/
static void edit_passwords(void);
static void read_password_masked(char *buf);
static s32  input_value(const char *prompt, s32 minVal, s32 maxVal);
static void edit_time(void);
static void edit_date(void);
static void edit_day(void);
static void clk_setting(void);

/* External interrupt 2 (EINT2) handler, fired when the admin push-button
 * is pressed. Just raises a flag and clears the pending interrupt; the
 * actual menu logic runs later from the main loop, not from inside the
 * ISR, to keep the ISR itself short. */
__irq void EINT2_ISR(void)
{
    admin_flag = 1;
    EXTINT = (1UL << ADMIN_EINT_BIT);   /* Clear the EINT2 pending flag */
    VICVectAddr = 0;                     /* Acknowledge to the VIC (end-of-interrupt) */
}

/* Configure P0.7 for its EINT2 alternate function and arm the interrupt:
 * falling-edge triggered (EXTMODE = edge, EXTPOLAR = falling), routed
 * through VIC channel 16 to the EINT2_ISR handler above. */
void admin_int_init(void)
{
    PINSEL0 &= ~(3UL << 14);
    PINSEL0 |=  (3UL << 14);        /* P0.7 -> EINT2 alternate function */

    EXTMODE  |= (1UL << ADMIN_EINT_BIT);   /* Edge-sensitive (not level) */
    EXTPOLAR &= ~(1UL << ADMIN_EINT_BIT);  /* Falling edge (button press pulls the line low) */
    EXTINT    = (1UL << ADMIN_EINT_BIT);   /* Clear any stale pending flag */

    VICIntSelect &= ~(1UL << ADMIN_VIC_SRC);  /* EINT2 (VIC source 16) -> IRQ, not FIQ */
    VICVectAddr2  = (u32)EINT2_ISR;           /* Register the ISR in vectored slot 2   */
    VICVectCntl2  = 0x20 | ADMIN_VIC_SRC;     /* Enable slot 2, assign it to source 16 */
    VICIntEnable  = (1UL << ADMIN_VIC_SRC);   /* Unmask the EINT2 interrupt source     */
}

/*============================================================
 * input_value  (PRIVATE)  -- adapted from EnviroTime's InputValue()
 * Shows 'prompt' on LCD line 1, collects digit key presses on
 * line 2, validates against [minVal, maxVal], and returns the
 * value once confirmed with '#'.
 *
 *   '0'-'9' -> append digit      '*' -> backspace
 *   '#'     -> confirm entry     'D' -> cancel (returns -1)
 *============================================================*/
static s32 input_value(const char *prompt, s32 minVal, s32 maxVal)
{
    s32 value    = 0;
    u8  hasInput = 0U;
    char key;

    lcd_clear();
    lcd_gotoxy(0, 0);
    lcd_string(prompt);
    lcd_gotoxy(1, 0);
    lcd_data('>');

    while (1)
    {
        key = keypad_getkey();

        if (key == '#')
        {
            if (hasInput == 0U)
                continue;   /* Nothing entered yet - ignore confirm */

            if ((value >= minVal) && (value <= maxVal))
                return value;

            /* Out of range: show an error, then let the admin retry */
            lcd_clear();
            lcd_gotoxy(0, 0);
            lcd_string("Out of Range!");
            lcd_gotoxy(1, 0);
            lcd_string("Min:");
            lcd_int(minVal);
            lcd_string(" Max:");
            lcd_int(maxVal);
            delay_ms(1500);

            value    = 0;
            hasInput = 0U;
            lcd_clear();
            lcd_gotoxy(0, 0);
            lcd_string(prompt);
            lcd_gotoxy(1, 0);
            lcd_data('>');
        }
        else if (key == '*')
        {
            /* Backspace: drop the last digit and redraw line 2 */
            value /= 10;
            lcd_gotoxy(1, 0);
            lcd_string("     ");
            lcd_gotoxy(1, 0);
            lcd_data('>');
            if (value > 0)
                lcd_int(value);
            else
                hasInput = 0U;
        }
        else if (key == 'D')
        {
            return -1;   /* Cancel */
        }
        else if ((key >= '0') && (key <= '9'))
        {
            if (value < 9999)
            {
                value    = (value * 10) + (key - '0');
                hasInput = 1U;
                lcd_gotoxy(1, 0);
                lcd_data('>');
                lcd_int(value);
            }
        }
        /* Any other key is ignored */
    }
}

/* CLK Setting sub-menu: edit RTC hour/minute/second. */
static void edit_time(void)
{
    s32 val;
    char key;

    while (1)
    {
        lcd_clear();
        lcd_string("1H 2M 3S");
        lcd_gotoxy(1, 0);
        lcd_string("D=Back");

        key = keypad_getkey();

        switch (key)
        {
            case '1':
                val = input_value("Set Hour(0-23)", 0, 23);
                if (val >= 0) { HOUR = (u32)val; log_event("CLK Setting: Hour updated"); }
                break;
            case '2':
                val = input_value("Set Min(0-59)", 0, 59);
                if (val >= 0) { MIN = (u32)val; log_event("CLK Setting: Minute updated"); }
                break;
            case '3':
                val = input_value("Set Sec(0-59)", 0, 59);
                if (val >= 0) { SEC = (u32)val; log_event("CLK Setting: Second updated"); }
                break;
            case 'D':
                return;
            default:
                break;
        }
    }
}

/* CLK Setting sub-menu: edit RTC day-of-month/month/year. */
static void edit_date(void)
{
    s32 val;
    char key;

    while (1)
    {
        lcd_clear();
        lcd_string("1D 2M 3Y");
        lcd_gotoxy(1, 0);
        lcd_string("D=Back");

        key = keypad_getkey();

        switch (key)
        {
            case '1':
                val = input_value("Set Date(1-31)", 1, 31);
                if (val >= 0) { DOM = (u32)val; log_event("CLK Setting: Date (day-of-month) updated"); }
                break;
            case '2':
                val = input_value("Set Month(1-12)", 1, 12);
                if (val >= 0) { MONTH = (u32)val; log_event("CLK Setting: Month updated"); }
                break;
            case '3':
                val = input_value("Set Year(2000-)", 2000, 4095);
                if (val >= 0) { YEAR = (u32)val; log_event("CLK Setting: Year updated"); }
                break;
            case 'D':
                return;
            default:
                break;
        }
    }
}

/* CLK Setting sub-menu: edit the RTC day-of-week (0=SUN .. 6=SAT). */
static void edit_day(void)
{
    s32 val;

    lcd_clear();
    lcd_string("0=SUN 6=SAT");
    delay_ms(1000);

    val = input_value("Set Day(0-6)", 0, 6);

    if (val >= 0)
    {
        DOW = (u32)val;
        lcd_clear();
        lcd_string("Day Updated!");
        log_event("CLK Setting: Day-of-week updated");
        delay_ms(1000);
    }
}

/* Unified "CLK Setting" sub-menu consolidating Time/Date/Day. */
static void clk_setting(void)
{
    char key;

    while (1)
    {
        lcd_clear();
        lcd_string("1=Time 2=Date");
        lcd_gotoxy(1, 0);
        lcd_string("3=Day  D=Back");

        key = keypad_getkey();

        switch (key)
        {
            case '1': edit_time(); break;
            case '2': edit_date(); break;
            case '3': edit_day();  break;
            case 'D': return;
            default:  break;
        }
    }
}

/*============================================================
 * read_password_masked  (PRIVATE)
 * Reads exactly PWD_LEN digits from the keypad onto LCD row 1,
 * col 0, masking each with '*'. Unlike a plain fixed-count loop,
 * this supports correcting a mistake before all digits are in:
 *
 *   '0'-'9' -> append digit (masked with '*')
 *   '*'     -> backspace: erase the last digit entered
 *   '#'     -> clear the entire entry so far, start over
 *
 * Returns once exactly PWD_LEN digits have been accepted.
 *============================================================*/
static void read_password_masked(char *buf)
{
    u8 i = 0;
    char key;

    lcd_gotoxy(1, 0);

    while (i < PWD_LEN)
    {
        key = keypad_getkey();

        if ((key >= '0') && (key <= '9'))
        {
            buf[i++] = key;
            lcd_data('*');
        }
        else if (key == '*')   /* Backspace: erase the previous digit */
        {
            if (i > 0)
            {
                i--;
                buf[i] = '\0';
                lcd_gotoxy(1, i);
                lcd_data(' ');
                lcd_gotoxy(1, i);
            }
        }
        else if (key == '#')   /* Clear: erase the whole entry so far */
        {
            while (i > 0)
            {
                i--;
                buf[i] = '\0';
                lcd_gotoxy(1, i);
                lcd_data(' ');
            }
            lcd_gotoxy(1, 0);
        }
        /* Any other key is ignored */
    }

    buf[PWD_LEN] = '\0';
}

/* Password sub-menu (unchanged from the original SecureLocker admin
 * menu): choose which stored password to change (L1 = Bluetooth,
 * L2 = keypad), verify the current password, then read and confirm a
 * new one before committing it to EEPROM. */
static void edit_passwords(void)
{
    char oldp[PWD_LEN + 1];
    char newp[PWD_LEN + 1];
    char conf[PWD_LEN + 1];
    char cur[PWD_LEN + 1];
    unsigned int addr;
    char sel;

    lcd_clear();
    lcd_string("1:L1 2:L2");
    sel = keypad_getkey();

    if (sel == '1')      addr = EEPROM_L1_ADDR;
    else if (sel == '2') addr = EEPROM_L2_ADDR;
    else                 return;

    eeprom_read_str(addr, cur, PWD_LEN);

    lcd_clear();
    lcd_string("OLD PWD:");
    read_password_masked(oldp);

    if (strcmp(oldp, cur) != 0)
    {
        lcd_clear();
        lcd_string("WRONG OLD");
        log_event("Password change failed: wrong old password");
        buzzer_alert(3);
        delay_ms(1000);
        return;
    }

    lcd_clear();
    lcd_string("NEW PWD:");
    read_password_masked(newp);

    lcd_clear();
    lcd_string("CONFIRM:");
    read_password_masked(conf);

    if (strcmp(newp, conf) == 0)
    {
        char readback[PWD_LEN + 1];

        /* Write the new password to the AT24C256 EEPROM. EEPROM is
         * non-volatile, so this survives a power cycle/reset - but as
         * a safety check, immediately read it back and compare before
         * declaring success, in case of a bus glitch during the write. */
        eeprom_write_str(addr, newp, PWD_LEN);
        eeprom_read_str(addr, readback, PWD_LEN);

        if (strcmp(readback, newp) == 0)
        {
            lcd_clear();
            lcd_string("PWD UPDATED");
            log_event("Password updated and verified in EEPROM");
        }
        else
        {
            /* Read-back didn't match what was written - retry once
             * before giving up, since this should be rare. */
            eeprom_write_str(addr, newp, PWD_LEN);
            eeprom_read_str(addr, readback, PWD_LEN);

            if (strcmp(readback, newp) == 0)
            {
                lcd_clear();
                lcd_string("PWD UPDATED");
                log_event("Password updated and verified in EEPROM (after retry)");
            }
            else
            {
                lcd_clear();
                lcd_string("EEPROM WRITE FAIL");
                log_event("Password update FAILED: EEPROM read-back mismatch");
                buzzer_alert(3);
            }
        }
    }
    else
    {
        lcd_clear();
        lcd_string("MISMATCH");
        log_event("Password update failed: mismatch");
        buzzer_alert(3);
    }

    delay_ms(1000);
}

/* Top-level admin menu, entered from the main loop whenever admin_flag
 * is set (i.e. the admin push-button was pressed). Presents the 3-option
 * menu (CLK Setting / Exit / Password) and dispatches to the relevant
 * sub-menu. If no key is pressed for 15 seconds while this top-level
 * menu is showing, it exits automatically (ATM-style idle timeout). */
void admin_menu(void)
{
#define MENU_TIMEOUT_MS  15000U   /* 15 seconds idle -> auto-exit */
#define MENU_POLL_MS     100U     /* Check for a key every 100 ms */
#define MENU_IDLE_LIMIT  (MENU_TIMEOUT_MS / MENU_POLL_MS)

    char key;
    u8   stay      = 1U;
    u32  idleCount = 0U;

    admin_flag = 0;
    log_event("Admin menu entered");

    while (stay == 1U)
    {
        lcd_clear();
        lcd_string("1=CLK  2=Exit");
        lcd_gotoxy(1,0);
        lcd_string("3=Pwd  4=Set");

        /* Idle-wait for a key press, using the non-blocking keypad_scan()
         * so the 15-second auto-timeout can be measured. */
        idleCount = 0U;
        key = 0;
        while (key == 0)
        {
            key = keypad_scan();
            if (key != 0)
                break;

            delay_ms(MENU_POLL_MS);
            idleCount++;
            if (idleCount >= MENU_IDLE_LIMIT)
            {
                lcd_clear();
                lcd_string("Menu Timeout");
                lcd_gotoxy(1,0);
                lcd_string("Returning...");
                delay_ms(1200);
                admin_flag = 0;
                return;
            }
        }

        switch (key)
        {
            case '1':
                log_event("CLK Setting menu accessed");
                clk_setting();
                break;

            case '2':
            case 'D':
                lcd_clear();
                lcd_string("Exiting Menu");
                lcd_gotoxy(1,0);
                lcd_string("Returning...");
                buzzer_on();
                delay_ms(150);
                buzzer_off();
                delay_ms(1000);
                stay = 0U;
                break;

            case '3':
                log_event("Password menu accessed");
                edit_passwords();
                break;

            case '4':
                lcd_clear();
                lcd_string("Settings Saved");
                lcd_gotoxy(1,0);
                lcd_string("Returning...");
                buzzer_on();
                delay_ms(150);
                buzzer_off();
                delay_ms(1200);
                stay = 0U;
                break;

            default:
                break;
        }
    }

    log_event("Admin menu exit");
    admin_flag = 0;
}
