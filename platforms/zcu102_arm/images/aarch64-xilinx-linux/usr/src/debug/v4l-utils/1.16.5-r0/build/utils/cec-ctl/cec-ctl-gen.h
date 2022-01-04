static const char *abort_usage =
	"  --feature-abort abort-msg=<val>,reason=<val>\n"
	"                                  'reason' can have these values:\n"
	"                                      unrecognized-op (" xstr(CEC_OP_ABORT_UNRECOGNIZED_OP) ")\n"
	"                                      incorrect-mode (" xstr(CEC_OP_ABORT_INCORRECT_MODE) ")\n"
	"                                      no-source (" xstr(CEC_OP_ABORT_NO_SOURCE) ")\n"
	"                                      invalid-op (" xstr(CEC_OP_ABORT_INVALID_OP) ")\n"
	"                                      refused (" xstr(CEC_OP_ABORT_REFUSED) ")\n"
	"                                      undetermined (" xstr(CEC_OP_ABORT_UNDETERMINED) ")\n"
	"                                  Send FEATURE_ABORT message (" xstr(CEC_MSG_FEATURE_ABORT) ")\n"
	"  --abort                         Send ABORT message (" xstr(CEC_MSG_ABORT) ")\n";

static const char *audio_rate_control_usage =
	"  --set-audio-rate audio-rate=<val>\n"
	"                                  'audio-rate' can have these values:\n"
	"                                      off (" xstr(CEC_OP_AUD_RATE_OFF) ")\n"
	"                                      wide-std (" xstr(CEC_OP_AUD_RATE_WIDE_STD) ")\n"
	"                                      wide-fast (" xstr(CEC_OP_AUD_RATE_WIDE_FAST) ")\n"
	"                                      wide-slow (" xstr(CEC_OP_AUD_RATE_WIDE_SLOW) ")\n"
	"                                      narrow-std (" xstr(CEC_OP_AUD_RATE_NARROW_STD) ")\n"
	"                                      narrow-fast (" xstr(CEC_OP_AUD_RATE_NARROW_FAST) ")\n"
	"                                      narrow-slow (" xstr(CEC_OP_AUD_RATE_NARROW_SLOW) ")\n"
	"                                  Send SET_AUDIO_RATE message (" xstr(CEC_MSG_SET_AUDIO_RATE) ")\n";

static const char *audio_return_channel_control_usage =
	"  --report-arc-initiated          Send REPORT_ARC_INITIATED message (" xstr(CEC_MSG_REPORT_ARC_INITIATED) ")\n"
	"  --initiate-arc                  Send INITIATE_ARC message (" xstr(CEC_MSG_INITIATE_ARC) ")\n"
	"  --request-arc-initiation        Send REQUEST_ARC_INITIATION message (" xstr(CEC_MSG_REQUEST_ARC_INITIATION) ")\n"
	"  --report-arc-terminated         Send REPORT_ARC_TERMINATED message (" xstr(CEC_MSG_REPORT_ARC_TERMINATED) ")\n"
	"  --terminate-arc                 Send TERMINATE_ARC message (" xstr(CEC_MSG_TERMINATE_ARC) ")\n"
	"  --request-arc-termination       Send REQUEST_ARC_TERMINATION message (" xstr(CEC_MSG_REQUEST_ARC_TERMINATION) ")\n";

static const char *capability_discovery_and_control_usage =
	"  --cdc-hec-inquire-state phys-addr1=<val>,phys-addr2=<val>\n"
	"                                  Send CDC_HEC_INQUIRE_STATE message (" xstr(CEC_MSG_CDC_HEC_INQUIRE_STATE) ", bcast)\n"
	"  --cdc-hec-report-state target-phys-addr=<val>,hec-func-state=<val>,host-func-state=<val>,enc-func-state=<val>,cdc-errcode=<val>,has-field=<val>,hec-field=<val>\n"
	"                                  'hec-func-state' can have these values:\n"
	"                                      not-supported (" xstr(CEC_OP_HEC_FUNC_STATE_NOT_SUPPORTED) ")\n"
	"                                      inactive (" xstr(CEC_OP_HEC_FUNC_STATE_INACTIVE) ")\n"
	"                                      active (" xstr(CEC_OP_HEC_FUNC_STATE_ACTIVE) ")\n"
	"                                      activation-field (" xstr(CEC_OP_HEC_FUNC_STATE_ACTIVATION_FIELD) ")\n"
	"                                  'host-func-state' can have these values:\n"
	"                                      not-supported (" xstr(CEC_OP_HOST_FUNC_STATE_NOT_SUPPORTED) ")\n"
	"                                      inactive (" xstr(CEC_OP_HOST_FUNC_STATE_INACTIVE) ")\n"
	"                                      active (" xstr(CEC_OP_HOST_FUNC_STATE_ACTIVE) ")\n"
	"                                  'enc-func-state' can have these values:\n"
	"                                      not-supported (" xstr(CEC_OP_ENC_FUNC_STATE_EXT_CON_NOT_SUPPORTED) ")\n"
	"                                      inactive (" xstr(CEC_OP_ENC_FUNC_STATE_EXT_CON_INACTIVE) ")\n"
	"                                      active (" xstr(CEC_OP_ENC_FUNC_STATE_EXT_CON_ACTIVE) ")\n"
	"                                  'cdc-errcode' can have these values:\n"
	"                                      none (" xstr(CEC_OP_CDC_ERROR_CODE_NONE) ")\n"
	"                                      cap-unsupported (" xstr(CEC_OP_CDC_ERROR_CODE_CAP_UNSUPPORTED) ")\n"
	"                                      wrong-state (" xstr(CEC_OP_CDC_ERROR_CODE_WRONG_STATE) ")\n"
	"                                      other (" xstr(CEC_OP_CDC_ERROR_CODE_OTHER) ")\n"
	"                                  Send CDC_HEC_REPORT_STATE message (" xstr(CEC_MSG_CDC_HEC_REPORT_STATE) ", bcast)\n"
	"  --cdc-hec-set-state phys-addr1=<val>,phys-addr2=<val>,hec-set-state=<val>,phys-addr3=<val>,phys-addr4=<val>,phys-addr5=<val>\n"
	"                                  'hec-set-state' can have these values:\n"
	"                                      deactivate (" xstr(CEC_OP_HEC_SET_STATE_DEACTIVATE) ")\n"
	"                                      activate (" xstr(CEC_OP_HEC_SET_STATE_ACTIVATE) ")\n"
	"                                  Send CDC_HEC_SET_STATE message (" xstr(CEC_MSG_CDC_HEC_SET_STATE) ", bcast)\n"
	"  --cdc-hec-set-state-adjacent phys-addr1=<val>,hec-set-state=<val>\n"
	"                                  'hec-set-state' can have these values:\n"
	"                                      deactivate (" xstr(CEC_OP_HEC_SET_STATE_DEACTIVATE) ")\n"
	"                                      activate (" xstr(CEC_OP_HEC_SET_STATE_ACTIVATE) ")\n"
	"                                  Send CDC_HEC_SET_STATE_ADJACENT message (" xstr(CEC_MSG_CDC_HEC_SET_STATE_ADJACENT) ", bcast)\n"
	"  --cdc-hec-request-deactivation phys-addr1=<val>,phys-addr2=<val>,phys-addr3=<val>\n"
	"                                  Send CDC_HEC_REQUEST_DEACTIVATION message (" xstr(CEC_MSG_CDC_HEC_REQUEST_DEACTIVATION) ", bcast)\n"
	"  --cdc-hec-notify-alive          Send CDC_HEC_NOTIFY_ALIVE message (" xstr(CEC_MSG_CDC_HEC_NOTIFY_ALIVE) ", bcast)\n"
	"  --cdc-hec-discover              Send CDC_HEC_DISCOVER message (" xstr(CEC_MSG_CDC_HEC_DISCOVER) ", bcast)\n"
	"  --cdc-hpd-set-state input-port=<val>,hpd-state=<val>\n"
	"                                  'hpd-state' can have these values:\n"
	"                                      cp-edid-disable (" xstr(CEC_OP_HPD_STATE_CP_EDID_DISABLE) ")\n"
	"                                      cp-edid-enable (" xstr(CEC_OP_HPD_STATE_CP_EDID_ENABLE) ")\n"
	"                                      cp-edid-disable-enable (" xstr(CEC_OP_HPD_STATE_CP_EDID_DISABLE_ENABLE) ")\n"
	"                                      edid-disable (" xstr(CEC_OP_HPD_STATE_EDID_DISABLE) ")\n"
	"                                      edid-enable (" xstr(CEC_OP_HPD_STATE_EDID_ENABLE) ")\n"
	"                                      edid-disable-enable (" xstr(CEC_OP_HPD_STATE_EDID_DISABLE_ENABLE) ")\n"
	"                                  Send CDC_HPD_SET_STATE message (" xstr(CEC_MSG_CDC_HPD_SET_STATE) ", bcast)\n"
	"  --cdc-hpd-report-state hpd-state=<val>,hpd-error=<val>\n"
	"                                  'hpd-state' can have these values:\n"
	"                                      cp-edid-disable (" xstr(CEC_OP_HPD_STATE_CP_EDID_DISABLE) ")\n"
	"                                      cp-edid-enable (" xstr(CEC_OP_HPD_STATE_CP_EDID_ENABLE) ")\n"
	"                                      cp-edid-disable-enable (" xstr(CEC_OP_HPD_STATE_CP_EDID_DISABLE_ENABLE) ")\n"
	"                                      edid-disable (" xstr(CEC_OP_HPD_STATE_EDID_DISABLE) ")\n"
	"                                      edid-enable (" xstr(CEC_OP_HPD_STATE_EDID_ENABLE) ")\n"
	"                                      edid-disable-enable (" xstr(CEC_OP_HPD_STATE_EDID_DISABLE_ENABLE) ")\n"
	"                                  'hpd-error' can have these values:\n"
	"                                      none (" xstr(CEC_OP_HPD_ERROR_NONE) ")\n"
	"                                      initiator-not-capable (" xstr(CEC_OP_HPD_ERROR_INITIATOR_NOT_CAPABLE) ")\n"
	"                                      initiator-wrong-state (" xstr(CEC_OP_HPD_ERROR_INITIATOR_WRONG_STATE) ")\n"
	"                                      other (" xstr(CEC_OP_HPD_ERROR_OTHER) ")\n"
	"                                      none-no-video (" xstr(CEC_OP_HPD_ERROR_NONE_NO_VIDEO) ")\n"
	"                                  Send CDC_HPD_REPORT_STATE message (" xstr(CEC_MSG_CDC_HPD_REPORT_STATE) ", bcast)\n";

static const char *deck_control_usage =
	"  --deck-control deck-control-mode=<val>\n"
	"                                  'deck-control-mode' can have these values:\n"
	"                                      skip-fwd (" xstr(CEC_OP_DECK_CTL_MODE_SKIP_FWD) ")\n"
	"                                      skip-rev (" xstr(CEC_OP_DECK_CTL_MODE_SKIP_REV) ")\n"
	"                                      stop (" xstr(CEC_OP_DECK_CTL_MODE_STOP) ")\n"
	"                                      eject (" xstr(CEC_OP_DECK_CTL_MODE_EJECT) ")\n"
	"                                  Send DECK_CONTROL message (" xstr(CEC_MSG_DECK_CONTROL) ")\n"
	"  --deck-status deck-info=<val>\n"
	"                                  'deck-info' can have these values:\n"
	"                                      play (" xstr(CEC_OP_DECK_INFO_PLAY) ")\n"
	"                                      record (" xstr(CEC_OP_DECK_INFO_RECORD) ")\n"
	"                                      play-rev (" xstr(CEC_OP_DECK_INFO_PLAY_REV) ")\n"
	"                                      still (" xstr(CEC_OP_DECK_INFO_STILL) ")\n"
	"                                      slow (" xstr(CEC_OP_DECK_INFO_SLOW) ")\n"
	"                                      slow-rev (" xstr(CEC_OP_DECK_INFO_SLOW_REV) ")\n"
	"                                      fast-fwd (" xstr(CEC_OP_DECK_INFO_FAST_FWD) ")\n"
	"                                      fast-rev (" xstr(CEC_OP_DECK_INFO_FAST_REV) ")\n"
	"                                      no-media (" xstr(CEC_OP_DECK_INFO_NO_MEDIA) ")\n"
	"                                      stop (" xstr(CEC_OP_DECK_INFO_STOP) ")\n"
	"                                      skip-fwd (" xstr(CEC_OP_DECK_INFO_SKIP_FWD) ")\n"
	"                                      skip-rev (" xstr(CEC_OP_DECK_INFO_SKIP_REV) ")\n"
	"                                      index-search-fwd (" xstr(CEC_OP_DECK_INFO_INDEX_SEARCH_FWD) ")\n"
	"                                      index-search-rev (" xstr(CEC_OP_DECK_INFO_INDEX_SEARCH_REV) ")\n"
	"                                      other (" xstr(CEC_OP_DECK_INFO_OTHER) ")\n"
	"                                  Send DECK_STATUS message (" xstr(CEC_MSG_DECK_STATUS) ")\n"
	"  --give-deck-status status-req=<val>\n"
	"                                  'status-req' can have these values:\n"
	"                                      on (" xstr(CEC_OP_STATUS_REQ_ON) ")\n"
	"                                      off (" xstr(CEC_OP_STATUS_REQ_OFF) ")\n"
	"                                      once (" xstr(CEC_OP_STATUS_REQ_ONCE) ")\n"
	"                                  Send GIVE_DECK_STATUS message (" xstr(CEC_MSG_GIVE_DECK_STATUS) ")\n"
	"  --play play-mode=<val>\n"
	"                                  'play-mode' can have these values:\n"
	"                                      fwd (" xstr(CEC_OP_PLAY_MODE_PLAY_FWD) ")\n"
	"                                      rev (" xstr(CEC_OP_PLAY_MODE_PLAY_REV) ")\n"
	"                                      still (" xstr(CEC_OP_PLAY_MODE_PLAY_STILL) ")\n"
	"                                      fast-fwd-min (" xstr(CEC_OP_PLAY_MODE_PLAY_FAST_FWD_MIN) ")\n"
	"                                      fast-fwd-med (" xstr(CEC_OP_PLAY_MODE_PLAY_FAST_FWD_MED) ")\n"
	"                                      fast-fwd-max (" xstr(CEC_OP_PLAY_MODE_PLAY_FAST_FWD_MAX) ")\n"
	"                                      fast-rev-min (" xstr(CEC_OP_PLAY_MODE_PLAY_FAST_REV_MIN) ")\n"
	"                                      fast-rev-med (" xstr(CEC_OP_PLAY_MODE_PLAY_FAST_REV_MED) ")\n"
	"                                      fast-rev-max (" xstr(CEC_OP_PLAY_MODE_PLAY_FAST_REV_MAX) ")\n"
	"                                      slow-fwd-min (" xstr(CEC_OP_PLAY_MODE_PLAY_SLOW_FWD_MIN) ")\n"
	"                                      slow-fwd-med (" xstr(CEC_OP_PLAY_MODE_PLAY_SLOW_FWD_MED) ")\n"
	"                                      slow-fwd-max (" xstr(CEC_OP_PLAY_MODE_PLAY_SLOW_FWD_MAX) ")\n"
	"                                      slow-rev-min (" xstr(CEC_OP_PLAY_MODE_PLAY_SLOW_REV_MIN) ")\n"
	"                                      slow-rev-med (" xstr(CEC_OP_PLAY_MODE_PLAY_SLOW_REV_MED) ")\n"
	"                                      slow-rev-max (" xstr(CEC_OP_PLAY_MODE_PLAY_SLOW_REV_MAX) ")\n"
	"                                  Send PLAY message (" xstr(CEC_MSG_PLAY) ")\n";

static const char *device_menu_control_usage =
	"  --menu-status menu-state=<val>\n"
	"                                  'menu-state' can have these values:\n"
	"                                      activated (" xstr(CEC_OP_MENU_STATE_ACTIVATED) ")\n"
	"                                      deactivated (" xstr(CEC_OP_MENU_STATE_DEACTIVATED) ")\n"
	"                                  Send MENU_STATUS message (" xstr(CEC_MSG_MENU_STATUS) ")\n"
	"  --menu-request menu-req=<val>\n"
	"                                  'menu-req' can have these values:\n"
	"                                      activate (" xstr(CEC_OP_MENU_REQUEST_ACTIVATE) ")\n"
	"                                      deactivate (" xstr(CEC_OP_MENU_REQUEST_DEACTIVATE) ")\n"
	"                                      query (" xstr(CEC_OP_MENU_REQUEST_QUERY) ")\n"
	"                                  Send MENU_REQUEST message (" xstr(CEC_MSG_MENU_REQUEST) ")\n"
	"  --user-control-pressed ui-cmd=<val>,has-opt-arg=<val>,play-mode=<val>,ui-function-media=<val>,ui-function-select-av-input=<val>,ui-function-select-audio-input=<val>,ui-bcast-type=<val>,ui-snd-pres-ctl=<val>,channel-number-fmt=<val>,major=<val>,minor=<val>\n"
	"                                  'play-mode' can have these values:\n"
	"                                      fwd (" xstr(CEC_OP_PLAY_MODE_PLAY_FWD) ")\n"
	"                                      rev (" xstr(CEC_OP_PLAY_MODE_PLAY_REV) ")\n"
	"                                      still (" xstr(CEC_OP_PLAY_MODE_PLAY_STILL) ")\n"
	"                                      fast-fwd-min (" xstr(CEC_OP_PLAY_MODE_PLAY_FAST_FWD_MIN) ")\n"
	"                                      fast-fwd-med (" xstr(CEC_OP_PLAY_MODE_PLAY_FAST_FWD_MED) ")\n"
	"                                      fast-fwd-max (" xstr(CEC_OP_PLAY_MODE_PLAY_FAST_FWD_MAX) ")\n"
	"                                      fast-rev-min (" xstr(CEC_OP_PLAY_MODE_PLAY_FAST_REV_MIN) ")\n"
	"                                      fast-rev-med (" xstr(CEC_OP_PLAY_MODE_PLAY_FAST_REV_MED) ")\n"
	"                                      fast-rev-max (" xstr(CEC_OP_PLAY_MODE_PLAY_FAST_REV_MAX) ")\n"
	"                                      slow-fwd-min (" xstr(CEC_OP_PLAY_MODE_PLAY_SLOW_FWD_MIN) ")\n"
	"                                      slow-fwd-med (" xstr(CEC_OP_PLAY_MODE_PLAY_SLOW_FWD_MED) ")\n"
	"                                      slow-fwd-max (" xstr(CEC_OP_PLAY_MODE_PLAY_SLOW_FWD_MAX) ")\n"
	"                                      slow-rev-min (" xstr(CEC_OP_PLAY_MODE_PLAY_SLOW_REV_MIN) ")\n"
	"                                      slow-rev-med (" xstr(CEC_OP_PLAY_MODE_PLAY_SLOW_REV_MED) ")\n"
	"                                      slow-rev-max (" xstr(CEC_OP_PLAY_MODE_PLAY_SLOW_REV_MAX) ")\n"
	"                                  'ui-bcast-type' can have these values:\n"
	"                                      toggle-all (" xstr(CEC_OP_UI_BCAST_TYPE_TOGGLE_ALL) ")\n"
	"                                      toggle-dig-ana (" xstr(CEC_OP_UI_BCAST_TYPE_TOGGLE_DIG_ANA) ")\n"
	"                                      analogue (" xstr(CEC_OP_UI_BCAST_TYPE_ANALOGUE) ")\n"
	"                                      analogue-t (" xstr(CEC_OP_UI_BCAST_TYPE_ANALOGUE_T) ")\n"
	"                                      analogue-cable (" xstr(CEC_OP_UI_BCAST_TYPE_ANALOGUE_CABLE) ")\n"
	"                                      analogue-sat (" xstr(CEC_OP_UI_BCAST_TYPE_ANALOGUE_SAT) ")\n"
	"                                      digital (" xstr(CEC_OP_UI_BCAST_TYPE_DIGITAL) ")\n"
	"                                      digital-t (" xstr(CEC_OP_UI_BCAST_TYPE_DIGITAL_T) ")\n"
	"                                      digital-cable (" xstr(CEC_OP_UI_BCAST_TYPE_DIGITAL_CABLE) ")\n"
	"                                      digital-sat (" xstr(CEC_OP_UI_BCAST_TYPE_DIGITAL_SAT) ")\n"
	"                                      digital-com-sat (" xstr(CEC_OP_UI_BCAST_TYPE_DIGITAL_COM_SAT) ")\n"
	"                                      digital-com-sat2 (" xstr(CEC_OP_UI_BCAST_TYPE_DIGITAL_COM_SAT2) ")\n"
	"                                      ip (" xstr(CEC_OP_UI_BCAST_TYPE_IP) ")\n"
	"                                  'ui-snd-pres-ctl' can have these values:\n"
	"                                      dual-mono (" xstr(CEC_OP_UI_SND_PRES_CTL_DUAL_MONO) ")\n"
	"                                      karaoke (" xstr(CEC_OP_UI_SND_PRES_CTL_KARAOKE) ")\n"
	"                                      downmix (" xstr(CEC_OP_UI_SND_PRES_CTL_DOWNMIX) ")\n"
	"                                      reverb (" xstr(CEC_OP_UI_SND_PRES_CTL_REVERB) ")\n"
	"                                      equalizer (" xstr(CEC_OP_UI_SND_PRES_CTL_EQUALIZER) ")\n"
	"                                      bass-up (" xstr(CEC_OP_UI_SND_PRES_CTL_BASS_UP) ")\n"
	"                                      bass-neutral (" xstr(CEC_OP_UI_SND_PRES_CTL_BASS_NEUTRAL) ")\n"
	"                                      bass-down (" xstr(CEC_OP_UI_SND_PRES_CTL_BASS_DOWN) ")\n"
	"                                      treble-up (" xstr(CEC_OP_UI_SND_PRES_CTL_TREBLE_UP) ")\n"
	"                                      treble-neutral (" xstr(CEC_OP_UI_SND_PRES_CTL_TREBLE_NEUTRAL) ")\n"
	"                                      treble-down (" xstr(CEC_OP_UI_SND_PRES_CTL_TREBLE_DOWN) ")\n"
	"                                  'channel-number-fmt' can have these values:\n"
	"                                      1-part (" xstr(CEC_OP_CHANNEL_NUMBER_FMT_1_PART) ")\n"
	"                                      2-part (" xstr(CEC_OP_CHANNEL_NUMBER_FMT_2_PART) ")\n"
	"                                  Send USER_CONTROL_PRESSED message (" xstr(CEC_MSG_USER_CONTROL_PRESSED) ")\n"
	"  --user-control-released         Send USER_CONTROL_RELEASED message (" xstr(CEC_MSG_USER_CONTROL_RELEASED) ")\n";

static const char *device_osd_transfer_usage =
	"  --set-osd-name name=<val>\n"
	"                                  Send SET_OSD_NAME message (" xstr(CEC_MSG_SET_OSD_NAME) ")\n"
	"  --give-osd-name                 Send GIVE_OSD_NAME message (" xstr(CEC_MSG_GIVE_OSD_NAME) ")\n";

static const char *dynamic_audio_lipsync_usage =
	"  --report-current-latency phys-addr=<val>,video-latency=<val>,low-latency-mode=<val>,audio-out-compensated=<val>,audio-out-delay=<val>\n"
	"                                  'low-latency-mode' can have these values:\n"
	"                                      off (" xstr(CEC_OP_LOW_LATENCY_MODE_OFF) ")\n"
	"                                      on (" xstr(CEC_OP_LOW_LATENCY_MODE_ON) ")\n"
	"                                  'audio-out-compensated' can have these values:\n"
	"                                      na (" xstr(CEC_OP_AUD_OUT_COMPENSATED_NA) ")\n"
	"                                      delay (" xstr(CEC_OP_AUD_OUT_COMPENSATED_DELAY) ")\n"
	"                                      no-delay (" xstr(CEC_OP_AUD_OUT_COMPENSATED_NO_DELAY) ")\n"
	"                                      partial-delay (" xstr(CEC_OP_AUD_OUT_COMPENSATED_PARTIAL_DELAY) ")\n"
	"                                  Send REPORT_CURRENT_LATENCY message (" xstr(CEC_MSG_REPORT_CURRENT_LATENCY) ", bcast)\n"
	"  --request-current-latency phys-addr=<val>\n"
	"                                  Send REQUEST_CURRENT_LATENCY message (" xstr(CEC_MSG_REQUEST_CURRENT_LATENCY) ", bcast)\n";

