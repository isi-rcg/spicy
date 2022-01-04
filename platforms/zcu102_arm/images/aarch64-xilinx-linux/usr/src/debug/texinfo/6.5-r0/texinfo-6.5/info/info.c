/* info.c -- Display nodes of Info files in multiple windows.
   $Id: info.c 7911 2017-07-09 15:12:16Z gavin $

   Copyright 1993, 1996, 1997, 1998, 1999, 2000, 2001, 2002, 2003,
   2004, 2005, 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015,
   2016, 2017 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   Originally written by Brian Fox.  */

#include "info.h"
#include "filesys.h"
#include "info-utils.h"
#include "session.h"
#include "indices.h"
#include "dribble.h"
#include "getopt.h"
#include "man.h"
#include "variables.h"

char *program_name = "info";

/* Non-zero means search all indices for APROPOS_SEARCH_STRING. */
static int apropos_p = 0;

/* Variable containing the string to search for when apropos_p is non-zero. */
static char *apropos_search_string = NULL;

/* Non-zero means search all indices for INDEX_SEARCH_STRING.  Unlike
   apropos, this puts the user at the node, running info. */
static int index_search_p = 0;

/* Non-zero means look for the node which describes the invocation
   and command-line options of the program, and start the info
   session at that node.  */
static int goto_invocation_p = 0;

static char *invocation_program_name = 0;

/* Variable containing the string to search for when index_search_p is
   non-zero. */
static char *index_search_string = NULL;

/* Non-zero means print version info only. */
static int print_version_p = 0;

/* Non-zero means print a short description of the options. */
static int print_help_p = 0;

/* Name of file to start session with.  Default file for --node arguments. */
static char *initial_file = 0;

/* Array of the names of nodes that the user specified with "--node" on the
   command line. */
static char **user_nodenames = NULL;
static size_t user_nodenames_index = 0;
static size_t user_nodenames_slots = 0;

/* References to the nodes to start the session with. */
static REFERENCE **ref_list = NULL;
static size_t ref_slots = 0;
static size_t ref_index = 0;

/* String specifying the first file to load.  This string can only be set
   by the user specifying "--file" on the command line. */
static char *user_filename = NULL;

/* String specifying the name of the file to dump nodes to.  This value is
   filled if the user speficies "--output" on the command line. */
static char *user_output_filename = NULL;

/* Non-zero indicates that when "--output" is specified, all of the menu
   items of the specified nodes (and their subnodes as well) should be
   dumped in the order encountered.  This basically can print a book. */
int dump_subnodes = 0;

/* Non-zero means make default keybindings be loosely modeled on vi(1).  */
int vi_keys_p = 0;

/* Non-zero means don't remove ANSI escape sequences.  */
int raw_escapes_p = 1;

/* Print/visit all matching documents. */
static int all_matches_p = 0;

/* Non-zero means print the absolute location of the file to be loaded.  */
static int print_where_p = 0;

/* Non-zero means don't try to be smart when searching for nodes.  */
int strict_node_location_p = 0;

#if defined(__MSDOS__) || defined(__MINGW32__)
/* Non-zero indicates that screen output should be made 'speech-friendly'.
   Since on MSDOS the usual behavior is to write directly to the video
   memory, speech synthesizer software cannot grab the output.  Therefore,
   we provide a user option which tells us to avoid direct screen output
   and use stdout instead (which loses the color output).  */
int speech_friendly = 0;
#endif

/* Structure describing the options that Info accepts.  We pass this structure
   to getopt_long ().  If you add or otherwise change this structure, you must
   also change the string which follows it. */
#define DRIBBLE_OPTION 2
#define RESTORE_OPTION 3
#define IDXSRCH_OPTION 4
#define INITFLE_OPTION 5
#define VIRTIDX_OPTION 6

