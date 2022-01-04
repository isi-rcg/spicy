#ifndef TR2_CFG_H
#define TR2_CFG_H

/*
 * Iterate over all config settings and emit 'def_param' events for the
 * "interesting" ones to TRACE2.
 */
void tr2_cfg_list_config_fl(const char *file, int line);

/*
 * Emit a "def_param" event for the given key/value pair IF we consider
 * the key to be "interesting".
 */
void tr2_cfg_set_fl(const char *file, int line, const char *key,
		    const char *value);

void tr2_cfg_free_patterns(void);

#endif /* TR2_CFG_H */
