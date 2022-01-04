/*
 * Copyright (C) 2000-2012 Free Software Foundation, Inc.
 * Copyright (C) 2017 Red Hat, Inc.
 *
 * This file is part of GnuTLS.
 *
 * GnuTLS is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GnuTLS is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see
 * <https://www.gnu.org/licenses/>.
 */

#include <config.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <gnutls/gnutls.h>
#include <sys/time.h>
#if HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#elif HAVE_WS2TCPIP_H
#include <ws2tcpip.h>
#endif
#include <tests.h>
#include <common.h>
#include <ctype.h>
#include <cli-debug-args.h>
#include <socket.h>

/* Gnulib portability files. */
#include "sockets.h"

static void cmd_parser(int argc, char **argv);

/* global stuff here */
int resume;
char *hostname = NULL;
int port;
int record_max_size;
int fingerprint;
static int debug = 0;

gnutls_srp_client_credentials_t srp_cred;
gnutls_anon_client_credentials_t anon_cred;
gnutls_certificate_credentials_t xcred;

/* end of global stuff */

unsigned int verbose = 0;

extern int tls1_ok;
extern int tls1_1_ok;
extern int tls1_2_ok;
extern int tls1_3_ok;
extern int ssl3_ok;
extern const char *ext_text;

static void tls_log_func(int level, const char *str)
{
	fprintf(stderr, "|<%d>| %s", level, str);
}

typedef test_code_t(*TEST_FUNC) (gnutls_session_t);

typedef struct {
	const char *test_name;
	TEST_FUNC func;
	const char *suc_str;
	const char *fail_str;
	const char *unsure_str;
	unsigned https_only;
} TLS_TEST;