static struct option long_options[] = {
  { "all", 0, 0, 'a' },
  { "apropos", 1, 0, 'k' },
  { "debug", 1, 0, 'x' },
  { "directory", 1, 0, 'd' },
  { "dribble", 1, 0, DRIBBLE_OPTION },
  { "file", 1, 0, 'f' },
  { "help", 0, &print_help_p, 1 },
  { "index-search", 1, 0, IDXSRCH_OPTION },
  { "init-file", 1, 0, INITFLE_OPTION },
  { "location", 0, &print_where_p, 1 },
  { "node", 1, 0, 'n' },
  { "output", 1, 0, 'o' },
  { "raw-escapes", 0, &raw_escapes_p, 1 },
  { "no-raw-escapes", 0, &raw_escapes_p, 0 },
  { "show-malformed-multibytes", 0, &show_malformed_multibyte_p, 1 },
  { "no-show-malformed-multibytes", 0, &show_malformed_multibyte_p, 0 },
  { "restore", 1, 0, RESTORE_OPTION },
  { "show-options", 0, 0, 'O' },
  { "strict-node-location", 0, &strict_node_location_p, 1 },
  { "subnodes", 0, &dump_subnodes, 1 },
  { "usage", 0, 0, 'O' },
  { "variable", 1, 0, 'v' },
  { "version", 0, &print_version_p, 1 },
  { "vi-keys", 0, &vi_keys_p, 1 },
  { "where", 0, &print_where_p, 1 },
#if defined(__MSDOS__) || defined(__MINGW32__)
  { "speech-friendly", 0, &speech_friendly, 1 },
#endif
  {NULL, 0, NULL, 0}
};

/* String describing the shorthand versions of the long options found above. */
#if defined(__MSDOS__) || defined(__MINGW32__)
static char *short_options = "ak:d:n:f:ho:ORsv:wbx:";
#else
static char *short_options = "ak:d:n:f:ho:ORv:wsx:";
#endif

/* When non-zero, the Info window system has been initialized. */
int info_windows_initialized_p = 0;

/* Some "forward" declarations. */
static void info_short_help (void);
static void init_messages (void);


/* Find the first file to load (and possibly the first node as well).
   If the --file option is given, use that as the file, otherwise try to
   interpret the first non-option argument, either by looking it up as a dir 
   entry, looking for a file by that name, or finding a man page by that name.  
   Set INITIAL_FILE to the name of the initial Info file. */
static void
get_initial_file (int *argc, char ***argv, char **error)
{
  REFERENCE *entry;

  /* User used "--file". */
  if (user_filename)
    {
      if (!IS_ABSOLUTE(user_filename) && HAS_SLASH(user_filename)
          && !(user_filename[0] == '.' && IS_SLASH(user_filename[1])))
        {
          /* Prefix "./" to the filename to prevent a lookup
             in INFOPATH.  */
          char *s;
          asprintf (&s, "%s%s", "./", user_filename);
          free (user_filename);
          user_filename = s;
        }
      if (IS_ABSOLUTE(user_filename) || HAS_SLASH(user_filename))
        initial_file = info_add_extension (0, user_filename, 0);
      else
        initial_file = info_find_fullpath (user_filename, 0);

      if (!initial_file)
        {
          if (!filesys_error_number)
            filesys_error_number = ENOENT;
          *error = filesys_error_string (user_filename, filesys_error_number);
        }

      return;
    }

  if (!(*argv)[0])
    {
      /* No more non-option arguments. */
      initial_file = xstrdup("dir");
      return;
    }

  /* If first argument begins with '(', add it as if it were given with 
     '--node'.  This is to support invoking like
     "info '(emacs)Buffers'".  If it is a well-formed node spec then
     the rest of the arguments are menu entries to follow, or an
     index entry.  */
  if ((*argv)[0][0] == '(')
    {
      info_parse_node ((*argv)[0]);
      if (info_parsed_filename)
        {
          initial_file = info_find_fullpath (info_parsed_filename, 0);
          if (initial_file)
            {
              add_pointer_to_array (info_new_reference (initial_file,
                                                        info_parsed_nodename),
                                    ref_index, ref_list, ref_slots, 2);
              /* Remove this argument from the argument list. */
              memmove (*argv, *argv + 1, *argc-- * sizeof (char *));
              return;
            }
        }
    }

  /* If there are any more arguments, the initial file is the
     dir entry given by the first one. */
    {
      /* If they say info info (or info -O info, etc.), show them 
         info-stnd.texi.  (Get info.texi with info -f info.) */
      if ((*argv)[0] && mbscasecmp ((*argv)[0], "info") == 0)
        (*argv)[0] = "info-stnd";

      entry = lookup_dir_entry ((*argv)[0], 0);
      if (entry)
        {
          initial_file = info_find_fullpath (entry->filename, 0);
          if (initial_file)
            {
              REFERENCE *copy;
              (*argv)++; /* Advance past first remaining argument. */
              (*argc)--;

              copy = info_copy_reference (entry);
              /* Store full path, so that we find the already loaded file in
                 info_find_file, and show the full path if --where is used. */
              free (copy->filename);
              copy->filename = xstrdup (initial_file);
              add_pointer_to_array (copy, ref_index, ref_list, ref_slots, 2);
              return;
            }
        }
    }

  /* File name lookup. */
    {
      /* Try finding a file with this name, in case
         it exists, but wasn't listed in dir. */
      initial_file = info_find_fullpath ((*argv)[0], 0);
      if (initial_file)
        {
          add_pointer_to_array (info_new_reference ((*argv)[0], "Top"),
                                ref_index, ref_list, ref_slots, 2);
          (*argv)++; /* Advance past first remaining argument. */
          (*argc)--;
          return;
        }
      else
        asprintf (error, _("No menu item '%s' in node '%s'"),
            (*argv)[0], "(dir)Top");
    }

  /* Fall back to loading man page. */
    {
      NODE *man_node;

      debug (3, ("falling back to manpage node"));

      man_node = get_manpage_node ((*argv)[0]);
      if (man_node)
        {
          add_pointer_to_array
            (info_new_reference (MANPAGE_FILE_BUFFER_NAME, (*argv)[0]),
             ref_index, ref_list, ref_slots, 2);

          initial_file = MANPAGE_FILE_BUFFER_NAME;
          return;
        }
    }

  /* Inexact dir lookup. */
    {
      entry = lookup_dir_entry ((*argv)[0], 1);
      if (entry)
        {
          initial_file = info_find_fullpath (entry->filename, 0);
          if (initial_file)
            {
              REFERENCE *copy;
              (*argv)++; /* Advance past first remaining argument. */
              (*argc)--;
              /* Clear error message. */
              free (*error);
              *error = 0;

              copy = info_copy_reference (entry);
              /* Store full path, so that we find the already loaded file in
                 info_find_file, and show the full path if --where is used. */
              free (copy->filename);
              copy->filename = initial_file;
              add_pointer_to_array (copy, ref_index, ref_list, ref_slots, 2);
              return;
            }
        }
    }

  return;
}

