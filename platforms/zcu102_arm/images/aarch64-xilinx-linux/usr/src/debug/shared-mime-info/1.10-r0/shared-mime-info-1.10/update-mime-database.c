#include <config.h>

#define N_(x) x
#define _(x) (x)

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <glib/gstdio.h>
#include <errno.h>
#include <dirent.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#define XML_NS XML_XML_NAMESPACE
#define XMLNS_NS "http://www.w3.org/2000/xmlns/"
#define FREE_NS (xmlChar *)"http://www.freedesktop.org/standards/shared-mime-info"

#define COPYING								\
	     N_("Copyright (C) 2003 Thomas Leonard.\n"			\
		"update-mime-database comes with ABSOLUTELY NO WARRANTY,\n" \
		"to the extent permitted by law.\n"			\
		"You may redistribute copies of update-mime-database\n"	\
		"under the terms of the GNU General Public License.\n"	\
		"For more information about these matters, "		\
		"see the file named COPYING.\n")

#define MIME_ERROR g_quark_from_static_string("mime-error-quark")

#define NOGLOBS "__NOGLOBS__"
#define NOMAGIC "__NOMAGIC__"

#ifndef PATH_SEPARATOR
# ifdef _WIN32
#  define PATH_SEPARATOR ";"
# else
#  define PATH_SEPARATOR ":"
# endif
#endif

/* This is the list of directories to scan when finding old type files to
 * delete. It is also used to warn about invalid MIME types.
 */
const char *media_types[] = {
	"all",
	"uri",
	"print",
	"text",
	"application",
	"image",
	"audio",
	"inode",
	"video",
	"message",
	"model",
	"multipart",
	"x-content",
	"x-epoc",
	"x-scheme-handler",
	"font",
};

/* Represents a MIME type */
typedef struct _Type Type;

/* A parsed <magic> element */
typedef struct _Magic Magic;

/* A parsed <match> element */
typedef struct _Match Match;

/* A parsed <treemagic> element */
typedef struct _TreeMagic TreeMagic;

/* A parsed <treematch> element */
typedef struct _TreeMatch TreeMatch;

/* A parsed <glob> element */
typedef struct _Glob Glob;

struct _Type {
	char *media;
	char *subtype;

	/* Contains xmlNodes for elements that are being copied to the output.
	 * That is, <comment>, <sub-class-of> and <alias> nodes, and anything
	 * with an unknown namespace.
	 */
	xmlDoc	*output;
};

struct _Glob {
	int weight;
	char *pattern;
	Type *type;
	gboolean noglob;
	gboolean case_sensitive;
};

struct _Magic {
	int priority;
	Type *type;
	GList *matches;
	gboolean nomagic;
};

struct _Match {
	long range_start;
	int range_length;
	char word_size;
	int data_length;
	char *data;
	char *mask;
	GList *matches;
};

struct _TreeMagic {
	int priority;
	Type *type;
	GList *matches;
};

struct _TreeMatch {
	char *path;
	gboolean match_case;
	gboolean executable;
	gboolean non_empty;
	gint type;
	char *mimetype;

	GList *matches;
};

/* Maps MIME type names to Types */
static GHashTable *types = NULL;

/* Maps "namespaceURI localName" strings to Types */
static GHashTable *namespace_hash = NULL;

/* Maps glob patterns to Types */
static GHashTable *globs_hash = NULL;

/* 'magic' nodes */
static GPtrArray *magic_array = NULL;

/* 'treemagic' nodes */
static GPtrArray *tree_magic_array = NULL;

/* Maps MIME type names to superclass names */
static GHashTable *subclass_hash = NULL;

/* Maps aliases to Types */
static GHashTable *alias_hash = NULL;

/* Maps MIME type names to icon names */
static GHashTable *icon_hash = NULL;

/* Maps MIME type names to icon names */
static GHashTable *generic_icon_hash = NULL;

/* Lists enabled log levels */
static GLogLevelFlags enabled_log_levels = G_LOG_LEVEL_ERROR | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING;

/* Static prototypes */
static Magic *magic_new(xmlNode *node, Type *type, GError **error);
static Match *match_new(void);

static TreeMagic *tree_magic_new(xmlNode *node, Type *type, GError **error);

static void g_log_handler (const gchar   *log_domain,
			   GLogLevelFlags log_level,
			   const gchar   *message,
			   gpointer       unused_data)
{
    if (log_level & enabled_log_levels) {
        g_printf("%s\n", message);
    }
}

static void
fatal_gerror (GError *error) G_GNUC_NORETURN;

static void
fatal_gerror (GError *error)
{
	g_assert(error != NULL);
	g_printerr("%s\n", error->message);
	g_error_free(error);
	exit (EXIT_FAILURE);
}

static void usage(const char *name)
{
	g_fprintf(stderr, _("Usage: %s [-hvVn] MIME-DIR\n"), name);
}

static void free_type(gpointer data)
{
	Type *type = (Type *) data;

	g_free(type->media);
	g_free(type->subtype);

	xmlFreeDoc(type->output);

	g_free(type);
}

/* Ugly workaround to shut up gcc4 warnings about signedness issues
 * (xmlChar is typedef'ed to unsigned char)
 */
static char *my_xmlGetNsProp (xmlNodePtr node, 
			      const char *name,
			      const xmlChar *namespace)
{
	return (char *)xmlGetNsProp (node, (xmlChar *)name, namespace);
}

/* If we've seen this type before, return the existing object.
 * Otherwise, create a new one. Checks that the name looks sensible;
 * if not, sets error and returns NULL.
 * Also warns about unknown media types, but does not set error.
 */
static Type *get_type(const char *name, GError **error)
{
	xmlNode *root;
	xmlNs *ns;
	const char *slash;
	Type *type;
	int i;

	slash = strchr(name, '/');
	if (!slash || strchr(slash + 1, '/'))
	{
		g_set_error(error, MIME_ERROR, 0,
				_("Invalid MIME-type '%s'"), name);
		return NULL;
	}

	type = g_hash_table_lookup(types, name);
	if (type)
		return type;

	type = g_new(Type, 1);
	type->media = g_strndup(name, slash - name);
	type->subtype = g_strdup(slash + 1);
	g_hash_table_insert(types, g_strdup(name), type);

	type->output = xmlNewDoc((xmlChar *)"1.0");
	root = xmlNewDocNode(type->output, NULL, (xmlChar *)"mime-type", NULL);
	ns = xmlNewNs(root, FREE_NS, NULL);
	xmlSetNs(root, ns);
	xmlDocSetRootElement(type->output, root);
	xmlSetNsProp(root, NULL, (xmlChar *)"type", (xmlChar *)name);
	xmlAddChild(root, xmlNewDocComment(type->output,
		(xmlChar *)"Created automatically by update-mime-database. DO NOT EDIT!"));

	for (i = 0; i < G_N_ELEMENTS(media_types); i++)
	{
		if (strcmp(media_types[i], type->media) == 0)
			return type;
	}

	g_message("Unknown media type in type '%s'", name);

	return type;
}

/* Test that this node has the expected name and namespace */
static gboolean match_node(xmlNode *node,
			   const char *namespaceURI,
			   const char *localName)
{
	if (namespaceURI)
		return node->ns &&
			strcmp((char *)node->ns->href, namespaceURI) == 0 &&
  		        strcmp((char *)node->name, localName) == 0;
	else
		return strcmp((char *)node->name, localName) == 0 && !node->ns;
}

static int get_int_attribute(xmlNode *node, const char *name)
{
	char *prio_string;
	int p;

	prio_string = my_xmlGetNsProp(node, name, NULL);
	if (prio_string)
	{
		char *end;

		p = strtol(prio_string, &end, 10);
		if (*prio_string == '\0' || *end != '\0')
			p = -1;
		xmlFree(prio_string);
		if (p < 0 || p > 100)
			return -1;
		return p;
	}
	else
		return 50;
}

/* Return the priority of a <magic> node.
 * Returns 50 if no priority is given, or -1 if a priority is given but
 * is invalid.
 */
static int get_priority(xmlNode *node)
{
       return get_int_attribute (node, "priority");
}

/* Return the weight a <glob> node.
 * Returns 50 if no weight is given, or -1 if a weight is given but
 * is invalid.
 */
static int get_weight(xmlNode *node)
{
       return get_int_attribute (node, "weight");
}

/* Return the value of a false/true attribute, which defaults to false.
 * Returns 0 or 1.
 */
static gboolean get_boolean_attribute(xmlNode *node, const char* name)
{
	char *attr;
	attr = my_xmlGetNsProp(node, name, NULL);
	if (attr)
	{
	    if (strcmp (attr, "true") == 0) 
	    {
		return TRUE;
	    }
	    xmlFree(attr);
	}
	return FALSE;
}

/* Process a <root-XML> element by adding a rule to namespace_hash */
static void add_namespace(Type *type, const char *namespaceURI,
			  const char *localName, GError **error)
{
	g_return_if_fail(type != NULL);

	if (!namespaceURI)
	{
		g_set_error(error, MIME_ERROR, 0,
			_("Missing 'namespaceURI' attribute'"));
		return;
	}

	if (!localName)
	{
		g_set_error(error, MIME_ERROR, 0,
			_("Missing 'localName' attribute'"));
		return;
	}

	if (!*namespaceURI && !*localName)
	{
		g_set_error(error, MIME_ERROR, 0,
			_("namespaceURI and localName attributes can't "
			  "both be empty"));
		return;
	}

	if (strpbrk(namespaceURI, " \n") || strpbrk(localName, " \n"))
	{
		g_set_error(error, MIME_ERROR, 0,
			_("namespaceURI and localName cannot contain "
			  "spaces or newlines"));
		return;
	}

	g_hash_table_insert(namespace_hash,
			g_strconcat(namespaceURI, " ", localName, NULL),
			type);
}

/* 'field' was found in the definition of 'type' and has the freedesktop.org
 * namespace. If it's a known field, process it and return TRUE, else
 * return FALSE to add it to the output XML document.
 * On error, returns FALSE and sets 'error'.
 */
