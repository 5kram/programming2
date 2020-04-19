

#ifndef _OBJDB_H
#define _OBJDB_H
#include <stdio.h>

int main (int argc, char *argv[]);
int open(FILE **fp, char dbname[]);
int metadata(FILE **fp);
int find_name (FILE **fp, char objname[]);
int move_block (FILE **fp, FILE **op, char objname[]);
int import (FILE **fp, char fname[], char objname[]);
int close(FILE **fp);

#endif