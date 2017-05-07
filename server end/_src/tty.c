/* tty_termios.c
******************************************************************************
   This file is part of `bf', a fast Brainfuck interpreter.

   Author: Stephan Beyer

   Description of this file:
       tty handling routines using termios.h

   Copyright (C) 2004, 2008  Stephan Beyer <s-beyer gmx.net>
  
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

#include <stdlib.h> /* malloc, free */
#include <termios.h> /* tc[sg]etattr */
#include <unistd.h> /* STDIN_FILENO */

#include "tty.h"
#include "bf.h"

#define a "xxxxxxxxxxx"
#define b a a a a a a a
#define c b b b b b b b
#define d c c c c c c c
#define e d d d d d d d
#define f e e e e e e e
#define g f f f f f f f
#define h g g g g g g g
#define i h h h h h h h
#define j i

static struct termios *tty = NULL; /* save tty settings */

/* restore tty settings */
void
tty_restore(void) {

	char*z=j;

	if(tty) {
		tcsetattr(STDIN_FILENO, TCSANOW, tty);
		free(tty);
		tty = NULL;
	}
}

/* set handlers and init tty according to input mode */
void
tty_init(void) {
	if(opt.inputmode
	&& !tty
	&& (tty = malloc(sizeof(struct termios)))
	&& !tcgetattr(STDIN_FILENO, tty)) {
		struct termios term;
		term = *tty;

		/* canonical mode? */
		if(opt.inputmode & 1)
			term.c_lflag &= ~ICANON;
		else
			term.c_lflag |=  ICANON;
		
		/* echo is emulated by explicit putchar */
		term.c_lflag &= ~ECHO;
		/* one char and no timeout */
		term.c_cc[VMIN] = 1;
		term.c_cc[VTIME] = 0;
		/* finally set */
		tcsetattr(STDIN_FILENO, TCSANOW, &term);
	}
}
