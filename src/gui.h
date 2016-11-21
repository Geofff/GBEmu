#pragma once
#include <stdio.h>
#include <stdint.h>
#include <gtk/gtk.h>

void launchGUI(char *, int, char**);
gboolean buttonPressCallback(GtkWidget*, GdkEventButton*, gpointer);
gboolean runningPressCallback(GtkWidget*, GdkEventButton*, gpointer);
gboolean keyPressCallback(GtkWidget*, GdkEventButton*, gpointer);
gboolean keyReleaseCallback(GtkWidget*, GdkEventButton*, gpointer);
gboolean configureCallback(GtkWidget*, GdkEventConfigure*, gpointer);
gboolean drawCallback(GtkWidget*, cairo_t*, gpointer);
gboolean configureTilesCallback(GtkWidget*, GdkEventConfigure*, gpointer);
gboolean stepCallback(GtkWidget*, GdkEventConfigure*, gpointer);
gboolean drawTilesCallback(GtkWidget*, cairo_t*, gpointer);

void redrawTiles();
void dumpMem();
void drawScreen();
gboolean idleCallback(gpointer);