static gboolean process_freedesktop_node(Type *type, xmlNode *field,
					 GError **error)
{
	gboolean copy_to_xml;

	copy_to_xml = FALSE;
	if (strcmp((char *)field->name, "glob") == 0)
	{
		gchar *pattern;	
		gint weight;
		gboolean case_sensitive;

		weight = get_weight(field);
		case_sensitive = get_boolean_attribute(field, "case-sensitive");

		if (weight == -1)
		{
			g_set_error(error, MIME_ERROR, 0,
				    _("Bad weight (%d) in <glob> element"), weight);
		}
		pattern = my_xmlGetNsProp(field, "pattern", NULL);

		if (pattern && *pattern)
		{
			Glob *glob;
			char *pat = case_sensitive ? g_strdup (pattern) : g_ascii_strdown (pattern, -1);
			GList *list = g_hash_table_lookup (globs_hash, pat);
			
			glob = g_new0 (Glob, 1);
			glob->pattern = pat;
			glob->type = type;
			glob->weight = weight;
			glob->case_sensitive = case_sensitive;
			list = g_list_append (list, glob);
			g_hash_table_insert(globs_hash, g_strdup (glob->pattern), list);
			xmlFree(pattern);
			copy_to_xml = TRUE;
		}
		else
		{
			if (pattern)
				xmlFree(pattern);
			g_set_error(error, MIME_ERROR, 0,
				_("Missing 'pattern' attribute in <glob> "
				  "element"));
		}
	}
	else if (strcmp((char *)field->name, "glob-deleteall") == 0)
	{
		Glob *glob;
		GList *list = g_hash_table_lookup (globs_hash, NOGLOBS);

		glob = g_new0 (Glob, 1);
		glob->pattern = g_strdup (NOGLOBS);
		glob->type = type;
		glob->weight = 0;
		glob->noglob = TRUE;
		glob->case_sensitive = FALSE;
		list = g_list_append (list, glob);
		g_hash_table_insert(globs_hash, g_strdup (glob->pattern), list);
		copy_to_xml = TRUE;
	}
	else if (strcmp((char *)field->name, "magic") == 0)
	{
		Magic *magic;

		magic = magic_new(field, type, error);

		if (!*error)
		{
			g_return_val_if_fail(magic != NULL, FALSE);
			g_ptr_array_add(magic_array, magic);
		}
		else
			g_return_val_if_fail(magic == NULL, FALSE);
	}
	else if (strcmp((char *)field->name, "magic-deleteall") == 0)
	{
		Magic *magic;
		Match *match;

		magic = g_new0(Magic, 1);
		magic->priority = 0;
		magic->type = type;
		magic->nomagic = TRUE;
		match = match_new ();
		match->data = g_strdup (NOMAGIC);
		match->data_length = strlen (NOMAGIC);
		magic->matches = g_list_prepend (NULL, match);

		g_ptr_array_add(magic_array, magic);
	}
	else if (strcmp((char *)field->name, "treemagic") == 0)
	{
		TreeMagic *magic;

		magic = tree_magic_new(field, type, error);

		if (!*error)
		{
			g_return_val_if_fail(magic != NULL, FALSE);
			g_ptr_array_add(tree_magic_array, magic);
		}
		else
			g_return_val_if_fail(magic == NULL, FALSE);
	}
	else if (strcmp((char *)field->name, "comment") == 0 ||
		 strcmp((char *)field->name, "acronym") == 0 ||
		 strcmp((char *)field->name, "expanded-acronym") == 0)
		copy_to_xml = TRUE;
	else if (strcmp((char *)field->name, "alias") == 0 ||
		 strcmp((char *)field->name, "sub-class-of") == 0)
	{
		char *other_type;
		gboolean valid;
		GSList *list, *nlist;

		other_type = my_xmlGetNsProp(field, "type", NULL);
		valid = other_type && strchr(other_type, '/');
		if (valid)
		{
			char *typename;

			typename = g_strdup_printf("%s/%s", 
						   type->media,
						   type->subtype);
			
			if (strcmp((char *)field->name, "alias") == 0)
				g_hash_table_insert(alias_hash,
						    g_strdup(other_type), type);
				
			else
			{
				list = g_hash_table_lookup(subclass_hash, typename);
				nlist = g_slist_append (list, g_strdup(other_type));
				if (list == NULL)
					g_hash_table_insert(subclass_hash, 
							    g_strdup(typename), nlist);
			}
			g_free(typename);
			xmlFree(other_type);

			copy_to_xml = TRUE; /* Copy through */
		}
		else
		{
			xmlFree(other_type);
			g_set_error(error, MIME_ERROR, 0,
				    _("Incorrect or missing 'type' attribute "
				      "in <%s>"), field->name);
		}
	}
	else if (strcmp((char *)field->name, "root-XML") == 0)
	{
		char *namespaceURI, *localName;

		namespaceURI = my_xmlGetNsProp(field, "namespaceURI", NULL);
		localName = my_xmlGetNsProp(field, "localName", NULL);

		add_namespace(type, namespaceURI, localName, error);

		if (namespaceURI)
			xmlFree(namespaceURI);
		if (localName)
			xmlFree(localName);
	}
	else if (strcmp((char *)field->name, "generic-icon") == 0 ||
		 strcmp((char *)field->name, "icon") == 0) 
	{
		char *icon;
		char *typename;

		icon = my_xmlGetNsProp(field, "name", NULL);

		if (icon) 
		{
			typename = g_strdup_printf("%s/%s",
						   type->media,
						   type->subtype);

			if (strcmp((char *)field->name, "icon") == 0)
				g_hash_table_insert(icon_hash,
						    typename, g_strdup (icon));
			else
				g_hash_table_insert(generic_icon_hash,
						    typename, g_strdup (icon));

			xmlFree (icon);

			copy_to_xml = TRUE; /* Copy through */
		}
	}

	if (*error)
		return FALSE;
	return !copy_to_xml;
}

/* Checks to see if 'node' has the given value for xml:lang.
 * If 'lang' is NULL, checks that 'node' doesn't have an xml:lang.
 */
static gboolean has_lang(xmlNode *node, const char *lang)
{
	char *lang2;

	lang2 = my_xmlGetNsProp(node, "lang", XML_NS);
	if (!lang2)
		return !lang;

	if (lang && strcmp(lang, lang2) == 0)
	{
		xmlFree(lang2);
		return TRUE;
	}
	xmlFree(lang2);
	return FALSE;
}

/* We're about to add 'new' to the list of fields to be output for the
 * type. Remove any existing nodes which it replaces.
 */
static void remove_old(Type *type, xmlNode *new)
{
	xmlNode *field, *fields;
	char *lang;

	if (new->ns == NULL || xmlStrcmp(new->ns->href, FREE_NS) != 0)
		return;	/* No idea what we're doing -- leave it in! */

	if (strcmp((char *)new->name, "comment") != 0)
		return;

	lang = my_xmlGetNsProp(new, "lang", XML_NS);

	fields = xmlDocGetRootElement(type->output);
	for (field = fields->xmlChildrenNode; field; field = field->next)
	{
		if (match_node(field, (char *)FREE_NS, "comment") &&
		    has_lang(field, lang))
		{
			xmlUnlinkNode(field);
			xmlFreeNode(field);
			break;
		}
	}

	xmlFree(lang);
}

/* 'node' is a <mime-type> node from a source file, whose type is 'type'.
 * Process all the child elements, setting 'error' if anything goes wrong.
 */
static void load_type(Type *type, xmlNode *node, GError **error)
{
	xmlNode *field;

	g_return_if_fail(type != NULL);
	g_return_if_fail(node != NULL);
	g_return_if_fail(error != NULL);

	for (field = node->xmlChildrenNode; field; field = field->next)
	{
		xmlNode *copy;

		if (field->type != XML_ELEMENT_NODE)
			continue;

		if (field->ns && xmlStrcmp(field->ns->href, FREE_NS) == 0)
		{
			if (process_freedesktop_node(type, field, error))
			{
				g_return_if_fail(*error == NULL);
				continue;
			}
		}

		if (*error)
			return;

		copy = xmlDocCopyNode(field, type->output, 1);
		
		/* Ugly hack to stop the xmlns= attributes appearing on
		 * every node...
		 */
		if (copy->ns && copy->ns->prefix == NULL &&
			xmlStrcmp(copy->ns->href, FREE_NS) == 0)
		{
			if (copy->nsDef)
			{
				/* Still used somewhere... */
				/* xmlFreeNsList(copy->nsDef); */
				/* (this leaks) */
				copy->nsDef = NULL;
			}
		}

		remove_old(type, field);

		xmlAddChild(xmlDocGetRootElement(type->output), copy);
	}
}

/* Parse 'filename' as an XML file and add all the information to the
 * database. If called more than once, information read in later calls
 * overrides information read previously.
 */
static void load_source_file(const char *filename)
{
	xmlDoc *doc;
	xmlNode *root, *node;

	doc = xmlParseFile(filename);
	if (!doc)
	{
		g_warning(_("Failed to parse '%s'"), filename);
		return;
	}

	g_message("Parsing source file %s...", filename);

	root = xmlDocGetRootElement(doc);

	if (root->ns == NULL || xmlStrcmp(root->ns->href, FREE_NS) != 0)
	{
		g_warning("Wrong namespace on document element in '%s' (should be %s)", filename, FREE_NS);
		goto out;
	}

	if (strcmp((char *)root->name, "mime-info") != 0)
	{
		g_warning("Root element <%s> is not <mime-info> (in '%s')", root->name, filename);
		goto out;
	}

	for (node = root->xmlChildrenNode; node; node = node->next)
	{
		Type *type = NULL;
		char *type_name = NULL;
		GError *error = NULL;

		if (node->type != XML_ELEMENT_NODE)
			continue;

		if (!match_node(node, (char *)FREE_NS, "mime-type"))
			g_set_error(&error, MIME_ERROR, 0,
				_("Excepted <mime-type>, but got wrong name "
				  "or namespace"));

		if (!error)
		{
			type_name = my_xmlGetNsProp(node, "type", NULL);

			if (!type_name)
				g_set_error(&error, MIME_ERROR, 0,
					_("<mime-type> element has no 'type' "
					  "attribute"));
		}

		if (type_name)
		{
			type = get_type(type_name, &error);
			xmlFree(type_name);
		}

		if (!error)
		{
			g_return_if_fail(type != NULL);
			load_type(type, node, &error);
		}
		else
			g_return_if_fail(type == NULL);

		if (error)
		{
			g_warning("Error in type '%s/%s' (in %s): %s.",
				  type ? type->media : _("unknown"),
				  type ? type->subtype : _("unknown"),
				  filename, error->message);
			g_error_free(error);
		}
	}
out:
	xmlFreeDoc(doc);
}

/* Used as the sort function for sorting GPtrArrays */
static gint strcmp2(gconstpointer a, gconstpointer b)
{
	const char *aa = *(char **) a;
	const char *bb = *(char **) b;

	return strcmp(aa, bb);
}

/* 'path' should be a 'packages' directory. Loads the information from
 * every file in the directory.
 */
static void scan_source_dir(const char *path)
{
	DIR *dir;
	struct dirent *ent;
	char *filename;
	GPtrArray *files;
	int i;
	gboolean have_override = FALSE;

	dir = opendir(path);
	if (!dir)
	{
		perror("scan_source_dir");
		exit(EXIT_FAILURE);
	}

	files = g_ptr_array_new();
	while ((ent = readdir(dir)))
	{
		int l;
		l = strlen(ent->d_name);
		if (l < 4 || strcmp(ent->d_name + l - 4, ".xml") != 0)
			continue;
		if (strcmp(ent->d_name, "Override.xml") == 0)
		{
			have_override = TRUE;
			continue;
		}
		g_ptr_array_add(files, g_strdup(ent->d_name));
	}
	closedir(dir);

	g_ptr_array_sort(files, strcmp2);

	if (have_override)
		g_ptr_array_add(files, g_strdup("Override.xml"));

	for (i = 0; i < files->len; i++)
	{
		gchar *leaf = (gchar *) files->pdata[i];

		filename = g_strconcat(path, "/", leaf, NULL);
		load_source_file(filename);
		g_free(filename);
	}

	for (i = 0; i < files->len; i++)
		g_free(files->pdata[i]);
	g_ptr_array_free(files, TRUE);
}

static gboolean save_xml_file(xmlDocPtr doc, const gchar *filename, GError **error)
{
#if LIBXML_VERSION > 20400
	if (xmlSaveFormatFileEnc(filename, doc, "utf-8", 1) < 0)
	{
		g_set_error(error, G_FILE_ERROR, G_FILE_ERROR_FAILED,
			    "Failed to write XML file; For permission problems, try rerunning as root");
		return FALSE;
	}
#else
	FILE *out;
	
	out = fopen_gerror(filename, error);
	if (!out)
		return FALSE;

	xmlDocDump(out, doc);  /* Some versions return void */

	if (!fclose_gerror(out, error))
		return FALSE;
#endif

	return TRUE;
}

/* Write out globs for one pattern to the 'globs' file */
static void write_out_glob(GList *globs, FILE *stream)
{
	GList *list;
	Glob *glob;
	Type *type;

	for (list = globs; list; list = list->next) {
		glob = (Glob *)list->data;
		type = glob->type;
		if (strchr(glob->pattern, '\n'))
			g_warning("Glob patterns can't contain literal newlines "
				  "(%s in type %s/%s)", glob->pattern,
				  type->media, type->subtype);
		else
			g_fprintf(stream, "%s/%s:%s\n",
				type->media, type->subtype, glob->pattern);
	}
}

/* Write out globs and weights for one pattern to the 'globs2' file */
static void write_out_glob2(GList *globs, FILE *stream)
{
	GList *list;
	Glob *glob;
	Type *type;
	gboolean need_flags;

	for (list = globs ; list; list = list->next) {
		glob = (Glob *)list->data;
		type = glob->type;
		if (strchr(glob->pattern, '\n'))
			g_warning("Glob patterns can't contain literal newlines "
				  "(%s in type %s/%s)", glob->pattern,
				  type->media, type->subtype);
		else
		{
			need_flags = FALSE;
			if (glob->case_sensitive)
				need_flags = TRUE;

			if (need_flags) {
				g_fprintf(stream, "%d:%s/%s:%s%s\n",
						  glob->weight, type->media, type->subtype, glob->pattern,
						  glob->case_sensitive ? ":cs" : "");
			}

			/* Always write the line without the flags, for older parsers */
			g_fprintf(stream, "%d:%s/%s:%s\n",
					  glob->weight, type->media, type->subtype, glob->pattern);
		}
	}
}

static void collect_glob2(gpointer key, gpointer value, gpointer data)
{
	GList **listp = data;

	*listp = g_list_concat (*listp, g_list_copy ((GList *)value));
}

static int compare_glob_by_weight (gpointer a, gpointer b)
{
	Glob *ag = (Glob *)a;
	Glob *bg = (Glob *)b;

	if (ag->noglob || bg->noglob)
		return bg->noglob - ag->noglob;

	return bg->weight - ag->weight;
}

static void
set_error_from_errno (GError **error)
{
	int errsv = errno;
	g_set_error_literal(error, G_FILE_ERROR, g_file_error_from_errno(errsv),
			    g_strerror(errsv));
}

