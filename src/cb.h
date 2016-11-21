#pragma once
#include <stdint.h>

void (*cb_handlers[256])();
char *cb_names[256];
int cb_cycles[256];
int cb_implemented[256];
void *cb_args_1[256];
void *cb_args_2[256];
uint8_t cbcode;
void executeCB(uint8_t);
void setupCB();
#define REGISTER_CB(o, c, h, n, a, b) {cb_cycles[0x##o] = c; cb_handlers[0x##o] = cb_handler_##h; cb_names[0x##o] = n; cb_implemented[0x##o] = 1; cb_args_1[0x##o] = (void*)a;cb_args_2[0x##o] = b;}
