/*
 * Copyright (c) 2011-2014 - Mauro Carvalho Chehab
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation version 2.1 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 * Or, point your browser to http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 */
#ifndef _LIBSAT_H
#define _LIBSAT_H

#include "dvb-v5-std.h"

/**
 * @file dvb-sat.h
 * @ingroup satellite
 * @brief Provides interfaces to deal with DVB Satellite systems.
 * @copyright GNU Lesser General Public License version 2.1 (LGPLv2.1)
 * @author Mauro Carvalho Chehab
 *
 * @par Bug Report
 * Please submit bug reports and patches to linux-media@vger.kernel.org
 */

/*
 * Satellite handling functions
 */

/**
 * @struct dvb_sat_lnb
 * @brief Stores the information of a LNBf
 * @ingroup satellite
 *
 * @param name		long name of the LNBf type
 * @param alias		short name for the LNBf type
 *
 * The LNBf (low-noise block downconverter) is a type of amplifier that is
 * installed inside the parabolic dishes. It converts the antenna signal to
 * an Intermediate Frequency. Several Ku-band LNBf have more than one IF.
 * The lower IF is stored at lowfreq, the higher IF at highfreq.
 * The exact setup for those structs actually depend on the model of the LNBf,
 * and its usage.
 */
struct dvb_sat_lnb {
	const char *name;
	const char *alias;

	/*
	 * Legacy fields, kept just to avoid ABI breakages
	 * Should not be used by new applications
	 */
	unsigned lowfreq, highfreq;
	unsigned rangeswitch;
	struct dvbsat_freqrange {
		unsigned low, high;
	} freqrange[2];
};

struct dvb_v5_fe_parms;

#ifdef __cplusplus
extern "C" {
#endif

/* From libsat.c */

/**
 * @brief search for a LNBf entry
 * @ingroup satellite
 *
 * @param name	name of the LNBf entry to seek.
 *
 * On sucess, it returns a non-negative number with corresponds to the LNBf
 * entry inside the LNBf structure at dvb-sat.c.
 *
 * @return A -1 return code indicates that the LNBf was not found.
 */
int dvb_sat_search_lnb(const char *name);

/**
 * @brief prints the contents of a LNBf entry at STDOUT.
 * @ingroup satellite
 *
 * @param index		index for the entry
 *
 * @return returns -1 if the index is out of range, zero otherwise.
 */
int dvb_print_lnb(int index);

/**
 * @brief Prints all LNBf entries at STDOUT.
 * @ingroup satellite
 *
 * This function doesn't return anything. Internally, it calls dvb_print_lnb()
 * for all entries inside its LNBf database.
 */
void dvb_print_all_lnb(void);

/**
 * @brief gets a LNBf entry at its internal database
 * @ingroup satellite
 *
 * @param index		index for the entry.
 *
 * @return returns NULL if not found, of a struct dvb_sat_lnb pointer otherwise.
 *
 * NOTE: none of the strings are i18n translated. In order to get the
 * translated name, you should use dvb_sat_get_lnb_name()
 */
const struct dvb_sat_lnb *dvb_sat_get_lnb(int index);

/**
 * @brief gets a LNBf entry at its internal database
 * @ingroup satellite
 *
 * @param index		index for the entry.
 *
 * @return returns NULL if not found, of the name of a LNBf,
 * translated to the user language, if translation is available.
 */
const char *dvb_sat_get_lnb_name(int index);

/**
 * @brief sets the satellite parameters
 * @ingroup satellite
 *
 * @param parms	struct dvb_v5_fe_parms pointer.
 *
 * This function is called internally by the library to set the LNBf
 * parameters, if the dvb_v5_fe_parms::lnb field is filled.
 *
 * @return 0 on success.
 */
int dvb_sat_set_parms(struct dvb_v5_fe_parms *parms);

/**
 * @brief return the real satellite frequency
 * @ingroup satellite
 *
 * @param parms	struct dvb_v5_fe_parms pointer.
 *
 * This function is called internally by the library to get the satellite
 * frequency, considering the LO frequency.
 *
 * @return frequency.
 */
int dvb_sat_real_freq(struct dvb_v5_fe_parms *p, int freq);


#ifdef __cplusplus
}
#endif

#endif // _LIBSAT_H
