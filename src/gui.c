#include "gui.h"
#include "gpu.h"
#include "gui/window-debugger.h"
#include "cpu.h"

// Storage of our drawing surface
cairo_surface_t *surface = NULL;
cairo_surface_t *tileSurface = NULL;
uint8_t image[8*8*4];
GtkWidget *drawingArea;
GtkWidget *tileArea;
GtkBuilder *builder;

void launchGUI(char *interfaceName, int argc, char **argv){
    GtkBuilder *builder;
    GObject *window;
    GObject *tileWindow;
    GObject *button;
    GObject *runningButton;
    gtk_init(&argc, &argv);
    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, interfaceName, NULL);

    /* Connect signal handlers to the constructed widgets. */
    window = GTK_WIDGET(gtk_builder_get_object (builder, "GBEmu"));
    g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
    tileWindow = GTK_WIDGET(gtk_builder_get_object (builder, "tileset"));
    g_signal_connect (tileWindow, "destroy", G_CALLBACK (gtk_main_quit), NULL);

    tileArea = gtk_builder_get_object (builder, "tilearea");
    g_signal_connect (tileArea, "draw",
                    G_CALLBACK (drawTilesCallback), NULL);
    g_signal_connect (tileArea, "configure-event",
                    G_CALLBACK (configureTilesCallback), NULL);

    // Button to switch running and not
    runningButton = gtk_builder_get_object (builder, "running");
    g_signal_connect (runningButton, "clicked", G_CALLBACK (runningPressCallback), NULL);
    debuggerInitWindow(builder);


    drawingArea = gtk_builder_get_object (builder, "drawingarea");
    g_signal_connect (drawingArea, "button-press-event",
                    G_CALLBACK (buttonPressCallback), NULL);
    g_signal_connect (drawingArea, "key-press-event",
                    G_CALLBACK (keyPressCallback), NULL);
    g_signal_connect (drawingArea, "key-release-event",
                    G_CALLBACK (keyReleaseCallback), NULL);
    g_signal_connect (drawingArea, "configure-event",
                    G_CALLBACK (configureCallback), NULL);
    g_signal_connect (drawingArea, "draw",
                    G_CALLBACK (drawCallback), NULL);

    button = gtk_builder_get_object (builder, "exit");
    g_signal_connect (button, "clicked", G_CALLBACK (gtk_main_quit), NULL);
    button = gtk_builder_get_object (builder, "dump");
    g_signal_connect (button, "clicked", G_CALLBACK (dumpMem), NULL);
    button = gtk_builder_get_object (builder, "button2");
    g_signal_connect (button, "clicked", G_CALLBACK (configureTilesCallback), NULL);
    button = gtk_builder_get_object (builder, "button3");
    g_signal_connect (button, "clicked", G_CALLBACK (redrawTiles), NULL);
    button = gtk_builder_get_object (builder, "step");
    g_signal_connect (button, "clicked", G_CALLBACK (stepCallback), NULL);
    g_timeout_add(1, idleCallback, NULL);
    gtk_main();
}

gboolean buttonPressCallback(GtkWidget *widget, GdkEventButton *event, gpointer data){
    printf("Button pressed\n");
}


gboolean runningPressCallback(GtkWidget *widget, GdkEventButton *event, gpointer data){
    printf("Running pressed\n");
    cpu.running = !cpu.running;
}

gboolean keyPressCallback(GtkWidget *widget, GdkEventButton *event, gpointer data){
    printf("Key pressed\n");
}

gboolean keyReleaseCallback(GtkWidget *widget, GdkEventButton *event, gpointer data){
    printf("Key Released\n");
}

gboolean configureCallback(GtkWidget *widget, GdkEventConfigure *event, gpointer data){
    for(int i = 0; i < 64; i++){
        image[i*4] = 0xFF;
        image[i*4+1] = 0x00;
        image[i*4+2] = 0x00;
        image[i*4+3] = 0xFF;
    }

    printf("Configuring...\n");
    
    if (surface){
        cairo_surface_destroy(surface);
    }
    surface = gdk_window_create_similar_surface(gtk_widget_get_window(widget), CAIRO_CONTENT_COLOR, gtk_widget_get_allocated_width(widget), gtk_widget_get_allocated_height(widget));
    cairo_t *cr = cairo_create(surface);
    
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_data((guchar*)image, GDK_COLORSPACE_RGB, TRUE, 8, 8, 8, 32, NULL, NULL);
    //GdkPixbuf *pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, 8, 8);
    gdk_cairo_set_source_pixbuf(cr, pixbuf, 0, 0);
    //cairo_set_source_rgb(cr, 1, 0, 0);
    //cairo_rectangle(cr, 20, 20, 6, 6);
    //cairo_fill(cr);
    cairo_paint(cr);
    cairo_destroy(cr);
    gtk_widget_queue_draw(drawingArea);

    return TRUE;
}

