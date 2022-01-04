/*
 * Builtin "git notes"
 *
 * Copyright (c) 2010 Johan Herland <johan@herland.net>
 *
 * Based on git-notes.sh by Johannes Schindelin,
 * and builtin/tag.c by Kristian Høgsberg and Carlos Rica.
 */

#include "cache.h"
#include "config.h"
#include "builtin.h"
#include "notes.h"
#include "object-store.h"
#include "repository.h"
#include "blob.h"
#include "pretty.h"
#include "refs.h"
#include "exec-cmd.h"
#include "run-command.h"
#include "parse-options.h"
#include "string-list.h"
#include "notes-merge.h"
#include "notes-utils.h"
#include "worktree.h"

static const char * const git_notes_usage[] = {
	N_("git notes [--ref <notes-ref>] [list [<object>]]"),
	N_("git notes [--ref <notes-ref>] add [-f] [--allow-empty] [-m <msg> | -F <file> | (-c | -C) <object>] [<object>]"),
	N_("git notes [--ref <notes-ref>] copy [-f] <from-object> <to-object>"),
	N_("git notes [--ref <notes-ref>] append [--allow-empty] [-m <msg> | -F <file> | (-c | -C) <object>] [<object>]"),
	N_("git notes [--ref <notes-ref>] edit [--allow-empty] [<object>]"),
	N_("git notes [--ref <notes-ref>] show [<object>]"),
	N_("git notes [--ref <notes-ref>] merge [-v | -q] [-s <strategy>] <notes-ref>"),
	N_("git notes merge --commit [-v | -q]"),
	N_("git notes merge --abort [-v | -q]"),
	N_("git notes [--ref <notes-ref>] remove [<object>...]"),
	N_("git notes [--ref <notes-ref>] prune [-n] [-v]"),
	N_("git notes [--ref <notes-ref>] get-ref"),
	NULL
};

static const char * const git_notes_list_usage[] = {
	N_("git notes [list [<object>]]"),
	NULL
};

static const char * const git_notes_add_usage[] = {
	N_("git notes add [<options>] [<object>]"),
	NULL
};

static const char * const git_notes_copy_usage[] = {
	N_("git notes copy [<options>] <from-object> <to-object>"),
	N_("git notes copy --stdin [<from-object> <to-object>]..."),
	NULL
};

static const char * const git_notes_append_usage[] = {
	N_("git notes append [<options>] [<object>]"),
	NULL
};

static const char * const git_notes_edit_usage[] = {
	N_("git notes edit [<object>]"),
	NULL
};

static const char * const git_notes_show_usage[] = {
	N_("git notes show [<object>]"),
	NULL
};

static const char * const git_notes_merge_usage[] = {
	N_("git notes merge [<options>] <notes-ref>"),
	N_("git notes merge --commit [<options>]"),
	N_("git notes merge --abort [<options>]"),
	NULL
};

static const char * const git_notes_remove_usage[] = {
	N_("git notes remove [<object>]"),
	NULL
};

static const char * const git_notes_prune_usage[] = {
	N_("git notes prune [<options>]"),
	NULL
};

static const char * const git_notes_get_ref_usage[] = {
	N_("git notes get-ref"),
	NULL
};

static const char note_template[] =
	N_("Write/edit the notes for the following object:");

struct note_data {
	int given;
	int use_editor;
	char *edit_path;
	struct strbuf buf;
};

static void free_note_data(struct note_data *d)
{
	if (d->edit_path) {
		unlink_or_warn(d->edit_path);
		free(d->edit_path);
	}
	strbuf_release(&d->buf);
}

static int list_each_note(const struct object_id *object_oid,
		const struct object_id *note_oid, char *note_path,
		void *cb_data)
{
	printf("%s %s\n", oid_to_hex(note_oid), oid_to_hex(object_oid));
	return 0;
}

static void copy_obj_to_fd(int fd, const struct object_id *oid)
{
	unsigned long size;
	enum object_type type;
	char *buf = read_object_file(oid, &type, &size);
	if (buf) {
		if (size)
			write_or_die(fd, buf, size);
		free(buf);
	}
}

