/* Copyright (c) 1988 by Sozobon, Limited.  Author: Johann Ruegg
 *
 * Permission is granted to anyone to use this software for any purpose
 * on any computer system, and to redistribute it freely, with the
 * following restrictions:
 * 1) No charge may be made other than reasonable charges for reproduction.
 * 2) Modified versions must be clearly marked as such.
 * 3) The authors are not responsible for any harmful consequences
 *    of using this software, even if they result from defects in it.
 *
 *	out.c
 *
 *	Code generation output routines.
 */

#include <stdio.h>
#include "param.h"
#include "nodes.h"
#include "flags.h"
#include "bstok.h"
#include "tytok.h"
#include "gen.h"

#ifdef dLibs
#include <ctype.h>
#endif

#if MMCC
overlay "pass2"
#endif

#if CC68
FILE *fopen();
#endif

#if NEEDBUF
char my_obuf[BUFSIZ];
#endif

#define T_SEG	0
#define D_SEG	1
#define B_SEG	2

#define TO_TEXT	to_seg(T_SEG)
#define TO_DATA	to_seg(D_SEG)
#define TO_BSS	to_seg(B_SEG)

#define isareg(np)	((np)->g_token == REGVAR && (np)->g_rno >= AREG)

extern FILE *output;

static int in_seg;
static int lblnum;
static int dat_size;

out_start(s)
char *s;
{
	char *scopy(), *outs;
	register int len;

	outs = scopy(s);
	len = strlen(outs);
	if (len >= 2 && outs[len-2] == '.' &&
			tolower(outs[len-1]) == 'c') {
		outs[len-1] = 's';
		output = fopen(outs, "w");
		if (output == NULL)
			fatals("Cant open", outs);
#if NEEDBUF
		setbuf(output, my_obuf);
#endif
	} else
		output = stdout;
	sfree(outs);

	in_seg = -1;
	lblnum = 0;
	dat_size = 0;
}

out_end()
{
	if (output != stdout)
		fclose(output);
}

static char *sg_go[] = {
	".text",
	".data",
	".bss"
};

to_text()
{
	TO_TEXT;
}

to_seg(sg)
{
	if (sg == in_seg)
		return;
	fprintf(output, "\t%s\n", sg_go[sg]);
	in_seg = sg;
}

o_aln(x)
{
	if (x && (dat_size & 1)) {
		dat_size++;
		TO_DATA;
		fprintf(output, "\t.even\n");
	}
}

char *rnms[] = {
	"d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7",
	"a0", "a1", "a2", "a3", "a4", "a5", "a6", "sp",
};

char *regnm(n)
{
	return rnms[n];
}

char *
init_str(n)
{
	char *s;

	switch (n) {
	case 1:
		s = ".dc.b";	break;
	case 2:
		s = ".dc.w";	break;
	default:
		s = ".dc.l";	break;
	}
	return s;
}

tlen(n)
{
	switch (n) {
	case 1:
		return 'b';
	case 2:
		return 'w';
	default:
		return 'l';
	}
}

o_vinit(tp, xp)
NODEP tp, xp;
{
	fprintf(output, "\t%s\t", init_str((int)tp->t_size));
	dat_size += tp->t_size;

	p2_expr(&xp);
	asn_chk(tp, xp);
	to_init(xp, tp);

	fputc('\n', output);
}

to_init(np, typ)
NODEP np, typ;
{
	NODEP tp;

	tp = allocnode();
	tp->e_token = TCONV;
	tp->n_tptr = typ;
	tp->n_flags |= N_COPYT;
	tp->n_left = np;
	tp->e_type = E_UNARY;
	strcpy(tp->n_name, "i cast");

	genx(tp, FORINIT);
}

out_argreg(np)
NODEP np;
{
	fprintf(output, "\tmove.%c\t%d(a6),%s\n",
		tlen((int)np->n_tptr->t_size), (int)np->e_offs,
		regnm(np->e_rno));
}