gboolean drawCallback(GtkWidget *widget, cairo_t *cr, gpointer data){
    printf("Drawing...\n");
    if (!surface){
        printf("Null in regular draw\n");
        return FALSE;
    }
    cairo_set_source_surface(cr, surface, 0, 0);
    cairo_paint (cr);
}

gboolean drawTilesCallback(GtkWidget *widget, cairo_t *cr, gpointer data){
    printf("Drawing tiles...\n");
    if (!tileSurface){
        printf("Null in tile draw\n");
        return FALSE;
    }
    cairo_set_source_surface(cr, tileSurface, 0, 0);
    cairo_paint (cr);
}

gboolean configureTilesCallback(GtkWidget *widget, GdkEventConfigure *event, gpointer data){


    printf("Configuring tiles...\n");
    updateLabels();
    
    if (tileSurface){
        cairo_surface_destroy(tileSurface);
    }
    tileSurface = gdk_window_create_similar_surface(gtk_widget_get_window(tileArea), CAIRO_CONTENT_COLOR, gtk_widget_get_allocated_width(tileArea), gtk_widget_get_allocated_height(tileArea));
    cairo_t *cr = cairo_create(tileSurface);
    
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_data((guchar*)image, GDK_COLORSPACE_RGB, TRUE, 8, 8, 8, 32, NULL, NULL);
    gdk_cairo_set_source_pixbuf(cr, pixbuf, 0, 0);
    cairo_paint(cr);
    cairo_destroy(cr);
    gtk_widget_queue_draw(tileArea);

    return TRUE;
}

void redrawTiles(){
    int tilesWide = 16;
    int tilesTall = 24;
    uint8_t imageMap[tilesWide*tilesTall*8*8*4];
    for(int i = 0; i < tilesWide * tilesTall; i++){
        for(int x = 0; x < 8; x++){
            for(int y = 0; y < 8; y++){
                uint32_t colour;
                switch(tiles[i][y][x]){
                    case 0x0:
                        colour = 0xFFFFFF;
                        break;
                    case 0x1:
                        colour = 0xFF0000;
                        break;
                    case 0x2:
                        colour = 0x00FF00;
                        break;
                    case 0x3:
                        colour = 0x0000FF;
                        break;
                }
                uint16_t x1 = (i%tilesWide)*8+x;
                uint16_t y1 = (i/tilesWide)*8+y;
                imageMap[(y1*tilesWide*8+x1)*4] = (colour & 0xFF0000) >> 16;
                imageMap[(y1*tilesWide*8+x1)*4+1] = (colour & 0xFF00) >> 8;
                imageMap[(y1*tilesWide*8+x1)*4+2] = colour & 0xFF;
                imageMap[(y1*tilesWide*8+x1)*4+3] = 0xFF;
            }
        }
    }
    cairo_t *cr = cairo_create(tileSurface);
    cairo_scale(cr, 2, 2);
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_data((guchar*)imageMap, GDK_COLORSPACE_RGB, TRUE, 8, tilesWide*8, tilesTall*8, tilesWide*8*4, NULL, NULL);
    gdk_cairo_set_source_pixbuf(cr, pixbuf, 0, 0);
    cairo_paint(cr);
    cairo_destroy(cr);
    gtk_widget_queue_draw(tileArea);
}

void dumpMem(){    
    FILE *vdump = fopen("dump/VRAM.dump","wb");
    fwrite(VRAM, sizeof(uint8_t), VRAM_SIZE, vdump);
    fclose(vdump);
    FILE *wdump = fopen("dump/WRAM.dump","wb");
    fwrite(WRAM, sizeof(uint8_t), WRAM_SIZE, wdump);
    fclose(wdump);
}

gboolean idleCallback(gpointer data){
    if (cpu.running){
        executeOpcode();
        updateLabels();
        gpuTick();
        if (gpu.draw){
            redrawTiles();
            gpu.draw = 0;
        }
    }
    return TRUE;
}


gboolean stepCallback(GtkWidget *widget, GdkEventConfigure *event, gpointer data){executeOpcode();
        updateLabels();
        gpuTick();
        if (gpu.draw){
            redrawTiles();
            gpu.draw = 0;
        }
}