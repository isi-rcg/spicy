static const struct cec_enum_values type_service_id_method[] = {
	{ "dig-id", CEC_OP_SERVICE_ID_METHOD_BY_DIG_ID },
	{ "channel", CEC_OP_SERVICE_ID_METHOD_BY_CHANNEL },
};

static const struct cec_enum_values type_dig_bcast_system[] = {
	{ "arib-gen", CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ARIB_GEN },
	{ "atsc-gen", CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ATSC_GEN },
	{ "dvb-gen", CEC_OP_DIG_SERVICE_BCAST_SYSTEM_DVB_GEN },
	{ "arib-bs", CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ARIB_BS },
	{ "arib-cs", CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ARIB_CS },
	{ "arib-t", CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ARIB_T },
	{ "atsc-cable", CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ATSC_CABLE },
	{ "atsc-sat", CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ATSC_SAT },
	{ "atsc-t", CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ATSC_T },
	{ "dvb-c", CEC_OP_DIG_SERVICE_BCAST_SYSTEM_DVB_C },
	{ "dvb-s", CEC_OP_DIG_SERVICE_BCAST_SYSTEM_DVB_S },
	{ "dvb-s2", CEC_OP_DIG_SERVICE_BCAST_SYSTEM_DVB_S2 },
	{ "dvb-t", CEC_OP_DIG_SERVICE_BCAST_SYSTEM_DVB_T },
};

static const struct cec_enum_values type_channel_number_fmt[] = {
	{ "1-part", CEC_OP_CHANNEL_NUMBER_FMT_1_PART },
	{ "2-part", CEC_OP_CHANNEL_NUMBER_FMT_2_PART },
};

static const struct cec_enum_values type_ana_bcast_type[] = {
	{ "cable", CEC_OP_ANA_BCAST_TYPE_CABLE },
	{ "satellite", CEC_OP_ANA_BCAST_TYPE_SATELLITE },
	{ "terrestrial", CEC_OP_ANA_BCAST_TYPE_TERRESTRIAL },
};

static const struct cec_enum_values type_bcast_system[] = {
	{ "pal-bg", CEC_OP_BCAST_SYSTEM_PAL_BG },
	{ "secam-lq", CEC_OP_BCAST_SYSTEM_SECAM_LQ },
	{ "pal-m", CEC_OP_BCAST_SYSTEM_PAL_M },
	{ "ntsc-m", CEC_OP_BCAST_SYSTEM_NTSC_M },
	{ "pal-i", CEC_OP_BCAST_SYSTEM_PAL_I },
	{ "secam-dk", CEC_OP_BCAST_SYSTEM_SECAM_DK },
	{ "secam-bg", CEC_OP_BCAST_SYSTEM_SECAM_BG },
	{ "secam-l", CEC_OP_BCAST_SYSTEM_SECAM_L },
	{ "pal-dk", CEC_OP_BCAST_SYSTEM_PAL_DK },
	{ "other", CEC_OP_BCAST_SYSTEM_OTHER },
};

static const struct cec_enum_values type_rec_status[] = {
	{ "cur-src", CEC_OP_RECORD_STATUS_CUR_SRC },
	{ "dig-service", CEC_OP_RECORD_STATUS_DIG_SERVICE },
	{ "ana-service", CEC_OP_RECORD_STATUS_ANA_SERVICE },
	{ "ext-input", CEC_OP_RECORD_STATUS_EXT_INPUT },
	{ "no-dig-service", CEC_OP_RECORD_STATUS_NO_DIG_SERVICE },
	{ "no-ana-service", CEC_OP_RECORD_STATUS_NO_ANA_SERVICE },
	{ "no-service", CEC_OP_RECORD_STATUS_NO_SERVICE },
	{ "invalid-ext-plug", CEC_OP_RECORD_STATUS_INVALID_EXT_PLUG },
	{ "invalid-ext-phys-addr", CEC_OP_RECORD_STATUS_INVALID_EXT_PHYS_ADDR },
	{ "unsup-ca", CEC_OP_RECORD_STATUS_UNSUP_CA },
	{ "no-ca-entitlements", CEC_OP_RECORD_STATUS_NO_CA_ENTITLEMENTS },
	{ "cant-copy-src", CEC_OP_RECORD_STATUS_CANT_COPY_SRC },
	{ "no-more-copies", CEC_OP_RECORD_STATUS_NO_MORE_COPIES },
	{ "no-media", CEC_OP_RECORD_STATUS_NO_MEDIA },
	{ "playing", CEC_OP_RECORD_STATUS_PLAYING },
	{ "already-recording", CEC_OP_RECORD_STATUS_ALREADY_RECORDING },
	{ "media-prot", CEC_OP_RECORD_STATUS_MEDIA_PROT },
	{ "no-signal", CEC_OP_RECORD_STATUS_NO_SIGNAL },
	{ "media-problem", CEC_OP_RECORD_STATUS_MEDIA_PROBLEM },
	{ "no-space", CEC_OP_RECORD_STATUS_NO_SPACE },
	{ "parental-lock", CEC_OP_RECORD_STATUS_PARENTAL_LOCK },
	{ "terminated-ok", CEC_OP_RECORD_STATUS_TERMINATED_OK },
	{ "already-term", CEC_OP_RECORD_STATUS_ALREADY_TERM },
	{ "other", CEC_OP_RECORD_STATUS_OTHER },
};

static const struct cec_enum_values type_timer_overlap_warning[] = {
	{ "no-overlap", CEC_OP_TIMER_OVERLAP_WARNING_NO_OVERLAP },
	{ "overlap", CEC_OP_TIMER_OVERLAP_WARNING_OVERLAP },
};

static const struct cec_enum_values type_media_info[] = {
	{ "unprot-media", CEC_OP_MEDIA_INFO_UNPROT_MEDIA },
	{ "prot-media", CEC_OP_MEDIA_INFO_PROT_MEDIA },
	{ "no-media", CEC_OP_MEDIA_INFO_NO_MEDIA },
};

static const struct cec_enum_values type_prog_info[] = {
	{ "enough-space", CEC_OP_PROG_INFO_ENOUGH_SPACE },
	{ "not-enough-space", CEC_OP_PROG_INFO_NOT_ENOUGH_SPACE },
	{ "might-not-be-enough-space", CEC_OP_PROG_INFO_MIGHT_NOT_BE_ENOUGH_SPACE },
	{ "none-available", CEC_OP_PROG_INFO_NONE_AVAILABLE },
};

static const struct cec_enum_values type_prog_error[] = {
	{ "no-free-timer", CEC_OP_PROG_ERROR_NO_FREE_TIMER },
	{ "date-out-of-range", CEC_OP_PROG_ERROR_DATE_OUT_OF_RANGE },
	{ "rec-seq-error", CEC_OP_PROG_ERROR_REC_SEQ_ERROR },
	{ "inv-ext-plug", CEC_OP_PROG_ERROR_INV_EXT_PLUG },
	{ "inv-ext-phys-addr", CEC_OP_PROG_ERROR_INV_EXT_PHYS_ADDR },
	{ "ca-unsupp", CEC_OP_PROG_ERROR_CA_UNSUPP },
	{ "insuf-ca-entitlements", CEC_OP_PROG_ERROR_INSUF_CA_ENTITLEMENTS },
	{ "resolution-unsupp", CEC_OP_PROG_ERROR_RESOLUTION_UNSUPP },
	{ "parental-lock", CEC_OP_PROG_ERROR_PARENTAL_LOCK },
	{ "clock-failure", CEC_OP_PROG_ERROR_CLOCK_FAILURE },
	{ "duplicate", CEC_OP_PROG_ERROR_DUPLICATE },
};

static const struct cec_enum_values type_timer_cleared_status[] = {
	{ "recording", CEC_OP_TIMER_CLR_STAT_RECORDING },
	{ "no-matching", CEC_OP_TIMER_CLR_STAT_NO_MATCHING },
	{ "no-info", CEC_OP_TIMER_CLR_STAT_NO_INFO },
	{ "cleared", CEC_OP_TIMER_CLR_STAT_CLEARED },
};

static const struct cec_enum_values type_recording_seq[] = {
	{ "sunday", CEC_OP_REC_SEQ_SUNDAY },
	{ "monday", CEC_OP_REC_SEQ_MONDAY },
	{ "tuesday", CEC_OP_REC_SEQ_TUESDAY },
	{ "wednesday", CEC_OP_REC_SEQ_WEDNESDAY },
	{ "thursday", CEC_OP_REC_SEQ_THURSDAY },
	{ "friday", CEC_OP_REC_SEQ_FRIDAY },
	{ "saterday", CEC_OP_REC_SEQ_SATERDAY },
	{ "once-only", CEC_OP_REC_SEQ_ONCE_ONLY },
};

static const struct cec_enum_values type_ext_src_spec[] = {
	{ "plug", CEC_OP_EXT_SRC_PLUG },
	{ "phys-addr", CEC_OP_EXT_SRC_PHYS_ADDR },
};

static const struct cec_enum_values type_cec_version[] = {
	{ "version-1-3a", CEC_OP_CEC_VERSION_1_3A },
	{ "version-1-4", CEC_OP_CEC_VERSION_1_4 },
	{ "version-2-0", CEC_OP_CEC_VERSION_2_0 },
};

static const struct cec_enum_values type_prim_devtype[] = {
	{ "tv", CEC_OP_PRIM_DEVTYPE_TV },
	{ "record", CEC_OP_PRIM_DEVTYPE_RECORD },
	{ "tuner", CEC_OP_PRIM_DEVTYPE_TUNER },
	{ "playback", CEC_OP_PRIM_DEVTYPE_PLAYBACK },
	{ "audiosystem", CEC_OP_PRIM_DEVTYPE_AUDIOSYSTEM },
	{ "switch", CEC_OP_PRIM_DEVTYPE_SWITCH },
	{ "processor", CEC_OP_PRIM_DEVTYPE_PROCESSOR },
};

static const struct cec_enum_values type_all_device_types[] = {
	{ "tv", CEC_OP_ALL_DEVTYPE_TV },
	{ "record", CEC_OP_ALL_DEVTYPE_RECORD },
	{ "tuner", CEC_OP_ALL_DEVTYPE_TUNER },
	{ "playback", CEC_OP_ALL_DEVTYPE_PLAYBACK },
	{ "audiosystem", CEC_OP_ALL_DEVTYPE_AUDIOSYSTEM },
	{ "switch", CEC_OP_ALL_DEVTYPE_SWITCH },
};

static const struct cec_enum_values type_rc_profile[] = {
	{ "tv-profile-none", CEC_OP_FEAT_RC_TV_PROFILE_NONE },
	{ "tv-profile-1", CEC_OP_FEAT_RC_TV_PROFILE_1 },
	{ "tv-profile-2", CEC_OP_FEAT_RC_TV_PROFILE_2 },
	{ "tv-profile-3", CEC_OP_FEAT_RC_TV_PROFILE_3 },
	{ "tv-profile-4", CEC_OP_FEAT_RC_TV_PROFILE_4 },
	{ "src-has-dev-root-menu", CEC_OP_FEAT_RC_SRC_HAS_DEV_ROOT_MENU },
	{ "src-has-dev-setup-menu", CEC_OP_FEAT_RC_SRC_HAS_DEV_SETUP_MENU },
	{ "src-has-contents-menu", CEC_OP_FEAT_RC_SRC_HAS_CONTENTS_MENU },
	{ "src-has-media-top-menu", CEC_OP_FEAT_RC_SRC_HAS_MEDIA_TOP_MENU },
	{ "src-has-media-context-menu", CEC_OP_FEAT_RC_SRC_HAS_MEDIA_CONTEXT_MENU },
};

static const struct cec_enum_values type_dev_features[] = {
	{ "has-record-tv-screen", CEC_OP_FEAT_DEV_HAS_RECORD_TV_SCREEN },
	{ "has-set-osd-string", CEC_OP_FEAT_DEV_HAS_SET_OSD_STRING },
	{ "has-deck-control", CEC_OP_FEAT_DEV_HAS_DECK_CONTROL },
	{ "has-set-audio-rate", CEC_OP_FEAT_DEV_HAS_SET_AUDIO_RATE },
	{ "sink-has-arc-tx", CEC_OP_FEAT_DEV_SINK_HAS_ARC_TX },
	{ "source-has-arc-rx", CEC_OP_FEAT_DEV_SOURCE_HAS_ARC_RX },
};

static const struct cec_enum_values type_deck_control_mode[] = {
	{ "skip-fwd", CEC_OP_DECK_CTL_MODE_SKIP_FWD },
	{ "skip-rev", CEC_OP_DECK_CTL_MODE_SKIP_REV },
	{ "stop", CEC_OP_DECK_CTL_MODE_STOP },
	{ "eject", CEC_OP_DECK_CTL_MODE_EJECT },
};

static const struct cec_enum_values type_deck_info[] = {
	{ "play", CEC_OP_DECK_INFO_PLAY },
	{ "record", CEC_OP_DECK_INFO_RECORD },
	{ "play-rev", CEC_OP_DECK_INFO_PLAY_REV },
	{ "still", CEC_OP_DECK_INFO_STILL },
	{ "slow", CEC_OP_DECK_INFO_SLOW },
	{ "slow-rev", CEC_OP_DECK_INFO_SLOW_REV },
	{ "fast-fwd", CEC_OP_DECK_INFO_FAST_FWD },
	{ "fast-rev", CEC_OP_DECK_INFO_FAST_REV },
	{ "no-media", CEC_OP_DECK_INFO_NO_MEDIA },
	{ "stop", CEC_OP_DECK_INFO_STOP },
	{ "skip-fwd", CEC_OP_DECK_INFO_SKIP_FWD },
	{ "skip-rev", CEC_OP_DECK_INFO_SKIP_REV },
	{ "index-search-fwd", CEC_OP_DECK_INFO_INDEX_SEARCH_FWD },
	{ "index-search-rev", CEC_OP_DECK_INFO_INDEX_SEARCH_REV },
	{ "other", CEC_OP_DECK_INFO_OTHER },
};

static const struct cec_enum_values type_status_req[] = {
	{ "on", CEC_OP_STATUS_REQ_ON },
	{ "off", CEC_OP_STATUS_REQ_OFF },
	{ "once", CEC_OP_STATUS_REQ_ONCE },
};

static const struct cec_enum_values type_play_mode[] = {
	{ "fwd", CEC_OP_PLAY_MODE_PLAY_FWD },
	{ "rev", CEC_OP_PLAY_MODE_PLAY_REV },
	{ "still", CEC_OP_PLAY_MODE_PLAY_STILL },
	{ "fast-fwd-min", CEC_OP_PLAY_MODE_PLAY_FAST_FWD_MIN },
	{ "fast-fwd-med", CEC_OP_PLAY_MODE_PLAY_FAST_FWD_MED },
	{ "fast-fwd-max", CEC_OP_PLAY_MODE_PLAY_FAST_FWD_MAX },
	{ "fast-rev-min", CEC_OP_PLAY_MODE_PLAY_FAST_REV_MIN },
	{ "fast-rev-med", CEC_OP_PLAY_MODE_PLAY_FAST_REV_MED },
	{ "fast-rev-max", CEC_OP_PLAY_MODE_PLAY_FAST_REV_MAX },
	{ "slow-fwd-min", CEC_OP_PLAY_MODE_PLAY_SLOW_FWD_MIN },
	{ "slow-fwd-med", CEC_OP_PLAY_MODE_PLAY_SLOW_FWD_MED },
	{ "slow-fwd-max", CEC_OP_PLAY_MODE_PLAY_SLOW_FWD_MAX },
	{ "slow-rev-min", CEC_OP_PLAY_MODE_PLAY_SLOW_REV_MIN },
	{ "slow-rev-med", CEC_OP_PLAY_MODE_PLAY_SLOW_REV_MED },
	{ "slow-rev-max", CEC_OP_PLAY_MODE_PLAY_SLOW_REV_MAX },
};

static const struct cec_enum_values type_rec_flag[] = {
	{ "used", CEC_OP_REC_FLAG_USED },
	{ "not-used", CEC_OP_REC_FLAG_NOT_USED },
};

static const struct cec_enum_values type_tuner_display_info[] = {
	{ "digital", CEC_OP_TUNER_DISPLAY_INFO_DIGITAL },
	{ "none", CEC_OP_TUNER_DISPLAY_INFO_NONE },
	{ "analogue", CEC_OP_TUNER_DISPLAY_INFO_ANALOGUE },
};

static const struct cec_enum_values type_disp_ctl[] = {
	{ "default", CEC_OP_DISP_CTL_DEFAULT },
	{ "until-cleared", CEC_OP_DISP_CTL_UNTIL_CLEARED },
	{ "clear", CEC_OP_DISP_CTL_CLEAR },
};

static const struct cec_enum_values type_menu_state[] = {
	{ "activated", CEC_OP_MENU_STATE_ACTIVATED },
	{ "deactivated", CEC_OP_MENU_STATE_DEACTIVATED },
};

