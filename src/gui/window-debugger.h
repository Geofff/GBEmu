#pragma once
#include <stdio.h>
#include <stdint.h>
#include <gtk/gtk.h>
#include <string.h>
void debuggerInitWindow(GtkBuilder *);

void updateLabels();
void updateLabel(char*, uint16_t );