static void write_commented_object(int fd, const struct object_id *object)
{
	const char *show_args[5] =
		{"show", "--stat", "--no-notes", oid_to_hex(object), NULL};
	struct child_process show = CHILD_PROCESS_INIT;
	struct strbuf buf = STRBUF_INIT;
	struct strbuf cbuf = STRBUF_INIT;

	/* Invoke "git show --stat --no-notes $object" */
	show.argv = show_args;
	show.no_stdin = 1;
	show.out = -1;
	show.err = 0;
	show.git_cmd = 1;
	if (start_command(&show))
		die(_("unable to start 'show' for object '%s'"),
		    oid_to_hex(object));

	if (strbuf_read(&buf, show.out, 0) < 0)
		die_errno(_("could not read 'show' output"));
	strbuf_add_commented_lines(&cbuf, buf.buf, buf.len);
	write_or_die(fd, cbuf.buf, cbuf.len);

	strbuf_release(&cbuf);
	strbuf_release(&buf);

	if (finish_command(&show))
		die(_("failed to finish 'show' for object '%s'"),
		    oid_to_hex(object));
}

static void prepare_note_data(const struct object_id *object, struct note_data *d,
		const struct object_id *old_note)
{
	if (d->use_editor || !d->given) {
		int fd;
		struct strbuf buf = STRBUF_INIT;

		/* write the template message before editing: */
		d->edit_path = git_pathdup("NOTES_EDITMSG");
		fd = open(d->edit_path, O_CREAT | O_TRUNC | O_WRONLY, 0600);
		if (fd < 0)
			die_errno(_("could not create file '%s'"), d->edit_path);

		if (d->given)
			write_or_die(fd, d->buf.buf, d->buf.len);
		else if (old_note)
			copy_obj_to_fd(fd, old_note);

		strbuf_addch(&buf, '\n');
		strbuf_add_commented_lines(&buf, "\n", strlen("\n"));
		strbuf_add_commented_lines(&buf, _(note_template), strlen(_(note_template)));
		strbuf_addch(&buf, '\n');
		write_or_die(fd, buf.buf, buf.len);

		write_commented_object(fd, object);

		close(fd);
		strbuf_release(&buf);
		strbuf_reset(&d->buf);

		if (launch_editor(d->edit_path, &d->buf, NULL)) {
			die(_("please supply the note contents using either -m or -F option"));
		}
		strbuf_stripspace(&d->buf, 1);
	}
}

static void write_note_data(struct note_data *d, struct object_id *oid)
{
	if (write_object_file(d->buf.buf, d->buf.len, blob_type, oid)) {
		error(_("unable to write note object"));
		if (d->edit_path)
			error(_("the note contents have been left in %s"),
				d->edit_path);
		exit(128);
	}
}

static int parse_msg_arg(const struct option *opt, const char *arg, int unset)
{
	struct note_data *d = opt->value;

	BUG_ON_OPT_NEG(unset);

	strbuf_grow(&d->buf, strlen(arg) + 2);
	if (d->buf.len)
		strbuf_addch(&d->buf, '\n');
	strbuf_addstr(&d->buf, arg);
	strbuf_stripspace(&d->buf, 0);

	d->given = 1;
	return 0;
}

static int parse_file_arg(const struct option *opt, const char *arg, int unset)
{
	struct note_data *d = opt->value;

	BUG_ON_OPT_NEG(unset);

	if (d->buf.len)
		strbuf_addch(&d->buf, '\n');
	if (!strcmp(arg, "-")) {
		if (strbuf_read(&d->buf, 0, 1024) < 0)
			die_errno(_("cannot read '%s'"), arg);
	} else if (strbuf_read_file(&d->buf, arg, 1024) < 0)
		die_errno(_("could not open or read '%s'"), arg);
	strbuf_stripspace(&d->buf, 0);

	d->given = 1;
	return 0;
}

static int parse_reuse_arg(const struct option *opt, const char *arg, int unset)
{
	struct note_data *d = opt->value;
	char *buf;
	struct object_id object;
	enum object_type type;
	unsigned long len;

	BUG_ON_OPT_NEG(unset);

	if (d->buf.len)
		strbuf_addch(&d->buf, '\n');

	if (get_oid(arg, &object))
		die(_("failed to resolve '%s' as a valid ref."), arg);
	if (!(buf = read_object_file(&object, &type, &len)))
		die(_("failed to read object '%s'."), arg);
	if (type != OBJ_BLOB) {
		free(buf);
		die(_("cannot read note data from non-blob object '%s'."), arg);
	}
	strbuf_add(&d->buf, buf, len);
	free(buf);

	d->given = 1;
	return 0;
}

static int parse_reedit_arg(const struct option *opt, const char *arg, int unset)
{
	struct note_data *d = opt->value;
	BUG_ON_OPT_NEG(unset);
	d->use_editor = 1;
	return parse_reuse_arg(opt, arg, unset);
}

