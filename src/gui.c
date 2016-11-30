#include "gui.h"
#include "gpu.h"
#include "gui/window-debugger.h"
#include "gui/window-breakpoint.h"
#include "gui/window-disassembly.h"
#include "cpu.h"
#include <glib.h>
#define TILES_WIDE 16
#define TILES_TALL 24
// Storage of our drawing surface
cairo_surface_t *surface = NULL;
cairo_surface_t *tileSurface = NULL;
extern uint8_t fullMap[160*144*4];
GtkWidget *drawingArea;
GtkWidget *tileArea;
GtkBuilder *builder;
gint64 lastTime;

void launchGUI(char *interfaceName, int argc, char **argv){
    GtkBuilder *builder;
    GObject *window;
    GObject *tileWindow;
    GObject *button;
    GObject *runningButton;
    gtk_init(&argc, &argv);
    builder = gtk_builder_new();
	GError *error = NULL;
    int result = gtk_builder_add_from_file(builder, interfaceName, &error);

	printf("Result was %d\n", result);
	if (!result) {
		g_warning("Encountered an error: %s\n", error->message);
	}
    /* Connect signal handlers to the constructed widgets. */
	printf("Loading main window...");
    window = gtk_builder_get_object (builder, "GBEmu");
    g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
	gtk_widget_show(window);
	printf("Loaded\n");

	// Tileset window rendering
    tileWindow = GTK_WIDGET(gtk_builder_get_object (builder, "tileset"));

    tileArea = gtk_builder_get_object (builder, "tilearea");
    g_signal_connect (tileArea, "expose-event",
                    G_CALLBACK (tilesExposeEvent), NULL);
    g_signal_connect (tileArea, "configure-event",
                    G_CALLBACK (configureTilesCallback), NULL);

    // Button to switch running and not
    runningButton = gtk_builder_get_object (builder, "running");
    g_signal_connect (runningButton, "clicked", G_CALLBACK (runningPressCallback), NULL);

	/*
		Menu buttons
	*/
	GObject *menuItem = gtk_builder_get_object(builder, "menu_exit");
	g_signal_connect(menuItem, "activate", G_CALLBACK(gtk_main_quit), NULL);
	// TODO connect open
	menuItem = gtk_builder_get_object(builder, "menu_open");
	g_signal_connect(menuItem, "activate", G_CALLBACK(openPressed), NULL);
	menuItem = gtk_builder_get_object(builder, "menu_reload");
	g_signal_connect(menuItem, "activate", G_CALLBACK(reloadPressed), NULL);
	// TODO about menu
	menuItem = gtk_builder_get_object(builder, "menu_about");
	g_signal_connect(menuItem, "activate", G_CALLBACK(gtk_main_quit), NULL);
	menuItem = gtk_builder_get_object(builder, "menu_tile");
	g_signal_connect(menuItem, "activate", G_CALLBACK(tilePressed), NULL);
	menuItem = gtk_builder_get_object(builder, "menu_break");
	g_signal_connect(menuItem, "activate", G_CALLBACK(breakPressed), NULL);
	menuItem = gtk_builder_get_object(builder, "menu_debug");
	g_signal_connect(menuItem, "activate", G_CALLBACK(debugPressed), NULL);
	// TODO Dissasmbler menu
	menuItem = gtk_builder_get_object(builder, "menu_dis");
	g_signal_connect(menuItem, "activate", G_CALLBACK(disPressed), NULL);



    debuggerInitWindow(builder); 
	breakpointInitWindow(builder);
	disassemblerInitWindow(builder);

	// Main drawing area rendering
    drawingArea = gtk_builder_get_object (builder, "drawingarea");
    g_signal_connect (drawingArea, "button-press-event",
                    G_CALLBACK (buttonPressCallback), NULL);
    g_signal_connect (drawingArea, "key-press-event",
                    G_CALLBACK (keyPressCallback), NULL);
    g_signal_connect (drawingArea, "key-release-event",
                    G_CALLBACK (keyReleaseCallback), NULL);
    g_signal_connect (drawingArea, "configure-event",
                    G_CALLBACK (configureCallback), NULL);
    g_signal_connect (drawingArea, "expose-event",
                    G_CALLBACK (drawExposeEvent), NULL);

	// Main controls

    button = gtk_builder_get_object (builder, "exit");
    g_signal_connect (button, "clicked", G_CALLBACK (gtk_main_quit), NULL);
    button = gtk_builder_get_object (builder, "dump");
    g_signal_connect (button, "clicked", G_CALLBACK (dumpMem), NULL);
    button = gtk_builder_get_object (builder, "button2");
    g_signal_connect (button, "clicked", G_CALLBACK (configureTilesCallback), NULL);
    g_signal_connect (button, "clicked", G_CALLBACK (configureCallback), NULL);
    button = gtk_builder_get_object (builder, "button3");
    g_signal_connect (button, "clicked", G_CALLBACK (drawScreen), NULL);
    g_signal_connect (button, "clicked", G_CALLBACK (redrawTiles), NULL);
    button = gtk_builder_get_object (builder, "step");
    g_signal_connect (button, "clicked", G_CALLBACK (stepCallback), NULL);
    g_timeout_add(1, idleCallback, NULL);
	lastTime = 0;
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
    printf("Configuring...\n");
    
    if (surface){
        cairo_surface_destroy(surface);
    }
    surface = gdk_window_create_similar_surface(gtk_widget_get_window(drawingArea), CAIRO_CONTENT_COLOR, 160, 144);
    return TRUE;
}