static const char *htng_usage =
	"  --htng-tuner-1part-chan htng-tuner-type=<val>,chan=<val>\n"
	"                                  'htng-tuner-type' can have these values:\n"
	"                                      air (" xstr(CEC_OP_HTNG_TUNER_TYPE_AIR) ")\n"
	"                                      cable (" xstr(CEC_OP_HTNG_TUNER_TYPE_CABLE) ")\n"
	"                                      sat (" xstr(CEC_OP_HTNG_TUNER_TYPE_SAT) ")\n"
	"                                      not-specified (" xstr(CEC_OP_HTNG_TUNER_TYPE_NOT_SPECIFIED) ")\n"
	"                                  Send HTNG_TUNER_1PART_CHAN message (" xstr(CEC_MSG_HTNG_TUNER_1PART_CHAN) ")\n"
	"  --htng-tuner-2part-chan htng-tuner-type=<val>,major-chan=<val>,minor-chan=<val>\n"
	"                                  'htng-tuner-type' can have these values:\n"
	"                                      air (" xstr(CEC_OP_HTNG_TUNER_TYPE_AIR) ")\n"
	"                                      cable (" xstr(CEC_OP_HTNG_TUNER_TYPE_CABLE) ")\n"
	"                                      sat (" xstr(CEC_OP_HTNG_TUNER_TYPE_SAT) ")\n"
	"                                      not-specified (" xstr(CEC_OP_HTNG_TUNER_TYPE_NOT_SPECIFIED) ")\n"
	"                                  Send HTNG_TUNER_2PART_CHAN message (" xstr(CEC_MSG_HTNG_TUNER_2PART_CHAN) ")\n"
	"  --htng-input-sel-av input=<val>\n"
	"                                  Send HTNG_INPUT_SEL_AV message (" xstr(CEC_MSG_HTNG_INPUT_SEL_AV) ")\n"
	"  --htng-input-sel-pc input=<val>\n"
	"                                  Send HTNG_INPUT_SEL_PC message (" xstr(CEC_MSG_HTNG_INPUT_SEL_PC) ")\n"
	"  --htng-input-sel-hdmi input=<val>\n"
	"                                  Send HTNG_INPUT_SEL_HDMI message (" xstr(CEC_MSG_HTNG_INPUT_SEL_HDMI) ")\n"
	"  --htng-input-sel-component input=<val>\n"
	"                                  Send HTNG_INPUT_SEL_COMPONENT message (" xstr(CEC_MSG_HTNG_INPUT_SEL_COMPONENT) ")\n"
	"  --htng-input-sel-dvi input=<val>\n"
	"                                  Send HTNG_INPUT_SEL_DVI message (" xstr(CEC_MSG_HTNG_INPUT_SEL_DVI) ")\n"
	"  --htng-input-sel-dp input=<val>\n"
	"                                  Send HTNG_INPUT_SEL_DP message (" xstr(CEC_MSG_HTNG_INPUT_SEL_DP) ")\n"
	"  --htng-input-sel-usb input=<val>\n"
	"                                  Send HTNG_INPUT_SEL_USB message (" xstr(CEC_MSG_HTNG_INPUT_SEL_USB) ")\n"
	"  --htng-set-def-pwr-on-input-src htng-input-src=<val>,htng-tuner-type=<val>,major=<val>,input=<val>\n"
	"                                  'htng-input-src' can have these values:\n"
	"                                      tuner-1part (" xstr(CEC_OP_HTNG_INPUT_SRC_TUNER_1PART) ")\n"
	"                                      tuner-2part (" xstr(CEC_OP_HTNG_INPUT_SRC_TUNER_2PART) ")\n"
	"                                      av (" xstr(CEC_OP_HTNG_INPUT_SRC_AV) ")\n"
	"                                      pc (" xstr(CEC_OP_HTNG_INPUT_SRC_PC) ")\n"
	"                                      hdmi (" xstr(CEC_OP_HTNG_INPUT_SRC_HDMI) ")\n"
	"                                      component (" xstr(CEC_OP_HTNG_INPUT_SRC_COMPONENT) ")\n"
	"                                      dvi (" xstr(CEC_OP_HTNG_INPUT_SRC_DVI) ")\n"
	"                                      dp (" xstr(CEC_OP_HTNG_INPUT_SRC_DP) ")\n"
	"                                      usb (" xstr(CEC_OP_HTNG_INPUT_SRC_USB) ")\n"
	"                                  'htng-tuner-type' can have these values:\n"
	"                                      air (" xstr(CEC_OP_HTNG_TUNER_TYPE_AIR) ")\n"
	"                                      cable (" xstr(CEC_OP_HTNG_TUNER_TYPE_CABLE) ")\n"
	"                                      sat (" xstr(CEC_OP_HTNG_TUNER_TYPE_SAT) ")\n"
	"                                      not-specified (" xstr(CEC_OP_HTNG_TUNER_TYPE_NOT_SPECIFIED) ")\n"
	"                                  Send HTNG_SET_DEF_PWR_ON_INPUT_SRC message (" xstr(CEC_MSG_HTNG_SET_DEF_PWR_ON_INPUT_SRC) ")\n"
	"  --htng-set-tv-speakers on=<val>\n"
	"                                  Send HTNG_SET_TV_SPEAKERS message (" xstr(CEC_MSG_HTNG_SET_TV_SPEAKERS) ")\n"
	"  --htng-set-dig-audio on=<val>\n"
	"                                  Send HTNG_SET_DIG_AUDIO message (" xstr(CEC_MSG_HTNG_SET_DIG_AUDIO) ")\n"
	"  --htng-set-ana-audio on=<val>\n"
	"                                  Send HTNG_SET_ANA_AUDIO message (" xstr(CEC_MSG_HTNG_SET_ANA_AUDIO) ")\n"
	"  --htng-set-def-pwr-on-vol vol=<val>\n"
	"                                  Send HTNG_SET_DEF_PWR_ON_VOL message (" xstr(CEC_MSG_HTNG_SET_DEF_PWR_ON_VOL) ")\n"
	"  --htng-set-max-vol vol=<val>\n"
	"                                  Send HTNG_SET_MAX_VOL message (" xstr(CEC_MSG_HTNG_SET_MAX_VOL) ")\n"
	"  --htng-set-min-vol vol=<val>\n"
	"                                  Send HTNG_SET_MIN_VOL message (" xstr(CEC_MSG_HTNG_SET_MIN_VOL) ")\n"
	"  --htng-set-blue-screen blue=<val>\n"
	"                                  Send HTNG_SET_BLUE_SCREEN message (" xstr(CEC_MSG_HTNG_SET_BLUE_SCREEN) ")\n"
	"  --htng-set-brightness brightness=<val>\n"
	"                                  Send HTNG_SET_BRIGHTNESS message (" xstr(CEC_MSG_HTNG_SET_BRIGHTNESS) ")\n"
	"  --htng-set-color color=<val>\n"
	"                                  Send HTNG_SET_COLOR message (" xstr(CEC_MSG_HTNG_SET_COLOR) ")\n"
	"  --htng-set-contrast contrast=<val>\n"
	"                                  Send HTNG_SET_CONTRAST message (" xstr(CEC_MSG_HTNG_SET_CONTRAST) ")\n"
	"  --htng-set-sharpness sharpness=<val>\n"
	"                                  Send HTNG_SET_SHARPNESS message (" xstr(CEC_MSG_HTNG_SET_SHARPNESS) ")\n"
	"  --htng-set-hue hue=<val>\n"
	"                                  Send HTNG_SET_HUE message (" xstr(CEC_MSG_HTNG_SET_HUE) ")\n"
	"  --htng-set-led-backlight led-backlight=<val>\n"
	"                                  Send HTNG_SET_LED_BACKLIGHT message (" xstr(CEC_MSG_HTNG_SET_LED_BACKLIGHT) ")\n"
	"  --htng-set-tv-osd-control on=<val>\n"
	"                                  Send HTNG_SET_TV_OSD_CONTROL message (" xstr(CEC_MSG_HTNG_SET_TV_OSD_CONTROL) ")\n"
	"  --htng-set-audio-only-display on=<val>\n"
	"                                  Send HTNG_SET_AUDIO_ONLY_DISPLAY message (" xstr(CEC_MSG_HTNG_SET_AUDIO_ONLY_DISPLAY) ")\n"
	"  --htng-set-date date=<val>\n"
	"                                  Send HTNG_SET_DATE message (" xstr(CEC_MSG_HTNG_SET_DATE) ")\n"
	"  --htng-set-date-format ddmm=<val>\n"
	"                                  Send HTNG_SET_DATE_FORMAT message (" xstr(CEC_MSG_HTNG_SET_DATE_FORMAT) ")\n"
	"  --htng-set-time time=<val>\n"
	"                                  Send HTNG_SET_TIME message (" xstr(CEC_MSG_HTNG_SET_TIME) ")\n"
	"  --htng-set-clk-brightness-standby brightness=<val>\n"
	"                                  Send HTNG_SET_CLK_BRIGHTNESS_STANDBY message (" xstr(CEC_MSG_HTNG_SET_CLK_BRIGHTNESS_STANDBY) ")\n"
	"  --htng-set-clk-brightness-on brightness=<val>\n"
	"                                  Send HTNG_SET_CLK_BRIGHTNESS_ON message (" xstr(CEC_MSG_HTNG_SET_CLK_BRIGHTNESS_ON) ")\n"
	"  --htng-led-control htng-led-control=<val>\n"
	"                                  'htng-led-control' can have these values:\n"
	"                                      default (" xstr(CEC_OP_HTNG_LED_CONTROL_DEFAULT) ")\n"
	"                                      on (" xstr(CEC_OP_HTNG_LED_CONTROL_ON) ")\n"
	"                                      off (" xstr(CEC_OP_HTNG_LED_CONTROL_OFF) ")\n"
	"                                  Send HTNG_LED_CONTROL message (" xstr(CEC_MSG_HTNG_LED_CONTROL) ")\n"
	"  --htng-lock-tv-pwr-button on=<val>\n"
	"                                  Send HTNG_LOCK_TV_PWR_BUTTON message (" xstr(CEC_MSG_HTNG_LOCK_TV_PWR_BUTTON) ")\n"
	"  --htng-lock-tv-vol-buttons on=<val>\n"
	"                                  Send HTNG_LOCK_TV_VOL_BUTTONS message (" xstr(CEC_MSG_HTNG_LOCK_TV_VOL_BUTTONS) ")\n"
	"  --htng-lock-tv-chan-buttons on=<val>\n"
	"                                  Send HTNG_LOCK_TV_CHAN_BUTTONS message (" xstr(CEC_MSG_HTNG_LOCK_TV_CHAN_BUTTONS) ")\n"
	"  --htng-lock-tv-input-buttons on=<val>\n"
	"                                  Send HTNG_LOCK_TV_INPUT_BUTTONS message (" xstr(CEC_MSG_HTNG_LOCK_TV_INPUT_BUTTONS) ")\n"
	"  --htng-lock-tv-other-buttons on=<val>\n"
	"                                  Send HTNG_LOCK_TV_OTHER_BUTTONS message (" xstr(CEC_MSG_HTNG_LOCK_TV_OTHER_BUTTONS) ")\n"
	"  --htng-lock-everything on=<val>\n"
	"                                  Send HTNG_LOCK_EVERYTHING message (" xstr(CEC_MSG_HTNG_LOCK_EVERYTHING) ")\n"
	"  --htng-lock-everything-but-pwr on=<val>\n"
	"                                  Send HTNG_LOCK_EVERYTHING_BUT_PWR message (" xstr(CEC_MSG_HTNG_LOCK_EVERYTHING_BUT_PWR) ")\n"
	"  --htng-hotel-mode on=<val>,options=<val>\n"
	"                                  Send HTNG_HOTEL_MODE message (" xstr(CEC_MSG_HTNG_HOTEL_MODE) ")\n"
	"  --htng-set-pwr-saving-profile on=<val>,val=<val>\n"
	"                                  Send HTNG_SET_PWR_SAVING_PROFILE message (" xstr(CEC_MSG_HTNG_SET_PWR_SAVING_PROFILE) ")\n"
	"  --htng-set-sleep-timer minutes=<val>\n"
	"                                  Send HTNG_SET_SLEEP_TIMER message (" xstr(CEC_MSG_HTNG_SET_SLEEP_TIMER) ")\n"
	"  --htng-set-wakeup-time time=<val>\n"
	"                                  Send HTNG_SET_WAKEUP_TIME message (" xstr(CEC_MSG_HTNG_SET_WAKEUP_TIME) ")\n"
	"  --htng-set-auto-off-time time=<val>\n"
	"                                  Send HTNG_SET_AUTO_OFF_TIME message (" xstr(CEC_MSG_HTNG_SET_AUTO_OFF_TIME) ")\n"
	"  --htng-set-wakeup-src htng-input-src=<val>,htng-tuner-type=<val>,major=<val>,input=<val>\n"
	"                                  'htng-input-src' can have these values:\n"
	"                                      tuner-1part (" xstr(CEC_OP_HTNG_INPUT_SRC_TUNER_1PART) ")\n"
	"                                      tuner-2part (" xstr(CEC_OP_HTNG_INPUT_SRC_TUNER_2PART) ")\n"
	"                                      av (" xstr(CEC_OP_HTNG_INPUT_SRC_AV) ")\n"
	"                                      pc (" xstr(CEC_OP_HTNG_INPUT_SRC_PC) ")\n"
	"                                      hdmi (" xstr(CEC_OP_HTNG_INPUT_SRC_HDMI) ")\n"
	"                                      component (" xstr(CEC_OP_HTNG_INPUT_SRC_COMPONENT) ")\n"
	"                                      dvi (" xstr(CEC_OP_HTNG_INPUT_SRC_DVI) ")\n"
	"                                      dp (" xstr(CEC_OP_HTNG_INPUT_SRC_DP) ")\n"
	"                                      usb (" xstr(CEC_OP_HTNG_INPUT_SRC_USB) ")\n"
	"                                  'htng-tuner-type' can have these values:\n"
	"                                      air (" xstr(CEC_OP_HTNG_TUNER_TYPE_AIR) ")\n"
	"                                      cable (" xstr(CEC_OP_HTNG_TUNER_TYPE_CABLE) ")\n"
	"                                      sat (" xstr(CEC_OP_HTNG_TUNER_TYPE_SAT) ")\n"
	"                                      not-specified (" xstr(CEC_OP_HTNG_TUNER_TYPE_NOT_SPECIFIED) ")\n"
	"                                  Send HTNG_SET_WAKEUP_SRC message (" xstr(CEC_MSG_HTNG_SET_WAKEUP_SRC) ")\n"
	"  --htng-set-init-wakeup-vol vol=<val>,minutes=<val>\n"
	"                                  Send HTNG_SET_INIT_WAKEUP_VOL message (" xstr(CEC_MSG_HTNG_SET_INIT_WAKEUP_VOL) ")\n"
	"  --htng-clr-all-sleep-wake       Send HTNG_CLR_ALL_SLEEP_WAKE message (" xstr(CEC_MSG_HTNG_CLR_ALL_SLEEP_WAKE) ")\n"
	"  --htng-global-direct-tune-freq htng-chan-type=<val>,htng-prog-type=<val>,htng-system-type=<val>,freq=<val>,service-id=<val>,htng-mod-type=<val>,htng-symbol-rate=<val>,symbol-rate=<val>\n"
	"                                  'htng-chan-type' can have these values:\n"
	"                                      auto (" xstr(CEC_OP_HTNG_CHAN_TYPE_AUTO) ")\n"
	"                                      ana-ant (" xstr(CEC_OP_HTNG_CHAN_TYPE_ANA_ANT) ")\n"
	"                                      ana-cable (" xstr(CEC_OP_HTNG_CHAN_TYPE_ANA_CABLE) ")\n"
	"                                      dig-ant (" xstr(CEC_OP_HTNG_CHAN_TYPE_DIG_ANT) ")\n"
	"                                      dig-cable (" xstr(CEC_OP_HTNG_CHAN_TYPE_DIG_CABLE) ")\n"
	"                                      satellite (" xstr(CEC_OP_HTNG_CHAN_TYPE_SATELLITE) ")\n"
	"                                  'htng-prog-type' can have these values:\n"
	"                                      av (" xstr(CEC_OP_HTNG_PROG_TYPE_AV) ")\n"
	"                                      radio (" xstr(CEC_OP_HTNG_PROG_TYPE_RADIO) ")\n"
	"                                  'htng-system-type' can have these values:\n"
	"                                      pal-bg (" xstr(CEC_OP_HTNG_SYSTEM_TYPE_PAL_BG) ")\n"
	"                                      pal-i (" xstr(CEC_OP_HTNG_SYSTEM_TYPE_PAL_I) ")\n"
	"                                      pal-dk (" xstr(CEC_OP_HTNG_SYSTEM_TYPE_PAL_DK) ")\n"
	"                                      pal-m (" xstr(CEC_OP_HTNG_SYSTEM_TYPE_PAL_M) ")\n"
	"                                      pal-n (" xstr(CEC_OP_HTNG_SYSTEM_TYPE_PAL_N) ")\n"
	"                                      secam-bg (" xstr(CEC_OP_HTNG_SYSTEM_TYPE_SECAM_BG) ")\n"
	"                                      secam-dk (" xstr(CEC_OP_HTNG_SYSTEM_TYPE_SECAM_DK) ")\n"
	"                                      secam-l (" xstr(CEC_OP_HTNG_SYSTEM_TYPE_SECAM_L) ")\n"
	"                                      ntsc-m (" xstr(CEC_OP_HTNG_SYSTEM_TYPE_NTSC_M) ")\n"
	"                                  'htng-mod-type' can have these values:\n"
	"                                      auto (" xstr(CEC_OP_HTNG_MOD_TYPE_AUTO) ")\n"
	"                                      qpsk (" xstr(CEC_OP_HTNG_MOD_TYPE_QPSK) ")\n"
	"                                      qcam16 (" xstr(CEC_OP_HTNG_MOD_TYPE_QCAM16) ")\n"
	"                                      qcam32 (" xstr(CEC_OP_HTNG_MOD_TYPE_QCAM32) ")\n"
	"                                      qcam64 (" xstr(CEC_OP_HTNG_MOD_TYPE_QCAM64) ")\n"
	"                                      qcam128 (" xstr(CEC_OP_HTNG_MOD_TYPE_QCAM128) ")\n"
	"                                      qcam256 (" xstr(CEC_OP_HTNG_MOD_TYPE_QCAM256) ")\n"
	"                                      dqpsk (" xstr(CEC_OP_HTNG_MOD_TYPE_DQPSK) ")\n"
	"                                  'htng-symbol-rate' can have these values:\n"
	"                                      auto (" xstr(CEC_OP_HTNG_SYMBOL_RATE_AUTO) ")\n"
	"                                      manual (" xstr(CEC_OP_HTNG_SYMBOL_RATE_MANUAL) ")\n"
	"                                  Send HTNG_GLOBAL_DIRECT_TUNE_FREQ message (" xstr(CEC_MSG_HTNG_GLOBAL_DIRECT_TUNE_FREQ) ")\n"
	"  --htng-global-direct-tune-chan htng-chan-type=<val>,htng-prog-type=<val>,chan=<val>\n"
	"                                  'htng-chan-type' can have these values:\n"
	"                                      auto (" xstr(CEC_OP_HTNG_CHAN_TYPE_AUTO) ")\n"
	"                                      ana-ant (" xstr(CEC_OP_HTNG_CHAN_TYPE_ANA_ANT) ")\n"
	"                                      ana-cable (" xstr(CEC_OP_HTNG_CHAN_TYPE_ANA_CABLE) ")\n"
	"                                      dig-ant (" xstr(CEC_OP_HTNG_CHAN_TYPE_DIG_ANT) ")\n"
	"                                      dig-cable (" xstr(CEC_OP_HTNG_CHAN_TYPE_DIG_CABLE) ")\n"
	"                                      satellite (" xstr(CEC_OP_HTNG_CHAN_TYPE_SATELLITE) ")\n"
	"                                  'htng-prog-type' can have these values:\n"
	"                                      av (" xstr(CEC_OP_HTNG_PROG_TYPE_AV) ")\n"
	"                                      radio (" xstr(CEC_OP_HTNG_PROG_TYPE_RADIO) ")\n"
	"                                  Send HTNG_GLOBAL_DIRECT_TUNE_CHAN message (" xstr(CEC_MSG_HTNG_GLOBAL_DIRECT_TUNE_CHAN) ")\n"
	"  --htng-global-direct-tune-ext-freq htng-ext-chan-type=<val>,htng-prog-type=<val>,htng-system-type=<val>,freq=<val>,service-id=<val>,htng-mod-type=<val>,htng-onid=<val>,onid=<val>,htng-nid=<val>,nid=<val>,htng-tsid-plp=<val>,tsid-plp=<val>,htng-symbol-rate=<val>,symbol-rate=<val>\n"
	"                                  'htng-ext-chan-type' can have these values:\n"
	"                                      auto (" xstr(CEC_OP_HTNG_EXT_CHAN_TYPE_AUTO) ")\n"
	"                                      ana-ant (" xstr(CEC_OP_HTNG_EXT_CHAN_TYPE_ANA_ANT) ")\n"
	"                                      ana-cable (" xstr(CEC_OP_HTNG_EXT_CHAN_TYPE_ANA_CABLE) ")\n"
	"                                      dvb-t-isdb-t-dtmb (" xstr(CEC_OP_HTNG_EXT_CHAN_TYPE_DVB_T_ISDB_T_DTMB) ")\n"
	"                                      dvb-c (" xstr(CEC_OP_HTNG_EXT_CHAN_TYPE_DVB_C) ")\n"
	"                                      dvb-t2 (" xstr(CEC_OP_HTNG_EXT_CHAN_TYPE_DVB_T2) ")\n"
	"                                  'htng-prog-type' can have these values:\n"
	"                                      av (" xstr(CEC_OP_HTNG_PROG_TYPE_AV) ")\n"
	"                                      radio (" xstr(CEC_OP_HTNG_PROG_TYPE_RADIO) ")\n"
	"                                  'htng-system-type' can have these values:\n"
	"                                      pal-bg (" xstr(CEC_OP_HTNG_SYSTEM_TYPE_PAL_BG) ")\n"
	"                                      pal-i (" xstr(CEC_OP_HTNG_SYSTEM_TYPE_PAL_I) ")\n"
	"                                      pal-dk (" xstr(CEC_OP_HTNG_SYSTEM_TYPE_PAL_DK) ")\n"
	"                                      pal-m (" xstr(CEC_OP_HTNG_SYSTEM_TYPE_PAL_M) ")\n"
	"                                      pal-n (" xstr(CEC_OP_HTNG_SYSTEM_TYPE_PAL_N) ")\n"
	"                                      secam-bg (" xstr(CEC_OP_HTNG_SYSTEM_TYPE_SECAM_BG) ")\n"
	"                                      secam-dk (" xstr(CEC_OP_HTNG_SYSTEM_TYPE_SECAM_DK) ")\n"
	"                                      secam-l (" xstr(CEC_OP_HTNG_SYSTEM_TYPE_SECAM_L) ")\n"
	"                                      ntsc-m (" xstr(CEC_OP_HTNG_SYSTEM_TYPE_NTSC_M) ")\n"
	"                                  'htng-mod-type' can have these values:\n"
	"                                      auto (" xstr(CEC_OP_HTNG_MOD_TYPE_AUTO) ")\n"
	"                                      qpsk (" xstr(CEC_OP_HTNG_MOD_TYPE_QPSK) ")\n"
	"                                      qcam16 (" xstr(CEC_OP_HTNG_MOD_TYPE_QCAM16) ")\n"
	"                                      qcam32 (" xstr(CEC_OP_HTNG_MOD_TYPE_QCAM32) ")\n"
	"                                      qcam64 (" xstr(CEC_OP_HTNG_MOD_TYPE_QCAM64) ")\n"
	"                                      qcam128 (" xstr(CEC_OP_HTNG_MOD_TYPE_QCAM128) ")\n"
	"                                      qcam256 (" xstr(CEC_OP_HTNG_MOD_TYPE_QCAM256) ")\n"
	"                                      dqpsk (" xstr(CEC_OP_HTNG_MOD_TYPE_DQPSK) ")\n"
	"                                  'htng-onid' can have these values:\n"
	"                                      auto (" xstr(CEC_OP_HTNG_ONID_AUTO) ")\n"
	"                                      manual (" xstr(CEC_OP_HTNG_ONID_MANUAL) ")\n"
	"                                  'htng-nid' can have these values:\n"
	"                                      auto (" xstr(CEC_OP_HTNG_NID_AUTO) ")\n"
	"                                      manual (" xstr(CEC_OP_HTNG_NID_MANUAL) ")\n"
	"                                  'htng-tsid-plp' can have these values:\n"
	"                                      auto (" xstr(CEC_OP_HTNG_TSID_PLP_AUTO) ")\n"
	"                                      manual (" xstr(CEC_OP_HTNG_TSID_PLP_MANUAL) ")\n"
	"                                  'htng-symbol-rate' can have these values:\n"
	"                                      auto (" xstr(CEC_OP_HTNG_SYMBOL_RATE_AUTO) ")\n"
	"                                      manual (" xstr(CEC_OP_HTNG_SYMBOL_RATE_MANUAL) ")\n"
	"                                  Send HTNG_GLOBAL_DIRECT_TUNE_EXT_FREQ message (" xstr(CEC_MSG_HTNG_GLOBAL_DIRECT_TUNE_EXT_FREQ) ")\n";

static const char *osd_display_usage =
	"  --set-osd-string disp-ctl=<val>,osd=<val>\n"
	"                                  'disp-ctl' can have these values:\n"
	"                                      default (" xstr(CEC_OP_DISP_CTL_DEFAULT) ")\n"
	"                                      until-cleared (" xstr(CEC_OP_DISP_CTL_UNTIL_CLEARED) ")\n"
	"                                      clear (" xstr(CEC_OP_DISP_CTL_CLEAR) ")\n"
	"                                  Send SET_OSD_STRING message (" xstr(CEC_MSG_SET_OSD_STRING) ")\n";

static const char *one_touch_play_usage =
	"  --active-source phys-addr=<val>\n"
	"                                  Send ACTIVE_SOURCE message (" xstr(CEC_MSG_ACTIVE_SOURCE) ", bcast)\n"
	"  --image-view-on                 Send IMAGE_VIEW_ON message (" xstr(CEC_MSG_IMAGE_VIEW_ON) ")\n"
	"  --text-view-on                  Send TEXT_VIEW_ON message (" xstr(CEC_MSG_TEXT_VIEW_ON) ")\n";