out_fstart(np)
NODEP np;
{
	extern	int	pflag;

	TO_TEXT;
	if (np->e_sc != K_STATIC) {
		fprintf(output, "\t.globl\t");
		und_nnm(np);
		fputc('\n', output);
	}
	und_nnm(np);
	fprintf(output, ":\n");	

	if (pflag) {
		int	tlab = new_lbl();

		TO_BSS;
		fprintf(output, "L%d:\t.ds.l\t1\n", tlab);
		TO_TEXT;
		fprintf(output, "\tmove.l\t#");
		und_nnm(np);
		fprintf(output, ",a0\n");

		fprintf(output, "\tmove.l\t#L%d,a1\n", tlab);
		fprintf(output,	"\tjsr\tmcount\n");
	}
}

static char rbuf[30];

char *
regstr(regs)
{
	int lod, hid, loa, hia;
	register i;
	char *bp = rbuf;

	lod = 999;
	hid = -1;
	for (i=DRV_START; i<=DRV_END; i++)
		if (regs & (1<<i)) {
			if (i < lod)  lod = i;
			if (i > hid)  hid = i;
		}
	loa = 999;
	hia = -1;
	for (i=ARV_START; i<=ARV_END; i++)
		if (regs & (1<<i)) {
			if (i < loa)  loa = i;
			if (i > hia)  hia = i;
		}
	if (lod < 999) {
		if (lod != hid)
			sprintf(bp, "d%d-d%d", lod, hid);
		else
			sprintf(bp, "d%d", lod);
		if (loa < 999) {
			bp += strlen(rbuf);
			*bp++ = '/';
		}
	}
	if (loa < 999) {
		if (loa != hia)
			sprintf(bp, "a%d-a%d", loa-AREG, hia-AREG);
		else
			sprintf(bp, "a%d", loa-AREG);
	}
	return rbuf;
}

out_fend(regs, lsize)
long lsize;
{
	if (lsize < 0x7fff)
		fprintf(output, "\tlink\ta6,#-%d\n", (int)lsize);
	else
		fprintf(output, "\tlink\ta6,#0\n\tsub.l\t#%ld,sp\n",
			lsize);
	if (regs)
		fprintf(output, "\tmovem.l\t%s,-(sp)\n", regstr(regs));
}

out_fret(regs, strl)
{
	if (regs)
		fprintf(output, "\tmovem.l\t(sp)+,%s\n", regstr(regs));
	if (strl)
		fprintf(output, "\tmove.l\t#L%d,a0\n", strl);
	fprintf(output, "\tunlk\ta6\n\trts\n");
}

out_fs(strl, size)
long size;
{
	TO_BSS;
	def_lbl(strl);
	fprintf(output, "\t.ds.w\t%ld\n", size/2);
}

out_gv(np, isbss)
register NODEP np;
{
	long sz;
	char c;

	if (np->e_sc == K_STATIC) {
		np->e_offs = lblnum++;
	}
	if (np->e_sc != K_EXTERN) {
		to_seg(isbss ? B_SEG : D_SEG);
		if (np->e_sc != K_STATIC) {
			fprintf(output, "\t.globl\t");
			out_nm(np);
			fputc('\n', output);
		}
		if (isbss) {
			if (np->e_sc == K_STATIC) {
				out_nm(np);
				sz = np->n_tptr->t_size;
				c = 'b';
				if (np->n_tptr->t_aln) {
					c = 'w';
					sz /= 2;
				}
				fprintf(output, ":\t.ds.%c\t%ld\n", c, sz);
			} else {
				fprintf(output, "\t.comm\t");
				out_nm(np);
				sz = np->n_tptr->t_size;
				if (sz & 1) sz++;  /* ALCYON hack */
				fprintf(output, ",%ld\n", sz);
			}
		} else {
			out_nm(np);
			fprintf(output, ":\n");
		}
	}
}

new_lbl()
{
	return lblnum++;
}

def_lbl(l)
{
	fprintf(output, "L%d:\n", l);
}

out_br(l)
{
	if (l < 0)
		error("bad branch");
	else
		fprintf(output, "\tbra\tL%d\n", l);
}

static char *bnm[] = {
	"",
	"beq",
	"bne",
	"blt",
	"bge",
	"ble",
	"bgt",
	"bra",
	"nop",
	"bcs",
	"bcc",
	"bls",
	"bhi"
};

out_b(key, l)
{
	if (key != B_NO)
		fprintf(output, "\t%s\tL%d\n", bnm[key], l);
}

