#ifndef PSF_H
#define PSF_H

#include <LySys/types.h>

#define PSF1_MAGIC 0x0436

struct psf1_header {
    uint16_t magic;
    uint8_t mode;
    uint8_t charsize;
};

extern uint8_t _binary_zap_light16_psf_start;
extern uint8_t _binary_zap_light16_psf_end;

#endif