static const char *one_touch_record_usage =
	"  --record-off                    Send RECORD_OFF message (" xstr(CEC_MSG_RECORD_OFF) ")\n"
	"  --record-on-own                 Send RECORD_ON message (" xstr(CEC_MSG_RECORD_ON) ")\n"
	"  --record-on-digital service-id-method=<val>,dig-bcast-system=<val>,transport-id=<val>,service-id=<val>,orig-network-id=<val>,program-number=<val>,channel-number-fmt=<val>,major=<val>,minor=<val>\n"
	"                                  'service-id-method' can have these values:\n"
	"                                      dig-id (" xstr(CEC_OP_SERVICE_ID_METHOD_BY_DIG_ID) ")\n"
	"                                      channel (" xstr(CEC_OP_SERVICE_ID_METHOD_BY_CHANNEL) ")\n"
	"                                  'dig-bcast-system' can have these values:\n"
	"                                      arib-gen (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ARIB_GEN) ")\n"
	"                                      atsc-gen (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ATSC_GEN) ")\n"
	"                                      dvb-gen (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_DVB_GEN) ")\n"
	"                                      arib-bs (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ARIB_BS) ")\n"
	"                                      arib-cs (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ARIB_CS) ")\n"
	"                                      arib-t (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ARIB_T) ")\n"
	"                                      atsc-cable (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ATSC_CABLE) ")\n"
	"                                      atsc-sat (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ATSC_SAT) ")\n"
	"                                      atsc-t (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ATSC_T) ")\n"
	"                                      dvb-c (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_DVB_C) ")\n"
	"                                      dvb-s (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_DVB_S) ")\n"
	"                                      dvb-s2 (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_DVB_S2) ")\n"
	"                                      dvb-t (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_DVB_T) ")\n"
	"                                  'channel-number-fmt' can have these values:\n"
	"                                      1-part (" xstr(CEC_OP_CHANNEL_NUMBER_FMT_1_PART) ")\n"
	"                                      2-part (" xstr(CEC_OP_CHANNEL_NUMBER_FMT_2_PART) ")\n"
	"                                  Send RECORD_ON message (" xstr(CEC_MSG_RECORD_ON) ")\n"
	"  --record-on-analog ana-bcast-type=<val>,ana-freq=<val>,bcast-system=<val>\n"
	"                                  'ana-bcast-type' can have these values:\n"
	"                                      cable (" xstr(CEC_OP_ANA_BCAST_TYPE_CABLE) ")\n"
	"                                      satellite (" xstr(CEC_OP_ANA_BCAST_TYPE_SATELLITE) ")\n"
	"                                      terrestrial (" xstr(CEC_OP_ANA_BCAST_TYPE_TERRESTRIAL) ")\n"
	"                                  'bcast-system' can have these values:\n"
	"                                      pal-bg (" xstr(CEC_OP_BCAST_SYSTEM_PAL_BG) ")\n"
	"                                      secam-lq (" xstr(CEC_OP_BCAST_SYSTEM_SECAM_LQ) ")\n"
	"                                      pal-m (" xstr(CEC_OP_BCAST_SYSTEM_PAL_M) ")\n"
	"                                      ntsc-m (" xstr(CEC_OP_BCAST_SYSTEM_NTSC_M) ")\n"
	"                                      pal-i (" xstr(CEC_OP_BCAST_SYSTEM_PAL_I) ")\n"
	"                                      secam-dk (" xstr(CEC_OP_BCAST_SYSTEM_SECAM_DK) ")\n"
	"                                      secam-bg (" xstr(CEC_OP_BCAST_SYSTEM_SECAM_BG) ")\n"
	"                                      secam-l (" xstr(CEC_OP_BCAST_SYSTEM_SECAM_L) ")\n"
	"                                      pal-dk (" xstr(CEC_OP_BCAST_SYSTEM_PAL_DK) ")\n"
	"                                      other (" xstr(CEC_OP_BCAST_SYSTEM_OTHER) ")\n"
	"                                  Send RECORD_ON message (" xstr(CEC_MSG_RECORD_ON) ")\n"
	"  --record-on-plug plug=<val>\n"
	"                                  Send RECORD_ON message (" xstr(CEC_MSG_RECORD_ON) ")\n"
	"  --record-on-phys-addr phys-addr=<val>\n"
	"                                  Send RECORD_ON message (" xstr(CEC_MSG_RECORD_ON) ")\n"
	"  --record-status rec-status=<val>\n"
	"                                  'rec-status' can have these values:\n"
	"                                      cur-src (" xstr(CEC_OP_RECORD_STATUS_CUR_SRC) ")\n"
	"                                      dig-service (" xstr(CEC_OP_RECORD_STATUS_DIG_SERVICE) ")\n"
	"                                      ana-service (" xstr(CEC_OP_RECORD_STATUS_ANA_SERVICE) ")\n"
	"                                      ext-input (" xstr(CEC_OP_RECORD_STATUS_EXT_INPUT) ")\n"
	"                                      no-dig-service (" xstr(CEC_OP_RECORD_STATUS_NO_DIG_SERVICE) ")\n"
	"                                      no-ana-service (" xstr(CEC_OP_RECORD_STATUS_NO_ANA_SERVICE) ")\n"
	"                                      no-service (" xstr(CEC_OP_RECORD_STATUS_NO_SERVICE) ")\n"
	"                                      invalid-ext-plug (" xstr(CEC_OP_RECORD_STATUS_INVALID_EXT_PLUG) ")\n"
	"                                      invalid-ext-phys-addr (" xstr(CEC_OP_RECORD_STATUS_INVALID_EXT_PHYS_ADDR) ")\n"
	"                                      unsup-ca (" xstr(CEC_OP_RECORD_STATUS_UNSUP_CA) ")\n"
	"                                      no-ca-entitlements (" xstr(CEC_OP_RECORD_STATUS_NO_CA_ENTITLEMENTS) ")\n"
	"                                      cant-copy-src (" xstr(CEC_OP_RECORD_STATUS_CANT_COPY_SRC) ")\n"
	"                                      no-more-copies (" xstr(CEC_OP_RECORD_STATUS_NO_MORE_COPIES) ")\n"
	"                                      no-media (" xstr(CEC_OP_RECORD_STATUS_NO_MEDIA) ")\n"
	"                                      playing (" xstr(CEC_OP_RECORD_STATUS_PLAYING) ")\n"
	"                                      already-recording (" xstr(CEC_OP_RECORD_STATUS_ALREADY_RECORDING) ")\n"
	"                                      media-prot (" xstr(CEC_OP_RECORD_STATUS_MEDIA_PROT) ")\n"
	"                                      no-signal (" xstr(CEC_OP_RECORD_STATUS_NO_SIGNAL) ")\n"
	"                                      media-problem (" xstr(CEC_OP_RECORD_STATUS_MEDIA_PROBLEM) ")\n"
	"                                      no-space (" xstr(CEC_OP_RECORD_STATUS_NO_SPACE) ")\n"
	"                                      parental-lock (" xstr(CEC_OP_RECORD_STATUS_PARENTAL_LOCK) ")\n"
	"                                      terminated-ok (" xstr(CEC_OP_RECORD_STATUS_TERMINATED_OK) ")\n"
	"                                      already-term (" xstr(CEC_OP_RECORD_STATUS_ALREADY_TERM) ")\n"
	"                                      other (" xstr(CEC_OP_RECORD_STATUS_OTHER) ")\n"
	"                                  Send RECORD_STATUS message (" xstr(CEC_MSG_RECORD_STATUS) ")\n"
	"  --record-tv-screen              Send RECORD_TV_SCREEN message (" xstr(CEC_MSG_RECORD_TV_SCREEN) ")\n";

static const char *power_status_usage =
	"  --report-power-status pwr-state=<val>\n"
	"                                  'pwr-state' can have these values:\n"
	"                                      on (" xstr(CEC_OP_POWER_STATUS_ON) ")\n"
	"                                      standby (" xstr(CEC_OP_POWER_STATUS_STANDBY) ")\n"
	"                                      to-on (" xstr(CEC_OP_POWER_STATUS_TO_ON) ")\n"
	"                                      to-standby (" xstr(CEC_OP_POWER_STATUS_TO_STANDBY) ")\n"
	"                                  Send REPORT_POWER_STATUS message (" xstr(CEC_MSG_REPORT_POWER_STATUS) ")\n"
	"  --give-device-power-status      Send GIVE_DEVICE_POWER_STATUS message (" xstr(CEC_MSG_GIVE_DEVICE_POWER_STATUS) ")\n";

static const char *routing_control_usage =
	"  --inactive-source phys-addr=<val>\n"
	"                                  Send INACTIVE_SOURCE message (" xstr(CEC_MSG_INACTIVE_SOURCE) ")\n"
	"  --request-active-source         Send REQUEST_ACTIVE_SOURCE message (" xstr(CEC_MSG_REQUEST_ACTIVE_SOURCE) ", bcast)\n"
	"  --routing-information phys-addr=<val>\n"
	"                                  Send ROUTING_INFORMATION message (" xstr(CEC_MSG_ROUTING_INFORMATION) ", bcast)\n"
	"  --routing-change orig-phys-addr=<val>,new-phys-addr=<val>\n"
	"                                  Send ROUTING_CHANGE message (" xstr(CEC_MSG_ROUTING_CHANGE) ", bcast)\n"
	"  --set-stream-path phys-addr=<val>\n"
	"                                  Send SET_STREAM_PATH message (" xstr(CEC_MSG_SET_STREAM_PATH) ", bcast)\n"
	"  --active-source phys-addr=<val>\n"
	"                                  Send ACTIVE_SOURCE message (" xstr(CEC_MSG_ACTIVE_SOURCE) ", bcast)\n";

static const char *standby_usage =
	"  --standby                       Send STANDBY message (" xstr(CEC_MSG_STANDBY) ")\n";

static const char *system_audio_control_usage =
	"  --report-audio-status aud-mute-status=<val>,aud-vol-status=<val>\n"
	"                                  'aud-mute-status' can have these values:\n"
	"                                      off (" xstr(CEC_OP_AUD_MUTE_STATUS_OFF) ")\n"
	"                                      on (" xstr(CEC_OP_AUD_MUTE_STATUS_ON) ")\n"
	"                                  Send REPORT_AUDIO_STATUS message (" xstr(CEC_MSG_REPORT_AUDIO_STATUS) ")\n"
	"  --give-audio-status             Send GIVE_AUDIO_STATUS message (" xstr(CEC_MSG_GIVE_AUDIO_STATUS) ")\n"
	"  --set-system-audio-mode sys-aud-status=<val>\n"
	"                                  'sys-aud-status' can have these values:\n"
	"                                      off (" xstr(CEC_OP_SYS_AUD_STATUS_OFF) ")\n"
	"                                      on (" xstr(CEC_OP_SYS_AUD_STATUS_ON) ")\n"
	"                                  Send SET_SYSTEM_AUDIO_MODE message (" xstr(CEC_MSG_SET_SYSTEM_AUDIO_MODE) ")\n"
	"  --system-audio-mode-request phys-addr=<val>\n"
	"                                  Send SYSTEM_AUDIO_MODE_REQUEST message (" xstr(CEC_MSG_SYSTEM_AUDIO_MODE_REQUEST) ")\n"
	"  --system-audio-mode-status sys-aud-status=<val>\n"
	"                                  'sys-aud-status' can have these values:\n"
	"                                      off (" xstr(CEC_OP_SYS_AUD_STATUS_OFF) ")\n"
	"                                      on (" xstr(CEC_OP_SYS_AUD_STATUS_ON) ")\n"
	"                                  Send SYSTEM_AUDIO_MODE_STATUS message (" xstr(CEC_MSG_SYSTEM_AUDIO_MODE_STATUS) ")\n"
	"  --give-system-audio-mode-status Send GIVE_SYSTEM_AUDIO_MODE_STATUS message (" xstr(CEC_MSG_GIVE_SYSTEM_AUDIO_MODE_STATUS) ")\n"
	"  --report-short-audio-descriptor num-descriptors=<val>,descriptor1=<val>,descriptor2=<val>,descriptor3=<val>,descriptor4=<val>\n"
	"                                  Send REPORT_SHORT_AUDIO_DESCRIPTOR message (" xstr(CEC_MSG_REPORT_SHORT_AUDIO_DESCRIPTOR) ")\n"
	"  --request-short-audio-descriptor num-descriptors=<val>,audio-format-id1=<val>,audio-format-code1=<val>,audio-format-id2=<val>,audio-format-code2=<val>,audio-format-id3=<val>,audio-format-code3=<val>,audio-format-id4=<val>,audio-format-code4=<val>\n"
	"                                  Send REQUEST_SHORT_AUDIO_DESCRIPTOR message (" xstr(CEC_MSG_REQUEST_SHORT_AUDIO_DESCRIPTOR) ")\n"
	"  --user-control-pressed ui-cmd=<val>,has-opt-arg=<val>,play-mode=<val>,ui-function-media=<val>,ui-function-select-av-input=<val>,ui-function-select-audio-input=<val>,ui-bcast-type=<val>,ui-snd-pres-ctl=<val>,channel-number-fmt=<val>,major=<val>,minor=<val>\n"
	"                                  'play-mode' can have these values:\n"
	"                                      fwd (" xstr(CEC_OP_PLAY_MODE_PLAY_FWD) ")\n"
	"                                      rev (" xstr(CEC_OP_PLAY_MODE_PLAY_REV) ")\n"
	"                                      still (" xstr(CEC_OP_PLAY_MODE_PLAY_STILL) ")\n"
	"                                      fast-fwd-min (" xstr(CEC_OP_PLAY_MODE_PLAY_FAST_FWD_MIN) ")\n"
	"                                      fast-fwd-med (" xstr(CEC_OP_PLAY_MODE_PLAY_FAST_FWD_MED) ")\n"
	"                                      fast-fwd-max (" xstr(CEC_OP_PLAY_MODE_PLAY_FAST_FWD_MAX) ")\n"
	"                                      fast-rev-min (" xstr(CEC_OP_PLAY_MODE_PLAY_FAST_REV_MIN) ")\n"
	"                                      fast-rev-med (" xstr(CEC_OP_PLAY_MODE_PLAY_FAST_REV_MED) ")\n"
	"                                      fast-rev-max (" xstr(CEC_OP_PLAY_MODE_PLAY_FAST_REV_MAX) ")\n"
	"                                      slow-fwd-min (" xstr(CEC_OP_PLAY_MODE_PLAY_SLOW_FWD_MIN) ")\n"
	"                                      slow-fwd-med (" xstr(CEC_OP_PLAY_MODE_PLAY_SLOW_FWD_MED) ")\n"
	"                                      slow-fwd-max (" xstr(CEC_OP_PLAY_MODE_PLAY_SLOW_FWD_MAX) ")\n"
	"                                      slow-rev-min (" xstr(CEC_OP_PLAY_MODE_PLAY_SLOW_REV_MIN) ")\n"
	"                                      slow-rev-med (" xstr(CEC_OP_PLAY_MODE_PLAY_SLOW_REV_MED) ")\n"
	"                                      slow-rev-max (" xstr(CEC_OP_PLAY_MODE_PLAY_SLOW_REV_MAX) ")\n"
	"                                  'ui-bcast-type' can have these values:\n"
	"                                      toggle-all (" xstr(CEC_OP_UI_BCAST_TYPE_TOGGLE_ALL) ")\n"
	"                                      toggle-dig-ana (" xstr(CEC_OP_UI_BCAST_TYPE_TOGGLE_DIG_ANA) ")\n"
	"                                      analogue (" xstr(CEC_OP_UI_BCAST_TYPE_ANALOGUE) ")\n"
	"                                      analogue-t (" xstr(CEC_OP_UI_BCAST_TYPE_ANALOGUE_T) ")\n"
	"                                      analogue-cable (" xstr(CEC_OP_UI_BCAST_TYPE_ANALOGUE_CABLE) ")\n"
	"                                      analogue-sat (" xstr(CEC_OP_UI_BCAST_TYPE_ANALOGUE_SAT) ")\n"
	"                                      digital (" xstr(CEC_OP_UI_BCAST_TYPE_DIGITAL) ")\n"
	"                                      digital-t (" xstr(CEC_OP_UI_BCAST_TYPE_DIGITAL_T) ")\n"
	"                                      digital-cable (" xstr(CEC_OP_UI_BCAST_TYPE_DIGITAL_CABLE) ")\n"
	"                                      digital-sat (" xstr(CEC_OP_UI_BCAST_TYPE_DIGITAL_SAT) ")\n"
	"                                      digital-com-sat (" xstr(CEC_OP_UI_BCAST_TYPE_DIGITAL_COM_SAT) ")\n"
	"                                      digital-com-sat2 (" xstr(CEC_OP_UI_BCAST_TYPE_DIGITAL_COM_SAT2) ")\n"
	"                                      ip (" xstr(CEC_OP_UI_BCAST_TYPE_IP) ")\n"
	"                                  'ui-snd-pres-ctl' can have these values:\n"
	"                                      dual-mono (" xstr(CEC_OP_UI_SND_PRES_CTL_DUAL_MONO) ")\n"
	"                                      karaoke (" xstr(CEC_OP_UI_SND_PRES_CTL_KARAOKE) ")\n"
	"                                      downmix (" xstr(CEC_OP_UI_SND_PRES_CTL_DOWNMIX) ")\n"
	"                                      reverb (" xstr(CEC_OP_UI_SND_PRES_CTL_REVERB) ")\n"
	"                                      equalizer (" xstr(CEC_OP_UI_SND_PRES_CTL_EQUALIZER) ")\n"
	"                                      bass-up (" xstr(CEC_OP_UI_SND_PRES_CTL_BASS_UP) ")\n"
	"                                      bass-neutral (" xstr(CEC_OP_UI_SND_PRES_CTL_BASS_NEUTRAL) ")\n"
	"                                      bass-down (" xstr(CEC_OP_UI_SND_PRES_CTL_BASS_DOWN) ")\n"
	"                                      treble-up (" xstr(CEC_OP_UI_SND_PRES_CTL_TREBLE_UP) ")\n"
	"                                      treble-neutral (" xstr(CEC_OP_UI_SND_PRES_CTL_TREBLE_NEUTRAL) ")\n"
	"                                      treble-down (" xstr(CEC_OP_UI_SND_PRES_CTL_TREBLE_DOWN) ")\n"
	"                                  'channel-number-fmt' can have these values:\n"
	"                                      1-part (" xstr(CEC_OP_CHANNEL_NUMBER_FMT_1_PART) ")\n"
	"                                      2-part (" xstr(CEC_OP_CHANNEL_NUMBER_FMT_2_PART) ")\n"
	"                                  Send USER_CONTROL_PRESSED message (" xstr(CEC_MSG_USER_CONTROL_PRESSED) ")\n"
	"  --user-control-released         Send USER_CONTROL_RELEASED message (" xstr(CEC_MSG_USER_CONTROL_RELEASED) ")\n";

static const char *system_information_usage =
	"  --cec-version cec-version=<val>\n"
	"                                  'cec-version' can have these values:\n"
	"                                      version-1-3a (" xstr(CEC_OP_CEC_VERSION_1_3A) ")\n"
	"                                      version-1-4 (" xstr(CEC_OP_CEC_VERSION_1_4) ")\n"
	"                                      version-2-0 (" xstr(CEC_OP_CEC_VERSION_2_0) ")\n"
	"                                  Send CEC_VERSION message (" xstr(CEC_MSG_CEC_VERSION) ")\n"
	"  --get-cec-version               Send GET_CEC_VERSION message (" xstr(CEC_MSG_GET_CEC_VERSION) ")\n"
	"  --report-physical-addr phys-addr=<val>,prim-devtype=<val>\n"
	"                                  'prim-devtype' can have these values:\n"
	"                                      tv (" xstr(CEC_OP_PRIM_DEVTYPE_TV) ")\n"
	"                                      record (" xstr(CEC_OP_PRIM_DEVTYPE_RECORD) ")\n"
	"                                      tuner (" xstr(CEC_OP_PRIM_DEVTYPE_TUNER) ")\n"
	"                                      playback (" xstr(CEC_OP_PRIM_DEVTYPE_PLAYBACK) ")\n"
	"                                      audiosystem (" xstr(CEC_OP_PRIM_DEVTYPE_AUDIOSYSTEM) ")\n"
	"                                      switch (" xstr(CEC_OP_PRIM_DEVTYPE_SWITCH) ")\n"
	"                                      processor (" xstr(CEC_OP_PRIM_DEVTYPE_PROCESSOR) ")\n"
	"                                  Send REPORT_PHYSICAL_ADDR message (" xstr(CEC_MSG_REPORT_PHYSICAL_ADDR) ", bcast)\n"
	"  --give-physical-addr            Send GIVE_PHYSICAL_ADDR message (" xstr(CEC_MSG_GIVE_PHYSICAL_ADDR) ")\n"
	"  --set-menu-language language=<val>\n"
	"                                  Send SET_MENU_LANGUAGE message (" xstr(CEC_MSG_SET_MENU_LANGUAGE) ", bcast)\n"
	"  --get-menu-language             Send GET_MENU_LANGUAGE message (" xstr(CEC_MSG_GET_MENU_LANGUAGE) ")\n"
	"  --report-features cec-version=<val>,all-device-types=<val>,rc-profile=<val>,dev-features=<val>\n"
	"                                  'cec-version' can have these values:\n"
	"                                      version-1-3a (" xstr(CEC_OP_CEC_VERSION_1_3A) ")\n"
	"                                      version-1-4 (" xstr(CEC_OP_CEC_VERSION_1_4) ")\n"
	"                                      version-2-0 (" xstr(CEC_OP_CEC_VERSION_2_0) ")\n"
	"                                  'all-device-types' can have these values:\n"
	"                                      tv (" xstr(CEC_OP_ALL_DEVTYPE_TV) ")\n"
	"                                      record (" xstr(CEC_OP_ALL_DEVTYPE_RECORD) ")\n"
	"                                      tuner (" xstr(CEC_OP_ALL_DEVTYPE_TUNER) ")\n"
	"                                      playback (" xstr(CEC_OP_ALL_DEVTYPE_PLAYBACK) ")\n"
	"                                      audiosystem (" xstr(CEC_OP_ALL_DEVTYPE_AUDIOSYSTEM) ")\n"
	"                                      switch (" xstr(CEC_OP_ALL_DEVTYPE_SWITCH) ")\n"
	"                                  'rc-profile' can have these values:\n"
	"                                      tv-profile-none (" xstr(CEC_OP_FEAT_RC_TV_PROFILE_NONE) ")\n"
	"                                      tv-profile-1 (" xstr(CEC_OP_FEAT_RC_TV_PROFILE_1) ")\n"
	"                                      tv-profile-2 (" xstr(CEC_OP_FEAT_RC_TV_PROFILE_2) ")\n"
	"                                      tv-profile-3 (" xstr(CEC_OP_FEAT_RC_TV_PROFILE_3) ")\n"
	"                                      tv-profile-4 (" xstr(CEC_OP_FEAT_RC_TV_PROFILE_4) ")\n"
	"                                      src-has-dev-root-menu (" xstr(CEC_OP_FEAT_RC_SRC_HAS_DEV_ROOT_MENU) ")\n"
	"                                      src-has-dev-setup-menu (" xstr(CEC_OP_FEAT_RC_SRC_HAS_DEV_SETUP_MENU) ")\n"
	"                                      src-has-contents-menu (" xstr(CEC_OP_FEAT_RC_SRC_HAS_CONTENTS_MENU) ")\n"
	"                                      src-has-media-top-menu (" xstr(CEC_OP_FEAT_RC_SRC_HAS_MEDIA_TOP_MENU) ")\n"
	"                                      src-has-media-context-menu (" xstr(CEC_OP_FEAT_RC_SRC_HAS_MEDIA_CONTEXT_MENU) ")\n"
	"                                  'dev-features' can have these values:\n"
	"                                      has-record-tv-screen (" xstr(CEC_OP_FEAT_DEV_HAS_RECORD_TV_SCREEN) ")\n"
	"                                      has-set-osd-string (" xstr(CEC_OP_FEAT_DEV_HAS_SET_OSD_STRING) ")\n"
	"                                      has-deck-control (" xstr(CEC_OP_FEAT_DEV_HAS_DECK_CONTROL) ")\n"
	"                                      has-set-audio-rate (" xstr(CEC_OP_FEAT_DEV_HAS_SET_AUDIO_RATE) ")\n"
	"                                      sink-has-arc-tx (" xstr(CEC_OP_FEAT_DEV_SINK_HAS_ARC_TX) ")\n"
	"                                      source-has-arc-rx (" xstr(CEC_OP_FEAT_DEV_SOURCE_HAS_ARC_RX) ")\n"
	"                                  Send REPORT_FEATURES message (" xstr(CEC_MSG_REPORT_FEATURES) ", bcast)\n"
	"  --give-features                 Send GIVE_FEATURES message (" xstr(CEC_MSG_GIVE_FEATURES) ")\n";

