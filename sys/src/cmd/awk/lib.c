/****************************************************************
Copyright (C) Lucent Technologies 1997
All Rights Reserved

Permission to use, copy, modify, and distribute this software and
its documentation for any purpose and without fee is hereby
granted, provided that the above copyright notice appear in all
copies and that both that the copyright notice and this
permission notice and warranty disclaimer appear in supporting
documentation, and that the name Lucent Technologies or any of
its entities not be used in advertising or publicity pertaining
to distribution of the software without specific, written prior
permission.

LUCENT DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
IN NO EVENT SHALL LUCENT OR ANY OF ITS ENTITIES BE LIABLE FOR ANY
SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
THIS SOFTWARE.
****************************************************************/

#include <u.h>
#include <libc.h>
#include <ctype.h>
#include <bio.h>
#include "awk.h"
#include "y.tab.h"

Biobuf	*infile;
char	*file	= "";
char	*record;
int	recsize	= RECSIZE;
char	*fields;
int	fieldssize = RECSIZE;

Cell	**fldtab;	/* pointers to Cells */
char	inputFS[100] = " ";

#define	MAXFLD	200
int	nfields	= MAXFLD;	/* last allocated slot for $i */

int	donefld;	/* 1 = implies rec broken into fields */
int	donerec;	/* 1 = record is valid (no flds have changed) */

int	lastfld	= 0;	/* last used field */
int	argno	= 1;	/* current input argument number */
extern	Awkfloat *AARGC;

static Cell dollar0 = { OCELL, CFLD, nil, "", 0.0, REC|STR|DONTFREE };
static Cell dollar1 = { OCELL, CFLD, nil, "", 0.0, FLD|STR|DONTFREE };

void recinit(unsigned int n)
{
	assert(n > 0);
	record = (char *) malloc(n);
	fields = (char *) malloc(n);
	fldtab = (Cell **) malloc((nfields+1) * sizeof(Cell *));
	if (record == nil || fields == nil || fldtab == nil)
		FATAL("out of space for $0 and fields");
	record[0] = '\0';
	fldtab[0] = (Cell *) malloc(sizeof (Cell));
	*fldtab[0] = dollar0;
	fldtab[0]->sval = record;
	fldtab[0]->nval = tostring("0");
	makefields(1, nfields);
}

void makefields(int n1, int n2)		/* create $n1..$n2 inclusive */
{
	char temp[50];
	int i;

	for (i = n1; i <= n2; i++) {
		fldtab[i] = (Cell *) malloc(sizeof (struct Cell));
		if (fldtab[i] == nil)
			FATAL("out of space in makefields %d", i);
		*fldtab[i] = dollar1;
		sprint(temp, "%d", i);
		fldtab[i]->nval = tostring(temp);
	}
}

void initgetrec(void)
{
	int i;
	char *p;

	for (i = 1; i < *AARGC; i++) {
		if (!isclvar(p = getargv(i))) {	/* find 1st real filename */
			setsval(lookup("FILENAME", symtab), p);
			return;
		}
		setclvar(p);	/* a commandline assignment before filename */
		argno++;
	}
	infile = &stdin;		/* no filenames, so use &stdin */
}

int getrec(char **pbuf, int *pbufsize, int isrecord)	/* get next input record */
{			/* note: cares whether buf == record */
	int c;
	static int firsttime = 1;
	char *buf = *pbuf;
	int bufsize = *pbufsize;

	if (firsttime) {
		firsttime = 0;
		initgetrec();
	}
 	dprint( ("RS=<%s>, FS=<%s>, AARGC=%g, FILENAME=%s\n",
		*RS, *FS, *AARGC, *FILENAME) );
	if (isrecord) {
		donefld = 0;
		donerec = 1;
	}
	buf[0] = 0;
	while (argno < *AARGC || infile == &stdin) {
		   dprint( ("argno=%d, file=|%s|\n", argno, file) );
		if (infile == nil) {	/* have to open a new file */
			file = getargv(argno);
			if (*file == '\0') {	/* it's been zapped */
				argno++;
				continue;
			}
			if (isclvar(file)) {	/* a var=value arg */
				setclvar(file);
				argno++;
				continue;
			}
			*FILENAME = file;
			   dprint( ("opening file %s\n", file) );
			if (*file == '-' && *(file+1) == '\0')
				infile = &stdin;
			else if ((infile = Bopen(file, OREAD)) == nil)
				FATAL("can't open file %s", file);
			setfval(fnrloc, 0.0);
		}
		c = readrec(&buf, &bufsize, infile);
		if (c != 0 || buf[0] != '\0') {	/* normal record */
			if (isrecord) {
				if (freeable(fldtab[0]))
					xfree(fldtab[0]->sval);
				fldtab[0]->sval = buf;	/* buf == record */
				fldtab[0]->tval = REC | STR | DONTFREE;
				if (is_number(fldtab[0]->sval)) {
					fldtab[0]->fval = atof(fldtab[0]->sval);
					fldtab[0]->tval |= NUM;
				}
			}
			setfval(nrloc, nrloc->fval+1);
			setfval(fnrloc, fnrloc->fval+1);
			*pbuf = buf;
			*pbufsize = bufsize;
			return 1;
		}
		/* Beof arrived on this file; set up next */
		nextfile();
	}
	*pbuf = buf;
	*pbufsize = bufsize;
	return 0;	/* true end of file */
}

