/* bf.h */
#ifndef BF_H
#define BF_H

#define VERSION "20080330"
#define CELLS	0x2000	/* 8k */

/* options */
typedef struct {
	const char *filename;
	unsigned long cells;		/* -c */
	unsigned inputmode  : 3;	/* -, */
	unsigned wraparound : 1;	/* -w */
	unsigned null       : 1;	/* -n */
	unsigned flush      : 1;	/* -f */
} Opt;
extern Opt opt; /* bf options */

#endif /* BF_H */