#ifdef HAVE_FDATASYNC
static gboolean
sync_enabled(void)
{
	const char *env;

	env = g_getenv("PKGSYSTEM_ENABLE_FSYNC");
	if (!env)
		return TRUE;
	return atoi(env);
}
#endif

static int
sync_file(const gchar *pathname, GError **error)
{
#ifdef HAVE_FDATASYNC
	int fd;

	if (!sync_enabled())
		return 0;

	fd = open(pathname, O_RDWR);
	if (fd == -1)
	{
		set_error_from_errno(error);
		return -1;
	}
	if (fdatasync(fd) == -1)
	{
		set_error_from_errno(error);
		return -1;
	}
	if (close(fd) == -1)
	{
		set_error_from_errno(error);
		return -1;
	}
#endif

	return 0;
}

/* Renames pathname by removing the .new extension */
static gboolean atomic_update(const gchar *pathname, GError **error)
{
	gboolean ret = FALSE;
	gchar *new_name = NULL;
	int len;

	len = strlen(pathname);

	g_return_val_if_fail(strcmp(pathname + len - 4, ".new") == 0, FALSE);

	new_name = g_strndup(pathname, len - 4);

	if (sync_file(pathname, error) == -1)
		goto out;

#ifdef _WIN32
	/* we need to remove the old file first! */
	remove(new_name);
#endif
	if (rename(pathname, new_name) == -1)
	{
		int errsv = errno;
		g_set_error(error, G_FILE_ERROR, g_file_error_from_errno(errsv),
			    "Failed to rename %s as %s: %s", pathname, new_name,
			    g_strerror(errsv));
		goto out;
	}

	ret = TRUE;
out:
	g_free(new_name);
	return ret;
}

/* Write out an XML file for one type */
static void write_out_type(gpointer key, gpointer value, gpointer data)
{
	Type *type = (Type *) value;
	const char *mime_dir = (char *) data;
	char *media, *filename;
	GError *local_error = NULL;
	char *lower;

	lower = g_ascii_strdown(type->media, -1);
	media = g_strconcat(mime_dir, "/", lower, NULL);
	g_free(lower);
#ifdef _WIN32
	mkdir(media);
#else
	mkdir(media, 0755);
#endif

	lower = g_ascii_strdown(type->subtype, -1);
	filename = g_strconcat(media, "/", lower, ".xml.new", NULL);
	g_free(lower);
	g_free(media);
	media = NULL;

	if (!save_xml_file(type->output, filename, &local_error))
		fatal_gerror(local_error);

	if (!atomic_update(filename, &local_error))
		fatal_gerror(local_error);

	g_free(filename);
}

/* Comparison function to get the magic rules in priority order */
static gint cmp_magic(gconstpointer a, gconstpointer b)
{
	Magic *aa = *(Magic **) a;
	Magic *bb = *(Magic **) b;
	int retval;

	/* Sort nomagic items at start */
	if (aa->nomagic || bb->nomagic)
		return bb->nomagic - aa->nomagic;

	if (aa->priority > bb->priority)
		return -1;
	else if (aa->priority < bb->priority)
		return 1;

	retval = strcmp(aa->type->media, bb->type->media);
	if (!retval)
		retval = strcmp(aa->type->subtype, bb->type->subtype);

	return retval;
}

/* Comparison function to get the tree magic rules in priority order */
static gint cmp_tree_magic(gconstpointer a, gconstpointer b)
{
	TreeMagic *aa = *(TreeMagic **) a;
	TreeMagic *bb = *(TreeMagic **) b;
	int retval;

	if (aa->priority > bb->priority)
		return -1;
	else if (aa->priority < bb->priority)
		return 1;

	retval = strcmp(aa->type->media, bb->type->media);
	if (!retval)
		retval = strcmp(aa->type->subtype, bb->type->subtype);

	return retval;
}

/* Write out 'n' as a two-byte big-endian number to 'stream' */
static void write16(FILE *stream, guint32 n)
{
	guint16 big = GUINT16_TO_BE(n);

	g_return_if_fail(n <= 0xffff);

	fwrite(&big, sizeof(big), 1, stream);
}

/* Single hex char to int; -1 if not a hex char.
 * From file(1).
 */
static int hextoint(int c)
{
	if (!isascii((unsigned char) c))
		return -1;
	if (isdigit((unsigned char) c))
		return c - '0';
	if ((c >= 'a')&&(c <= 'f'))
		return c + 10 - 'a';
	if (( c>= 'A')&&(c <= 'F'))
		return c + 10 - 'A';
	return -1;
}

/*
 * Convert a string containing C character escapes.  Stop at an unescaped
 * space or tab.
 * Copy the converted version to "p", returning its length in *slen.
 * Return updated scan pointer as function result.
 * Stolen from file(1) and heavily modified.
 */
static void getstr(const char *s, GString *out)
{
	int	c;
	int	val;

	while ((c = *s++) != '\0') {
		if(c == '\\') {
			switch(c = *s++) {

			case '\0':
				return;

			default:
				g_string_append_c(out, (char) c);
				break;

			case 'n':
				g_string_append_c(out, '\n');
				break;

			case 'r':
				g_string_append_c(out, '\r');
				break;

			case 'b':
				g_string_append_c(out, '\b');
				break;

			case 't':
				g_string_append_c(out, '\t');
				break;

			case 'f':
				g_string_append_c(out, '\f');
				break;

			case 'v':
				g_string_append_c(out, '\v');
				break;

			/* \ and up to 3 octal digits */
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
				val = c - '0';
				c = *s++;  /* try for 2 */
				if(c >= '0' && c <= '7') {
					val = (val<<3) | (c - '0');
					c = *s++;  /* try for 3 */
					if(c >= '0' && c <= '7')
						val = (val<<3) | (c-'0');
					else
						--s;
				}
				else
					--s;
				g_string_append_c(out, (char)val);
				break;

			/* \x and up to 2 hex digits */
			case 'x':
				val = 'x';	/* Default if no digits */
				c = hextoint(*s++);	/* Get next char */
				if (c >= 0) {
					val = c;
					c = hextoint(*s++);
					if (c >= 0)
						val = (val << 4) + c;
					else
						--s;
				} else
					--s;
				g_string_append_c(out, (char)val);
				break;
			}
		} else
			g_string_append_c(out, (char)c);
	}
}

/* Parse the value and mask attributes of a <match> element with a
 * numerical type (anything except "string").
 */
static void parse_int_value(int bytes, const char *in, const char *in_mask,
			    GString *parsed_value, char **parsed_mask,
			    gboolean big_endian, GError **error)
{
	char *end;
	char *out_mask = NULL;
	unsigned long value;
	int b;

	value = strtoul(in, &end, 0);
	if (errno == ERANGE) {
		g_set_error(error, MIME_ERROR, 0,
			    "Number out-of-range (%s should fit in %d bytes)",
			    in, bytes);
		return;
	}

	if (*end != '\0')
	{
		g_set_error(error, MIME_ERROR, 0, "Value is not a number");
		return;
	}

	for (b = 0; b < bytes; b++)
	{
		int shift = (big_endian ? (bytes - b - 1) : b) * 8;
		g_string_append_c(parsed_value, (value >> shift) & 0xff);
	}

	if ((bytes == 1 && (value & ~0xff)) ||
	    (bytes == 2 && (value & ~0xffff)))
	{
		g_set_error(error, MIME_ERROR, 0,
			    "Number out-of-range (%lx should fit in %d bytes)",
			    value, bytes);
		return;
	}

	if (in_mask)
	{
		int b;
		unsigned long mask;
		
		mask = strtoul(in_mask, &end, 0);
		if (errno == ERANGE) {
			g_set_error(error, MIME_ERROR, 0,
				    "Mask out-of-range (%s should fit in %d bytes)",
				    in_mask, bytes);
			return;
		}


		if (*end != '\0')
		{
			g_set_error(error, MIME_ERROR, 0,
				    "Mask is not a number");
			return;
		}

		out_mask = g_new(char, bytes);
		for (b = 0; b < bytes; b++)
		{
			int shift = (big_endian ? (bytes - b - 1) : b) * 8;
			out_mask[b] = (mask >> shift) & 0xff;
		}
	}

	*parsed_mask = out_mask;
}

/* 'len' is the length of the value. The mask created will be the same
 * length.
 */
static char *parse_string_mask(const char *mask, int len, GError **error)
{
	int i;
	char *parsed_mask = NULL;

	g_return_val_if_fail(mask != NULL, NULL);
	g_return_val_if_fail(len > 0, NULL);

	if (mask[0] != '0' || mask[1] != 'x')
	{
		g_set_error(error, MIME_ERROR, 0,
			"String masks must be in base 16 (starting with 0x)");
		goto err;
	}
	mask += 2;

	parsed_mask = g_new0(char, len);

	i = 0; /* Nybble to write to next */
	while (mask[i])
	{
		int c;

		c = hextoint(mask[i]);
		if (c == -1)
		{
			g_set_error(error, MIME_ERROR, 0,
				"'%c' is not a valid hex digit", mask[i]);
			goto err;
		}

		if (i >= len * 2)
		{
			g_set_error(error, MIME_ERROR, 0,
				"Mask is longer than value");
			goto err;
		}
		
		if (i & 1)
			parsed_mask[i >> 1] |= c;
		else
			parsed_mask[i >> 1] |= c << 4;

		i++;
	}

	return parsed_mask;
err:
	g_return_val_if_fail(error == NULL || *error != NULL, NULL);
	g_free(parsed_mask);
	return NULL;
}

/* Parse the value and mask attributes for a <match> element */
static void parse_value(const char *type, const char *in, const char *in_mask,
			GString *parsed_value, char **parsed_mask,
			GError **error)
{
	*parsed_mask = NULL;

	if (in == NULL || !in[0])
	{
		g_set_error(error, MIME_ERROR, 0, "No value specified");
		return;
	}

	if (strstr(type, "16"))
		parse_int_value(2, in, in_mask, parsed_value, parsed_mask,
				type[0] != 'l', error);
	else if (strstr(type, "32"))
		parse_int_value(4, in, in_mask, parsed_value, parsed_mask,
				type[0] != 'l', error);
	else if (strcmp(type, "byte") == 0)
		parse_int_value(1, in, in_mask, parsed_value, parsed_mask,
				FALSE, error);
	else if (strcmp(type, "string") == 0)
	{
		getstr(in, parsed_value);
		if (in_mask)
			*parsed_mask = parse_string_mask(in_mask,
						parsed_value->len, error);
	}
	else
		g_assert_not_reached();
}

static Match *match_new(void)
{
	Match *match;

	match = g_new(Match, 1);
	match->range_start = 0;
	match->range_length = 1;
	match->word_size = 1;
	match->data_length = 0;
	match->data = NULL;
	match->mask = NULL;
	match->matches = NULL;

	return match;
}

static void match_free(Match *match)
{
	GList *next;

	g_return_if_fail(match != NULL);

	for (next = match->matches; next; next = next->next)
		match_free((Match *) next->data);

	g_list_free(match->matches);

	g_free(match->data);
	g_free(match->mask);

	g_free(match);
}

/* Sets match->range_start and match->range_length */
static void match_offset(Match *match, xmlNode *node, GError **error)
{
	char *offset = NULL;
	char *end;

	offset = my_xmlGetNsProp(node, "offset", NULL);
	if (offset == NULL || !*offset)
	{
		g_set_error(error, MIME_ERROR, 0, "Missing 'offset' attribute");
		goto err;
	}

	match->range_start = strtol(offset, &end, 10);
	if (errno == ERANGE) {
		char *number;
		number = g_strndup(offset, end-offset);
		g_set_error(error, MIME_ERROR, 0,
			    "Number out-of-range (%s should fit in 4 bytes)",
			    number);
		g_free(number);
		return;
	}

	if (*end == ':' && end[1] && match->range_start >= 0)
	{
		int last;
		char *begin;

		begin = end + 1;
		last = strtol(begin, &end, 10);
		if (errno == ERANGE) {
			char *number;
			number = g_strndup(begin, end-begin);
			g_set_error(error, MIME_ERROR, 0,
				    "Number out-of-range (%s should fit in 8 bytes)",
				    number);
			g_free(number);
			return;
		}

		if (*end == '\0' && last >= match->range_start)
			match->range_length = last - match->range_start + 1;
		else
			g_set_error(error, MIME_ERROR, 0, "Invalid offset");
	}
	else if (*end != '\0')
		g_set_error(error, MIME_ERROR, 0, "Invalid offset");
err:
	xmlFree(offset);
}

