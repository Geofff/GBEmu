#include "window-debugger.h"
#include "../cpu.h"

GtkBuilder *builder;
GtkListStore *stackStore;
void debuggerInitWindow(GtkBuilder *b){
    builder = b;
    GObject *label;
    printf("Finding debugger window\n");
    GObject *window = gtk_builder_get_object (builder, "debugger");
    printf("Found debugger window\n");

	GtkWidget *view = GTK_WIDGET(gtk_builder_get_object(builder, "stack"));
	GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view), -1, "Index", renderer, "text", 0, NULL);
	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view), -1, "Value", renderer, "text", 1, NULL);
	GtkTreeIter iter;
	stackStore = gtk_list_store_new(2, G_TYPE_UINT, G_TYPE_STRING);
	gtk_list_store_append(stackStore, &iter);
	gtk_list_store_set(stackStore, &iter, 0, 0, 1, "First", -1);
	gtk_list_store_append(stackStore, &iter);
	gtk_list_store_set(stackStore, &iter, 0,	1, 1, "Second", -1);
	gtk_tree_view_set_model(GTK_TREE_VIEW(view), GTK_TREE_MODEL(stackStore));
	
}

void updateLabels(){
    updateLabel("PC_var", cpu.PC);
    updateLabel("A_var", cpu.A);
    updateLabel("BC_var", cpu.BC);
    updateLabel("DE_var", cpu.DE);
    updateLabel("SP_var", cpu.SP);
    updateLabel("HL_var", cpu.HL);
    updateLabel("OPCODE_var", opcode);
    updateStack();
}

void updateLabel(char* labelName, uint16_t value){
    //printf("Updating %s\n", labelName);
    GObject *label = gtk_builder_get_object(builder, labelName);
    char formatString[7];
    sprintf(formatString, "0x%04X", value);
    gtk_label_set_text(label, formatString);
}

void updateStack(){
	gtk_list_store_clear(stackStore);
	GtkTreeIter iter;
	uint8_t index = 0;
	char formatString[5];
	return;
	for (uint16_t sp = 0xFFFE; sp > cpu.SP; sp--) {
		uint8_t value = readByte(sp);
		gtk_list_store_append(stackStore, &iter);
		sprintf(formatString, "0x%02X", value);
		gtk_list_store_set(stackStore, &iter, 0, index++, 1, formatString, -1);
	}
    printf("updating store\n");
}

void debugShowWindow() {
	GObject *window = gtk_builder_get_object(builder, "debugger");
	gtk_widget_show(window);
}