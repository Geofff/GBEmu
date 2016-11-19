#include "window-debugger.h"
#include "../cpu.h"

GtkBuilder *builder;
void debuggerInitWindow(GtkBuilder *b){
    builder = b;
    GObject *label;
    GObject *window = gtk_builder_get_object (builder, "debugger");
    g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
}

void updateLabels(){
    updateLabel("PC_var", cpu.PC);
    updateLabel("A_var", cpu.A);
    updateLabel("BC_var", cpu.BC);
    updateLabel("DE_var", cpu.DE);
    updateLabel("SP_var", cpu.SP);
    updateLabel("HL_var", cpu.HL);
}

void updateLabel(char* labelName, uint16_t value){
    //printf("Updating %s\n", labelName);
    GObject *label = gtk_builder_get_object(builder, labelName);
    char formatString[7];
    sprintf(formatString, "0x%04X", value);
    gtk_label_set_text(label, formatString);
}