static const char *timer_programming_usage =
	"  --timer-status timer-overlap-warning=<val>,media-info=<val>,prog-info=<val>,prog-error=<val>,duration-hr=<val>,duration-min=<val>\n"
	"                                  'timer-overlap-warning' can have these values:\n"
	"                                      no-overlap (" xstr(CEC_OP_TIMER_OVERLAP_WARNING_NO_OVERLAP) ")\n"
	"                                      overlap (" xstr(CEC_OP_TIMER_OVERLAP_WARNING_OVERLAP) ")\n"
	"                                  'media-info' can have these values:\n"
	"                                      unprot-media (" xstr(CEC_OP_MEDIA_INFO_UNPROT_MEDIA) ")\n"
	"                                      prot-media (" xstr(CEC_OP_MEDIA_INFO_PROT_MEDIA) ")\n"
	"                                      no-media (" xstr(CEC_OP_MEDIA_INFO_NO_MEDIA) ")\n"
	"                                  'prog-info' can have these values:\n"
	"                                      enough-space (" xstr(CEC_OP_PROG_INFO_ENOUGH_SPACE) ")\n"
	"                                      not-enough-space (" xstr(CEC_OP_PROG_INFO_NOT_ENOUGH_SPACE) ")\n"
	"                                      might-not-be-enough-space (" xstr(CEC_OP_PROG_INFO_MIGHT_NOT_BE_ENOUGH_SPACE) ")\n"
	"                                      none-available (" xstr(CEC_OP_PROG_INFO_NONE_AVAILABLE) ")\n"
	"                                  'prog-error' can have these values:\n"
	"                                      no-free-timer (" xstr(CEC_OP_PROG_ERROR_NO_FREE_TIMER) ")\n"
	"                                      date-out-of-range (" xstr(CEC_OP_PROG_ERROR_DATE_OUT_OF_RANGE) ")\n"
	"                                      rec-seq-error (" xstr(CEC_OP_PROG_ERROR_REC_SEQ_ERROR) ")\n"
	"                                      inv-ext-plug (" xstr(CEC_OP_PROG_ERROR_INV_EXT_PLUG) ")\n"
	"                                      inv-ext-phys-addr (" xstr(CEC_OP_PROG_ERROR_INV_EXT_PHYS_ADDR) ")\n"
	"                                      ca-unsupp (" xstr(CEC_OP_PROG_ERROR_CA_UNSUPP) ")\n"
	"                                      insuf-ca-entitlements (" xstr(CEC_OP_PROG_ERROR_INSUF_CA_ENTITLEMENTS) ")\n"
	"                                      resolution-unsupp (" xstr(CEC_OP_PROG_ERROR_RESOLUTION_UNSUPP) ")\n"
	"                                      parental-lock (" xstr(CEC_OP_PROG_ERROR_PARENTAL_LOCK) ")\n"
	"                                      clock-failure (" xstr(CEC_OP_PROG_ERROR_CLOCK_FAILURE) ")\n"
	"                                      duplicate (" xstr(CEC_OP_PROG_ERROR_DUPLICATE) ")\n"
	"                                  Send TIMER_STATUS message (" xstr(CEC_MSG_TIMER_STATUS) ")\n"
	"  --timer-cleared-status timer-cleared-status=<val>\n"
	"                                  'timer-cleared-status' can have these values:\n"
	"                                      recording (" xstr(CEC_OP_TIMER_CLR_STAT_RECORDING) ")\n"
	"                                      no-matching (" xstr(CEC_OP_TIMER_CLR_STAT_NO_MATCHING) ")\n"
	"                                      no-info (" xstr(CEC_OP_TIMER_CLR_STAT_NO_INFO) ")\n"
	"                                      cleared (" xstr(CEC_OP_TIMER_CLR_STAT_CLEARED) ")\n"
	"                                  Send TIMER_CLEARED_STATUS message (" xstr(CEC_MSG_TIMER_CLEARED_STATUS) ")\n"
	"  --clear-analogue-timer day=<val>,month=<val>,start-hr=<val>,start-min=<val>,duration-hr=<val>,duration-min=<val>,recording-seq=<val>,ana-bcast-type=<val>,ana-freq=<val>,bcast-system=<val>\n"
	"                                  'recording-seq' can have these values:\n"
	"                                      sunday (" xstr(CEC_OP_REC_SEQ_SUNDAY) ")\n"
	"                                      monday (" xstr(CEC_OP_REC_SEQ_MONDAY) ")\n"
	"                                      tuesday (" xstr(CEC_OP_REC_SEQ_TUESDAY) ")\n"
	"                                      wednesday (" xstr(CEC_OP_REC_SEQ_WEDNESDAY) ")\n"
	"                                      thursday (" xstr(CEC_OP_REC_SEQ_THURSDAY) ")\n"
	"                                      friday (" xstr(CEC_OP_REC_SEQ_FRIDAY) ")\n"
	"                                      saterday (" xstr(CEC_OP_REC_SEQ_SATERDAY) ")\n"
	"                                      once-only (" xstr(CEC_OP_REC_SEQ_ONCE_ONLY) ")\n"
	"                                  'ana-bcast-type' can have these values:\n"
	"                                      cable (" xstr(CEC_OP_ANA_BCAST_TYPE_CABLE) ")\n"
	"                                      satellite (" xstr(CEC_OP_ANA_BCAST_TYPE_SATELLITE) ")\n"
	"                                      terrestrial (" xstr(CEC_OP_ANA_BCAST_TYPE_TERRESTRIAL) ")\n"
	"                                  'bcast-system' can have these values:\n"
	"                                      pal-bg (" xstr(CEC_OP_BCAST_SYSTEM_PAL_BG) ")\n"
	"                                      secam-lq (" xstr(CEC_OP_BCAST_SYSTEM_SECAM_LQ) ")\n"
	"                                      pal-m (" xstr(CEC_OP_BCAST_SYSTEM_PAL_M) ")\n"
	"                                      ntsc-m (" xstr(CEC_OP_BCAST_SYSTEM_NTSC_M) ")\n"
	"                                      pal-i (" xstr(CEC_OP_BCAST_SYSTEM_PAL_I) ")\n"
	"                                      secam-dk (" xstr(CEC_OP_BCAST_SYSTEM_SECAM_DK) ")\n"
	"                                      secam-bg (" xstr(CEC_OP_BCAST_SYSTEM_SECAM_BG) ")\n"
	"                                      secam-l (" xstr(CEC_OP_BCAST_SYSTEM_SECAM_L) ")\n"
	"                                      pal-dk (" xstr(CEC_OP_BCAST_SYSTEM_PAL_DK) ")\n"
	"                                      other (" xstr(CEC_OP_BCAST_SYSTEM_OTHER) ")\n"
	"                                  Send CLEAR_ANALOGUE_TIMER message (" xstr(CEC_MSG_CLEAR_ANALOGUE_TIMER) ")\n"
	"  --clear-digital-timer day=<val>,month=<val>,start-hr=<val>,start-min=<val>,duration-hr=<val>,duration-min=<val>,recording-seq=<val>,service-id-method=<val>,dig-bcast-system=<val>,transport-id=<val>,service-id=<val>,orig-network-id=<val>,program-number=<val>,channel-number-fmt=<val>,major=<val>,minor=<val>\n"
	"                                  'recording-seq' can have these values:\n"
	"                                      sunday (" xstr(CEC_OP_REC_SEQ_SUNDAY) ")\n"
	"                                      monday (" xstr(CEC_OP_REC_SEQ_MONDAY) ")\n"
	"                                      tuesday (" xstr(CEC_OP_REC_SEQ_TUESDAY) ")\n"
	"                                      wednesday (" xstr(CEC_OP_REC_SEQ_WEDNESDAY) ")\n"
	"                                      thursday (" xstr(CEC_OP_REC_SEQ_THURSDAY) ")\n"
	"                                      friday (" xstr(CEC_OP_REC_SEQ_FRIDAY) ")\n"
	"                                      saterday (" xstr(CEC_OP_REC_SEQ_SATERDAY) ")\n"
	"                                      once-only (" xstr(CEC_OP_REC_SEQ_ONCE_ONLY) ")\n"
	"                                  'service-id-method' can have these values:\n"
	"                                      dig-id (" xstr(CEC_OP_SERVICE_ID_METHOD_BY_DIG_ID) ")\n"
	"                                      channel (" xstr(CEC_OP_SERVICE_ID_METHOD_BY_CHANNEL) ")\n"
	"                                  'dig-bcast-system' can have these values:\n"
	"                                      arib-gen (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ARIB_GEN) ")\n"
	"                                      atsc-gen (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ATSC_GEN) ")\n"
	"                                      dvb-gen (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_DVB_GEN) ")\n"
	"                                      arib-bs (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ARIB_BS) ")\n"
	"                                      arib-cs (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ARIB_CS) ")\n"
	"                                      arib-t (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ARIB_T) ")\n"
	"                                      atsc-cable (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ATSC_CABLE) ")\n"
	"                                      atsc-sat (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ATSC_SAT) ")\n"
	"                                      atsc-t (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ATSC_T) ")\n"
	"                                      dvb-c (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_DVB_C) ")\n"
	"                                      dvb-s (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_DVB_S) ")\n"
	"                                      dvb-s2 (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_DVB_S2) ")\n"
	"                                      dvb-t (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_DVB_T) ")\n"
	"                                  'channel-number-fmt' can have these values:\n"
	"                                      1-part (" xstr(CEC_OP_CHANNEL_NUMBER_FMT_1_PART) ")\n"
	"                                      2-part (" xstr(CEC_OP_CHANNEL_NUMBER_FMT_2_PART) ")\n"
	"                                  Send CLEAR_DIGITAL_TIMER message (" xstr(CEC_MSG_CLEAR_DIGITAL_TIMER) ")\n"
	"  --clear-ext-timer day=<val>,month=<val>,start-hr=<val>,start-min=<val>,duration-hr=<val>,duration-min=<val>,recording-seq=<val>,ext-src-spec=<val>,plug=<val>,phys-addr=<val>\n"
	"                                  'recording-seq' can have these values:\n"
	"                                      sunday (" xstr(CEC_OP_REC_SEQ_SUNDAY) ")\n"
	"                                      monday (" xstr(CEC_OP_REC_SEQ_MONDAY) ")\n"
	"                                      tuesday (" xstr(CEC_OP_REC_SEQ_TUESDAY) ")\n"
	"                                      wednesday (" xstr(CEC_OP_REC_SEQ_WEDNESDAY) ")\n"
	"                                      thursday (" xstr(CEC_OP_REC_SEQ_THURSDAY) ")\n"
	"                                      friday (" xstr(CEC_OP_REC_SEQ_FRIDAY) ")\n"
	"                                      saterday (" xstr(CEC_OP_REC_SEQ_SATERDAY) ")\n"
	"                                      once-only (" xstr(CEC_OP_REC_SEQ_ONCE_ONLY) ")\n"
	"                                  'ext-src-spec' can have these values:\n"
	"                                      plug (" xstr(CEC_OP_EXT_SRC_PLUG) ")\n"
	"                                      phys-addr (" xstr(CEC_OP_EXT_SRC_PHYS_ADDR) ")\n"
	"                                  Send CLEAR_EXT_TIMER message (" xstr(CEC_MSG_CLEAR_EXT_TIMER) ")\n"
	"  --set-analogue-timer day=<val>,month=<val>,start-hr=<val>,start-min=<val>,duration-hr=<val>,duration-min=<val>,recording-seq=<val>,ana-bcast-type=<val>,ana-freq=<val>,bcast-system=<val>\n"
	"                                  'recording-seq' can have these values:\n"
	"                                      sunday (" xstr(CEC_OP_REC_SEQ_SUNDAY) ")\n"
	"                                      monday (" xstr(CEC_OP_REC_SEQ_MONDAY) ")\n"
	"                                      tuesday (" xstr(CEC_OP_REC_SEQ_TUESDAY) ")\n"
	"                                      wednesday (" xstr(CEC_OP_REC_SEQ_WEDNESDAY) ")\n"
	"                                      thursday (" xstr(CEC_OP_REC_SEQ_THURSDAY) ")\n"
	"                                      friday (" xstr(CEC_OP_REC_SEQ_FRIDAY) ")\n"
	"                                      saterday (" xstr(CEC_OP_REC_SEQ_SATERDAY) ")\n"
	"                                      once-only (" xstr(CEC_OP_REC_SEQ_ONCE_ONLY) ")\n"
	"                                  'ana-bcast-type' can have these values:\n"
	"                                      cable (" xstr(CEC_OP_ANA_BCAST_TYPE_CABLE) ")\n"
	"                                      satellite (" xstr(CEC_OP_ANA_BCAST_TYPE_SATELLITE) ")\n"
	"                                      terrestrial (" xstr(CEC_OP_ANA_BCAST_TYPE_TERRESTRIAL) ")\n"
	"                                  'bcast-system' can have these values:\n"
	"                                      pal-bg (" xstr(CEC_OP_BCAST_SYSTEM_PAL_BG) ")\n"
	"                                      secam-lq (" xstr(CEC_OP_BCAST_SYSTEM_SECAM_LQ) ")\n"
	"                                      pal-m (" xstr(CEC_OP_BCAST_SYSTEM_PAL_M) ")\n"
	"                                      ntsc-m (" xstr(CEC_OP_BCAST_SYSTEM_NTSC_M) ")\n"
	"                                      pal-i (" xstr(CEC_OP_BCAST_SYSTEM_PAL_I) ")\n"
	"                                      secam-dk (" xstr(CEC_OP_BCAST_SYSTEM_SECAM_DK) ")\n"
	"                                      secam-bg (" xstr(CEC_OP_BCAST_SYSTEM_SECAM_BG) ")\n"
	"                                      secam-l (" xstr(CEC_OP_BCAST_SYSTEM_SECAM_L) ")\n"
	"                                      pal-dk (" xstr(CEC_OP_BCAST_SYSTEM_PAL_DK) ")\n"
	"                                      other (" xstr(CEC_OP_BCAST_SYSTEM_OTHER) ")\n"
	"                                  Send SET_ANALOGUE_TIMER message (" xstr(CEC_MSG_SET_ANALOGUE_TIMER) ")\n"
	"  --set-digital-timer day=<val>,month=<val>,start-hr=<val>,start-min=<val>,duration-hr=<val>,duration-min=<val>,recording-seq=<val>,service-id-method=<val>,dig-bcast-system=<val>,transport-id=<val>,service-id=<val>,orig-network-id=<val>,program-number=<val>,channel-number-fmt=<val>,major=<val>,minor=<val>\n"
	"                                  'recording-seq' can have these values:\n"
	"                                      sunday (" xstr(CEC_OP_REC_SEQ_SUNDAY) ")\n"
	"                                      monday (" xstr(CEC_OP_REC_SEQ_MONDAY) ")\n"
	"                                      tuesday (" xstr(CEC_OP_REC_SEQ_TUESDAY) ")\n"
	"                                      wednesday (" xstr(CEC_OP_REC_SEQ_WEDNESDAY) ")\n"
	"                                      thursday (" xstr(CEC_OP_REC_SEQ_THURSDAY) ")\n"
	"                                      friday (" xstr(CEC_OP_REC_SEQ_FRIDAY) ")\n"
	"                                      saterday (" xstr(CEC_OP_REC_SEQ_SATERDAY) ")\n"
	"                                      once-only (" xstr(CEC_OP_REC_SEQ_ONCE_ONLY) ")\n"
	"                                  'service-id-method' can have these values:\n"
	"                                      dig-id (" xstr(CEC_OP_SERVICE_ID_METHOD_BY_DIG_ID) ")\n"
	"                                      channel (" xstr(CEC_OP_SERVICE_ID_METHOD_BY_CHANNEL) ")\n"
	"                                  'dig-bcast-system' can have these values:\n"
	"                                      arib-gen (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ARIB_GEN) ")\n"
	"                                      atsc-gen (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ATSC_GEN) ")\n"
	"                                      dvb-gen (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_DVB_GEN) ")\n"
	"                                      arib-bs (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ARIB_BS) ")\n"
	"                                      arib-cs (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ARIB_CS) ")\n"
	"                                      arib-t (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ARIB_T) ")\n"
	"                                      atsc-cable (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ATSC_CABLE) ")\n"
	"                                      atsc-sat (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ATSC_SAT) ")\n"
	"                                      atsc-t (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ATSC_T) ")\n"
	"                                      dvb-c (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_DVB_C) ")\n"
	"                                      dvb-s (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_DVB_S) ")\n"
	"                                      dvb-s2 (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_DVB_S2) ")\n"
	"                                      dvb-t (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_DVB_T) ")\n"
	"                                  'channel-number-fmt' can have these values:\n"
	"                                      1-part (" xstr(CEC_OP_CHANNEL_NUMBER_FMT_1_PART) ")\n"
	"                                      2-part (" xstr(CEC_OP_CHANNEL_NUMBER_FMT_2_PART) ")\n"
	"                                  Send SET_DIGITAL_TIMER message (" xstr(CEC_MSG_SET_DIGITAL_TIMER) ")\n"
	"  --set-ext-timer day=<val>,month=<val>,start-hr=<val>,start-min=<val>,duration-hr=<val>,duration-min=<val>,recording-seq=<val>,ext-src-spec=<val>,plug=<val>,phys-addr=<val>\n"
	"                                  'recording-seq' can have these values:\n"
	"                                      sunday (" xstr(CEC_OP_REC_SEQ_SUNDAY) ")\n"
	"                                      monday (" xstr(CEC_OP_REC_SEQ_MONDAY) ")\n"
	"                                      tuesday (" xstr(CEC_OP_REC_SEQ_TUESDAY) ")\n"
	"                                      wednesday (" xstr(CEC_OP_REC_SEQ_WEDNESDAY) ")\n"
	"                                      thursday (" xstr(CEC_OP_REC_SEQ_THURSDAY) ")\n"
	"                                      friday (" xstr(CEC_OP_REC_SEQ_FRIDAY) ")\n"
	"                                      saterday (" xstr(CEC_OP_REC_SEQ_SATERDAY) ")\n"
	"                                      once-only (" xstr(CEC_OP_REC_SEQ_ONCE_ONLY) ")\n"
	"                                  'ext-src-spec' can have these values:\n"
	"                                      plug (" xstr(CEC_OP_EXT_SRC_PLUG) ")\n"
	"                                      phys-addr (" xstr(CEC_OP_EXT_SRC_PHYS_ADDR) ")\n"
	"                                  Send SET_EXT_TIMER message (" xstr(CEC_MSG_SET_EXT_TIMER) ")\n"
	"  --set-timer-program-title prog-title=<val>\n"
	"                                  Send SET_TIMER_PROGRAM_TITLE message (" xstr(CEC_MSG_SET_TIMER_PROGRAM_TITLE) ")\n";

static const char *tuner_control_usage =
	"  --tuner-device-status-analog rec-flag=<val>,tuner-display-info=<val>,ana-bcast-type=<val>,ana-freq=<val>,bcast-system=<val>\n"
	"                                  'rec-flag' can have these values:\n"
	"                                      used (" xstr(CEC_OP_REC_FLAG_USED) ")\n"
	"                                      not-used (" xstr(CEC_OP_REC_FLAG_NOT_USED) ")\n"
	"                                  'tuner-display-info' can have these values:\n"
	"                                      digital (" xstr(CEC_OP_TUNER_DISPLAY_INFO_DIGITAL) ")\n"
	"                                      none (" xstr(CEC_OP_TUNER_DISPLAY_INFO_NONE) ")\n"
	"                                      analogue (" xstr(CEC_OP_TUNER_DISPLAY_INFO_ANALOGUE) ")\n"
	"                                  'ana-bcast-type' can have these values:\n"
	"                                      cable (" xstr(CEC_OP_ANA_BCAST_TYPE_CABLE) ")\n"
	"                                      satellite (" xstr(CEC_OP_ANA_BCAST_TYPE_SATELLITE) ")\n"
	"                                      terrestrial (" xstr(CEC_OP_ANA_BCAST_TYPE_TERRESTRIAL) ")\n"
	"                                  'bcast-system' can have these values:\n"
	"                                      pal-bg (" xstr(CEC_OP_BCAST_SYSTEM_PAL_BG) ")\n"
	"                                      secam-lq (" xstr(CEC_OP_BCAST_SYSTEM_SECAM_LQ) ")\n"
	"                                      pal-m (" xstr(CEC_OP_BCAST_SYSTEM_PAL_M) ")\n"
	"                                      ntsc-m (" xstr(CEC_OP_BCAST_SYSTEM_NTSC_M) ")\n"
	"                                      pal-i (" xstr(CEC_OP_BCAST_SYSTEM_PAL_I) ")\n"
	"                                      secam-dk (" xstr(CEC_OP_BCAST_SYSTEM_SECAM_DK) ")\n"
	"                                      secam-bg (" xstr(CEC_OP_BCAST_SYSTEM_SECAM_BG) ")\n"
	"                                      secam-l (" xstr(CEC_OP_BCAST_SYSTEM_SECAM_L) ")\n"
	"                                      pal-dk (" xstr(CEC_OP_BCAST_SYSTEM_PAL_DK) ")\n"
	"                                      other (" xstr(CEC_OP_BCAST_SYSTEM_OTHER) ")\n"
	"                                  Send TUNER_DEVICE_STATUS message (" xstr(CEC_MSG_TUNER_DEVICE_STATUS) ")\n"
	"  --tuner-device-status-digital rec-flag=<val>,tuner-display-info=<val>,service-id-method=<val>,dig-bcast-system=<val>,transport-id=<val>,service-id=<val>,orig-network-id=<val>,program-number=<val>,channel-number-fmt=<val>,major=<val>,minor=<val>\n"
	"                                  'rec-flag' can have these values:\n"
	"                                      used (" xstr(CEC_OP_REC_FLAG_USED) ")\n"
	"                                      not-used (" xstr(CEC_OP_REC_FLAG_NOT_USED) ")\n"
	"                                  'tuner-display-info' can have these values:\n"
	"                                      digital (" xstr(CEC_OP_TUNER_DISPLAY_INFO_DIGITAL) ")\n"
	"                                      none (" xstr(CEC_OP_TUNER_DISPLAY_INFO_NONE) ")\n"
	"                                      analogue (" xstr(CEC_OP_TUNER_DISPLAY_INFO_ANALOGUE) ")\n"
	"                                  'service-id-method' can have these values:\n"
	"                                      dig-id (" xstr(CEC_OP_SERVICE_ID_METHOD_BY_DIG_ID) ")\n"
	"                                      channel (" xstr(CEC_OP_SERVICE_ID_METHOD_BY_CHANNEL) ")\n"
	"                                  'dig-bcast-system' can have these values:\n"
	"                                      arib-gen (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ARIB_GEN) ")\n"
	"                                      atsc-gen (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ATSC_GEN) ")\n"
	"                                      dvb-gen (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_DVB_GEN) ")\n"
	"                                      arib-bs (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ARIB_BS) ")\n"
	"                                      arib-cs (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ARIB_CS) ")\n"
	"                                      arib-t (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ARIB_T) ")\n"
	"                                      atsc-cable (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ATSC_CABLE) ")\n"
	"                                      atsc-sat (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ATSC_SAT) ")\n"
	"                                      atsc-t (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ATSC_T) ")\n"
	"                                      dvb-c (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_DVB_C) ")\n"
	"                                      dvb-s (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_DVB_S) ")\n"
	"                                      dvb-s2 (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_DVB_S2) ")\n"
	"                                      dvb-t (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_DVB_T) ")\n"
	"                                  'channel-number-fmt' can have these values:\n"
	"                                      1-part (" xstr(CEC_OP_CHANNEL_NUMBER_FMT_1_PART) ")\n"
	"                                      2-part (" xstr(CEC_OP_CHANNEL_NUMBER_FMT_2_PART) ")\n"
	"                                  Send TUNER_DEVICE_STATUS message (" xstr(CEC_MSG_TUNER_DEVICE_STATUS) ")\n"
	"  --give-tuner-device-status status-req=<val>\n"
	"                                  'status-req' can have these values:\n"
	"                                      on (" xstr(CEC_OP_STATUS_REQ_ON) ")\n"
	"                                      off (" xstr(CEC_OP_STATUS_REQ_OFF) ")\n"
	"                                      once (" xstr(CEC_OP_STATUS_REQ_ONCE) ")\n"
	"                                  Send GIVE_TUNER_DEVICE_STATUS message (" xstr(CEC_MSG_GIVE_TUNER_DEVICE_STATUS) ")\n"
	"  --select-analogue-service ana-bcast-type=<val>,ana-freq=<val>,bcast-system=<val>\n"
	"                                  'ana-bcast-type' can have these values:\n"
	"                                      cable (" xstr(CEC_OP_ANA_BCAST_TYPE_CABLE) ")\n"
	"                                      satellite (" xstr(CEC_OP_ANA_BCAST_TYPE_SATELLITE) ")\n"
	"                                      terrestrial (" xstr(CEC_OP_ANA_BCAST_TYPE_TERRESTRIAL) ")\n"
	"                                  'bcast-system' can have these values:\n"
	"                                      pal-bg (" xstr(CEC_OP_BCAST_SYSTEM_PAL_BG) ")\n"
	"                                      secam-lq (" xstr(CEC_OP_BCAST_SYSTEM_SECAM_LQ) ")\n"
	"                                      pal-m (" xstr(CEC_OP_BCAST_SYSTEM_PAL_M) ")\n"
	"                                      ntsc-m (" xstr(CEC_OP_BCAST_SYSTEM_NTSC_M) ")\n"
	"                                      pal-i (" xstr(CEC_OP_BCAST_SYSTEM_PAL_I) ")\n"
	"                                      secam-dk (" xstr(CEC_OP_BCAST_SYSTEM_SECAM_DK) ")\n"
	"                                      secam-bg (" xstr(CEC_OP_BCAST_SYSTEM_SECAM_BG) ")\n"
	"                                      secam-l (" xstr(CEC_OP_BCAST_SYSTEM_SECAM_L) ")\n"
	"                                      pal-dk (" xstr(CEC_OP_BCAST_SYSTEM_PAL_DK) ")\n"
	"                                      other (" xstr(CEC_OP_BCAST_SYSTEM_OTHER) ")\n"
	"                                  Send SELECT_ANALOGUE_SERVICE message (" xstr(CEC_MSG_SELECT_ANALOGUE_SERVICE) ")\n"
	"  --select-digital-service service-id-method=<val>,dig-bcast-system=<val>,transport-id=<val>,service-id=<val>,orig-network-id=<val>,program-number=<val>,channel-number-fmt=<val>,major=<val>,minor=<val>\n"
	"                                  'service-id-method' can have these values:\n"
	"                                      dig-id (" xstr(CEC_OP_SERVICE_ID_METHOD_BY_DIG_ID) ")\n"
	"                                      channel (" xstr(CEC_OP_SERVICE_ID_METHOD_BY_CHANNEL) ")\n"
	"                                  'dig-bcast-system' can have these values:\n"
	"                                      arib-gen (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ARIB_GEN) ")\n"
	"                                      atsc-gen (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ATSC_GEN) ")\n"
	"                                      dvb-gen (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_DVB_GEN) ")\n"
	"                                      arib-bs (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ARIB_BS) ")\n"
	"                                      arib-cs (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ARIB_CS) ")\n"
	"                                      arib-t (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ARIB_T) ")\n"
	"                                      atsc-cable (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ATSC_CABLE) ")\n"
	"                                      atsc-sat (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ATSC_SAT) ")\n"
	"                                      atsc-t (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ATSC_T) ")\n"
	"                                      dvb-c (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_DVB_C) ")\n"
	"                                      dvb-s (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_DVB_S) ")\n"
	"                                      dvb-s2 (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_DVB_S2) ")\n"
	"                                      dvb-t (" xstr(CEC_OP_DIG_SERVICE_BCAST_SYSTEM_DVB_T) ")\n"
	"                                  'channel-number-fmt' can have these values:\n"
	"                                      1-part (" xstr(CEC_OP_CHANNEL_NUMBER_FMT_1_PART) ")\n"
	"                                      2-part (" xstr(CEC_OP_CHANNEL_NUMBER_FMT_2_PART) ")\n"
	"                                  Send SELECT_DIGITAL_SERVICE message (" xstr(CEC_MSG_SELECT_DIGITAL_SERVICE) ")\n"
	"  --tuner-step-decrement          Send TUNER_STEP_DECREMENT message (" xstr(CEC_MSG_TUNER_STEP_DECREMENT) ")\n"
	"  --tuner-step-increment          Send TUNER_STEP_INCREMENT message (" xstr(CEC_MSG_TUNER_STEP_INCREMENT) ")\n";

static const char *vendor_specific_commands_usage =
	"  --device-vendor-id vendor-id=<val>\n"
	"                                  Send DEVICE_VENDOR_ID message (" xstr(CEC_MSG_DEVICE_VENDOR_ID) ", bcast)\n"
	"  --give-device-vendor-id         Send GIVE_DEVICE_VENDOR_ID message (" xstr(CEC_MSG_GIVE_DEVICE_VENDOR_ID) ")\n"
	VENDOR_EXTRA
	"  --vendor-remote-button-up       Send VENDOR_REMOTE_BUTTON_UP message (" xstr(CEC_MSG_VENDOR_REMOTE_BUTTON_UP) ")\n"
	"  --cec-version cec-version=<val>\n"
	"                                  'cec-version' can have these values:\n"
	"                                      version-1-3a (" xstr(CEC_OP_CEC_VERSION_1_3A) ")\n"
	"                                      version-1-4 (" xstr(CEC_OP_CEC_VERSION_1_4) ")\n"
	"                                      version-2-0 (" xstr(CEC_OP_CEC_VERSION_2_0) ")\n"
	"                                  Send CEC_VERSION message (" xstr(CEC_MSG_CEC_VERSION) ")\n"
	"  --get-cec-version               Send GET_CEC_VERSION message (" xstr(CEC_MSG_GET_CEC_VERSION) ")\n";