out_bnol(key)
{
	fprintf(output, "\t%s\t", bnm[key]);
}

out_d0cmp(x)
{
	fprintf(output, "\tcmp.w\t#%d,d0\n", x);
}

out_d0sub(x)
{
	fprintf(output, "\tsub.w\t#%d,d0\n", x);
}

out_tlbl(l)
{
	fprintf(output, "\t.dc.l\tL%d\n", l);
}

out_tsw()
{
	fprintf(output, "\text.l\td0\n");
	fprintf(output, "\tasl.l\t#2,d0\n");
	fprintf(output, "\tmove.l\t4(pc,d0.l),a0\n");
	fprintf(output, "\tjmp\t(a0)\n");
}

out_nm(np)
NODEP np;
{
	if (np->e_sc == K_STATIC)
		fprintf(output, "L%d", (int)np->e_offs);
	else
		und_nnm(np);
}

out_zi(tp)
NODEP tp;
{
	char *s;
/*
	switch (tp->t_token) {
	case K_FLOAT:
		fprintf(output, "\t.float\t0.0\n");	return;
	case K_DOUBLE:
		fprintf(output, "\t.double\t0.0\n");	return;
	}
*/
	dat_size += tp->t_size;
	s = init_str((int)tp->t_size);
	fprintf(output, "\t%s\t0\n", s);
}

o_nz(sz, aln)
long sz;
{
	dat_size += sz;
	if (aln) {
		if (sz & 1)
			fprintf(output, "\t.ds.b\t1\n");
		sz >>= 1;
		fprintf(output, "\t.ds.w\t%ld\n", sz);
	} else {
		fprintf(output, "\t.ds.b\t%ld\n", sz);
	}
}

dumpstrs(np)
NODEP np;
{
	TO_DATA;
more:
	if (np == NULL)
		return;
	fprintf(output, "L%d:", (int)np->g_offs);
	out_scon(np);
	np = np->n_next;
	goto more;
}

int see_esc;

out_scon(np)
NODEP np;
{
	int len = 0;

	if (np == NULL)
		return 0;
	see_esc = 0;
more:
	if (np->n_name[0]) {
		fprintf(output, "\t.dc.b\t");
		len += out_str(np->n_name);
		putc('\n', output);
	}
	np = np->n_nmx;
	if (np)
		goto more;

	fprintf(output, "\t.dc.b\t0\n");
	len++;
	dat_size += len;
	return len;
}

out_str(s)
char *s;
{
	int len;
	register c;

	len = 0;
	for ( ; c = *s; s++) {
		if (see_esc) {	/* allow null */
			c--;
			see_esc = 0;
		} else if (c == 1) {
			see_esc = 1;
			continue;
		}
		if (len)
			putc(',', output);
		out_1c(c);
		len++;
	}
	return len;
}

out_asm(np)
NODEP np;
{
	putc('\t', output);
more:
	fprintf(output, "%s", np->n_name);	/* no \0 or \1 please! */
	np = np->n_nmx;
	if (np)
		goto more;
	putc('\n', output);
}

und_nnm(np)
NODEP np;
{
	fputc('_', output);
	fput_nnm(np);
}

out_1c(c)
char c;
{
	fprintf(output, "$%x", c & 0xff);
}

outcode(np)
register NODEP np;
{
	NODEP tp;

	if (np == NULL) return;

	switch (np->g_type) {
	case EV_NONE:
		break;
	case EV_RL:
		outcode(np->n_right);
		outsub(np->g_betw, np);
		/* fall through */
	case EV_LEFT:
		outcode(np->n_left);
		break;
	case EV_LR:
	case EV_LRSEP:
		outcode(np->n_left);
		outsub(np->g_betw, np);
		/* fall through */
	case EV_RIGHT:
		outcode(np->n_right);
		break;
	default:
		printf("bad eval %d ", np->g_type);
	}
	if (np->n_flags & N_COPYT)	/* g_code is a char * */
		outsub(np->g_code, np);
	else				/* g_code is a list of nodes */
		for (tp=np->g_code; tp != NULL; tp = tp->g_code)
			outsub(tp->n_name, np);
}

