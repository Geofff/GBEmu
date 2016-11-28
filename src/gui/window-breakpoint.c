#include "window-breakpoint.h"
#include "../cpu.h"

GtkBuilder *builder;
GtkListStore *breakStore;
uint8_t index;
void breakpointInitWindow(GtkBuilder *b) {
	builder = b;
	GObject *window = gtk_builder_get_object(builder, "breakpoint");
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	gtk_widget_show(window);
	GObject *button = gtk_builder_get_object(builder, "add_command");
	g_signal_connect(button, "clicked", G_CALLBACK(addCommand), NULL);
	button = gtk_builder_get_object(builder, "add_location");
	g_signal_connect(button, "clicked", G_CALLBACK(addLocation), NULL);
	GObject *entry = gtk_builder_get_object(builder, "command");
	g_signal_connect(entry, "activate", G_CALLBACK(addCommand), NULL);
	entry = gtk_builder_get_object(builder, "location");
	g_signal_connect(entry, "activate", G_CALLBACK(addLocation), NULL);

	GtkWidget *view = GTK_WIDGET(gtk_builder_get_object(builder, "breakpoints"));
	GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view), -1, "Index", renderer, "text", 0, NULL);
	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view), -1, "Type", renderer, "text", 1, NULL);
	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view), -1, "Value", renderer, "text", 2, NULL);
	GtkTreeIter iter;
	breakStore = gtk_list_store_new(3, G_TYPE_UINT, G_TYPE_STRING, G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(view), GTK_TREE_MODEL(breakStore));
	index = 0;
}

void addCommand() {
	GObject *textbox = gtk_builder_get_object(builder, "command");
	gchar *text = gtk_entry_get_text(textbox);
	uint16_t value = processEntry(text);
	char formattedString[7];
	snprintf(formattedString, 7, "0x%04X", value);
	if (numOpBreakpoints == 15) {
		return;
	}
	opBreakpoints[numOpBreakpoints++] = value;
	GtkTreeIter iter;
	gtk_list_store_append(breakStore, &iter);
	gtk_list_store_set(breakStore, &iter, 0, index++, 1, "Command", 2, formattedString, -1);
	gtk_entry_set_text(textbox, "");
}

void addLocation() {
	GObject *textbox = gtk_builder_get_object(builder, "location");
	gchar *text = gtk_entry_get_text(textbox);
	uint16_t value = processEntry(text);
	char formattedString[7];
	snprintf(formattedString, 7, "0x%04X", value);
	if (numLocBreakpoints == 15) {
		return;
	}
	locBreakpoints[numLocBreakpoints++] = value;
	GtkTreeIter iter;
	gtk_list_store_append(breakStore, &iter);
	gtk_list_store_set(breakStore, &iter, 0, index++, 1, "Location", 2, formattedString, -1);
	gtk_entry_set_text(textbox, "");
}

int processEntry(char* entry) {
	printf("Checking entry %s...\n", entry);
	int value = 0;
	if (sscanf(entry, "%X", &value) == 1) return value;
	if (sscanf(entry, "%x", &value) == 1) return value;
	return value;
}
