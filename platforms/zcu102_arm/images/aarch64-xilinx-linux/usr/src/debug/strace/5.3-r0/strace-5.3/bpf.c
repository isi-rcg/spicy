/*
 * Copyright (c) 2015-2017 Dmitry V. Levin <ldv@altlinux.org>
 * Copyright (c) 2017 Quentin Monnet <quentin.monnet@6wind.com>
 * Copyright (c) 2015-2019 The strace developers.
 * All rights reserved.
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "defs.h"
#include "print_fields.h"

#ifdef HAVE_LINUX_BPF_H
# include <linux/bpf.h>
#endif
#include <linux/filter.h>

#include "bpf_attr.h"

#include "xlat/bpf_commands.h"
#include "xlat/bpf_file_mode_flags.h"
#include "xlat/bpf_map_types.h"
#include "xlat/bpf_map_flags.h"
#include "xlat/bpf_prog_types.h"
#include "xlat/bpf_prog_flags.h"
#include "xlat/bpf_map_update_elem_flags.h"
#include "xlat/bpf_attach_type.h"
#include "xlat/bpf_attach_flags.h"
#include "xlat/bpf_query_flags.h"
#include "xlat/bpf_task_fd_type.h"
#include "xlat/ebpf_regs.h"
#include "xlat/numa_node.h"

#define DECL_BPF_CMD_DECODER(bpf_cmd_decoder)				\
int									\
bpf_cmd_decoder(struct tcb *const tcp,					\
		const kernel_ulong_t addr,				\
		const unsigned int size,				\
		void *const data)					\
/* End of DECL_BPF_CMD_DECODER definition. */