/* Sets match->data, match->data_length and match->mask */
static void match_value_and_mask(Match *match, xmlNode *node, GError **error)
{
	char *mask = NULL;
	char *value = NULL;
	char *type = NULL;
	char *parsed_mask = NULL;
	GString *parsed_value;

	type = my_xmlGetNsProp(node, "type", NULL);
	g_return_if_fail(type != NULL);

	mask = my_xmlGetNsProp(node, "mask", NULL);
	value = my_xmlGetNsProp(node, "value", NULL);

	parsed_value = g_string_new(NULL);

	parse_value(type, value, mask, parsed_value,
			&parsed_mask, error);

	if (*error)
	{
		g_string_free(parsed_value, TRUE);
		g_return_if_fail(parsed_mask == NULL);
	}
	else
	{
		match->data = parsed_value->str;
		match->data_length = parsed_value->len;
		match->mask = parsed_mask;

		g_string_free(parsed_value, FALSE);
	}

	if (mask)
		xmlFree(mask);
	if (value)
		xmlFree(value);
	xmlFree(type);
}

/* Sets match->word_size */
static void match_word_size(Match *match, xmlNode *node, GError **error)
{
	char *type;

	type = my_xmlGetNsProp(node, "type", NULL);

	if (!type)
	{
		g_set_error(error, MIME_ERROR, 0,
			_("Missing 'type' attribute in <match>"));
		return;
	}

	if (strcmp(type, "host16") == 0)
		match->word_size = 2;
	else if (strcmp(type, "host32") == 0)
		match->word_size = 4;
	else if (!*error && strcmp(type, "big16") &&
			strcmp(type, "big32") &&
			strcmp(type, "little16") && strcmp(type, "little32") &&
			strcmp(type, "string") && strcmp(type, "byte"))
	{
		g_set_error(error, MIME_ERROR, 0,
				"Unknown magic type '%s'", type);
	}

	xmlFree(type);
}

/* Turn the list of child nodes of 'parent' into a list of Matches */
static GList *build_matches(xmlNode *parent, GError **error)
{
	xmlNode *node;
	GList *out = NULL;

	g_return_val_if_fail(error != NULL, NULL);

	for (node = parent->xmlChildrenNode; node; node = node->next)
	{
		Match *match;

		if (node->type != XML_ELEMENT_NODE)
			continue;

		if (node->ns == NULL || xmlStrcmp(node->ns->href, FREE_NS) != 0)
		{
			g_set_error(error, MIME_ERROR, 0,
				_("Element found with non-freedesktop.org "
				  "namespace"));
			break;
		}

		if (strcmp((char *)node->name, "match") != 0)
		{
			g_set_error(error, MIME_ERROR, 0,
				_("Expected <match> element, but found "
				  "<%s> instead"), node->name);
			break;
		}

		match = match_new();
		match_offset(match, node, error);
		if (!*error)
			match_word_size(match, node, error);
		if (!*error)
			match_value_and_mask(match, node, error);

		if (*error)
		{
			match_free(match);
			break;
		}

		out = g_list_append(out, match);

		match->matches = build_matches(node, error);
		if (*error)
			break;
	}

	return out;
}

static void magic_free(Magic *magic)
{
	GList *next;

	g_return_if_fail(magic != NULL);

	for (next = magic->matches; next; next = next->next)
		match_free((Match *) next->data);
	g_list_free(magic->matches);

	g_free(magic);
}

/* Create a new Magic object by parsing 'node' (a <magic> element) */
static Magic *magic_new(xmlNode *node, Type *type, GError **error)
{
	Magic *magic = NULL;
	int prio;

	g_return_val_if_fail(node != NULL, NULL);
	g_return_val_if_fail(type != NULL, NULL);
	g_return_val_if_fail(error != NULL, NULL);

	prio = get_priority(node);

	if (prio == -1)
	{
		g_set_error(error, MIME_ERROR, 0,
			_("Bad priority (%d) in <magic> element"), prio);
	}
	else
	{
		magic = g_new0(Magic, 1);
		magic->priority = prio;
		magic->type = type;
		magic->matches = build_matches(node, error);


		if (*error)
		{
			gchar *old = (*error)->message;
			magic_free(magic);
			magic = NULL;
			(*error)->message = g_strconcat(
				_("Error in <match> element: "), old, NULL);
			g_free(old);
		} else if (magic->matches == NULL) {
			magic_free(magic);
			magic = NULL;
			g_set_error(error, MIME_ERROR, 0,
				    _("Incomplete <magic> element"));
		}
	}

	return magic;
}

static TreeMatch *tree_match_new(void)
{
	TreeMatch *match;

	match = g_new(TreeMatch, 1);
	match->path = NULL;
	match->match_case = 0;
	match->executable = 0;
	match->non_empty = 0;
	match->type = 0;
	match->mimetype = NULL;
	match->matches = NULL;

	return match;
}

static void tree_match_free(TreeMatch *match)
{
	GList *next;

	g_return_if_fail(match != NULL);

	for (next = match->matches; next; next = next->next)
		tree_match_free((TreeMatch *) next->data);

	g_list_free(match->matches);

	g_free(match->path);
	g_free(match->mimetype);

	g_free(match);
}

/* Turn the list of child nodes of 'parent' into a list of TreeMatches */
static GList *build_tree_matches(xmlNode *parent, GError **error)
{
	xmlNode *node;
	GList *out = NULL;
	char *attr;

	g_return_val_if_fail(error != NULL, NULL);

	for (node = parent->xmlChildrenNode; node; node = node->next)
	{
		TreeMatch *match;

		if (node->type != XML_ELEMENT_NODE)
			continue;

		if (node->ns == NULL || xmlStrcmp(node->ns->href, FREE_NS) != 0)
		{
			g_set_error(error, MIME_ERROR, 0,
				_("Element found with non-freedesktop.org "
				  "namespace"));
			break;
		}

		if (strcmp((char *)node->name, "treematch") != 0)
		{
			g_set_error(error, MIME_ERROR, 0,
				_("Expected <treematch> element, but found "
				  "<%s> instead"), node->name);
			break;
		}

		match = tree_match_new();

		attr = my_xmlGetNsProp(node, "path", NULL);
		if (attr)
		{
			match->path = g_strdup (attr);
			xmlFree (attr);
		}
		else 
		{
                	g_set_error(error, MIME_ERROR, 0,
                        	_("Missing 'path' attribute in <treematch>"));
        	}
		if (!*error) 
		{
			attr = my_xmlGetNsProp(node, "type", NULL);
			if (attr) 
			{
				if (strcmp (attr, "file") == 0) 
				{
					match->type = 1;
				}
				else if (strcmp (attr, "directory") == 0)
				{
					match->type = 2;
				}
				else if (strcmp (attr, "link") == 0)
				{
					match->type = 3;
				}
				else
				{
                			g_set_error(error, MIME_ERROR, 0,
						_("Invalid 'type' attribute in <treematch>"));
				}
				xmlFree(attr);
			}
		}
		if (!*error)
		{
			attr = my_xmlGetNsProp(node, "executable", NULL);
			if (attr)
			{
				if (strcmp (attr, "true") == 0) 
				{
					match->executable = 1;
				}
				xmlFree(attr);
			}
		}
		if (!*error)
		{
			attr = my_xmlGetNsProp(node, "match-case", NULL);
			if (attr)
			{
				if (strcmp (attr, "true") == 0) 
				{
					match->match_case = 1;
				}
				xmlFree(attr);
			}
		}
		if (!*error)
		{
			attr = my_xmlGetNsProp(node, "non-empty", NULL);
			if (attr)
			{
				if (strcmp (attr, "true") == 0) 
				{
					match->non_empty = 1;
				}
				xmlFree(attr);
			}
		}
		if (!*error)
		{
			attr = my_xmlGetNsProp(node, "mimetype", NULL);
			if (attr)
			{
				match->mimetype = g_strdup (attr);
				xmlFree(attr);
			}
		}

		if (*error)
		{
			tree_match_free(match);
			break;
		}

		out = g_list_append(out, match);

		match->matches = build_tree_matches(node, error);
		if (*error)
			break;
	}

	return out;
}

static void tree_magic_free(TreeMagic *magic)
{
	GList *next;

	g_return_if_fail(magic != NULL);

	for (next = magic->matches; next; next = next->next)
		tree_match_free((TreeMatch *) next->data);
	g_list_free(magic->matches);

	g_free(magic);
}

/* Create a new TreeMagic object by parsing 'node' (a <treemagic> element) */
static TreeMagic *tree_magic_new(xmlNode *node, Type *type, GError **error)
{
	TreeMagic *magic = NULL;
	int prio;

	g_return_val_if_fail(node != NULL, NULL);
	g_return_val_if_fail(type != NULL, NULL);
	g_return_val_if_fail(error != NULL, NULL);

	prio = get_priority(node);

	if (prio == -1)
	{
		g_set_error(error, MIME_ERROR, 0,
			_("Bad priority (%d) in <treemagic> element"), prio);
	}
	else
	{
		magic = g_new(TreeMagic, 1);
		magic->priority = prio;
		magic->type = type;
		magic->matches = build_tree_matches(node, error);

		if (*error)
		{
			gchar *old = (*error)->message;
			tree_magic_free(magic);
			magic = NULL;
			(*error)->message = g_strconcat(
				_("Error in <treematch> element: "), old, NULL);
			g_free(old);
		}
	}

	return magic;
}

/* Write a list of Match elements (and their children) to the 'magic' file */
static void write_magic_children(FILE *stream, GList *matches, int indent)
{
	GList *next;

	for (next = matches; next; next = next->next)
	{
		Match *match = (Match *) next->data;

		if (indent)
			g_fprintf(stream,
				  "%d>%ld=",
				  indent,
				  match->range_start);
		else
			g_fprintf(stream, ">%ld=", match->range_start);

		write16(stream, match->data_length);
		fwrite(match->data, match->data_length, 1, stream);
		if (match->mask)
		{
			fputc('&', stream);
			fwrite(match->mask, match->data_length, 1, stream);
		}
		if (match->word_size != 1)
			g_fprintf(stream, "~%d", match->word_size);
		if (match->range_length != 1)
			g_fprintf(stream, "+%d", match->range_length);

		fputc('\n', stream);

		write_magic_children(stream, match->matches, indent + 1);
	}
}

/* Write a whole Magic element to the 'magic' file */
static void write_magic(FILE *stream, Magic *magic)
{
	g_fprintf(stream, "[%d:%s/%s]\n", magic->priority,
		magic->type->media, magic->type->subtype);

	write_magic_children(stream, magic->matches, 0);
}

/* Write a list of TreeMatch elements (and their children) to the 'treemagic' file */
static void write_tree_magic_children(FILE *stream, GList *matches, int indent)
{
	GList *next;

	for (next = matches; next; next = next->next)
	{
		TreeMatch *match = (TreeMatch *) next->data;

		if (indent)
			g_fprintf(stream,
				  "%d>\"%s\"=",
				  indent,
				  match->path);
		else
			g_fprintf(stream, ">\"%s\"=", match->path);

		switch (match->type)
		{
		default:
		case 0: 
			fputs("any", stream);
			break;
		case 1: 
			fputs("file", stream);
			break;
		case 2: 
			fputs("directory", stream);
			break;
		case 3: 
			fputs("link", stream);
			break;
		}
		if (match->match_case)
			fputs (",match-case", stream);
		if (match->executable)
			fputs (",executable", stream);
		if (match->non_empty)
			fputs (",non-empty", stream);
		if (match->mimetype)
			g_fprintf (stream, ",%s", match->mimetype);

		fputc('\n', stream);

		write_tree_magic_children(stream, match->matches, indent + 1);
	}
}
/* Write a whole TreeMagic element to the 'treemagic' file */
static void write_tree_magic(FILE *stream, TreeMagic *magic)
{
	g_fprintf(stream, "[%d:%s/%s]\n", magic->priority,
		magic->type->media, magic->type->subtype);

	write_tree_magic_children(stream, magic->matches, 0);
}

/* Check each of the directories with generated XML files, looking for types
 * which we didn't get on this scan, and delete them.
 */
static void delete_old_types(const gchar *mime_dir)
{
	int i;

	for (i = 0; i < G_N_ELEMENTS(media_types); i++)
	{
		gchar *media_dir;
		DIR   *dir;
		struct dirent *ent;
		
		media_dir = g_strconcat(mime_dir, "/", media_types[i], NULL);
		dir = opendir(media_dir);
		g_free(media_dir);
		if (!dir)
			continue;

		while ((ent = readdir(dir)))
		{
			char *type_name;
			int l;
			l = strlen(ent->d_name);
			if (l < 4 || strcmp(ent->d_name + l - 4, ".xml") != 0)
				continue;

			type_name = g_strconcat(media_types[i], "/",
						ent->d_name, NULL);
			type_name[strlen(type_name) - 4] = '\0';
			if (!g_hash_table_lookup(types, type_name))
			{
				char *path;
				path = g_strconcat(mime_dir, "/",
						type_name, ".xml", NULL);
#if 0
				g_warning("Removing old info for type %s",
						path);
#endif
				unlink(path);
				g_free(path);
			}
			g_free(type_name);
		}
		
		closedir(dir);
	}
}