/* Expand list of nodes to be loaded. */
static void
add_initial_nodes (int argc, char **argv, char **error)
{
  /* Add nodes specified with --node. */
  if (user_nodenames)
    {
      int i;

      /* If any --node arguments were given, the node in ref_list[0] is only 
         used to set initial_file. */
      if (user_nodenames_index > 0 && ref_index > 0)
        {
          info_reference_free (ref_list[0]);
          ref_list[0] = 0;
          ref_index = 0;
        }

      for (i = 0; user_nodenames[i]; i++)
        {
          char *node_filename = 0;
          char *node_nodename = 0;

          /* Parse node spec to support invoking
             like info --node "(emacs)Buffers". */
          info_parse_node (user_nodenames[i]);
          if (info_parsed_filename)
            {
              node_filename = info_parsed_filename;
              node_nodename = info_parsed_nodename;
            }
          else
            {
              FILE_BUFFER *file_buffer;
              TAG *tag;
              int j;

              if (!initial_file)
                continue; /* Shouldn't happen. */

              /* Check for a node by this name, and if there isn't one
                 look for an inexact match. */

              node_filename = initial_file;
              node_nodename = 0;

              file_buffer = info_find_file (node_filename);
              if (!file_buffer)
                continue;

              /* First look for an exact match. */
              for (j = 0; (tag = file_buffer->tags[j]); j++)
                if (strcmp (user_nodenames[i], tag->nodename) == 0)
                  {
                    node_nodename = tag->nodename;
                    break;
                  }

              if (!node_nodename)
                {
                  int best_guess = -1;
                  int len = strlen (user_nodenames[i]);
                  for (j = 0; (tag = file_buffer->tags[j]); j++)
                    {
                      if (mbscasecmp (user_nodenames[i], tag->nodename) == 0)
                        {
                          /* Exact, case-insensitive match. */
                          node_nodename = tag->nodename;
                          best_guess = -1;
                          break;
                        }
                      else if (best_guess == -1
                               && (mbsncasecmp (user_nodenames[i],
                                                tag->nodename, len) == 0))
                        /* Case-insensitive initial substring. */
                        best_guess = j;
                    }
                  if (best_guess != -1)
                    {
                      node_nodename = file_buffer->tags[best_guess]->nodename;
                    }
                }

              if (!node_nodename)
                {
                  free (*error);
                  asprintf (error, _("Cannot find node '%s'"),
                            user_nodenames[i]);
                  continue;
                }
            }

          if (node_filename && node_nodename)
            add_pointer_to_array
              (info_new_reference (node_filename, node_nodename),
               ref_index, ref_list, ref_slots, 2);
        }
    }

  if (goto_invocation_p)
    {
      NODE *top_node = 0;
      REFERENCE *invoc_ref = 0;

      char *program;

      if (ref_index == 0)
        {
          info_error (_("No program name given"));
          exit (1);
        }

      if (invocation_program_name)
        program = xstrdup (invocation_program_name);
      else if (ref_list[0] && ref_list[0]->filename)
        /* If there's no command-line arguments to
           supply the program name, use the Info file
           name (sans extension and leading directories)
           instead.  */
        program = program_name_from_file_name (ref_list[0]->filename);
      else
        program = xstrdup ("");
      
      if (ref_index > 0)
        top_node = info_get_node (ref_list[0]->filename, 
                                  ref_list[0]->nodename);
      if (top_node)
        invoc_ref = info_intuit_options_node (top_node, program);
      if (invoc_ref)
        {
          info_reference_free (ref_list[0]);
          ref_index = 0;

          add_pointer_to_array (invoc_ref, ref_index, ref_list, ref_slots, 2);
        }
      free (program);
    }

  /* Default is the "Top" node if there were no other nodes. */
  if (ref_index == 0 && initial_file)
    {
       add_pointer_to_array (info_new_reference (initial_file, "Top"), 
                             ref_index, ref_list, ref_slots, 2);
    }

  /* If there are arguments remaining, they are the names of menu items
     in sequential info files starting from the first one loaded. */
  if (*argv && ref_index > 0)
    {
      NODE *initial_node; /* Node to start following menus from. */
      NODE *node_via_menus;

      initial_node = info_get_node_with_defaults (ref_list[0]->filename,
                                                  ref_list[0]->nodename, 0);
      if (!initial_node)
        return;

      node_via_menus = info_follow_menus (initial_node, argv, error, 1);
      if (node_via_menus)
        {
          argv += argc; argc = 0;

          info_reference_free (ref_list[0]);
          ref_list[0] = info_new_reference (node_via_menus->fullpath,
                                            node_via_menus->nodename);
          free_history_node (node_via_menus);
        }

      /* If no nodes found, and there is exactly one argument remaining,
         check for it as an index entry. */
      else if (argc == 1 && argv[0])
        {
          FILE_BUFFER *fb;
          REFERENCE *match;

          debug (3, ("looking in indices"));
          fb = info_find_file (ref_list[0]->filename);
          if (fb)
            {
              match = look_in_indices (fb, argv[0], 0);
              if (match)
                {
                  argv += argc; argc = 0;
                  free (*error); *error = 0;

                  info_reference_free (ref_list[0]);
                  ref_list[0] = info_copy_reference (match);
                }
            }
        }

      /* If there are arguments remaining, follow menus inexactly. */
      if (argc != 0)
        {
          initial_node = info_get_node_with_defaults (ref_list[0]->filename,
                                                      ref_list[0]->nodename,
                                                      0);
          free (*error); *error = 0;
          node_via_menus = info_follow_menus (initial_node, argv, error, 0);
          if (node_via_menus)
            {
              if (argc >= 2 || !*error)
                {
                  argv += argc; argc = 0;

                  info_reference_free (ref_list[0]);
                  ref_list[0] = info_new_reference (node_via_menus->fullpath,
                                                    node_via_menus->nodename);
                }
              free_history_node (node_via_menus);
            }
        }

      /* If still no nodes found, and there is exactly one argument remaining,
         look in indices sloppily. */
      if (argc == 1)
        {
          FILE_BUFFER *fb;
          REFERENCE *nearest;

          debug (3, ("looking in indices sloppily"));
          fb = info_find_file (ref_list[0]->filename);
          if (fb)
            {
              nearest = look_in_indices (fb, argv[0], 1);
              if (nearest)
                {
                  argv += argc; argc = 0;
                  free (*error); *error = 0;

                  info_reference_free (ref_list[0]);
                  ref_list[0] = info_copy_reference (nearest);
                }
            }
        }
    }

  return;
}