static const TLS_TEST tls_tests[] = {
#ifdef ENABLE_SSL3
	{"for SSL 3.0 (RFC6101) support", test_ssl3, "yes", "no", "dunno"},
	/* The following tests will disable TLS 1.x if the server is
	 * buggy */
#endif
	{"whether we need to disable TLS 1.2", test_tls_disable2, "no",
	 "yes", "dunno"},
	{"whether we need to disable TLS 1.1", test_tls_disable1, "no",
	 "yes", "dunno"},
	{"whether we need to disable TLS 1.0", test_tls_disable0, "no",
	 "yes", "dunno"},
	{"whether \%NO_EXTENSIONS is required", test_no_extensions, "no", "yes",
	 "dunno"},
	{"whether \%COMPAT is required", test_record_padding, "no", "yes",
	 "dunno"},
	{"for TLS 1.0 (RFC2246) support", test_tls1, "yes", "no", "dunno"},
	{"for TLS 1.0 (RFC2246) support with TLS 1.0 record version", test_tls1_nossl3, "yes", "no", "dunno"},
	{"for TLS 1.1 (RFC4346) support", test_tls1_1, "yes", "no", "dunno"},
	{"fallback from TLS 1.1 to", test_tls1_1_fallback, "TLS 1.0",
	 "failed",
	 "SSL 3.0"},
	{"for TLS 1.2 (RFC5246) support", test_tls1_2, "yes", "no", "dunno"},
	{"for TLS 1.3 (RFC8446) support", test_tls1_3, "yes", "no", "dunno"},
	{"TLS1.2 neg fallback from TLS 1.6 to", test_tls1_6_fallback, NULL,
	 "failed (server requires fallback dance)", "dunno"},
	{"for inappropriate fallback (RFC7507) support", test_rfc7507, "yes", "no", "dunno"},
	{"for HTTPS server name", test_server, NULL, "failed", "not checked", 1},
	{"for certificate information", test_certificate, NULL, "", ""},
	{"for certificate chain order", test_chain_order, "sorted", "unsorted", "unknown"},
	{"for trusted CAs", test_server_cas, NULL, "", ""},
	{"for safe renegotiation (RFC5746) support", test_safe_renegotiation, "yes",
	 "no", "dunno"},
	{"for Safe renegotiation support (SCSV)",
	 test_safe_renegotiation_scsv,
	 "yes", "no", "dunno"},
	{"for encrypt-then-MAC (RFC7366) support", test_etm, "yes", "no", "dunno"},
	{"for ext master secret (RFC7627) support", test_ext_master_secret, "yes", "no", "dunno"},
	{"for heartbeat (RFC6520) support", test_heartbeat_extension, "yes", "no", "dunno"},
	{"for version rollback bug in RSA PMS", test_rsa_pms, "no", "yes",
	 "dunno"},
	{"for version rollback bug in Client Hello", test_version_rollback,
	 "no", "yes", "dunno"},
	{"whether the server ignores the RSA PMS version",
	 test_rsa_pms_version_check, "yes", "no", "dunno"},
	{"whether small records (512 bytes) are tolerated on handshake",
	 test_small_records, "yes", "no", "dunno"},
	{"whether cipher suites not in SSL 3.0 spec are accepted",
	 test_unknown_ciphersuites, "yes", "no", "dunno"},
	{"whether a bogus TLS record version in the client hello is accepted", test_version_oob, "yes", "no", "dunno"},
	{"whether the server understands TLS closure alerts", test_bye,
	 "yes", "no", "partially"},
	/* the fact that is after the closure alert test does matter.
	 */
	{"whether the server supports session resumption",
	 test_session_resume2, "yes", "no", "dunno"},
#ifdef ENABLE_ANON
	{"for anonymous authentication support", test_anonymous, "yes",
	 "no",
	 "dunno"},
	{"anonymous Diffie-Hellman group info", test_dhe_group, NULL, "N/A",
	 "N/A"},
#endif
	{"for ephemeral Diffie-Hellman support", test_dhe, "yes", "no",
	 "dunno"},
	{"for RFC7919 Diffie-Hellman support", test_rfc7919, "yes", "no",
	 "dunno"},
	{"ephemeral Diffie-Hellman group info", test_dhe_group, NULL, "N/A",
	 "N/A"},
	{"for ephemeral EC Diffie-Hellman support", test_ecdhe, "yes",
	 "no",
	 "dunno"},
	{"for curve SECP256r1 (RFC4492)", test_ecdhe_secp256r1, "yes", "no", "dunno"},
	{"for curve SECP384r1 (RFC4492)", test_ecdhe_secp384r1, "yes", "no", "dunno"},
	{"for curve SECP521r1 (RFC4492)", test_ecdhe_secp521r1, "yes", "no", "dunno"},
	{"for curve X25519 (RFC8422)", test_ecdhe_x25519, "yes", "no", "dunno"},
	{"for AES-GCM cipher (RFC5288) support", test_aes_gcm, "yes", "no",
	 "dunno"},
	{"for AES-CCM cipher (RFC6655) support", test_aes_ccm, "yes", "no",
	 "dunno"},
	{"for AES-CCM-8 cipher (RFC6655) support", test_aes_ccm_8, "yes", "no",
	 "dunno"},
	{"for AES-CBC cipher (RFC3268) support", test_aes, "yes", "no",
	 "dunno"},
	{"for CAMELLIA-GCM cipher (RFC6367) support", test_camellia_gcm, "yes", "no",
	 "dunno"},
	{"for CAMELLIA-CBC cipher (RFC5932) support", test_camellia_cbc, "yes", "no",
	 "dunno"},
	{"for 3DES-CBC cipher (RFC2246) support", test_3des, "yes", "no", "dunno"},
	{"for ARCFOUR 128 cipher (RFC2246) support", test_arcfour, "yes", "no",
	 "dunno"},
	{"for CHACHA20-POLY1305 cipher (RFC7905) support", test_chacha20, "yes", "no",
	 "dunno"},
	{"for MD5 MAC support", test_md5, "yes", "no", "dunno"},
	{"for SHA1 MAC support", test_sha, "yes", "no", "dunno"},
	{"for SHA256 MAC support", test_sha256, "yes", "no", "dunno"},
	{"for max record size (RFC6066) support", test_max_record_size, "yes",
	 "no", "dunno"},
#ifdef ENABLE_OCSP
	{"for OCSP status response (RFC6066) support", test_ocsp_status, "yes",
	 "no", "dunno"},
#endif
	{NULL, NULL, NULL, NULL, NULL}
};

const char *ip;

gnutls_session_t init_tls_session(const char *host)
{
	gnutls_session_t state = NULL;
	gnutls_init(&state, GNUTLS_CLIENT);

	set_read_funcs(state);
	if (host && is_ip(host) == 0)
		gnutls_server_name_set(state, GNUTLS_NAME_DNS,
				       host, strlen(host));

	return state;
}

int do_handshake(socket_st * socket)
{
	return 0; /* we do it locally */
}