/* Extract one entry from namespace_hash and put it in the GPtrArray so
 * we can sort it.
 */
static void add_ns(gpointer key, gpointer value, gpointer data)
{
	GPtrArray *lines = (GPtrArray *) data;
	const gchar *ns = (gchar *) key;
	Type *type = (Type *) value;

	g_ptr_array_add(lines, g_strconcat(ns, " ", type->media,
					   "/", type->subtype, "\n", NULL));
}

/* Write all the collected namespace rules to 'XMLnamespaces' */
static void write_namespaces(FILE *stream)
{
	GPtrArray *lines;
	int i;
	
	lines = g_ptr_array_new();

	g_hash_table_foreach(namespace_hash, add_ns, lines);

	g_ptr_array_sort(lines, strcmp2);

	for (i = 0; i < lines->len; i++)
	{
		char *line = (char *) lines->pdata[i];

		fwrite(line, 1, strlen(line), stream);

		g_free(line);
	}

	g_ptr_array_free(lines, TRUE);
}

static void write_subclass(gpointer key, gpointer value, gpointer data)
{
	GSList *list = value;
	FILE *stream = data;
	GSList *l;
	char *line;

	for (l = list; l; l = l->next)
	{
		line = g_strconcat (key, " ", l->data, "\n", NULL);
		fwrite(line, 1, strlen(line), stream);
		g_free (line);
	}
}

/* Write all the collected subclass information to 'subclasses' */
static void write_subclasses(FILE *stream)
{
	g_hash_table_foreach(subclass_hash, write_subclass, stream);
}

/* Extract one entry from alias_hash and put it in the GPtrArray so
 * we can sort it.
 */
static void add_alias(gpointer key, gpointer value, gpointer data)
{
	GPtrArray *lines = (GPtrArray *) data;
	const gchar *alias = (gchar *) key;
	Type *type = (Type *) value;
	
	g_ptr_array_add(lines, g_strconcat(alias, " ", type->media,
					   "/", type->subtype, "\n", 
					   NULL));
}

/* Write all the collected aliases */
static void write_aliases(FILE *stream)
{
	GPtrArray *lines;
	int i;
	
	lines = g_ptr_array_new();

	g_hash_table_foreach(alias_hash, add_alias, lines);

	g_ptr_array_sort(lines, strcmp2);

	for (i = 0; i < lines->len; i++)
	{
		char *line = (char *) lines->pdata[i];

		fwrite(line, 1, strlen(line), stream);

		g_free(line);
	}

	g_ptr_array_free(lines, TRUE);
}

static void add_type(gpointer key, gpointer value, gpointer data)
{
	GPtrArray *lines = (GPtrArray *) data;
	
	g_ptr_array_add(lines, g_strconcat((char *)key, "\n", NULL));
}

/* Write all the collected types */
static void write_types(FILE *stream)
{
	GPtrArray *lines;
	int i;
	
	lines = g_ptr_array_new();

	g_hash_table_foreach(types, add_type, lines);

	g_ptr_array_sort(lines, strcmp2);

	for (i = 0; i < lines->len; i++)
	{
		char *line = (char *) lines->pdata[i];

		fwrite(line, 1, strlen(line), stream);

		g_free(line);
	}

	g_ptr_array_free(lines, TRUE);
}


static void write_one_icon(gpointer key, gpointer value, gpointer data)
{
	char *mimetype = (char *)key;
	char *iconname = (char *)value;
	FILE *stream = (FILE *)data;
	char *line;

	line = g_strconcat (mimetype, ":", iconname, "\n", NULL);
	fwrite(line, 1, strlen(line), stream);
	g_free (line);
}

static void write_icons(GHashTable *icons, FILE *stream)
{
	g_hash_table_foreach(icons, write_one_icon, stream);
}

/* Issue a warning if 'path' won't be found by applications */
static void check_in_path_xdg_data(const char *mime_path)
{
	struct stat path_info, dir_info;
	const char *env;
	char **dirs;
	char *path;
	int i, n;

	path = g_path_get_dirname(mime_path);

	if (stat(path, &path_info))
	{
		g_warning("Can't stat '%s' directory: %s",
			  path, g_strerror(errno));
		goto out;
	}

	env = getenv("XDG_DATA_DIRS");
	if (!env)
		env = "/usr/local/share/"PATH_SEPARATOR"/usr/share/";
	dirs = g_strsplit(env, PATH_SEPARATOR, 0);
	g_return_if_fail(dirs != NULL);
	for (n = 0; dirs[n]; n++)
		;
	env = getenv("XDG_DATA_HOME");
	if (env)
		dirs[n] = g_strdup(env);
	else
		dirs[n] = g_build_filename(g_get_home_dir(), ".local",
						"share", NULL);
	n++;
	
	for (i = 0; i < n; i++)
	{
		if (stat(dirs[i], &dir_info) == 0 &&
		    dir_info.st_ino == path_info.st_ino &&
		    dir_info.st_dev == path_info.st_dev)
			break;
	}

	if (i == n)
	{
		g_printerr(_("\nNote that '%s' is not in the search path\n"
			     "set by the XDG_DATA_HOME and XDG_DATA_DIRS\n"
			     "environment variables, so applications may not\n"
			     "be able to find it until you set them. The\n"
			     "directories currently searched are:\n\n"), path);
		g_printerr("- %s\n", dirs[n - 1]);
		for (i = 0; i < n - 1; i++)
			g_printerr("- %s\n", dirs[i]);
		g_printerr("\n");
	}

	for (i = 0; i < n; i++)
		g_free(dirs[i]);
	g_free(dirs);
out:
	g_free(path);
}

static void free_string_list(gpointer data)
{
  GSList *list = data;

  g_slist_foreach(list, (GFunc)g_free, NULL);
  g_slist_free(list);
}

#define ALIGN_VALUE(this, boundary) \
  (( ((unsigned long)(this)) + (((unsigned long)(boundary)) -1)) & (~(((unsigned long)(boundary))-1)))


static gint
write_data (FILE *cache, const gchar *n, gint len)
{
  gchar *s;
  int i, l;
  
  l = ALIGN_VALUE (len, 4);
  
  s = g_malloc0 (l);
  memcpy (s, n, len);

  i = fwrite (s, l, 1, cache);

  return i == 1;
  
}

static gint
write_string (FILE *cache, const gchar *n)
{
  return write_data (cache, n, strlen (n) + 1);
}

static gboolean
write_card16 (FILE *cache, guint16 n)
{
  int i;

  n = GUINT16_TO_BE (n);
  
  i = fwrite ((char *)&n, 2, 1, cache);

  return i == 1;
}

static gboolean
write_card32 (FILE *cache, guint32 n)
{
  int i;

  n = GUINT32_TO_BE (n);
  
  i = fwrite ((char *)&n, 4, 1, cache);

  return i == 1;
}

#define MAJOR_VERSION 1
#define MINOR_VERSION 2

static gboolean
write_header (FILE *cache,   
	      gint  alias_offset,
	      gint  parent_offset,
	      gint  literal_offset,
	      gint  suffix_offset,
	      gint  glob_offset,
	      gint  magic_offset,
	      gint  namespace_offset,
	      gint  icons_list_offset,
	      gint  generic_icons_list_offset,
	      gint  type_offset,
	      guint *offset)
{
  *offset = 44;

  return (write_card16 (cache, MAJOR_VERSION) &&
	  write_card16 (cache, MINOR_VERSION) &&
	  write_card32 (cache, alias_offset) &&
	  write_card32 (cache, parent_offset) &&
	  write_card32 (cache, literal_offset) &&
	  write_card32 (cache, suffix_offset) &&
	  write_card32 (cache, glob_offset) &&
	  write_card32 (cache, magic_offset) &&
	  write_card32 (cache, namespace_offset) &&
	  write_card32 (cache, icons_list_offset) &&
	  write_card32 (cache, generic_icons_list_offset) &&
	  write_card32 (cache, type_offset));
}


typedef gboolean (FilterFunc) (gpointer key);
typedef gchar ** (GetValueFunc) (gpointer data, gchar *key);

typedef struct
{
  FILE         *cache;
  GHashTable   *pool;
  guint         offset;
  GetValueFunc *get_value;
  gpointer      data;
  gboolean      weighted;
  gboolean      error;
} MapData;

static void
write_map_entry (gpointer key,
		 gpointer data)
{
  MapData *map_data = (MapData *)data;
  gchar **values;
  guint offset, i;
  guint weight;

  values = (* map_data->get_value) (map_data->data, key);
  for (i = 0; values[i]; i++)
    {
      if (map_data->weighted && (i % 3 == 2)) 
        {
          weight = atoi (values[i]);

          if (!write_card32 (map_data->cache, weight))
            map_data->error = TRUE;

          map_data->offset += 4;
        }
      else 
        {
          offset = GPOINTER_TO_UINT (g_hash_table_lookup (map_data->pool, values[i]));
          if (offset == 0)
            {
              g_warning ("Missing string: '%s'", values[i]);
              map_data->error = TRUE;
            }
          if (!write_card32 (map_data->cache, offset))
          map_data->error = TRUE;
          map_data->offset += 4;
        }
    }

  g_strfreev (values);
}

typedef struct 
{
  FilterFunc *filter;
  GPtrArray  *keys;
} FilterData;

static void 
add_key (gpointer key, 
	 gpointer value, 
	 gpointer data)
{
  FilterData *filter_data = (FilterData *)data;

  if (!filter_data->filter || (* filter_data->filter) (key))
    g_ptr_array_add (filter_data->keys, key);
}

typedef struct
{
  GetValueFunc *get_value;
  gpointer      data;
  guint count;
  gboolean weighted;
} CountData;

static void
count_map_entry (gpointer key,
		 gpointer data)
{
  CountData *count_data = (CountData *)data;
  gchar **values;

  values = (* count_data->get_value) (count_data->data, key);
  count_data->count += g_strv_length (values) / (count_data->weighted ? 3 : 2);
  g_strfreev (values);
}

static gboolean
write_map (FILE         *cache,
	   GHashTable   *strings,
	   GHashTable   *map,
	   FilterFunc   *filter,
           GetValueFunc *get_value,
           gboolean      weighted,
	   guint        *offset)
{
  GPtrArray *keys;
  MapData map_data;
  FilterData filter_data;
  CountData count_data;

  keys = g_ptr_array_new ();
  
  filter_data.keys = keys;
  filter_data.filter = filter;
  g_hash_table_foreach (map, add_key, &filter_data);

  g_ptr_array_sort (keys, strcmp2);

  count_data.data = map;
  count_data.count = 0;
  count_data.get_value = get_value;
  count_data.weighted = weighted;

  g_ptr_array_foreach (keys, count_map_entry, &count_data);

  if (!write_card32 (cache, count_data.count))
    return FALSE;

  map_data.cache = cache;
  map_data.pool = strings;
  map_data.get_value = get_value;
  map_data.data = map;
  map_data.weighted = weighted;
  map_data.offset = *offset + 4;
  map_data.error = FALSE;

  g_ptr_array_foreach (keys, write_map_entry, &map_data);

  *offset = map_data.offset;

  return !map_data.error;
}

static gchar **
get_type_value (gpointer  data, 
		gchar    *key)
{
  Type *type;
  gchar **result;

  type = (Type *)g_hash_table_lookup ((GHashTable *)data, key);
  
  result = g_new0 (gchar *, 3);
  result[0] = g_strdup (key);
  result[1] = g_strdup_printf ("%s/%s", type->media, type->subtype);

  return result;
}

static guint32
get_glob_weight_and_flags (Glob *glob)
{
  guint32 res;

  res = glob->weight & 0xff;
  if (glob->case_sensitive)
    res |= 0x100;
  return res;
}

static gchar **
get_glob_list_value (gpointer  data, 
		     gchar    *key)
{
  GList *list;
  Glob *glob;
  Type *type;
  gchar **result;
  gint i;

  list = (GList *)g_hash_table_lookup ((GHashTable *)data, key);
  
  result = g_new0 (gchar *, 1 + 3 * g_list_length (list));
  
  i = 0;
  for (; list; list = list->next)
    {
      glob = (Glob *)list->data;
      type = glob->type;

      result[i++] = g_strdup (glob->pattern);
      result[i++] = g_strdup_printf ("%s/%s", type->media, type->subtype);
      result[i++] = g_strdup_printf ("%ud", get_glob_weight_and_flags (glob));
    }
  return result;
}