static int notes_copy_from_stdin(int force, const char *rewrite_cmd)
{
	struct strbuf buf = STRBUF_INIT;
	struct notes_rewrite_cfg *c = NULL;
	struct notes_tree *t = NULL;
	int ret = 0;
	const char *msg = "Notes added by 'git notes copy'";

	if (rewrite_cmd) {
		c = init_copy_notes_for_rewrite(rewrite_cmd);
		if (!c)
			return 0;
	} else {
		init_notes(NULL, NULL, NULL, NOTES_INIT_WRITABLE);
		t = &default_notes_tree;
	}

	while (strbuf_getline_lf(&buf, stdin) != EOF) {
		struct object_id from_obj, to_obj;
		struct strbuf **split;
		int err;

		split = strbuf_split(&buf, ' ');
		if (!split[0] || !split[1])
			die(_("malformed input line: '%s'."), buf.buf);
		strbuf_rtrim(split[0]);
		strbuf_rtrim(split[1]);
		if (get_oid(split[0]->buf, &from_obj))
			die(_("failed to resolve '%s' as a valid ref."), split[0]->buf);
		if (get_oid(split[1]->buf, &to_obj))
			die(_("failed to resolve '%s' as a valid ref."), split[1]->buf);

		if (rewrite_cmd)
			err = copy_note_for_rewrite(c, &from_obj, &to_obj);
		else
			err = copy_note(t, &from_obj, &to_obj, force,
					combine_notes_overwrite);

		if (err) {
			error(_("failed to copy notes from '%s' to '%s'"),
			      split[0]->buf, split[1]->buf);
			ret = 1;
		}

		strbuf_list_free(split);
	}

	if (!rewrite_cmd) {
		commit_notes(the_repository, t, msg);
		free_notes(t);
	} else {
		finish_copy_notes_for_rewrite(the_repository, c, msg);
	}
	strbuf_release(&buf);
	return ret;
}

static struct notes_tree *init_notes_check(const char *subcommand,
					   int flags)
{
	struct notes_tree *t;
	const char *ref;
	init_notes(NULL, NULL, NULL, flags);
	t = &default_notes_tree;

	ref = (flags & NOTES_INIT_WRITABLE) ? t->update_ref : t->ref;
	if (!starts_with(ref, "refs/notes/"))
		/*
		 * TRANSLATORS: the first %s will be replaced by a git
		 * notes command: 'add', 'merge', 'remove', etc.
		 */
		die(_("refusing to %s notes in %s (outside of refs/notes/)"),
		    subcommand, ref);
	return t;
}

static int list(int argc, const char **argv, const char *prefix)
{
	struct notes_tree *t;
	struct object_id object;
	const struct object_id *note;
	int retval = -1;
	struct option options[] = {
		OPT_END()
	};

	if (argc)
		argc = parse_options(argc, argv, prefix, options,
				     git_notes_list_usage, 0);

	if (1 < argc) {
		error(_("too many parameters"));
		usage_with_options(git_notes_list_usage, options);
	}

	t = init_notes_check("list", 0);
	if (argc) {
		if (get_oid(argv[0], &object))
			die(_("failed to resolve '%s' as a valid ref."), argv[0]);
		note = get_note(t, &object);
		if (note) {
			puts(oid_to_hex(note));
			retval = 0;
		} else
			retval = error(_("no note found for object %s."),
				       oid_to_hex(&object));
	} else
		retval = for_each_note(t, 0, list_each_note, NULL);

	free_notes(t);
	return retval;
}

static int append_edit(int argc, const char **argv, const char *prefix);

