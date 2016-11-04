/*
 * $Id: tacacs.h,v 1.1 2009-07-17 16:10:52 heas Exp $
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
 *
 * TACACS/libtacacs
 */

#ifndef TACACS_H
#define TACACS_H	1

#ifndef TAC_PLUS_PORT
#define	TAC_PLUS_PORT			49
#define	TAC_PLUS_PORTSTR		"49"
#endif

#define TAC_PLUS_READ_TIMEOUT		180	/* seconds */
#define TAC_PLUS_WRITE_TIMEOUT		180	/* seconds */
#define TAC_PLUS_ACCEPT_TIMEOUT		15	/* seconds */

/*
 * All tacacs+ packets have the same header format
 *
 *  1 2 3 4 5 6 7 8  1 2 3 4 5 6 7 8  1 2 3 4 5 6 7 8  1 2 3 4 5 6 7 8
 * +----------------+----------------+----------------+----------------+
 * |major  | minor  |                |                |                |
 * |version| version|      type      |     seq_no     |   flags        |
 * +----------------+----------------+----------------+----------------+
 * |                                                                   |
 * |                            session_id                             |
 * +----------------+----------------+----------------+----------------+
 * |                                                                   |
 * |                              length                               |
 * +----------------+----------------+----------------+----------------+
 */
struct tac_plus_pak_hdr {
    u_char version;
#define TAC_PLUS_MAJOR_VER_MASK 0xf0
#define TAC_PLUS_MAJOR_VER      0xc0

#define TAC_PLUS_MINOR_VER_0    0x0
#define TAC_PLUS_VER_0  (TAC_PLUS_MAJOR_VER | TAC_PLUS_MINOR_VER_0)

#define TAC_PLUS_MINOR_VER_1    0x01
#define TAC_PLUS_VER_1  (TAC_PLUS_MAJOR_VER | TAC_PLUS_MINOR_VER_1)

    u_char type;
#define TAC_PLUS_AUTHEN			1
#define TAC_PLUS_AUTHOR			2
#define TAC_PLUS_ACCT			3

    u_char seq_no;		/* packet sequence number */
    u_char flags;		/* protocol flags, as below */
#define	TAC_PLUS_UNENCRYPTED	0x1		/* packet is NOT encrypted   */
#define	TAC_PLUS_SINGLE_CONNECT_FLAG 0x04

    int session_id;		/* session identifier */
    int datalength;		/* length of encrypted data following this
				 * header */
    /* data bytes of encrypted data follows */
};

#define TAC_PLUS_HDR_SIZE	12

#define TAC_MD5_DIGEST_LEN	16

/* accounting record structure */
struct acct_rec {
    int acct_type;		/* start, stop, update */
#define ACCT_TYPE_START      1
#define ACCT_TYPE_STOP       2
#define ACCT_TYPE_UPDATE     3
    struct identity *identity;
    int authen_method;
    int authen_type;
    int authen_service;
    char *msg;       /* output field */
    char *admin_msg; /* output field */
    int num_args;
    char **args;
};

/* Authentication packet NAS sends to us */
struct authen_start {
    u_char action;
#define TAC_PLUS_AUTHEN_LOGIN    0x1
#define TAC_PLUS_AUTHEN_CHPASS   0x2
#define TAC_PLUS_AUTHEN_SENDPASS 0x3 /* deprecated */
#define TAC_PLUS_AUTHEN_SENDAUTH 0x4
    u_char priv_lvl;
#define TAC_PLUS_PRIV_LVL_MIN 0x0
#define TAC_PLUS_PRIV_LVL_MAX 0xf
    u_char authen_type;
#define TAC_PLUS_AUTHEN_TYPE_ASCII  1
#define TAC_PLUS_AUTHEN_TYPE_PAP    2
#define TAC_PLUS_AUTHEN_TYPE_CHAP   3
#define TAC_PLUS_AUTHEN_TYPE_ARAP   4
#ifdef MSCHAP
#define TAC_PLUS_AUTHEN_TYPE_MSCHAP 5
#endif /* MSCHAP */
    u_char service;
#define TAC_PLUS_AUTHEN_SVC_LOGIN  1
#define TAC_PLUS_AUTHEN_SVC_ENABLE 2
#define TAC_PLUS_AUTHEN_SVC_PPP    3
#define TAC_PLUS_AUTHEN_SVC_ARAP   4
#define TAC_PLUS_AUTHEN_SVC_PT     5
#define TAC_PLUS_AUTHEN_SVC_RCMD   6
#define TAC_PLUS_AUTHEN_SVC_X25    7
#define TAC_PLUS_AUTHEN_SVC_NASI   8
    u_char user_len;				/* bytes of char data */
    u_char port_len;				/* bytes of char data */
    u_char rem_addr_len;			/* bytes of u_char data */
    u_char data_len;				/* bytes of u_char data */
};

