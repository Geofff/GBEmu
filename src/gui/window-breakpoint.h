#pragma once
#include <stdio.h>
#include <stdint.h>
#include <gtk/gtk.h>
#include <string.h>
void breakpointInitWindow(GtkBuilder *);
void addCommand();
void addLocation();

int processEntry(char * entry);
void breakShowWindow();