static int add(int argc, const char **argv, const char *prefix)
{
	int force = 0, allow_empty = 0;
	const char *object_ref;
	struct notes_tree *t;
	struct object_id object, new_note;
	const struct object_id *note;
	struct note_data d = { 0, 0, NULL, STRBUF_INIT };
	struct option options[] = {
		{ OPTION_CALLBACK, 'm', "message", &d, N_("message"),
			N_("note contents as a string"), PARSE_OPT_NONEG,
			parse_msg_arg},
		{ OPTION_CALLBACK, 'F', "file", &d, N_("file"),
			N_("note contents in a file"), PARSE_OPT_NONEG,
			parse_file_arg},
		{ OPTION_CALLBACK, 'c', "reedit-message", &d, N_("object"),
			N_("reuse and edit specified note object"), PARSE_OPT_NONEG,
			parse_reedit_arg},
		{ OPTION_CALLBACK, 'C', "reuse-message", &d, N_("object"),
			N_("reuse specified note object"), PARSE_OPT_NONEG,
			parse_reuse_arg},
		OPT_BOOL(0, "allow-empty", &allow_empty,
			N_("allow storing empty note")),
		OPT__FORCE(&force, N_("replace existing notes"), PARSE_OPT_NOCOMPLETE),
		OPT_END()
	};

	argc = parse_options(argc, argv, prefix, options, git_notes_add_usage,
			     PARSE_OPT_KEEP_ARGV0);

	if (2 < argc) {
		error(_("too many parameters"));
		usage_with_options(git_notes_add_usage, options);
	}

	object_ref = argc > 1 ? argv[1] : "HEAD";

	if (get_oid(object_ref, &object))
		die(_("failed to resolve '%s' as a valid ref."), object_ref);

	t = init_notes_check("add", NOTES_INIT_WRITABLE);
	note = get_note(t, &object);

	if (note) {
		if (!force) {
			free_notes(t);
			if (d.given) {
				free_note_data(&d);
				return error(_("Cannot add notes. "
					"Found existing notes for object %s. "
					"Use '-f' to overwrite existing notes"),
					oid_to_hex(&object));
			}
			/*
			 * Redirect to "edit" subcommand.
			 *
			 * We only end up here if none of -m/-F/-c/-C or -f are
			 * given. The original args are therefore still in
			 * argv[0-1].
			 */
			argv[0] = "edit";
			return append_edit(argc, argv, prefix);
		}
		fprintf(stderr, _("Overwriting existing notes for object %s\n"),
			oid_to_hex(&object));
	}

	prepare_note_data(&object, &d, note);
	if (d.buf.len || allow_empty) {
		write_note_data(&d, &new_note);
		if (add_note(t, &object, &new_note, combine_notes_overwrite))
			BUG("combine_notes_overwrite failed");
		commit_notes(the_repository, t,
			     "Notes added by 'git notes add'");
	} else {
		fprintf(stderr, _("Removing note for object %s\n"),
			oid_to_hex(&object));
		remove_note(t, object.hash);
		commit_notes(the_repository, t,
			     "Notes removed by 'git notes add'");
	}

	free_note_data(&d);
	free_notes(t);
	return 0;
}

static int copy(int argc, const char **argv, const char *prefix)
{
	int retval = 0, force = 0, from_stdin = 0;
	const struct object_id *from_note, *note;
	const char *object_ref;
	struct object_id object, from_obj;
	struct notes_tree *t;
	const char *rewrite_cmd = NULL;
	struct option options[] = {
		OPT__FORCE(&force, N_("replace existing notes"), PARSE_OPT_NOCOMPLETE),
		OPT_BOOL(0, "stdin", &from_stdin, N_("read objects from stdin")),
		OPT_STRING(0, "for-rewrite", &rewrite_cmd, N_("command"),
			   N_("load rewriting config for <command> (implies "
			      "--stdin)")),
		OPT_END()
	};

	argc = parse_options(argc, argv, prefix, options, git_notes_copy_usage,
			     0);

	if (from_stdin || rewrite_cmd) {
		if (argc) {
			error(_("too many parameters"));
			usage_with_options(git_notes_copy_usage, options);
		} else {
			return notes_copy_from_stdin(force, rewrite_cmd);
		}
	}

	if (argc < 2) {
		error(_("too few parameters"));
		usage_with_options(git_notes_copy_usage, options);
	}
	if (2 < argc) {
		error(_("too many parameters"));
		usage_with_options(git_notes_copy_usage, options);
	}

	if (get_oid(argv[0], &from_obj))
		die(_("failed to resolve '%s' as a valid ref."), argv[0]);

	object_ref = 1 < argc ? argv[1] : "HEAD";

	if (get_oid(object_ref, &object))
		die(_("failed to resolve '%s' as a valid ref."), object_ref);

	t = init_notes_check("copy", NOTES_INIT_WRITABLE);
	note = get_note(t, &object);

	if (note) {
		if (!force) {
			retval = error(_("Cannot copy notes. Found existing "
				       "notes for object %s. Use '-f' to "
				       "overwrite existing notes"),
				       oid_to_hex(&object));
			goto out;
		}
		fprintf(stderr, _("Overwriting existing notes for object %s\n"),
			oid_to_hex(&object));
	}

	from_note = get_note(t, &from_obj);
	if (!from_note) {
		retval = error(_("missing notes on source object %s. Cannot "
			       "copy."), oid_to_hex(&from_obj));
		goto out;
	}

	if (add_note(t, &object, from_note, combine_notes_overwrite))
		BUG("combine_notes_overwrite failed");
	commit_notes(the_repository, t,
		     "Notes added by 'git notes copy'");
out:
	free_notes(t);
	return retval;
}