gboolean drawCallback(GtkWidget *widget, cairo_t *cr, gpointer data){
    printf("Drawing...\n");
    if (!surface){
        printf("Null in regular draw\n");
        return FALSE;
    }

    cairo_set_source_surface(cr, surface, 0, 0);
	printf("surface set ok\n");
    cairo_paint (cr);
	return;
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

gboolean drawExposeEvent(GtkWidget * widget, GdkEventExpose * event) {
	GdkRegion *region;
	GtkWidget *child;
	cairo_t *cr;
	uint8_t imageMap[160 * 144 * 4];
	for (int i = 0; i < 160 * 144 * 4; i++) {
		imageMap[i] = 0x00;
	}

	for (int x = 0; x < 20; x++) {
		for (int y = 0; y < 18; y++) {
			uint8_t tile = VRAM[0x9800 + y * 20 + x];
			for (int xx = 0; xx < 8; xx++) {
				for (int yy = 0; yy < 8; yy++) {
					int offset = (y * 8 + yy)*(160 * 4) + (x * 8 + xx) * 4;
					uint8_t pallete = tiles[tile][yy][xx];
					switch (pallete) {
						case 0:
							imageMap[offset + 0] = 0xFF;
							imageMap[offset + 1] = 0xFF;
							imageMap[offset + 2] = 0xFF;
							imageMap[offset + 3] = 0xFF;
							break;
						case 1:
							imageMap[offset + 0] = 0xFF;
							imageMap[offset + 1] = 0x00;
							imageMap[offset + 2] = 0x00;
							imageMap[offset + 3] = 0xFF;
							break;
						case 2:
							imageMap[offset + 0] = 0x00;
							imageMap[offset + 1] = 0xFF;
							imageMap[offset + 2] = 0x00;
							imageMap[offset + 3] = 0xFF;
							break;
						case 3:
							imageMap[offset + 0] = 0x00;
							imageMap[offset + 1] = 0x00;
							imageMap[offset + 2] = 0xFF;
							imageMap[offset + 3] = 0xFF;
							break;
					}
				}
			}
		}
	}


	cr = gdk_cairo_create(widget->window);
	GdkPixbuf *pixbuf = gdk_pixbuf_new_from_data((guchar*)imageMap, GDK_COLORSPACE_RGB, TRUE, 8, 160, 144, 160 * 4, NULL, NULL);
	gdk_cairo_set_source_pixbuf(cr, pixbuf, 0, 0);
	cairo_paint(cr);
	cairo_destroy(cr);
	return FALSE;
}

gboolean tilePressed(GtkWidget * widget, gpointer item) {
	GObject *window = gtk_builder_get_object(builder, "tileset");
	gtk_widget_show(window);
	return FALSE;
}

gboolean disPressed(GtkWidget * widget, gpointer item) {
	disassemblerShowWindow();
	return FALSE;
}

gboolean tilesExposeEvent(GtkWidget * widget, GdkEventExpose * event) {
	uint8_t imageMap[TILES_WIDE*TILES_TALL * 8 * 8 * 4];
	for (int i = 0; i < TILES_WIDE * TILES_TALL; i++) {
		for (int x = 0; x < 8; x++) {
			for (int y = 0; y < 8; y++) {
				uint32_t colour;
				switch (tiles[i][y][x]) {
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
				uint16_t x1 = (i%TILES_WIDE) * 8 + x;
				uint16_t y1 = (i / TILES_WIDE) * 8 + y;
				imageMap[(y1*TILES_WIDE * 8 + x1) * 4] = (colour & 0xFF0000) >> 16;
				imageMap[(y1*TILES_WIDE * 8 + x1) * 4 + 1] = (colour & 0xFF00) >> 8;
				imageMap[(y1*TILES_WIDE * 8 + x1) * 4 + 2] = colour & 0xFF;
				imageMap[(y1*TILES_WIDE * 8 + x1) * 4 + 3] = 0xFF;
			}
		}
	}
	cairo_t *cr = gdk_cairo_create(widget->window);
	cairo_scale(cr, 2, 2);
	GdkPixbuf *pixbuf = gdk_pixbuf_new_from_data((guchar*)imageMap, GDK_COLORSPACE_RGB, TRUE, 8, TILES_WIDE * 8, TILES_TALL * 8, TILES_WIDE * 8 * 4, NULL, NULL);
	gdk_cairo_set_source_pixbuf(cr, pixbuf, 0, 0);
	cairo_paint(cr);
	cairo_destroy(cr);
	gtk_widget_queue_draw(tileArea);
	return FALSE;
}

gboolean configureTilesCallback(GtkWidget *widget, GdkEventConfigure *event, gpointer data){


    printf("Configuring tiles...\n");
    updateLabels();
    
    if (tileSurface){
        cairo_surface_destroy(tileSurface);
    }
    tileSurface = gdk_window_create_similar_surface(gtk_widget_get_window(tileArea), CAIRO_CONTENT_COLOR, TILES_WIDE*8*2, TILES_TALL*8*2);
    return TRUE;
}

gboolean breakPressed(GtkWidget * widget, gpointer item) {
	breakShowWindow();
	return FALSE;
}

gboolean debugPressed(GtkWidget * widget, gpointer item) {
	debugShowWindow();
	return FALSE;
}

gboolean openPressed(GtkWidget * widget, gpointer item) {
	GtkWidget *dialog;
	GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
	gint res;
	dialog = gtk_file_chooser_dialog_new("Open File", widget, action, "_Cancel", GTK_RESPONSE_CANCEL, "_Open", GTK_RESPONSE_ACCEPT, NULL);
	res = gtk_dialog_run(GTK_DIALOG(dialog));
	if (res == GTK_RESPONSE_ACCEPT) {
		char *filename;
		GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
		filename = gtk_file_chooser_get_filename(chooser);
		resetMemory();
		initCPU();
		FILE *rom = fopen(filename, "rb");
		readFile(0, rom);
		fclose(rom);
		g_free(filename);
	}
	gtk_widget_destroy(dialog);
	return FALSE;
}

gboolean reloadPressed(GtkWidget * widget, gpointer item) {
	// TODO reset memory
	// Reload ROM
	initCPU();
	return FALSE;
}

void redrawTiles(){
    uint8_t imageMap[TILES_WIDE*TILES_TALL*8*8*4];
    for(int i = 0; i < TILES_WIDE * TILES_TALL; i++){
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
                uint16_t x1 = (i%TILES_WIDE)*8+x;
                uint16_t y1 = (i/ TILES_WIDE)*8+y;
                imageMap[(y1*TILES_WIDE *8+x1)*4] = (colour & 0xFF0000) >> 16;
                imageMap[(y1*TILES_WIDE *8+x1)*4+1] = (colour & 0xFF00) >> 8;
                imageMap[(y1*TILES_WIDE *8+x1)*4+2] = colour & 0xFF;
                imageMap[(y1*TILES_WIDE *8+x1)*4+3] = 0xFF;
            }
        }
    }
    cairo_t *cr = cairo_create(tileSurface);
    cairo_scale(cr, 2, 2);
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_data((guchar*)imageMap, GDK_COLORSPACE_RGB, TRUE, 8, TILES_WIDE *8, TILES_TALL *8, TILES_WIDE *8*4, NULL, NULL);
    gdk_cairo_set_source_pixbuf(cr, pixbuf, 0, 0);
    cairo_paint(cr);
    cairo_destroy(cr);
    gtk_widget_queue_draw(tileArea);
}

