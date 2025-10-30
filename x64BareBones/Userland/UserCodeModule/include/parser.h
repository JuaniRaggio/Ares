#ifndef _PARSER_H_
#define _PARSER_H_

#pragma once

#include <stdint.h>

static uint8_t *commands[] = {"help", "registers", "tron", 0};

// @return true (1) if valid prompt
uint8_t analize_prompt(uint8_t *prompt);

#endif