#ifndef __TIRPOD_H__
#define __TIRPOD_H__

#include "relay.h"
#include "stepmotor.h"

void tirpod_init(void);

void tirpod_limit_scan(uint8_t *x_limt, uint8_t *y_limt);
void tirpod_XY(uint16_t x_corrd, uint16_t y_corrd);

#endif
