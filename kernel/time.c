#include <LySys/time.h>
#include <asm/io.h>

#define CMOS_READ(addr) ({ \
    outb(0x70, 0x80 | (addr)); \
    inb(0x71); \
})

#define BCD_TO_BIN(val) ((val) = ((val) & 0x0F) + (((val) >> 4) * 10))

void time_init() {
    unsigned char status_b;
    unsigned char last_sec;

    do {
        time.tm_sec = CMOS_READ(0x00);
        time.tm_min = CMOS_READ(0x02);
        time.tm_hour = CMOS_READ(0x04);
        time.tm_mday = CMOS_READ(0x07);
        time.tm_mon = CMOS_READ(0x08);
        time.tm_year = CMOS_READ(0x09);
        last_sec = CMOS_READ(0x00);
    } while (time.tm_sec != last_sec);

    status_b = CMOS_READ(0x0B);

    if (!(status_b & 0x04)) {
        BCD_TO_BIN(time.tm_sec);
        BCD_TO_BIN(time.tm_min);
        BCD_TO_BIN(time.tm_hour);
        BCD_TO_BIN(time.tm_mday);
        BCD_TO_BIN(time.tm_mon);
        BCD_TO_BIN(time.tm_year);
    }

    if (!(status_b & 0x02) && (time.tm_hour & 0x80)) {
        time.tm_hour = ((time.tm_hour & 0x7F) + 12) % 24;
    }

    time.tm_mon -= 1;
    time.tm_year += 2000;
}

struct tm time;