enum {
	OptMessages = 255,
	OptActiveSource,
	OptImageViewOn,
	OptTextViewOn,
	OptInactiveSource,
	OptRequestActiveSource,
	OptRoutingInformation,
	OptRoutingChange,
	OptSetStreamPath,
	OptStandby,
	OptRecordOff,
	OptRecordOnOwn,
	OptRecordOnDigital,
	OptRecordOnAnalog,
	OptRecordOnPlug,
	OptRecordOnPhysAddr,
	OptRecordStatus,
	OptRecordTvScreen,
	OptTimerStatus,
	OptTimerClearedStatus,
	OptClearAnalogueTimer,
	OptClearDigitalTimer,
	OptClearExtTimer,
	OptSetAnalogueTimer,
	OptSetDigitalTimer,
	OptSetExtTimer,
	OptSetTimerProgramTitle,
	OptCecVersion,
	OptGetCecVersion,
	OptReportPhysicalAddr,
	OptGivePhysicalAddr,
	OptSetMenuLanguage,
	OptGetMenuLanguage,
	OptReportFeatures,
	OptGiveFeatures,
	OptDeckControl,
	OptDeckStatus,
	OptGiveDeckStatus,
	OptPlay,
	OptTunerDeviceStatusAnalog,
	OptTunerDeviceStatusDigital,
	OptGiveTunerDeviceStatus,
	OptSelectAnalogueService,
	OptSelectDigitalService,
	OptTunerStepDecrement,
	OptTunerStepIncrement,
	OptDeviceVendorId,
	OptGiveDeviceVendorId,
	OptVendorRemoteButtonUp,
	OptSetOsdString,
	OptSetOsdName,
	OptGiveOsdName,
	OptMenuStatus,
	OptMenuRequest,
	OptUserControlPressed,
	OptUserControlReleased,
	OptReportPowerStatus,
	OptGiveDevicePowerStatus,
	OptFeatureAbort,
	OptAbort,
	OptReportAudioStatus,
	OptGiveAudioStatus,
	OptSetSystemAudioMode,
	OptSystemAudioModeRequest,
	OptSystemAudioModeStatus,
	OptGiveSystemAudioModeStatus,
	OptReportShortAudioDescriptor,
	OptRequestShortAudioDescriptor,
	OptSetAudioRate,
	OptReportArcInitiated,
	OptInitiateArc,
	OptRequestArcInitiation,
	OptReportArcTerminated,
	OptTerminateArc,
	OptRequestArcTermination,
	OptReportCurrentLatency,
	OptRequestCurrentLatency,
	OptCdcHecInquireState,
	OptCdcHecReportState,
	OptCdcHecSetState,
	OptCdcHecSetStateAdjacent,
	OptCdcHecRequestDeactivation,
	OptCdcHecNotifyAlive,
	OptCdcHecDiscover,
	OptCdcHpdSetState,
	OptCdcHpdReportState,
	OptHtngTuner_1partChan,
	OptHtngTuner_2partChan,
	OptHtngInputSelAv,
	OptHtngInputSelPc,
	OptHtngInputSelHdmi,
	OptHtngInputSelComponent,
	OptHtngInputSelDvi,
	OptHtngInputSelDp,
	OptHtngInputSelUsb,
	OptHtngSetDefPwrOnInputSrc,
	OptHtngSetTvSpeakers,
	OptHtngSetDigAudio,
	OptHtngSetAnaAudio,
	OptHtngSetDefPwrOnVol,
	OptHtngSetMaxVol,
	OptHtngSetMinVol,
	OptHtngSetBlueScreen,
	OptHtngSetBrightness,
	OptHtngSetColor,
	OptHtngSetContrast,
	OptHtngSetSharpness,
	OptHtngSetHue,
	OptHtngSetLedBacklight,
	OptHtngSetTvOsdControl,
	OptHtngSetAudioOnlyDisplay,
	OptHtngSetDate,
	OptHtngSetDateFormat,
	OptHtngSetTime,
	OptHtngSetClkBrightnessStandby,
	OptHtngSetClkBrightnessOn,
	OptHtngLedControl,
	OptHtngLockTvPwrButton,
	OptHtngLockTvVolButtons,
	OptHtngLockTvChanButtons,
	OptHtngLockTvInputButtons,
	OptHtngLockTvOtherButtons,
	OptHtngLockEverything,
	OptHtngLockEverythingButPwr,
	OptHtngHotelMode,
	OptHtngSetPwrSavingProfile,
	OptHtngSetSleepTimer,
	OptHtngSetWakeupTime,
	OptHtngSetAutoOffTime,
	OptHtngSetWakeupSrc,
	OptHtngSetInitWakeupVol,
	OptHtngClrAllSleepWake,
	OptHtngGlobalDirectTuneFreq,
	OptHtngGlobalDirectTuneChan,
	OptHtngGlobalDirectTuneExtFreq,
	OptHelpAll,
	OptHelpAbort,
	OptHelpAudioRateControl,
	OptHelpAudioReturnChannelControl,
	OptHelpCapabilityDiscoveryandControl,
	OptHelpDeckControl,
	OptHelpDeviceMenuControl,
	OptHelpDeviceOSDTransfer,
	OptHelpDynamicAudioLipsync,
	OptHelpHTNG,
	OptHelpOSDDisplay,
	OptHelpOneTouchPlay,
	OptHelpOneTouchRecord,
	OptHelpPowerStatus,
	OptHelpRoutingControl,
	OptHelpStandby,
	OptHelpSystemAudioControl,
	OptHelpSystemInformation,
	OptHelpTimerProgramming,
	OptHelpTunerControl,
	OptHelpVendorSpecificCommands,

	OptLast = 512
};

#define CEC_LONG_OPTS \
	{ "active-source", required_argument, 0, OptActiveSource }, \
	{ "image-view-on", no_argument, 0, OptImageViewOn }, \
	{ "text-view-on", no_argument, 0, OptTextViewOn }, \
	{ "inactive-source", required_argument, 0, OptInactiveSource }, \
	{ "request-active-source", no_argument, 0, OptRequestActiveSource }, \
	{ "routing-information", required_argument, 0, OptRoutingInformation }, \
	{ "routing-change", required_argument, 0, OptRoutingChange }, \
	{ "set-stream-path", required_argument, 0, OptSetStreamPath }, \
	{ "standby", no_argument, 0, OptStandby }, \
	{ "record-off", no_argument, 0, OptRecordOff }, \
	{ "record-on-own", no_argument, 0, OptRecordOnOwn }, \
	{ "record-on-digital", required_argument, 0, OptRecordOnDigital }, \
	{ "record-on-analog", required_argument, 0, OptRecordOnAnalog }, \
	{ "record-on-plug", required_argument, 0, OptRecordOnPlug }, \
	{ "record-on-phys-addr", required_argument, 0, OptRecordOnPhysAddr }, \
	{ "record-status", required_argument, 0, OptRecordStatus }, \
	{ "record-tv-screen", no_argument, 0, OptRecordTvScreen }, \
	{ "timer-status", required_argument, 0, OptTimerStatus }, \
	{ "timer-cleared-status", required_argument, 0, OptTimerClearedStatus }, \
	{ "clear-analogue-timer", required_argument, 0, OptClearAnalogueTimer }, \
	{ "clear-digital-timer", required_argument, 0, OptClearDigitalTimer }, \
	{ "clear-ext-timer", required_argument, 0, OptClearExtTimer }, \
	{ "set-analogue-timer", required_argument, 0, OptSetAnalogueTimer }, \
	{ "set-digital-timer", required_argument, 0, OptSetDigitalTimer }, \
	{ "set-ext-timer", required_argument, 0, OptSetExtTimer }, \
	{ "set-timer-program-title", required_argument, 0, OptSetTimerProgramTitle }, \
	{ "cec-version", required_argument, 0, OptCecVersion }, \
	{ "get-cec-version", no_argument, 0, OptGetCecVersion }, \
	{ "report-physical-addr", required_argument, 0, OptReportPhysicalAddr }, \
	{ "give-physical-addr", no_argument, 0, OptGivePhysicalAddr }, \
	{ "set-menu-language", required_argument, 0, OptSetMenuLanguage }, \
	{ "get-menu-language", no_argument, 0, OptGetMenuLanguage }, \
	{ "report-features", required_argument, 0, OptReportFeatures }, \
	{ "give-features", no_argument, 0, OptGiveFeatures }, \
	{ "deck-control", required_argument, 0, OptDeckControl }, \
	{ "deck-status", required_argument, 0, OptDeckStatus }, \
	{ "give-deck-status", required_argument, 0, OptGiveDeckStatus }, \
	{ "play", required_argument, 0, OptPlay }, \
	{ "tuner-device-status-analog", required_argument, 0, OptTunerDeviceStatusAnalog }, \
	{ "tuner-device-status-digital", required_argument, 0, OptTunerDeviceStatusDigital }, \
	{ "give-tuner-device-status", required_argument, 0, OptGiveTunerDeviceStatus }, \
	{ "select-analogue-service", required_argument, 0, OptSelectAnalogueService }, \
	{ "select-digital-service", required_argument, 0, OptSelectDigitalService }, \
	{ "tuner-step-decrement", no_argument, 0, OptTunerStepDecrement }, \
	{ "tuner-step-increment", no_argument, 0, OptTunerStepIncrement }, \
	{ "device-vendor-id", required_argument, 0, OptDeviceVendorId }, \
	{ "give-device-vendor-id", no_argument, 0, OptGiveDeviceVendorId }, \
	{ "vendor-remote-button-up", no_argument, 0, OptVendorRemoteButtonUp }, \
	{ "set-osd-string", required_argument, 0, OptSetOsdString }, \
	{ "set-osd-name", required_argument, 0, OptSetOsdName }, \
	{ "give-osd-name", no_argument, 0, OptGiveOsdName }, \
	{ "menu-status", required_argument, 0, OptMenuStatus }, \
	{ "menu-request", required_argument, 0, OptMenuRequest }, \
	{ "user-control-pressed", required_argument, 0, OptUserControlPressed }, \
	{ "user-control-released", no_argument, 0, OptUserControlReleased }, \
	{ "report-power-status", required_argument, 0, OptReportPowerStatus }, \
	{ "give-device-power-status", no_argument, 0, OptGiveDevicePowerStatus }, \
	{ "feature-abort", required_argument, 0, OptFeatureAbort }, \
	{ "abort", no_argument, 0, OptAbort }, \
	{ "report-audio-status", required_argument, 0, OptReportAudioStatus }, \
	{ "give-audio-status", no_argument, 0, OptGiveAudioStatus }, \
	{ "set-system-audio-mode", required_argument, 0, OptSetSystemAudioMode }, \
	{ "system-audio-mode-request", required_argument, 0, OptSystemAudioModeRequest }, \
	{ "system-audio-mode-status", required_argument, 0, OptSystemAudioModeStatus }, \
	{ "give-system-audio-mode-status", no_argument, 0, OptGiveSystemAudioModeStatus }, \
	{ "report-short-audio-descriptor", required_argument, 0, OptReportShortAudioDescriptor }, \
	{ "request-short-audio-descriptor", required_argument, 0, OptRequestShortAudioDescriptor }, \
	{ "set-audio-rate", required_argument, 0, OptSetAudioRate }, \
	{ "report-arc-initiated", no_argument, 0, OptReportArcInitiated }, \
	{ "initiate-arc", no_argument, 0, OptInitiateArc }, \
	{ "request-arc-initiation", no_argument, 0, OptRequestArcInitiation }, \
	{ "report-arc-terminated", no_argument, 0, OptReportArcTerminated }, \
	{ "terminate-arc", no_argument, 0, OptTerminateArc }, \
	{ "request-arc-termination", no_argument, 0, OptRequestArcTermination }, \
	{ "report-current-latency", required_argument, 0, OptReportCurrentLatency }, \
	{ "request-current-latency", required_argument, 0, OptRequestCurrentLatency }, \
	{ "cdc-hec-inquire-state", required_argument, 0, OptCdcHecInquireState }, \
	{ "cdc-hec-report-state", required_argument, 0, OptCdcHecReportState }, \
	{ "cdc-hec-set-state", required_argument, 0, OptCdcHecSetState }, \
	{ "cdc-hec-set-state-adjacent", required_argument, 0, OptCdcHecSetStateAdjacent }, \
	{ "cdc-hec-request-deactivation", required_argument, 0, OptCdcHecRequestDeactivation }, \
	{ "cdc-hec-notify-alive", no_argument, 0, OptCdcHecNotifyAlive }, \
	{ "cdc-hec-discover", no_argument, 0, OptCdcHecDiscover }, \
	{ "cdc-hpd-set-state", required_argument, 0, OptCdcHpdSetState }, \
	{ "cdc-hpd-report-state", required_argument, 0, OptCdcHpdReportState }, \
	{ "htng-tuner-1part-chan", required_argument, 0, OptHtngTuner_1partChan }, \
	{ "htng-tuner-2part-chan", required_argument, 0, OptHtngTuner_2partChan }, \
	{ "htng-input-sel-av", required_argument, 0, OptHtngInputSelAv }, \
	{ "htng-input-sel-pc", required_argument, 0, OptHtngInputSelPc }, \
	{ "htng-input-sel-hdmi", required_argument, 0, OptHtngInputSelHdmi }, \
	{ "htng-input-sel-component", required_argument, 0, OptHtngInputSelComponent }, \
	{ "htng-input-sel-dvi", required_argument, 0, OptHtngInputSelDvi }, \
	{ "htng-input-sel-dp", required_argument, 0, OptHtngInputSelDp }, \
	{ "htng-input-sel-usb", required_argument, 0, OptHtngInputSelUsb }, \
	{ "htng-set-def-pwr-on-input-src", required_argument, 0, OptHtngSetDefPwrOnInputSrc }, \
	{ "htng-set-tv-speakers", required_argument, 0, OptHtngSetTvSpeakers }, \
	{ "htng-set-dig-audio", required_argument, 0, OptHtngSetDigAudio }, \
	{ "htng-set-ana-audio", required_argument, 0, OptHtngSetAnaAudio }, \
	{ "htng-set-def-pwr-on-vol", required_argument, 0, OptHtngSetDefPwrOnVol }, \
	{ "htng-set-max-vol", required_argument, 0, OptHtngSetMaxVol }, \
	{ "htng-set-min-vol", required_argument, 0, OptHtngSetMinVol }, \
	{ "htng-set-blue-screen", required_argument, 0, OptHtngSetBlueScreen }, \
	{ "htng-set-brightness", required_argument, 0, OptHtngSetBrightness }, \
	{ "htng-set-color", required_argument, 0, OptHtngSetColor }, \
	{ "htng-set-contrast", required_argument, 0, OptHtngSetContrast }, \
	{ "htng-set-sharpness", required_argument, 0, OptHtngSetSharpness }, \
	{ "htng-set-hue", required_argument, 0, OptHtngSetHue }, \
	{ "htng-set-led-backlight", required_argument, 0, OptHtngSetLedBacklight }, \
	{ "htng-set-tv-osd-control", required_argument, 0, OptHtngSetTvOsdControl }, \
	{ "htng-set-audio-only-display", required_argument, 0, OptHtngSetAudioOnlyDisplay }, \
	{ "htng-set-date", required_argument, 0, OptHtngSetDate }, \
	{ "htng-set-date-format", required_argument, 0, OptHtngSetDateFormat }, \
	{ "htng-set-time", required_argument, 0, OptHtngSetTime }, \
	{ "htng-set-clk-brightness-standby", required_argument, 0, OptHtngSetClkBrightnessStandby }, \
	{ "htng-set-clk-brightness-on", required_argument, 0, OptHtngSetClkBrightnessOn }, \
	{ "htng-led-control", required_argument, 0, OptHtngLedControl }, \
	{ "htng-lock-tv-pwr-button", required_argument, 0, OptHtngLockTvPwrButton }, \
	{ "htng-lock-tv-vol-buttons", required_argument, 0, OptHtngLockTvVolButtons }, \
	{ "htng-lock-tv-chan-buttons", required_argument, 0, OptHtngLockTvChanButtons }, \
	{ "htng-lock-tv-input-buttons", required_argument, 0, OptHtngLockTvInputButtons }, \
	{ "htng-lock-tv-other-buttons", required_argument, 0, OptHtngLockTvOtherButtons }, \
	{ "htng-lock-everything", required_argument, 0, OptHtngLockEverything }, \
	{ "htng-lock-everything-but-pwr", required_argument, 0, OptHtngLockEverythingButPwr }, \
	{ "htng-hotel-mode", required_argument, 0, OptHtngHotelMode }, \
	{ "htng-set-pwr-saving-profile", required_argument, 0, OptHtngSetPwrSavingProfile }, \
	{ "htng-set-sleep-timer", required_argument, 0, OptHtngSetSleepTimer }, \
	{ "htng-set-wakeup-time", required_argument, 0, OptHtngSetWakeupTime }, \
	{ "htng-set-auto-off-time", required_argument, 0, OptHtngSetAutoOffTime }, \
	{ "htng-set-wakeup-src", required_argument, 0, OptHtngSetWakeupSrc }, \
	{ "htng-set-init-wakeup-vol", required_argument, 0, OptHtngSetInitWakeupVol }, \
	{ "htng-clr-all-sleep-wake", no_argument, 0, OptHtngClrAllSleepWake }, \
	{ "htng-global-direct-tune-freq", required_argument, 0, OptHtngGlobalDirectTuneFreq }, \
	{ "htng-global-direct-tune-chan", required_argument, 0, OptHtngGlobalDirectTuneChan }, \
	{ "htng-global-direct-tune-ext-freq", required_argument, 0, OptHtngGlobalDirectTuneExtFreq }, \
	{ "help-abort", no_argument, 0, OptHelpAbort }, \
	{ "help-audio-rate-control", no_argument, 0, OptHelpAudioRateControl }, \
	{ "help-audio-return-channel-control", no_argument, 0, OptHelpAudioReturnChannelControl }, \
	{ "help-capability-discovery-and-control", no_argument, 0, OptHelpCapabilityDiscoveryandControl }, \
	{ "help-deck-control", no_argument, 0, OptHelpDeckControl }, \
	{ "help-device-menu-control", no_argument, 0, OptHelpDeviceMenuControl }, \
	{ "help-device-osd-transfer", no_argument, 0, OptHelpDeviceOSDTransfer }, \
	{ "help-dynamic-audio-lipsync", no_argument, 0, OptHelpDynamicAudioLipsync }, \
	{ "help-htng", no_argument, 0, OptHelpHTNG }, \
	{ "help-osd-display", no_argument, 0, OptHelpOSDDisplay }, \
	{ "help-one-touch-play", no_argument, 0, OptHelpOneTouchPlay }, \
	{ "help-one-touch-record", no_argument, 0, OptHelpOneTouchRecord }, \
	{ "help-power-status", no_argument, 0, OptHelpPowerStatus }, \
	{ "help-routing-control", no_argument, 0, OptHelpRoutingControl }, \
	{ "help-standby", no_argument, 0, OptHelpStandby }, \
	{ "help-system-audio-control", no_argument, 0, OptHelpSystemAudioControl }, \
	{ "help-system-information", no_argument, 0, OptHelpSystemInformation }, \
	{ "help-timer-programming", no_argument, 0, OptHelpTimerProgramming }, \
	{ "help-tuner-control", no_argument, 0, OptHelpTunerControl }, \
	{ "help-vendor-specific-commands", no_argument, 0, OptHelpVendorSpecificCommands }, \