static void
info_find_matching_files (char *filename)
{
  int i;
  char *searchdir;

  NODE *man_node;

  /* Check for dir entries first. */
  i = 0;
  for (searchdir = infopath_first (&i); searchdir;
       searchdir = infopath_next (&i))
    {
      REFERENCE *new_ref = dir_entry_of_infodir (filename, searchdir);

      if (new_ref)
        add_pointer_to_array (new_ref, ref_index, ref_list, ref_slots, 2);
    }

  /* Look for files with matching names. */
  i = 0;
  while (1)
    {
      char *p;
      int j;

      p = info_file_find_next_in_path (filename, &i, 0);
      if (!p)
        break;

      /* Add to list only if the file is not in the list already (which would
         happen if there was a dir entry with the label and filename both
         being this file). */
      for (j = 0; j < ref_index; j++)
        {
          if (!strcmp (p, ref_list[j]->filename))
            break;
        }

      if (j == ref_index)
        {
          add_pointer_to_array (info_new_reference (p, 0),
            ref_index, ref_list, ref_slots, 2);
        }
      free (p);
    }

  /* Check for man page. */
  man_node = get_manpage_node (filename);
  if (man_node)
    {
      free (man_node);
      add_pointer_to_array
        (info_new_reference (MANPAGE_FILE_BUFFER_NAME, filename),
         ref_index, ref_list, ref_slots, 2);
    }
}


