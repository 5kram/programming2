

#ifndef _OBJDB_H
#define _OBJDB_H
#include <stdio.h>

int main (int argc, char *argv[]);
int open(FILE *fp, char dbname[]);
int metadata(FILE **fp);

#endif