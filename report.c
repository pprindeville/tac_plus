/*
 * $Id: report.c,v 1.16 2009-07-16 16:58:23 heas Exp $
 *
 * Copyright (c) 1995-1998 by Cisco systems, Inc.
 *
 * Permission to use, copy, modify, and distribute this software for
 * any purpose and without fee is hereby granted, provided that this
 * copyright and permission notice appear on all copies of the
 * software and supporting documentation, the name of Cisco Systems,
 * Inc. not be used in advertising or publicity pertaining to
 * distribution of the program without specific prior permission, and
 * notice be given in supporting documentation that modification,
 * copying and distribution is by permission of Cisco Systems, Inc.
 *
 * Cisco Systems, Inc. makes no representations about the suitability
 * of this software for any purpose.  THIS SOFTWARE IS PROVIDED ``AS
 * IS'' AND WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
 * WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE.
 */

#include "tac_plus.h"
#include <stdio.h>

#ifdef AIX
#include <sys/types.h>
#else
#include <time.h>
#endif

#ifdef __STDC__
#include <stdarg.h>		/* ANSI C, variable length args */
#else
#include <varargs.h>		/* has 'vararg' definitions */
#endif

FILE *ostream = NULL;

char *logfile = TACPLUS_LOGFILE;

unsigned ts_format = REPORT_TS_UNIX;

/* report:
 *
 * This routine reports errors and such via stderr and syslog() if
 * appopriate.
 *
 * LOG_DEBUG messages are ignored unless debugging is on.
 * All other priorities are always logged to syslog.
 */
#ifdef __STDC__
void
report(int priority, char *fmt, ...)
#else
/* VARARGS2 */
void
report(priority, fmt, va_alist)
    int priority;
    char *fmt;
    va_dcl				/* no terminating semi-colon */
#endif
{
    char msg[4096];		/* temporary string */
    size_t nchars;
    FILE *mem;
    va_list ap;

    mem = fmemopen(msg, sizeof(msg), "w");

#ifdef __STDC__
    va_start(ap, fmt);
#else
    va_start(ap);
#endif
    vfprintf(mem, fmt, ap);
    va_end(ap);

    fflush(mem);
    nchars = ftell(mem);

    fclose(mem);

    if (console) {
	if (!ostream)
	    ostream = fopen("/dev/console", "w");

	if (ostream) {
	    if (priority == LOG_ERR)
		fprintf(ostream, "Error ");
	    fwrite(msg, nchars, 1, ostream);
	    fputc('\n', ostream);
	} else
	    syslog(LOG_ERR, "Cannot open /dev/console errno=%d", errno);
    }

    if (debug) {
	int logfd;

	logfd = open(logfile, O_CREAT | O_WRONLY | O_APPEND, 0644);
	if (logfd >= 0) {
	    time_t t = time(NULL);
	    char *ct = ctime(&t);
	    FILE *flog;

	    ct[24] = '\0';

	    tac_lockfd(logfile, logfd);
	    flog = fdopen(logfd, "a");

	    fprintf(flog, "%s [%ld]: ", ct, (long)getpid());
	    if (priority == LOG_ERR)
		fputs("Error ", flog);
	    fwrite(msg, nchars, 1, flog);
	    fputc('\n', flog);
	    fclose(flog);
	}
    }

    if (single) {
#ifdef USE_STDERR_TIMESTAMPS
	switch (ts_format) {
	case REPORT_TS_NONE:
	    break;
	case REPORT_TS_UNIX:
	    fputs(tac_timestamp(), stderr);
	    fputc(' ', stderr);
	    break;
	case REPORT_TS_ISO:
	    fputs(tac_iso_timestamp(), stderr);
	    fputc(' ', stderr);
	    break;
	}
#endif
	fwrite(msg, nchars, 1, stderr);
	fputc('\n', stderr);
    }

    if (priority == LOG_ERR)
	syslog(priority, "Error %s", msg);
    else
	syslog(priority, "%s", msg);
}

/* format a hex dump for syslog */
void
report_hex(int priority, u_char *p, unsigned len)
{
    char buf[256];
    unsigned i;
    FILE *mem;

    if (len <= 0)
	return;

    mem = fmemopen(buf, sizeof(buf), "w");

    if (len > 255)
	len = 255;

    for (i = 0; i < len; i++, p++) {

	fprintf(mem, "0x%02x ", *p);

	if (ftell(mem) > 75) {
	    fflush(mem);
	    report(priority, "%s", buf);
	    rewind(mem);
	}
    }

    if (ftell(mem) > 0) {
	fflush(mem);
	report(priority, "%s", buf);
    }

    fclose(mem);

    return;
}

/* format a non-null terminated string for syslog */
void
report_string(int priority, u_char *p, unsigned len)
{
    char buf[256];
    unsigned i, n;
    FILE *mem;

    if (len <= 0)
	return;

    if (len > 255)
	len = 255;

    mem = fmemopen(buf, sizeof(buf), "w");

    for (i = n = 0; i < len && n < len; i++, p++) {
	/* ASCII printable, else ... */
	if (32 <= *p && *p <= 126) {
	    fputc(*p, mem);
	    ++n;
	} else {
	    fprintf(mem, " 0x%02x", *p);
	    n += 6;
	}
    }

    fclose(mem);

    report(priority, "%s", buf);
}

void
regerror(char *s)
{
    report(LOG_ERR, "in regular expression %s", s);
}
