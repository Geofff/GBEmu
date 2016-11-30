#include "window-disassembly.h"
#include "../cpu.h"

GtkBuilder *builder;
GtkListStore *disList;
void disassemblerInitWindow(GtkBuilder *b){
    builder = b;
    GObject *window = gtk_builder_get_object (builder, "disassembler");

	GtkWidget *view = GTK_WIDGET(gtk_builder_get_object(builder, "dis_tree"));
	GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view), -1, "Offset", renderer, "text", 0, NULL);
	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view), -1, "Value", renderer, "text", 1, NULL);
	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view), -1, "Command", renderer, "text", 2, NULL);
	GtkTreeIter iter;
	disList = gtk_list_store_new(3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
	gtk_list_store_append(disList, &iter);
	gtk_list_store_set(disList, &iter, 0, "0x0", 1, "First",2, "LD 5", -1);
	gtk_list_store_append(disList, &iter);
	gtk_list_store_set(disList, &iter, 0, "0x1", 1, "Second", 2, "LD 5",-1);
	gtk_tree_view_set_model(GTK_TREE_VIEW(view), GTK_TREE_MODEL(disList));
	
}


void disassemblerShowWindow() {
	// First we need to recalculate rows based on opcode
	gtk_list_store_clear(disList);
	GtkTreeIter iter;
	uint16_t count = 0;
	uint16_t offset = 0;
	while (offset < 0x100) {
		uint8_t opcode = readByte(offset);
		if (opcode_implemented[opcode] == 0) {
			offset++;
			continue;
		}
		char offsetString[7];
		char opcodeString[5];
		snprintf(offsetString, 7, "0x%04X", offset);
		snprintf(opcodeString, 5, "0x%02X", opcode);
		uint8_t d8 = readByte(offset + 1);
		uint16_t d16 = (readByte(offset + 2) << 8) + readByte(offset + 1);

		// What a shitty hack
		char buffer[512];
		char secondBuffer[512];
		strcpy(buffer, opcode_names[opcode]);
		uint16_t toReplace = 0;
		// Replace 8 bit
		for (int i = 0; i < strlen(buffer) - 1; i++) {
			if (buffer[i + 1] == '8' && (buffer[i] == 'r' || buffer[i] == 'd' || buffer[i] == 'a')) {
				strncpy(secondBuffer, buffer, i);
				secondBuffer[i] = '\0';
				strcat(secondBuffer + i, "0x%02X");
				if (buffer[i + 2] != '\0') {
					strcat(secondBuffer + i + 4, buffer + i + 2);
				}
				toReplace = d8;
			}
		}

		// Replace 16 bit
		for (int i = 0; i < strlen(buffer) - 2; i++) {
			if (buffer[i + 1] == '1' && buffer[i + 2] == '6' && (buffer[i] == 'r' || buffer[i] == 'd' || buffer[i] == 'a')) {
				strncpy(secondBuffer, buffer, i);
				secondBuffer[i] = '\0';
				strcat(secondBuffer + i, "0x%04X");
				if (buffer[i + 2] != '\0') {
					strcat(secondBuffer + i + 4, buffer + i + 3);
				}
				toReplace = d16;
			}
		}

		// Naughty overflow
		char formattedString[512];
		if (toReplace != NULL) {
			sprintf(formattedString, secondBuffer, toReplace);
		} else {
			sprintf(formattedString, "%s", opcode_names[opcode]);
		}

		gtk_list_store_append(disList, &iter);
		gtk_list_store_set(disList, &iter, 0, offsetString, 1, opcodeString, 2, formattedString, -1);

		offset += opcode_length[opcode];
		if (opcode_length[opcode] == 0) {
			offset++;
		}
		count++;
	}
	GObject *window = gtk_builder_get_object(builder, "disassembler");
	gtk_widget_show(window);
}