outsub(cp, np)
register char *cp;
register NODEP np;
{
	register char c;

	if (cp == NULL) return;
	while (c = *cp++)
		if (c == '<')
			out_let(*cp++, np->n_left);
		else if (c == '>')
			out_let(*cp++, np->n_right);
		else if (c == '\'') {
			c = *cp++;
			fputc(c, output);
		} else if (c == 'L')
			seelab(*cp++, np);
		else if (c == 'R')
			seereg(np, *cp++);
		else if (c >= 'A' && c <= 'Z') {
			out_let(c, np);
		} else
			fputc(c, output);
}

seereg(np, c)
NODEP np;
{
	int i;

	switch (c) {
	case '0':	i = np->g_rno;  break;
	case '1':	i = np->g_r1;   break;
	case '2':	i = np->g_r2;   break;
	}
	fprintf(output, regnm(i));	
}

out_let(c, np)
register NODEP np;
{
	int i;

	switch (c) {
	case 'A':
		if (np->g_flags & IMMEDID)
			fputc('#', output);
		out_a(np, output);
		break;
	case 'F':	/* branch if false */
		i = cctok(np);
		i = (i&1) ? i+1 : i-1;	/* reverse truth */
		out_bnol(i);
		break;
	case 'K':
		fprintf(output, "%ld", np->g_bsize);
		break;
	case 'N':
		fprintf(output, "%s", np->n_name);
		break;
	case 'O':
		fprintf(output, "%ld", np->g_offs);
		break;
	case 'Q':
		if (np->g_flags & IMMEDID) {
			warn("constant test expr");
			if (np->g_token == ICON && np->g_offs == 0)
				fprintf(output, "\tor\t#$FF,ccr\n");
			else
				fprintf(output, "\tand\t#0,ccr\n");
			return;
		}
		fprintf(output, "\t%s.%c\t", isareg(np) ? "cmp" : "tst",
			tlen(np->g_sz));
		if (isareg(np))
			fprintf(output, "#0,");
		out_let('A', np);
		fputc('\n', output);
		break;
	case 'S':
		fputc(tlen(np->g_sz), output);
		break;
	case 'T':	/* branch if true */
		out_bnol(cctok(np));
		break;
	case 'U':
		fputc(np->g_ty == ET_U ? 'u' : 's', output);
		break;
	case 'W':	/* field width 1's */
		fprintf(output, "$%x", ones(np->g_fldw));
		break;
	case 'X':	/* ~(W << offset) */
		fprintf(output, "$%x", ~(ones(np->g_fldw)<<np->g_fldo));
		break;
	case 'Y':	/* field offset */
		fprintf(output, "%d", np->g_fldo);
		break;
	case 'Z':	/* field offset - 8 */
		fprintf(output, "%d", np->g_fldo - 8);
		break;
	default:
		printf("bad out_let %c ", c);
	}
}

out_a(np, fd)
register NODEP np;
FILE *fd;
{
	int offs = np->g_offs;

	switch (np->g_token) {
	case ICON:
		fprintf(fd, "%ld", np->g_offs);
		break;
	case FCON:
		/* works for ALCYON C */
		/* otherwise depends on floating internal format */
		fprintf(fd, "$%lx", np->g_offs);
		break;
	case ONAME:
		while (np->g_flags & (CHILDNM|RCHILDNM)) {
			np = (np->g_flags & CHILDNM) ? 
				np->n_left : np->n_right;
		}
		qput_nnm(np, fd);
		if (offs)
			fprintf(fd, offs > 0 ? "+%d" : "%d", offs);
		break;
	case PUSHER:
		fprintf(fd, "(sp)+");
		break;
	case OREG:
		if (offs)
			fprintf(fd, "%d", offs);
		fprintf(fd, "(%s)", regnm(np->g_rno));
		break;
	case REGVAR:
		fprintf(fd, regnm(np->g_rno));
		break;
	case ',':
		fputc(',', fd);		/* for debug */
		break;
	default:
		if (np->g_token >= BR_TOK) {
			fprintf(fd, "B_%s", bnm[np->g_token - BR_TOK]);
			break;
		}
		printf("? tok %d ", np->g_token);
	}
}

seelab(c, np)
char c;
NODEP np;
{
	c -= '1';
	fprintf(output, "L%d", (int)np->g_bsize+c);
}

ones(n)
{
	return (1 << n) - 1;
}
	
