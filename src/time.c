
/*
  The MEGA65 really only has a Real-Time Clock (RTC), so we just have handy
  functions for that.

  The RTC varies between revisions of the MEGA65, though, so we have to take
  that into account.
*/

#include <mega65/memory.h>
#include <mega65/time.h>
#include <mega65/targets.h>
#include <mega65/hal.h>

#define I2CDELAY 5000L

uint8_t bcd_work;

/**
 * @brief Convert a binary value to binary coded decimal (BCD)
 * @return uint8_t BCD value
 *
 * While the double dabble algorithm is more time efficient, we mostly care
 * about saving space, so use a simple loop.  Getting/setting time should not be
 * called particularly often.
 */
uint8_t tobcd(uint8_t in)
{
    if (in > 99)
        return 0;
    bcd_work = 0;
    while (in > 9) {
        bcd_work += 0x10;
        in -= 10;
    }
    bcd_work += in;
    return bcd_work;
}

/**
 * @brief Convert a binary coded decimal (BCD) value to binary
 * @param in Input BCD value
 * @return unsigned char Decimal value
 */
uint8_t unbcd(uint8_t in)
{
    bcd_work = 0;
    while (in & 0xf0) {
        bcd_work += 10;
        in -= 0x10;
    }
    bcd_work += in;
    return bcd_work;
}

void getrtc(struct m65_tm* tm)
{
    if (!tm) {
        return;
    }

    tm->tm_sec = 0;
    tm->tm_min = 0;
    tm->tm_hour = 0;
    tm->tm_mday = 0;
    tm->tm_mon = 0;
    tm->tm_year = 0;
    tm->tm_wday = 0;
    tm->tm_isdst = 0;

    switch (detect_target()) {
    case TARGET_EMULATION:
    case TARGET_MEGA65R2:
    case TARGET_MEGA65R3:
        tm->tm_sec = unbcd(lpeek_debounced(0xffd7110));
        tm->tm_min = unbcd(lpeek_debounced(0xffd7111));
        tm->tm_hour = lpeek_debounced(0xffd7112);
        if (tm->tm_hour & 0x80) {
            tm->tm_hour = unbcd(tm->tm_hour & 0x3f);
        }
        else {
            if (tm->tm_hour & 0x20) {
                tm->tm_hour = unbcd(tm->tm_hour & 0x1f) + 12;
            }
            else {
                tm->tm_hour = unbcd(tm->tm_hour & 0x1f);
            }
        }
        tm->tm_mday = unbcd(lpeek_debounced(0xffd7113));
        tm->tm_mon = unbcd(lpeek_debounced(0xffd7114));
        // RTC is based on 2000, not 1900
        tm->tm_year = unbcd(lpeek_debounced(0xffd7115)) + 100;
        tm->tm_wday = unbcd(lpeek_debounced(0xffd7116));
        tm->tm_isdst = lpeek_debounced(0xffd7117) & 0x20;
        break;
    case TARGET_MEGA65R4:
    case TARGET_MEGA65R5:
    case TARGET_MEGA65R6:
        tm->tm_sec = unbcd(lpeek_debounced(0xffd7110));
        tm->tm_min = unbcd(lpeek_debounced(0xffd7111));
        tm->tm_hour = unbcd(lpeek_debounced(0xffd7112) & 0x3f);
        tm->tm_mday = unbcd(lpeek_debounced(0xffd7113));
        tm->tm_mon = unbcd(lpeek_debounced(0xffd7114));
        // RTC is based on 2000, not 1900
        tm->tm_year = unbcd(lpeek_debounced(0xffd7115)) + 100;
        tm->tm_wday = unbcd(lpeek_debounced(0xffd7116));
        tm->tm_isdst = 0;
        break;
    case TARGET_MEGAPHONER1:
        break;
    default:
        return;
    }
}

void setrtc(struct m65_tm* tm)
{
    if (!tm) {
        return;
    }

    switch (detect_target()) {
    case TARGET_MEGA65R2:
    case TARGET_MEGA65R3:
        // Unlock RTC registers
        usleep(I2CDELAY);
        lpoke(0xffd7118, 0x41);

        usleep(I2CDELAY);
        lpoke(0xffd7110, tobcd(tm->tm_sec));
        usleep(I2CDELAY);
        lpoke(0xffd7111, tobcd(tm->tm_min));
        if (lpeek_debounced(0xffd7112) & 0x80) {
            usleep(I2CDELAY);
            lpoke(0xffd7112, tobcd(tm->tm_hour) | 0x80);
        }
        else {
            if (tm->tm_hour >= 12) {
                // PM
                usleep(I2CDELAY);
                lpoke(0xffd7112, tobcd(tm->tm_hour - 12) | 0x20);
            }
            else {
                // AM
                usleep(I2CDELAY);
                lpoke(0xffd7112, tobcd(tm->tm_hour));
            }
        }

        usleep(I2CDELAY);
        lpoke(0xffd7113, tobcd(tm->tm_mday));
        usleep(I2CDELAY);
        lpoke(0xffd7114, tobcd(tm->tm_mon));
        if (tm->tm_year >= 100 && tm->tm_year <= 199) {
            usleep(I2CDELAY);
            lpoke(0xffd7115, tobcd((uint8_t)(tm->tm_year - 100)));
        }
        usleep(I2CDELAY);
        lpoke(0xffd7116, tobcd(tm->tm_wday));
        usleep(I2CDELAY);
        if (tm->tm_isdst) {
            lpoke(0xffd7117, lpeek_debounced(0xffd7117) | 0x20);
        }
        else {
            lpoke(0xffd7117, lpeek_debounced(0xffd7117) & (0xff - 0x20));
        }

        // Re-lock RTC registers
        usleep(I2CDELAY);
        lpoke(0xffd7118, 0x01);

        break;
    case TARGET_MEGA65R4:
    case TARGET_MEGA65R5:
    case TARGET_MEGA65R6:
        usleep(I2CDELAY);
        lpoke(0xffd7110, tobcd(tm->tm_sec));
        usleep(I2CDELAY);
        lpoke(0xffd7111, tobcd(tm->tm_min));
        usleep(I2CDELAY);
        lpoke(0xffd7112, tobcd(tm->tm_hour)); // no need to set 24h mode, swiss RTC does not have AM/PM
        usleep(I2CDELAY);
        lpoke(0xffd7113, tobcd(tm->tm_mday));
        usleep(I2CDELAY);
        lpoke(0xffd7114, tobcd(tm->tm_mon));
        if (tm->tm_year >= 100 && tm->tm_year <= 199) {
            usleep(I2CDELAY);
            lpoke(0xffd7115, tobcd((unsigned char)tm->tm_year - 100));
        }
        usleep(I2CDELAY);
        lpoke(0xffd7116, tobcd(tm->tm_wday < 7 ? tm->tm_wday : 0));

        break;
    case TARGET_MEGAPHONER1:
        break;
    default:
        return;
    }
}