static int append_edit(int argc, const char **argv, const char *prefix)
{
	int allow_empty = 0;
	const char *object_ref;
	struct notes_tree *t;
	struct object_id object, new_note;
	const struct object_id *note;
	char *logmsg;
	const char * const *usage;
	struct note_data d = { 0, 0, NULL, STRBUF_INIT };
	struct option options[] = {
		{ OPTION_CALLBACK, 'm', "message", &d, N_("message"),
			N_("note contents as a string"), PARSE_OPT_NONEG,
			parse_msg_arg},
		{ OPTION_CALLBACK, 'F', "file", &d, N_("file"),
			N_("note contents in a file"), PARSE_OPT_NONEG,
			parse_file_arg},
		{ OPTION_CALLBACK, 'c', "reedit-message", &d, N_("object"),
			N_("reuse and edit specified note object"), PARSE_OPT_NONEG,
			parse_reedit_arg},
		{ OPTION_CALLBACK, 'C', "reuse-message", &d, N_("object"),
			N_("reuse specified note object"), PARSE_OPT_NONEG,
			parse_reuse_arg},
		OPT_BOOL(0, "allow-empty", &allow_empty,
			N_("allow storing empty note")),
		OPT_END()
	};
	int edit = !strcmp(argv[0], "edit");

	usage = edit ? git_notes_edit_usage : git_notes_append_usage;
	argc = parse_options(argc, argv, prefix, options, usage,
			     PARSE_OPT_KEEP_ARGV0);

	if (2 < argc) {
		error(_("too many parameters"));
		usage_with_options(usage, options);
	}

	if (d.given && edit)
		fprintf(stderr, _("The -m/-F/-c/-C options have been deprecated "
			"for the 'edit' subcommand.\n"
			"Please use 'git notes add -f -m/-F/-c/-C' instead.\n"));

	object_ref = 1 < argc ? argv[1] : "HEAD";

	if (get_oid(object_ref, &object))
		die(_("failed to resolve '%s' as a valid ref."), object_ref);

	t = init_notes_check(argv[0], NOTES_INIT_WRITABLE);
	note = get_note(t, &object);

	prepare_note_data(&object, &d, edit && note ? note : NULL);

	if (note && !edit) {
		/* Append buf to previous note contents */
		unsigned long size;
		enum object_type type;
		char *prev_buf = read_object_file(note, &type, &size);

		strbuf_grow(&d.buf, size + 1);
		if (d.buf.len && prev_buf && size)
			strbuf_insert(&d.buf, 0, "\n", 1);
		if (prev_buf && size)
			strbuf_insert(&d.buf, 0, prev_buf, size);
		free(prev_buf);
	}

	if (d.buf.len || allow_empty) {
		write_note_data(&d, &new_note);
		if (add_note(t, &object, &new_note, combine_notes_overwrite))
			BUG("combine_notes_overwrite failed");
		logmsg = xstrfmt("Notes added by 'git notes %s'", argv[0]);
	} else {
		fprintf(stderr, _("Removing note for object %s\n"),
			oid_to_hex(&object));
		remove_note(t, object.hash);
		logmsg = xstrfmt("Notes removed by 'git notes %s'", argv[0]);
	}
	commit_notes(the_repository, t, logmsg);

	free(logmsg);
	free_note_data(&d);
	free_notes(t);
	return 0;
}

static int show(int argc, const char **argv, const char *prefix)
{
	const char *object_ref;
	struct notes_tree *t;
	struct object_id object;
	const struct object_id *note;
	int retval;
	struct option options[] = {
		OPT_END()
	};

	argc = parse_options(argc, argv, prefix, options, git_notes_show_usage,
			     0);

	if (1 < argc) {
		error(_("too many parameters"));
		usage_with_options(git_notes_show_usage, options);
	}

	object_ref = argc ? argv[0] : "HEAD";

	if (get_oid(object_ref, &object))
		die(_("failed to resolve '%s' as a valid ref."), object_ref);

	t = init_notes_check("show", 0);
	note = get_note(t, &object);

	if (!note)
		retval = error(_("no note found for object %s."),
			       oid_to_hex(&object));
	else {
		const char *show_args[3] = {"show", oid_to_hex(note), NULL};
		retval = execv_git_cmd(show_args);
	}
	free_notes(t);
	return retval;
}