void nextfile(void)
{
	if (infile != nil && infile != &stdin)
		Bterm(infile);
	infile = nil;
	argno++;
}

int readrec(char **pbuf, int *pbufsize, Biobuf *inf)	/* read one record into buf */
{
	int sep, c;
	char *rr, *buf = *pbuf;
	int bufsize = *pbufsize;

	if (strlen(*FS) >= sizeof(inputFS))
		FATAL("field separator %.10s... is too long", *FS);
	strcpy(inputFS, *FS);	/* for subsequent field splitting */
	if ((sep = **RS) == 0) {
		sep = '\n';
		while ((c=Bgetc(inf)) == '\n' && c != Beof)	/* skip leading \n's */
			;
		if (c != Beof)
			Bungetc(inf);
	}
	for (rr = buf; ; ) {
		for (; (c=Bgetc(inf)) != sep && c != Beof; ) {
			if (rr-buf+1 > bufsize)
				if (!adjbuf(&buf, &bufsize, 1+rr-buf, recsize, &rr, "readrec 1"))
					FATAL("input record `%.30s...' too long", buf);
			*rr++ = c;
		}
		if (**RS == sep || c == Beof)
			break;
		if ((c = Bgetc(inf)) == '\n' || c == Beof) /* 2 in a row */
			break;
		if (!adjbuf(&buf, &bufsize, 2+rr-buf, recsize, &rr, "readrec 2"))
			FATAL("input record `%.30s...' too long", buf);
		*rr++ = '\n';
		*rr++ = c;
	}
	if (!adjbuf(&buf, &bufsize, 1+rr-buf, recsize, &rr, "readrec 3"))
		FATAL("input record `%.30s...' too long", buf);
	*rr = 0;
	   dprint( ("readrec saw <%s>, returns %d\n", buf, c == Beof && rr == buf ? 0 : 1) );
	*pbuf = buf;
	*pbufsize = bufsize;
	return c == Beof && rr == buf ? 0 : 1;
}

char *getargv(int n)	/* get ARGV[n] */
{
	Cell *x;
	char *s, temp[50];
	extern Array *ARGVtab;

	sprint(temp, "%d", n);
	x = setsymtab(temp, "", 0.0, STR, ARGVtab);
	s = getsval(x);
	dprint( ("getargv(%d) returns |%s|\n", n, s) );
	return s;
}

void setclvar(char *s)	/* set var=value from s */
{
	char *p;
	Cell *q;

	for (p=s; *p != '='; p++)
		;
	*p++ = 0;
	p = qstring(p, '\0');
	q = setsymtab(s, p, 0.0, STR, symtab);
	setsval(q, p);
	if (is_number(q->sval)) {
		q->fval = atof(q->sval);
		q->tval |= NUM;
	}
	   dprint( ("command line set %s to |%s|\n", s, p) );
}


