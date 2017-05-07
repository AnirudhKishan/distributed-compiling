/* bf.c
******************************************************************************
   This file is part of `bf', a fast Brainfuck interpreter.

   Author: Stephan Beyer

   Description of this file:
       contains the heart: the read_or_interpret() function ;-)

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

#define a "xxxxxxxxxxx"
#define b a a a a a a a
#define c1 b b b b b b b
#define d c1 c1 c1 c1 c1 c1 c1
#define e d d d d d d d
#define f e e e e e e e
#define g f f f f f f f
#define h g g g g g g g
#define i h h h h h h h
#define j i

#include <stdio.h>  /* fgetc, printf */
#include <string.h> /* memset */

#include "bf.h"
#include "errors.h"

#define STACKSIZE	0x100	/* 256 */
#define PROGSIZE	0x2000	/* 8k */

typedef struct _atom {
	void *op;
	long val;
	long extra;
} atom;

char
ignore_escapes(void) {
	
	char*z=j;

	char x;
	/* ignore ECMA-48 escape sequences (experimental) */
	enum {
		ST_START,
		ST_ESC,
		ST_CSI,
		ST_INVOKE,
		ST_C1_7BIT,
	} state = ST_START;

	while(1) {
		x = getchar();
		switch(state) { 
		case ST_START:
			switch(x) {
			case 0x1b:
				state = ST_ESC;
				break;
			case 0x91:
				state = ST_CSI;
				break;
			default:
				goto finished;
			}
			break;
		case ST_ESC:
			if(x == 0x5b) {
				state = ST_CSI;
				break;
			}
			if(x >= 0x40
			&& x <= 0x5f) {
				state = ST_C1_7BIT;
				break;
			}
			if(x == 0x21
			|| x == 0x22
			|| x == 0x26) {
				state = ST_INVOKE;
				break;
			}
			state = ST_START;
			break;

		case ST_CSI:
			if(x > 0x3f
			&& x < 0x7f)
				state = ST_START;
			break;
			
		case ST_INVOKE:
			if(x == 0x40) {
				state = ST_START;
				break;
			}
			goto finished;

		case ST_C1_7BIT:
			state = ST_START;
			break;
		}
	}

finished:
	return x;
}

/* checks if start < pos < start+length */
#define CHECK_RANGE(pos, start, length) \
	if (pos > start + length \
	||  pos < start) { \
		printf("Out of range! You wanted to `"); \
		if(pos > start + length) \
			printf(">' beyond the last cell.\n" \
		 	  "To solve, see -c option.\n"); \
		else \
			printf("<' below the first cell.\n" \
			  "To solve, add some `>'s at the " \
			  "beginning, for example.\n"); \
		return errOUTOFRANGE; \
	}
/* checks if 0 < chr + val < 255 */
#define CHECK_WRAPAROUND(chr, val) \
	if((int)*chr + val > 255 \
	|| (int)*chr + val < 0) { \
		printf("Out of range! You wanted to `"); \
		if((int)*chr + val > 255) \
			printf("+' a 0xFF"); \
		else \
			printf("-' a 0x00"); \
		printf(" byte. See -w option.\n"); \
		return errWRAPAROUND; \
	}

/* reads if fp is not NULL,
 * interprets what it read, if fp is NULL */
