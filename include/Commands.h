#pragma once

#include <CommandHandler.h>

//Set commands
void Cmd_Set(CommandParameter &params);

void SetMaxAmps(CommandParameter &params);
void SetSSID(CommandParameter &params);
void SetPass(CommandParameter &params);

//GFCI ommands

void Cmd_GFCI(CommandParameter &params);

//AC Detector commands

void Cmd_AC(CommandParameter &params);

//General
void Cmd_ReadSettings(CommandParameter &params);

constexpr unsigned int hash(const char *s, int off);