void fldbld(void)	/* create fields from current record */
{
	/* this relies on having fields[] the same length as $0 */
	/* the fields are all stored in this one array with \0's */
	char *r, *fr, sep;
	Cell *p;
	int i, j, n, w;

	if (donefld)
		return;
	if (!isstr(fldtab[0]))
		getsval(fldtab[0]);
	r = fldtab[0]->sval;
	n = strlen(r);
	if (n > fieldssize) {
		xfree(fields);
		if ((fields = (char *) malloc(n+2)) == nil)  /* possibly 2 final \0s */
			FATAL("out of space for fields in fldbld %d", n);
		fieldssize = n;
	}
	fr = fields;
	i = 0;	/* number of fields accumulated here */
	if (strlen(inputFS) > 1) {	/* it's a regular expression */
		i = refldbld(r, inputFS);
	} else if (*inputFS == ' ') {	/* default whitespace */
		for (i = 0; ; ) {
			while (*r == ' ' || *r == '\t' || *r == '\n')
				r++;
			if (*r == 0)
				break;
			i++;
			if (i > nfields)
				growfldtab(i);
			if (freeable(fldtab[i]))
				xfree(fldtab[i]->sval);
			fldtab[i]->sval = fr;
			fldtab[i]->tval = FLD | STR | DONTFREE;
			do
				*fr++ = *r++;
			while (*r != ' ' && *r != '\t' && *r != '\n' && *r != '\0');
			*fr++ = 0;
		}
		*fr = 0;
	} else if ((sep = *inputFS) == 0) {		/* new: FS="" => 1 char/field */
		for (i = 0; *r != 0; r += w) {
			char buf[UTFmax + 1];
			Rune chr;

			i++;
			if (i > nfields)
				growfldtab(i);
			if (freeable(fldtab[i]))
				xfree(fldtab[i]->sval);
			w = chartorune(&chr, r);
			n = runetochar(buf, &chr);
			buf[n] = 0;
			fldtab[i]->sval = tostring(buf);
			fldtab[i]->tval = FLD | STR;
		}
		*fr = 0;
	} else if (*r != 0) {	/* if 0, it's a null field */
		for (;;) {
			i++;
			if (i > nfields)
				growfldtab(i);
			if (freeable(fldtab[i]))
				xfree(fldtab[i]->sval);
			fldtab[i]->sval = fr;
			fldtab[i]->tval = FLD | STR | DONTFREE;
			while (*r != sep && *r != '\n' && *r != '\0')	/* \n is always a separator */
				*fr++ = *r++;
			*fr++ = 0;
			if (*r++ == 0)
				break;
		}
		*fr = 0;
	}
	if (i > nfields)
		FATAL("record `%.30s...' has too many fields; can't happen", r);
	cleanfld(i+1, lastfld);	/* clean out junk from previous record */
	lastfld = i;
	donefld = 1;
	for (j = 1; j <= lastfld; j++) {
		p = fldtab[j];
		if(is_number(p->sval)) {
			p->fval = atof(p->sval);
			p->tval |= NUM;
		}
	}
	setfval(nfloc, (Awkfloat) lastfld);
	if (dbg) {
		for (j = 0; j <= lastfld; j++) {
			p = fldtab[j];
			print("field %d (%s): |%s|\n", j, p->nval, p->sval);
		}
	}
}

void cleanfld(int n1, int n2)	/* clean out fields n1 .. n2 inclusive */
{				/* nvals remain intact */
	Cell *p;
	int i;

	for (i = n1; i <= n2; i++) {
		p = fldtab[i];
		if (freeable(p))
			xfree(p->sval);
		p->sval = "";
		p->tval = FLD | STR | DONTFREE;
	}
}

void newfld(int n)	/* add field n after end of existing lastfld */
{
	if (n > nfields)
		growfldtab(n);
	cleanfld(lastfld+1, n);
	lastfld = n;
	setfval(nfloc, (Awkfloat) n);
}

Cell *fieldadr(int n)	/* get nth field */
{
	if (n < 0)
		FATAL("trying to access field %d", n);
	if (n > nfields)	/* fields after NF are empty */
		growfldtab(n);	/* but does not increase NF */
	return(fldtab[n]);
}

void growfldtab(int n)	/* make new fields up to at least $n */
{
	int nf = 2 * nfields;

	if (n > nf)
		nf = n;
	fldtab = (Cell **) realloc(fldtab, (nf+1) * (sizeof (struct Cell *)));
	if (fldtab == nil)
		FATAL("out of space creating %d fields", nf);
	makefields(nfields+1, nf);
	nfields = nf;
}