static int merge_abort(struct notes_merge_options *o)
{
	int ret = 0;

	/*
	 * Remove .git/NOTES_MERGE_PARTIAL and .git/NOTES_MERGE_REF, and call
	 * notes_merge_abort() to remove .git/NOTES_MERGE_WORKTREE.
	 */

	if (delete_ref(NULL, "NOTES_MERGE_PARTIAL", NULL, 0))
		ret += error(_("failed to delete ref NOTES_MERGE_PARTIAL"));
	if (delete_ref(NULL, "NOTES_MERGE_REF", NULL, REF_NO_DEREF))
		ret += error(_("failed to delete ref NOTES_MERGE_REF"));
	if (notes_merge_abort(o))
		ret += error(_("failed to remove 'git notes merge' worktree"));
	return ret;
}

static int merge_commit(struct notes_merge_options *o)
{
	struct strbuf msg = STRBUF_INIT;
	struct object_id oid, parent_oid;
	struct notes_tree *t;
	struct commit *partial;
	struct pretty_print_context pretty_ctx;
	void *local_ref_to_free;
	int ret;

	/*
	 * Read partial merge result from .git/NOTES_MERGE_PARTIAL,
	 * and target notes ref from .git/NOTES_MERGE_REF.
	 */

	if (get_oid("NOTES_MERGE_PARTIAL", &oid))
		die(_("failed to read ref NOTES_MERGE_PARTIAL"));
	else if (!(partial = lookup_commit_reference(the_repository, &oid)))
		die(_("could not find commit from NOTES_MERGE_PARTIAL."));
	else if (parse_commit(partial))
		die(_("could not parse commit from NOTES_MERGE_PARTIAL."));

	if (partial->parents)
		oidcpy(&parent_oid, &partial->parents->item->object.oid);
	else
		oidclr(&parent_oid);

	t = xcalloc(1, sizeof(struct notes_tree));
	init_notes(t, "NOTES_MERGE_PARTIAL", combine_notes_overwrite, 0);

	o->local_ref = local_ref_to_free =
		resolve_refdup("NOTES_MERGE_REF", 0, &oid, NULL);
	if (!o->local_ref)
		die(_("failed to resolve NOTES_MERGE_REF"));

	if (notes_merge_commit(o, t, partial, &oid))
		die(_("failed to finalize notes merge"));

	/* Reuse existing commit message in reflog message */
	memset(&pretty_ctx, 0, sizeof(pretty_ctx));
	format_commit_message(partial, "%s", &msg, &pretty_ctx);
	strbuf_trim(&msg);
	strbuf_insert(&msg, 0, "notes: ", 7);
	update_ref(msg.buf, o->local_ref, &oid,
		   is_null_oid(&parent_oid) ? NULL : &parent_oid,
		   0, UPDATE_REFS_DIE_ON_ERR);

	free_notes(t);
	strbuf_release(&msg);
	ret = merge_abort(o);
	free(local_ref_to_free);
	return ret;
}

static int git_config_get_notes_strategy(const char *key,
					 enum notes_merge_strategy *strategy)
{
	char *value;

	if (git_config_get_string(key, &value))
		return 1;
	if (parse_notes_merge_strategy(value, strategy))
		git_die_config(key, _("unknown notes merge strategy %s"), value);

	free(value);
	return 0;
}