static void
set_debug_level (const char *arg)
{
  char *p;
  long n = strtol (arg, &p, 10);
  if (*p)
    {
      fprintf (stderr, _("invalid number: %s\n"), arg);
      exit (EXIT_FAILURE);
    }
  if (n < 0 || n > UINT_MAX)
    debug_level = UINT_MAX;
  else
    debug_level = n;
}
      
static void
add_file_directory_to_path (char *filename)
{
  char *directory_name = xstrdup (filename);
  char *temp = filename_non_directory (directory_name);

  if (temp != directory_name)
    {
      if (HAVE_DRIVE (directory_name) && temp == directory_name + 2)
	{
	  /* The directory of "d:foo" is stored as "d:.", to avoid
	     mixing it with "d:/" when a slash is appended.  */
	  *temp = '.';
	  temp += 2;
	}
      temp[-1] = 0;
      infopath_add (directory_name);
    }

  free (directory_name);
}


/* **************************************************************** */
/*                                                                  */
/*                Main Entry Point to the Info Program              */
/*                                                                  */
/* **************************************************************** */

int
main (int argc, char *argv[])
{
  int getopt_long_index;       /* Index returned by getopt_long (). */
  char *init_file = 0;         /* Name of init file specified. */
  char *error = 0;             /* Error message to display in mini-buffer. */

#ifdef HAVE_SETLOCALE
  /* Set locale via LC_ALL.  */
  setlocale (LC_ALL, "");
#endif /* HAVE_SETLOCALE */

#ifdef ENABLE_NLS
  /* Set the text message domain.  */
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);
#endif

  init_messages ();
  while (1)
    {
      int option_character;

      option_character = getopt_long (argc, argv, short_options, long_options,
				      &getopt_long_index);

      /* getopt_long returns EOF when there are no more long options. */
      if (option_character == EOF)
        break;

      /* If this is a long option, then get the short version of it. */
      if (option_character == 0 && long_options[getopt_long_index].flag == 0)
        option_character = long_options[getopt_long_index].val;

      /* Case on the option that we have received. */
      switch (option_character)
        {
        case 0:
          break;

	case 'a':
	  all_matches_p = 1;
	  break;
	  
          /* User wants to add a directory. */
        case 'd':
          infopath_add (optarg);
          break;

          /* User is specifying a particular node. */
        case 'n':
          add_pointer_to_array (optarg, user_nodenames_index, user_nodenames,
                                user_nodenames_slots, 10);
          break;

          /* User is specifying a particular Info file. */
        case 'f':
          if (user_filename)
            free (user_filename);

          user_filename = xstrdup (optarg);
          break;

          /* Treat -h like --help. */
        case 'h':
          print_help_p = 1;
          break;

          /* User is specifying the name of a file to output to. */
        case 'o':
          if (user_output_filename)
            free (user_output_filename);
          user_output_filename = xstrdup (optarg);
          break;

         /* User has specified that she wants to find the "Options"
             or "Invocation" node for the program.  */
        case 'O':
          goto_invocation_p = 1;
          break;

	  /* User has specified that she wants the escape sequences
	     in man pages to be passed thru unaltered.  */
        case 'R':
          raw_escapes_p = 1;
          break;

          /* User is specifying that she wishes to dump the subnodes of
             the node that she is dumping. */
        case 's':
          dump_subnodes = 1;
          break;

          /* For compatibility with man, -w is --where.  */
        case 'w':
          print_where_p = 1;
          break;

#if defined(__MSDOS__) || defined(__MINGW32__)
	  /* User wants speech-friendly output.  */
	case 'b':
	  speech_friendly = 1;
	  break;
#endif /* __MSDOS__ || __MINGW32__ */

          /* User has specified a string to search all indices for. */
        case 'k':
          apropos_p = 1;
          free (apropos_search_string);
          apropos_search_string = xstrdup (optarg);
          break;

          /* User has specified a dribble file to receive keystrokes. */
        case DRIBBLE_OPTION:
          close_dribble_file ();
          open_dribble_file (optarg);
          break;

          /* User has specified an alternate input stream. */
        case RESTORE_OPTION:
          info_set_input_from_file (optarg);
          break;

          /* User has specified a string to search all indices for. */
        case IDXSRCH_OPTION:
          index_search_p = 1;
          free (index_search_string);
          index_search_string = xstrdup (optarg);
          break;

          /* User has specified a file to use as the init file. */
        case INITFLE_OPTION:
          init_file = optarg;
          break;

	case 'v':
	  {
            VARIABLE_ALIST *var;
	    char *p;
	    p = strchr (optarg, '=');
	    if (!p)
	      {
		info_error (_("malformed variable assignment: %s"), optarg);
		exit (EXIT_FAILURE);
	      }
	    *p++ = 0;

            if (!(var = variable_by_name (optarg)))
              {
                info_error (_("%s: no such variable"), optarg);
                exit (EXIT_FAILURE);
              }

	    if (!set_variable_to_value (var, p, SET_ON_COMMAND_LINE))
	      {
                info_error (_("value %s is not valid for variable %s"),
                            p, optarg);
		exit (EXIT_FAILURE);
	      }	
	  }
	  break;
	  
	case 'x':
	  set_debug_level (optarg);
	  break;
	  
        default:
          fprintf (stderr, _("Try --help for more information.\n"));
          exit (EXIT_FAILURE);
        }
    }

  /* If the output device is not a terminal, and no output filename has been
     specified, make user_output_filename be "-", so that the info is written
     to stdout, and turn on the dumping of subnodes. */
  if ((!isatty (fileno (stdout))) && (user_output_filename == NULL))
    {
      user_output_filename = xstrdup ("-");
      dump_subnodes = 1;
    }

  /* If the user specified --version, then show the version and exit. */
  if (print_version_p)
    {
      printf ("info (GNU %s) %s\n", PACKAGE, VERSION);
      puts ("");
      printf (_("Copyright (C) %s Free Software Foundation, Inc.\n\
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>\n\
This is free software: you are free to change and redistribute it.\n\
There is NO WARRANTY, to the extent permitted by law.\n"),
	      "2017");
      exit (EXIT_SUCCESS);
    }

  /* If the `--help' option was present, show the help and exit. */
  if (print_help_p)
    {
      info_short_help ();
      exit (EXIT_SUCCESS);
    }

  argc -= optind;
  argv += optind;
  
  /* If --file was not used and there is a slash in the first non-option
     argument (e.g. "info subdir/file.info"), do not search the dir files
     for a matching entry. */
  if (!user_filename && argv[0] && HAS_SLASH (argv[0]))
    {
      user_filename = xstrdup (argv[0]);
      argv++; /* Advance past first remaining argument. */
      argc--;
    }

  /* If the user specified a particular filename, add the path of that
     file to the contents of INFOPATH. */
  if (user_filename)
    add_file_directory_to_path (user_filename);

  /* Load custom key mappings and variable settings */
  initialize_terminal_and_keymaps (init_file);

  /* Add extra search directories to any already specified with
     --directory. */
  infopath_init ();

  /* If the user wants to search every known index for a given string,
     do that now, and report the results. */
  if (apropos_p)
    {
      REFERENCE **apropos_list;

      apropos_list = apropos_in_all_indices (apropos_search_string, 0);

      if (!apropos_list)
        info_error (_(APROPOS_NONE), apropos_search_string);
      else
        {
          register int i;
          REFERENCE *entry;

          for (i = 0; (entry = apropos_list[i]); i++)
            fprintf (stdout, "\"(%s)%s\" -- %s\n",
                entry->filename, entry->nodename, entry->label);
        }
      exit (0);
    }

  /* Initialize empty list of nodes to load. */
  add_pointer_to_array (0, ref_index, ref_list, ref_slots, 2);
  ref_index--;

  if (all_matches_p && !index_search_p)
    {
      /* --all */
      if (!user_filename && argv[0])
        {
          user_filename = xstrdup (argv[0]);
          argv++; argc--;
        }
      else if (!user_filename)
        {
          exit (1);
        }
      info_find_matching_files (user_filename);
      /* If only one match, don't start in a menu of matches. */
      if (ref_index == 1)
        all_matches_p = 0;

      /* --where */
      if (print_where_p)
        {
          int i;
          if (!ref_list)
            exit (1);

          for (i = 0; ref_list[i]; i++)
            printf ("%s\n", ref_list[i]->filename);
          exit (0);
        }
    }
  else
    {
      if (goto_invocation_p)
        {
          /* If they said "info --show-options foo bar baz",
             the last of the arguments is the program whose
             options they want to see.  */
          char **p = argv;
          if (*p)
            {
              while (p[1])
                p++;
              invocation_program_name = *p;
            }
        }

      get_initial_file (&argc, &argv, &error);

      /* If the user specified `--index-search=STRING --all', create
         and display the menu of results. */
      if (index_search_p && all_matches_p && initial_file)
        {
          FILE_BUFFER *initial_fb;
          initial_fb = info_find_file (initial_file);
          if (initial_fb)
            {
              NODE *node = create_virtual_index (initial_fb,
                                                 index_search_string);
              if (node)
                {
                  if (user_output_filename)
                    {
                      FILE *output_stream = 0;
                      if (strcmp (user_output_filename, "-") == 0)
                        output_stream = stdout;
                      else
                        output_stream = fopen (user_output_filename, "w");
                      if (output_stream)
                        {
                          write_node_to_stream (node, output_stream);
                        }
                      exit (0);
                    }
                  else
                    {
                      initialize_info_session ();
                      info_set_node_of_window (active_window, node);
                      info_read_and_dispatch ();
                      close_info_session ();
                      exit (0);
                    }
                }
            }
        }

      /* If the user specified `--index-search=STRING', 
         start the info session in the node corresponding
         to what they want. */
      else if (index_search_p && initial_file && !user_output_filename)
        {
          FILE_BUFFER *initial_fb;
          initial_fb = info_find_file (initial_file);
          if (initial_fb)
            {
              REFERENCE *result;
              int i, match_offset;

              result = next_index_match (initial_fb, index_search_string, 0, 1,
                                         &i, &match_offset);

              if (result)
                {
                  initialize_info_session ();
                  report_index_match (i, match_offset);
                  info_select_reference (active_window, result);
                  info_read_and_dispatch ();
                  close_info_session ();
                  exit (0);
                }
            }

          fprintf (stderr, _("no index entries found for '%s'\n"),
                   index_search_string);
          close_dribble_file ();
          exit (1);
        }

      /* Add nodes to start with (unless we fell back to the man page). */
      if (!ref_list[0] || strcmp (ref_list[0]->filename, 
                                  MANPAGE_FILE_BUFFER_NAME))
        {
          add_initial_nodes (argc, argv, &error);
        }

      /* --where */
      if (print_where_p)
        {
          if (initial_file)
            printf ("%s\n", initial_file);
          exit (0);
        }

    }

  /* --output */
  if (user_output_filename)
    {
      if (error)
        info_error ("%s", error);

      preprocess_nodes_p = 0;
      dump_nodes_to_file (ref_list, user_output_filename, dump_subnodes);
      exit (0);
    }

  if (ref_index == 0)
    {
      if (error)
        {
          info_error ("%s", error);
          exit (1);
        }
      exit (0);
    }
    
  info_session (ref_list, all_matches_p ? user_filename : 0, error);
  close_info_session ();
  exit (0);
}


