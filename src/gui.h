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
gboolean tilesExposeEvent(GtkWidget *widget, GdkEventExpose *event);
gboolean drawExposeEvent(GtkWidget *widget, GdkEventExpose *event);
gboolean tilePressed(GtkWidget *widget, gpointer item);
gboolean breakPressed(GtkWidget *widget, gpointer item);
gboolean debugPressed(GtkWidget *widget, gpointer item);
gboolean openPressed(GtkWidget *widget, gpointer item);
gboolean disPressed(GtkWidget *widget, gpointer item);
gboolean reloadPressed(GtkWidget *widget, gpointer item);

void redrawTiles();
void dumpMem();
void drawScreen();
gboolean idleCallback(gpointer);