#define TAC_AUTHEN_START_FIXED_FIELDS_SIZE 8

/* Authentication continue packet NAS sends to us */
struct authen_cont {
    u_short user_msg_len;
    u_short user_data_len;
    u_char flags;
#define TAC_PLUS_CONTINUE_FLAG_ABORT 0x1
    /* <user_msg_len bytes of u_char data> */
    /* <user_data_len bytes of u_char data> */
};

#define TAC_AUTHEN_CONT_FIXED_FIELDS_SIZE 5

/* Authentication reply packet we send to NAS */
struct authen_reply {
    u_char status;
#define TAC_PLUS_AUTHEN_STATUS_PASS     1
#define TAC_PLUS_AUTHEN_STATUS_FAIL     2
#define TAC_PLUS_AUTHEN_STATUS_GETDATA  3
#define TAC_PLUS_AUTHEN_STATUS_GETUSER  4
#define TAC_PLUS_AUTHEN_STATUS_GETPASS  5
#define TAC_PLUS_AUTHEN_STATUS_RESTART  6
#define TAC_PLUS_AUTHEN_STATUS_ERROR    7
#define TAC_PLUS_AUTHEN_STATUS_FOLLOW   0x21
    u_char flags;
#define TAC_PLUS_AUTHEN_FLAG_NOECHO     0x1
    u_short msg_len;
    u_short data_len;
    /* <msg_len bytes of char data> */
    /* <data_len bytes of u_char data> */
};

#define TAC_AUTHEN_REPLY_FIXED_FIELDS_SIZE 6

/* An authorization request packet */
struct author {
    u_char authen_method;
    u_char priv_lvl;
    u_char authen_type;
    u_char service;

    u_char user_len;				/* bytes of char data */
    u_char port_len;				/* bytes of char data */
    u_char rem_addr_len;			/* bytes of u_char data */
    u_char arg_cnt;				/* the number of args */

    /* <arg_cnt u_chars containing the lengths of args 1 to arg n> */
    /* <char data for each arg> */
};

#define TAC_AUTHOR_REQ_FIXED_FIELDS_SIZE 8

/* An authorization reply packet */
struct author_reply {
    u_char status;
    u_char arg_cnt;
    u_short msg_len;
    u_short data_len;

    /* <arg_cnt u_chars containing the lengths of arg 1 to arg n> */
    /* <msg_len bytes of char data> */
    /* <data_len bytes of char data> */
    /* <char data for each arg> */
};

#define TAC_AUTHOR_REPLY_FIXED_FIELDS_SIZE 6

/* This structure describes a principal that is to be authenticated. */
struct identity {
    char *username;		/* principals name (ASCII, null terminated) */
    char *NAS_name;		/* name of the NAS where the user is */
    char *NAS_port;		/* port on the NAS where the user is */
    char *NAS_ip;		/* IP address of the NAS */
    char *NAC_address;		/* remote user location. This may be a remote
				 * IP address or a caller-ID or ...
				 */
    int priv_lvl;		/* user's requested privilege level */
};

int countusers_by_finger(struct identity *);
int countuser(struct identity *);
void loguser(struct acct_rec *);
void maxsess_loginit(void);

/* XXX add a function to set this in the library */
extern char *wholog;

/*
 * This is state kept per user/session for maxsess wholog file
 */
struct peruser {
    char username[64];		/* User name */
    char NAS_name[32];		/* NAS user logged into */
    char NAS_port[32];		/*  ...port on that NAS */
    char NAC_address[64];	/*  ...IP address of NAS */
};

#endif	/* TACACS_H */