static gboolean
write_alias_cache (FILE       *cache, 
		   GHashTable *strings,
		   guint      *offset)
{
  return write_map (cache, strings, alias_hash, NULL, get_type_value, FALSE, offset);
}
		   
static void
write_parent_entry (gpointer key,
		    gpointer data)
{
  gchar *mimetype = (gchar *)key;
  MapData *map_data = (MapData *)data;
  guint parents_offset, offset;
  GList *parents;

  parents = (GList *)g_hash_table_lookup (subclass_hash, mimetype);
  offset = GPOINTER_TO_UINT (g_hash_table_lookup (map_data->pool, mimetype));
  if (offset == 0)
    {
      g_warning ("Missing string: '%s'", (gchar *)key);
      map_data->error = TRUE;
    }

  parents_offset = map_data->offset;
  map_data->offset += 4 + 4 * g_list_length (parents);

  if (!write_card32 (map_data->cache, offset) ||
      !write_card32 (map_data->cache, parents_offset))
    map_data->error = TRUE;
}

static void
write_parent_list (gpointer key,
		   gpointer data)
{
  gchar *mimetype = (gchar *)key;
  MapData *map_data = (MapData *)data;
  guint offset;
  GList *parents, *p;

  parents = (GList *)g_hash_table_lookup (subclass_hash, mimetype);

  if (!write_card32 (map_data->cache, g_list_length (parents)))
    map_data->error = TRUE;

  for (p = parents; p; p = p->next)
    {
      gchar *parent = (gchar *)p->data;
      
      offset = GPOINTER_TO_UINT (g_hash_table_lookup (map_data->pool, parent));
      if (offset == 0)
	{
	  g_warning ("Missing string: '%s'", parent);
	  map_data->error = TRUE;
	}
      
      if (!write_card32 (map_data->cache, offset))
	map_data->error = TRUE;
    }

  map_data->offset += 4 + 4 * g_list_length (parents);
}

static gboolean
write_parent_cache (FILE       *cache,
		    GHashTable *strings,
		    guint      *offset)
{
  GPtrArray *keys;
  MapData map_data;
  FilterData filter_data;

  keys = g_ptr_array_new ();

  filter_data.keys = keys;
  filter_data.filter = NULL;
  g_hash_table_foreach (subclass_hash, add_key, &filter_data);

  g_ptr_array_sort (keys, strcmp2);

  if (!write_card32 (cache, keys->len))
    return FALSE;

  map_data.cache = cache;
  map_data.pool = strings;
  map_data.offset = *offset + 4 + keys->len * 8;
  map_data.error = FALSE;

  g_ptr_array_foreach (keys, write_parent_entry, &map_data);

  map_data.offset = *offset + 4 + keys->len * 8;
  g_ptr_array_foreach (keys, write_parent_list, &map_data);

  *offset = map_data.offset;

  return !map_data.error;
}

typedef enum 
{
  GLOB_LITERAL,
  GLOB_SIMPLE,
  GLOB_FULL
} GlobType;

static GlobType
glob_type (gchar *glob)
{
  gchar *ptr;
  gboolean maybe_in_simple_glob = FALSE;
  gboolean first_char = TRUE;

  ptr = glob;

  while (*ptr != '\0')
    {
      if (*ptr == '*' && first_char)
	maybe_in_simple_glob = TRUE;
      else if (*ptr == '\\' || *ptr == '[' || *ptr == '?' || *ptr == '*')
	return GLOB_FULL;
      
      first_char = FALSE;
      ptr = g_utf8_next_char (ptr);
    }

  if (maybe_in_simple_glob)
    return GLOB_SIMPLE;

  return GLOB_LITERAL;
}

static gboolean
is_literal_glob (gpointer key)
{
  return glob_type ((gchar *)key) == GLOB_LITERAL;
}

static gboolean
is_simple_glob (gpointer key)
{
  return glob_type ((gchar *)key) == GLOB_SIMPLE;
}

static gboolean
is_full_glob (gpointer key)
{
  return glob_type ((gchar *)key) == GLOB_FULL;
}

static gboolean
write_literal_cache (FILE       *cache,
		     GHashTable *strings,
		     guint      *offset)
{
  return write_map (cache, strings, globs_hash, is_literal_glob, 
		    get_glob_list_value, TRUE, offset); 
}

static gboolean
write_glob_cache (FILE       *cache,
		  GHashTable *strings,
		  guint      *offset)
{
  return write_map (cache, strings, globs_hash, is_full_glob, 
		    get_glob_list_value, TRUE, offset); 
}

typedef struct _SuffixEntry SuffixEntry;

struct _SuffixEntry
{
  gunichar character;
  gchar *mimetype;
  gint weight;
  guint32 flags;
  GList *children;
  guint size;
  guint depth;
};

static GList *
insert_suffix (gunichar *suffix, 
	       gchar    *mimetype,
	       gint      weight,
	       guint32   flags,
	       GList    *suffixes)
{
  GList *l;
  SuffixEntry *s = NULL;

  for (l = suffixes; l; l = l->next)
    {
      s = (SuffixEntry *)l->data;

      if (s->character > suffix[0])
	{
	  s = g_new0 (SuffixEntry, 1);
	  s->character = suffix[0];
	  s->mimetype = NULL;
	  s->children = NULL;

	  suffixes = g_list_insert_before (suffixes, l, s);
	}

      if (s->character == suffix[0])
	break;
    }

  if (!s || s->character != suffix[0])
    {
      s = g_new0 (SuffixEntry, 1);
      s->character = suffix[0];
      s->mimetype = NULL;
      s->children = NULL;

      suffixes = g_list_append (suffixes, s);
    }

  if (suffix[1] == 0)
    {
      GList *l2;
      SuffixEntry *s2;
      gboolean found = FALSE;

      for (l2 = s->children; l2; l2 = l2->next)
	{
	  s2 = (SuffixEntry *)l2->data;
	  if (s2->character != 0)
	    break;
	  if (strcmp (s2->mimetype, mimetype) == 0)
	    {
	      if (s2->weight < weight)
		s2->weight = weight;
	      found = TRUE;
	      break;
	    }
	}
      if (!found)
	{
	  s2 = g_new0 (SuffixEntry, 1);
	  s2->character = 0;
	  s2->mimetype = mimetype;
	  s2->weight = weight;
	  s2->flags = flags;
	  s2->children = NULL;
	  s->children = g_list_insert_before (s->children, l2, s2);
	}
    }
  else
    s->children = insert_suffix (suffix + 1, mimetype, weight, flags, s->children);

  return suffixes;
}

static void
ucs4_reverse (gunichar *in, glong len)
{
  int i;
  gunichar c;

  for (i = 0; i < len - i - 1; i++)
    {
      c = in[i];
      in[i] = in[len - i - 1];
      in[len - i - 1] = c;
    }
}

static void
build_suffixes (gpointer key,
		gpointer value,
		gpointer data)
{
  gchar *pattern = (gchar *)key;
  GList *list = (GList *)value;
  GList **suffixes = (GList **)data;
  gunichar *suffix;
  gchar *mimetype;
  Glob *glob;
  Type *type;
  glong len;
  guint32 flags;
  
  if (is_simple_glob (pattern))
    {
      suffix = g_utf8_to_ucs4 (pattern + 1, -1, NULL, &len, NULL);
      
      if (suffix == NULL)
	{
	  g_warning ("Glob '%s' is not valid UTF-8", pattern);
	  return;
	}

      ucs4_reverse (suffix, len);
      for ( ; list; list = list->next)
        {
          glob = (Glob *)list->data;
          type = glob->type;
          mimetype = g_strdup_printf ("%s/%s", type->media, type->subtype);

	  flags = 0;
	  if (glob->case_sensitive)
	    flags |= 0x100;
          *suffixes = insert_suffix (suffix, mimetype, glob->weight, flags, *suffixes);
        }

      g_free (suffix);
    }
}

static void
calculate_size (SuffixEntry *entry)
{
  GList *s;

  entry->size = 0;
  entry->depth = 0;
  for (s = entry->children; s; s= s->next)
    {
      SuffixEntry *child = (SuffixEntry *)s->data;

      calculate_size (child);
      entry->size += 1 + child->size;
      entry->depth = MAX (entry->depth, child->depth + 1);
    }
}

static gboolean 
write_suffix_entries (FILE        *cache, 
		      guint        depth,
		      SuffixEntry *entry,
		      GHashTable *strings, 
		      guint      *child_offset)
{
  GList *c;
  guint offset;

  if (depth > 0)
    {
      gboolean error = FALSE;

      for (c = entry->children; c; c = c->next)
	{
	  SuffixEntry *child = (SuffixEntry *)c->data;
	  if (!write_suffix_entries (cache, depth - 1, child, strings, child_offset))
	    error = TRUE;
	}

      return !error;
    }
    
  if (entry->mimetype)
    {
      offset = GPOINTER_TO_UINT(g_hash_table_lookup (strings, entry->mimetype));
      if (offset == 0)
	{
	  g_warning ("Missing string: '%s'", entry->mimetype);
	  return FALSE;
	}
    }
  else
    offset = 0;

  if (entry->character == 0)
    {
      if (!write_card32 (cache, entry->character))
        return FALSE;

      if (!write_card32 (cache, offset))
        return FALSE;

      if (!write_card32 (cache, (entry->weight & 0xff) | entry->flags))
        return FALSE;
    }
  else
    {
      if (!write_card32 (cache, entry->character))
        return FALSE;

      if (!write_card32 (cache, g_list_length (entry->children)))
        return FALSE;
  
      if (!write_card32 (cache, *child_offset))
        return FALSE;
    }

  *child_offset += 12 * g_list_length (entry->children);

  return TRUE;
}

static gboolean
write_suffix_cache (FILE        *cache, 
		    GHashTable *strings, 
		    guint      *offset)
{
  GList *suffixes, *s;
  guint n_entries;
  guint child_offset;
  guint depth, d;

  suffixes = NULL;

  g_hash_table_foreach (globs_hash, build_suffixes, &suffixes);

  n_entries = g_list_length (suffixes);

  *offset += 8;
  child_offset = *offset + 12 * n_entries;
  depth = 0;
  for (s = suffixes; s; s= s->next)
    {
      SuffixEntry *entry = (SuffixEntry *)s->data;
      calculate_size (entry);
      depth = MAX (depth, entry->depth + 1);
    }

  if (!write_card32 (cache, n_entries) || !write_card32 (cache, *offset))
    return FALSE;

  for (d = 0; d < depth; d++)
    {
      for (s = suffixes; s; s = s->next)
	{
	  SuffixEntry *entry = (SuffixEntry *)s->data;
	  
	  if (!write_suffix_entries (cache,  d, entry, strings, &child_offset))
	    return FALSE;
	}
    }

  *offset = child_offset;

  return TRUE;
}

typedef struct {
  FILE       *cache;
  GHashTable *strings;
  GList      *matches;
  guint       offset;
  gboolean    error;
} WriteMatchData;


static void
write_match (gpointer key,
	     gpointer data)
{
  Magic *magic = (Magic *)key;
  WriteMatchData *mdata = (WriteMatchData *)data;
  gchar *mimetype;
  guint offset;

  if (!write_card32 (mdata->cache, magic->priority))
    {
      mdata->error = TRUE;
      return;
    }

  mimetype = g_strdup_printf ("%s/%s", magic->type->media, magic->type->subtype);
  offset = GPOINTER_TO_UINT (g_hash_table_lookup (mdata->strings, mimetype));
  if (offset == 0)
    {
      g_warning ("Missing string: '%s'", mimetype);
      g_free (mimetype);
      mdata->error = TRUE;
      return;
    }
  g_free (mimetype);
  
  if (!write_card32 (mdata->cache, offset))
    {
      mdata->error = TRUE;
      return;
    }

  if (!write_card32 (mdata->cache, g_list_length (magic->matches)))
    {
      mdata->error = TRUE;
      return;
    }

    offset = mdata->offset + 32 * g_list_index (mdata->matches, magic->matches->data);

  if (!write_card32 (mdata->cache, offset))
    {
      mdata->error = TRUE;
      return;
    }
}