static const struct cec_enum_values type_menu_req[] = {
	{ "activate", CEC_OP_MENU_REQUEST_ACTIVATE },
	{ "deactivate", CEC_OP_MENU_REQUEST_DEACTIVATE },
	{ "query", CEC_OP_MENU_REQUEST_QUERY },
};

static const struct cec_enum_values type_ui_bcast_type[] = {
	{ "toggle-all", CEC_OP_UI_BCAST_TYPE_TOGGLE_ALL },
	{ "toggle-dig-ana", CEC_OP_UI_BCAST_TYPE_TOGGLE_DIG_ANA },
	{ "analogue", CEC_OP_UI_BCAST_TYPE_ANALOGUE },
	{ "analogue-t", CEC_OP_UI_BCAST_TYPE_ANALOGUE_T },
	{ "analogue-cable", CEC_OP_UI_BCAST_TYPE_ANALOGUE_CABLE },
	{ "analogue-sat", CEC_OP_UI_BCAST_TYPE_ANALOGUE_SAT },
	{ "digital", CEC_OP_UI_BCAST_TYPE_DIGITAL },
	{ "digital-t", CEC_OP_UI_BCAST_TYPE_DIGITAL_T },
	{ "digital-cable", CEC_OP_UI_BCAST_TYPE_DIGITAL_CABLE },
	{ "digital-sat", CEC_OP_UI_BCAST_TYPE_DIGITAL_SAT },
	{ "digital-com-sat", CEC_OP_UI_BCAST_TYPE_DIGITAL_COM_SAT },
	{ "digital-com-sat2", CEC_OP_UI_BCAST_TYPE_DIGITAL_COM_SAT2 },
	{ "ip", CEC_OP_UI_BCAST_TYPE_IP },
};

static const struct cec_enum_values type_ui_snd_pres_ctl[] = {
	{ "dual-mono", CEC_OP_UI_SND_PRES_CTL_DUAL_MONO },
	{ "karaoke", CEC_OP_UI_SND_PRES_CTL_KARAOKE },
	{ "downmix", CEC_OP_UI_SND_PRES_CTL_DOWNMIX },
	{ "reverb", CEC_OP_UI_SND_PRES_CTL_REVERB },
	{ "equalizer", CEC_OP_UI_SND_PRES_CTL_EQUALIZER },
	{ "bass-up", CEC_OP_UI_SND_PRES_CTL_BASS_UP },
	{ "bass-neutral", CEC_OP_UI_SND_PRES_CTL_BASS_NEUTRAL },
	{ "bass-down", CEC_OP_UI_SND_PRES_CTL_BASS_DOWN },
	{ "treble-up", CEC_OP_UI_SND_PRES_CTL_TREBLE_UP },
	{ "treble-neutral", CEC_OP_UI_SND_PRES_CTL_TREBLE_NEUTRAL },
	{ "treble-down", CEC_OP_UI_SND_PRES_CTL_TREBLE_DOWN },
};

static const struct cec_enum_values type_pwr_state[] = {
	{ "on", CEC_OP_POWER_STATUS_ON },
	{ "standby", CEC_OP_POWER_STATUS_STANDBY },
	{ "to-on", CEC_OP_POWER_STATUS_TO_ON },
	{ "to-standby", CEC_OP_POWER_STATUS_TO_STANDBY },
};

static const struct cec_enum_values type_reason[] = {
	{ "unrecognized-op", CEC_OP_ABORT_UNRECOGNIZED_OP },
	{ "incorrect-mode", CEC_OP_ABORT_INCORRECT_MODE },
	{ "no-source", CEC_OP_ABORT_NO_SOURCE },
	{ "invalid-op", CEC_OP_ABORT_INVALID_OP },
	{ "refused", CEC_OP_ABORT_REFUSED },
	{ "undetermined", CEC_OP_ABORT_UNDETERMINED },
};

static const struct cec_enum_values type_aud_mute_status[] = {
	{ "off", CEC_OP_AUD_MUTE_STATUS_OFF },
	{ "on", CEC_OP_AUD_MUTE_STATUS_ON },
};

static const struct cec_enum_values type_sys_aud_status[] = {
	{ "off", CEC_OP_SYS_AUD_STATUS_OFF },
	{ "on", CEC_OP_SYS_AUD_STATUS_ON },
};

static const struct cec_enum_values type_audio_rate[] = {
	{ "off", CEC_OP_AUD_RATE_OFF },
	{ "wide-std", CEC_OP_AUD_RATE_WIDE_STD },
	{ "wide-fast", CEC_OP_AUD_RATE_WIDE_FAST },
	{ "wide-slow", CEC_OP_AUD_RATE_WIDE_SLOW },
	{ "narrow-std", CEC_OP_AUD_RATE_NARROW_STD },
	{ "narrow-fast", CEC_OP_AUD_RATE_NARROW_FAST },
	{ "narrow-slow", CEC_OP_AUD_RATE_NARROW_SLOW },
};

static const struct cec_enum_values type_low_latency_mode[] = {
	{ "off", CEC_OP_LOW_LATENCY_MODE_OFF },
	{ "on", CEC_OP_LOW_LATENCY_MODE_ON },
};

static const struct cec_enum_values type_audio_out_compensated[] = {
	{ "na", CEC_OP_AUD_OUT_COMPENSATED_NA },
	{ "delay", CEC_OP_AUD_OUT_COMPENSATED_DELAY },
	{ "no-delay", CEC_OP_AUD_OUT_COMPENSATED_NO_DELAY },
	{ "partial-delay", CEC_OP_AUD_OUT_COMPENSATED_PARTIAL_DELAY },
};

static const struct cec_enum_values type_hec_func_state[] = {
	{ "not-supported", CEC_OP_HEC_FUNC_STATE_NOT_SUPPORTED },
	{ "inactive", CEC_OP_HEC_FUNC_STATE_INACTIVE },
	{ "active", CEC_OP_HEC_FUNC_STATE_ACTIVE },
	{ "activation-field", CEC_OP_HEC_FUNC_STATE_ACTIVATION_FIELD },
};

static const struct cec_enum_values type_host_func_state[] = {
	{ "not-supported", CEC_OP_HOST_FUNC_STATE_NOT_SUPPORTED },
	{ "inactive", CEC_OP_HOST_FUNC_STATE_INACTIVE },
	{ "active", CEC_OP_HOST_FUNC_STATE_ACTIVE },
};

static const struct cec_enum_values type_enc_func_state[] = {
	{ "not-supported", CEC_OP_ENC_FUNC_STATE_EXT_CON_NOT_SUPPORTED },
	{ "inactive", CEC_OP_ENC_FUNC_STATE_EXT_CON_INACTIVE },
	{ "active", CEC_OP_ENC_FUNC_STATE_EXT_CON_ACTIVE },
};

static const struct cec_enum_values type_cdc_errcode[] = {
	{ "none", CEC_OP_CDC_ERROR_CODE_NONE },
	{ "cap-unsupported", CEC_OP_CDC_ERROR_CODE_CAP_UNSUPPORTED },
	{ "wrong-state", CEC_OP_CDC_ERROR_CODE_WRONG_STATE },
	{ "other", CEC_OP_CDC_ERROR_CODE_OTHER },
};

static const struct cec_enum_values type_hec_set_state[] = {
	{ "deactivate", CEC_OP_HEC_SET_STATE_DEACTIVATE },
	{ "activate", CEC_OP_HEC_SET_STATE_ACTIVATE },
};

static const struct cec_enum_values type_hpd_state[] = {
	{ "cp-edid-disable", CEC_OP_HPD_STATE_CP_EDID_DISABLE },
	{ "cp-edid-enable", CEC_OP_HPD_STATE_CP_EDID_ENABLE },
	{ "cp-edid-disable-enable", CEC_OP_HPD_STATE_CP_EDID_DISABLE_ENABLE },
	{ "edid-disable", CEC_OP_HPD_STATE_EDID_DISABLE },
	{ "edid-enable", CEC_OP_HPD_STATE_EDID_ENABLE },
	{ "edid-disable-enable", CEC_OP_HPD_STATE_EDID_DISABLE_ENABLE },
};

static const struct cec_enum_values type_hpd_error[] = {
	{ "none", CEC_OP_HPD_ERROR_NONE },
	{ "initiator-not-capable", CEC_OP_HPD_ERROR_INITIATOR_NOT_CAPABLE },
	{ "initiator-wrong-state", CEC_OP_HPD_ERROR_INITIATOR_WRONG_STATE },
	{ "other", CEC_OP_HPD_ERROR_OTHER },
	{ "none-no-video", CEC_OP_HPD_ERROR_NONE_NO_VIDEO },
};

static const struct cec_enum_values type_htng_tuner_type[] = {
	{ "air", CEC_OP_HTNG_TUNER_TYPE_AIR },
	{ "cable", CEC_OP_HTNG_TUNER_TYPE_CABLE },
	{ "sat", CEC_OP_HTNG_TUNER_TYPE_SAT },
	{ "not-specified", CEC_OP_HTNG_TUNER_TYPE_NOT_SPECIFIED },
};

static const struct cec_enum_values type_htng_input_src[] = {
	{ "tuner-1part", CEC_OP_HTNG_INPUT_SRC_TUNER_1PART },
	{ "tuner-2part", CEC_OP_HTNG_INPUT_SRC_TUNER_2PART },
	{ "av", CEC_OP_HTNG_INPUT_SRC_AV },
	{ "pc", CEC_OP_HTNG_INPUT_SRC_PC },
	{ "hdmi", CEC_OP_HTNG_INPUT_SRC_HDMI },
	{ "component", CEC_OP_HTNG_INPUT_SRC_COMPONENT },
	{ "dvi", CEC_OP_HTNG_INPUT_SRC_DVI },
	{ "dp", CEC_OP_HTNG_INPUT_SRC_DP },
	{ "usb", CEC_OP_HTNG_INPUT_SRC_USB },
};

static const struct cec_enum_values type_htng_led_control[] = {
	{ "default", CEC_OP_HTNG_LED_CONTROL_DEFAULT },
	{ "on", CEC_OP_HTNG_LED_CONTROL_ON },
	{ "off", CEC_OP_HTNG_LED_CONTROL_OFF },
};

static const struct cec_enum_values type_htng_chan_type[] = {
	{ "auto", CEC_OP_HTNG_CHAN_TYPE_AUTO },
	{ "ana-ant", CEC_OP_HTNG_CHAN_TYPE_ANA_ANT },
	{ "ana-cable", CEC_OP_HTNG_CHAN_TYPE_ANA_CABLE },
	{ "dig-ant", CEC_OP_HTNG_CHAN_TYPE_DIG_ANT },
	{ "dig-cable", CEC_OP_HTNG_CHAN_TYPE_DIG_CABLE },
	{ "satellite", CEC_OP_HTNG_CHAN_TYPE_SATELLITE },
};

static const struct cec_enum_values type_htng_prog_type[] = {
	{ "av", CEC_OP_HTNG_PROG_TYPE_AV },
	{ "radio", CEC_OP_HTNG_PROG_TYPE_RADIO },
};

static const struct cec_enum_values type_htng_system_type[] = {
	{ "pal-bg", CEC_OP_HTNG_SYSTEM_TYPE_PAL_BG },
	{ "pal-i", CEC_OP_HTNG_SYSTEM_TYPE_PAL_I },
	{ "pal-dk", CEC_OP_HTNG_SYSTEM_TYPE_PAL_DK },
	{ "pal-m", CEC_OP_HTNG_SYSTEM_TYPE_PAL_M },
	{ "pal-n", CEC_OP_HTNG_SYSTEM_TYPE_PAL_N },
	{ "secam-bg", CEC_OP_HTNG_SYSTEM_TYPE_SECAM_BG },
	{ "secam-dk", CEC_OP_HTNG_SYSTEM_TYPE_SECAM_DK },
	{ "secam-l", CEC_OP_HTNG_SYSTEM_TYPE_SECAM_L },
	{ "ntsc-m", CEC_OP_HTNG_SYSTEM_TYPE_NTSC_M },
};

static const struct cec_enum_values type_htng_mod_type[] = {
	{ "auto", CEC_OP_HTNG_MOD_TYPE_AUTO },
	{ "qpsk", CEC_OP_HTNG_MOD_TYPE_QPSK },
	{ "qcam16", CEC_OP_HTNG_MOD_TYPE_QCAM16 },
	{ "qcam32", CEC_OP_HTNG_MOD_TYPE_QCAM32 },
	{ "qcam64", CEC_OP_HTNG_MOD_TYPE_QCAM64 },
	{ "qcam128", CEC_OP_HTNG_MOD_TYPE_QCAM128 },
	{ "qcam256", CEC_OP_HTNG_MOD_TYPE_QCAM256 },
	{ "dqpsk", CEC_OP_HTNG_MOD_TYPE_DQPSK },
};

static const struct cec_enum_values type_htng_symbol_rate[] = {
	{ "auto", CEC_OP_HTNG_SYMBOL_RATE_AUTO },
	{ "manual", CEC_OP_HTNG_SYMBOL_RATE_MANUAL },
};

static const struct cec_enum_values type_htng_ext_chan_type[] = {
	{ "auto", CEC_OP_HTNG_EXT_CHAN_TYPE_AUTO },
	{ "ana-ant", CEC_OP_HTNG_EXT_CHAN_TYPE_ANA_ANT },
	{ "ana-cable", CEC_OP_HTNG_EXT_CHAN_TYPE_ANA_CABLE },
	{ "dvb-t-isdb-t-dtmb", CEC_OP_HTNG_EXT_CHAN_TYPE_DVB_T_ISDB_T_DTMB },
	{ "dvb-c", CEC_OP_HTNG_EXT_CHAN_TYPE_DVB_C },
	{ "dvb-t2", CEC_OP_HTNG_EXT_CHAN_TYPE_DVB_T2 },
};

static const struct cec_enum_values type_htng_onid[] = {
	{ "auto", CEC_OP_HTNG_ONID_AUTO },
	{ "manual", CEC_OP_HTNG_ONID_MANUAL },
};

static const struct cec_enum_values type_htng_nid[] = {
	{ "auto", CEC_OP_HTNG_NID_AUTO },
	{ "manual", CEC_OP_HTNG_NID_MANUAL },
};

static const struct cec_enum_values type_htng_tsid_plp[] = {
	{ "auto", CEC_OP_HTNG_TSID_PLP_AUTO },
	{ "manual", CEC_OP_HTNG_TSID_PLP_MANUAL },
};

#define arg_phys_addr arg_u16
#define arg_orig_phys_addr arg_u16
#define arg_new_phys_addr arg_u16
static const struct arg arg_service_id_method = {
	CEC_TYPE_ENUM, 2, type_service_id_method
};

static const struct arg arg_dig_bcast_system = {
	CEC_TYPE_ENUM, 13, type_dig_bcast_system
};

#define arg_transport_id arg_u16
#define arg_service_id arg_u16
#define arg_orig_network_id arg_u16
#define arg_program_number arg_u16
static const struct arg arg_channel_number_fmt = {
	CEC_TYPE_ENUM, 2, type_channel_number_fmt
};

#define arg_major arg_u16
#define arg_minor arg_u16
static const struct arg arg_ana_bcast_type = {
	CEC_TYPE_ENUM, 3, type_ana_bcast_type
};

#define arg_ana_freq arg_u16
static const struct arg arg_bcast_system = {
	CEC_TYPE_ENUM, 10, type_bcast_system
};

#define arg_plug arg_u8
static const struct arg arg_rec_status = {
	CEC_TYPE_ENUM, 24, type_rec_status
};

static const struct arg arg_timer_overlap_warning = {
	CEC_TYPE_ENUM, 2, type_timer_overlap_warning
};

static const struct arg arg_media_info = {
	CEC_TYPE_ENUM, 3, type_media_info
};

static const struct arg arg_prog_info = {
	CEC_TYPE_ENUM, 4, type_prog_info
};

static const struct arg arg_prog_error = {
	CEC_TYPE_ENUM, 11, type_prog_error
};

#define arg_duration_hr arg_u8
#define arg_duration_min arg_u8
static const struct arg arg_timer_cleared_status = {
	CEC_TYPE_ENUM, 4, type_timer_cleared_status
};

#define arg_day arg_u8
#define arg_month arg_u8
#define arg_start_hr arg_u8
#define arg_start_min arg_u8
static const struct arg arg_recording_seq = {
	CEC_TYPE_ENUM, 8, type_recording_seq
};

static const struct arg arg_ext_src_spec = {
	CEC_TYPE_ENUM, 2, type_ext_src_spec
};

#define arg_prog_title arg_string
static const struct arg arg_cec_version = {
	CEC_TYPE_ENUM, 3, type_cec_version
};

static const struct arg arg_prim_devtype = {
	CEC_TYPE_ENUM, 7, type_prim_devtype
};

#define arg_language arg_string
static const struct arg arg_all_device_types = {
	CEC_TYPE_ENUM, 6, type_all_device_types
};

