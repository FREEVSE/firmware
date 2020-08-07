#include <CommandHandler.h>

//Set commands
void Cmd_Set(CommandParameter &params);

void SetMaxAmps(CommandParameter &params);
void SetSSID(CommandParameter &params);
void SetPass(CommandParameter &params);

//GFCI ommands

void Cmd_GFCI(CommandParameter &params);

//General
void Cmd_ReadSettings(CommandParameter &params);

void Cmd_WiFiConnect(CommandParameter &params);

constexpr unsigned int hash(const char *s, int off);