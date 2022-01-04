/*
 * Copyright (C) 2011-2012 Free Software Foundation, Inc.
 *
 * Author: Nikos Mavrogiannopoulos
 *
 * This file is part of GnuTLS.
 *
 * The GnuTLS is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>
 *
 */

#include "gnutls_int.h"
#include <algorithms.h>
#include "errors.h"
#include <x509/common.h>
#include <pk.h>
#include "c-strcase.h"

/* Supported ECC curves
 */

static const gnutls_ecc_curve_entry_st ecc_curves[] = {
	{
	 .name = "SECP192R1",
	 .oid = "1.2.840.10045.3.1.1",
	 .id = GNUTLS_ECC_CURVE_SECP192R1,
	 .pk = GNUTLS_PK_ECDSA,
	 .size = 24,
	},
	{
	 .name = "SECP224R1",
	 .oid = "1.3.132.0.33",
	 .id = GNUTLS_ECC_CURVE_SECP224R1,
	 .pk = GNUTLS_PK_ECDSA,
	 .size = 28,
	},
	{
	 .name = "SECP256R1",
	 .oid = "1.2.840.10045.3.1.7",
	 .id = GNUTLS_ECC_CURVE_SECP256R1,
	 .pk = GNUTLS_PK_ECDSA,
	 .size = 32,
	},
	{
	 .name = "SECP384R1",
	 .oid = "1.3.132.0.34",
	 .id = GNUTLS_ECC_CURVE_SECP384R1,
	 .pk = GNUTLS_PK_ECDSA,
	 .size = 48,
	},
	{
	 .name = "SECP521R1",
	 .oid = "1.3.132.0.35",
	 .id = GNUTLS_ECC_CURVE_SECP521R1,
	 .pk = GNUTLS_PK_ECDSA,
	 .size = 66,
	},
	{
	 .name = "X25519",
	 .id = GNUTLS_ECC_CURVE_X25519,
	 .pk = GNUTLS_PK_ECDH_X25519,
	 .size = 32,
	},
	{
	 .name = "Ed25519",
	 .oid = SIG_EDDSA_SHA512_OID,
	 .id = GNUTLS_ECC_CURVE_ED25519,
	 .pk = GNUTLS_PK_EDDSA_ED25519,
	 .size = 32,
	 .sig_size = 64
	},
	{
	 .name = "CryptoPro-A",
	 .oid = "1.2.643.2.2.35.1",
	 .id = GNUTLS_ECC_CURVE_GOST256CPA,
	 .pk = GNUTLS_PK_UNKNOWN,
	 .size = 32,
	 .gost_curve = 1,
	},
	{
	 .name = "CryptoPro-B",
	 .oid = "1.2.643.2.2.35.2",
	 .id = GNUTLS_ECC_CURVE_GOST256CPB,
	 .pk = GNUTLS_PK_UNKNOWN,
	 .size = 32,
	 .gost_curve = 1,
	},
	{
	 .name = "CryptoPro-C",
	 .oid = "1.2.643.2.2.35.3",
	 .id = GNUTLS_ECC_CURVE_GOST256CPC,
	 .pk = GNUTLS_PK_UNKNOWN,
	 .size = 32,
	 .gost_curve = 1,
	},
	{
	 .name = "CryptoPro-XchA",
	 .oid = "1.2.643.2.2.36.0",
	 .id = GNUTLS_ECC_CURVE_GOST256CPXA,
	 .pk = GNUTLS_PK_UNKNOWN,
	 .size = 32,
	 .gost_curve = 1,
	},
	{
	 .name = "CryptoPro-XchB",
	 .oid = "1.2.643.2.2.36.1",
	 .id = GNUTLS_ECC_CURVE_GOST256CPXB,
	 .pk = GNUTLS_PK_UNKNOWN,
	 .size = 32,
	 .gost_curve = 1,
	},
	{
	 .name = "TC26-512-A",
	 .oid = "1.2.643.7.1.2.1.2.1",
	 .id = GNUTLS_ECC_CURVE_GOST512A,
	 .pk = GNUTLS_PK_GOST_12_512,
	 .size = 64,
	 .gost_curve = 1,
	},
	{
	 .name = "TC26-512-B",
	 .oid = "1.2.643.7.1.2.1.2.2",
	 .id = GNUTLS_ECC_CURVE_GOST512B,
	 .pk = GNUTLS_PK_GOST_12_512,
	 .size = 64,
	 .gost_curve = 1,
	},
	{0, 0, 0}
};

#define GNUTLS_ECC_CURVE_LOOP(b) \
	{ const gnutls_ecc_curve_entry_st *p; \
		for(p = ecc_curves; p->name != NULL; p++) { b ; } }


/**
 * gnutls_ecc_curve_list:
 *
 * Get the list of supported elliptic curves.
 *
 * This function is not thread safe.
 *
 * Returns: Return a (0)-terminated list of #gnutls_ecc_curve_t
 *   integers indicating the available curves.
 **/
const gnutls_ecc_curve_t *gnutls_ecc_curve_list(void)
{
	static gnutls_ecc_curve_t supported_curves[MAX_ALGOS] = { 0 };

	if (supported_curves[0] == 0) {
		int i = 0;

		GNUTLS_ECC_CURVE_LOOP(
			if (_gnutls_pk_curve_exists(p->id))
				supported_curves[i++] = p->id;
		);
		supported_curves[i++] = 0;
	}

	return supported_curves;
}

/**
 * gnutls_oid_to_ecc_curve:
 * @oid: is a curve's OID
 *
 * Returns: return a #gnutls_ecc_curve_t value corresponding to
 *   the specified OID, or %GNUTLS_ECC_CURVE_INVALID on error.
 *
 * Since: 3.4.3
 **/
