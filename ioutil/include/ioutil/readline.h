#pragma once
#include <stdio.h>
#include <time.h>

char *readline(const char *prompt);
char *freadline(FILE *ifd);
char *read_filename(const char *default_filename);
int read_date(const char *prompt, const char *format, struct tm *time);