int refldbld(char *rec, char *fs)	/* build fields from reg expr in FS */
{
	/* this relies on having fields[] the same length as $0 */
	/* the fields are all stored in this one array with \0's */
	char *fr;
	void *p;
	int i, n;

	n = strlen(rec);
	if (n > fieldssize) {
		xfree(fields);
		if ((fields = (char *) malloc(n+1)) == nil)
			FATAL("out of space for fields in refldbld %d", n);
		fieldssize = n;
	}
	fr = fields;
	*fr = '\0';
	if (*rec == '\0')
		return 0;
	p = compre(fs);
	   dprint( ("into refldbld, rec = <%s>, pat = <%s>\n", rec, fs) );
	for (i = 1; ; i++) {
		if (i > nfields)
			growfldtab(i);
		if (freeable(fldtab[i]))
			xfree(fldtab[i]->sval);
		fldtab[i]->tval = FLD | STR | DONTFREE;
		fldtab[i]->sval = fr;
		   dprint( ("refldbld: i=%d\n", i) );
		if (nematch(p, rec, rec)) {
			   dprint( ("match %s (%d chars)\n", patbeg, patlen) );
			strncpy(fr, rec, patbeg-rec);
			fr += patbeg - rec + 1;
			*(fr-1) = '\0';
			rec = patbeg + patlen;
		} else {
			   dprint( ("no match %s\n", rec) );
			strcpy(fr, rec);
			break;
		}
	}
	return i;		
}

void recbld(void)	/* create $0 from $1..$NF if necessary */
{
	int i;
	char *r, *p;

	if (donerec == 1)
		return;
	r = record;
	for (i = 1; i <= *NF; i++) {
		p = getsval(fldtab[i]);
		if (!adjbuf(&record, &recsize, 1+strlen(p)+r-record, recsize, &r, "recbld 1"))
			FATAL("created $0 `%.30s...' too long", record);
		while ((*r = *p++) != 0)
			r++;
		if (i < *NF) {
			if (!adjbuf(&record, &recsize, 2+strlen(*OFS)+r-record, recsize, &r, "recbld 2"))
				FATAL("created $0 `%.30s...' too long", record);
			for (p = *OFS; (*r = *p++) != 0; )
				r++;
		}
	}
	if (!adjbuf(&record, &recsize, 2+r-record, recsize, &r, "recbld 3"))
		FATAL("built giant record `%.30s...'", record);
	*r = '\0';
	   dprint( ("in recbld inputFS=%s, fldtab[0]=%p\n", inputFS, fldtab[0]) );

	if (freeable(fldtab[0]))
		xfree(fldtab[0]->sval);
	fldtab[0]->tval = REC | STR | DONTFREE;
	fldtab[0]->sval = record;

	   dprint( ("in recbld inputFS=%s, fldtab[0]=%p\n", inputFS, fldtab[0]) );
	   dprint( ("recbld = |%s|\n", record) );
	donerec = 1;
}

char	*exitstatus	= nil;

void yyerror(char *s)
{
	SYNTAX(s);
}

void SYNTAX(char *fmt, ...)
{
	extern char *cmdname, *curfname;
	static int been_here = 0;
	va_list varg;

	if (been_here++ > 2)
		return;
	Bprint(&stderr, "%s: ", cmdname);
	va_start(varg, fmt);
	Bvprint(&stderr, fmt, varg);
	va_end(varg);
	if(compile_time == 1 && cursource() != nil)
		Bprint(&stderr, " at %s:%d", cursource(), lineno);
	else
		Bprint(&stderr, " at line %d", lineno);
	if (curfname != nil)
		Bprint(&stderr, " in function %s", curfname);
	Bprint(&stderr, "\n");
	exitstatus = "syntax error";
	eprint();
}

int handler(void *, char *err)
{
	Bflush(&stdout);
	fprint(2, "%s\n", err);
	return 0;
}

extern int bracecnt, brackcnt, parencnt;

void bracecheck(void)
{
	int c;
	static int beenhere = 0;

	if (beenhere++)
		return;
	while ((c = input()) != Beof && c != '\0')
		bclass(c);
	bcheck2(bracecnt, '{', '}');
	bcheck2(brackcnt, '[', ']');
	bcheck2(parencnt, '(', ')');
}

void bcheck2(int n, int, int c2)
{
	if (n == 1)
		Bprint(&stderr, "\tmissing %c\n", c2);
	else if (n > 1)
		Bprint(&stderr, "\t%d missing %c's\n", n, c2);
	else if (n == -1)
		Bprint(&stderr, "\textra %c\n", c2);
	else if (n < -1)
		Bprint(&stderr, "\t%d extra %c's\n", -n, c2);
}