static const struct arg arg_rc_profile = {
	CEC_TYPE_ENUM, 10, type_rc_profile
};

static const struct arg arg_dev_features = {
	CEC_TYPE_ENUM, 6, type_dev_features
};

static const struct arg arg_deck_control_mode = {
	CEC_TYPE_ENUM, 4, type_deck_control_mode
};

static const struct arg arg_deck_info = {
	CEC_TYPE_ENUM, 15, type_deck_info
};

static const struct arg arg_status_req = {
	CEC_TYPE_ENUM, 3, type_status_req
};

static const struct arg arg_play_mode = {
	CEC_TYPE_ENUM, 15, type_play_mode
};

static const struct arg arg_rec_flag = {
	CEC_TYPE_ENUM, 2, type_rec_flag
};

static const struct arg arg_tuner_display_info = {
	CEC_TYPE_ENUM, 3, type_tuner_display_info
};

#define arg_vendor_id arg_u32
static const struct arg arg_disp_ctl = {
	CEC_TYPE_ENUM, 3, type_disp_ctl
};

#define arg_osd arg_string
#define arg_name arg_string
static const struct arg arg_menu_state = {
	CEC_TYPE_ENUM, 2, type_menu_state
};

static const struct arg arg_menu_req = {
	CEC_TYPE_ENUM, 3, type_menu_req
};

#define arg_ui_cmd arg_u8
#define arg_has_opt_arg arg_u8
#define arg_ui_function_media arg_u8
#define arg_ui_function_select_av_input arg_u8
#define arg_ui_function_select_audio_input arg_u8
static const struct arg arg_ui_bcast_type = {
	CEC_TYPE_ENUM, 13, type_ui_bcast_type
};

static const struct arg arg_ui_snd_pres_ctl = {
	CEC_TYPE_ENUM, 11, type_ui_snd_pres_ctl
};

static const struct arg arg_pwr_state = {
	CEC_TYPE_ENUM, 4, type_pwr_state
};

#define arg_abort_msg arg_u8
static const struct arg arg_reason = {
	CEC_TYPE_ENUM, 6, type_reason
};

static const struct arg arg_aud_mute_status = {
	CEC_TYPE_ENUM, 2, type_aud_mute_status
};

#define arg_aud_vol_status arg_u8
static const struct arg arg_sys_aud_status = {
	CEC_TYPE_ENUM, 2, type_sys_aud_status
};

#define arg_num_descriptors arg_u8
#define arg_descriptor1 arg_u8
#define arg_descriptor2 arg_u8
#define arg_descriptor3 arg_u8
#define arg_descriptor4 arg_u8
#define arg_audio_format_id1 arg_u8
#define arg_audio_format_code1 arg_u8
#define arg_audio_format_id2 arg_u8
#define arg_audio_format_code2 arg_u8
#define arg_audio_format_id3 arg_u8
#define arg_audio_format_code3 arg_u8
#define arg_audio_format_id4 arg_u8
#define arg_audio_format_code4 arg_u8
static const struct arg arg_audio_rate = {
	CEC_TYPE_ENUM, 7, type_audio_rate
};

#define arg_video_latency arg_u8
static const struct arg arg_low_latency_mode = {
	CEC_TYPE_ENUM, 2, type_low_latency_mode
};

static const struct arg arg_audio_out_compensated = {
	CEC_TYPE_ENUM, 4, type_audio_out_compensated
};

#define arg_audio_out_delay arg_u8
#define arg_phys_addr1 arg_u16
#define arg_phys_addr2 arg_u16
#define arg_target_phys_addr arg_u16
static const struct arg arg_hec_func_state = {
	CEC_TYPE_ENUM, 4, type_hec_func_state
};

static const struct arg arg_host_func_state = {
	CEC_TYPE_ENUM, 3, type_host_func_state
};

static const struct arg arg_enc_func_state = {
	CEC_TYPE_ENUM, 3, type_enc_func_state
};

static const struct arg arg_cdc_errcode = {
	CEC_TYPE_ENUM, 4, type_cdc_errcode
};

#define arg_has_field arg_u8
#define arg_hec_field arg_u16
static const struct arg arg_hec_set_state = {
	CEC_TYPE_ENUM, 2, type_hec_set_state
};

#define arg_phys_addr3 arg_u16
#define arg_phys_addr4 arg_u16
#define arg_phys_addr5 arg_u16
#define arg_input_port arg_u8
static const struct arg arg_hpd_state = {
	CEC_TYPE_ENUM, 6, type_hpd_state
};

static const struct arg arg_hpd_error = {
	CEC_TYPE_ENUM, 5, type_hpd_error
};

static const struct arg arg_htng_tuner_type = {
	CEC_TYPE_ENUM, 4, type_htng_tuner_type
};

#define arg_chan arg_u16
#define arg_major_chan arg_u8
#define arg_minor_chan arg_u16
#define arg_input arg_u16
static const struct arg arg_htng_input_src = {
	CEC_TYPE_ENUM, 9, type_htng_input_src
};

#define arg_on arg_u8
#define arg_vol arg_u8
#define arg_blue arg_u8
#define arg_brightness arg_u8
#define arg_color arg_u8
#define arg_contrast arg_u8
#define arg_sharpness arg_u8
#define arg_hue arg_u8
#define arg_led_backlight arg_u8
#define arg_date arg_string
#define arg_ddmm arg_u8
#define arg_time arg_string
static const struct arg arg_htng_led_control = {
	CEC_TYPE_ENUM, 3, type_htng_led_control
};

#define arg_options arg_u8
#define arg_val arg_u8
#define arg_minutes arg_u8
static const struct arg arg_htng_chan_type = {
	CEC_TYPE_ENUM, 6, type_htng_chan_type
};

static const struct arg arg_htng_prog_type = {
	CEC_TYPE_ENUM, 2, type_htng_prog_type
};

static const struct arg arg_htng_system_type = {
	CEC_TYPE_ENUM, 9, type_htng_system_type
};

#define arg_freq arg_u16
static const struct arg arg_htng_mod_type = {
	CEC_TYPE_ENUM, 8, type_htng_mod_type
};

static const struct arg arg_htng_symbol_rate = {
	CEC_TYPE_ENUM, 2, type_htng_symbol_rate
};

#define arg_symbol_rate arg_u16
static const struct arg arg_htng_ext_chan_type = {
	CEC_TYPE_ENUM, 6, type_htng_ext_chan_type
};

static const struct arg arg_htng_onid = {
	CEC_TYPE_ENUM, 2, type_htng_onid
};

#define arg_onid arg_u16
static const struct arg arg_htng_nid = {
	CEC_TYPE_ENUM, 2, type_htng_nid
};

#define arg_nid arg_u16
static const struct arg arg_htng_tsid_plp = {
	CEC_TYPE_ENUM, 2, type_htng_tsid_plp
};

#define arg_tsid_plp arg_u16

