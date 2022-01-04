#ifndef MC__FILEHIGHLIGHT_H
#define MC__FILEHIGHLIGHT_H

#include "lib/mcconfig.h"       /* mc_config_t */
#include "lib/util.h"           /* file_entry_t */

/*** typedefs(not structures) and defined constants **********************************************/

/*** enums ***************************************************************************************/

/*** structures declarations (and typedefs of structures)*****************************************/

typedef struct mc_fhl_struct
{
    mc_config_t *config;
    GPtrArray *filters;
} mc_fhl_t;

/*** global variables defined in .c file *********************************************************/

/*** declarations of public functions ************************************************************/

mc_fhl_t *mc_fhl_new (gboolean);
void mc_fhl_free (mc_fhl_t **);

int mc_fhl_get_color (mc_fhl_t *, file_entry_t *);

gboolean mc_fhl_read_ini_file (mc_fhl_t *, const gchar *);
gboolean mc_fhl_parse_ini_file (mc_fhl_t *);
void mc_fhl_clear (mc_fhl_t *);

/*** inline functions ****************************************************************************/

#endif /* MC__FILEHIGHLIGHT_H */