static gboolean
write_matchlet (FILE           *cache,
		Match          *match,
		GList          *matches,
		gint            offset,
		gint           *offset2)
{
  if (!write_card32 (cache, match->range_start) ||
      !write_card32 (cache, match->range_length) ||
      !write_card32 (cache, match->word_size) ||
      !write_card32 (cache, match->data_length) ||
      !write_card32 (cache, *offset2))
    return FALSE;
  
  *offset2 = ALIGN_VALUE (*offset2 + match->data_length, 4);
      
  if (match->mask)
    {
      if (!write_card32 (cache, *offset2))
	return FALSE;
      
      *offset2 = ALIGN_VALUE (*offset2 + match->data_length, 4);
    }
  else
    {
      if (!write_card32 (cache, 0))
	return FALSE;
    }

  if (match->matches)
    {
      if (!write_card32 (cache, g_list_length (match->matches)) ||
	  !write_card32 (cache, offset + 32 * g_list_index (matches, match->matches->data)))
	return FALSE;
    }
  else
    {
      if (!write_card32 (cache, 0) ||
	  !write_card32 (cache, 0))
	return FALSE;
    }

  return TRUE;
}  

static gboolean
write_matchlet_data (FILE           *cache,
		     Match          *match,
		     gint           *offset2)
{
  if (!write_data (cache, match->data, match->data_length))
    return FALSE;
  
  *offset2 = ALIGN_VALUE (*offset2 + match->data_length, 4);

  if (match->mask)
    {
      if (!write_data (cache, match->mask, match->data_length))
	return FALSE;

      *offset2 = ALIGN_VALUE (*offset2 + match->data_length, 4);
    }

  return TRUE;
}

static void
collect_matches_list (GList *list, GList **matches)
{
  GList *l;

  for (l = list; l; l = l->next)
    *matches = g_list_prepend (*matches, l->data);

  for (l = list; l; l = l->next)
    {  
      Match *match = (Match *)l->data;
      collect_matches_list (match->matches, matches);
    }
}

static void
collect_matches (gpointer key, gpointer data)
{
  Magic *magic = (Magic *)key;
  GList **matches = (GList **)data;

  collect_matches_list (magic->matches, matches);
}

static gboolean
write_magic_cache (FILE        *cache, 
		   GHashTable *strings, 
		   guint      *offset)
{
  guint n_entries, max_extent;
  gint offset2;
  GList *m;
  WriteMatchData data;
  
  data.matches = NULL;
  g_ptr_array_foreach (magic_array, collect_matches, &data.matches);
  data.matches = g_list_reverse (data.matches);

  max_extent = 0;
  for (m = data.matches; m; m = m->next)
    {
      Match *match = (Match *)m->data;
      max_extent = MAX (max_extent, match->data_length + match->range_start + match->range_length);
    }

  n_entries = magic_array->len;

  *offset += 12;
  
  if (!write_card32 (cache, n_entries) ||
      !write_card32 (cache, max_extent) ||
      !write_card32 (cache, *offset))
    return FALSE;

  *offset += 16 * n_entries;

  data.cache = cache;
  data.strings = strings;
  data.offset = *offset;
  data.error = FALSE;

  offset2 = *offset + 32 * g_list_length (data.matches);

  g_ptr_array_foreach (magic_array, write_match, &data);
  for (m = data.matches; m; m = m->next)
    {
      Match *match = (Match *)m->data;
      write_matchlet (cache, match, data.matches, *offset, &offset2);
    }

  offset2 = *offset + 32 * g_list_length (data.matches);

  for (m = data.matches; m; m = m->next)
    {
      Match *match = (Match *)m->data;
      write_matchlet_data (cache, match, &offset2);
    }

  *offset = offset2;

  g_list_free (data.matches);

  return !data.error;
}

static gchar **
get_namespace_value (gpointer  data, 
		     gchar    *key)
{
  Type *type;
  gchar **result;
  gchar *space;

  type = (Type *)g_hash_table_lookup ((GHashTable *)data, key);
  
  result = g_new0 (gchar *, 4);
  space = strchr (key, ' ');
  if (*space)
    {
      *space = '\0';
      result[0] = g_strdup (key);
      result[1] = g_strdup (space + 1);
      *space = ' ';
    }
  else 
    result[0] = g_strdup (key);

  result[2] = g_strdup_printf ("%s/%s", type->media, type->subtype);

  return result;
}

static gboolean
write_namespace_cache (FILE       *cache,
		       GHashTable *strings,
		       guint      *offset)
{
  return write_map (cache, strings, namespace_hash, NULL, 
		    get_namespace_value, FALSE, offset); 
}

static gchar **
get_icon_value (gpointer  data, 
                gchar    *key)
{
  gchar *iconname;
  gchar **result;

  iconname = (gchar *)g_hash_table_lookup ((GHashTable *)data, key);
  
  result = g_new0 (gchar *, 3);
  result[0] = g_strdup (key);
  result[1] = g_strdup (iconname);
  result[2] = NULL;

  return result;
}

static gboolean
write_icons_cache (FILE       *cache,
                   GHashTable *strings,
                   GHashTable *icon_hash,
                   guint      *offset)
{
  return write_map (cache, strings, icon_hash, NULL, 
                    get_icon_value, FALSE, offset); 
}

/* Write all the collected types */
static gboolean
write_types_cache (FILE       *cache,
                   GHashTable *strings,
                   GHashTable *types,
                   guint      *offset)
{
	GPtrArray *lines;
	int i;
	char *mimetype;
	guint mime_offset;
	
	lines = g_ptr_array_new();

	g_hash_table_foreach(types, add_type, lines);

	g_ptr_array_sort(lines, strcmp2);

  	if (!write_card32 (cache, lines->len))
    		return FALSE;

	for (i = 0; i < lines->len; i++)
	{
		mimetype = (char *) lines->pdata[i];
		mime_offset = GPOINTER_TO_UINT (g_hash_table_lookup (strings, mimetype));
		if (!write_card32 (cache, mime_offset))
			return FALSE;

		g_free(mimetype);
	}

  	*offset += 4 + 4 * lines->len;

	g_ptr_array_free(lines, TRUE);

	return TRUE;
}

static void
collect_alias (gpointer key,
	       gpointer value,
	       gpointer data)
{
  GHashTable *strings = (GHashTable *)data;
  Type *type = (Type *)value;
  gchar *mimetype;
  
  mimetype = g_strdup_printf ("%s/%s", type->media, type->subtype);
  g_hash_table_insert (strings, key, NULL);
  g_hash_table_insert (strings, mimetype, NULL);
}


static void
collect_parents (gpointer key,
		 gpointer value,
		 gpointer data)
{
  GList *parents = (GList *)value;
  GHashTable *strings = (GHashTable *)data;
  GList *p;
  
  g_hash_table_insert (strings, key, NULL);
  for (p = parents; p; p = p->next)
    g_hash_table_insert (strings, p->data, NULL);
}

static void
collect_glob (gpointer key,
	      gpointer value,
	      gpointer data)
{
  GList *list = (GList *)value;
  GHashTable *strings = (GHashTable *)data;
  gchar *mimetype;
  Glob *glob;
  Type *type;

  switch (glob_type ((char *)key))
    {
      case GLOB_LITERAL:
      case GLOB_FULL:
        g_hash_table_insert (strings, key, NULL);
        break;
     default:
        break;
   }

  for (; list; list = list->next)
    {
      glob = (Glob *)list->data;
      type = glob->type;
      mimetype = g_strdup_printf ("%s/%s", type->media, type->subtype);

     g_hash_table_insert (strings, mimetype, NULL);
    }
}

static void
collect_magic (gpointer key,
	       gpointer data)
{
  Magic *magic = (Magic *)key;
  GHashTable *strings = (GHashTable *)data;
  gchar *mimetype;
  
  mimetype = g_strdup_printf ("%s/%s", magic->type->media, magic->type->subtype);
  g_hash_table_insert (strings, mimetype, NULL);
}

static void
collect_namespace (gpointer key,
		   gpointer value,
		   gpointer data)
{
  gchar *ns = (gchar *)key;
  Type *type = (Type *)value;
  GHashTable *strings = (GHashTable *)data;
  gchar *mimetype;
  gchar *space;

  mimetype = g_strdup_printf ("%s/%s", type->media, type->subtype);
  g_hash_table_insert (strings, mimetype, NULL);
  
  space = strchr (ns, ' ');

  if (space)
    {
      *space = '\0';
      g_hash_table_insert (strings, g_strdup (ns), NULL);
      g_hash_table_insert (strings, space + 1, NULL);
      *space = ' ';
    }
  else     
    g_hash_table_insert (strings, ns, NULL);
}

static void 
collect_icons(gpointer key, 
              gpointer value, 
              gpointer data)
{
  gchar *mimetype = (gchar *)key;
  gchar *iconname = (gchar *)value;
  GHashTable *strings = (GHashTable *)data;

  g_hash_table_insert (strings, mimetype, NULL);
  g_hash_table_insert (strings, iconname, NULL);
}


static void
collect_strings (GHashTable *strings)
{
  g_hash_table_foreach (alias_hash, collect_alias, strings); 
  g_hash_table_foreach (subclass_hash, collect_parents, strings); 
  g_hash_table_foreach (globs_hash, collect_glob, strings); 
  g_ptr_array_foreach (magic_array, collect_magic, strings); 
  g_hash_table_foreach (namespace_hash, collect_namespace, strings); 
  g_hash_table_foreach (generic_icon_hash, collect_icons, strings); 
  g_hash_table_foreach (icon_hash, collect_icons, strings); 
}

typedef struct 
{
  FILE       *cache;
  GHashTable *strings;
  guint       offset;
  gboolean    error;
} StringData;

static void
write_one_string (gpointer key,
		  gpointer value,
		  gpointer data)
{
  gchar *str = (gchar *)key;
  StringData *sdata = (StringData *)data;

  if (!write_string (sdata->cache, str))
    sdata->error = TRUE;

  g_hash_table_insert (sdata->strings, str, GUINT_TO_POINTER (sdata->offset));
  
  sdata->offset = ALIGN_VALUE (sdata->offset + strlen (str) + 1, 4);
}

static gboolean
write_strings (FILE       *cache, 
	       GHashTable *strings,       
	       guint      *offset)
{
  StringData data;

  data.cache = cache;
  data.strings = strings;
  data.offset = *offset;
  data.error = FALSE;

  g_hash_table_foreach (strings, write_one_string, &data);

  *offset = data.offset;

  return !data.error;
}

static gboolean 
write_cache (FILE *cache)
{
  guint strings_offset;
  guint alias_offset;
  guint parent_offset;
  guint literal_offset;
  guint suffix_offset;
  guint glob_offset;
  guint magic_offset;
  guint namespace_offset;
  guint icons_list_offset;
  guint generic_icons_list_offset;
  guint type_offset;
  guint offset;
  GHashTable *strings;

  offset = 0;
  if (!write_header (cache, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, &offset))
    {
      g_warning ("Failed to write header");
      return FALSE;
    }

  strings = g_hash_table_new (g_str_hash, g_str_equal);
  collect_strings (strings);
  strings_offset = offset;

  if (!write_strings (cache, strings, &offset))
    {
      g_warning ("Failed to write strings");
      return FALSE;
    }
  g_message ("Wrote %d strings at %x - %x",
	   g_hash_table_size (strings), strings_offset, offset);

  alias_offset = offset;
  if (!write_alias_cache (cache, strings, &offset))
    {
      g_warning ("Failed to write alias list");
      return FALSE;
    }
  g_message ("Wrote aliases at %x - %x", alias_offset, offset);

  parent_offset = offset;
  if (!write_parent_cache (cache, strings, &offset))
    {
      g_warning ("Failed to write parent list");
      return FALSE;
    }
  g_message ("Wrote parents at %x - %x", parent_offset, offset);

  literal_offset = offset;
  if (!write_literal_cache (cache, strings, &offset))
    {
      g_warning ("Failed to write literal list");
      return FALSE;
    }
  g_message ("Wrote literal globs at %x - %x", literal_offset, offset);

  suffix_offset = offset;
  if (!write_suffix_cache (cache, strings, &offset))
    {
      g_warning ("Failed to write suffix list");
      return FALSE;
    }
  g_message ("Wrote suffix globs at %x - %x", suffix_offset, offset);

  glob_offset = offset;
  if (!write_glob_cache (cache, strings, &offset))
    {
      g_warning ("Failed to write glob list");
      return FALSE;
    }
  g_message ("Wrote full globs at %x - %x", glob_offset, offset);

  magic_offset = offset;
  if (!write_magic_cache (cache, strings, &offset))
    {
      g_warning ("Failed to write magic list");
      return FALSE;
    }
  g_message ("Wrote magic at %x - %x", magic_offset, offset);

  namespace_offset = offset;
  if (!write_namespace_cache (cache, strings, &offset))
    {
      g_warning ("Failed to write namespace list");
      return FALSE;
    }
  g_message ("Wrote namespace list at %x - %x", namespace_offset, offset);

  icons_list_offset = offset;
  if (!write_icons_cache (cache, strings, icon_hash, &offset))
    {
      g_warning ("Failed to write icons list");
      return FALSE;
    }
  g_message ("Wrote icons list at %x - %x", icons_list_offset, offset);

  generic_icons_list_offset = offset;
  if (!write_icons_cache (cache, strings, generic_icon_hash, &offset))
    {
      g_warning ("Failed to write generic icons list");
      return FALSE;
    }
  g_message ("Wrote generic icons list at %x - %x", generic_icons_list_offset, offset);

  type_offset = offset;
  if (!write_types_cache (cache, strings, types, &offset))
    {
      g_warning ("Failed to write types list");
      return FALSE;
    }
  g_message ("Wrote types list at %x - %x", type_offset, offset);

  rewind (cache);
  offset = 0; 

  if (!write_header (cache, 
		     alias_offset, parent_offset, literal_offset,
		     suffix_offset, glob_offset, magic_offset, 
		     namespace_offset, icons_list_offset,
		     generic_icons_list_offset, type_offset, 
		     &offset))
    {
      g_warning ("Failed to rewrite header");
      return FALSE;
    }

  g_hash_table_destroy (strings);

  return TRUE;
}