static const struct message messages[] = {
	{
		CEC_MSG_ACTIVE_SOURCE,
		1, { "phys-addr" },
		{ &arg_phys_addr },
		"ACTIVE_SOURCE"
	}, {
		CEC_MSG_IMAGE_VIEW_ON,
		0, { }, { },
		"IMAGE_VIEW_ON"
	}, {
		CEC_MSG_TEXT_VIEW_ON,
		0, { }, { },
		"TEXT_VIEW_ON"
	}, {
		CEC_MSG_INACTIVE_SOURCE,
		1, { "phys-addr" },
		{ &arg_phys_addr },
		"INACTIVE_SOURCE"
	}, {
		CEC_MSG_REQUEST_ACTIVE_SOURCE,
		0, { }, { },
		"REQUEST_ACTIVE_SOURCE"
	}, {
		CEC_MSG_ROUTING_INFORMATION,
		1, { "phys-addr" },
		{ &arg_phys_addr },
		"ROUTING_INFORMATION"
	}, {
		CEC_MSG_ROUTING_CHANGE,
		2, { "orig-phys-addr", "new-phys-addr" },
		{ &arg_orig_phys_addr, &arg_new_phys_addr },
		"ROUTING_CHANGE"
	}, {
		CEC_MSG_SET_STREAM_PATH,
		1, { "phys-addr" },
		{ &arg_phys_addr },
		"SET_STREAM_PATH"
	}, {
		CEC_MSG_STANDBY,
		0, { }, { },
		"STANDBY"
	}, {
		CEC_MSG_RECORD_OFF,
		0, { }, { },
		"RECORD_OFF"
	}, {
		CEC_MSG_RECORD_ON,
		0, { }, { },
		"RECORD_ON"
	}, {
		CEC_MSG_RECORD_ON,
		9, { "service-id-method", "dig-bcast-system", "transport-id", "service-id", "orig-network-id", "program-number", "channel-number-fmt", "major", "minor" },
		{ &arg_service_id_method, &arg_dig_bcast_system, &arg_transport_id, &arg_service_id, &arg_orig_network_id, &arg_program_number, &arg_channel_number_fmt, &arg_major, &arg_minor },
		"RECORD_ON"
	}, {
		CEC_MSG_RECORD_ON,
		3, { "ana-bcast-type", "ana-freq", "bcast-system" },
		{ &arg_ana_bcast_type, &arg_ana_freq, &arg_bcast_system },
		"RECORD_ON"
	}, {
		CEC_MSG_RECORD_ON,
		1, { "plug" },
		{ &arg_plug },
		"RECORD_ON"
	}, {
		CEC_MSG_RECORD_ON,
		1, { "phys-addr" },
		{ &arg_phys_addr },
		"RECORD_ON"
	}, {
		CEC_MSG_RECORD_STATUS,
		1, { "rec-status" },
		{ &arg_rec_status },
		"RECORD_STATUS"
	}, {
		CEC_MSG_RECORD_TV_SCREEN,
		0, { }, { },
		"RECORD_TV_SCREEN"
	}, {
		CEC_MSG_TIMER_STATUS,
		6, { "timer-overlap-warning", "media-info", "prog-info", "prog-error", "duration-hr", "duration-min" },
		{ &arg_timer_overlap_warning, &arg_media_info, &arg_prog_info, &arg_prog_error, &arg_duration_hr, &arg_duration_min },
		"TIMER_STATUS"
	}, {
		CEC_MSG_TIMER_CLEARED_STATUS,
		1, { "timer-cleared-status" },
		{ &arg_timer_cleared_status },
		"TIMER_CLEARED_STATUS"
	}, {
		CEC_MSG_CLEAR_ANALOGUE_TIMER,
		10, { "day", "month", "start-hr", "start-min", "duration-hr", "duration-min", "recording-seq", "ana-bcast-type", "ana-freq", "bcast-system" },
		{ &arg_day, &arg_month, &arg_start_hr, &arg_start_min, &arg_duration_hr, &arg_duration_min, &arg_recording_seq, &arg_ana_bcast_type, &arg_ana_freq, &arg_bcast_system },
		"CLEAR_ANALOGUE_TIMER"
	}, {
		CEC_MSG_CLEAR_DIGITAL_TIMER,
		16, { "day", "month", "start-hr", "start-min", "duration-hr", "duration-min", "recording-seq", "service-id-method", "dig-bcast-system", "transport-id", "service-id", "orig-network-id", "program-number", "channel-number-fmt", "major", "minor" },
		{ &arg_day, &arg_month, &arg_start_hr, &arg_start_min, &arg_duration_hr, &arg_duration_min, &arg_recording_seq, &arg_service_id_method, &arg_dig_bcast_system, &arg_transport_id, &arg_service_id, &arg_orig_network_id, &arg_program_number, &arg_channel_number_fmt, &arg_major, &arg_minor },
		"CLEAR_DIGITAL_TIMER"
	}, {
		CEC_MSG_CLEAR_EXT_TIMER,
		10, { "day", "month", "start-hr", "start-min", "duration-hr", "duration-min", "recording-seq", "ext-src-spec", "plug", "phys-addr" },
		{ &arg_day, &arg_month, &arg_start_hr, &arg_start_min, &arg_duration_hr, &arg_duration_min, &arg_recording_seq, &arg_ext_src_spec, &arg_plug, &arg_phys_addr },
		"CLEAR_EXT_TIMER"
	}, {
		CEC_MSG_SET_ANALOGUE_TIMER,
		10, { "day", "month", "start-hr", "start-min", "duration-hr", "duration-min", "recording-seq", "ana-bcast-type", "ana-freq", "bcast-system" },
		{ &arg_day, &arg_month, &arg_start_hr, &arg_start_min, &arg_duration_hr, &arg_duration_min, &arg_recording_seq, &arg_ana_bcast_type, &arg_ana_freq, &arg_bcast_system },
		"SET_ANALOGUE_TIMER"
	}, {
		CEC_MSG_SET_DIGITAL_TIMER,
		16, { "day", "month", "start-hr", "start-min", "duration-hr", "duration-min", "recording-seq", "service-id-method", "dig-bcast-system", "transport-id", "service-id", "orig-network-id", "program-number", "channel-number-fmt", "major", "minor" },
		{ &arg_day, &arg_month, &arg_start_hr, &arg_start_min, &arg_duration_hr, &arg_duration_min, &arg_recording_seq, &arg_service_id_method, &arg_dig_bcast_system, &arg_transport_id, &arg_service_id, &arg_orig_network_id, &arg_program_number, &arg_channel_number_fmt, &arg_major, &arg_minor },
		"SET_DIGITAL_TIMER"
	}, {
		CEC_MSG_SET_EXT_TIMER,
		10, { "day", "month", "start-hr", "start-min", "duration-hr", "duration-min", "recording-seq", "ext-src-spec", "plug", "phys-addr" },
		{ &arg_day, &arg_month, &arg_start_hr, &arg_start_min, &arg_duration_hr, &arg_duration_min, &arg_recording_seq, &arg_ext_src_spec, &arg_plug, &arg_phys_addr },
		"SET_EXT_TIMER"
	}, {
		CEC_MSG_SET_TIMER_PROGRAM_TITLE,
		1, { "prog-title" },
		{ &arg_prog_title },
		"SET_TIMER_PROGRAM_TITLE"
	}, {
		CEC_MSG_CEC_VERSION,
		1, { "cec-version" },
		{ &arg_cec_version },
		"CEC_VERSION"
	}, {
		CEC_MSG_GET_CEC_VERSION,
		0, { }, { },
		"GET_CEC_VERSION"
	}, {
		CEC_MSG_REPORT_PHYSICAL_ADDR,
		2, { "phys-addr", "prim-devtype" },
		{ &arg_phys_addr, &arg_prim_devtype },
		"REPORT_PHYSICAL_ADDR"
	}, {
		CEC_MSG_GIVE_PHYSICAL_ADDR,
		0, { }, { },
		"GIVE_PHYSICAL_ADDR"
	}, {
		CEC_MSG_SET_MENU_LANGUAGE,
		1, { "language" },
		{ &arg_language },
		"SET_MENU_LANGUAGE"
	}, {
		CEC_MSG_GET_MENU_LANGUAGE,
		0, { }, { },
		"GET_MENU_LANGUAGE"
	}, {
		CEC_MSG_REPORT_FEATURES,
		4, { "cec-version", "all-device-types", "rc-profile", "dev-features" },
		{ &arg_cec_version, &arg_all_device_types, &arg_rc_profile, &arg_dev_features },
		"REPORT_FEATURES"
	}, {
		CEC_MSG_GIVE_FEATURES,
		0, { }, { },
		"GIVE_FEATURES"
	}, {
		CEC_MSG_DECK_CONTROL,
		1, { "deck-control-mode" },
		{ &arg_deck_control_mode },
		"DECK_CONTROL"
	}, {
		CEC_MSG_DECK_STATUS,
		1, { "deck-info" },
		{ &arg_deck_info },
		"DECK_STATUS"
	}, {
		CEC_MSG_GIVE_DECK_STATUS,
		1, { "status-req" },
		{ &arg_status_req },
		"GIVE_DECK_STATUS"
	}, {
		CEC_MSG_PLAY,
		1, { "play-mode" },
		{ &arg_play_mode },
		"PLAY"
	}, {
		CEC_MSG_TUNER_DEVICE_STATUS,
		5, { "rec-flag", "tuner-display-info", "ana-bcast-type", "ana-freq", "bcast-system" },
		{ &arg_rec_flag, &arg_tuner_display_info, &arg_ana_bcast_type, &arg_ana_freq, &arg_bcast_system },
		"TUNER_DEVICE_STATUS"
	}, {
		CEC_MSG_TUNER_DEVICE_STATUS,
		11, { "rec-flag", "tuner-display-info", "service-id-method", "dig-bcast-system", "transport-id", "service-id", "orig-network-id", "program-number", "channel-number-fmt", "major", "minor" },
		{ &arg_rec_flag, &arg_tuner_display_info, &arg_service_id_method, &arg_dig_bcast_system, &arg_transport_id, &arg_service_id, &arg_orig_network_id, &arg_program_number, &arg_channel_number_fmt, &arg_major, &arg_minor },
		"TUNER_DEVICE_STATUS"
	}, {
		CEC_MSG_GIVE_TUNER_DEVICE_STATUS,
		1, { "status-req" },
		{ &arg_status_req },
		"GIVE_TUNER_DEVICE_STATUS"
	}, {
		CEC_MSG_SELECT_ANALOGUE_SERVICE,
		3, { "ana-bcast-type", "ana-freq", "bcast-system" },
		{ &arg_ana_bcast_type, &arg_ana_freq, &arg_bcast_system },
		"SELECT_ANALOGUE_SERVICE"
	}, {
		CEC_MSG_SELECT_DIGITAL_SERVICE,
		9, { "service-id-method", "dig-bcast-system", "transport-id", "service-id", "orig-network-id", "program-number", "channel-number-fmt", "major", "minor" },
		{ &arg_service_id_method, &arg_dig_bcast_system, &arg_transport_id, &arg_service_id, &arg_orig_network_id, &arg_program_number, &arg_channel_number_fmt, &arg_major, &arg_minor },
		"SELECT_DIGITAL_SERVICE"
	}, {
		CEC_MSG_TUNER_STEP_DECREMENT,
		0, { }, { },
		"TUNER_STEP_DECREMENT"
	}, {
		CEC_MSG_TUNER_STEP_INCREMENT,
		0, { }, { },
		"TUNER_STEP_INCREMENT"
	}, {
		CEC_MSG_DEVICE_VENDOR_ID,
		1, { "vendor-id" },
		{ &arg_vendor_id },
		"DEVICE_VENDOR_ID"
	}, {
		CEC_MSG_GIVE_DEVICE_VENDOR_ID,
		0, { }, { },
		"GIVE_DEVICE_VENDOR_ID"
	}, {
		CEC_MSG_VENDOR_REMOTE_BUTTON_UP,
		0, { }, { },
		"VENDOR_REMOTE_BUTTON_UP"
	}, {
		CEC_MSG_SET_OSD_STRING,
		2, { "disp-ctl", "osd" },
		{ &arg_disp_ctl, &arg_osd },
		"SET_OSD_STRING"
	}, {
		CEC_MSG_SET_OSD_NAME,
		1, { "name" },
		{ &arg_name },
		"SET_OSD_NAME"
	}, {
		CEC_MSG_GIVE_OSD_NAME,
		0, { }, { },
		"GIVE_OSD_NAME"
	}, {
		CEC_MSG_MENU_STATUS,
		1, { "menu-state" },
		{ &arg_menu_state },
		"MENU_STATUS"
	}, {
		CEC_MSG_MENU_REQUEST,
		1, { "menu-req" },
		{ &arg_menu_req },
		"MENU_REQUEST"
	}, {
		CEC_MSG_USER_CONTROL_PRESSED,
		11, { "ui-cmd", "has-opt-arg", "play-mode", "ui-function-media", "ui-function-select-av-input", "ui-function-select-audio-input", "ui-bcast-type", "ui-snd-pres-ctl", "channel-number-fmt", "major", "minor" },
		{ &arg_rc_ui_cmd, &arg_has_opt_arg, &arg_play_mode, &arg_ui_function_media, &arg_ui_function_select_av_input, &arg_ui_function_select_audio_input, &arg_ui_bcast_type, &arg_ui_snd_pres_ctl, &arg_channel_number_fmt, &arg_major, &arg_minor },
		"USER_CONTROL_PRESSED"
	}, {
		CEC_MSG_USER_CONTROL_RELEASED,
		0, { }, { },
		"USER_CONTROL_RELEASED"
	}, {
		CEC_MSG_REPORT_POWER_STATUS,
		1, { "pwr-state" },
		{ &arg_pwr_state },
		"REPORT_POWER_STATUS"
	}, {
		CEC_MSG_GIVE_DEVICE_POWER_STATUS,
		0, { }, { },
		"GIVE_DEVICE_POWER_STATUS"
	}, {
		CEC_MSG_FEATURE_ABORT,
		2, { "abort-msg", "reason" },
		{ &arg_abort_msg, &arg_reason },
		"FEATURE_ABORT"
	}, {
		CEC_MSG_ABORT,
		0, { }, { },
		"ABORT"
	}, {
		CEC_MSG_REPORT_AUDIO_STATUS,
		2, { "aud-mute-status", "aud-vol-status" },
		{ &arg_aud_mute_status, &arg_aud_vol_status },
		"REPORT_AUDIO_STATUS"
	}, {
		CEC_MSG_GIVE_AUDIO_STATUS,
		0, { }, { },
		"GIVE_AUDIO_STATUS"
	}, {
		CEC_MSG_SET_SYSTEM_AUDIO_MODE,
		1, { "sys-aud-status" },
		{ &arg_sys_aud_status },
		"SET_SYSTEM_AUDIO_MODE"
	}, {
		CEC_MSG_SYSTEM_AUDIO_MODE_REQUEST,
		1, { "phys-addr" },
		{ &arg_phys_addr },
		"SYSTEM_AUDIO_MODE_REQUEST"
	}, {
		CEC_MSG_SYSTEM_AUDIO_MODE_STATUS,
		1, { "sys-aud-status" },
		{ &arg_sys_aud_status },
		"SYSTEM_AUDIO_MODE_STATUS"
	}, {
		CEC_MSG_GIVE_SYSTEM_AUDIO_MODE_STATUS,
		0, { }, { },
		"GIVE_SYSTEM_AUDIO_MODE_STATUS"
	}, {
		CEC_MSG_REPORT_SHORT_AUDIO_DESCRIPTOR,
		5, { "num-descriptors", "descriptor1", "descriptor2", "descriptor3", "descriptor4" },
		{ &arg_num_descriptors, &arg_descriptor1, &arg_descriptor2, &arg_descriptor3, &arg_descriptor4 },
		"REPORT_SHORT_AUDIO_DESCRIPTOR"
	}, {
		CEC_MSG_REQUEST_SHORT_AUDIO_DESCRIPTOR,
		9, { "num-descriptors", "audio-format-id1", "audio-format-code1", "audio-format-id2", "audio-format-code2", "audio-format-id3", "audio-format-code3", "audio-format-id4", "audio-format-code4" },
		{ &arg_num_descriptors, &arg_audio_format_id1, &arg_audio_format_code1, &arg_audio_format_id2, &arg_audio_format_code2, &arg_audio_format_id3, &arg_audio_format_code3, &arg_audio_format_id4, &arg_audio_format_code4 },
		"REQUEST_SHORT_AUDIO_DESCRIPTOR"
	}, {
		CEC_MSG_SET_AUDIO_RATE,
		1, { "audio-rate" },
		{ &arg_audio_rate },
		"SET_AUDIO_RATE"
	}, {
		CEC_MSG_REPORT_ARC_INITIATED,
		0, { }, { },
		"REPORT_ARC_INITIATED"
	}, {
		CEC_MSG_INITIATE_ARC,
		0, { }, { },
		"INITIATE_ARC"
	}, {
		CEC_MSG_REQUEST_ARC_INITIATION,
		0, { }, { },
		"REQUEST_ARC_INITIATION"
	}, {
		CEC_MSG_REPORT_ARC_TERMINATED,
		0, { }, { },
		"REPORT_ARC_TERMINATED"
	}, {
		CEC_MSG_TERMINATE_ARC,
		0, { }, { },
		"TERMINATE_ARC"
	}, {
		CEC_MSG_REQUEST_ARC_TERMINATION,
		0, { }, { },
		"REQUEST_ARC_TERMINATION"
	}, {
		CEC_MSG_REPORT_CURRENT_LATENCY,
		5, { "phys-addr", "video-latency", "low-latency-mode", "audio-out-compensated", "audio-out-delay" },
		{ &arg_phys_addr, &arg_video_latency, &arg_low_latency_mode, &arg_audio_out_compensated, &arg_audio_out_delay },
		"REPORT_CURRENT_LATENCY"
	}, {
		CEC_MSG_REQUEST_CURRENT_LATENCY,
		1, { "phys-addr" },
		{ &arg_phys_addr },
		"REQUEST_CURRENT_LATENCY"
	}, {
		CEC_MSG_CDC_HEC_INQUIRE_STATE,
		2, { "phys-addr1", "phys-addr2" },
		{ &arg_phys_addr1, &arg_phys_addr2 },
		"CDC_HEC_INQUIRE_STATE"
	}, {
		CEC_MSG_CDC_HEC_REPORT_STATE,
		7, { "target-phys-addr", "hec-func-state", "host-func-state", "enc-func-state", "cdc-errcode", "has-field", "hec-field" },
		{ &arg_target_phys_addr, &arg_hec_func_state, &arg_host_func_state, &arg_enc_func_state, &arg_cdc_errcode, &arg_has_field, &arg_hec_field },
		"CDC_HEC_REPORT_STATE"
	}, {
		CEC_MSG_CDC_HEC_SET_STATE,
		6, { "phys-addr1", "phys-addr2", "hec-set-state", "phys-addr3", "phys-addr4", "phys-addr5" },
		{ &arg_phys_addr1, &arg_phys_addr2, &arg_hec_set_state, &arg_phys_addr3, &arg_phys_addr4, &arg_phys_addr5 },
		"CDC_HEC_SET_STATE"
	}, {
		CEC_MSG_CDC_HEC_SET_STATE_ADJACENT,
		2, { "phys-addr1", "hec-set-state" },
		{ &arg_phys_addr1, &arg_hec_set_state },
		"CDC_HEC_SET_STATE_ADJACENT"
	}, {
		CEC_MSG_CDC_HEC_REQUEST_DEACTIVATION,
		3, { "phys-addr1", "phys-addr2", "phys-addr3" },
		{ &arg_phys_addr1, &arg_phys_addr2, &arg_phys_addr3 },
		"CDC_HEC_REQUEST_DEACTIVATION"
	}, {
		CEC_MSG_CDC_HEC_NOTIFY_ALIVE,
		0, { }, { },
		"CDC_HEC_NOTIFY_ALIVE"
	}, {
		CEC_MSG_CDC_HEC_DISCOVER,
		0, { }, { },
		"CDC_HEC_DISCOVER"
	}, {
		CEC_MSG_CDC_HPD_SET_STATE,
		2, { "input-port", "hpd-state" },
		{ &arg_input_port, &arg_hpd_state },
		"CDC_HPD_SET_STATE"
	}, {
		CEC_MSG_CDC_HPD_REPORT_STATE,
		2, { "hpd-state", "hpd-error" },
		{ &arg_hpd_state, &arg_hpd_error },
		"CDC_HPD_REPORT_STATE"
	}, {
		CEC_MSG_HTNG_TUNER_1PART_CHAN,
		2, { "htng-tuner-type", "chan" },
		{ &arg_htng_tuner_type, &arg_chan },
		"HTNG_TUNER_1PART_CHAN"
	}, {
		CEC_MSG_HTNG_TUNER_2PART_CHAN,
		3, { "htng-tuner-type", "major-chan", "minor-chan" },
		{ &arg_htng_tuner_type, &arg_major_chan, &arg_minor_chan },
		"HTNG_TUNER_2PART_CHAN"
	}, {
		CEC_MSG_HTNG_INPUT_SEL_AV,
		1, { "input" },
		{ &arg_input },
		"HTNG_INPUT_SEL_AV"
	}, {
		CEC_MSG_HTNG_INPUT_SEL_PC,
		1, { "input" },
		{ &arg_input },
		"HTNG_INPUT_SEL_PC"
	}, {
		CEC_MSG_HTNG_INPUT_SEL_HDMI,
		1, { "input" },
		{ &arg_input },
		"HTNG_INPUT_SEL_HDMI"
	}, {
		CEC_MSG_HTNG_INPUT_SEL_COMPONENT,
		1, { "input" },
		{ &arg_input },
		"HTNG_INPUT_SEL_COMPONENT"
	}, {
		CEC_MSG_HTNG_INPUT_SEL_DVI,
		1, { "input" },
		{ &arg_input },
		"HTNG_INPUT_SEL_DVI"
	}, {
		CEC_MSG_HTNG_INPUT_SEL_DP,
		1, { "input" },
		{ &arg_input },
		"HTNG_INPUT_SEL_DP"
	}, {
		CEC_MSG_HTNG_INPUT_SEL_USB,
		1, { "input" },
		{ &arg_input },
		"HTNG_INPUT_SEL_USB"
	}, {
		CEC_MSG_HTNG_SET_DEF_PWR_ON_INPUT_SRC,
		4, { "htng-input-src", "htng-tuner-type", "major", "input" },
		{ &arg_htng_input_src, &arg_htng_tuner_type, &arg_major, &arg_input },
		"HTNG_SET_DEF_PWR_ON_INPUT_SRC"
	}, {
		CEC_MSG_HTNG_SET_TV_SPEAKERS,
		1, { "on" },
		{ &arg_on },
		"HTNG_SET_TV_SPEAKERS"
	}, {
		CEC_MSG_HTNG_SET_DIG_AUDIO,
		1, { "on" },
		{ &arg_on },
		"HTNG_SET_DIG_AUDIO"
	}, {
		CEC_MSG_HTNG_SET_ANA_AUDIO,
		1, { "on" },
		{ &arg_on },
		"HTNG_SET_ANA_AUDIO"
	}, {
		CEC_MSG_HTNG_SET_DEF_PWR_ON_VOL,
		1, { "vol" },
		{ &arg_vol },
		"HTNG_SET_DEF_PWR_ON_VOL"
	}, {
		CEC_MSG_HTNG_SET_MAX_VOL,
		1, { "vol" },
		{ &arg_vol },
		"HTNG_SET_MAX_VOL"
	}, {
		CEC_MSG_HTNG_SET_MIN_VOL,
		1, { "vol" },
		{ &arg_vol },
		"HTNG_SET_MIN_VOL"
	}, {
		CEC_MSG_HTNG_SET_BLUE_SCREEN,
		1, { "blue" },
		{ &arg_blue },
		"HTNG_SET_BLUE_SCREEN"
	}, {
		CEC_MSG_HTNG_SET_BRIGHTNESS,
		1, { "brightness" },
		{ &arg_brightness },
		"HTNG_SET_BRIGHTNESS"
	}, {
		CEC_MSG_HTNG_SET_COLOR,
		1, { "color" },
		{ &arg_color },
		"HTNG_SET_COLOR"
	}, {
		CEC_MSG_HTNG_SET_CONTRAST,
		1, { "contrast" },
		{ &arg_contrast },
		"HTNG_SET_CONTRAST"
	}, {
		CEC_MSG_HTNG_SET_SHARPNESS,
		1, { "sharpness" },
		{ &arg_sharpness },
		"HTNG_SET_SHARPNESS"
	}, {
		CEC_MSG_HTNG_SET_HUE,
		1, { "hue" },
		{ &arg_hue },
		"HTNG_SET_HUE"
	}, {
		CEC_MSG_HTNG_SET_LED_BACKLIGHT,
		1, { "led-backlight" },
		{ &arg_led_backlight },
		"HTNG_SET_LED_BACKLIGHT"
	}, {
		CEC_MSG_HTNG_SET_TV_OSD_CONTROL,
		1, { "on" },
		{ &arg_on },
		"HTNG_SET_TV_OSD_CONTROL"
	}, {
		CEC_MSG_HTNG_SET_AUDIO_ONLY_DISPLAY,
		1, { "on" },
		{ &arg_on },
		"HTNG_SET_AUDIO_ONLY_DISPLAY"
	}, {
		CEC_MSG_HTNG_SET_DATE,
		1, { "date" },
		{ &arg_date },
		"HTNG_SET_DATE"
	}, {
		CEC_MSG_HTNG_SET_DATE_FORMAT,
		1, { "ddmm" },
		{ &arg_ddmm },
		"HTNG_SET_DATE_FORMAT"
	}, {
		CEC_MSG_HTNG_SET_TIME,
		1, { "time" },
		{ &arg_time },
		"HTNG_SET_TIME"
	}, {
		CEC_MSG_HTNG_SET_CLK_BRIGHTNESS_STANDBY,
		1, { "brightness" },
		{ &arg_brightness },
		"HTNG_SET_CLK_BRIGHTNESS_STANDBY"
	}, {
		CEC_MSG_HTNG_SET_CLK_BRIGHTNESS_ON,
		1, { "brightness" },
		{ &arg_brightness },
		"HTNG_SET_CLK_BRIGHTNESS_ON"
	}, {
		CEC_MSG_HTNG_LED_CONTROL,
		1, { "htng-led-control" },
		{ &arg_htng_led_control },
		"HTNG_LED_CONTROL"
	}, {
		CEC_MSG_HTNG_LOCK_TV_PWR_BUTTON,
		1, { "on" },
		{ &arg_on },
		"HTNG_LOCK_TV_PWR_BUTTON"
	}, {
		CEC_MSG_HTNG_LOCK_TV_VOL_BUTTONS,
		1, { "on" },
		{ &arg_on },
		"HTNG_LOCK_TV_VOL_BUTTONS"
	}, {
		CEC_MSG_HTNG_LOCK_TV_CHAN_BUTTONS,
		1, { "on" },
		{ &arg_on },
		"HTNG_LOCK_TV_CHAN_BUTTONS"
	}, {
		CEC_MSG_HTNG_LOCK_TV_INPUT_BUTTONS,
		1, { "on" },
		{ &arg_on },
		"HTNG_LOCK_TV_INPUT_BUTTONS"
	}, {
		CEC_MSG_HTNG_LOCK_TV_OTHER_BUTTONS,
		1, { "on" },
		{ &arg_on },
		"HTNG_LOCK_TV_OTHER_BUTTONS"
	}, {
		CEC_MSG_HTNG_LOCK_EVERYTHING,
		1, { "on" },
		{ &arg_on },
		"HTNG_LOCK_EVERYTHING"
	}, {
		CEC_MSG_HTNG_LOCK_EVERYTHING_BUT_PWR,
		1, { "on" },
		{ &arg_on },
		"HTNG_LOCK_EVERYTHING_BUT_PWR"
	}, {
		CEC_MSG_HTNG_HOTEL_MODE,
		2, { "on", "options" },
		{ &arg_on, &arg_options },
		"HTNG_HOTEL_MODE"
	}, {
		CEC_MSG_HTNG_SET_PWR_SAVING_PROFILE,
		2, { "on", "val" },
		{ &arg_on, &arg_val },
		"HTNG_SET_PWR_SAVING_PROFILE"
	}, {
		CEC_MSG_HTNG_SET_SLEEP_TIMER,
		1, { "minutes" },
		{ &arg_minutes },
		"HTNG_SET_SLEEP_TIMER"
	}, {
		CEC_MSG_HTNG_SET_WAKEUP_TIME,
		1, { "time" },
		{ &arg_time },
		"HTNG_SET_WAKEUP_TIME"
	}, {
		CEC_MSG_HTNG_SET_AUTO_OFF_TIME,
		1, { "time" },
		{ &arg_time },
		"HTNG_SET_AUTO_OFF_TIME"
	}, {
		CEC_MSG_HTNG_SET_WAKEUP_SRC,
		4, { "htng-input-src", "htng-tuner-type", "major", "input" },
		{ &arg_htng_input_src, &arg_htng_tuner_type, &arg_major, &arg_input },
		"HTNG_SET_WAKEUP_SRC"
	}, {
		CEC_MSG_HTNG_SET_INIT_WAKEUP_VOL,
		2, { "vol", "minutes" },
		{ &arg_vol, &arg_minutes },
		"HTNG_SET_INIT_WAKEUP_VOL"
	}, {
		CEC_MSG_HTNG_CLR_ALL_SLEEP_WAKE,
		0, { }, { },
		"HTNG_CLR_ALL_SLEEP_WAKE"
	}, {
		CEC_MSG_HTNG_GLOBAL_DIRECT_TUNE_FREQ,
		8, { "htng-chan-type", "htng-prog-type", "htng-system-type", "freq", "service-id", "htng-mod-type", "htng-symbol-rate", "symbol-rate" },
		{ &arg_htng_chan_type, &arg_htng_prog_type, &arg_htng_system_type, &arg_freq, &arg_service_id, &arg_htng_mod_type, &arg_htng_symbol_rate, &arg_symbol_rate },
		"HTNG_GLOBAL_DIRECT_TUNE_FREQ"
	}, {
		CEC_MSG_HTNG_GLOBAL_DIRECT_TUNE_CHAN,
		3, { "htng-chan-type", "htng-prog-type", "chan" },
		{ &arg_htng_chan_type, &arg_htng_prog_type, &arg_chan },
		"HTNG_GLOBAL_DIRECT_TUNE_CHAN"
	}, {
		CEC_MSG_HTNG_GLOBAL_DIRECT_TUNE_EXT_FREQ,
		14, { "htng-ext-chan-type", "htng-prog-type", "htng-system-type", "freq", "service-id", "htng-mod-type", "htng-onid", "onid", "htng-nid", "nid", "htng-tsid-plp", "tsid-plp", "htng-symbol-rate", "symbol-rate" },
		{ &arg_htng_ext_chan_type, &arg_htng_prog_type, &arg_htng_system_type, &arg_freq, &arg_service_id, &arg_htng_mod_type, &arg_htng_onid, &arg_onid, &arg_htng_nid, &arg_nid, &arg_htng_tsid_plp, &arg_tsid_plp, &arg_htng_symbol_rate, &arg_symbol_rate },
		"HTNG_GLOBAL_DIRECT_TUNE_EXT_FREQ"
	}, {
	}
};