/* Produce a scaled down description of the available options to Info. */
static void
info_short_help (void)
{
  printf (_("\
Usage: %s [OPTION]... [MENU-ITEM...]\n\
\n\
Read documentation in Info format.\n"), program_name);
  puts ("");

  puts (_("\
Frequently-used options:\n\
  -a, --all                    use all matching manuals\n\
  -k, --apropos=STRING         look up STRING in all indices of all manuals\n\
  -d, --directory=DIR          add DIR to INFOPATH\n\
  -f, --file=MANUAL            specify Info manual to visit"));

  puts (_("\
  -h, --help                   display this help and exit\n\
      --index-search=STRING    go to node pointed by index entry STRING\n\
  -n, --node=NODENAME          specify nodes in first visited Info file\n\
  -o, --output=FILE            output selected nodes to FILE"));

  puts (_("\
  -O, --show-options, --usage  go to command-line options node"));

#if defined(__MSDOS__) || defined(__MINGW32__)
  puts (_("\
  -b, --speech-friendly        be friendly to speech synthesizers"));
#endif

  puts (_("\
      --subnodes               recursively output menu items\n\
  -v, --variable VAR=VALUE     assign VALUE to Info variable VAR\n\
      --version                display version information and exit\n\
  -w, --where, --location      print physical location of Info file"));

  puts (_("\n\
The first non-option argument, if present, is the menu entry to start from;\n\
it is searched for in all 'dir' files along INFOPATH.\n\
If it is not present, info merges all 'dir' files and shows the result.\n\
Any remaining arguments are treated as the names of menu\n\
items relative to the initial node visited."));

  puts (_("\n\
For a summary of key bindings, type H within Info."));

  puts (_("\n\
Examples:\n\
  info                         show top-level dir menu\n\
  info info-stnd               show the manual for this Info program\n\
  info emacs                   start at emacs node from top-level dir\n\
  info emacs buffers           select buffers menu entry in emacs manual\n\
  info emacs -n Files          start at Files node within emacs manual\n\
  info '(emacs)Files'          alternative way to start at Files node\n\
  info --show-options emacs    start at node with emacs' command line options\n\
  info --subnodes -o out.txt emacs\n\
                               dump entire emacs manual to out.txt\n\
  info -f ./foo.info           show file ./foo.info, not searching dir"));

  puts ("");

  puts (_("\
Email bug reports to bug-texinfo@gnu.org,\n\
general questions and discussion to help-texinfo@gnu.org.\n\
Texinfo home page: http://www.gnu.org/software/texinfo/"));

  exit (EXIT_SUCCESS);
}