#define BEGIN_BPF_CMD_DECODER(bpf_cmd)					\
	static DECL_BPF_CMD_DECODER(decode_ ## bpf_cmd)			\
	{								\
		struct bpf_cmd ## _struct attr = {};			\
		const size_t attr_size = bpf_cmd ## _struct_size;	\
		const unsigned int len = MIN(size, attr_size);		\
		memcpy(&attr, data, len);				\
		do {							\
/* End of BEGIN_BPF_CMD_DECODER definition. */

#define END_BPF_CMD_DECODER(rval)					\
			decode_attr_extra_data(tcp, data, size, attr_size); \
		} while (0);						\
		tprints("}");						\
		return (rval);						\
	}								\
/* End of END_BPF_CMD_DECODER definition. */

#define BPF_CMD_ENTRY(bpf_cmd)						\
	[bpf_cmd] = decode_ ## bpf_cmd

typedef DECL_BPF_CMD_DECODER((*bpf_cmd_decoder_t));

/*
 * A note about bpf syscall decoder: it doesn't perform any size sanity checks,
 * so even if it leads to partial copying of one of the fields, the command
 * handler will still use the (partially-copied-from-userspace, partially
 * zeroed) field value.  That's why we stop decoding and check for known sizes
 * that correspond to released versions of the structure used by the specific
 * command - it looks like the most sensible way to parse this insanity.
 */

static int
decode_attr_extra_data(struct tcb *const tcp,
		       const char *data,
		       unsigned int size,
		       const size_t attr_size)
{
	if (size <= attr_size)
		return 0;

	data += attr_size;
	size -= attr_size;

	unsigned int i;
	for (i = 0; i < size; ++i) {
		if (data[i]) {
			tprints(", ");
			if (abbrev(tcp)) {
				tprints("...");
			} else {
				tprintf("/* bytes %zu..%zu */ ",
					attr_size, attr_size + size - 1);
				print_quoted_string(data, size,
						    QUOTE_FORCE_HEX);
			}
			return RVAL_DECODED;
		}
	}

	return 0;
}

struct ebpf_insn {
	uint8_t code;
	uint8_t dst_reg:4;
	uint8_t src_reg:4;
	int16_t off;
	int32_t imm;
};

struct ebpf_insns_data {
	unsigned int count;
};

static bool
print_ebpf_insn(struct tcb * const tcp, void * const elem_buf,
		const size_t elem_size, void * const data)
{
	struct ebpf_insns_data *eid = data;
	struct ebpf_insn *insn = elem_buf;

	if (eid->count++ >= BPF_MAXINSNS) {
		tprints("...");
		return false;
	}

	tprints("{code=");
	print_bpf_filter_code(insn->code, true);

	/* We can't use PRINT_FIELD_XVAL on bit fields */
	tprints(", dst_reg=");
	printxval(ebpf_regs, insn->dst_reg, "BPF_REG_???");
	tprints(", src_reg=");
	printxval(ebpf_regs, insn->src_reg, "BPF_REG_???");

	PRINT_FIELD_D(", ", *insn, off);
	PRINT_FIELD_X(", ", *insn, imm);
	tprints("}");

	return true;
}

static void
print_ebpf_prog(struct tcb *const tcp, const uint64_t addr, const uint32_t len)
{
	print_big_u64_addr(addr);
	if (abbrev(tcp)) {
		printaddr(addr);
	} else {
		struct ebpf_insns_data eid = {};
		struct ebpf_insn insn;

		print_array(tcp, addr, len, &insn, sizeof(insn),
			    tfetch_mem, print_ebpf_insn, &eid);
	}
}

BEGIN_BPF_CMD_DECODER(BPF_MAP_CREATE)
{
	PRINT_FIELD_XVAL("{", attr, map_type, bpf_map_types,
			 "BPF_MAP_TYPE_???");
	PRINT_FIELD_U(", ", attr, key_size);
	PRINT_FIELD_U(", ", attr, value_size);
	PRINT_FIELD_U(", ", attr, max_entries);

	/* map_flags field was added in Linux commit v4.6-rc1~91^2~108^2~6. */
	if (len <= offsetof(struct BPF_MAP_CREATE_struct, map_flags))
		break;
	PRINT_FIELD_FLAGS(", ", attr, map_flags, bpf_map_flags, "BPF_F_???");

	/*
	 * inner_map_fd field was added in Linux commit
	 * v4.12-rc1~64^3~373^2~2.
	 */
	if (len <= offsetof(struct BPF_MAP_CREATE_struct, inner_map_fd))
		break;
	PRINT_FIELD_FD(", ", attr, inner_map_fd, tcp);

	/* numa_node field was added in Linux commit v4.14-rc1~130^2~196^2~1. */
	if (len <= offsetof(struct BPF_MAP_CREATE_struct, numa_node))
		break;
	if (attr.map_flags & BPF_F_NUMA_NODE) {
		/*
		 * Kernel uses the value of -1 as a designation for "no NUMA
		 * node specified", and even uses NUMA_NO_NODE constant;
		 * however, the constant definition is not a part of UAPI
		 * headers, thus we can't simply print this named constant
		 * instead of the value. Let's force verbose xlat style instead
		 * in order to provide the information for the user while
		 * not hampering the availability to derive the actual value
		 * without the access to the kernel headers.
		 */
		tprints(", numa_node=");
		printxvals_ex(attr.numa_node, NULL,
			      XLAT_STYLE_FMT_U | XLAT_STYLE_VERBOSE,
			      numa_node, NULL);
	}

	/* map_name field was added in Linux commit v4.15-rc1~84^2~605^2~3. */
	if (len <= offsetof(struct BPF_MAP_CREATE_struct, map_name))
		break;
	PRINT_FIELD_CSTRING_SZ(", ", attr, map_name,
			       MIN(sizeof(attr.map_name),
				   len - offsetof(struct BPF_MAP_CREATE_struct,
						  map_name)));

	/*
	 * map_ifindex field was added in Linux commit
	 * v4.16-rc1~123^2~145^2~5^2~8.
	 */
	if (len <= offsetof(struct BPF_MAP_CREATE_struct, map_ifindex))
		break;
	PRINT_FIELD_IFINDEX(", ", attr, map_ifindex);

	/*
	 * The following three fields were introduced by Linux commits
	 * v4.18-rc1~114^2~417^2~1^2~3 and v4.18-rc1~114^2~148^2~7^2~2.
	 */
	if (len <= offsetof(struct BPF_MAP_CREATE_struct, btf_fd))
		break;
	PRINT_FIELD_FD(", ", attr, btf_fd, tcp);
	PRINT_FIELD_U(", ", attr, btf_key_type_id);
	PRINT_FIELD_U(", ", attr, btf_value_type_id);
}
END_BPF_CMD_DECODER(RVAL_DECODED | RVAL_FD)

BEGIN_BPF_CMD_DECODER(BPF_MAP_LOOKUP_ELEM)
{
	PRINT_FIELD_FD("{", attr, map_fd, tcp);
	PRINT_FIELD_ADDR64(", ", attr, key);
	PRINT_FIELD_ADDR64(", ", attr, value);
}
END_BPF_CMD_DECODER(RVAL_DECODED)

#define decode_BPF_MAP_LOOKUP_AND_DELETE_ELEM decode_BPF_MAP_LOOKUP_ELEM

BEGIN_BPF_CMD_DECODER(BPF_MAP_UPDATE_ELEM)
{
	PRINT_FIELD_FD("{", attr, map_fd, tcp);
	PRINT_FIELD_ADDR64(", ", attr, key);
	PRINT_FIELD_ADDR64(", ", attr, value);
	PRINT_FIELD_XVAL(", ", attr, flags, bpf_map_update_elem_flags,
			 "BPF_???");
}
END_BPF_CMD_DECODER(RVAL_DECODED)

BEGIN_BPF_CMD_DECODER(BPF_MAP_DELETE_ELEM)
{
	PRINT_FIELD_FD("{", attr, map_fd, tcp);
	PRINT_FIELD_ADDR64(", ", attr, key);
}
END_BPF_CMD_DECODER(RVAL_DECODED)

BEGIN_BPF_CMD_DECODER(BPF_MAP_GET_NEXT_KEY)
{
	PRINT_FIELD_FD("{", attr, map_fd, tcp);
	PRINT_FIELD_ADDR64(", ", attr, key);
	PRINT_FIELD_ADDR64(", ", attr, next_key);
}
END_BPF_CMD_DECODER(RVAL_DECODED)

BEGIN_BPF_CMD_DECODER(BPF_MAP_FREEZE)
{
	PRINT_FIELD_FD("{", attr, map_fd, tcp);
}
END_BPF_CMD_DECODER(RVAL_DECODED)

BEGIN_BPF_CMD_DECODER(BPF_PROG_LOAD)
{
	PRINT_FIELD_XVAL("{", attr, prog_type, bpf_prog_types,
			 "BPF_PROG_TYPE_???");
	PRINT_FIELD_U(", ", attr, insn_cnt);
	tprints(", insns=");
	print_ebpf_prog(tcp, attr.insns, attr.insn_cnt);

	tprintf(", license=");
	print_big_u64_addr(attr.license);
	printstr(tcp, attr.license);

	/* log_* fields were added in Linux commit v3.18-rc1~52^2~1^2~4.  */
	if (len <= offsetof(struct BPF_PROG_LOAD_struct, log_level))
		break;
	PRINT_FIELD_U(", ", attr, log_level);
	PRINT_FIELD_U(", ", attr, log_size);
	tprintf(", log_buf=");
	print_big_u64_addr(attr.log_buf);
	printstr_ex(tcp, attr.log_buf, attr.log_size, QUOTE_0_TERMINATED);

	/* kern_version field was added in Linux commit v4.1-rc1~84^2~50.  */
	if (len <= offsetof(struct BPF_PROG_LOAD_struct, kern_version))
		break;
	tprints(", kern_version=");
	print_kernel_version(attr.kern_version);
	/* prog_flags field was added in Linux commit v4.12-rc2~34^2~29^2~2.  */
	if (len <= offsetof(struct BPF_PROG_LOAD_struct, prog_flags))
		break;
	PRINT_FIELD_FLAGS(", ", attr, prog_flags, bpf_prog_flags, "BPF_F_???");

	/* prog_name field was added in Linux commit v4.15-rc1~84^2~605^2~4. */
	if (len <= offsetof(struct BPF_PROG_LOAD_struct, prog_name))
		break;
	PRINT_FIELD_CSTRING_SZ(", ", attr, prog_name,
			       MIN(sizeof(attr.prog_name),
				   len - offsetof(struct BPF_PROG_LOAD_struct,
						   prog_name)));

	/*
	 * prog_ifindex field was added as prog_target_ifindex in Linux commit
	 * v4.15-rc1~84^2~127^2~13 and renamed to its current name in
	 * v4.15-rc1~15^2~5^2~3^2~7.
	 */
	if (len <= offsetof(struct BPF_PROG_LOAD_struct, prog_ifindex))
		break;
	PRINT_FIELD_IFINDEX(", ", attr, prog_ifindex);

	/*
	 * expected_attach_type was added in Linux commit
	 * v4.17-rc1~148^2~19^2^2~8.
	 */
	if (len <= offsetof(struct BPF_PROG_LOAD_struct, expected_attach_type))
		break;
	PRINT_FIELD_XVAL(", ", attr, expected_attach_type, bpf_attach_type,
			 "BPF_???");

	/*
	 * The following seven fields were introduced by Linux commits
	 * v5.0-rc1~129^2~209^2~16^2~8 and v5.0-rc1~129^2~114^2~5^2~6.
	 */
	if (len <= offsetof(struct BPF_PROG_LOAD_struct, prog_btf_fd))
		break;
	PRINT_FIELD_FD(", ", attr, prog_btf_fd, tcp);
	PRINT_FIELD_U(", ", attr, func_info_rec_size);
	PRINT_FIELD_ADDR64(", ", attr, func_info);
	PRINT_FIELD_U(", ", attr, func_info_cnt);
	PRINT_FIELD_U(", ", attr, line_info_rec_size);
	PRINT_FIELD_ADDR64(", ", attr, line_info);
	PRINT_FIELD_U(", ", attr, line_info_cnt);
}
END_BPF_CMD_DECODER(RVAL_DECODED | RVAL_FD)

BEGIN_BPF_CMD_DECODER(BPF_OBJ_PIN)
{
	tprintf("{pathname=");
	print_big_u64_addr(attr.pathname);
	printpath(tcp, attr.pathname);

	PRINT_FIELD_FD(", ", attr, bpf_fd, tcp);

	/* file_flags field was added in Linux v4.15-rc1~84^2~384^2~4 */
	if (len <= offsetof(struct BPF_OBJ_PIN_struct, file_flags))
		break;
	PRINT_FIELD_FLAGS(", ", attr, file_flags, bpf_file_mode_flags,
			  "BPF_F_???");
}
END_BPF_CMD_DECODER(RVAL_DECODED | RVAL_FD)

#define decode_BPF_OBJ_GET decode_BPF_OBJ_PIN

BEGIN_BPF_CMD_DECODER(BPF_PROG_ATTACH)
{
	PRINT_FIELD_FD("{", attr, target_fd, tcp);
	PRINT_FIELD_FD(", ", attr, attach_bpf_fd, tcp);
	PRINT_FIELD_XVAL(", ", attr, attach_type, bpf_attach_type, "BPF_???");
	PRINT_FIELD_FLAGS(", ", attr, attach_flags, bpf_attach_flags,
			  "BPF_F_???");
}
END_BPF_CMD_DECODER(RVAL_DECODED)

BEGIN_BPF_CMD_DECODER(BPF_PROG_DETACH)
{
	PRINT_FIELD_FD("{", attr, target_fd, tcp);
	PRINT_FIELD_XVAL(", ", attr, attach_type, bpf_attach_type, "BPF_???");
}
END_BPF_CMD_DECODER(RVAL_DECODED)

BEGIN_BPF_CMD_DECODER(BPF_PROG_TEST_RUN)
{
	PRINT_FIELD_FD("{test={", attr, prog_fd, tcp);
	PRINT_FIELD_U(", ", attr, retval);
	PRINT_FIELD_U(", ", attr, data_size_in);
	PRINT_FIELD_U(", ", attr, data_size_out);
	PRINT_FIELD_ADDR64(", ", attr, data_in);
	PRINT_FIELD_ADDR64(", ", attr, data_out);
	PRINT_FIELD_U(", ", attr, repeat);
	PRINT_FIELD_U(", ", attr, duration);
	/*
	 * The following four fields were introduced by Linux commit
	 * v5.2-rc1~133^2~193^2~6.
	 */
	if (len > offsetof(struct BPF_PROG_TEST_RUN_struct, ctx_size_in)) {
		PRINT_FIELD_U(", ", attr, ctx_size_in);
		PRINT_FIELD_U(", ", attr, ctx_size_out);
		PRINT_FIELD_ADDR64(", ", attr, ctx_in);
		PRINT_FIELD_ADDR64(", ", attr, ctx_out);
	}
	tprints("}");
}
END_BPF_CMD_DECODER(RVAL_DECODED)

BEGIN_BPF_CMD_DECODER(BPF_PROG_GET_NEXT_ID)
{
	PRINT_FIELD_U("{", attr, start_id);
	PRINT_FIELD_U(", ", attr, next_id);

	/* open_flags field has been added in Linux v4.15-rc1~84^2~384^2~4 */
	if (len <= offsetof(struct BPF_PROG_GET_NEXT_ID_struct, open_flags))
		break;
	PRINT_FIELD_FLAGS(", ", attr, open_flags, bpf_file_mode_flags,
			  "BPF_F_???");
}
END_BPF_CMD_DECODER(RVAL_DECODED)

#define decode_BPF_MAP_GET_NEXT_ID decode_BPF_PROG_GET_NEXT_ID

BEGIN_BPF_CMD_DECODER(BPF_PROG_GET_FD_BY_ID)
{
	PRINT_FIELD_U("{", attr, prog_id);
	PRINT_FIELD_U(", ", attr, next_id);

	/* open_flags field has been added in Linux v4.15-rc1~84^2~384^2~4 */
	if (len <= offsetof(struct BPF_PROG_GET_FD_BY_ID_struct, open_flags))
		break;
	PRINT_FIELD_FLAGS(", ", attr, open_flags, bpf_file_mode_flags,
			  "BPF_F_???");
}
END_BPF_CMD_DECODER(RVAL_DECODED | RVAL_FD)

BEGIN_BPF_CMD_DECODER(BPF_MAP_GET_FD_BY_ID)
{
	PRINT_FIELD_U("{", attr, map_id);
	PRINT_FIELD_U(", ", attr, next_id);

	/* open_flags field has been added in Linux v4.15-rc1~84^2~384^2~4 */
	if (len <= offsetof(struct BPF_MAP_GET_FD_BY_ID_struct, open_flags))
		break;
	PRINT_FIELD_FLAGS(", ", attr, open_flags, bpf_file_mode_flags,
			  "BPF_F_???");
}
END_BPF_CMD_DECODER(RVAL_DECODED | RVAL_FD)

struct obj_get_info_saved;
typedef void (*print_bpf_obj_info_fn)(struct tcb *,
				      uint32_t bpf_fd,
				      const char *info_buf,
				      uint32_t size,
				      struct obj_get_info_saved *saved);

struct obj_get_info_saved {
	print_bpf_obj_info_fn print_fn;

	uint32_t info_len;

	uint32_t jited_prog_len;
	uint32_t xlated_prog_len;
	uint32_t nr_map_ids;

	uint32_t nr_jited_ksyms;
	uint32_t nr_jited_func_lens;
	uint64_t jited_ksyms;
	uint64_t jited_func_lens;

	uint32_t func_info_rec_size;
	uint32_t nr_func_info;
	uint32_t nr_line_info;
	uint32_t nr_jited_line_info;
	uint64_t jited_line_info;
	uint32_t line_info_rec_size;
	uint32_t jited_line_info_rec_size;
	uint32_t nr_prog_tags;
};

static void
print_bpf_map_info(struct tcb * const tcp, uint32_t bpf_fd,
		   const char *info_buf, uint32_t size,
		   struct obj_get_info_saved *saved)
{
	if (entering(tcp))
		return;

	struct bpf_map_info_struct info = { 0 };
	const unsigned int len = MIN(size, bpf_map_info_struct_size);

	memcpy(&info, info_buf, len);

	PRINT_FIELD_XVAL("{", info, type, bpf_map_types, "BPF_MAP_TYPE_???");
	PRINT_FIELD_U(", ", info, id);
	PRINT_FIELD_U(", ", info, key_size);
	PRINT_FIELD_U(", ", info, value_size);
	PRINT_FIELD_U(", ", info, max_entries);
	PRINT_FIELD_FLAGS(", ", info, map_flags, bpf_map_flags, "BPF_F_???");

	/*
	 * "name" field was introduced by Linux commit v4.15-rc1~84^2~605^2~3.
	 */
	if (len <= offsetof(struct bpf_map_info_struct, name))
		goto print_bpf_map_info_end;
	PRINT_FIELD_CSTRING(", ", info, name);

	/*
	 * ifindex, netns_dev, and netns_ino fields were introduced
	 * by Linux commit v4.16-rc1~123^2~109^2~5^2~4.
	 */
	if (len <= offsetof(struct bpf_map_info_struct, ifindex))
		goto print_bpf_map_info_end;
	PRINT_FIELD_IFINDEX(", ", info, ifindex);
	PRINT_FIELD_DEV(", ", info, netns_dev);
	PRINT_FIELD_U(", ", info, netns_ino);

	/*
	 * The next three fields were introduced by Linux commits
	 * v4.18-rc1~114^2~223^2~21^2~4 and v4.18-rc1~114^2~148^2~7^2~2.
	 */
	if (len <= offsetof(struct bpf_map_info_struct, btf_id))
		goto print_bpf_map_info_end;
	PRINT_FIELD_U(", ", info, btf_id);
	PRINT_FIELD_U(", ", info, btf_key_type_id);
	PRINT_FIELD_U(", ", info, btf_value_type_id);

	decode_attr_extra_data(tcp, info_buf, size, bpf_map_info_struct_size);

print_bpf_map_info_end:
	tprints("}");
}

static void
print_bpf_prog_info(struct tcb * const tcp, uint32_t bpf_fd,
		    const char *info_buf, uint32_t size,
		    struct obj_get_info_saved *saved)
{
	struct bpf_prog_info_struct info = { 0 };
	const unsigned int len = MIN(size, bpf_prog_info_struct_size);
	uint64_t map_id_buf;

	memcpy(&info, info_buf, len);

	if (entering(tcp)) {
		saved->jited_prog_len = info.jited_prog_len;
		saved->xlated_prog_len = info.xlated_prog_len;
		saved->nr_map_ids = info.nr_map_ids;
		saved->nr_jited_ksyms = info.nr_jited_ksyms;
		saved->nr_jited_func_lens = info.nr_jited_func_lens;
		saved->jited_ksyms = info.jited_ksyms;
		saved->jited_func_lens = info.jited_func_lens;

		saved->func_info_rec_size = info.func_info_rec_size;
		saved->nr_func_info = info.nr_func_info;
		saved->nr_line_info = info.nr_line_info;
		saved->nr_jited_line_info = info.nr_jited_line_info;
		saved->jited_line_info = info.jited_line_info;
		saved->line_info_rec_size = info.line_info_rec_size;
		saved->jited_line_info_rec_size = info.jited_line_info_rec_size;
		saved->nr_prog_tags = info.nr_prog_tags;

		return;
	}

	PRINT_FIELD_XVAL("{", info, type, bpf_prog_types, "BPF_PROG_TYPE_???");
	PRINT_FIELD_U(", ", info, id);
	PRINT_FIELD_HEX_ARRAY(", ", info, tag);

	tprints(", jited_prog_len=");
	if (saved->jited_prog_len != info.jited_prog_len)
		tprintf("%" PRIu32 " => ", saved->jited_prog_len);
	tprintf("%" PRIu32, info.jited_prog_len);

	tprints(", jited_prog_insns=");
	print_big_u64_addr(info.jited_prog_insns);
	printstr_ex(tcp, info.jited_prog_insns, info.jited_prog_len,
		    QUOTE_FORCE_HEX);

	tprints(", xlated_prog_len=");
	if (saved->xlated_prog_len != info.xlated_prog_len)
		tprintf("%" PRIu32 " => ", saved->xlated_prog_len);
	tprintf("%" PRIu32, info.xlated_prog_len);

	tprints(", xlated_prog_insns=");
	print_ebpf_prog(tcp, info.xlated_prog_insns,
			MIN(saved->xlated_prog_len, info.xlated_prog_len) / 8);

	/*
	 * load_time, created_by_uid, nr_map_ids, map_ids, and name fields
	 * were introduced by Linux commit v4.15-rc1~84^2~605^2~4.
	 */
	if (len <= offsetof(struct bpf_prog_info_struct, load_time))
		goto print_bpf_prog_info_end;
	PRINT_FIELD_U(", ", info, load_time);
	PRINT_FIELD_UID(", ", info, created_by_uid);

	tprints(", nr_map_ids=");
	if (saved->nr_map_ids != info.nr_map_ids)
		tprintf("%" PRIu32 " => ", saved->nr_map_ids);
	tprintf("%" PRIu32, info.nr_map_ids);

	tprints(", map_ids=");
	print_big_u64_addr(info.map_ids);
	print_array(tcp, info.map_ids, MIN(saved->nr_map_ids, info.nr_map_ids),
		    &map_id_buf, sizeof(map_id_buf),
		    tfetch_mem, print_uint32_array_member, 0);

	PRINT_FIELD_CSTRING(", ", info, name);

	/*
	 * ifindex, netns_dev, and netns_ino fields were introduced
	 * by Linux commit v4.16-rc1~123^2~227^2~5^2~2, and
	 * gpl_compatible was added later by Linux commit
	 * v4.18-rc1~114^2~376^2~6.
	 */
	if (len <= offsetof(struct bpf_prog_info_struct, ifindex))
		goto print_bpf_prog_info_end;
	PRINT_FIELD_IFINDEX(", ", info, ifindex);
	tprintf(", gpl_compatible=%u", info.gpl_compatible);
	PRINT_FIELD_DEV(", ", info, netns_dev);
	PRINT_FIELD_U(", ", info, netns_ino);

	/*
	 * The next four fields were introduced by Linux commits
	 * v4.18-rc1~114^2~148^2~3^2~6 and v4.18-rc1~114^2~148^2~3^2~2.
	 */
	if (len <= offsetof(struct bpf_prog_info_struct, nr_jited_ksyms))
		goto print_bpf_prog_info_end;

	tprints(", nr_jited_ksyms=");
	if (saved->nr_jited_ksyms != info.nr_jited_ksyms)
		tprintf("%" PRIu32 " => ", saved->nr_jited_ksyms);
	tprintf("%" PRIu32, info.nr_jited_ksyms);

	tprints(", nr_jited_func_lens=");
	if (saved->nr_jited_func_lens != info.nr_jited_func_lens)
		tprintf("%" PRIu32 " => ", saved->nr_jited_func_lens);
	tprintf("%" PRIu32, info.nr_jited_func_lens);

	tprints(", jited_ksyms=");
	if (saved->jited_ksyms != info.jited_ksyms) {
		printaddr64(saved->jited_ksyms);
		tprints(" => ");
	}
	printaddr64(info.jited_ksyms);

	tprints(", jited_func_lens=");
	if (saved->jited_func_lens != info.jited_func_lens) {
		printaddr64(saved->jited_func_lens);
		tprints(" => ");
	}
	printaddr64(info.jited_func_lens);

	/*
	 * The next twelve fields were introduced by Linux commits
	 * v5.0-rc1~129^2~209^2~16^2~8
	 * v5.0-rc1~129^2~114^2~5^2~6
	 * v5.0-rc1~129^2~114^2^2~2
	 * v5.0-rc1~129^2~15^2~22
	 */
	if (len <= offsetof(struct bpf_prog_info_struct, btf_id))
		goto print_bpf_prog_info_end;

	PRINT_FIELD_U(", ", info, btf_id);

	tprints(", func_info_rec_size=");
	if (saved->func_info_rec_size != info.func_info_rec_size)
		tprintf("%" PRIu32 " => ", saved->func_info_rec_size);
	tprintf("%" PRIu32, info.func_info_rec_size);

	PRINT_FIELD_ADDR64(", ", info, func_info);

	tprints(", nr_func_info=");
	if (saved->nr_func_info != info.nr_func_info)
		tprintf("%" PRIu32 " => ", saved->nr_func_info);
	tprintf("%" PRIu32, info.nr_func_info);

	tprints(", nr_line_info=");
	if (saved->nr_line_info != info.nr_line_info)
		tprintf("%" PRIu32 " => ", saved->nr_line_info);
	tprintf("%" PRIu32, info.nr_line_info);

	PRINT_FIELD_ADDR64(", ", info, line_info);

	tprints(", jited_line_info=");
	if (saved->jited_line_info != info.jited_line_info) {
		printaddr64(saved->jited_line_info);
		tprints(" => ");
	}
	printaddr64(info.jited_line_info);

	tprints(", nr_jited_line_info=");
	if (saved->nr_jited_line_info != info.nr_jited_line_info)
		tprintf("%" PRIu32 " => ", saved->nr_jited_line_info);
	tprintf("%" PRIu32, info.nr_jited_line_info);

	tprints(", line_info_rec_size=");
	if (saved->line_info_rec_size != info.line_info_rec_size)
		tprintf("%" PRIu32 " => ", saved->line_info_rec_size);
	tprintf("%" PRIu32, info.line_info_rec_size);

	tprints(", jited_line_info_rec_size=");
	if (saved->jited_line_info_rec_size != info.jited_line_info_rec_size)
		tprintf("%" PRIu32 " => ", saved->jited_line_info_rec_size);
	tprintf("%" PRIu32, info.jited_line_info_rec_size);

	tprints(", nr_prog_tags=");
	if (saved->nr_prog_tags != info.nr_prog_tags)
		tprintf("%" PRIu32 " => ", saved->nr_prog_tags);
	tprintf("%" PRIu32, info.nr_prog_tags);

	PRINT_FIELD_ADDR64(", ", info, prog_tags);

	/*
	 * run_time_ns and run_cnt fields were introduced
	 * by Linux commit v5.1-rc1~178^2~17^2~15^2~2.
	 */
	if (len <= offsetof(struct bpf_prog_info_struct, run_time_ns))
		goto print_bpf_prog_info_end;

	PRINT_FIELD_U(", ", info, run_time_ns);
	PRINT_FIELD_U(", ", info, run_cnt);

	decode_attr_extra_data(tcp, info_buf, size, bpf_prog_info_struct_size);

print_bpf_prog_info_end:
	tprints("}");
}

static const char *
fetch_bpf_obj_info(struct tcb * const tcp, uint64_t info, uint32_t size)
{
	static char *info_buf;

	if (!info_buf)
		info_buf = xmalloc(get_pagesize());

	memset(info_buf, 0, get_pagesize());

	if (size > 0 && size <= get_pagesize()
	    && !umoven(tcp, info, size, info_buf))
		return info_buf;

	return NULL;
}

static void
print_bpf_obj_info_addr(struct tcb * const tcp, uint64_t addr)
{
	if (exiting(tcp))
		printaddr64(addr);
}

static void
print_bpf_obj_info(struct tcb * const tcp, uint32_t bpf_fd, uint64_t info,
		   uint32_t size, struct obj_get_info_saved *saved)
{
	if (abbrev(tcp)) {
		print_bpf_obj_info_addr(tcp, info);
		return;
	}

	static struct {
		const char *id;
		print_bpf_obj_info_fn print_fn;
	} obj_printers[] = {
		{ "anon_inode:bpf-map", print_bpf_map_info },
		{ "anon_inode:bpf-prog", print_bpf_prog_info }
	};

	if (entering(tcp)) {
		char path[PATH_MAX + 1];

		if (getfdpath(tcp, bpf_fd, path, sizeof(path)) > 0) {
			for (size_t i = 0; i < ARRAY_SIZE(obj_printers); ++i) {
				if (!strcmp(path, obj_printers[i].id)) {
					saved->print_fn =
						obj_printers[i].print_fn;
					break;
				}
			}
		}
	}

	if (!saved || !saved->print_fn) {
		print_bpf_obj_info_addr(tcp, info);
		return;
	}

	const char *info_buf = fetch_bpf_obj_info(tcp, info, size);

	if (info_buf)
		saved->print_fn(tcp, bpf_fd, info_buf, size, saved);
	else
		print_bpf_obj_info_addr(tcp, info);
}

BEGIN_BPF_CMD_DECODER(BPF_OBJ_GET_INFO_BY_FD)
{
	struct obj_get_info_saved *saved;

	if (entering(tcp)) {
		saved = xzalloc(sizeof(*saved));
		saved->info_len = attr.info_len;
		set_tcb_priv_data(tcp, saved, free);

		PRINT_FIELD_FD("{info={", attr, bpf_fd, tcp);
		PRINT_FIELD_U(", ", attr, info_len);
	} else {
		saved = get_tcb_priv_data(tcp);

		if (saved && (saved->info_len != attr.info_len))
			tprintf(" => %u", attr.info_len);

		tprintf(", info=");
	}

	print_bpf_obj_info(tcp, attr.bpf_fd, attr.info, attr.info_len, saved);

	if (entering(tcp))
		return 0;

	tprints("}");
}
END_BPF_CMD_DECODER(RVAL_DECODED)

BEGIN_BPF_CMD_DECODER(BPF_PROG_QUERY)
{
	uint32_t prog_id_buf;

	if (entering(tcp)) {
		PRINT_FIELD_FD("{query={", attr, target_fd, tcp);
		PRINT_FIELD_XVAL(", ", attr, attach_type, bpf_attach_type,
				 "BPF_???");
		PRINT_FIELD_FLAGS(", ", attr, query_flags, bpf_query_flags,
				  "BPF_F_QUERY_???");
		PRINT_FIELD_FLAGS(", ", attr, attach_flags, bpf_attach_flags,
				  "BPF_F_???");

		tprints(", prog_ids=");

		set_tcb_priv_ulong(tcp, attr.prog_cnt);

		return 0;
	}

	print_big_u64_addr(attr.prog_ids);
	print_array(tcp, attr.prog_ids, attr.prog_cnt, &prog_id_buf,
		    sizeof(prog_id_buf), tfetch_mem,
		    print_uint32_array_member, 0);

	tprints(", prog_cnt=");
	const uint32_t prog_cnt_entering = get_tcb_priv_ulong(tcp);
	if (prog_cnt_entering != attr.prog_cnt)
		tprintf("%" PRIu32 " => ", prog_cnt_entering);
	tprintf("%" PRIu32, attr.prog_cnt);
	tprints("}");
}
END_BPF_CMD_DECODER(RVAL_DECODED)

BEGIN_BPF_CMD_DECODER(BPF_RAW_TRACEPOINT_OPEN)
{
	enum { TP_NAME_SIZE = 128 };

	tprintf("{raw_tracepoint={name=");
	print_big_u64_addr(attr.name);
	printstr_ex(tcp, attr.name, TP_NAME_SIZE, QUOTE_0_TERMINATED);

	PRINT_FIELD_FD(", ", attr, prog_fd, tcp);

	tprints("}");
}
END_BPF_CMD_DECODER(RVAL_DECODED)

BEGIN_BPF_CMD_DECODER(BPF_BTF_LOAD)
{
	tprints("{btf=");
	print_big_u64_addr(attr.btf);
	printstrn(tcp, attr.btf, attr.btf_size);
	PRINT_FIELD_ADDR64(", ", attr, btf_log_buf);
	PRINT_FIELD_U(", ", attr, btf_size);
	PRINT_FIELD_U(", ", attr, btf_log_size);
	PRINT_FIELD_U(", ", attr, btf_log_level);
}
END_BPF_CMD_DECODER(RVAL_DECODED | RVAL_FD)

BEGIN_BPF_CMD_DECODER(BPF_BTF_GET_FD_BY_ID)
{
	PRINT_FIELD_U("{", attr, btf_id);
}
END_BPF_CMD_DECODER(RVAL_DECODED | RVAL_FD)

BEGIN_BPF_CMD_DECODER(BPF_TASK_FD_QUERY)
{
	if (entering(tcp)) {
		set_tcb_priv_ulong(tcp, attr.buf_len);

		PRINT_FIELD_U("{task_fd_query={", attr, pid);
		PRINT_FIELD_FD(", ", attr, fd, tcp);
		PRINT_FIELD_U(", ", attr, flags);
		PRINT_FIELD_U(", ", attr, buf_len);

		return 0;
	}

	unsigned int saved_buf_len = get_tcb_priv_ulong(tcp);

	if (saved_buf_len != attr.buf_len)
		tprintf(" => %u", attr.buf_len);

	const unsigned int buf_len = MIN(saved_buf_len, attr.buf_len);
	tprintf(", buf=");
	print_big_u64_addr(attr.buf);
	printstr_ex(tcp, attr.buf, buf_len, QUOTE_0_TERMINATED);
	PRINT_FIELD_U(", ", attr, prog_id);
	PRINT_FIELD_XVAL(", ", attr, fd_type, bpf_task_fd_type,
			 "BPF_FD_TYPE_???");
	PRINT_FIELD_X(", ", attr, probe_offset);
	PRINT_FIELD_X(", ", attr, probe_addr);

	tprints("}");
}
END_BPF_CMD_DECODER(RVAL_DECODED)

SYS_FUNC(bpf)
{
	static const bpf_cmd_decoder_t bpf_cmd_decoders[] = {
		BPF_CMD_ENTRY(BPF_MAP_CREATE),
		BPF_CMD_ENTRY(BPF_MAP_LOOKUP_ELEM),
		BPF_CMD_ENTRY(BPF_MAP_UPDATE_ELEM),
		BPF_CMD_ENTRY(BPF_MAP_DELETE_ELEM),
		BPF_CMD_ENTRY(BPF_MAP_GET_NEXT_KEY),
		BPF_CMD_ENTRY(BPF_PROG_LOAD),
		BPF_CMD_ENTRY(BPF_OBJ_PIN),
		BPF_CMD_ENTRY(BPF_OBJ_GET),
		BPF_CMD_ENTRY(BPF_PROG_ATTACH),
		BPF_CMD_ENTRY(BPF_PROG_DETACH),
		BPF_CMD_ENTRY(BPF_PROG_TEST_RUN),
		BPF_CMD_ENTRY(BPF_PROG_GET_NEXT_ID),
		BPF_CMD_ENTRY(BPF_MAP_GET_NEXT_ID),
		BPF_CMD_ENTRY(BPF_PROG_GET_FD_BY_ID),
		BPF_CMD_ENTRY(BPF_MAP_GET_FD_BY_ID),
		BPF_CMD_ENTRY(BPF_OBJ_GET_INFO_BY_FD),
		BPF_CMD_ENTRY(BPF_PROG_QUERY),
		BPF_CMD_ENTRY(BPF_RAW_TRACEPOINT_OPEN),
		BPF_CMD_ENTRY(BPF_BTF_LOAD),
		BPF_CMD_ENTRY(BPF_BTF_GET_FD_BY_ID),
		BPF_CMD_ENTRY(BPF_TASK_FD_QUERY),
		BPF_CMD_ENTRY(BPF_MAP_LOOKUP_AND_DELETE_ELEM),
		BPF_CMD_ENTRY(BPF_MAP_FREEZE),
	};

	const unsigned int cmd = tcp->u_arg[0];
	const kernel_ulong_t addr = tcp->u_arg[1];
	const unsigned int size = tcp->u_arg[2];
	int rc = RVAL_DECODED;

	if (entering(tcp)) {
		printxval(bpf_commands, cmd, "BPF_???");
		tprints(", ");
	}

	if (size > 0
	    && size <= get_pagesize()
	    && cmd < ARRAY_SIZE(bpf_cmd_decoders)
	    && bpf_cmd_decoders[cmd]) {
		static char *buf;

		if (!buf)
			buf = xmalloc(get_pagesize());

		if (!umoven_or_printaddr_ignore_syserror(tcp, addr, size, buf))
			rc = bpf_cmd_decoders[cmd](tcp, addr, size, buf);
	} else {
		printaddr(addr);
	}

	if (exiting(tcp) || (rc & RVAL_DECODED))
		tprintf(", %u", size);

	return rc;
}