static int merge(int argc, const char **argv, const char *prefix)
{
	struct strbuf remote_ref = STRBUF_INIT, msg = STRBUF_INIT;
	struct object_id result_oid;
	struct notes_tree *t;
	struct notes_merge_options o;
	int do_merge = 0, do_commit = 0, do_abort = 0;
	int verbosity = 0, result;
	const char *strategy = NULL;
	struct option options[] = {
		OPT_GROUP(N_("General options")),
		OPT__VERBOSITY(&verbosity),
		OPT_GROUP(N_("Merge options")),
		OPT_STRING('s', "strategy", &strategy, N_("strategy"),
			   N_("resolve notes conflicts using the given strategy "
			      "(manual/ours/theirs/union/cat_sort_uniq)")),
		OPT_GROUP(N_("Committing unmerged notes")),
		OPT_SET_INT_F(0, "commit", &do_commit,
			      N_("finalize notes merge by committing unmerged notes"),
			      1, PARSE_OPT_NONEG),
		OPT_GROUP(N_("Aborting notes merge resolution")),
		OPT_SET_INT_F(0, "abort", &do_abort,
			      N_("abort notes merge"),
			      1, PARSE_OPT_NONEG),
		OPT_END()
	};

	argc = parse_options(argc, argv, prefix, options,
			     git_notes_merge_usage, 0);

	if (strategy || do_commit + do_abort == 0)
		do_merge = 1;
	if (do_merge + do_commit + do_abort != 1) {
		error(_("cannot mix --commit, --abort or -s/--strategy"));
		usage_with_options(git_notes_merge_usage, options);
	}

	if (do_merge && argc != 1) {
		error(_("must specify a notes ref to merge"));
		usage_with_options(git_notes_merge_usage, options);
	} else if (!do_merge && argc) {
		error(_("too many parameters"));
		usage_with_options(git_notes_merge_usage, options);
	}

	init_notes_merge_options(the_repository, &o);
	o.verbosity = verbosity + NOTES_MERGE_VERBOSITY_DEFAULT;

	if (do_abort)
		return merge_abort(&o);
	if (do_commit)
		return merge_commit(&o);

	o.local_ref = default_notes_ref();
	strbuf_addstr(&remote_ref, argv[0]);
	expand_loose_notes_ref(&remote_ref);
	o.remote_ref = remote_ref.buf;

	t = init_notes_check("merge", NOTES_INIT_WRITABLE);

	if (strategy) {
		if (parse_notes_merge_strategy(strategy, &o.strategy)) {
			error(_("unknown -s/--strategy: %s"), strategy);
			usage_with_options(git_notes_merge_usage, options);
		}
	} else {
		struct strbuf merge_key = STRBUF_INIT;
		const char *short_ref = NULL;

		if (!skip_prefix(o.local_ref, "refs/notes/", &short_ref))
			BUG("local ref %s is outside of refs/notes/",
			    o.local_ref);

		strbuf_addf(&merge_key, "notes.%s.mergeStrategy", short_ref);

		if (git_config_get_notes_strategy(merge_key.buf, &o.strategy))
			git_config_get_notes_strategy("notes.mergeStrategy", &o.strategy);

		strbuf_release(&merge_key);
	}

	strbuf_addf(&msg, "notes: Merged notes from %s into %s",
		    remote_ref.buf, default_notes_ref());
	strbuf_add(&(o.commit_msg), msg.buf + 7, msg.len - 7); /* skip "notes: " */

	result = notes_merge(&o, t, &result_oid);

	if (result >= 0) /* Merge resulted (trivially) in result_oid */
		/* Update default notes ref with new commit */
		update_ref(msg.buf, default_notes_ref(), &result_oid, NULL, 0,
			   UPDATE_REFS_DIE_ON_ERR);
	else { /* Merge has unresolved conflicts */
		const struct worktree *wt;
		/* Update .git/NOTES_MERGE_PARTIAL with partial merge result */
		update_ref(msg.buf, "NOTES_MERGE_PARTIAL", &result_oid, NULL,
			   0, UPDATE_REFS_DIE_ON_ERR);
		/* Store ref-to-be-updated into .git/NOTES_MERGE_REF */
		wt = find_shared_symref("NOTES_MERGE_REF", default_notes_ref());
		if (wt)
			die(_("a notes merge into %s is already in-progress at %s"),
			    default_notes_ref(), wt->path);
		if (create_symref("NOTES_MERGE_REF", default_notes_ref(), NULL))
			die(_("failed to store link to current notes ref (%s)"),
			    default_notes_ref());
		fprintf(stderr, _("Automatic notes merge failed. Fix conflicts in %s "
				  "and commit the result with 'git notes merge --commit', "
				  "or abort the merge with 'git notes merge --abort'.\n"),
			git_path(NOTES_MERGE_WORKTREE));
	}

	free_notes(t);
	strbuf_release(&remote_ref);
	strbuf_release(&msg);
	return result < 0; /* return non-zero on conflicts */
}

#define IGNORE_MISSING 1

static int remove_one_note(struct notes_tree *t, const char *name, unsigned flag)
{
	int status;
	struct object_id oid;
	if (get_oid(name, &oid))
		return error(_("Failed to resolve '%s' as a valid ref."), name);
	status = remove_note(t, oid.hash);
	if (status)
		fprintf(stderr, _("Object %s has no note\n"), name);
	else
		fprintf(stderr, _("Removing note for object %s\n"), name);
	return (flag & IGNORE_MISSING) ? 0 : status;
}