int main(int argc, char **argv)
{
	int ret;
	int i;
	char portname[6];
	socket_st hd;
	char app_proto[32] = "";

	cmd_parser(argc, argv);

#ifndef _WIN32
	signal(SIGPIPE, SIG_IGN);
#endif

	sockets_init();

	if (gnutls_global_init() < 0) {
		fprintf(stderr, "global state initialization error\n");
		exit(1);
	}

	gnutls_global_set_log_function(tls_log_func);
	gnutls_global_set_log_level(debug);

	/* get server name */
	snprintf(portname, sizeof(portname), "%d", port);

	/* X509 stuff */
	if (gnutls_certificate_allocate_credentials(&xcred) < 0) {	/* space for 2 certificates */
		fprintf(stderr, "memory error\n");
		exit(1);
	}

	/* SRP stuff */
#ifdef ENABLE_SRP
	if (gnutls_srp_allocate_client_credentials(&srp_cred) < 0) {
		fprintf(stderr, "memory error\n");
		exit(1);
	}
#endif

#ifdef ENABLE_ANON
	/* ANON stuff */
	if (gnutls_anon_allocate_client_credentials(&anon_cred) < 0) {
		fprintf(stderr, "memory error\n");
		exit(1);
	}
#endif

	if (HAVE_OPT(STARTTLS_PROTO)) {
		snprintf(app_proto, sizeof(app_proto), "%s", OPT_ARG(STARTTLS_PROTO));
	}

	if (app_proto[0] == 0) {
		snprintf(app_proto, sizeof(app_proto), "%s", port_to_service(portname, "tcp"));
	}

	sockets_init();

	i = 0;

	printf("GnuTLS debug client %s\n", gnutls_check_version(NULL));

	canonicalize_host(hostname, portname, sizeof(portname));
	printf("Checking %s:%s\n", hostname, portname);
	do {

		if (tls_tests[i].test_name == NULL)
			break;	/* finished */

		/* if neither of SSL3 and TLSv1 are supported, exit
		 */
		if (i > 11 && tls1_2_ok == 0 && tls1_1_ok == 0 && tls1_ok == 0
		    && ssl3_ok == 0 && tls1_3_ok == 0) {
			fprintf(stderr,
				"\nServer does not support any of SSL 3.0, TLS 1.0, 1.1, 1.2 and 1.3\n");
			break;
		}

		socket_open(&hd, hostname, portname, app_proto, SOCKET_FLAG_STARTTLS|SOCKET_FLAG_RAW, NULL, NULL);
		hd.verbose = verbose;

		do {
			if (strcmp(app_proto, "https") != 0 && tls_tests[i].https_only != 0) {
				i++;
				break;
			}

			ret = tls_tests[i].func(hd.session);

			if (ret != TEST_IGNORE && ret != TEST_IGNORE2) {
				printf("%58s...", tls_tests[i].test_name);
				fflush(stdout);
			}

			if (ret == TEST_SUCCEED) {
				if (tls_tests[i].suc_str == NULL)
					printf(" %s\n", ext_text);
				else
					printf(" %s\n", tls_tests[i].suc_str);
			} else if (ret == TEST_FAILED)
				printf(" %s\n", tls_tests[i].fail_str);
			else if (ret == TEST_UNSURE)
				printf(" %s\n", tls_tests[i].unsure_str);
			else if (ret == TEST_IGNORE) {
				if (tls_tests[i+1].test_name)
					i++;
				else
					break;
			}
		}
		while (ret == TEST_IGNORE
		       && tls_tests[i].test_name != NULL);

		socket_bye(&hd, 1);

		i++;
	}
	while (1);

#ifdef ENABLE_SRP
	gnutls_srp_free_client_credentials(srp_cred);
#endif
	gnutls_certificate_free_credentials(xcred);
#ifdef ENABLE_ANON
	gnutls_anon_free_client_credentials(anon_cred);
#endif
	gnutls_global_deinit();

	return 0;
}

static void cmd_parser(int argc, char **argv)
{
	char *rest = NULL;
	static char lh[] = "localhost";
	int optct = optionProcess(&gnutls_cli_debugOptions, argc, argv);
	argc -= optct;
	argv += optct;

	if (rest == NULL && argc > 0)
		rest = argv[0];

	if (HAVE_OPT(PORT))
		port = OPT_VALUE_PORT;
	else {
		if (HAVE_OPT(STARTTLS_PROTO))
			port = starttls_proto_to_port(OPT_ARG(STARTTLS_PROTO));
		else
			port = 443;
	}

	if (rest == NULL)
		hostname = lh;
	else
		hostname = rest;

	if (HAVE_OPT(DEBUG))
		debug = OPT_VALUE_DEBUG;

	if (HAVE_OPT(VERBOSE))
		verbose++;

}
