#ifndef TERMINAL_H
#define TERMINAL_H

#include "sys/defs.h"

void append_buffer(const char *s, int size);

void flush_buffer();

int read_stdin(char *buf, int size);

int write_stdout(char *buf, int size);

int write_stderr(char *buf, int size);

#endif
