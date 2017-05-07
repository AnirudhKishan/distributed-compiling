/* errors.h
 * (C) Stephan Beyer, 2003, 2004, 2008, GPL */
#ifndef ERRORS_H
#define ERRORS_H

#define ERROCCMSG	"an error occured"
#define DoAndErr(COND,STR,ERR)	if (COND) HandleError(#STR, __FILE__, __LINE__, ERR)

enum errorcodes
{
	errNO = 0,
	errMEMORY,
	errOPT,
	errFILE,
	errBRACKET,
	errOUTOFRANGE,
	errWRAPAROUND,
	errSIGNAL,
	errUNDEFINED
};

void ErrorMsg(signed int, const char *);
void HandleError(const char *, char *, unsigned int, signed int);

#endif
