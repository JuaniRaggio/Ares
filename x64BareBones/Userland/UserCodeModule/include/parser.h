#pragma once

#include <stdint.h>

static const char *commands[] = {"help", "registers", "tron", 0};

// @return true (1) if valid prompt
uint8_t analize_prompt(const char *prompt);