gnutls_ecc_curve_t gnutls_oid_to_ecc_curve(const char *oid)
{
	gnutls_ecc_curve_t ret = GNUTLS_ECC_CURVE_INVALID;

	GNUTLS_ECC_CURVE_LOOP(
		if (p->oid != NULL && c_strcasecmp(p->oid, oid) == 0 && _gnutls_pk_curve_exists(p->id)) {
			ret = p->id;
			break;
		}
	);

	return ret;
}

/**
 * gnutls_ecc_curve_get_id:
 * @name: is a curve name
 *
 * The names are compared in a case insensitive way.
 *
 * Returns: return a #gnutls_ecc_curve_t value corresponding to
 *   the specified curve, or %GNUTLS_ECC_CURVE_INVALID on error.
 *
 * Since: 3.4.3
 **/
gnutls_ecc_curve_t gnutls_ecc_curve_get_id(const char *name)
{
	gnutls_ecc_curve_t ret = GNUTLS_ECC_CURVE_INVALID;

	GNUTLS_ECC_CURVE_LOOP(
		if (c_strcasecmp(p->name, name) == 0 && _gnutls_pk_curve_exists(p->id)) {
			ret = p->id;
			break;
		}
	);

	return ret;
}

static int _gnutls_ecc_pk_compatible(const gnutls_ecc_curve_entry_st *p,
				     gnutls_pk_algorithm_t pk)
{
	if (!_gnutls_pk_curve_exists(p->id))
		return 0;

	if (pk == GNUTLS_PK_GOST_01 ||
	    pk == GNUTLS_PK_GOST_12_256)
		return p->gost_curve && p->size == 32;

	return pk == p->pk;
}

/*-
 * _gnutls_ecc_bits_to_curve:
 * @bits: is a security parameter in bits
 *
 * Returns: return a #gnutls_ecc_curve_t value corresponding to
 *   the specified bit length, or %GNUTLS_ECC_CURVE_INVALID on error.
 -*/
gnutls_ecc_curve_t _gnutls_ecc_bits_to_curve(gnutls_pk_algorithm_t pk, int bits)
{
	gnutls_ecc_curve_t ret;

	if (pk == GNUTLS_PK_ECDSA)
		ret = GNUTLS_ECC_CURVE_SECP256R1;
	else if (pk == GNUTLS_PK_GOST_01 ||
		 pk == GNUTLS_PK_GOST_12_256)
		ret = GNUTLS_ECC_CURVE_GOST256CPA;
	else if (pk == GNUTLS_PK_GOST_12_512)
		ret = GNUTLS_ECC_CURVE_GOST512A;
	else
		ret = GNUTLS_ECC_CURVE_ED25519;

	GNUTLS_ECC_CURVE_LOOP(
		if (_gnutls_ecc_pk_compatible(p, pk) && 8 * p->size >= (unsigned)bits) {
			ret = p->id;
			break;
		}
	);

	return ret;
}

/**
 * gnutls_ecc_curve_get_name:
 * @curve: is an ECC curve
 *
 * Convert a #gnutls_ecc_curve_t value to a string.
 *
 * Returns: a string that contains the name of the specified
 *   curve or %NULL.
 *
 * Since: 3.0
 **/
const char *gnutls_ecc_curve_get_name(gnutls_ecc_curve_t curve)
{
	const char *ret = NULL;

	GNUTLS_ECC_CURVE_LOOP(
		if (p->id == curve) {
			ret = p->name;
			break;
		}
	);

	return ret;
}

/**
 * gnutls_ecc_curve_get_oid:
 * @curve: is an ECC curve
 *
 * Convert a #gnutls_ecc_curve_t value to its object identifier.
 *
 * Returns: a string that contains the OID of the specified
 *   curve or %NULL.
 *
 * Since: 3.4.3
 **/
const char *gnutls_ecc_curve_get_oid(gnutls_ecc_curve_t curve)
{
	const char *ret = NULL;

	GNUTLS_ECC_CURVE_LOOP(
		if (p->id == curve) {
			ret = p->oid;
			break;
		}
	);

	return ret;
}

/*-
 * _gnutls_ecc_curve_get_params:
 * @curve: is an ECC curve
 *
 * Returns the information on a curve.
 *
 * Returns: a pointer to #gnutls_ecc_curve_entry_st or %NULL.
 -*/
const gnutls_ecc_curve_entry_st
    *_gnutls_ecc_curve_get_params(gnutls_ecc_curve_t curve)
{
	const gnutls_ecc_curve_entry_st *ret = NULL;

	GNUTLS_ECC_CURVE_LOOP(
		if (p->id == curve) {
			ret = p;
			break;
		}
	);

	return ret;
}

/**
 * gnutls_ecc_curve_get_size:
 * @curve: is an ECC curve
 *
 * Returns: the size in bytes of the curve or 0 on failure.
 *
 * Since: 3.0
 **/
int gnutls_ecc_curve_get_size(gnutls_ecc_curve_t curve)
{
	int ret = 0;

	GNUTLS_ECC_CURVE_LOOP(
		if (p->id == curve) {
			ret = p->size;
			break;
		}
	);

	return ret;
}

/**
 * gnutls_ecc_curve_get_pk:
 * @curve: is an ECC curve
 *
 * Returns: the public key algorithm associated with the named curve or %GNUTLS_PK_UNKNOWN.
 *
 * Since: 3.5.0
 **/
gnutls_pk_algorithm_t gnutls_ecc_curve_get_pk(gnutls_ecc_curve_t curve)
{
	int ret = GNUTLS_PK_UNKNOWN;

	GNUTLS_ECC_CURVE_LOOP(
		if (p->id == curve) {
			ret = p->pk;
			break;
		}
	);

	return ret;
}