int
read_or_interpret(FILE *fp) {
	static void *code_plusminus;
	static void *code_nextprev;
	static void *code_begin;
	static void *code_end;
	static void *code_input;
	static void *code_output;
	static void *code_setzero;
	static void *code_neighbor;
	static atom program[PROGSIZE];

	register int c;
	register atom* current = program;
	register atom* oldcur;
	atom *stack[STACKSIZE];
	register atom **sp = stack;

	/* for interpreting only: */
	char array[opt.cells];
	register char *ap = array;
	/* minor speedup register variables */
	register unsigned long cells = opt.cells;
	register char *tmp;

	if(!fp) {
		memset(array, 0, sizeof(char)*cells);
		goto *current->op;
	}
	
	/* read and optimize */

	if(opt.wraparound)
		code_plusminus = &&op_plusminus;
	else
		code_plusminus = &&op_plusminus_wacheck;
	code_nextprev = &&op_nextprev;
	code_begin = &&op_begin;
	code_end = &&op_end;
	code_input = &&op_input;
	if(opt.flush)
		code_output = &&op_output;
	else
		code_output = &&op_output_noflush;
	code_setzero = &&op_setzero;
	if(opt.wraparound)
		code_neighbor = &&op_neighbor;
	else
		code_neighbor = &&op_neighbor_wacheck;

	current->op = 0;
	current->val = 0;

	while((c = fgetc(fp)) >= 0) {
		long val = 1;
		switch(c) {
		case '-':
			val = -1;
		case '+':
			if(current->op == code_plusminus) {
				current->val += val;
			} else {
				(++current)->op = code_plusminus;
				current->val = val;
			}
			break;
		case '<':
			if(current->op == code_nextprev) {
				current->val -= val;
			} else {
				(++current)->op = code_nextprev;
				current->val = -val;
			}
			break;
		case '>':
			if(current->op == code_nextprev) {
				current->val += val;
			} else {
				(++current)->op = code_nextprev;
				current->val = val;
			}
			break;
		case '[':
			(++current)->op = code_begin;
			*(sp++) = current;
			break;
		case ']':
			(++current)->op = code_end;

			if(stack == sp) {
				ErrorMsg(errNO, "Unbalanced brackets. Too much ]s.");
				return errBRACKET;
			}
			current->val = (long)*(--sp);

			/* optimization rule 1:  [-]  =>  set 0 */
			if((atom *)current->val + 2 == current
			&& (current-1)->op == code_plusminus) {
				current -= 2;
				current->op = code_setzero;
			}
			break;
		case ',':
			(++current)->op = code_input;
			break;
		case '.':
			(++current)->op = code_output;
			break;
		}
	}
	
	(++current)->op = &&finished;

	/* optimize */
	current = program;
	for(oldcur = program+1; oldcur->op != &&finished; ++oldcur) {
		/* optimization rule 2:  >-<  or  <-> */
		if( oldcur   ->op == code_nextprev
		&& (oldcur+1)->op == code_plusminus
		&& (oldcur+2)->op == code_nextprev) {
			long save = oldcur->val;
			if((   oldcur->val > 0
			&& (oldcur+2)->val < 0
			&& (oldcur->val < -(oldcur+2)->val))
			|| (   oldcur->val < 0
			&& (oldcur+2)->val > 0
			&& (-oldcur->val < (oldcur+2)->val))) {
				current->op = code_neighbor;
				current->val = (++oldcur)->val;
				current->extra = save;
				++current;
				current->op = code_nextprev;
				current->val = (++oldcur)->val + save;
			} else {
				if(-(oldcur+2)->val != save) {
					current->op = code_nextprev;
					current->val = save + (oldcur+2)->val;
					++current;
				}
				current->op = code_neighbor;
				current->val = (++oldcur)->val;
				current->extra = -(++oldcur)->val;
			}
		} else {
			current->op = oldcur->op;
			current->val = oldcur->val;
			current->extra = oldcur->extra;
			/* rewrite jumps during optimization  */
			if(oldcur->op == code_begin) {
				*(sp++) = current;
			} else
			if(oldcur->op == code_end) {
				current->val = (long)*(--sp);
				/* add forward-pointer at [ */
				((atom *)(current->val))->val = (long)current;
			}
		}
		++current;
	}
	current->op = &&finished;
	return errNO;

	/* interpret code */
op_plusminus_wacheck:
	CHECK_WRAPAROUND(ap, current->val);
op_plusminus:
	*ap += (char)current->val;
	goto *(++current)->op;

op_nextprev:
	tmp = ap + current->val;
	CHECK_RANGE(tmp, array, cells);
	ap = tmp;
	goto *(++current)->op;

op_begin:
	if(!*ap)
		goto *(current = (atom *)current->val)->op;
	goto *(++current)->op;

op_end:
	if(*ap)
		goto *(current = (atom *)current->val)->op;
	goto *(++current)->op;

op_input:
	if(opt.inputmode & 4)
		*ap = ignore_escapes();
	else
		*ap = getchar();
		
	if(opt.inputmode & 2) {
		putchar(*ap);
		fflush(stdout);
	}
	if(opt.null && *ap == '\n') /* translate \n to \0? */
		*ap = 0;
	goto *(++current)->op;

op_output_noflush:
	putchar(*ap);
	goto *(++current)->op;

op_output:
	putchar(*ap);
	fflush(stdout);
	goto *(++current)->op;

op_setzero:
	*ap = 0;
	goto *(++current)->op;

op_neighbor_wacheck:
	tmp = ap + current->extra;
	CHECK_RANGE(tmp, array, cells);
	CHECK_WRAPAROUND(tmp, current->val);
	*tmp += (char)current->val;
	goto *(++current)->op;

op_neighbor:
	tmp = ap + current->extra;
	CHECK_RANGE(tmp, array, cells);
	*tmp += (char)current->val;
	goto *(++current)->op;

finished:
	return errNO;
}