void log_msg(const struct cec_msg *msg)
{
	if (msg->len == 1) {
		printf("CEC_MSG_POLL\n");
		goto status;
	}

	switch (msg->msg[1]) {
	case CEC_MSG_ACTIVE_SOURCE: {
		__u16 phys_addr;

		cec_ops_active_source(msg, &phys_addr);
		printf("CEC_MSG_ACTIVE_SOURCE (0x%02x):\n", CEC_MSG_ACTIVE_SOURCE);
		log_arg(&arg_phys_addr, "phys-addr", phys_addr);
		break;
	}
	case CEC_MSG_IMAGE_VIEW_ON:
		printf("CEC_MSG_IMAGE_VIEW_ON (0x%02x)\n", CEC_MSG_IMAGE_VIEW_ON);
		break;

	case CEC_MSG_TEXT_VIEW_ON:
		printf("CEC_MSG_TEXT_VIEW_ON (0x%02x)\n", CEC_MSG_TEXT_VIEW_ON);
		break;

	case CEC_MSG_INACTIVE_SOURCE: {
		__u16 phys_addr;

		cec_ops_inactive_source(msg, &phys_addr);
		printf("CEC_MSG_INACTIVE_SOURCE (0x%02x):\n", CEC_MSG_INACTIVE_SOURCE);
		log_arg(&arg_phys_addr, "phys-addr", phys_addr);
		break;
	}
	case CEC_MSG_REQUEST_ACTIVE_SOURCE:
		printf("CEC_MSG_REQUEST_ACTIVE_SOURCE (0x%02x)\n", CEC_MSG_REQUEST_ACTIVE_SOURCE);
		break;

	case CEC_MSG_ROUTING_INFORMATION: {
		__u16 phys_addr;

		cec_ops_routing_information(msg, &phys_addr);
		printf("CEC_MSG_ROUTING_INFORMATION (0x%02x):\n", CEC_MSG_ROUTING_INFORMATION);
		log_arg(&arg_phys_addr, "phys-addr", phys_addr);
		break;
	}
	case CEC_MSG_ROUTING_CHANGE: {
		__u16 orig_phys_addr;
		__u16 new_phys_addr;

		cec_ops_routing_change(msg, &orig_phys_addr, &new_phys_addr);
		printf("CEC_MSG_ROUTING_CHANGE (0x%02x):\n", CEC_MSG_ROUTING_CHANGE);
		log_arg(&arg_orig_phys_addr, "orig-phys-addr", orig_phys_addr);
		log_arg(&arg_new_phys_addr, "new-phys-addr", new_phys_addr);
		break;
	}
	case CEC_MSG_SET_STREAM_PATH: {
		__u16 phys_addr;

		cec_ops_set_stream_path(msg, &phys_addr);
		printf("CEC_MSG_SET_STREAM_PATH (0x%02x):\n", CEC_MSG_SET_STREAM_PATH);
		log_arg(&arg_phys_addr, "phys-addr", phys_addr);
		break;
	}
	case CEC_MSG_STANDBY:
		printf("CEC_MSG_STANDBY (0x%02x)\n", CEC_MSG_STANDBY);
		break;

	case CEC_MSG_RECORD_OFF:
		printf("CEC_MSG_RECORD_OFF (0x%02x)\n", CEC_MSG_RECORD_OFF);
		break;

	case CEC_MSG_RECORD_ON: {
		struct cec_op_record_src rec_src = {};

		cec_ops_record_on(msg, &rec_src);
		printf("CEC_MSG_RECORD_ON (0x%02x):\n", CEC_MSG_RECORD_ON);
		log_rec_src("rec-src", &rec_src);
		break;
	}
	case CEC_MSG_RECORD_STATUS: {
		__u8 rec_status;

		cec_ops_record_status(msg, &rec_status);
		printf("CEC_MSG_RECORD_STATUS (0x%02x):\n", CEC_MSG_RECORD_STATUS);
		log_arg(&arg_rec_status, "rec-status", rec_status);
		break;
	}
	case CEC_MSG_RECORD_TV_SCREEN:
		printf("CEC_MSG_RECORD_TV_SCREEN (0x%02x)\n", CEC_MSG_RECORD_TV_SCREEN);
		break;

	case CEC_MSG_TIMER_STATUS: {
		__u8 timer_overlap_warning;
		__u8 media_info;
		__u8 prog_info;
		__u8 prog_error;
		__u8 duration_hr;
		__u8 duration_min;

		cec_ops_timer_status(msg, &timer_overlap_warning, &media_info, &prog_info, &prog_error, &duration_hr, &duration_min);
		printf("CEC_MSG_TIMER_STATUS (0x%02x):\n", CEC_MSG_TIMER_STATUS);
		log_arg(&arg_timer_overlap_warning, "timer-overlap-warning", timer_overlap_warning);
		log_arg(&arg_media_info, "media-info", media_info);
		log_arg(&arg_prog_info, "prog-info", prog_info);
		log_arg(&arg_prog_error, "prog-error", prog_error);
		log_arg(&arg_duration_hr, "duration-hr", duration_hr);
		log_arg(&arg_duration_min, "duration-min", duration_min);
		break;
	}
	case CEC_MSG_TIMER_CLEARED_STATUS: {
		__u8 timer_cleared_status;

		cec_ops_timer_cleared_status(msg, &timer_cleared_status);
		printf("CEC_MSG_TIMER_CLEARED_STATUS (0x%02x):\n", CEC_MSG_TIMER_CLEARED_STATUS);
		log_arg(&arg_timer_cleared_status, "timer-cleared-status", timer_cleared_status);
		break;
	}
	case CEC_MSG_CLEAR_ANALOGUE_TIMER: {
		__u8 day;
		__u8 month;
		__u8 start_hr;
		__u8 start_min;
		__u8 duration_hr;
		__u8 duration_min;
		__u8 recording_seq;
		__u8 ana_bcast_type;
		__u16 ana_freq;
		__u8 bcast_system;

		cec_ops_clear_analogue_timer(msg, &day, &month, &start_hr, &start_min, &duration_hr, &duration_min, &recording_seq, &ana_bcast_type, &ana_freq, &bcast_system);
		printf("CEC_MSG_CLEAR_ANALOGUE_TIMER (0x%02x):\n", CEC_MSG_CLEAR_ANALOGUE_TIMER);
		log_arg(&arg_day, "day", day);
		log_arg(&arg_month, "month", month);
		log_arg(&arg_start_hr, "start-hr", start_hr);
		log_arg(&arg_start_min, "start-min", start_min);
		log_arg(&arg_duration_hr, "duration-hr", duration_hr);
		log_arg(&arg_duration_min, "duration-min", duration_min);
		log_arg(&arg_recording_seq, "recording-seq", recording_seq);
		log_arg(&arg_ana_bcast_type, "ana-bcast-type", ana_bcast_type);
		log_arg(&arg_ana_freq, "ana-freq", ana_freq);
		log_arg(&arg_bcast_system, "bcast-system", bcast_system);
		break;
	}
	case CEC_MSG_CLEAR_DIGITAL_TIMER: {
		__u8 day;
		__u8 month;
		__u8 start_hr;
		__u8 start_min;
		__u8 duration_hr;
		__u8 duration_min;
		__u8 recording_seq;
		struct cec_op_digital_service_id digital = {};

		cec_ops_clear_digital_timer(msg, &day, &month, &start_hr, &start_min, &duration_hr, &duration_min, &recording_seq, &digital);
		printf("CEC_MSG_CLEAR_DIGITAL_TIMER (0x%02x):\n", CEC_MSG_CLEAR_DIGITAL_TIMER);
		log_arg(&arg_day, "day", day);
		log_arg(&arg_month, "month", month);
		log_arg(&arg_start_hr, "start-hr", start_hr);
		log_arg(&arg_start_min, "start-min", start_min);
		log_arg(&arg_duration_hr, "duration-hr", duration_hr);
		log_arg(&arg_duration_min, "duration-min", duration_min);
		log_arg(&arg_recording_seq, "recording-seq", recording_seq);
		log_digital("digital", &digital);
		break;
	}
	case CEC_MSG_CLEAR_EXT_TIMER: {
		__u8 day;
		__u8 month;
		__u8 start_hr;
		__u8 start_min;
		__u8 duration_hr;
		__u8 duration_min;
		__u8 recording_seq;
		__u8 ext_src_spec;
		__u8 plug;
		__u16 phys_addr;

		cec_ops_clear_ext_timer(msg, &day, &month, &start_hr, &start_min, &duration_hr, &duration_min, &recording_seq, &ext_src_spec, &plug, &phys_addr);
		printf("CEC_MSG_CLEAR_EXT_TIMER (0x%02x):\n", CEC_MSG_CLEAR_EXT_TIMER);
		log_arg(&arg_day, "day", day);
		log_arg(&arg_month, "month", month);
		log_arg(&arg_start_hr, "start-hr", start_hr);
		log_arg(&arg_start_min, "start-min", start_min);
		log_arg(&arg_duration_hr, "duration-hr", duration_hr);
		log_arg(&arg_duration_min, "duration-min", duration_min);
		log_arg(&arg_recording_seq, "recording-seq", recording_seq);
		log_arg(&arg_ext_src_spec, "ext-src-spec", ext_src_spec);
		log_arg(&arg_plug, "plug", plug);
		log_arg(&arg_phys_addr, "phys-addr", phys_addr);
		break;
	}
	case CEC_MSG_SET_ANALOGUE_TIMER: {
		__u8 day;
		__u8 month;
		__u8 start_hr;
		__u8 start_min;
		__u8 duration_hr;
		__u8 duration_min;
		__u8 recording_seq;
		__u8 ana_bcast_type;
		__u16 ana_freq;
		__u8 bcast_system;

		cec_ops_set_analogue_timer(msg, &day, &month, &start_hr, &start_min, &duration_hr, &duration_min, &recording_seq, &ana_bcast_type, &ana_freq, &bcast_system);
		printf("CEC_MSG_SET_ANALOGUE_TIMER (0x%02x):\n", CEC_MSG_SET_ANALOGUE_TIMER);
		log_arg(&arg_day, "day", day);
		log_arg(&arg_month, "month", month);
		log_arg(&arg_start_hr, "start-hr", start_hr);
		log_arg(&arg_start_min, "start-min", start_min);
		log_arg(&arg_duration_hr, "duration-hr", duration_hr);
		log_arg(&arg_duration_min, "duration-min", duration_min);
		log_arg(&arg_recording_seq, "recording-seq", recording_seq);
		log_arg(&arg_ana_bcast_type, "ana-bcast-type", ana_bcast_type);
		log_arg(&arg_ana_freq, "ana-freq", ana_freq);
		log_arg(&arg_bcast_system, "bcast-system", bcast_system);
		break;
	}
	case CEC_MSG_SET_DIGITAL_TIMER: {
		__u8 day;
		__u8 month;
		__u8 start_hr;
		__u8 start_min;
		__u8 duration_hr;
		__u8 duration_min;
		__u8 recording_seq;
		struct cec_op_digital_service_id digital = {};

		cec_ops_set_digital_timer(msg, &day, &month, &start_hr, &start_min, &duration_hr, &duration_min, &recording_seq, &digital);
		printf("CEC_MSG_SET_DIGITAL_TIMER (0x%02x):\n", CEC_MSG_SET_DIGITAL_TIMER);
		log_arg(&arg_day, "day", day);
		log_arg(&arg_month, "month", month);
		log_arg(&arg_start_hr, "start-hr", start_hr);
		log_arg(&arg_start_min, "start-min", start_min);
		log_arg(&arg_duration_hr, "duration-hr", duration_hr);
		log_arg(&arg_duration_min, "duration-min", duration_min);
		log_arg(&arg_recording_seq, "recording-seq", recording_seq);
		log_digital("digital", &digital);
		break;
	}
	case CEC_MSG_SET_EXT_TIMER: {
		__u8 day;
		__u8 month;
		__u8 start_hr;
		__u8 start_min;
		__u8 duration_hr;
		__u8 duration_min;
		__u8 recording_seq;
		__u8 ext_src_spec;
		__u8 plug;
		__u16 phys_addr;

		cec_ops_set_ext_timer(msg, &day, &month, &start_hr, &start_min, &duration_hr, &duration_min, &recording_seq, &ext_src_spec, &plug, &phys_addr);
		printf("CEC_MSG_SET_EXT_TIMER (0x%02x):\n", CEC_MSG_SET_EXT_TIMER);
		log_arg(&arg_day, "day", day);
		log_arg(&arg_month, "month", month);
		log_arg(&arg_start_hr, "start-hr", start_hr);
		log_arg(&arg_start_min, "start-min", start_min);
		log_arg(&arg_duration_hr, "duration-hr", duration_hr);
		log_arg(&arg_duration_min, "duration-min", duration_min);
		log_arg(&arg_recording_seq, "recording-seq", recording_seq);
		log_arg(&arg_ext_src_spec, "ext-src-spec", ext_src_spec);
		log_arg(&arg_plug, "plug", plug);
		log_arg(&arg_phys_addr, "phys-addr", phys_addr);
		break;
	}
	case CEC_MSG_SET_TIMER_PROGRAM_TITLE: {
		char prog_title[16];

		cec_ops_set_timer_program_title(msg, prog_title);
		printf("CEC_MSG_SET_TIMER_PROGRAM_TITLE (0x%02x):\n", CEC_MSG_SET_TIMER_PROGRAM_TITLE);
		log_arg(&arg_prog_title, "prog-title", prog_title);
		break;
	}
	case CEC_MSG_CEC_VERSION: {
		__u8 cec_version;

		cec_ops_cec_version(msg, &cec_version);
		printf("CEC_MSG_CEC_VERSION (0x%02x):\n", CEC_MSG_CEC_VERSION);
		log_arg(&arg_cec_version, "cec-version", cec_version);
		break;
	}
	case CEC_MSG_GET_CEC_VERSION:
		printf("CEC_MSG_GET_CEC_VERSION (0x%02x)\n", CEC_MSG_GET_CEC_VERSION);
		break;

	case CEC_MSG_REPORT_PHYSICAL_ADDR: {
		__u16 phys_addr;
		__u8 prim_devtype;

		cec_ops_report_physical_addr(msg, &phys_addr, &prim_devtype);
		printf("CEC_MSG_REPORT_PHYSICAL_ADDR (0x%02x):\n", CEC_MSG_REPORT_PHYSICAL_ADDR);
		log_arg(&arg_phys_addr, "phys-addr", phys_addr);
		log_arg(&arg_prim_devtype, "prim-devtype", prim_devtype);
		break;
	}
	case CEC_MSG_GIVE_PHYSICAL_ADDR:
		printf("CEC_MSG_GIVE_PHYSICAL_ADDR (0x%02x)\n", CEC_MSG_GIVE_PHYSICAL_ADDR);
		break;

	case CEC_MSG_SET_MENU_LANGUAGE: {
		char language[16];

		cec_ops_set_menu_language(msg, language);
		printf("CEC_MSG_SET_MENU_LANGUAGE (0x%02x):\n", CEC_MSG_SET_MENU_LANGUAGE);
		log_arg(&arg_language, "language", language);
		break;
	}
	case CEC_MSG_GET_MENU_LANGUAGE:
		printf("CEC_MSG_GET_MENU_LANGUAGE (0x%02x)\n", CEC_MSG_GET_MENU_LANGUAGE);
		break;

	case CEC_MSG_REPORT_FEATURES: {
		__u8 cec_version;
		__u8 all_device_types;
		const __u8 *rc_profile = NULL;
		const __u8 *dev_features = NULL;

		cec_ops_report_features(msg, &cec_version, &all_device_types, &rc_profile, &dev_features);
		printf("CEC_MSG_REPORT_FEATURES (0x%02x):\n", CEC_MSG_REPORT_FEATURES);
		log_arg(&arg_cec_version, "cec-version", cec_version);
		log_arg(&arg_all_device_types, "all-device-types", all_device_types);
		log_features(&arg_rc_profile, "rc-profile", rc_profile);
		log_features(&arg_dev_features, "dev-features", dev_features);
		break;
	}
	case CEC_MSG_GIVE_FEATURES:
		printf("CEC_MSG_GIVE_FEATURES (0x%02x)\n", CEC_MSG_GIVE_FEATURES);
		break;

	case CEC_MSG_DECK_CONTROL: {
		__u8 deck_control_mode;

		cec_ops_deck_control(msg, &deck_control_mode);
		printf("CEC_MSG_DECK_CONTROL (0x%02x):\n", CEC_MSG_DECK_CONTROL);
		log_arg(&arg_deck_control_mode, "deck-control-mode", deck_control_mode);
		break;
	}
	case CEC_MSG_DECK_STATUS: {
		__u8 deck_info;

		cec_ops_deck_status(msg, &deck_info);
		printf("CEC_MSG_DECK_STATUS (0x%02x):\n", CEC_MSG_DECK_STATUS);
		log_arg(&arg_deck_info, "deck-info", deck_info);
		break;
	}
	case CEC_MSG_GIVE_DECK_STATUS: {
		__u8 status_req;

		cec_ops_give_deck_status(msg, &status_req);
		printf("CEC_MSG_GIVE_DECK_STATUS (0x%02x):\n", CEC_MSG_GIVE_DECK_STATUS);
		log_arg(&arg_status_req, "status-req", status_req);
		break;
	}
	case CEC_MSG_PLAY: {
		__u8 play_mode;

		cec_ops_play(msg, &play_mode);
		printf("CEC_MSG_PLAY (0x%02x):\n", CEC_MSG_PLAY);
		log_arg(&arg_play_mode, "play-mode", play_mode);
		break;
	}
	case CEC_MSG_TUNER_DEVICE_STATUS: {
		struct cec_op_tuner_device_info tuner_dev_info = {};

		cec_ops_tuner_device_status(msg, &tuner_dev_info);
		printf("CEC_MSG_TUNER_DEVICE_STATUS (0x%02x):\n", CEC_MSG_TUNER_DEVICE_STATUS);
		log_tuner_dev_info("tuner-dev-info", &tuner_dev_info);
		break;
	}
	case CEC_MSG_GIVE_TUNER_DEVICE_STATUS: {
		__u8 status_req;

		cec_ops_give_tuner_device_status(msg, &status_req);
		printf("CEC_MSG_GIVE_TUNER_DEVICE_STATUS (0x%02x):\n", CEC_MSG_GIVE_TUNER_DEVICE_STATUS);
		log_arg(&arg_status_req, "status-req", status_req);
		break;
	}
	case CEC_MSG_SELECT_ANALOGUE_SERVICE: {
		__u8 ana_bcast_type;
		__u16 ana_freq;
		__u8 bcast_system;

		cec_ops_select_analogue_service(msg, &ana_bcast_type, &ana_freq, &bcast_system);
		printf("CEC_MSG_SELECT_ANALOGUE_SERVICE (0x%02x):\n", CEC_MSG_SELECT_ANALOGUE_SERVICE);
		log_arg(&arg_ana_bcast_type, "ana-bcast-type", ana_bcast_type);
		log_arg(&arg_ana_freq, "ana-freq", ana_freq);
		log_arg(&arg_bcast_system, "bcast-system", bcast_system);
		break;
	}
	case CEC_MSG_SELECT_DIGITAL_SERVICE: {
		struct cec_op_digital_service_id digital = {};

		cec_ops_select_digital_service(msg, &digital);
		printf("CEC_MSG_SELECT_DIGITAL_SERVICE (0x%02x):\n", CEC_MSG_SELECT_DIGITAL_SERVICE);
		log_digital("digital", &digital);
		break;
	}
	case CEC_MSG_TUNER_STEP_DECREMENT:
		printf("CEC_MSG_TUNER_STEP_DECREMENT (0x%02x)\n", CEC_MSG_TUNER_STEP_DECREMENT);
		break;

	case CEC_MSG_TUNER_STEP_INCREMENT:
		printf("CEC_MSG_TUNER_STEP_INCREMENT (0x%02x)\n", CEC_MSG_TUNER_STEP_INCREMENT);
		break;

	case CEC_MSG_DEVICE_VENDOR_ID: {
		__u32 vendor_id;

		cec_ops_device_vendor_id(msg, &vendor_id);
		printf("CEC_MSG_DEVICE_VENDOR_ID (0x%02x):\n", CEC_MSG_DEVICE_VENDOR_ID);
		log_arg(&arg_vendor_id, "vendor-id", vendor_id);
		break;
	}
	case CEC_MSG_GIVE_DEVICE_VENDOR_ID:
		printf("CEC_MSG_GIVE_DEVICE_VENDOR_ID (0x%02x)\n", CEC_MSG_GIVE_DEVICE_VENDOR_ID);
		break;

	case CEC_MSG_VENDOR_REMOTE_BUTTON_UP:
		printf("CEC_MSG_VENDOR_REMOTE_BUTTON_UP (0x%02x)\n", CEC_MSG_VENDOR_REMOTE_BUTTON_UP);
		break;

	case CEC_MSG_SET_OSD_STRING: {
		__u8 disp_ctl;
		char osd[16];

		cec_ops_set_osd_string(msg, &disp_ctl, osd);
		printf("CEC_MSG_SET_OSD_STRING (0x%02x):\n", CEC_MSG_SET_OSD_STRING);
		log_arg(&arg_disp_ctl, "disp-ctl", disp_ctl);
		log_arg(&arg_osd, "osd", osd);
		break;
	}
	case CEC_MSG_SET_OSD_NAME: {
		char name[16];

		cec_ops_set_osd_name(msg, name);
		printf("CEC_MSG_SET_OSD_NAME (0x%02x):\n", CEC_MSG_SET_OSD_NAME);
		log_arg(&arg_name, "name", name);
		break;
	}
	case CEC_MSG_GIVE_OSD_NAME:
		printf("CEC_MSG_GIVE_OSD_NAME (0x%02x)\n", CEC_MSG_GIVE_OSD_NAME);
		break;

	case CEC_MSG_MENU_STATUS: {
		__u8 menu_state;

		cec_ops_menu_status(msg, &menu_state);
		printf("CEC_MSG_MENU_STATUS (0x%02x):\n", CEC_MSG_MENU_STATUS);
		log_arg(&arg_menu_state, "menu-state", menu_state);
		break;
	}
	case CEC_MSG_MENU_REQUEST: {
		__u8 menu_req;

		cec_ops_menu_request(msg, &menu_req);
		printf("CEC_MSG_MENU_REQUEST (0x%02x):\n", CEC_MSG_MENU_REQUEST);
		log_arg(&arg_menu_req, "menu-req", menu_req);
		break;
	}
	case CEC_MSG_USER_CONTROL_PRESSED: {
		struct cec_op_ui_command ui_cmd = {};

		cec_ops_user_control_pressed(msg, &ui_cmd);
		printf("CEC_MSG_USER_CONTROL_PRESSED (0x%02x):\n", CEC_MSG_USER_CONTROL_PRESSED);
		log_ui_command("ui-cmd", &ui_cmd);
		break;
	}
	case CEC_MSG_USER_CONTROL_RELEASED:
		printf("CEC_MSG_USER_CONTROL_RELEASED (0x%02x)\n", CEC_MSG_USER_CONTROL_RELEASED);
		break;

	case CEC_MSG_REPORT_POWER_STATUS: {
		__u8 pwr_state;

		cec_ops_report_power_status(msg, &pwr_state);
		printf("CEC_MSG_REPORT_POWER_STATUS (0x%02x):\n", CEC_MSG_REPORT_POWER_STATUS);
		log_arg(&arg_pwr_state, "pwr-state", pwr_state);
		break;
	}
	case CEC_MSG_GIVE_DEVICE_POWER_STATUS:
		printf("CEC_MSG_GIVE_DEVICE_POWER_STATUS (0x%02x)\n", CEC_MSG_GIVE_DEVICE_POWER_STATUS);
		break;

	case CEC_MSG_FEATURE_ABORT: {
		__u8 abort_msg;
		__u8 reason;

		cec_ops_feature_abort(msg, &abort_msg, &reason);
		printf("CEC_MSG_FEATURE_ABORT (0x%02x):\n", CEC_MSG_FEATURE_ABORT);
		log_arg(&arg_abort_msg, "abort-msg", abort_msg);
		log_arg(&arg_reason, "reason", reason);
		break;
	}
	case CEC_MSG_ABORT:
		printf("CEC_MSG_ABORT (0x%02x)\n", CEC_MSG_ABORT);
		break;

	case CEC_MSG_REPORT_AUDIO_STATUS: {
		__u8 aud_mute_status;
		__u8 aud_vol_status;

		cec_ops_report_audio_status(msg, &aud_mute_status, &aud_vol_status);
		printf("CEC_MSG_REPORT_AUDIO_STATUS (0x%02x):\n", CEC_MSG_REPORT_AUDIO_STATUS);
		log_arg(&arg_aud_mute_status, "aud-mute-status", aud_mute_status);
		log_arg(&arg_aud_vol_status, "aud-vol-status", aud_vol_status);
		break;
	}
	case CEC_MSG_GIVE_AUDIO_STATUS:
		printf("CEC_MSG_GIVE_AUDIO_STATUS (0x%02x)\n", CEC_MSG_GIVE_AUDIO_STATUS);
		break;

	case CEC_MSG_SET_SYSTEM_AUDIO_MODE: {
		__u8 sys_aud_status;

		cec_ops_set_system_audio_mode(msg, &sys_aud_status);
		printf("CEC_MSG_SET_SYSTEM_AUDIO_MODE (0x%02x):\n", CEC_MSG_SET_SYSTEM_AUDIO_MODE);
		log_arg(&arg_sys_aud_status, "sys-aud-status", sys_aud_status);
		break;
	}
	case CEC_MSG_SYSTEM_AUDIO_MODE_REQUEST: {
		__u16 phys_addr;

		cec_ops_system_audio_mode_request(msg, &phys_addr);
		printf("CEC_MSG_SYSTEM_AUDIO_MODE_REQUEST (0x%02x):\n", CEC_MSG_SYSTEM_AUDIO_MODE_REQUEST);
		log_arg(&arg_phys_addr, "phys-addr", phys_addr);
		break;
	}
	case CEC_MSG_SYSTEM_AUDIO_MODE_STATUS: {
		__u8 sys_aud_status;

		cec_ops_system_audio_mode_status(msg, &sys_aud_status);
		printf("CEC_MSG_SYSTEM_AUDIO_MODE_STATUS (0x%02x):\n", CEC_MSG_SYSTEM_AUDIO_MODE_STATUS);
		log_arg(&arg_sys_aud_status, "sys-aud-status", sys_aud_status);
		break;
	}
	case CEC_MSG_GIVE_SYSTEM_AUDIO_MODE_STATUS:
		printf("CEC_MSG_GIVE_SYSTEM_AUDIO_MODE_STATUS (0x%02x)\n", CEC_MSG_GIVE_SYSTEM_AUDIO_MODE_STATUS);
		break;

	case CEC_MSG_REPORT_SHORT_AUDIO_DESCRIPTOR: {
		__u8 num_descriptors;
		__u32 descriptors[4];

		cec_ops_report_short_audio_descriptor(msg, &num_descriptors, descriptors);
		printf("CEC_MSG_REPORT_SHORT_AUDIO_DESCRIPTOR (0x%02x):\n", CEC_MSG_REPORT_SHORT_AUDIO_DESCRIPTOR);
		log_arg(&arg_num_descriptors, "num-descriptors", num_descriptors);
		log_descriptors("descriptors", num_descriptors, descriptors);
		break;
	}
	case CEC_MSG_REQUEST_SHORT_AUDIO_DESCRIPTOR: {
		__u8 num_descriptors;
		__u8 audio_format_id[4];
		__u8 audio_format_code[4];

		cec_ops_request_short_audio_descriptor(msg, &num_descriptors, audio_format_id, audio_format_code);
		printf("CEC_MSG_REQUEST_SHORT_AUDIO_DESCRIPTOR (0x%02x):\n", CEC_MSG_REQUEST_SHORT_AUDIO_DESCRIPTOR);
		log_arg(&arg_num_descriptors, "num-descriptors", num_descriptors);
		log_u8_array("audio-format-id", num_descriptors, audio_format_id);
		log_u8_array("audio-format-code", num_descriptors, audio_format_code);
		break;
	}
	case CEC_MSG_SET_AUDIO_RATE: {
		__u8 audio_rate;

		cec_ops_set_audio_rate(msg, &audio_rate);
		printf("CEC_MSG_SET_AUDIO_RATE (0x%02x):\n", CEC_MSG_SET_AUDIO_RATE);
		log_arg(&arg_audio_rate, "audio-rate", audio_rate);
		break;
	}
	case CEC_MSG_REPORT_ARC_INITIATED:
		printf("CEC_MSG_REPORT_ARC_INITIATED (0x%02x)\n", CEC_MSG_REPORT_ARC_INITIATED);
		break;

	case CEC_MSG_INITIATE_ARC:
		printf("CEC_MSG_INITIATE_ARC (0x%02x)\n", CEC_MSG_INITIATE_ARC);
		break;

	case CEC_MSG_REQUEST_ARC_INITIATION:
		printf("CEC_MSG_REQUEST_ARC_INITIATION (0x%02x)\n", CEC_MSG_REQUEST_ARC_INITIATION);
		break;

	case CEC_MSG_REPORT_ARC_TERMINATED:
		printf("CEC_MSG_REPORT_ARC_TERMINATED (0x%02x)\n", CEC_MSG_REPORT_ARC_TERMINATED);
		break;

	case CEC_MSG_TERMINATE_ARC:
		printf("CEC_MSG_TERMINATE_ARC (0x%02x)\n", CEC_MSG_TERMINATE_ARC);
		break;

	case CEC_MSG_REQUEST_ARC_TERMINATION:
		printf("CEC_MSG_REQUEST_ARC_TERMINATION (0x%02x)\n", CEC_MSG_REQUEST_ARC_TERMINATION);
		break;

	case CEC_MSG_REPORT_CURRENT_LATENCY: {
		__u16 phys_addr;
		__u8 video_latency;
		__u8 low_latency_mode;
		__u8 audio_out_compensated;
		__u8 audio_out_delay;

		cec_ops_report_current_latency(msg, &phys_addr, &video_latency, &low_latency_mode, &audio_out_compensated, &audio_out_delay);
		printf("CEC_MSG_REPORT_CURRENT_LATENCY (0x%02x):\n", CEC_MSG_REPORT_CURRENT_LATENCY);
		log_arg(&arg_phys_addr, "phys-addr", phys_addr);
		log_arg(&arg_video_latency, "video-latency", video_latency);
		log_arg(&arg_low_latency_mode, "low-latency-mode", low_latency_mode);
		log_arg(&arg_audio_out_compensated, "audio-out-compensated", audio_out_compensated);
		log_arg(&arg_audio_out_delay, "audio-out-delay", audio_out_delay);
		break;
	}
	case CEC_MSG_REQUEST_CURRENT_LATENCY: {
		__u16 phys_addr;

		cec_ops_request_current_latency(msg, &phys_addr);
		printf("CEC_MSG_REQUEST_CURRENT_LATENCY (0x%02x):\n", CEC_MSG_REQUEST_CURRENT_LATENCY);
		log_arg(&arg_phys_addr, "phys-addr", phys_addr);
		break;
	}
	case CEC_MSG_CDC_MESSAGE:
	switch (msg->msg[4]) {
	case CEC_MSG_CDC_HEC_INQUIRE_STATE: {
		__u16 phys_addr1;
		__u16 phys_addr2;
		__u16 phys_addr;

		cec_ops_cdc_hec_inquire_state(msg, &phys_addr, &phys_addr1, &phys_addr2);
		printf("CEC_MSG_CDC_HEC_INQUIRE_STATE (0x%02x):\n", CEC_MSG_CDC_HEC_INQUIRE_STATE);
		log_arg(&arg_phys_addr, "phys-addr", phys_addr);
		log_arg(&arg_phys_addr1, "phys-addr1", phys_addr1);
		log_arg(&arg_phys_addr2, "phys-addr2", phys_addr2);
		break;
	}
	case CEC_MSG_CDC_HEC_REPORT_STATE: {
		__u16 target_phys_addr;
		__u8 hec_func_state;
		__u8 host_func_state;
		__u8 enc_func_state;
		__u8 cdc_errcode;
		__u8 has_field;
		__u16 hec_field;
		__u16 phys_addr;

		cec_ops_cdc_hec_report_state(msg, &phys_addr, &target_phys_addr, &hec_func_state, &host_func_state, &enc_func_state, &cdc_errcode, &has_field, &hec_field);
		printf("CEC_MSG_CDC_HEC_REPORT_STATE (0x%02x):\n", CEC_MSG_CDC_HEC_REPORT_STATE);
		log_arg(&arg_phys_addr, "phys-addr", phys_addr);
		log_arg(&arg_target_phys_addr, "target-phys-addr", target_phys_addr);
		log_arg(&arg_hec_func_state, "hec-func-state", hec_func_state);
		log_arg(&arg_host_func_state, "host-func-state", host_func_state);
		log_arg(&arg_enc_func_state, "enc-func-state", enc_func_state);
		log_arg(&arg_cdc_errcode, "cdc-errcode", cdc_errcode);
		log_arg(&arg_has_field, "has-field", has_field);
		log_arg(&arg_hec_field, "hec-field", hec_field);
		break;
	}
	case CEC_MSG_CDC_HEC_SET_STATE: {
		__u16 phys_addr1;
		__u16 phys_addr2;
		__u8 hec_set_state;
		__u16 phys_addr3;
		__u16 phys_addr4;
		__u16 phys_addr5;
		__u16 phys_addr;

		cec_ops_cdc_hec_set_state(msg, &phys_addr, &phys_addr1, &phys_addr2, &hec_set_state, &phys_addr3, &phys_addr4, &phys_addr5);
		printf("CEC_MSG_CDC_HEC_SET_STATE (0x%02x):\n", CEC_MSG_CDC_HEC_SET_STATE);
		log_arg(&arg_phys_addr, "phys-addr", phys_addr);
		log_arg(&arg_phys_addr1, "phys-addr1", phys_addr1);
		log_arg(&arg_phys_addr2, "phys-addr2", phys_addr2);
		log_arg(&arg_hec_set_state, "hec-set-state", hec_set_state);
		log_arg(&arg_phys_addr3, "phys-addr3", phys_addr3);
		log_arg(&arg_phys_addr4, "phys-addr4", phys_addr4);
		log_arg(&arg_phys_addr5, "phys-addr5", phys_addr5);
		break;
	}
	case CEC_MSG_CDC_HEC_SET_STATE_ADJACENT: {
		__u16 phys_addr1;
		__u8 hec_set_state;
		__u16 phys_addr;

		cec_ops_cdc_hec_set_state_adjacent(msg, &phys_addr, &phys_addr1, &hec_set_state);
		printf("CEC_MSG_CDC_HEC_SET_STATE_ADJACENT (0x%02x):\n", CEC_MSG_CDC_HEC_SET_STATE_ADJACENT);
		log_arg(&arg_phys_addr, "phys-addr", phys_addr);
		log_arg(&arg_phys_addr1, "phys-addr1", phys_addr1);
		log_arg(&arg_hec_set_state, "hec-set-state", hec_set_state);
		break;
	}
	case CEC_MSG_CDC_HEC_REQUEST_DEACTIVATION: {
		__u16 phys_addr1;
		__u16 phys_addr2;
		__u16 phys_addr3;
		__u16 phys_addr;

		cec_ops_cdc_hec_request_deactivation(msg, &phys_addr, &phys_addr1, &phys_addr2, &phys_addr3);
		printf("CEC_MSG_CDC_HEC_REQUEST_DEACTIVATION (0x%02x):\n", CEC_MSG_CDC_HEC_REQUEST_DEACTIVATION);
		log_arg(&arg_phys_addr, "phys-addr", phys_addr);
		log_arg(&arg_phys_addr1, "phys-addr1", phys_addr1);
		log_arg(&arg_phys_addr2, "phys-addr2", phys_addr2);
		log_arg(&arg_phys_addr3, "phys-addr3", phys_addr3);
		break;
	}
	case CEC_MSG_CDC_HEC_NOTIFY_ALIVE:
		printf("CEC_MSG_CDC_HEC_NOTIFY_ALIVE (0x%02x)\n", CEC_MSG_CDC_HEC_NOTIFY_ALIVE);
		break;

	case CEC_MSG_CDC_HEC_DISCOVER:
		printf("CEC_MSG_CDC_HEC_DISCOVER (0x%02x)\n", CEC_MSG_CDC_HEC_DISCOVER);
		break;

	case CEC_MSG_CDC_HPD_SET_STATE: {
		__u8 input_port;
		__u8 hpd_state;
		__u16 phys_addr;

		cec_ops_cdc_hpd_set_state(msg, &phys_addr, &input_port, &hpd_state);
		printf("CEC_MSG_CDC_HPD_SET_STATE (0x%02x):\n", CEC_MSG_CDC_HPD_SET_STATE);
		log_arg(&arg_phys_addr, "phys-addr", phys_addr);
		log_arg(&arg_input_port, "input-port", input_port);
		log_arg(&arg_hpd_state, "hpd-state", hpd_state);
		break;
	}
	case CEC_MSG_CDC_HPD_REPORT_STATE: {
		__u8 hpd_state;
		__u8 hpd_error;
		__u16 phys_addr;

		cec_ops_cdc_hpd_report_state(msg, &phys_addr, &hpd_state, &hpd_error);
		printf("CEC_MSG_CDC_HPD_REPORT_STATE (0x%02x):\n", CEC_MSG_CDC_HPD_REPORT_STATE);
		log_arg(&arg_phys_addr, "phys-addr", phys_addr);
		log_arg(&arg_hpd_state, "hpd-state", hpd_state);
		log_arg(&arg_hpd_error, "hpd-error", hpd_error);
		break;
	}
	default:
		log_unknown_msg(msg);
		break;
	}
	break;

	default:
		log_unknown_msg(msg);
		break;
	}

status:
	if ((msg->tx_status && !(msg->tx_status & CEC_TX_STATUS_OK)) ||
	    (msg->rx_status && !(msg->rx_status & (CEC_RX_STATUS_OK | CEC_RX_STATUS_FEATURE_ABORT))))
		printf("\t%s\n", status2s(*msg).c_str());
}
void log_htng_msg(const struct cec_msg *msg)
{
	if ((msg->tx_status && !(msg->tx_status & CEC_TX_STATUS_OK)) ||
	    (msg->rx_status && !(msg->rx_status & (CEC_RX_STATUS_OK | CEC_RX_STATUS_FEATURE_ABORT))))
		printf("\t%s\n", status2s(*msg).c_str());

	if (msg->len < 6)
		return;

	switch (msg->msg[5]) {
	case CEC_MSG_HTNG_TUNER_1PART_CHAN: {
		__u8 htng_tuner_type;
		__u16 chan;

		cec_ops_htng_tuner_1part_chan(msg, &htng_tuner_type, &chan);
		printf("CEC_MSG_HTNG_TUNER_1PART_CHAN (0x%02x):\n", CEC_MSG_HTNG_TUNER_1PART_CHAN);
		log_arg(&arg_htng_tuner_type, "htng-tuner-type", htng_tuner_type);
		log_arg(&arg_chan, "chan", chan);
		break;
	}
	case CEC_MSG_HTNG_TUNER_2PART_CHAN: {
		__u8 htng_tuner_type;
		__u8 major_chan;
		__u16 minor_chan;

		cec_ops_htng_tuner_2part_chan(msg, &htng_tuner_type, &major_chan, &minor_chan);
		printf("CEC_MSG_HTNG_TUNER_2PART_CHAN (0x%02x):\n", CEC_MSG_HTNG_TUNER_2PART_CHAN);
		log_arg(&arg_htng_tuner_type, "htng-tuner-type", htng_tuner_type);
		log_arg(&arg_major_chan, "major-chan", major_chan);
		log_arg(&arg_minor_chan, "minor-chan", minor_chan);
		break;
	}
	case CEC_MSG_HTNG_INPUT_SEL_AV: {
		__u16 input;

		cec_ops_htng_input_sel_av(msg, &input);
		printf("CEC_MSG_HTNG_INPUT_SEL_AV (0x%02x):\n", CEC_MSG_HTNG_INPUT_SEL_AV);
		log_arg(&arg_input, "input", input);
		break;
	}
	case CEC_MSG_HTNG_INPUT_SEL_PC: {
		__u16 input;

		cec_ops_htng_input_sel_pc(msg, &input);
		printf("CEC_MSG_HTNG_INPUT_SEL_PC (0x%02x):\n", CEC_MSG_HTNG_INPUT_SEL_PC);
		log_arg(&arg_input, "input", input);
		break;
	}
	case CEC_MSG_HTNG_INPUT_SEL_HDMI: {
		__u16 input;

		cec_ops_htng_input_sel_hdmi(msg, &input);
		printf("CEC_MSG_HTNG_INPUT_SEL_HDMI (0x%02x):\n", CEC_MSG_HTNG_INPUT_SEL_HDMI);
		log_arg(&arg_input, "input", input);
		break;
	}
	case CEC_MSG_HTNG_INPUT_SEL_COMPONENT: {
		__u16 input;

		cec_ops_htng_input_sel_component(msg, &input);
		printf("CEC_MSG_HTNG_INPUT_SEL_COMPONENT (0x%02x):\n", CEC_MSG_HTNG_INPUT_SEL_COMPONENT);
		log_arg(&arg_input, "input", input);
		break;
	}
	case CEC_MSG_HTNG_INPUT_SEL_DVI: {
		__u16 input;

		cec_ops_htng_input_sel_dvi(msg, &input);
		printf("CEC_MSG_HTNG_INPUT_SEL_DVI (0x%02x):\n", CEC_MSG_HTNG_INPUT_SEL_DVI);
		log_arg(&arg_input, "input", input);
		break;
	}
	case CEC_MSG_HTNG_INPUT_SEL_DP: {
		__u16 input;

		cec_ops_htng_input_sel_dp(msg, &input);
		printf("CEC_MSG_HTNG_INPUT_SEL_DP (0x%02x):\n", CEC_MSG_HTNG_INPUT_SEL_DP);
		log_arg(&arg_input, "input", input);
		break;
	}
	case CEC_MSG_HTNG_INPUT_SEL_USB: {
		__u16 input;

		cec_ops_htng_input_sel_usb(msg, &input);
		printf("CEC_MSG_HTNG_INPUT_SEL_USB (0x%02x):\n", CEC_MSG_HTNG_INPUT_SEL_USB);
		log_arg(&arg_input, "input", input);
		break;
	}
	case CEC_MSG_HTNG_SET_DEF_PWR_ON_INPUT_SRC: {
		__u8 htng_input_src;
		__u8 htng_tuner_type;
		__u8 major;
		__u16 input;

		cec_ops_htng_set_def_pwr_on_input_src(msg, &htng_input_src, &htng_tuner_type, &major, &input);
		printf("CEC_MSG_HTNG_SET_DEF_PWR_ON_INPUT_SRC (0x%02x):\n", CEC_MSG_HTNG_SET_DEF_PWR_ON_INPUT_SRC);
		log_arg(&arg_htng_input_src, "htng-input-src", htng_input_src);
		log_arg(&arg_htng_tuner_type, "htng-tuner-type", htng_tuner_type);
		log_arg(&arg_major, "major", major);
		log_arg(&arg_input, "input", input);
		break;
	}
	case CEC_MSG_HTNG_SET_TV_SPEAKERS: {
		__u8 on;

		cec_ops_htng_set_tv_speakers(msg, &on);
		printf("CEC_MSG_HTNG_SET_TV_SPEAKERS (0x%02x):\n", CEC_MSG_HTNG_SET_TV_SPEAKERS);
		log_arg(&arg_on, "on", on);
		break;
	}
	case CEC_MSG_HTNG_SET_DIG_AUDIO: {
		__u8 on;

		cec_ops_htng_set_dig_audio(msg, &on);
		printf("CEC_MSG_HTNG_SET_DIG_AUDIO (0x%02x):\n", CEC_MSG_HTNG_SET_DIG_AUDIO);
		log_arg(&arg_on, "on", on);
		break;
	}
	case CEC_MSG_HTNG_SET_ANA_AUDIO: {
		__u8 on;

		cec_ops_htng_set_ana_audio(msg, &on);
		printf("CEC_MSG_HTNG_SET_ANA_AUDIO (0x%02x):\n", CEC_MSG_HTNG_SET_ANA_AUDIO);
		log_arg(&arg_on, "on", on);
		break;
	}
	case CEC_MSG_HTNG_SET_DEF_PWR_ON_VOL: {
		__u8 vol;

		cec_ops_htng_set_def_pwr_on_vol(msg, &vol);
		printf("CEC_MSG_HTNG_SET_DEF_PWR_ON_VOL (0x%02x):\n", CEC_MSG_HTNG_SET_DEF_PWR_ON_VOL);
		log_arg(&arg_vol, "vol", vol);
		break;
	}
	case CEC_MSG_HTNG_SET_MAX_VOL: {
		__u8 vol;

		cec_ops_htng_set_max_vol(msg, &vol);
		printf("CEC_MSG_HTNG_SET_MAX_VOL (0x%02x):\n", CEC_MSG_HTNG_SET_MAX_VOL);
		log_arg(&arg_vol, "vol", vol);
		break;
	}
	case CEC_MSG_HTNG_SET_MIN_VOL: {
		__u8 vol;

		cec_ops_htng_set_min_vol(msg, &vol);
		printf("CEC_MSG_HTNG_SET_MIN_VOL (0x%02x):\n", CEC_MSG_HTNG_SET_MIN_VOL);
		log_arg(&arg_vol, "vol", vol);
		break;
	}
	case CEC_MSG_HTNG_SET_BLUE_SCREEN: {
		__u8 blue;

		cec_ops_htng_set_blue_screen(msg, &blue);
		printf("CEC_MSG_HTNG_SET_BLUE_SCREEN (0x%02x):\n", CEC_MSG_HTNG_SET_BLUE_SCREEN);
		log_arg(&arg_blue, "blue", blue);
		break;
	}
	case CEC_MSG_HTNG_SET_BRIGHTNESS: {
		__u8 brightness;

		cec_ops_htng_set_brightness(msg, &brightness);
		printf("CEC_MSG_HTNG_SET_BRIGHTNESS (0x%02x):\n", CEC_MSG_HTNG_SET_BRIGHTNESS);
		log_arg(&arg_brightness, "brightness", brightness);
		break;
	}
	case CEC_MSG_HTNG_SET_COLOR: {
		__u8 color;

		cec_ops_htng_set_color(msg, &color);
		printf("CEC_MSG_HTNG_SET_COLOR (0x%02x):\n", CEC_MSG_HTNG_SET_COLOR);
		log_arg(&arg_color, "color", color);
		break;
	}
	case CEC_MSG_HTNG_SET_CONTRAST: {
		__u8 contrast;

		cec_ops_htng_set_contrast(msg, &contrast);
		printf("CEC_MSG_HTNG_SET_CONTRAST (0x%02x):\n", CEC_MSG_HTNG_SET_CONTRAST);
		log_arg(&arg_contrast, "contrast", contrast);
		break;
	}
	case CEC_MSG_HTNG_SET_SHARPNESS: {
		__u8 sharpness;

		cec_ops_htng_set_sharpness(msg, &sharpness);
		printf("CEC_MSG_HTNG_SET_SHARPNESS (0x%02x):\n", CEC_MSG_HTNG_SET_SHARPNESS);
		log_arg(&arg_sharpness, "sharpness", sharpness);
		break;
	}
	case CEC_MSG_HTNG_SET_HUE: {
		__u8 hue;

		cec_ops_htng_set_hue(msg, &hue);
		printf("CEC_MSG_HTNG_SET_HUE (0x%02x):\n", CEC_MSG_HTNG_SET_HUE);
		log_arg(&arg_hue, "hue", hue);
		break;
	}
	case CEC_MSG_HTNG_SET_LED_BACKLIGHT: {
		__u8 led_backlight;

		cec_ops_htng_set_led_backlight(msg, &led_backlight);
		printf("CEC_MSG_HTNG_SET_LED_BACKLIGHT (0x%02x):\n", CEC_MSG_HTNG_SET_LED_BACKLIGHT);
		log_arg(&arg_led_backlight, "led-backlight", led_backlight);
		break;
	}
	case CEC_MSG_HTNG_SET_TV_OSD_CONTROL: {
		__u8 on;

		cec_ops_htng_set_tv_osd_control(msg, &on);
		printf("CEC_MSG_HTNG_SET_TV_OSD_CONTROL (0x%02x):\n", CEC_MSG_HTNG_SET_TV_OSD_CONTROL);
		log_arg(&arg_on, "on", on);
		break;
	}
	case CEC_MSG_HTNG_SET_AUDIO_ONLY_DISPLAY: {
		__u8 on;

		cec_ops_htng_set_audio_only_display(msg, &on);
		printf("CEC_MSG_HTNG_SET_AUDIO_ONLY_DISPLAY (0x%02x):\n", CEC_MSG_HTNG_SET_AUDIO_ONLY_DISPLAY);
		log_arg(&arg_on, "on", on);
		break;
	}
	case CEC_MSG_HTNG_SET_DATE: {
		char date[16];

		cec_ops_htng_set_date(msg, date);
		printf("CEC_MSG_HTNG_SET_DATE (0x%02x):\n", CEC_MSG_HTNG_SET_DATE);
		log_arg(&arg_date, "date", date);
		break;
	}
	case CEC_MSG_HTNG_SET_DATE_FORMAT: {
		__u8 ddmm;

		cec_ops_htng_set_date_format(msg, &ddmm);
		printf("CEC_MSG_HTNG_SET_DATE_FORMAT (0x%02x):\n", CEC_MSG_HTNG_SET_DATE_FORMAT);
		log_arg(&arg_ddmm, "ddmm", ddmm);
		break;
	}
	case CEC_MSG_HTNG_SET_TIME: {
		char time[16];

		cec_ops_htng_set_time(msg, time);
		printf("CEC_MSG_HTNG_SET_TIME (0x%02x):\n", CEC_MSG_HTNG_SET_TIME);
		log_arg(&arg_time, "time", time);
		break;
	}
	case CEC_MSG_HTNG_SET_CLK_BRIGHTNESS_STANDBY: {
		__u8 brightness;

		cec_ops_htng_set_clk_brightness_standby(msg, &brightness);
		printf("CEC_MSG_HTNG_SET_CLK_BRIGHTNESS_STANDBY (0x%02x):\n", CEC_MSG_HTNG_SET_CLK_BRIGHTNESS_STANDBY);
		log_arg(&arg_brightness, "brightness", brightness);
		break;
	}
	case CEC_MSG_HTNG_SET_CLK_BRIGHTNESS_ON: {
		__u8 brightness;

		cec_ops_htng_set_clk_brightness_on(msg, &brightness);
		printf("CEC_MSG_HTNG_SET_CLK_BRIGHTNESS_ON (0x%02x):\n", CEC_MSG_HTNG_SET_CLK_BRIGHTNESS_ON);
		log_arg(&arg_brightness, "brightness", brightness);
		break;
	}
	case CEC_MSG_HTNG_LED_CONTROL: {
		__u8 htng_led_control;

		cec_ops_htng_led_control(msg, &htng_led_control);
		printf("CEC_MSG_HTNG_LED_CONTROL (0x%02x):\n", CEC_MSG_HTNG_LED_CONTROL);
		log_arg(&arg_htng_led_control, "htng-led-control", htng_led_control);
		break;
	}
	case CEC_MSG_HTNG_LOCK_TV_PWR_BUTTON: {
		__u8 on;

		cec_ops_htng_lock_tv_pwr_button(msg, &on);
		printf("CEC_MSG_HTNG_LOCK_TV_PWR_BUTTON (0x%02x):\n", CEC_MSG_HTNG_LOCK_TV_PWR_BUTTON);
		log_arg(&arg_on, "on", on);
		break;
	}
	case CEC_MSG_HTNG_LOCK_TV_VOL_BUTTONS: {
		__u8 on;

		cec_ops_htng_lock_tv_vol_buttons(msg, &on);
		printf("CEC_MSG_HTNG_LOCK_TV_VOL_BUTTONS (0x%02x):\n", CEC_MSG_HTNG_LOCK_TV_VOL_BUTTONS);
		log_arg(&arg_on, "on", on);
		break;
	}
	case CEC_MSG_HTNG_LOCK_TV_CHAN_BUTTONS: {
		__u8 on;

		cec_ops_htng_lock_tv_chan_buttons(msg, &on);
		printf("CEC_MSG_HTNG_LOCK_TV_CHAN_BUTTONS (0x%02x):\n", CEC_MSG_HTNG_LOCK_TV_CHAN_BUTTONS);
		log_arg(&arg_on, "on", on);
		break;
	}
	case CEC_MSG_HTNG_LOCK_TV_INPUT_BUTTONS: {
		__u8 on;

		cec_ops_htng_lock_tv_input_buttons(msg, &on);
		printf("CEC_MSG_HTNG_LOCK_TV_INPUT_BUTTONS (0x%02x):\n", CEC_MSG_HTNG_LOCK_TV_INPUT_BUTTONS);
		log_arg(&arg_on, "on", on);
		break;
	}
	case CEC_MSG_HTNG_LOCK_TV_OTHER_BUTTONS: {
		__u8 on;

		cec_ops_htng_lock_tv_other_buttons(msg, &on);
		printf("CEC_MSG_HTNG_LOCK_TV_OTHER_BUTTONS (0x%02x):\n", CEC_MSG_HTNG_LOCK_TV_OTHER_BUTTONS);
		log_arg(&arg_on, "on", on);
		break;
	}
	case CEC_MSG_HTNG_LOCK_EVERYTHING: {
		__u8 on;

		cec_ops_htng_lock_everything(msg, &on);
		printf("CEC_MSG_HTNG_LOCK_EVERYTHING (0x%02x):\n", CEC_MSG_HTNG_LOCK_EVERYTHING);
		log_arg(&arg_on, "on", on);
		break;
	}
	case CEC_MSG_HTNG_LOCK_EVERYTHING_BUT_PWR: {
		__u8 on;

		cec_ops_htng_lock_everything_but_pwr(msg, &on);
		printf("CEC_MSG_HTNG_LOCK_EVERYTHING_BUT_PWR (0x%02x):\n", CEC_MSG_HTNG_LOCK_EVERYTHING_BUT_PWR);
		log_arg(&arg_on, "on", on);
		break;
	}
	case CEC_MSG_HTNG_HOTEL_MODE: {
		__u8 on;
		__u8 options;

		cec_ops_htng_hotel_mode(msg, &on, &options);
		printf("CEC_MSG_HTNG_HOTEL_MODE (0x%02x):\n", CEC_MSG_HTNG_HOTEL_MODE);
		log_arg(&arg_on, "on", on);
		log_arg(&arg_options, "options", options);
		break;
	}
	case CEC_MSG_HTNG_SET_PWR_SAVING_PROFILE: {
		__u8 on;
		__u8 val;

		cec_ops_htng_set_pwr_saving_profile(msg, &on, &val);
		printf("CEC_MSG_HTNG_SET_PWR_SAVING_PROFILE (0x%02x):\n", CEC_MSG_HTNG_SET_PWR_SAVING_PROFILE);
		log_arg(&arg_on, "on", on);
		log_arg(&arg_val, "val", val);
		break;
	}
	case CEC_MSG_HTNG_SET_SLEEP_TIMER: {
		__u8 minutes;

		cec_ops_htng_set_sleep_timer(msg, &minutes);
		printf("CEC_MSG_HTNG_SET_SLEEP_TIMER (0x%02x):\n", CEC_MSG_HTNG_SET_SLEEP_TIMER);
		log_arg(&arg_minutes, "minutes", minutes);
		break;
	}
	case CEC_MSG_HTNG_SET_WAKEUP_TIME: {
		char time[16];

		cec_ops_htng_set_wakeup_time(msg, time);
		printf("CEC_MSG_HTNG_SET_WAKEUP_TIME (0x%02x):\n", CEC_MSG_HTNG_SET_WAKEUP_TIME);
		log_arg(&arg_time, "time", time);
		break;
	}
	case CEC_MSG_HTNG_SET_AUTO_OFF_TIME: {
		char time[16];

		cec_ops_htng_set_auto_off_time(msg, time);
		printf("CEC_MSG_HTNG_SET_AUTO_OFF_TIME (0x%02x):\n", CEC_MSG_HTNG_SET_AUTO_OFF_TIME);
		log_arg(&arg_time, "time", time);
		break;
	}
	case CEC_MSG_HTNG_SET_WAKEUP_SRC: {
		__u8 htng_input_src;
		__u8 htng_tuner_type;
		__u8 major;
		__u16 input;

		cec_ops_htng_set_wakeup_src(msg, &htng_input_src, &htng_tuner_type, &major, &input);
		printf("CEC_MSG_HTNG_SET_WAKEUP_SRC (0x%02x):\n", CEC_MSG_HTNG_SET_WAKEUP_SRC);
		log_arg(&arg_htng_input_src, "htng-input-src", htng_input_src);
		log_arg(&arg_htng_tuner_type, "htng-tuner-type", htng_tuner_type);
		log_arg(&arg_major, "major", major);
		log_arg(&arg_input, "input", input);
		break;
	}
	case CEC_MSG_HTNG_SET_INIT_WAKEUP_VOL: {
		__u8 vol;
		__u8 minutes;

		cec_ops_htng_set_init_wakeup_vol(msg, &vol, &minutes);
		printf("CEC_MSG_HTNG_SET_INIT_WAKEUP_VOL (0x%02x):\n", CEC_MSG_HTNG_SET_INIT_WAKEUP_VOL);
		log_arg(&arg_vol, "vol", vol);
		log_arg(&arg_minutes, "minutes", minutes);
		break;
	}
	case CEC_MSG_HTNG_CLR_ALL_SLEEP_WAKE:
		printf("CEC_MSG_HTNG_CLR_ALL_SLEEP_WAKE (0x%02x)\n", CEC_MSG_HTNG_CLR_ALL_SLEEP_WAKE);
		break;

	case CEC_MSG_HTNG_GLOBAL_DIRECT_TUNE_FREQ: {
		__u8 htng_chan_type;
		__u8 htng_prog_type;
		__u8 htng_system_type;
		__u16 freq;
		__u16 service_id;
		__u8 htng_mod_type;
		__u8 htng_symbol_rate;
		__u16 symbol_rate;

		cec_ops_htng_global_direct_tune_freq(msg, &htng_chan_type, &htng_prog_type, &htng_system_type, &freq, &service_id, &htng_mod_type, &htng_symbol_rate, &symbol_rate);
		printf("CEC_MSG_HTNG_GLOBAL_DIRECT_TUNE_FREQ (0x%02x):\n", CEC_MSG_HTNG_GLOBAL_DIRECT_TUNE_FREQ);
		log_arg(&arg_htng_chan_type, "htng-chan-type", htng_chan_type);
		log_arg(&arg_htng_prog_type, "htng-prog-type", htng_prog_type);
		log_arg(&arg_htng_system_type, "htng-system-type", htng_system_type);
		log_arg(&arg_freq, "freq", freq);
		log_arg(&arg_service_id, "service-id", service_id);
		log_arg(&arg_htng_mod_type, "htng-mod-type", htng_mod_type);
		log_arg(&arg_htng_symbol_rate, "htng-symbol-rate", htng_symbol_rate);
		log_arg(&arg_symbol_rate, "symbol-rate", symbol_rate);
		break;
	}
	case CEC_MSG_HTNG_GLOBAL_DIRECT_TUNE_CHAN: {
		__u8 htng_chan_type;
		__u8 htng_prog_type;
		__u16 chan;

		cec_ops_htng_global_direct_tune_chan(msg, &htng_chan_type, &htng_prog_type, &chan);
		printf("CEC_MSG_HTNG_GLOBAL_DIRECT_TUNE_CHAN (0x%02x):\n", CEC_MSG_HTNG_GLOBAL_DIRECT_TUNE_CHAN);
		log_arg(&arg_htng_chan_type, "htng-chan-type", htng_chan_type);
		log_arg(&arg_htng_prog_type, "htng-prog-type", htng_prog_type);
		log_arg(&arg_chan, "chan", chan);
		break;
	}
	case CEC_MSG_HTNG_GLOBAL_DIRECT_TUNE_EXT_FREQ: {
		__u8 htng_ext_chan_type;
		__u8 htng_prog_type;
		__u8 htng_system_type;
		__u16 freq;
		__u16 service_id;
		__u8 htng_mod_type;
		__u8 htng_onid;
		__u16 onid;
		__u8 htng_nid;
		__u16 nid;
		__u8 htng_tsid_plp;
		__u16 tsid_plp;
		__u8 htng_symbol_rate;
		__u16 symbol_rate;

		cec_ops_htng_global_direct_tune_ext_freq(msg, &htng_ext_chan_type, &htng_prog_type, &htng_system_type, &freq, &service_id, &htng_mod_type, &htng_onid, &onid, &htng_nid, &nid, &htng_tsid_plp, &tsid_plp, &htng_symbol_rate, &symbol_rate);
		printf("CEC_MSG_HTNG_GLOBAL_DIRECT_TUNE_EXT_FREQ (0x%02x):\n", CEC_MSG_HTNG_GLOBAL_DIRECT_TUNE_EXT_FREQ);
		log_arg(&arg_htng_ext_chan_type, "htng-ext-chan-type", htng_ext_chan_type);
		log_arg(&arg_htng_prog_type, "htng-prog-type", htng_prog_type);
		log_arg(&arg_htng_system_type, "htng-system-type", htng_system_type);
		log_arg(&arg_freq, "freq", freq);
		log_arg(&arg_service_id, "service-id", service_id);
		log_arg(&arg_htng_mod_type, "htng-mod-type", htng_mod_type);
		log_arg(&arg_htng_onid, "htng-onid", htng_onid);
		log_arg(&arg_onid, "onid", onid);
		log_arg(&arg_htng_nid, "htng-nid", htng_nid);
		log_arg(&arg_nid, "nid", nid);
		log_arg(&arg_htng_tsid_plp, "htng-tsid-plp", htng_tsid_plp);
		log_arg(&arg_tsid_plp, "tsid-plp", tsid_plp);
		log_arg(&arg_htng_symbol_rate, "htng-symbol-rate", htng_symbol_rate);
		log_arg(&arg_symbol_rate, "symbol-rate", symbol_rate);
		break;
	}
	default:
		log_htng_unknown_msg(msg);
		break;
	}
}