static int remove_cmd(int argc, const char **argv, const char *prefix)
{
	unsigned flag = 0;
	int from_stdin = 0;
	struct option options[] = {
		OPT_BIT(0, "ignore-missing", &flag,
			N_("attempt to remove non-existent note is not an error"),
			IGNORE_MISSING),
		OPT_BOOL(0, "stdin", &from_stdin,
			    N_("read object names from the standard input")),
		OPT_END()
	};
	struct notes_tree *t;
	int retval = 0;

	argc = parse_options(argc, argv, prefix, options,
			     git_notes_remove_usage, 0);

	t = init_notes_check("remove", NOTES_INIT_WRITABLE);

	if (!argc && !from_stdin) {
		retval = remove_one_note(t, "HEAD", flag);
	} else {
		while (*argv) {
			retval |= remove_one_note(t, *argv, flag);
			argv++;
		}
	}
	if (from_stdin) {
		struct strbuf sb = STRBUF_INIT;
		while (strbuf_getwholeline(&sb, stdin, '\n') != EOF) {
			strbuf_rtrim(&sb);
			retval |= remove_one_note(t, sb.buf, flag);
		}
		strbuf_release(&sb);
	}
	if (!retval)
		commit_notes(the_repository, t,
			     "Notes removed by 'git notes remove'");
	free_notes(t);
	return retval;
}

static int prune(int argc, const char **argv, const char *prefix)
{
	struct notes_tree *t;
	int show_only = 0, verbose = 0;
	struct option options[] = {
		OPT__DRY_RUN(&show_only, N_("do not remove, show only")),
		OPT__VERBOSE(&verbose, N_("report pruned notes")),
		OPT_END()
	};

	argc = parse_options(argc, argv, prefix, options, git_notes_prune_usage,
			     0);

	if (argc) {
		error(_("too many parameters"));
		usage_with_options(git_notes_prune_usage, options);
	}

	t = init_notes_check("prune", NOTES_INIT_WRITABLE);

	prune_notes(t, (verbose ? NOTES_PRUNE_VERBOSE : 0) |
		(show_only ? NOTES_PRUNE_VERBOSE|NOTES_PRUNE_DRYRUN : 0) );
	if (!show_only)
		commit_notes(the_repository, t,
			     "Notes removed by 'git notes prune'");
	free_notes(t);
	return 0;
}

static int get_ref(int argc, const char **argv, const char *prefix)
{
	struct option options[] = { OPT_END() };
	argc = parse_options(argc, argv, prefix, options,
			     git_notes_get_ref_usage, 0);

	if (argc) {
		error(_("too many parameters"));
		usage_with_options(git_notes_get_ref_usage, options);
	}

	puts(default_notes_ref());
	return 0;
}

int cmd_notes(int argc, const char **argv, const char *prefix)
{
	int result;
	const char *override_notes_ref = NULL;
	struct option options[] = {
		OPT_STRING(0, "ref", &override_notes_ref, N_("notes-ref"),
			   N_("use notes from <notes-ref>")),
		OPT_END()
	};

	git_config(git_default_config, NULL);
	argc = parse_options(argc, argv, prefix, options, git_notes_usage,
			     PARSE_OPT_STOP_AT_NON_OPTION);

	if (override_notes_ref) {
		struct strbuf sb = STRBUF_INIT;
		strbuf_addstr(&sb, override_notes_ref);
		expand_notes_ref(&sb);
		setenv("GIT_NOTES_REF", sb.buf, 1);
		strbuf_release(&sb);
	}

	if (argc < 1 || !strcmp(argv[0], "list"))
		result = list(argc, argv, prefix);
	else if (!strcmp(argv[0], "add"))
		result = add(argc, argv, prefix);
	else if (!strcmp(argv[0], "copy"))
		result = copy(argc, argv, prefix);
	else if (!strcmp(argv[0], "append") || !strcmp(argv[0], "edit"))
		result = append_edit(argc, argv, prefix);
	else if (!strcmp(argv[0], "show"))
		result = show(argc, argv, prefix);
	else if (!strcmp(argv[0], "merge"))
		result = merge(argc, argv, prefix);
	else if (!strcmp(argv[0], "remove"))
		result = remove_cmd(argc, argv, prefix);
	else if (!strcmp(argv[0], "prune"))
		result = prune(argc, argv, prefix);
	else if (!strcmp(argv[0], "get-ref"))
		result = get_ref(argc, argv, prefix);
	else {
		result = error(_("unknown subcommand: %s"), argv[0]);
		usage_with_options(git_notes_usage, options);
	}

	return result ? 1 : 0;
}