/* Initialize strings for gettext.  Because gettext doesn't handle N_ or
   _ within macro definitions, we put shared messages into variables and
   use them that way.  This also has the advantage that there's only one
   copy of the strings.  */

const char *msg_cant_find_node;
const char *msg_cant_file_node;
const char *msg_cant_find_window;
const char *msg_cant_find_point;
const char *msg_cant_kill_last;
const char *msg_no_menu_node;
const char *msg_no_foot_node;
const char *msg_no_xref_node;
const char *msg_no_pointer;
const char *msg_unknown_command;
const char *msg_term_too_dumb;
const char *msg_at_node_bottom;
const char *msg_at_node_top;
const char *msg_one_window;
const char *msg_win_too_small;
const char *msg_cant_make_help;

static void
init_messages (void)
{
  msg_cant_find_node   = _("Cannot find node '%s'");
  msg_cant_file_node   = _("Cannot find node '(%s)%s'");
  msg_cant_find_window = _("Cannot find a window!");
  msg_cant_find_point  = _("Point doesn't appear within this window's node!");
  msg_cant_kill_last   = _("Cannot delete the last window");
  msg_no_menu_node     = _("No menu in this node");
  msg_no_foot_node     = _("No footnotes in this node");
  msg_no_xref_node     = _("No cross references in this node");
  msg_no_pointer       = _("No '%s' pointer for this node");
  msg_unknown_command  = _("Unknown Info command '%c'; try '?' for help");
  msg_term_too_dumb    = _("Terminal type '%s' is not smart enough to run Info");
  msg_at_node_bottom   = _("You are already at the last page of this node");
  msg_at_node_top      = _("You are already at the first page of this node");
  msg_one_window       = _("Only one window");
  msg_win_too_small    = _("Resulting window would be too small");
  msg_cant_make_help   = _("Not enough room for a help window, please delete a window");
}
