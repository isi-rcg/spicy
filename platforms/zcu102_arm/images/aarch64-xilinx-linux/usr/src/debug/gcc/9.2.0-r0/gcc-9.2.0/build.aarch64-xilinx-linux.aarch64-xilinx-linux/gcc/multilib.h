static const char *const multilib_raw[] = {
". !mabi=lp64;",
"lp64 mabi=lp64;",
NULL
};

static const char *const multilib_reuse_raw[] = {
NULL
};

static const char *const multilib_matches_raw[] = {
"mabi=lp64 mabi=lp64;",
NULL
};

static const char *multilib_extra = "";

static const char *const multilib_exclusions_raw[] = {
NULL
};

static const char *multilib_options = "mabi=lp64";