#define CEC_USAGE \
	"  --help-abort                        Show help for the Abort feature\n" \
	"  --help-audio-rate-control           Show help for the Audio Rate Control feature\n" \
	"  --help-audio-return-channel-control Show help for the Audio Return Channel Control feature\n" \
	"  --help-capability-discovery-and-control Show help for the Capability Discovery and Control feature\n" \
	"  --help-deck-control                 Show help for the Deck Control feature\n" \
	"  --help-device-menu-control          Show help for the Device Menu Control feature\n" \
	"  --help-device-osd-transfer          Show help for the Device OSD Transfer feature\n" \
	"  --help-dynamic-audio-lipsync        Show help for the Dynamic Audio Lipsync feature\n" \
	"  --help-htng                         Show help for the HTNG feature\n" \
	"  --help-osd-display                  Show help for the OSD Display feature\n" \
	"  --help-one-touch-play               Show help for the One Touch Play feature\n" \
	"  --help-one-touch-record             Show help for the One Touch Record feature\n" \
	"  --help-power-status                 Show help for the Power Status feature\n" \
	"  --help-routing-control              Show help for the Routing Control feature\n" \
	"  --help-standby                      Show help for the Standby feature\n" \
	"  --help-system-audio-control         Show help for the System Audio Control feature\n" \
	"  --help-system-information           Show help for the System Information feature\n" \
	"  --help-timer-programming            Show help for the Timer Programming feature\n" \
	"  --help-tuner-control                Show help for the Tuner Control feature\n" \
	"  --help-vendor-specific-commands     Show help for the Vendor Specific Commands feature\n" \


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
		OptActiveSource,
		1, { "phys-addr" },
		{ &arg_phys_addr },
		"ACTIVE_SOURCE"
	}, {
		CEC_MSG_IMAGE_VIEW_ON,
		OptImageViewOn,
		0, { }, { },
		"IMAGE_VIEW_ON"
	}, {
		CEC_MSG_TEXT_VIEW_ON,
		OptTextViewOn,
		0, { }, { },
		"TEXT_VIEW_ON"
	}, {
		CEC_MSG_INACTIVE_SOURCE,
		OptInactiveSource,
		1, { "phys-addr" },
		{ &arg_phys_addr },
		"INACTIVE_SOURCE"
	}, {
		CEC_MSG_REQUEST_ACTIVE_SOURCE,
		OptRequestActiveSource,
		0, { }, { },
		"REQUEST_ACTIVE_SOURCE"
	}, {
		CEC_MSG_ROUTING_INFORMATION,
		OptRoutingInformation,
		1, { "phys-addr" },
		{ &arg_phys_addr },
		"ROUTING_INFORMATION"
	}, {
		CEC_MSG_ROUTING_CHANGE,
		OptRoutingChange,
		2, { "orig-phys-addr", "new-phys-addr" },
		{ &arg_orig_phys_addr, &arg_new_phys_addr },
		"ROUTING_CHANGE"
	}, {
		CEC_MSG_SET_STREAM_PATH,
		OptSetStreamPath,
		1, { "phys-addr" },
		{ &arg_phys_addr },
		"SET_STREAM_PATH"
	}, {
		CEC_MSG_STANDBY,
		OptStandby,
		0, { }, { },
		"STANDBY"
	}, {
		CEC_MSG_RECORD_OFF,
		OptRecordOff,
		0, { }, { },
		"RECORD_OFF"
	}, {
		CEC_MSG_RECORD_ON,
		OptRecordOnOwn,
		0, { }, { },
		"RECORD_ON"
	}, {
		CEC_MSG_RECORD_ON,
		OptRecordOnDigital,
		9, { "service-id-method", "dig-bcast-system", "transport-id", "service-id", "orig-network-id", "program-number", "channel-number-fmt", "major", "minor" },
		{ &arg_service_id_method, &arg_dig_bcast_system, &arg_transport_id, &arg_service_id, &arg_orig_network_id, &arg_program_number, &arg_channel_number_fmt, &arg_major, &arg_minor },
		"RECORD_ON"
	}, {
		CEC_MSG_RECORD_ON,
		OptRecordOnAnalog,
		3, { "ana-bcast-type", "ana-freq", "bcast-system" },
		{ &arg_ana_bcast_type, &arg_ana_freq, &arg_bcast_system },
		"RECORD_ON"
	}, {
		CEC_MSG_RECORD_ON,
		OptRecordOnPlug,
		1, { "plug" },
		{ &arg_plug },
		"RECORD_ON"
	}, {
		CEC_MSG_RECORD_ON,
		OptRecordOnPhysAddr,
		1, { "phys-addr" },
		{ &arg_phys_addr },
		"RECORD_ON"
	}, {
		CEC_MSG_RECORD_STATUS,
		OptRecordStatus,
		1, { "rec-status" },
		{ &arg_rec_status },
		"RECORD_STATUS"
	}, {
		CEC_MSG_RECORD_TV_SCREEN,
		OptRecordTvScreen,
		0, { }, { },
		"RECORD_TV_SCREEN"
	}, {
		CEC_MSG_TIMER_STATUS,
		OptTimerStatus,
		6, { "timer-overlap-warning", "media-info", "prog-info", "prog-error", "duration-hr", "duration-min" },
		{ &arg_timer_overlap_warning, &arg_media_info, &arg_prog_info, &arg_prog_error, &arg_duration_hr, &arg_duration_min },
		"TIMER_STATUS"
	}, {
		CEC_MSG_TIMER_CLEARED_STATUS,
		OptTimerClearedStatus,
		1, { "timer-cleared-status" },
		{ &arg_timer_cleared_status },
		"TIMER_CLEARED_STATUS"
	}, {
		CEC_MSG_CLEAR_ANALOGUE_TIMER,
		OptClearAnalogueTimer,
		10, { "day", "month", "start-hr", "start-min", "duration-hr", "duration-min", "recording-seq", "ana-bcast-type", "ana-freq", "bcast-system" },
		{ &arg_day, &arg_month, &arg_start_hr, &arg_start_min, &arg_duration_hr, &arg_duration_min, &arg_recording_seq, &arg_ana_bcast_type, &arg_ana_freq, &arg_bcast_system },
		"CLEAR_ANALOGUE_TIMER"
	}, {
		CEC_MSG_CLEAR_DIGITAL_TIMER,
		OptClearDigitalTimer,
		16, { "day", "month", "start-hr", "start-min", "duration-hr", "duration-min", "recording-seq", "service-id-method", "dig-bcast-system", "transport-id", "service-id", "orig-network-id", "program-number", "channel-number-fmt", "major", "minor" },
		{ &arg_day, &arg_month, &arg_start_hr, &arg_start_min, &arg_duration_hr, &arg_duration_min, &arg_recording_seq, &arg_service_id_method, &arg_dig_bcast_system, &arg_transport_id, &arg_service_id, &arg_orig_network_id, &arg_program_number, &arg_channel_number_fmt, &arg_major, &arg_minor },
		"CLEAR_DIGITAL_TIMER"
	}, {
		CEC_MSG_CLEAR_EXT_TIMER,
		OptClearExtTimer,
		10, { "day", "month", "start-hr", "start-min", "duration-hr", "duration-min", "recording-seq", "ext-src-spec", "plug", "phys-addr" },
		{ &arg_day, &arg_month, &arg_start_hr, &arg_start_min, &arg_duration_hr, &arg_duration_min, &arg_recording_seq, &arg_ext_src_spec, &arg_plug, &arg_phys_addr },
		"CLEAR_EXT_TIMER"
	}, {
		CEC_MSG_SET_ANALOGUE_TIMER,
		OptSetAnalogueTimer,
		10, { "day", "month", "start-hr", "start-min", "duration-hr", "duration-min", "recording-seq", "ana-bcast-type", "ana-freq", "bcast-system" },
		{ &arg_day, &arg_month, &arg_start_hr, &arg_start_min, &arg_duration_hr, &arg_duration_min, &arg_recording_seq, &arg_ana_bcast_type, &arg_ana_freq, &arg_bcast_system },
		"SET_ANALOGUE_TIMER"
	}, {
		CEC_MSG_SET_DIGITAL_TIMER,
		OptSetDigitalTimer,
		16, { "day", "month", "start-hr", "start-min", "duration-hr", "duration-min", "recording-seq", "service-id-method", "dig-bcast-system", "transport-id", "service-id", "orig-network-id", "program-number", "channel-number-fmt", "major", "minor" },
		{ &arg_day, &arg_month, &arg_start_hr, &arg_start_min, &arg_duration_hr, &arg_duration_min, &arg_recording_seq, &arg_service_id_method, &arg_dig_bcast_system, &arg_transport_id, &arg_service_id, &arg_orig_network_id, &arg_program_number, &arg_channel_number_fmt, &arg_major, &arg_minor },
		"SET_DIGITAL_TIMER"
	}, {
		CEC_MSG_SET_EXT_TIMER,
		OptSetExtTimer,
		10, { "day", "month", "start-hr", "start-min", "duration-hr", "duration-min", "recording-seq", "ext-src-spec", "plug", "phys-addr" },
		{ &arg_day, &arg_month, &arg_start_hr, &arg_start_min, &arg_duration_hr, &arg_duration_min, &arg_recording_seq, &arg_ext_src_spec, &arg_plug, &arg_phys_addr },
		"SET_EXT_TIMER"
	}, {
		CEC_MSG_SET_TIMER_PROGRAM_TITLE,
		OptSetTimerProgramTitle,
		1, { "prog-title" },
		{ &arg_prog_title },
		"SET_TIMER_PROGRAM_TITLE"
	}, {
		CEC_MSG_CEC_VERSION,
		OptCecVersion,
		1, { "cec-version" },
		{ &arg_cec_version },
		"CEC_VERSION"
	}, {
		CEC_MSG_GET_CEC_VERSION,
		OptGetCecVersion,
		0, { }, { },
		"GET_CEC_VERSION"
	}, {
		CEC_MSG_REPORT_PHYSICAL_ADDR,
		OptReportPhysicalAddr,
		2, { "phys-addr", "prim-devtype" },
		{ &arg_phys_addr, &arg_prim_devtype },
		"REPORT_PHYSICAL_ADDR"
	}, {
		CEC_MSG_GIVE_PHYSICAL_ADDR,
		OptGivePhysicalAddr,
		0, { }, { },
		"GIVE_PHYSICAL_ADDR"
	}, {
		CEC_MSG_SET_MENU_LANGUAGE,
		OptSetMenuLanguage,
		1, { "language" },
		{ &arg_language },
		"SET_MENU_LANGUAGE"
	}, {
		CEC_MSG_GET_MENU_LANGUAGE,
		OptGetMenuLanguage,
		0, { }, { },
		"GET_MENU_LANGUAGE"
	}, {
		CEC_MSG_REPORT_FEATURES,
		OptReportFeatures,
		4, { "cec-version", "all-device-types", "rc-profile", "dev-features" },
		{ &arg_cec_version, &arg_all_device_types, &arg_rc_profile, &arg_dev_features },
		"REPORT_FEATURES"
	}, {
		CEC_MSG_GIVE_FEATURES,
		OptGiveFeatures,
		0, { }, { },
		"GIVE_FEATURES"
	}, {
		CEC_MSG_DECK_CONTROL,
		OptDeckControl,
		1, { "deck-control-mode" },
		{ &arg_deck_control_mode },
		"DECK_CONTROL"
	}, {
		CEC_MSG_DECK_STATUS,
		OptDeckStatus,
		1, { "deck-info" },
		{ &arg_deck_info },
		"DECK_STATUS"
	}, {
		CEC_MSG_GIVE_DECK_STATUS,
		OptGiveDeckStatus,
		1, { "status-req" },
		{ &arg_status_req },
		"GIVE_DECK_STATUS"
	}, {
		CEC_MSG_PLAY,
		OptPlay,
		1, { "play-mode" },
		{ &arg_play_mode },
		"PLAY"
	}, {
		CEC_MSG_TUNER_DEVICE_STATUS,
		OptTunerDeviceStatusAnalog,
		5, { "rec-flag", "tuner-display-info", "ana-bcast-type", "ana-freq", "bcast-system" },
		{ &arg_rec_flag, &arg_tuner_display_info, &arg_ana_bcast_type, &arg_ana_freq, &arg_bcast_system },
		"TUNER_DEVICE_STATUS"
	}, {
		CEC_MSG_TUNER_DEVICE_STATUS,
		OptTunerDeviceStatusDigital,
		11, { "rec-flag", "tuner-display-info", "service-id-method", "dig-bcast-system", "transport-id", "service-id", "orig-network-id", "program-number", "channel-number-fmt", "major", "minor" },
		{ &arg_rec_flag, &arg_tuner_display_info, &arg_service_id_method, &arg_dig_bcast_system, &arg_transport_id, &arg_service_id, &arg_orig_network_id, &arg_program_number, &arg_channel_number_fmt, &arg_major, &arg_minor },
		"TUNER_DEVICE_STATUS"
	}, {
		CEC_MSG_GIVE_TUNER_DEVICE_STATUS,
		OptGiveTunerDeviceStatus,
		1, { "status-req" },
		{ &arg_status_req },
		"GIVE_TUNER_DEVICE_STATUS"
	}, {
		CEC_MSG_SELECT_ANALOGUE_SERVICE,
		OptSelectAnalogueService,
		3, { "ana-bcast-type", "ana-freq", "bcast-system" },
		{ &arg_ana_bcast_type, &arg_ana_freq, &arg_bcast_system },
		"SELECT_ANALOGUE_SERVICE"
	}, {
		CEC_MSG_SELECT_DIGITAL_SERVICE,
		OptSelectDigitalService,
		9, { "service-id-method", "dig-bcast-system", "transport-id", "service-id", "orig-network-id", "program-number", "channel-number-fmt", "major", "minor" },
		{ &arg_service_id_method, &arg_dig_bcast_system, &arg_transport_id, &arg_service_id, &arg_orig_network_id, &arg_program_number, &arg_channel_number_fmt, &arg_major, &arg_minor },
		"SELECT_DIGITAL_SERVICE"
	}, {
		CEC_MSG_TUNER_STEP_DECREMENT,
		OptTunerStepDecrement,
		0, { }, { },
		"TUNER_STEP_DECREMENT"
	}, {
		CEC_MSG_TUNER_STEP_INCREMENT,
		OptTunerStepIncrement,
		0, { }, { },
		"TUNER_STEP_INCREMENT"
	}, {
		CEC_MSG_DEVICE_VENDOR_ID,
		OptDeviceVendorId,
		1, { "vendor-id" },
		{ &arg_vendor_id },
		"DEVICE_VENDOR_ID"
	}, {
		CEC_MSG_GIVE_DEVICE_VENDOR_ID,
		OptGiveDeviceVendorId,
		0, { }, { },
		"GIVE_DEVICE_VENDOR_ID"
	}, {
		CEC_MSG_VENDOR_REMOTE_BUTTON_UP,
		OptVendorRemoteButtonUp,
		0, { }, { },
		"VENDOR_REMOTE_BUTTON_UP"
	}, {
		CEC_MSG_SET_OSD_STRING,
		OptSetOsdString,
		2, { "disp-ctl", "osd" },
		{ &arg_disp_ctl, &arg_osd },
		"SET_OSD_STRING"
	}, {
		CEC_MSG_SET_OSD_NAME,
		OptSetOsdName,
		1, { "name" },
		{ &arg_name },
		"SET_OSD_NAME"
	}, {
		CEC_MSG_GIVE_OSD_NAME,
		OptGiveOsdName,
		0, { }, { },
		"GIVE_OSD_NAME"
	}, {
		CEC_MSG_MENU_STATUS,
		OptMenuStatus,
		1, { "menu-state" },
		{ &arg_menu_state },
		"MENU_STATUS"
	}, {
		CEC_MSG_MENU_REQUEST,
		OptMenuRequest,
		1, { "menu-req" },
		{ &arg_menu_req },
		"MENU_REQUEST"
	}, {
		CEC_MSG_USER_CONTROL_PRESSED,
		OptUserControlPressed,
		11, { "ui-cmd", "has-opt-arg", "play-mode", "ui-function-media", "ui-function-select-av-input", "ui-function-select-audio-input", "ui-bcast-type", "ui-snd-pres-ctl", "channel-number-fmt", "major", "minor" },
		{ &arg_rc_ui_cmd, &arg_has_opt_arg, &arg_play_mode, &arg_ui_function_media, &arg_ui_function_select_av_input, &arg_ui_function_select_audio_input, &arg_ui_bcast_type, &arg_ui_snd_pres_ctl, &arg_channel_number_fmt, &arg_major, &arg_minor },
		"USER_CONTROL_PRESSED"
	}, {
		CEC_MSG_USER_CONTROL_RELEASED,
		OptUserControlReleased,
		0, { }, { },
		"USER_CONTROL_RELEASED"
	}, {
		CEC_MSG_REPORT_POWER_STATUS,
		OptReportPowerStatus,
		1, { "pwr-state" },
		{ &arg_pwr_state },
		"REPORT_POWER_STATUS"
	}, {
		CEC_MSG_GIVE_DEVICE_POWER_STATUS,
		OptGiveDevicePowerStatus,
		0, { }, { },
		"GIVE_DEVICE_POWER_STATUS"
	}, {
		CEC_MSG_FEATURE_ABORT,
		OptFeatureAbort,
		2, { "abort-msg", "reason" },
		{ &arg_abort_msg, &arg_reason },
		"FEATURE_ABORT"
	}, {
		CEC_MSG_ABORT,
		OptAbort,
		0, { }, { },
		"ABORT"
	}, {
		CEC_MSG_REPORT_AUDIO_STATUS,
		OptReportAudioStatus,
		2, { "aud-mute-status", "aud-vol-status" },
		{ &arg_aud_mute_status, &arg_aud_vol_status },
		"REPORT_AUDIO_STATUS"
	}, {
		CEC_MSG_GIVE_AUDIO_STATUS,
		OptGiveAudioStatus,
		0, { }, { },
		"GIVE_AUDIO_STATUS"
	}, {
		CEC_MSG_SET_SYSTEM_AUDIO_MODE,
		OptSetSystemAudioMode,
		1, { "sys-aud-status" },
		{ &arg_sys_aud_status },
		"SET_SYSTEM_AUDIO_MODE"
	}, {
		CEC_MSG_SYSTEM_AUDIO_MODE_REQUEST,
		OptSystemAudioModeRequest,
		1, { "phys-addr" },
		{ &arg_phys_addr },
		"SYSTEM_AUDIO_MODE_REQUEST"
	}, {
		CEC_MSG_SYSTEM_AUDIO_MODE_STATUS,
		OptSystemAudioModeStatus,
		1, { "sys-aud-status" },
		{ &arg_sys_aud_status },
		"SYSTEM_AUDIO_MODE_STATUS"
	}, {
		CEC_MSG_GIVE_SYSTEM_AUDIO_MODE_STATUS,
		OptGiveSystemAudioModeStatus,
		0, { }, { },
		"GIVE_SYSTEM_AUDIO_MODE_STATUS"
	}, {
		CEC_MSG_REPORT_SHORT_AUDIO_DESCRIPTOR,
		OptReportShortAudioDescriptor,
		5, { "num-descriptors", "descriptor1", "descriptor2", "descriptor3", "descriptor4" },
		{ &arg_num_descriptors, &arg_descriptor1, &arg_descriptor2, &arg_descriptor3, &arg_descriptor4 },
		"REPORT_SHORT_AUDIO_DESCRIPTOR"
	}, {
		CEC_MSG_REQUEST_SHORT_AUDIO_DESCRIPTOR,
		OptRequestShortAudioDescriptor,
		9, { "num-descriptors", "audio-format-id1", "audio-format-code1", "audio-format-id2", "audio-format-code2", "audio-format-id3", "audio-format-code3", "audio-format-id4", "audio-format-code4" },
		{ &arg_num_descriptors, &arg_audio_format_id1, &arg_audio_format_code1, &arg_audio_format_id2, &arg_audio_format_code2, &arg_audio_format_id3, &arg_audio_format_code3, &arg_audio_format_id4, &arg_audio_format_code4 },
		"REQUEST_SHORT_AUDIO_DESCRIPTOR"
	}, {
		CEC_MSG_SET_AUDIO_RATE,
		OptSetAudioRate,
		1, { "audio-rate" },
		{ &arg_audio_rate },
		"SET_AUDIO_RATE"
	}, {
		CEC_MSG_REPORT_ARC_INITIATED,
		OptReportArcInitiated,
		0, { }, { },
		"REPORT_ARC_INITIATED"
	}, {
		CEC_MSG_INITIATE_ARC,
		OptInitiateArc,
		0, { }, { },
		"INITIATE_ARC"
	}, {
		CEC_MSG_REQUEST_ARC_INITIATION,
		OptRequestArcInitiation,
		0, { }, { },
		"REQUEST_ARC_INITIATION"
	}, {
		CEC_MSG_REPORT_ARC_TERMINATED,
		OptReportArcTerminated,
		0, { }, { },
		"REPORT_ARC_TERMINATED"
	}, {
		CEC_MSG_TERMINATE_ARC,
		OptTerminateArc,
		0, { }, { },
		"TERMINATE_ARC"
	}, {
		CEC_MSG_REQUEST_ARC_TERMINATION,
		OptRequestArcTermination,
		0, { }, { },
		"REQUEST_ARC_TERMINATION"
	}, {
		CEC_MSG_REPORT_CURRENT_LATENCY,
		OptReportCurrentLatency,
		5, { "phys-addr", "video-latency", "low-latency-mode", "audio-out-compensated", "audio-out-delay" },
		{ &arg_phys_addr, &arg_video_latency, &arg_low_latency_mode, &arg_audio_out_compensated, &arg_audio_out_delay },
		"REPORT_CURRENT_LATENCY"
	}, {
		CEC_MSG_REQUEST_CURRENT_LATENCY,
		OptRequestCurrentLatency,
		1, { "phys-addr" },
		{ &arg_phys_addr },
		"REQUEST_CURRENT_LATENCY"
	}, {
		CEC_MSG_CDC_HEC_INQUIRE_STATE,
		OptCdcHecInquireState,
		2, { "phys-addr1", "phys-addr2" },
		{ &arg_phys_addr1, &arg_phys_addr2 },
		"CDC_HEC_INQUIRE_STATE"
	}, {
		CEC_MSG_CDC_HEC_REPORT_STATE,
		OptCdcHecReportState,
		7, { "target-phys-addr", "hec-func-state", "host-func-state", "enc-func-state", "cdc-errcode", "has-field", "hec-field" },
		{ &arg_target_phys_addr, &arg_hec_func_state, &arg_host_func_state, &arg_enc_func_state, &arg_cdc_errcode, &arg_has_field, &arg_hec_field },
		"CDC_HEC_REPORT_STATE"
	}, {
		CEC_MSG_CDC_HEC_SET_STATE,
		OptCdcHecSetState,
		6, { "phys-addr1", "phys-addr2", "hec-set-state", "phys-addr3", "phys-addr4", "phys-addr5" },
		{ &arg_phys_addr1, &arg_phys_addr2, &arg_hec_set_state, &arg_phys_addr3, &arg_phys_addr4, &arg_phys_addr5 },
		"CDC_HEC_SET_STATE"
	}, {
		CEC_MSG_CDC_HEC_SET_STATE_ADJACENT,
		OptCdcHecSetStateAdjacent,
		2, { "phys-addr1", "hec-set-state" },
		{ &arg_phys_addr1, &arg_hec_set_state },
		"CDC_HEC_SET_STATE_ADJACENT"
	}, {
		CEC_MSG_CDC_HEC_REQUEST_DEACTIVATION,
		OptCdcHecRequestDeactivation,
		3, { "phys-addr1", "phys-addr2", "phys-addr3" },
		{ &arg_phys_addr1, &arg_phys_addr2, &arg_phys_addr3 },
		"CDC_HEC_REQUEST_DEACTIVATION"
	}, {
		CEC_MSG_CDC_HEC_NOTIFY_ALIVE,
		OptCdcHecNotifyAlive,
		0, { }, { },
		"CDC_HEC_NOTIFY_ALIVE"
	}, {
		CEC_MSG_CDC_HEC_DISCOVER,
		OptCdcHecDiscover,
		0, { }, { },
		"CDC_HEC_DISCOVER"
	}, {
		CEC_MSG_CDC_HPD_SET_STATE,
		OptCdcHpdSetState,
		2, { "input-port", "hpd-state" },
		{ &arg_input_port, &arg_hpd_state },
		"CDC_HPD_SET_STATE"
	}, {
		CEC_MSG_CDC_HPD_REPORT_STATE,
		OptCdcHpdReportState,
		2, { "hpd-state", "hpd-error" },
		{ &arg_hpd_state, &arg_hpd_error },
		"CDC_HPD_REPORT_STATE"
	}, {
		CEC_MSG_HTNG_TUNER_1PART_CHAN,
		OptHtngTuner_1partChan,
		2, { "htng-tuner-type", "chan" },
		{ &arg_htng_tuner_type, &arg_chan },
		"HTNG_TUNER_1PART_CHAN"
	}, {
		CEC_MSG_HTNG_TUNER_2PART_CHAN,
		OptHtngTuner_2partChan,
		3, { "htng-tuner-type", "major-chan", "minor-chan" },
		{ &arg_htng_tuner_type, &arg_major_chan, &arg_minor_chan },
		"HTNG_TUNER_2PART_CHAN"
	}, {
		CEC_MSG_HTNG_INPUT_SEL_AV,
		OptHtngInputSelAv,
		1, { "input" },
		{ &arg_input },
		"HTNG_INPUT_SEL_AV"
	}, {
		CEC_MSG_HTNG_INPUT_SEL_PC,
		OptHtngInputSelPc,
		1, { "input" },
		{ &arg_input },
		"HTNG_INPUT_SEL_PC"
	}, {
		CEC_MSG_HTNG_INPUT_SEL_HDMI,
		OptHtngInputSelHdmi,
		1, { "input" },
		{ &arg_input },
		"HTNG_INPUT_SEL_HDMI"
	}, {
		CEC_MSG_HTNG_INPUT_SEL_COMPONENT,
		OptHtngInputSelComponent,
		1, { "input" },
		{ &arg_input },
		"HTNG_INPUT_SEL_COMPONENT"
	}, {
		CEC_MSG_HTNG_INPUT_SEL_DVI,
		OptHtngInputSelDvi,
		1, { "input" },
		{ &arg_input },
		"HTNG_INPUT_SEL_DVI"
	}, {
		CEC_MSG_HTNG_INPUT_SEL_DP,
		OptHtngInputSelDp,
		1, { "input" },
		{ &arg_input },
		"HTNG_INPUT_SEL_DP"
	}, {
		CEC_MSG_HTNG_INPUT_SEL_USB,
		OptHtngInputSelUsb,
		1, { "input" },
		{ &arg_input },
		"HTNG_INPUT_SEL_USB"
	}, {
		CEC_MSG_HTNG_SET_DEF_PWR_ON_INPUT_SRC,
		OptHtngSetDefPwrOnInputSrc,
		4, { "htng-input-src", "htng-tuner-type", "major", "input" },
		{ &arg_htng_input_src, &arg_htng_tuner_type, &arg_major, &arg_input },
		"HTNG_SET_DEF_PWR_ON_INPUT_SRC"
	}, {
		CEC_MSG_HTNG_SET_TV_SPEAKERS,
		OptHtngSetTvSpeakers,
		1, { "on" },
		{ &arg_on },
		"HTNG_SET_TV_SPEAKERS"
	}, {
		CEC_MSG_HTNG_SET_DIG_AUDIO,
		OptHtngSetDigAudio,
		1, { "on" },
		{ &arg_on },
		"HTNG_SET_DIG_AUDIO"
	}, {
		CEC_MSG_HTNG_SET_ANA_AUDIO,
		OptHtngSetAnaAudio,
		1, { "on" },
		{ &arg_on },
		"HTNG_SET_ANA_AUDIO"
	}, {
		CEC_MSG_HTNG_SET_DEF_PWR_ON_VOL,
		OptHtngSetDefPwrOnVol,
		1, { "vol" },
		{ &arg_vol },
		"HTNG_SET_DEF_PWR_ON_VOL"
	}, {
		CEC_MSG_HTNG_SET_MAX_VOL,
		OptHtngSetMaxVol,
		1, { "vol" },
		{ &arg_vol },
		"HTNG_SET_MAX_VOL"
	}, {
		CEC_MSG_HTNG_SET_MIN_VOL,
		OptHtngSetMinVol,
		1, { "vol" },
		{ &arg_vol },
		"HTNG_SET_MIN_VOL"
	}, {
		CEC_MSG_HTNG_SET_BLUE_SCREEN,
		OptHtngSetBlueScreen,
		1, { "blue" },
		{ &arg_blue },
		"HTNG_SET_BLUE_SCREEN"
	}, {
		CEC_MSG_HTNG_SET_BRIGHTNESS,
		OptHtngSetBrightness,
		1, { "brightness" },
		{ &arg_brightness },
		"HTNG_SET_BRIGHTNESS"
	}, {
		CEC_MSG_HTNG_SET_COLOR,
		OptHtngSetColor,
		1, { "color" },
		{ &arg_color },
		"HTNG_SET_COLOR"
	}, {
		CEC_MSG_HTNG_SET_CONTRAST,
		OptHtngSetContrast,
		1, { "contrast" },
		{ &arg_contrast },
		"HTNG_SET_CONTRAST"
	}, {
		CEC_MSG_HTNG_SET_SHARPNESS,
		OptHtngSetSharpness,
		1, { "sharpness" },
		{ &arg_sharpness },
		"HTNG_SET_SHARPNESS"
	}, {
		CEC_MSG_HTNG_SET_HUE,
		OptHtngSetHue,
		1, { "hue" },
		{ &arg_hue },
		"HTNG_SET_HUE"
	}, {
		CEC_MSG_HTNG_SET_LED_BACKLIGHT,
		OptHtngSetLedBacklight,
		1, { "led-backlight" },
		{ &arg_led_backlight },
		"HTNG_SET_LED_BACKLIGHT"
	}, {
		CEC_MSG_HTNG_SET_TV_OSD_CONTROL,
		OptHtngSetTvOsdControl,
		1, { "on" },
		{ &arg_on },
		"HTNG_SET_TV_OSD_CONTROL"
	}, {
		CEC_MSG_HTNG_SET_AUDIO_ONLY_DISPLAY,
		OptHtngSetAudioOnlyDisplay,
		1, { "on" },
		{ &arg_on },
		"HTNG_SET_AUDIO_ONLY_DISPLAY"
	}, {
		CEC_MSG_HTNG_SET_DATE,
		OptHtngSetDate,
		1, { "date" },
		{ &arg_date },
		"HTNG_SET_DATE"
	}, {
		CEC_MSG_HTNG_SET_DATE_FORMAT,
		OptHtngSetDateFormat,
		1, { "ddmm" },
		{ &arg_ddmm },
		"HTNG_SET_DATE_FORMAT"
	}, {
		CEC_MSG_HTNG_SET_TIME,
		OptHtngSetTime,
		1, { "time" },
		{ &arg_time },
		"HTNG_SET_TIME"
	}, {
		CEC_MSG_HTNG_SET_CLK_BRIGHTNESS_STANDBY,
		OptHtngSetClkBrightnessStandby,
		1, { "brightness" },
		{ &arg_brightness },
		"HTNG_SET_CLK_BRIGHTNESS_STANDBY"
	}, {
		CEC_MSG_HTNG_SET_CLK_BRIGHTNESS_ON,
		OptHtngSetClkBrightnessOn,
		1, { "brightness" },
		{ &arg_brightness },
		"HTNG_SET_CLK_BRIGHTNESS_ON"
	}, {
		CEC_MSG_HTNG_LED_CONTROL,
		OptHtngLedControl,
		1, { "htng-led-control" },
		{ &arg_htng_led_control },
		"HTNG_LED_CONTROL"
	}, {
		CEC_MSG_HTNG_LOCK_TV_PWR_BUTTON,
		OptHtngLockTvPwrButton,
		1, { "on" },
		{ &arg_on },
		"HTNG_LOCK_TV_PWR_BUTTON"
	}, {
		CEC_MSG_HTNG_LOCK_TV_VOL_BUTTONS,
		OptHtngLockTvVolButtons,
		1, { "on" },
		{ &arg_on },
		"HTNG_LOCK_TV_VOL_BUTTONS"
	}, {
		CEC_MSG_HTNG_LOCK_TV_CHAN_BUTTONS,
		OptHtngLockTvChanButtons,
		1, { "on" },
		{ &arg_on },
		"HTNG_LOCK_TV_CHAN_BUTTONS"
	}, {
		CEC_MSG_HTNG_LOCK_TV_INPUT_BUTTONS,
		OptHtngLockTvInputButtons,
		1, { "on" },
		{ &arg_on },
		"HTNG_LOCK_TV_INPUT_BUTTONS"
	}, {
		CEC_MSG_HTNG_LOCK_TV_OTHER_BUTTONS,
		OptHtngLockTvOtherButtons,
		1, { "on" },
		{ &arg_on },
		"HTNG_LOCK_TV_OTHER_BUTTONS"
	}, {
		CEC_MSG_HTNG_LOCK_EVERYTHING,
		OptHtngLockEverything,
		1, { "on" },
		{ &arg_on },
		"HTNG_LOCK_EVERYTHING"
	}, {
		CEC_MSG_HTNG_LOCK_EVERYTHING_BUT_PWR,
		OptHtngLockEverythingButPwr,
		1, { "on" },
		{ &arg_on },
		"HTNG_LOCK_EVERYTHING_BUT_PWR"
	}, {
		CEC_MSG_HTNG_HOTEL_MODE,
		OptHtngHotelMode,
		2, { "on", "options" },
		{ &arg_on, &arg_options },
		"HTNG_HOTEL_MODE"
	}, {
		CEC_MSG_HTNG_SET_PWR_SAVING_PROFILE,
		OptHtngSetPwrSavingProfile,
		2, { "on", "val" },
		{ &arg_on, &arg_val },
		"HTNG_SET_PWR_SAVING_PROFILE"
	}, {
		CEC_MSG_HTNG_SET_SLEEP_TIMER,
		OptHtngSetSleepTimer,
		1, { "minutes" },
		{ &arg_minutes },
		"HTNG_SET_SLEEP_TIMER"
	}, {
		CEC_MSG_HTNG_SET_WAKEUP_TIME,
		OptHtngSetWakeupTime,
		1, { "time" },
		{ &arg_time },
		"HTNG_SET_WAKEUP_TIME"
	}, {
		CEC_MSG_HTNG_SET_AUTO_OFF_TIME,
		OptHtngSetAutoOffTime,
		1, { "time" },
		{ &arg_time },
		"HTNG_SET_AUTO_OFF_TIME"
	}, {
		CEC_MSG_HTNG_SET_WAKEUP_SRC,
		OptHtngSetWakeupSrc,
		4, { "htng-input-src", "htng-tuner-type", "major", "input" },
		{ &arg_htng_input_src, &arg_htng_tuner_type, &arg_major, &arg_input },
		"HTNG_SET_WAKEUP_SRC"
	}, {
		CEC_MSG_HTNG_SET_INIT_WAKEUP_VOL,
		OptHtngSetInitWakeupVol,
		2, { "vol", "minutes" },
		{ &arg_vol, &arg_minutes },
		"HTNG_SET_INIT_WAKEUP_VOL"
	}, {
		CEC_MSG_HTNG_CLR_ALL_SLEEP_WAKE,
		OptHtngClrAllSleepWake,
		0, { }, { },
		"HTNG_CLR_ALL_SLEEP_WAKE"
	}, {
		CEC_MSG_HTNG_GLOBAL_DIRECT_TUNE_FREQ,
		OptHtngGlobalDirectTuneFreq,
		8, { "htng-chan-type", "htng-prog-type", "htng-system-type", "freq", "service-id", "htng-mod-type", "htng-symbol-rate", "symbol-rate" },
		{ &arg_htng_chan_type, &arg_htng_prog_type, &arg_htng_system_type, &arg_freq, &arg_service_id, &arg_htng_mod_type, &arg_htng_symbol_rate, &arg_symbol_rate },
		"HTNG_GLOBAL_DIRECT_TUNE_FREQ"
	}, {
		CEC_MSG_HTNG_GLOBAL_DIRECT_TUNE_CHAN,
		OptHtngGlobalDirectTuneChan,
		3, { "htng-chan-type", "htng-prog-type", "chan" },
		{ &arg_htng_chan_type, &arg_htng_prog_type, &arg_chan },
		"HTNG_GLOBAL_DIRECT_TUNE_CHAN"
	}, {
		CEC_MSG_HTNG_GLOBAL_DIRECT_TUNE_EXT_FREQ,
		OptHtngGlobalDirectTuneExtFreq,
		14, { "htng-ext-chan-type", "htng-prog-type", "htng-system-type", "freq", "service-id", "htng-mod-type", "htng-onid", "onid", "htng-nid", "nid", "htng-tsid-plp", "tsid-plp", "htng-symbol-rate", "symbol-rate" },
		{ &arg_htng_ext_chan_type, &arg_htng_prog_type, &arg_htng_system_type, &arg_freq, &arg_service_id, &arg_htng_mod_type, &arg_htng_onid, &arg_onid, &arg_htng_nid, &arg_nid, &arg_htng_tsid_plp, &arg_tsid_plp, &arg_htng_symbol_rate, &arg_symbol_rate },
		"HTNG_GLOBAL_DIRECT_TUNE_EXT_FREQ"
	}, {
	}
};