void FATAL(char *fmt, ...)
{
	extern char *cmdname;
	va_list varg;

	Bflush(&stdout);
	Bprint(&stderr, "%s: ", cmdname);
	va_start(varg, fmt);
	Bvprint(&stderr, fmt, varg);
	va_end(varg);
	error();
	if (dbg > 1)		/* core dump if serious debugging on */
		abort();
	exits("FATAL");
}

void WARNING(char *fmt, ...)
{
	extern char *cmdname;
	va_list varg;

	Bflush(&stdout);
	Bprint(&stderr, "%s: ", cmdname);
	va_start(varg, fmt);
	Bvprint(&stderr, fmt, varg);
	va_end(varg);
	error();
}

void error()
{
	extern Node *curnode;
	int line;

	Bprint(&stderr, "\n");
	if (compile_time != 2 && NR && *NR > 0) {
		if (strcmp(*FILENAME, "-") != 0)
			Bprint(&stderr, " input record %s:%d", *FILENAME, (int) (*FNR));
		else
			Bprint(&stderr, " input record number %d", (int) (*FNR));
		Bprint(&stderr, "\n");
	}
	if (compile_time != 2 && curnode)
		line = curnode->lineno;
	else if (compile_time != 2 && lineno)
		line = lineno;
	else
		line = -1;
	if (compile_time == 1 && cursource() != nil){
		if(line >= 0)
			Bprint(&stderr, " source %s:%d", cursource(), line);
		else
			Bprint(&stderr, " source file %s", cursource());
	}else if(line >= 0)
		Bprint(&stderr, " source line %d", line);
	Bprint(&stderr, "\n");
	eprint();
}

void eprint(void)	/* try to print context around error */
{
	char *p, *q;
	int c;
	static int been_here = 0;
	extern char ebuf[], *ep;

	if (compile_time == 2 || compile_time == 0 || been_here++ > 0)
		return;
	p = ep - 1;
	if (p > ebuf && *p == '\n')
		p--;
	for ( ; p > ebuf && *p != '\n' && *p != '\0'; p--)
		;
	while (*p == '\n')
		p++;
	Bprint(&stderr, " context is\n\t");
	for (q=ep-1; q>=p && *q!=' ' && *q!='\t' && *q!='\n'; q--)
		;
	for ( ; p < q; p++)
		if (*p)
			Bputc(&stderr, *p);
	Bprint(&stderr, " >>> ");
	for ( ; p < ep; p++)
		if (*p)
			Bputc(&stderr, *p);
	Bprint(&stderr, " <<< ");
	if (*ep)
		while ((c = input()) != '\n' && c != '\0' && c != Beof) {
			Bputc(&stderr, c);
			bclass(c);
		}
	Bputc(&stderr, '\n');
	ep = ebuf;
}

void bclass(int c)
{
	switch (c) {
	case '{': bracecnt++; break;
	case '}': bracecnt--; break;
	case '[': brackcnt++; break;
	case ']': brackcnt--; break;
	case '(': parencnt++; break;
	case ')': parencnt--; break;
	}
}

double errcheck(double x, char *s)
{

	if (isNaN(x)) {
		WARNING("%s argument out of domain", s);
		x = 1;
	} else if (isInf(x, 1) || isInf(x, -1)) {
		WARNING("%s result out of range", s);
		x = 1;
	}
	return x;
}

int isclvar(char *s)	/* is s of form var=something ? */
{
	char *os = s;

	if (!isalpha(*s) && *s != '_')
		return 0;
	for ( ; *s; s++)
		if (!(isalnum(*s) || *s == '_'))
			break;
	return *s == '=' && s > os && *(s+1) != '=';
}

/* strtod is supposed to be a proper test of what's a valid number */

int is_number(char *s)
{
	double r;
	char *ep;

	/*
	 * fast could-it-be-a-number check before calling strtod,
	 * which takes a surprisingly long time to reject non-numbers.
	 */
	switch (*s) {
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
	case '\t':
	case '\n':
	case '\v':
	case '\f':
	case '\r':
	case ' ':
	case '-':
	case '+':
	case '.':
	case 'n':		/* nans */
	case 'N':
	case 'i':		/* infs */
	case 'I':
		break;
	default:
		return 0;	/* can't be a number */
	}

	r = strtod(s, &ep);
	if (ep == s || isInf(r, 1) || isInf(r, -1) || isNaN(r))
		return 0;
	while (*ep == ' ' || *ep == '\t' || *ep == '\n')
		ep++;
	if (*ep == '\0')
		return 1;
	else
		return 0;
}
