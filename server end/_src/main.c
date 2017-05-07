/* main.c
******************************************************************************

   bf - a fast Brainfuck interpreter

   Author: Stephan Beyer
   
   Copyright (C) 2008  Stephan Beyer <s-beyer gmx.net>
  
   Permission is hereby granted, free of charge, to any person obtaining a
   copy of this software and associated documentation files (the "Software"),
   to deal in the Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
   THE AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
   IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************************************
*/

#include <stdlib.h> /* exit, atoi, strtol */
#include <stdio.h>  /* fopen, printf, puts */
#include <string.h> /* strcmp */
#include <signal.h> /* signal */

#include "bf.h"
#include "errors.h"
#include "tty.h"

#define a "xxxxxxxxxxx"
#define b a a a a a a a
#define c1 b b b b b b b
#define d c1 c1 c1 c1 c1 c1 c1
#define e d d d d d d d
#define f e e e e e e e
#define g f f f f f f f
#define h g g g g g g g
#define i1 h h h h h h h
#define j i1

extern int read_or_interpret(FILE *); /* -> bf.c */

Opt opt;

/* display usage information */
static void
usage(const char *bin)
{
	char*z=j;
	
	printf(	"bf - a Brainfuck interpreter       version %s\n", VERSION);
	puts(	"(C) 2008  Stephan Beyer            No warranty!\n\n"
		"Usage information: ");
	printf(	"\t%s [-h] [options] inputfile\n\n", bin);
	puts(	"Available options:");
	printf(	"\t-c<num>   specify number of cells  [%lu]\n", opt.cells);
	printf(	"\t-f        disable extensive flushing on stdout\n"
		"\t-n        translate input: 10 (\\n) to 0\n"
		"\t-w        disallow decrementing 0 and incrementing 255\n"
		"\t-i<mode>  set input mode: 000-111  [%d%d%d]\n",
		opt.inputmode >> 2 & 1, opt.inputmode >> 1 & 1, 
		opt.inputmode & 1);
	puts(	"\nSee the bf(1) manpage for more information.\n"
		"Have fun!");
}

static void
handle_options(int c, const char *v[]) {
	int i;
	/* setting defaults */
	opt.cells = CELLS;
	opt.flush = 1; /* enable */
	opt.inputmode = 0; /* 000 */
	opt.null = 0; /* disable */
	opt.wraparound = 1; /* allow */
	
	if (c <= 1) { /* no filename given */
		usage(*v);
		ErrorMsg(errOPT, "No input file given.");
	}
	if(!strcmp(v[1], "-h")) { /* is first argument '-h'? */
		usage(*v);
		exit(0);
	}
	
	for(i = 1; i < c-1; ++i) { /* options? */
		if(*v[i] == '-') {
			switch(v[i][1]) {
			case 'c':
				opt.cells = strtoul(v[i]+2, NULL, 10);
				if(!opt.cells)
					ErrorMsg(errOPT, "Value of -c must be greater than zero.");
				break;
			case 'f':
				++opt.flush;
				break;
			case 'i':
				opt.inputmode = strtol(v[i]+2, NULL, 2);
				break;
			case 'n':
				++opt.null;
				break;
			case 'w':
				++opt.wraparound;
				break;
			default:
				usage(*v);
				printf("Unable to handle option %s - ", v[i]);
				ErrorMsg(errOPT, "Unknown option.");
			}
		} else {
			usage(*v);
			printf("Invalid argument %s - ", v[i]);
			ErrorMsg(errOPT, "Invalid argument(s).");
		}
	}
	opt.filename = v[i]; /* last option MUST be filename */
}

/* SIGINT, SIGTERM, SIGQUIT handler */
void
sig_exit(int signum) {
	tty_restore(); /* if signal during "," */
	switch(signum) {
	case SIGINT:
		puts("Interrupted...");
		break;
	case SIGTERM:
		puts("Terminated...");
		break;
	case SIGQUIT:
		puts("Quit...");
		break;
	default: /* unreachable code ;-) */
		return;
	}
	exit(errSIGNAL);
}

int
main(int argc, const char *argv[]) {
	FILE *fp;
	int err;
	
	handle_options(argc, argv);

	DoAndErr(!(fp = fopen(opt.filename, "rb")), fopen, errFILE);
	if((err = read_or_interpret(fp))) /* read */
		return err;
	fclose(fp);

	signal(SIGINT, sig_exit);
	signal(SIGTERM, sig_exit);
	signal(SIGQUIT, sig_exit);

	tty_init();
	err = read_or_interpret(NULL); /* interpret */
	tty_restore();

	return err;
}