static FILE *
fopen_gerror(const char *filename, GError **error)
{
	FILE *stream = fopen(filename, "wb");

	if (!stream)
		set_error_from_errno(error);

	return stream;
}

static gboolean
fclose_gerror(FILE *f, GError **error)
{
	int err = ferror(f);
	if (err != 0)
	{
		set_error_from_errno(error);
		return FALSE;
	}
	if (fclose(f) != 0)
	{
		set_error_from_errno(error);
		return FALSE;
	}
	return TRUE;
}

static gint64
newest_mtime(const char *packagedir)
{
	GDir *dir;
#if !GLIB_CHECK_VERSION(2,26,0)
	struct stat GStatBuf;
#else
	GStatBuf statbuf;
#endif
	gint64 mtime = G_MININT64;
	const char *name;
	int retval;

	retval = g_stat(packagedir, &statbuf);
	if (retval < 0)
		return mtime;
	mtime = statbuf.st_mtime;

	dir = g_dir_open(packagedir, 0, NULL);
	if (!dir)
		return mtime;

	while ((name = g_dir_read_name(dir))) {
		char *path;

		path = g_build_filename(packagedir, name, NULL);
		retval = g_stat(path, &statbuf);
		g_free(path);
		if (retval < 0)
			continue;
		if (statbuf.st_mtime > mtime)
			mtime = statbuf.st_mtime;
	}

	g_dir_close(dir);
	return mtime;
}

static gboolean
is_cache_up_to_date (const char *mimedir, const char *packagedir)
{
	GStatBuf version_stat;
	gint64 package_mtime;
	char *mimeversion;
	int retval;

	mimeversion = g_build_filename(mimedir, "/version", NULL);
	retval = g_stat(mimeversion, &version_stat);
	g_free(mimeversion);
	if (retval < 0)
		return FALSE;

	package_mtime = newest_mtime(packagedir);
	if (package_mtime < 0)
		return FALSE;

	return version_stat.st_mtime >= package_mtime;
}

int main(int argc, char **argv)
{
	char *mime_dir = NULL;
	char *package_dir = NULL;
	int opt;
	GError *local_error = NULL;
	GError **error = &local_error;
	gboolean if_newer = FALSE;

	/* Install the filtering log handler */
	g_log_set_default_handler(g_log_handler, NULL);

	while ((opt = getopt(argc, argv, "hvVn")) != -1)
	{
		switch (opt)
		{
			case '?':
				usage(argv[0]);
				return EXIT_FAILURE;
			case 'h':
				usage(argv[0]);
				return EXIT_SUCCESS;
			case 'v':
				g_fprintf(stderr,
					  "update-mime-database (" PACKAGE ") "
					  VERSION "\n" COPYING);
				return EXIT_SUCCESS;
			case 'V':
				enabled_log_levels |= G_LOG_LEVEL_MESSAGE
						      | G_LOG_LEVEL_INFO;
				break;
			case 'n':
				if_newer = TRUE;
				break;
			default:
				return EXIT_FAILURE;
		}
	}

	if (optind != argc - 1)
	{
		usage(argv[0]);
		return EXIT_FAILURE;
	}

	LIBXML_TEST_VERSION;

	mime_dir = argv[optind];

	/* Strip trailing / characters */
	{
		int l = strlen(mime_dir);
		while (l > 1 && mime_dir[l - 1] == '/')
		{
			l--;
			mime_dir[l] = '\0';
		}
	}

	package_dir = g_strconcat(mime_dir, "/packages", NULL);

	if (access(mime_dir, F_OK))
	{
		g_warning(_("Directory '%s' does not exist!"), package_dir);
		return EXIT_FAILURE;
	}

	g_message("Updating MIME database in %s...\n", mime_dir);

	if (access(package_dir, F_OK))
	{
		g_fprintf(stderr,
			_("Directory '%s' does not exist!\n"), package_dir);
		return EXIT_FAILURE;
	}

	if (if_newer && is_cache_up_to_date(mime_dir, package_dir)) {
		g_message ("Skipping mime update as the cache is up-to-date");
		return EXIT_SUCCESS;
	}

	types = g_hash_table_new_full(g_str_hash, g_str_equal,
					g_free, free_type);
	globs_hash = g_hash_table_new_full(g_str_hash, g_str_equal,
					g_free, NULL);
	namespace_hash = g_hash_table_new_full(g_str_hash, g_str_equal,
					g_free, NULL);
	magic_array = g_ptr_array_new();
	tree_magic_array = g_ptr_array_new();
	subclass_hash = g_hash_table_new_full(g_str_hash, g_str_equal,
					      g_free, free_string_list);
	alias_hash = g_hash_table_new_full(g_str_hash, g_str_equal,
					   g_free, NULL);
	icon_hash = g_hash_table_new_full(g_str_hash, g_str_equal,
					  g_free, NULL);
	generic_icon_hash = g_hash_table_new_full(g_str_hash, g_str_equal,
						  g_free, NULL);

	scan_source_dir(package_dir);
	g_free(package_dir);

	delete_old_types(mime_dir);

	g_hash_table_foreach(types, write_out_type, (gpointer) mime_dir);

	{
		FILE *globs;
		char *globs_path;
		GList *glob_list = NULL;

		g_hash_table_foreach(globs_hash, collect_glob2, &glob_list);
		glob_list = g_list_sort(glob_list, (GCompareFunc)compare_glob_by_weight);
		globs_path = g_strconcat(mime_dir, "/globs.new", NULL);
		globs = fopen_gerror(globs_path, error);
		if (!globs)
			goto out;
		g_fprintf(globs,
			  "# This file was automatically generated by the\n"
			  "# update-mime-database command. DO NOT EDIT!\n");
		write_out_glob(glob_list, globs);
		if (!fclose_gerror(globs, error))
			goto out;
		if (!atomic_update(globs_path, error))
			goto out;
		g_free(globs_path);

		globs_path = g_strconcat(mime_dir, "/globs2.new", NULL);
		globs = fopen_gerror(globs_path, error);
		if (!globs)
			goto out;
		g_fprintf(globs,
			  "# This file was automatically generated by the\n"
			  "# update-mime-database command. DO NOT EDIT!\n");
		write_out_glob2 (glob_list, globs);
		if (!fclose_gerror(globs, error))
			goto out;
		if (!atomic_update(globs_path, error))
			goto out;
		g_free(globs_path);

		g_list_free (glob_list);
	}

	{
		FILE *stream;
		char *magic_path;
		int i;
		magic_path = g_strconcat(mime_dir, "/magic.new", NULL);
		stream = fopen_gerror(magic_path, error);
		if (!stream)
			goto out;
		fwrite("MIME-Magic\0\n", 1, 12, stream);

		if (magic_array->len)
			g_ptr_array_sort(magic_array, cmp_magic);
		for (i = 0; i < magic_array->len; i++)
		{
			Magic *magic = (Magic *) magic_array->pdata[i];

			write_magic(stream, magic);
		}
		if (!fclose_gerror(stream, error))
			goto out;
		if (!atomic_update(magic_path, error))
			goto out;
		g_free(magic_path);
	}

	{
		FILE *stream;
		char *ns_path;

		ns_path = g_strconcat(mime_dir, "/XMLnamespaces.new", NULL);
		stream = fopen_gerror(ns_path, error);
		if (!stream)
			goto out;
		write_namespaces(stream);
		if (!fclose_gerror(stream, error))
			goto out;
		if (!atomic_update(ns_path, error))
			goto out;
		g_free(ns_path);
	}
	
	{
		FILE *stream;
		char *path;
		
		path = g_strconcat(mime_dir, "/subclasses.new", NULL);
		stream = fopen_gerror(path, error);
		if (!stream)
			goto out;
		write_subclasses(stream);
		if (!fclose_gerror(stream, error))
			goto out;
		if (!atomic_update(path, error))
			goto out;
		g_free(path);
	}

	{
		FILE *stream;
		char *path;
		
		path = g_strconcat(mime_dir, "/aliases.new", NULL);
		stream = fopen_gerror(path, error);
		if (!stream)
			goto out;
		write_aliases(stream);
		if (!fclose_gerror(stream, error))
			goto out;
		if (!atomic_update(path, error))
			goto out;
		g_free(path);
	}

	{
		FILE *stream;
		char *path;
		
		path = g_strconcat(mime_dir, "/types.new", NULL);
		stream = fopen_gerror(path, error);
		if (!stream)
			goto out;
		write_types(stream);
		if (!fclose_gerror(stream, error))
			goto out;
		if (!atomic_update(path, error))
			goto out;
		g_free(path);
	}

	{
		FILE *stream;
		char *icon_path;

		icon_path = g_strconcat(mime_dir, "/generic-icons.new", NULL);
		stream = fopen_gerror(icon_path, error);
		if (!stream)
			goto out;
		write_icons(generic_icon_hash, stream);
		if (!fclose_gerror(stream, error))
			goto out;
		if (!atomic_update(icon_path, error))
			goto out;
		g_free(icon_path);
	}

	{
		FILE *stream;
		char *icon_path;

		icon_path = g_strconcat(mime_dir, "/icons.new", NULL);
		stream = fopen_gerror(icon_path, error);
		if (!stream)
			goto out;
		write_icons(icon_hash, stream);
		if (!fclose_gerror(stream, error))
			goto out;
		if (!atomic_update(icon_path, error))
			goto out;
		g_free(icon_path);
	}

	{
		FILE *stream;
		char *path;
		int i;
		path = g_strconcat(mime_dir, "/treemagic.new", NULL);
		stream = fopen_gerror(path, error);
		if (!stream)
			goto out;
		fwrite("MIME-TreeMagic\0\n", 1, 16, stream);

		if (tree_magic_array->len)
			g_ptr_array_sort(tree_magic_array, cmp_tree_magic);
		for (i = 0; i < tree_magic_array->len; i++)
		{
			TreeMagic *magic = (TreeMagic *) tree_magic_array->pdata[i];

			write_tree_magic(stream, magic);
		}
		if (!fclose_gerror(stream, error))
			goto out;
		if (!atomic_update(path, error))
			goto out;
		g_free(path);
	}

	{
		FILE *stream;
		char *path;
		
		path = g_strconcat(mime_dir, "/mime.cache.new", NULL);
		stream = fopen_gerror(path, error);
		if (!stream)
			goto out;
		write_cache(stream);
		if (!fclose_gerror(stream, error))
			goto out;
		if (!atomic_update(path, error))
			goto out;
		g_free(path);
	}

	{
		FILE *stream;
		char *path;

		path = g_strconcat(mime_dir, "/version.new", NULL);
		stream = fopen_gerror(path, error);
		if (!stream)
			goto out;
		g_fprintf(stream,
			  VERSION "\n");
		if (!fclose_gerror(stream, error))
			goto out;
		if (!atomic_update(path, error))
			goto out;
		g_free(path);
	}

	g_ptr_array_foreach(magic_array, (GFunc)magic_free, NULL);
	g_ptr_array_free(magic_array, TRUE);
	g_ptr_array_foreach(tree_magic_array, (GFunc)tree_magic_free, NULL);
	g_ptr_array_free(tree_magic_array, TRUE);

	g_hash_table_destroy(types);
	g_hash_table_destroy(globs_hash);
	g_hash_table_destroy(namespace_hash);
	g_hash_table_destroy(subclass_hash);
	g_hash_table_destroy(alias_hash);
	g_hash_table_destroy(icon_hash);
	g_hash_table_destroy(generic_icon_hash);

	check_in_path_xdg_data(mime_dir);

out:
	if (local_error != NULL)
		fatal_gerror(local_error);
	return EXIT_SUCCESS;
}