static void usage_options(int ch)
{
	if (options[OptHelpAll] || options[OptHelpAbort]) {
		printf("Abort Feature:\n");
		printf("%s\n", abort_usage);
	}
	if (options[OptHelpAll] || options[OptHelpAudioRateControl]) {
		printf("Audio Rate Control Feature:\n");
		printf("%s\n", audio_rate_control_usage);
	}
	if (options[OptHelpAll] || options[OptHelpAudioReturnChannelControl]) {
		printf("Audio Return Channel Control Feature:\n");
		printf("%s\n", audio_return_channel_control_usage);
	}
	if (options[OptHelpAll] || options[OptHelpCapabilityDiscoveryandControl]) {
		printf("Capability Discovery and Control Feature:\n");
		printf("%s\n", capability_discovery_and_control_usage);
	}
	if (options[OptHelpAll] || options[OptHelpDeckControl]) {
		printf("Deck Control Feature:\n");
		printf("%s\n", deck_control_usage);
	}
	if (options[OptHelpAll] || options[OptHelpDeviceMenuControl]) {
		printf("Device Menu Control Feature:\n");
		printf("%s\n", device_menu_control_usage);
	}
	if (options[OptHelpAll] || options[OptHelpDeviceOSDTransfer]) {
		printf("Device OSD Transfer Feature:\n");
		printf("%s\n", device_osd_transfer_usage);
	}
	if (options[OptHelpAll] || options[OptHelpDynamicAudioLipsync]) {
		printf("Dynamic Audio Lipsync Feature:\n");
		printf("%s\n", dynamic_audio_lipsync_usage);
	}
	if (options[OptHelpAll] || options[OptHelpHTNG]) {
		printf("HTNG Feature:\n");
		printf("%s\n", htng_usage);
	}
	if (options[OptHelpAll] || options[OptHelpOSDDisplay]) {
		printf("OSD Display Feature:\n");
		printf("%s\n", osd_display_usage);
	}
	if (options[OptHelpAll] || options[OptHelpOneTouchPlay]) {
		printf("One Touch Play Feature:\n");
		printf("%s\n", one_touch_play_usage);
	}
	if (options[OptHelpAll] || options[OptHelpOneTouchRecord]) {
		printf("One Touch Record Feature:\n");
		printf("%s\n", one_touch_record_usage);
	}
	if (options[OptHelpAll] || options[OptHelpPowerStatus]) {
		printf("Power Status Feature:\n");
		printf("%s\n", power_status_usage);
	}
	if (options[OptHelpAll] || options[OptHelpRoutingControl]) {
		printf("Routing Control Feature:\n");
		printf("%s\n", routing_control_usage);
	}
	if (options[OptHelpAll] || options[OptHelpStandby]) {
		printf("Standby Feature:\n");
		printf("%s\n", standby_usage);
	}
	if (options[OptHelpAll] || options[OptHelpSystemAudioControl]) {
		printf("System Audio Control Feature:\n");
		printf("%s\n", system_audio_control_usage);
	}
	if (options[OptHelpAll] || options[OptHelpSystemInformation]) {
		printf("System Information Feature:\n");
		printf("%s\n", system_information_usage);
	}
	if (options[OptHelpAll] || options[OptHelpTimerProgramming]) {
		printf("Timer Programming Feature:\n");
		printf("%s\n", timer_programming_usage);
	}
	if (options[OptHelpAll] || options[OptHelpTunerControl]) {
		printf("Tuner Control Feature:\n");
		printf("%s\n", tuner_control_usage);
	}
	if (options[OptHelpAll] || options[OptHelpVendorSpecificCommands]) {
		printf("Vendor Specific Commands Feature:\n");
		printf("%s\n", vendor_specific_commands_usage);
	}
}