void drawScreen(){
    uint8_t imageMap[160*144*4];
    for(int i = 0 ; i < 160*144*4; i++){
        imageMap[i] = 0x00;
    }
    
    for(int x = 0; x < 20; x++){
        for(int y = 0; y < 18; y++){
            uint8_t tile = VRAM[0x9800+y*20+x];
            for(int xx = 0; xx < 8; xx++){
                for(int yy = 0; yy < 8; yy++){
                    int offset = (y*8+yy)*(160*4)+(x*8+xx)*4;
                    uint8_t pallete = tiles[tile][yy][xx];
                    switch(pallete){
                        case 0:
                            imageMap[offset+0] = 0xFF;
                            imageMap[offset+1] = 0xFF;
                            imageMap[offset+2] = 0xFF;
                            imageMap[offset+3] = 0xFF;
                            break;
                        case 1:
                            imageMap[offset+0] = 0xFF;
                            imageMap[offset+1] = 0x00;
                            imageMap[offset+2] = 0x00;
                            imageMap[offset+3] = 0xFF;
                            break;
                        case 2:
                            imageMap[offset+0] = 0x00;
                            imageMap[offset+1] = 0xFF;
                            imageMap[offset+2] = 0x00;
                            imageMap[offset+3] = 0xFF;
                            break;
                        case 3:
                            imageMap[offset+0] = 0x00;
                            imageMap[offset+1] = 0x00;
                            imageMap[offset+2] = 0xFF;
                            imageMap[offset+3] = 0xFF;
                            break;
                    }
                }
            }
        }
    }


    cairo_t *cr = cairo_create(surface);
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_data((guchar*)imageMap, GDK_COLORSPACE_RGB, TRUE, 8, 160, 144, 160*4, NULL, NULL);
    gdk_cairo_set_source_pixbuf(cr, pixbuf, 0, 0);
    cairo_paint(cr);
    cairo_destroy(cr);
	gtk_widget_queue_draw(drawingArea);
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
	gint64 currentTime = 0;
    if (cpu.running){
        gint64 timeDelta = currentTime - lastTime;
        
		uint32_t numCycles = 100 + cpu.ticks;
        while(cpu.ticks < numCycles && cpu.running){
            executeOpcode();
            gpuTick();
        }
        updateLabels();
        if (gpu.draw){
            redrawTiles();
            gpu.draw = 0;
        }
    }
    gpuTick();
    if (gpu.draw){
        redrawTiles();
        drawScreen();
        gpu.draw = 0;
    }
    lastTime = currentTime;
    return TRUE;
}


gboolean stepCallback(GtkWidget *widget, GdkEventConfigure *event, gpointer data){executeOpcode();
        updateLabels();
        gpuTick();
        if (gpu.draw){
            redrawTiles();
            drawScreen();
            gpu.draw = 0;
        }
}