static void parse_msg_args(struct cec_msg &msg, int reply, const message *opt, int ch)
{
	char *value, *subs = optarg;

	switch (ch) {
	case OptActiveSource: {
		__u16 phys_addr = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				phys_addr = parse_phys_addr(value);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_active_source(&msg, phys_addr);
		break;
	}

	case OptImageViewOn: {
		cec_msg_image_view_on(&msg);
		break;
	}

	case OptTextViewOn: {
		cec_msg_text_view_on(&msg);
		break;
	}

	case OptInactiveSource: {
		__u16 phys_addr = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				phys_addr = parse_phys_addr(value);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_inactive_source(&msg, phys_addr);
		break;
	}

	case OptRequestActiveSource: {
		cec_msg_request_active_source(&msg, reply);
		break;
	}

	case OptRoutingInformation: {
		__u16 phys_addr = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				phys_addr = parse_phys_addr(value);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_routing_information(&msg, phys_addr);
		break;
	}

	case OptRoutingChange: {
		__u16 orig_phys_addr = 0;
		__u16 new_phys_addr = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				orig_phys_addr = parse_phys_addr(value);
				break;
			case 1:
				new_phys_addr = parse_phys_addr(value);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_routing_change(&msg, reply, orig_phys_addr, new_phys_addr);
		break;
	}

	case OptSetStreamPath: {
		__u16 phys_addr = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				phys_addr = parse_phys_addr(value);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_set_stream_path(&msg, phys_addr);
		break;
	}

	case OptStandby: {
		cec_msg_standby(&msg);
		break;
	}

	case OptRecordOff: {
		cec_msg_record_off(&msg, reply);
		break;
	}

	case OptRecordOnOwn: {
		cec_msg_record_on_own(&msg);
		break;
	}

	case OptRecordOnDigital: {
		__u8 service_id_method = 0;
		__u8 dig_bcast_system = 0;
		__u16 transport_id = 0;
		__u16 service_id = 0;
		__u16 orig_network_id = 0;
		__u16 program_number = 0;
		__u8 channel_number_fmt = 0;
		__u16 major = 0;
		__u16 minor = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				service_id_method = parse_enum(value, opt->args[0]);
				break;
			case 1:
				dig_bcast_system = parse_enum(value, opt->args[1]);
				break;
			case 2:
				transport_id = strtol(value, 0L, 0);
				break;
			case 3:
				service_id = strtol(value, 0L, 0);
				break;
			case 4:
				orig_network_id = strtol(value, 0L, 0);
				break;
			case 5:
				program_number = strtol(value, 0L, 0);
				break;
			case 6:
				channel_number_fmt = parse_enum(value, opt->args[6]);
				break;
			case 7:
				major = strtol(value, 0L, 0);
				break;
			case 8:
				minor = strtol(value, 0L, 0);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_record_on_digital(&msg, args2digital_service_id(service_id_method, dig_bcast_system, transport_id, service_id, orig_network_id, program_number, channel_number_fmt, major, minor));
		break;
	}

	case OptRecordOnAnalog: {
		__u8 ana_bcast_type = 0;
		__u16 ana_freq = 0;
		__u8 bcast_system = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				ana_bcast_type = parse_enum(value, opt->args[0]);
				break;
			case 1:
				ana_freq = strtol(value, 0L, 0);
				break;
			case 2:
				bcast_system = parse_enum(value, opt->args[2]);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_record_on_analog(&msg, ana_bcast_type, ana_freq, bcast_system);
		break;
	}

	case OptRecordOnPlug: {
		__u8 plug = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				plug = strtol(value, 0L, 0);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_record_on_plug(&msg, plug);
		break;
	}

	case OptRecordOnPhysAddr: {
		__u16 phys_addr = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				phys_addr = parse_phys_addr(value);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_record_on_phys_addr(&msg, phys_addr);
		break;
	}

	case OptRecordStatus: {
		__u8 rec_status = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				rec_status = parse_enum(value, opt->args[0]);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_record_status(&msg, rec_status);
		break;
	}

	case OptRecordTvScreen: {
		cec_msg_record_tv_screen(&msg, reply);
		break;
	}

	case OptTimerStatus: {
		__u8 timer_overlap_warning = 0;
		__u8 media_info = 0;
		__u8 prog_info = 0;
		__u8 prog_error = 0;
		__u8 duration_hr = 0;
		__u8 duration_min = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				timer_overlap_warning = parse_enum(value, opt->args[0]);
				break;
			case 1:
				media_info = parse_enum(value, opt->args[1]);
				break;
			case 2:
				prog_info = parse_enum(value, opt->args[2]);
				break;
			case 3:
				prog_error = parse_enum(value, opt->args[3]);
				break;
			case 4:
				duration_hr = strtol(value, 0L, 0);
				break;
			case 5:
				duration_min = strtol(value, 0L, 0);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_timer_status(&msg, timer_overlap_warning, media_info, prog_info, prog_error, duration_hr, duration_min);
		break;
	}

	case OptTimerClearedStatus: {
		__u8 timer_cleared_status = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				timer_cleared_status = parse_enum(value, opt->args[0]);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_timer_cleared_status(&msg, timer_cleared_status);
		break;
	}

	case OptClearAnalogueTimer: {
		__u8 day = 0;
		__u8 month = 0;
		__u8 start_hr = 0;
		__u8 start_min = 0;
		__u8 duration_hr = 0;
		__u8 duration_min = 0;
		__u8 recording_seq = 0;
		__u8 ana_bcast_type = 0;
		__u16 ana_freq = 0;
		__u8 bcast_system = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				day = strtol(value, 0L, 0);
				break;
			case 1:
				month = strtol(value, 0L, 0);
				break;
			case 2:
				start_hr = strtol(value, 0L, 0);
				break;
			case 3:
				start_min = strtol(value, 0L, 0);
				break;
			case 4:
				duration_hr = strtol(value, 0L, 0);
				break;
			case 5:
				duration_min = strtol(value, 0L, 0);
				break;
			case 6:
				recording_seq = parse_enum(value, opt->args[6]);
				break;
			case 7:
				ana_bcast_type = parse_enum(value, opt->args[7]);
				break;
			case 8:
				ana_freq = strtol(value, 0L, 0);
				break;
			case 9:
				bcast_system = parse_enum(value, opt->args[9]);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_clear_analogue_timer(&msg, reply, day, month, start_hr, start_min, duration_hr, duration_min, recording_seq, ana_bcast_type, ana_freq, bcast_system);
		break;
	}

	case OptClearDigitalTimer: {
		__u8 day = 0;
		__u8 month = 0;
		__u8 start_hr = 0;
		__u8 start_min = 0;
		__u8 duration_hr = 0;
		__u8 duration_min = 0;
		__u8 recording_seq = 0;
		__u8 service_id_method = 0;
		__u8 dig_bcast_system = 0;
		__u16 transport_id = 0;
		__u16 service_id = 0;
		__u16 orig_network_id = 0;
		__u16 program_number = 0;
		__u8 channel_number_fmt = 0;
		__u16 major = 0;
		__u16 minor = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				day = strtol(value, 0L, 0);
				break;
			case 1:
				month = strtol(value, 0L, 0);
				break;
			case 2:
				start_hr = strtol(value, 0L, 0);
				break;
			case 3:
				start_min = strtol(value, 0L, 0);
				break;
			case 4:
				duration_hr = strtol(value, 0L, 0);
				break;
			case 5:
				duration_min = strtol(value, 0L, 0);
				break;
			case 6:
				recording_seq = parse_enum(value, opt->args[6]);
				break;
			case 7:
				service_id_method = parse_enum(value, opt->args[7]);
				break;
			case 8:
				dig_bcast_system = parse_enum(value, opt->args[8]);
				break;
			case 9:
				transport_id = strtol(value, 0L, 0);
				break;
			case 10:
				service_id = strtol(value, 0L, 0);
				break;
			case 11:
				orig_network_id = strtol(value, 0L, 0);
				break;
			case 12:
				program_number = strtol(value, 0L, 0);
				break;
			case 13:
				channel_number_fmt = parse_enum(value, opt->args[13]);
				break;
			case 14:
				major = strtol(value, 0L, 0);
				break;
			case 15:
				minor = strtol(value, 0L, 0);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_clear_digital_timer(&msg, reply, day, month, start_hr, start_min, duration_hr, duration_min, recording_seq, args2digital_service_id(service_id_method, dig_bcast_system, transport_id, service_id, orig_network_id, program_number, channel_number_fmt, major, minor));
		break;
	}

	case OptClearExtTimer: {
		__u8 day = 0;
		__u8 month = 0;
		__u8 start_hr = 0;
		__u8 start_min = 0;
		__u8 duration_hr = 0;
		__u8 duration_min = 0;
		__u8 recording_seq = 0;
		__u8 ext_src_spec = 0;
		__u8 plug = 0;
		__u16 phys_addr = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				day = strtol(value, 0L, 0);
				break;
			case 1:
				month = strtol(value, 0L, 0);
				break;
			case 2:
				start_hr = strtol(value, 0L, 0);
				break;
			case 3:
				start_min = strtol(value, 0L, 0);
				break;
			case 4:
				duration_hr = strtol(value, 0L, 0);
				break;
			case 5:
				duration_min = strtol(value, 0L, 0);
				break;
			case 6:
				recording_seq = parse_enum(value, opt->args[6]);
				break;
			case 7:
				ext_src_spec = parse_enum(value, opt->args[7]);
				break;
			case 8:
				plug = strtol(value, 0L, 0);
				break;
			case 9:
				phys_addr = parse_phys_addr(value);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_clear_ext_timer(&msg, reply, day, month, start_hr, start_min, duration_hr, duration_min, recording_seq, ext_src_spec, plug, phys_addr);
		break;
	}

	case OptSetAnalogueTimer: {
		__u8 day = 0;
		__u8 month = 0;
		__u8 start_hr = 0;
		__u8 start_min = 0;
		__u8 duration_hr = 0;
		__u8 duration_min = 0;
		__u8 recording_seq = 0;
		__u8 ana_bcast_type = 0;
		__u16 ana_freq = 0;
		__u8 bcast_system = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				day = strtol(value, 0L, 0);
				break;
			case 1:
				month = strtol(value, 0L, 0);
				break;
			case 2:
				start_hr = strtol(value, 0L, 0);
				break;
			case 3:
				start_min = strtol(value, 0L, 0);
				break;
			case 4:
				duration_hr = strtol(value, 0L, 0);
				break;
			case 5:
				duration_min = strtol(value, 0L, 0);
				break;
			case 6:
				recording_seq = parse_enum(value, opt->args[6]);
				break;
			case 7:
				ana_bcast_type = parse_enum(value, opt->args[7]);
				break;
			case 8:
				ana_freq = strtol(value, 0L, 0);
				break;
			case 9:
				bcast_system = parse_enum(value, opt->args[9]);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_set_analogue_timer(&msg, reply, day, month, start_hr, start_min, duration_hr, duration_min, recording_seq, ana_bcast_type, ana_freq, bcast_system);
		break;
	}

	case OptSetDigitalTimer: {
		__u8 day = 0;
		__u8 month = 0;
		__u8 start_hr = 0;
		__u8 start_min = 0;
		__u8 duration_hr = 0;
		__u8 duration_min = 0;
		__u8 recording_seq = 0;
		__u8 service_id_method = 0;
		__u8 dig_bcast_system = 0;
		__u16 transport_id = 0;
		__u16 service_id = 0;
		__u16 orig_network_id = 0;
		__u16 program_number = 0;
		__u8 channel_number_fmt = 0;
		__u16 major = 0;
		__u16 minor = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				day = strtol(value, 0L, 0);
				break;
			case 1:
				month = strtol(value, 0L, 0);
				break;
			case 2:
				start_hr = strtol(value, 0L, 0);
				break;
			case 3:
				start_min = strtol(value, 0L, 0);
				break;
			case 4:
				duration_hr = strtol(value, 0L, 0);
				break;
			case 5:
				duration_min = strtol(value, 0L, 0);
				break;
			case 6:
				recording_seq = parse_enum(value, opt->args[6]);
				break;
			case 7:
				service_id_method = parse_enum(value, opt->args[7]);
				break;
			case 8:
				dig_bcast_system = parse_enum(value, opt->args[8]);
				break;
			case 9:
				transport_id = strtol(value, 0L, 0);
				break;
			case 10:
				service_id = strtol(value, 0L, 0);
				break;
			case 11:
				orig_network_id = strtol(value, 0L, 0);
				break;
			case 12:
				program_number = strtol(value, 0L, 0);
				break;
			case 13:
				channel_number_fmt = parse_enum(value, opt->args[13]);
				break;
			case 14:
				major = strtol(value, 0L, 0);
				break;
			case 15:
				minor = strtol(value, 0L, 0);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_set_digital_timer(&msg, reply, day, month, start_hr, start_min, duration_hr, duration_min, recording_seq, args2digital_service_id(service_id_method, dig_bcast_system, transport_id, service_id, orig_network_id, program_number, channel_number_fmt, major, minor));
		break;
	}

	case OptSetExtTimer: {
		__u8 day = 0;
		__u8 month = 0;
		__u8 start_hr = 0;
		__u8 start_min = 0;
		__u8 duration_hr = 0;
		__u8 duration_min = 0;
		__u8 recording_seq = 0;
		__u8 ext_src_spec = 0;
		__u8 plug = 0;
		__u16 phys_addr = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				day = strtol(value, 0L, 0);
				break;
			case 1:
				month = strtol(value, 0L, 0);
				break;
			case 2:
				start_hr = strtol(value, 0L, 0);
				break;
			case 3:
				start_min = strtol(value, 0L, 0);
				break;
			case 4:
				duration_hr = strtol(value, 0L, 0);
				break;
			case 5:
				duration_min = strtol(value, 0L, 0);
				break;
			case 6:
				recording_seq = parse_enum(value, opt->args[6]);
				break;
			case 7:
				ext_src_spec = parse_enum(value, opt->args[7]);
				break;
			case 8:
				plug = strtol(value, 0L, 0);
				break;
			case 9:
				phys_addr = parse_phys_addr(value);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_set_ext_timer(&msg, reply, day, month, start_hr, start_min, duration_hr, duration_min, recording_seq, ext_src_spec, plug, phys_addr);
		break;
	}

	case OptSetTimerProgramTitle: {
		const char *prog_title = "";

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				prog_title = value;
				break;
			default:
				exit(1);
			}
		}
		cec_msg_set_timer_program_title(&msg, prog_title);
		break;
	}

	case OptCecVersion: {
		__u8 cec_version = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				cec_version = parse_enum(value, opt->args[0]);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_cec_version(&msg, cec_version);
		break;
	}

	case OptGetCecVersion: {
		cec_msg_get_cec_version(&msg, reply);
		break;
	}

	case OptReportPhysicalAddr: {
		__u16 phys_addr = 0;
		__u8 prim_devtype = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				phys_addr = parse_phys_addr(value);
				break;
			case 1:
				prim_devtype = parse_enum(value, opt->args[1]);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_report_physical_addr(&msg, phys_addr, prim_devtype);
		break;
	}

	case OptGivePhysicalAddr: {
		cec_msg_give_physical_addr(&msg, reply);
		break;
	}

	case OptSetMenuLanguage: {
		const char *language = "";

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				language = value;
				break;
			default:
				exit(1);
			}
		}
		cec_msg_set_menu_language(&msg, language);
		break;
	}

	case OptGetMenuLanguage: {
		cec_msg_get_menu_language(&msg, reply);
		break;
	}

	case OptReportFeatures: {
		__u8 cec_version = 0;
		__u8 all_device_types = 0;
		__u8 rc_profile = 0;
		__u8 dev_features = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				cec_version = parse_enum(value, opt->args[0]);
				break;
			case 1:
				all_device_types = parse_enum(value, opt->args[1]);
				break;
			case 2:
				rc_profile = parse_enum(value, opt->args[2]);
				break;
			case 3:
				dev_features = parse_enum(value, opt->args[3]);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_report_features(&msg, cec_version, all_device_types, rc_profile, dev_features);
		break;
	}

	case OptGiveFeatures: {
		cec_msg_give_features(&msg, reply);
		break;
	}

	case OptDeckControl: {
		__u8 deck_control_mode = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				deck_control_mode = parse_enum(value, opt->args[0]);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_deck_control(&msg, deck_control_mode);
		break;
	}

	case OptDeckStatus: {
		__u8 deck_info = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				deck_info = parse_enum(value, opt->args[0]);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_deck_status(&msg, deck_info);
		break;
	}

	case OptGiveDeckStatus: {
		__u8 status_req = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				status_req = parse_enum(value, opt->args[0]);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_give_deck_status(&msg, reply, status_req);
		break;
	}

	case OptPlay: {
		__u8 play_mode = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				play_mode = parse_enum(value, opt->args[0]);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_play(&msg, play_mode);
		break;
	}

	case OptTunerDeviceStatusAnalog: {
		__u8 rec_flag = 0;
		__u8 tuner_display_info = 0;
		__u8 ana_bcast_type = 0;
		__u16 ana_freq = 0;
		__u8 bcast_system = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				rec_flag = parse_enum(value, opt->args[0]);
				break;
			case 1:
				tuner_display_info = parse_enum(value, opt->args[1]);
				break;
			case 2:
				ana_bcast_type = parse_enum(value, opt->args[2]);
				break;
			case 3:
				ana_freq = strtol(value, 0L, 0);
				break;
			case 4:
				bcast_system = parse_enum(value, opt->args[4]);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_tuner_device_status_analog(&msg, rec_flag, tuner_display_info, ana_bcast_type, ana_freq, bcast_system);
		break;
	}

	case OptTunerDeviceStatusDigital: {
		__u8 rec_flag = 0;
		__u8 tuner_display_info = 0;
		__u8 service_id_method = 0;
		__u8 dig_bcast_system = 0;
		__u16 transport_id = 0;
		__u16 service_id = 0;
		__u16 orig_network_id = 0;
		__u16 program_number = 0;
		__u8 channel_number_fmt = 0;
		__u16 major = 0;
		__u16 minor = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				rec_flag = parse_enum(value, opt->args[0]);
				break;
			case 1:
				tuner_display_info = parse_enum(value, opt->args[1]);
				break;
			case 2:
				service_id_method = parse_enum(value, opt->args[2]);
				break;
			case 3:
				dig_bcast_system = parse_enum(value, opt->args[3]);
				break;
			case 4:
				transport_id = strtol(value, 0L, 0);
				break;
			case 5:
				service_id = strtol(value, 0L, 0);
				break;
			case 6:
				orig_network_id = strtol(value, 0L, 0);
				break;
			case 7:
				program_number = strtol(value, 0L, 0);
				break;
			case 8:
				channel_number_fmt = parse_enum(value, opt->args[8]);
				break;
			case 9:
				major = strtol(value, 0L, 0);
				break;
			case 10:
				minor = strtol(value, 0L, 0);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_tuner_device_status_digital(&msg, rec_flag, tuner_display_info, args2digital_service_id(service_id_method, dig_bcast_system, transport_id, service_id, orig_network_id, program_number, channel_number_fmt, major, minor));
		break;
	}

	case OptGiveTunerDeviceStatus: {
		__u8 status_req = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				status_req = parse_enum(value, opt->args[0]);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_give_tuner_device_status(&msg, reply, status_req);
		break;
	}

	case OptSelectAnalogueService: {
		__u8 ana_bcast_type = 0;
		__u16 ana_freq = 0;
		__u8 bcast_system = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				ana_bcast_type = parse_enum(value, opt->args[0]);
				break;
			case 1:
				ana_freq = strtol(value, 0L, 0);
				break;
			case 2:
				bcast_system = parse_enum(value, opt->args[2]);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_select_analogue_service(&msg, ana_bcast_type, ana_freq, bcast_system);
		break;
	}

	case OptSelectDigitalService: {
		__u8 service_id_method = 0;
		__u8 dig_bcast_system = 0;
		__u16 transport_id = 0;
		__u16 service_id = 0;
		__u16 orig_network_id = 0;
		__u16 program_number = 0;
		__u8 channel_number_fmt = 0;
		__u16 major = 0;
		__u16 minor = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				service_id_method = parse_enum(value, opt->args[0]);
				break;
			case 1:
				dig_bcast_system = parse_enum(value, opt->args[1]);
				break;
			case 2:
				transport_id = strtol(value, 0L, 0);
				break;
			case 3:
				service_id = strtol(value, 0L, 0);
				break;
			case 4:
				orig_network_id = strtol(value, 0L, 0);
				break;
			case 5:
				program_number = strtol(value, 0L, 0);
				break;
			case 6:
				channel_number_fmt = parse_enum(value, opt->args[6]);
				break;
			case 7:
				major = strtol(value, 0L, 0);
				break;
			case 8:
				minor = strtol(value, 0L, 0);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_select_digital_service(&msg, args2digital_service_id(service_id_method, dig_bcast_system, transport_id, service_id, orig_network_id, program_number, channel_number_fmt, major, minor));
		break;
	}

	case OptTunerStepDecrement: {
		cec_msg_tuner_step_decrement(&msg);
		break;
	}

	case OptTunerStepIncrement: {
		cec_msg_tuner_step_increment(&msg);
		break;
	}

	case OptDeviceVendorId: {
		__u32 vendor_id = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				vendor_id = strtol(value, 0L, 0);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_device_vendor_id(&msg, vendor_id);
		break;
	}

	case OptGiveDeviceVendorId: {
		cec_msg_give_device_vendor_id(&msg, reply);
		break;
	}

	case OptVendorRemoteButtonUp: {
		cec_msg_vendor_remote_button_up(&msg);
		break;
	}

	case OptSetOsdString: {
		__u8 disp_ctl = 0;
		const char *osd = "";

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				disp_ctl = parse_enum(value, opt->args[0]);
				break;
			case 1:
				osd = value;
				break;
			default:
				exit(1);
			}
		}
		cec_msg_set_osd_string(&msg, disp_ctl, osd);
		break;
	}

	case OptSetOsdName: {
		const char *name = "";

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				name = value;
				break;
			default:
				exit(1);
			}
		}
		cec_msg_set_osd_name(&msg, name);
		break;
	}

	case OptGiveOsdName: {
		cec_msg_give_osd_name(&msg, reply);
		break;
	}

	case OptMenuStatus: {
		__u8 menu_state = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				menu_state = parse_enum(value, opt->args[0]);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_menu_status(&msg, menu_state);
		break;
	}

	case OptMenuRequest: {
		__u8 menu_req = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				menu_req = parse_enum(value, opt->args[0]);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_menu_request(&msg, reply, menu_req);
		break;
	}

	case OptUserControlPressed: {
		__u8 ui_cmd = 0;
		__u8 has_opt_arg = 0;
		__u8 play_mode = 0;
		__u8 ui_function_media = 0;
		__u8 ui_function_select_av_input = 0;
		__u8 ui_function_select_audio_input = 0;
		__u8 ui_bcast_type = 0;
		__u8 ui_snd_pres_ctl = 0;
		__u8 channel_number_fmt = 0;
		__u16 major = 0;
		__u16 minor = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				ui_cmd = parse_enum(value, opt->args[0]);
				break;
			case 1:
				has_opt_arg = strtol(value, 0L, 0);
				break;
			case 2:
				play_mode = parse_enum(value, opt->args[2]);
				break;
			case 3:
				ui_function_media = strtol(value, 0L, 0);
				break;
			case 4:
				ui_function_select_av_input = strtol(value, 0L, 0);
				break;
			case 5:
				ui_function_select_audio_input = strtol(value, 0L, 0);
				break;
			case 6:
				ui_bcast_type = parse_enum(value, opt->args[6]);
				break;
			case 7:
				ui_snd_pres_ctl = parse_enum(value, opt->args[7]);
				break;
			case 8:
				channel_number_fmt = parse_enum(value, opt->args[8]);
				break;
			case 9:
				major = strtol(value, 0L, 0);
				break;
			case 10:
				minor = strtol(value, 0L, 0);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_user_control_pressed(&msg, args2ui_command(ui_cmd, has_opt_arg, play_mode, ui_function_media, ui_function_select_av_input, ui_function_select_audio_input, ui_bcast_type, ui_snd_pres_ctl, channel_number_fmt, major, minor));
		break;
	}

	case OptUserControlReleased: {
		cec_msg_user_control_released(&msg);
		break;
	}

	case OptReportPowerStatus: {
		__u8 pwr_state = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				pwr_state = parse_enum(value, opt->args[0]);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_report_power_status(&msg, pwr_state);
		break;
	}

	case OptGiveDevicePowerStatus: {
		cec_msg_give_device_power_status(&msg, reply);
		break;
	}

	case OptFeatureAbort: {
		__u8 abort_msg = 0;
		__u8 reason = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				abort_msg = strtol(value, 0L, 0);
				break;
			case 1:
				reason = parse_enum(value, opt->args[1]);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_feature_abort(&msg, abort_msg, reason);
		break;
	}

	case OptAbort: {
		cec_msg_abort(&msg);
		break;
	}

	case OptReportAudioStatus: {
		__u8 aud_mute_status = 0;
		__u8 aud_vol_status = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				aud_mute_status = parse_enum(value, opt->args[0]);
				break;
			case 1:
				aud_vol_status = strtol(value, 0L, 0);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_report_audio_status(&msg, aud_mute_status, aud_vol_status);
		break;
	}

	case OptGiveAudioStatus: {
		cec_msg_give_audio_status(&msg, reply);
		break;
	}

	case OptSetSystemAudioMode: {
		__u8 sys_aud_status = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				sys_aud_status = parse_enum(value, opt->args[0]);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_set_system_audio_mode(&msg, sys_aud_status);
		break;
	}

	case OptSystemAudioModeRequest: {
		__u16 phys_addr = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				phys_addr = parse_phys_addr(value);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_system_audio_mode_request(&msg, reply, phys_addr);
		break;
	}

	case OptSystemAudioModeStatus: {
		__u8 sys_aud_status = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				sys_aud_status = parse_enum(value, opt->args[0]);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_system_audio_mode_status(&msg, sys_aud_status);
		break;
	}

	case OptGiveSystemAudioModeStatus: {
		cec_msg_give_system_audio_mode_status(&msg, reply);
		break;
	}

	case OptReportShortAudioDescriptor: {
		__u8 num_descriptors = 0;
		__u8 descriptor1 = 0;
		__u8 descriptor2 = 0;
		__u8 descriptor3 = 0;
		__u8 descriptor4 = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				num_descriptors = strtol(value, 0L, 0);
				break;
			case 1:
				descriptor1 = strtol(value, 0L, 0);
				break;
			case 2:
				descriptor2 = strtol(value, 0L, 0);
				break;
			case 3:
				descriptor3 = strtol(value, 0L, 0);
				break;
			case 4:
				descriptor4 = strtol(value, 0L, 0);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_report_short_audio_descriptor(&msg, num_descriptors, args2short_descrs(descriptor1, descriptor2, descriptor3, descriptor4));
		break;
	}

	case OptRequestShortAudioDescriptor: {
		__u8 num_descriptors = 0;
		__u8 audio_format_id1 = 0;
		__u8 audio_format_code1 = 0;
		__u8 audio_format_id2 = 0;
		__u8 audio_format_code2 = 0;
		__u8 audio_format_id3 = 0;
		__u8 audio_format_code3 = 0;
		__u8 audio_format_id4 = 0;
		__u8 audio_format_code4 = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				num_descriptors = strtol(value, 0L, 0);
				break;
			case 1:
				audio_format_id1 = strtol(value, 0L, 0);
				break;
			case 2:
				audio_format_code1 = strtol(value, 0L, 0);
				break;
			case 3:
				audio_format_id2 = strtol(value, 0L, 0);
				break;
			case 4:
				audio_format_code2 = strtol(value, 0L, 0);
				break;
			case 5:
				audio_format_id3 = strtol(value, 0L, 0);
				break;
			case 6:
				audio_format_code3 = strtol(value, 0L, 0);
				break;
			case 7:
				audio_format_id4 = strtol(value, 0L, 0);
				break;
			case 8:
				audio_format_code4 = strtol(value, 0L, 0);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_request_short_audio_descriptor(&msg, reply, num_descriptors, args2short_aud_fmt_ids(audio_format_id1, audio_format_id2, audio_format_id3, audio_format_id4), args2short_aud_fmt_codes(audio_format_code1, audio_format_code2, audio_format_code3, audio_format_code4));
		break;
	}

	case OptSetAudioRate: {
		__u8 audio_rate = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				audio_rate = parse_enum(value, opt->args[0]);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_set_audio_rate(&msg, audio_rate);
		break;
	}

	case OptReportArcInitiated: {
		cec_msg_report_arc_initiated(&msg);
		break;
	}

	case OptInitiateArc: {
		cec_msg_initiate_arc(&msg, reply);
		break;
	}

	case OptRequestArcInitiation: {
		cec_msg_request_arc_initiation(&msg, reply);
		break;
	}

	case OptReportArcTerminated: {
		cec_msg_report_arc_terminated(&msg);
		break;
	}

	case OptTerminateArc: {
		cec_msg_terminate_arc(&msg, reply);
		break;
	}

	case OptRequestArcTermination: {
		cec_msg_request_arc_termination(&msg, reply);
		break;
	}

	case OptReportCurrentLatency: {
		__u16 phys_addr = 0;
		__u8 video_latency = 0;
		__u8 low_latency_mode = 0;
		__u8 audio_out_compensated = 0;
		__u8 audio_out_delay = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				phys_addr = parse_phys_addr(value);
				break;
			case 1:
				video_latency = strtol(value, 0L, 0);
				break;
			case 2:
				low_latency_mode = parse_enum(value, opt->args[2]);
				break;
			case 3:
				audio_out_compensated = parse_enum(value, opt->args[3]);
				break;
			case 4:
				audio_out_delay = strtol(value, 0L, 0);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_report_current_latency(&msg, phys_addr, video_latency, low_latency_mode, audio_out_compensated, audio_out_delay);
		break;
	}

	case OptRequestCurrentLatency: {
		__u16 phys_addr = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				phys_addr = parse_phys_addr(value);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_request_current_latency(&msg, reply, phys_addr);
		break;
	}

	case OptCdcHecInquireState: {
		__u16 phys_addr1 = 0;
		__u16 phys_addr2 = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				phys_addr1 = parse_phys_addr(value);
				break;
			case 1:
				phys_addr2 = parse_phys_addr(value);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_cdc_hec_inquire_state(&msg, phys_addr1, phys_addr2);
		break;
	}

	case OptCdcHecReportState: {
		__u16 target_phys_addr = 0;
		__u8 hec_func_state = 0;
		__u8 host_func_state = 0;
		__u8 enc_func_state = 0;
		__u8 cdc_errcode = 0;
		__u8 has_field = 0;
		__u16 hec_field = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				target_phys_addr = parse_phys_addr(value);
				break;
			case 1:
				hec_func_state = parse_enum(value, opt->args[1]);
				break;
			case 2:
				host_func_state = parse_enum(value, opt->args[2]);
				break;
			case 3:
				enc_func_state = parse_enum(value, opt->args[3]);
				break;
			case 4:
				cdc_errcode = parse_enum(value, opt->args[4]);
				break;
			case 5:
				has_field = strtol(value, 0L, 0);
				break;
			case 6:
				hec_field = strtol(value, 0L, 0);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_cdc_hec_report_state(&msg, target_phys_addr, hec_func_state, host_func_state, enc_func_state, cdc_errcode, has_field, hec_field);
		break;
	}

	case OptCdcHecSetState: {
		__u16 phys_addr1 = 0;
		__u16 phys_addr2 = 0;
		__u8 hec_set_state = 0;
		__u16 phys_addr3 = 0;
		__u16 phys_addr4 = 0;
		__u16 phys_addr5 = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				phys_addr1 = parse_phys_addr(value);
				break;
			case 1:
				phys_addr2 = parse_phys_addr(value);
				break;
			case 2:
				hec_set_state = parse_enum(value, opt->args[2]);
				break;
			case 3:
				phys_addr3 = parse_phys_addr(value);
				break;
			case 4:
				phys_addr4 = parse_phys_addr(value);
				break;
			case 5:
				phys_addr5 = parse_phys_addr(value);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_cdc_hec_set_state(&msg, phys_addr1, phys_addr2, hec_set_state, phys_addr3, phys_addr4, phys_addr5);
		break;
	}

	case OptCdcHecSetStateAdjacent: {
		__u16 phys_addr1 = 0;
		__u8 hec_set_state = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				phys_addr1 = parse_phys_addr(value);
				break;
			case 1:
				hec_set_state = parse_enum(value, opt->args[1]);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_cdc_hec_set_state_adjacent(&msg, phys_addr1, hec_set_state);
		break;
	}

	case OptCdcHecRequestDeactivation: {
		__u16 phys_addr1 = 0;
		__u16 phys_addr2 = 0;
		__u16 phys_addr3 = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				phys_addr1 = parse_phys_addr(value);
				break;
			case 1:
				phys_addr2 = parse_phys_addr(value);
				break;
			case 2:
				phys_addr3 = parse_phys_addr(value);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_cdc_hec_request_deactivation(&msg, phys_addr1, phys_addr2, phys_addr3);
		break;
	}

	case OptCdcHecNotifyAlive: {
		cec_msg_cdc_hec_notify_alive(&msg);
		break;
	}

	case OptCdcHecDiscover: {
		cec_msg_cdc_hec_discover(&msg);
		break;
	}

	case OptCdcHpdSetState: {
		__u8 input_port = 0;
		__u8 hpd_state = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				input_port = strtol(value, 0L, 0);
				break;
			case 1:
				hpd_state = parse_enum(value, opt->args[1]);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_cdc_hpd_set_state(&msg, input_port, hpd_state);
		break;
	}

	case OptCdcHpdReportState: {
		__u8 hpd_state = 0;
		__u8 hpd_error = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				hpd_state = parse_enum(value, opt->args[0]);
				break;
			case 1:
				hpd_error = parse_enum(value, opt->args[1]);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_cdc_hpd_report_state(&msg, hpd_state, hpd_error);
		break;
	}

	case OptHtngTuner_1partChan: {
		__u8 htng_tuner_type = 0;
		__u16 chan = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				htng_tuner_type = parse_enum(value, opt->args[0]);
				break;
			case 1:
				chan = strtol(value, 0L, 0);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_htng_tuner_1part_chan(&msg, htng_tuner_type, chan);
		break;
	}

	case OptHtngTuner_2partChan: {
		__u8 htng_tuner_type = 0;
		__u8 major_chan = 0;
		__u16 minor_chan = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				htng_tuner_type = parse_enum(value, opt->args[0]);
				break;
			case 1:
				major_chan = strtol(value, 0L, 0);
				break;
			case 2:
				minor_chan = strtol(value, 0L, 0);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_htng_tuner_2part_chan(&msg, htng_tuner_type, major_chan, minor_chan);
		break;
	}

	case OptHtngInputSelAv: {
		__u16 input = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				input = strtol(value, 0L, 0);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_htng_input_sel_av(&msg, input);
		break;
	}

	case OptHtngInputSelPc: {
		__u16 input = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				input = strtol(value, 0L, 0);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_htng_input_sel_pc(&msg, input);
		break;
	}

	case OptHtngInputSelHdmi: {
		__u16 input = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				input = strtol(value, 0L, 0);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_htng_input_sel_hdmi(&msg, input);
		break;
	}

	case OptHtngInputSelComponent: {
		__u16 input = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				input = strtol(value, 0L, 0);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_htng_input_sel_component(&msg, input);
		break;
	}

	case OptHtngInputSelDvi: {
		__u16 input = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				input = strtol(value, 0L, 0);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_htng_input_sel_dvi(&msg, input);
		break;
	}

	case OptHtngInputSelDp: {
		__u16 input = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				input = strtol(value, 0L, 0);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_htng_input_sel_dp(&msg, input);
		break;
	}

	case OptHtngInputSelUsb: {
		__u16 input = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				input = strtol(value, 0L, 0);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_htng_input_sel_usb(&msg, input);
		break;
	}

	case OptHtngSetDefPwrOnInputSrc: {
		__u8 htng_input_src = 0;
		__u8 htng_tuner_type = 0;
		__u8 major = 0;
		__u16 input = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				htng_input_src = parse_enum(value, opt->args[0]);
				break;
			case 1:
				htng_tuner_type = parse_enum(value, opt->args[1]);
				break;
			case 2:
				major = strtol(value, 0L, 0);
				break;
			case 3:
				input = strtol(value, 0L, 0);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_htng_set_def_pwr_on_input_src(&msg, htng_input_src, htng_tuner_type, major, input);
		break;
	}

	case OptHtngSetTvSpeakers: {
		__u8 on = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				on = strtol(value, 0L, 0);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_htng_set_tv_speakers(&msg, on);
		break;
	}

	case OptHtngSetDigAudio: {
		__u8 on = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				on = strtol(value, 0L, 0);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_htng_set_dig_audio(&msg, on);
		break;
	}

	case OptHtngSetAnaAudio: {
		__u8 on = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				on = strtol(value, 0L, 0);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_htng_set_ana_audio(&msg, on);
		break;
	}

	case OptHtngSetDefPwrOnVol: {
		__u8 vol = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				vol = strtol(value, 0L, 0);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_htng_set_def_pwr_on_vol(&msg, vol);
		break;
	}

	case OptHtngSetMaxVol: {
		__u8 vol = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				vol = strtol(value, 0L, 0);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_htng_set_max_vol(&msg, vol);
		break;
	}

	case OptHtngSetMinVol: {
		__u8 vol = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				vol = strtol(value, 0L, 0);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_htng_set_min_vol(&msg, vol);
		break;
	}

	case OptHtngSetBlueScreen: {
		__u8 blue = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				blue = strtol(value, 0L, 0);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_htng_set_blue_screen(&msg, blue);
		break;
	}

	case OptHtngSetBrightness: {
		__u8 brightness = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				brightness = strtol(value, 0L, 0);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_htng_set_brightness(&msg, brightness);
		break;
	}

	case OptHtngSetColor: {
		__u8 color = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				color = strtol(value, 0L, 0);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_htng_set_color(&msg, color);
		break;
	}

	case OptHtngSetContrast: {
		__u8 contrast = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				contrast = strtol(value, 0L, 0);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_htng_set_contrast(&msg, contrast);
		break;
	}

	case OptHtngSetSharpness: {
		__u8 sharpness = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				sharpness = strtol(value, 0L, 0);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_htng_set_sharpness(&msg, sharpness);
		break;
	}

	case OptHtngSetHue: {
		__u8 hue = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				hue = strtol(value, 0L, 0);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_htng_set_hue(&msg, hue);
		break;
	}

	case OptHtngSetLedBacklight: {
		__u8 led_backlight = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				led_backlight = strtol(value, 0L, 0);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_htng_set_led_backlight(&msg, led_backlight);
		break;
	}

	case OptHtngSetTvOsdControl: {
		__u8 on = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				on = strtol(value, 0L, 0);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_htng_set_tv_osd_control(&msg, on);
		break;
	}

	case OptHtngSetAudioOnlyDisplay: {
		__u8 on = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				on = strtol(value, 0L, 0);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_htng_set_audio_only_display(&msg, on);
		break;
	}

	case OptHtngSetDate: {
		const char *date = "";

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				date = value;
				break;
			default:
				exit(1);
			}
		}
		cec_msg_htng_set_date(&msg, date);
		break;
	}

	case OptHtngSetDateFormat: {
		__u8 ddmm = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				ddmm = strtol(value, 0L, 0);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_htng_set_date_format(&msg, ddmm);
		break;
	}

	case OptHtngSetTime: {
		const char *time = "";

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				time = value;
				break;
			default:
				exit(1);
			}
		}
		cec_msg_htng_set_time(&msg, time);
		break;
	}

	case OptHtngSetClkBrightnessStandby: {
		__u8 brightness = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				brightness = strtol(value, 0L, 0);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_htng_set_clk_brightness_standby(&msg, brightness);
		break;
	}

	case OptHtngSetClkBrightnessOn: {
		__u8 brightness = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				brightness = strtol(value, 0L, 0);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_htng_set_clk_brightness_on(&msg, brightness);
		break;
	}

	case OptHtngLedControl: {
		__u8 htng_led_control = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				htng_led_control = parse_enum(value, opt->args[0]);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_htng_led_control(&msg, htng_led_control);
		break;
	}

	case OptHtngLockTvPwrButton: {
		__u8 on = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				on = strtol(value, 0L, 0);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_htng_lock_tv_pwr_button(&msg, on);
		break;
	}

	case OptHtngLockTvVolButtons: {
		__u8 on = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				on = strtol(value, 0L, 0);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_htng_lock_tv_vol_buttons(&msg, on);
		break;
	}

	case OptHtngLockTvChanButtons: {
		__u8 on = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				on = strtol(value, 0L, 0);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_htng_lock_tv_chan_buttons(&msg, on);
		break;
	}

	case OptHtngLockTvInputButtons: {
		__u8 on = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				on = strtol(value, 0L, 0);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_htng_lock_tv_input_buttons(&msg, on);
		break;
	}

	case OptHtngLockTvOtherButtons: {
		__u8 on = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				on = strtol(value, 0L, 0);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_htng_lock_tv_other_buttons(&msg, on);
		break;
	}

	case OptHtngLockEverything: {
		__u8 on = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				on = strtol(value, 0L, 0);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_htng_lock_everything(&msg, on);
		break;
	}

	case OptHtngLockEverythingButPwr: {
		__u8 on = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				on = strtol(value, 0L, 0);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_htng_lock_everything_but_pwr(&msg, on);
		break;
	}

	case OptHtngHotelMode: {
		__u8 on = 0;
		__u8 options = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				on = strtol(value, 0L, 0);
				break;
			case 1:
				options = strtol(value, 0L, 0);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_htng_hotel_mode(&msg, on, options);
		break;
	}

	case OptHtngSetPwrSavingProfile: {
		__u8 on = 0;
		__u8 val = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				on = strtol(value, 0L, 0);
				break;
			case 1:
				val = strtol(value, 0L, 0);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_htng_set_pwr_saving_profile(&msg, on, val);
		break;
	}

	case OptHtngSetSleepTimer: {
		__u8 minutes = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				minutes = strtol(value, 0L, 0);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_htng_set_sleep_timer(&msg, minutes);
		break;
	}

	case OptHtngSetWakeupTime: {
		const char *time = "";

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				time = value;
				break;
			default:
				exit(1);
			}
		}
		cec_msg_htng_set_wakeup_time(&msg, time);
		break;
	}

	case OptHtngSetAutoOffTime: {
		const char *time = "";

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				time = value;
				break;
			default:
				exit(1);
			}
		}
		cec_msg_htng_set_auto_off_time(&msg, time);
		break;
	}

	case OptHtngSetWakeupSrc: {
		__u8 htng_input_src = 0;
		__u8 htng_tuner_type = 0;
		__u8 major = 0;
		__u16 input = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				htng_input_src = parse_enum(value, opt->args[0]);
				break;
			case 1:
				htng_tuner_type = parse_enum(value, opt->args[1]);
				break;
			case 2:
				major = strtol(value, 0L, 0);
				break;
			case 3:
				input = strtol(value, 0L, 0);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_htng_set_wakeup_src(&msg, htng_input_src, htng_tuner_type, major, input);
		break;
	}

	case OptHtngSetInitWakeupVol: {
		__u8 vol = 0;
		__u8 minutes = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				vol = strtol(value, 0L, 0);
				break;
			case 1:
				minutes = strtol(value, 0L, 0);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_htng_set_init_wakeup_vol(&msg, vol, minutes);
		break;
	}

	case OptHtngClrAllSleepWake: {
		cec_msg_htng_clr_all_sleep_wake(&msg);
		break;
	}

	case OptHtngGlobalDirectTuneFreq: {
		__u8 htng_chan_type = 0;
		__u8 htng_prog_type = 0;
		__u8 htng_system_type = 0;
		__u16 freq = 0;
		__u16 service_id = 0;
		__u8 htng_mod_type = 0;
		__u8 htng_symbol_rate = 0;
		__u16 symbol_rate = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				htng_chan_type = parse_enum(value, opt->args[0]);
				break;
			case 1:
				htng_prog_type = parse_enum(value, opt->args[1]);
				break;
			case 2:
				htng_system_type = parse_enum(value, opt->args[2]);
				break;
			case 3:
				freq = strtol(value, 0L, 0);
				break;
			case 4:
				service_id = strtol(value, 0L, 0);
				break;
			case 5:
				htng_mod_type = parse_enum(value, opt->args[5]);
				break;
			case 6:
				htng_symbol_rate = parse_enum(value, opt->args[6]);
				break;
			case 7:
				symbol_rate = strtol(value, 0L, 0);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_htng_global_direct_tune_freq(&msg, htng_chan_type, htng_prog_type, htng_system_type, freq, service_id, htng_mod_type, htng_symbol_rate, symbol_rate);
		break;
	}

	case OptHtngGlobalDirectTuneChan: {
		__u8 htng_chan_type = 0;
		__u8 htng_prog_type = 0;
		__u16 chan = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				htng_chan_type = parse_enum(value, opt->args[0]);
				break;
			case 1:
				htng_prog_type = parse_enum(value, opt->args[1]);
				break;
			case 2:
				chan = strtol(value, 0L, 0);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_htng_global_direct_tune_chan(&msg, htng_chan_type, htng_prog_type, chan);
		break;
	}

	case OptHtngGlobalDirectTuneExtFreq: {
		__u8 htng_ext_chan_type = 0;
		__u8 htng_prog_type = 0;
		__u8 htng_system_type = 0;
		__u16 freq = 0;
		__u16 service_id = 0;
		__u8 htng_mod_type = 0;
		__u8 htng_onid = 0;
		__u16 onid = 0;
		__u8 htng_nid = 0;
		__u16 nid = 0;
		__u8 htng_tsid_plp = 0;
		__u16 tsid_plp = 0;
		__u8 htng_symbol_rate = 0;
		__u16 symbol_rate = 0;

		while (*subs != '\0') {
			switch (parse_subopt(&subs, opt->arg_names, &value)) {
			case 0:
				htng_ext_chan_type = parse_enum(value, opt->args[0]);
				break;
			case 1:
				htng_prog_type = parse_enum(value, opt->args[1]);
				break;
			case 2:
				htng_system_type = parse_enum(value, opt->args[2]);
				break;
			case 3:
				freq = strtol(value, 0L, 0);
				break;
			case 4:
				service_id = strtol(value, 0L, 0);
				break;
			case 5:
				htng_mod_type = parse_enum(value, opt->args[5]);
				break;
			case 6:
				htng_onid = parse_enum(value, opt->args[6]);
				break;
			case 7:
				onid = strtol(value, 0L, 0);
				break;
			case 8:
				htng_nid = parse_enum(value, opt->args[8]);
				break;
			case 9:
				nid = strtol(value, 0L, 0);
				break;
			case 10:
				htng_tsid_plp = parse_enum(value, opt->args[10]);
				break;
			case 11:
				tsid_plp = strtol(value, 0L, 0);
				break;
			case 12:
				htng_symbol_rate = parse_enum(value, opt->args[12]);
				break;
			case 13:
				symbol_rate = strtol(value, 0L, 0);
				break;
			default:
				exit(1);
			}
		}
		cec_msg_htng_global_direct_tune_ext_freq(&msg, htng_ext_chan_type, htng_prog_type, htng_system_type, freq, service_id, htng_mod_type, htng_onid, onid, htng_nid, nid, htng_tsid_plp, tsid_plp, htng_symbol_rate, symbol_rate);
		break;
	}

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
