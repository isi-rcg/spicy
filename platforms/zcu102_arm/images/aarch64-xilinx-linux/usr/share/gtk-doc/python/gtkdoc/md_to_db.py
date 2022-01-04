# -*- python; coding: utf-8 -*-
#
# gtk-doc - GTK DocBook documentation generator.
# Copyright (C) 1998  Damon Chaplin
#               2007-2016  Stefan Sauer
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
#

"""
Markdown to Docbook converter
"""

import logging
import re

# external functions
ExpandAbbreviations = MakeXRef = MakeHashXRef = tagify = None

# Elements to consider non-block items in MarkDown parsing
MD_TEXT_LEVEL_ELEMENTS = {
    'emphasis', 'envar', 'filename', 'firstterm', 'footnote', 'function', 'literal',
    'manvolnum', 'option', 'replaceable', 'structfield', 'structname', 'title',
    'varname'
}
MD_ESCAPABLE_CHARS = r'\`*_{}[]()>#+-.!'
MD_GTK_ESCAPABLE_CHARS = r'@%'


def Init():
    # TODO(enonic): find a better way to do this
    global ExpandAbbreviations, MakeXRef, MakeHashXRef, tagify
    from .mkdb import ExpandAbbreviations, MakeXRef, MakeHashXRef, tagify


def MarkDownParseBlocks(lines, symbol, context):
    md_blocks = []
    md_block = {"type": ''}

    logging.debug("parsing %s lines", len(lines))
    for line in lines:
        logging.info("type='%s', int='%s', parsing '%s'", md_block["type"], md_block.get('interrupted'), line)
        first_char = None
        if line:
            first_char = line[0]

        if md_block["type"] == "markup":
            if 'closed' not in md_block:
                if md_block["start"] in line:
                    md_block["depth"] += 1

                if md_block["end"] in line:
                    if md_block["depth"] > 0:
                        md_block["depth"] -= 1
                    else:
                        logging.info("closing tag '%s'", line)
                        md_block["closed"] = 1
                        # TODO(ensonic): reparse inner text with MarkDownParseLines?

                md_block["text"] += "\n" + line
                logging.info("add to markup: '%s'", line)
                continue

        deindented_line = line.lstrip()

        if md_block["type"] == "heading":
            # a heading is ended by any level less than or equal
            if md_block["level"] == 1:
                heading_match = re.search(r'^[#][ \t]+(.+?)[ \t]*[#]*[ \t]*(?:{#([^}]+)})?[ \t]*$', line)
                if re.search(r'^={4,}[ \t]*$', line):
                    text = md_block["lines"].pop()
                    md_block.pop("interrupted", None)
                    md_blocks.append(md_block)
                    md_block = {'type': "heading",
                                'text': text,
                                'lines': [],
                                'level': 1,
                                }
                    continue
                elif heading_match:
                    md_block.pop("interrupted", None)
                    md_blocks.append(md_block)
                    md_block = {'type': "heading",
                                'text': heading_match.group(1),
                                'lines': [],
                                'level': 1,
                                }
                    if heading_match.group(2):
                        md_block['id'] = heading_match.group(2)
                    continue
                else:
                    # push lines into the block until the end is reached
                    md_block["lines"].append(line)
                    continue

            else:
                heading_match = re.search(r'^([#]{1,2})[ \t]+(.+?)[ \t]*[#]*[ \t]*(?:{#([^}]+)})?[ \t]*$', line)
                if re.search(r'^[=]{4,}[ \t]*$', line):
                    text = md_block["lines"].pop()
                    md_block.pop("interrupted", None)
                    md_blocks.append(md_block)
                    md_block = {'type': "heading",
                                'text': text,
                                'lines': [],
                                'level': 1,
                                }
                    continue
                elif re.search(r'^[-]{4,}[ \t]*$', line):
                    text = md_block["lines"].pop()
                    md_block.pop("interrupted", None)
                    md_blocks.append(md_block)
                    md_block = {'type': "heading",
                                'text': text,
                                'lines': [],
                                'level': 2,
                                }
                    continue
                elif heading_match:
                    md_block.pop("interrupted", None)
                    md_blocks.append(md_block)
                    md_block = {'type': "heading",
                                'text': heading_match.group(2),
                                'lines': [],
                                'level': len(heading_match.group(1))
                                }
                    if heading_match.group(3):
                        md_block['id'] = heading_match.group(3)
                    continue
                else:
                    # push lines into the block until the end is reached
                    md_block["lines"].append(line)
                    continue
        elif md_block["type"] == "code":
            end_of_code_match = re.search(r'^[ \t]*\]\|(.*)', line)
            if end_of_code_match:
                md_blocks.append(md_block)
                md_block = {'type': "paragraph",
                            'text': end_of_code_match.group(1),
                            'lines': [],
                            }
            else:
                md_block["lines"].append(line)
            continue

        if deindented_line == '':
            logging.info('setting "interrupted" due to empty line')
            md_block["interrupted"] = 1
            continue

        if md_block["type"] == "quote":
            if 'interrupted' not in md_block:
                line = re.sub(r'^[ ]*>[ ]?', '', line)
                md_block["lines"].append(line)
                continue

        elif md_block["type"] == "li":
            marker = md_block["marker"]
            marker_match = re.search(r'^([ ]{0,3})(%s)[ ](.*)' % marker, line)
            if marker_match:
                indentation = marker_match.group(1)
                if md_block["indentation"] != indentation:
                    md_block["lines"].append(line)
                else:
                    ordered = md_block["ordered"]
                    md_block.pop('last', None)
                    md_blocks.append(md_block)
                    md_block = {'type': "li",
                                'ordered': ordered,
                                'indentation': indentation,
                                'marker': marker,
                                'last': 1,
                                'lines': [re.sub(r'^[ ]{0,4}', '', marker_match.group(3))],
                                }
                continue

            if 'interrupted' in md_block:
                if first_char == " ":
                    md_block["lines"].append('')
                    line = re.sub(r'^[ ]{0,4}', '', line)
                    md_block["lines"].append(line)
                    md_block.pop("interrupted", None)
                    continue
            else:
                line = re.sub(r'^[ ]{0,4}', '', line)
                md_block["lines"].append(line)
                continue

        # indentation sensitive types
        heading_match = re.search(r'^([#]{1,2})[ \t]+(.+?)[ \t]*[#]*[ \t]*(?:{#([^}]+)})?[ \t]*$', line)
        code_match = re.search(r'^[ \t]*\|\[[ ]*(?:<!-- language="([^"]+?)" -->)?', line)
        if heading_match:
            # atx heading (#)
            md_blocks.append(md_block)
            md_block = {'type': "heading",
                        'text': heading_match.group(2),
                        'lines': [],
                        'level': len(heading_match.group(1)),
                        }
            if heading_match.group(3):
                md_block['id'] = heading_match.group(3)
            continue
        elif re.search(r'^={4,}[ \t]*$', line):
            # setext heading (====)

            if md_block["type"] == "paragraph" and "interrupted" in md_block:
                md_blocks.append(md_block.copy())
                md_block["type"] = "heading"
                md_block["lines"] = []
                md_block["level"] = 1
            continue
        elif re.search(r'^-{4,}[ \t]*$', line):
            # setext heading (-----)

            if md_block["type"] == "paragraph" and "interrupted" in md_block:
                md_blocks.append(md_block.copy())
                md_block["type"] = "heading"
                md_block["lines"] = []
                md_block["level"] = 2

            continue
        elif code_match:
            # code
            md_block["interrupted"] = 1
            md_blocks.append(md_block)
            md_block = {'type': "code",
                        'lines': [],
                        }
            if code_match.group(1):
                md_block['language'] = code_match.group(1)
            continue

        # indentation insensitive types
        markup_match = re.search(r'^[ ]*<\??(\w+)[^>]*([\/\?])?[ \t]*>', line)
        li_match = re.search(r'^([ ]*)[*+-][ ](.*)', line)
        quote_match = re.search(r'^[ ]*>[ ]?(.*)', line)
        if re.search(r'^[ ]*<!DOCTYPE/', line):
            md_blocks.append(md_block)
            md_block = {'type': "markup",
                        'text': deindented_line,
                        'start': '<',
                        'end': '>',
                        'depth': 0,
                        }

        elif markup_match:
            # markup, including <?xml version="1.0"?>
            tag = markup_match.group(1)
            is_self_closing = markup_match.group(2) is not None

            # skip link markdown
            # TODO(ensonic): consider adding more uri schemes (ftp, ...)
            if re.search(r'https?', tag):
                logging.info("skipping link '%s'", tag)
            else:
                # for TEXT_LEVEL_ELEMENTS, we want to keep them as-is in the paragraph
                # instead of creation a markdown block.
                scanning_for_end_of_text_level_tag = (
                    md_block["type"] == "paragraph" and
                    'start' in md_block and
                    'closed' not in md_block)
                logging.info("markup found '%s', scanning %s ?", tag, scanning_for_end_of_text_level_tag)
                if tag not in MD_TEXT_LEVEL_ELEMENTS and not scanning_for_end_of_text_level_tag:
                    md_blocks.append(md_block)

                    if is_self_closing:
                        logging.info("self-closing docbook '%s'", tag)
                        md_block = {'type': "self-closing tag",
                                    'text': deindented_line,
                                    }
                        is_self_closing = 0
                        continue

                    logging.info("new markup '%s'", tag)
                    md_block = {'type': "markup",
                                'text': deindented_line,
                                'start': '<' + tag + '>',
                                'end': '</' + tag + '>',
                                'depth': 0,
                                }
                    if re.search(r'<\/%s>' % tag, deindented_line):
                        md_block["closed"] = 1

                    continue
                else:
                    if tag in MD_TEXT_LEVEL_ELEMENTS:
                        logging.info("text level docbook '%s' in '%s' state", tag, md_block["type"])
                        # TODO(ensonic): handle nesting
                        if not scanning_for_end_of_text_level_tag:
                            if not re.search(r'<\/%s>' % tag, deindented_line):
                                logging.info("new text level markup '%s'", tag)
                                md_block["start"] = '<' + tag + '>'
                                md_block["end"] = '</' + tag + '>'
                                md_block.pop("closed", None)
                                logging.info("scanning for end of '%s'", tag)

                        else:
                            if md_block["end"] in deindented_line:
                                md_block["closed"] = 1
                                logging.info("found end of '%s'", tag)
        elif li_match:
            # li
            md_blocks.append(md_block)
            indentation = li_match.group(1)
            md_block = {'type': "li",
                        'ordered': 0,
                        'indentation': indentation,
                        'marker': "[*+-]",
                        'first': 1,
                        'last': 1,
                        'lines': [re.sub(r'^[ ]{0,4}', '', li_match.group(2))],
                        }
            continue
        elif quote_match:
            md_blocks.append(md_block)
            md_block = {'type': "quote",
                        'lines': [quote_match.group(1)],
                        }
            continue

        # list item
        list_item_match = re.search(r'^([ ]{0,4})\d+[.][ ]+(.*)', line)
        if list_item_match:
            md_blocks.append(md_block)
            indentation = list_item_match.group(1)
            md_block = {'type': "li",
                        'ordered': 1,
                        'indentation': indentation,
                        'marker': "\\d+[.]",
                        'first': 1,
                        'last': 1,
                        'lines': [re.sub(r'^[ ]{0,4}', '', list_item_match.group(2))],
                        }
            continue

        # paragraph
        if md_block["type"] == "paragraph":
            if "interrupted" in md_block:
                md_blocks.append(md_block)
                md_block = {'type': "paragraph",
                            'text': line,
                            }
                logging.info("new paragraph due to interrupted")
            else:
                md_block["text"] += "\n" + line
                logging.info("add to paragraph: '%s'", line)

        else:
            md_blocks.append(md_block)
            md_block = {'type': "paragraph",
                        'text': line,
                        }
            logging.info("new paragraph due to different block type")

    md_blocks.append(md_block)
    md_blocks.pop(0)

    return md_blocks


def MarkDownParseSpanElementsInner(text, markersref):
    markup = ''
    markers = {i: 1 for i in markersref}

    while text != '':
        closest_marker = ''
        closest_marker_position = -1
        text_marker = ''
        offset = 0
        markers_rest = []

        for marker, use in markers.items():
            if not use:
                continue

            marker_position = text.find(marker)

            if marker_position < 0:
                markers[marker] = 0
                continue

            if closest_marker == '' or marker_position < closest_marker_position:
                closest_marker = marker
                closest_marker_position = marker_position

        if closest_marker_position >= 0:
            text_marker = text[closest_marker_position:]

        if text_marker == '':
            markup += text
            text = ''
            continue

        markup += text[:closest_marker_position]
        text = text[closest_marker_position:]
        markers_rest = {k: v for k, v in markers.items() if v and k != closest_marker}

        if closest_marker == '![' or closest_marker == '[':
            # 'id-ref' : local id reference
            # 'title'  : link short description/alt-text/tooltip
            # 'a'      : linked text
            # 'href'   : external link
            # 'is-media': is link to media object
            element = None

            # FIXME: '(?R)' is a recursive subpattern
            # match a [...] block with no ][ inside or this thing again
            # m = re.search(r'\[((?:[^][]|(?R))*)\]', text)
            m = re.search(r'\[((?:[^][])*)\]', text)
            if ']' in text and m:
                element = {'is-media': text[0] == '!',
                           'a': EscapeEntities(m.group(1)),
                           }

                offset = len(m.group(0))
                if element['is-media']:
                    offset += 1
                logging.debug("Recursive md-expr match: off=%d, text='%s', match='%s'", offset, text, m.group(1))

                remaining_text = text[offset:]
                # (link "alt-text")
                m2 = re.search(r'''^\([ ]*([^)'"]*?)(?:[ ]+['"](.+?)['"])?[ ]*\)''', remaining_text)
                # [id-reference]
                m3 = re.search(r'^\s*\[([^\]<]*?)\]', remaining_text)
                if m2:
                    element['href'] = m2.group(1)
                    if m2.group(2):
                        element['title'] = m2.group(2)
                    offset += len(m2.group(0))
                elif m3:
                    element['id-ref'] = m3.group(1)
                    offset += len(m3.group(0))
                else:
                    element = None

            if element:
                logging.debug("output link for", element)

                if 'href' in element:
                    element['href'] = EscapeEntities(element['href'])

                if element['is-media']:
                    # media link
                    markup += '<inlinemediaobject><imageobject><imagedata fileref="' + \
                        element['href'] + '"></imagedata></imageobject>'

                    if 'a' in element:
                        markup += "<textobject><phrase>" + element['a'] + "</phrase></textobject>"

                    markup += "</inlinemediaobject>"
                elif 'id-ref' in element:
                    # internal link
                    element['a'] = MarkDownParseSpanElementsInner(element['a'], markers_rest)
                    markup += '<link linkend="' + element['id-ref'] + '"'

                    if 'title' in element:
                        # title attribute not supported
                        pass

                    markup += '>' + element['a'] + "</link>"
                else:
                    # external link
                    element['a'] = MarkDownParseSpanElementsInner(element['a'], markers_rest)
                    markup += '<ulink url="' + element['href'] + '"'

                    if 'title' in element:
                        # title attribute not supported
                        pass

                    markup += '>' + element['a'] + "</ulink>"

            else:
                markup += closest_marker
                if closest_marker == '![':
                    offset = 2
                else:
                    offset = 1

        elif closest_marker == '<':
            m4 = re.search(r'^<(https?:[\/]{2}[^\s]+?)>', text, flags=re.I)
            m5 = re.search(r'^<([A-Za-z0-9._-]+?@[A-Za-z0-9._-]+?)>', text)
            m6 = re.search(r'^<[^>]+?>', text)
            if m4:
                element_url = EscapeEntities(m4.group(1))

                markup += '<ulink url="' + element_url + '">' + element_url + '</ulink>'
                offset = len(m4.group(0))
            elif m5:
                markup += "<ulink url=\"mailto:" + m5.group(1) + "\">" + m5.group(1) + "</ulink>"
                offset = len(m5.group(0))
            elif m6:
                markup += m6.group(0)
                offset = len(m6.group(0))
            else:
                markup += "&lt;"
                offset = 1

        elif closest_marker == "\\":
            special_char = ''
            if len(text) > 1:
                special_char = text[1]
            if special_char in MD_ESCAPABLE_CHARS or special_char in MD_GTK_ESCAPABLE_CHARS:
                markup += special_char
                offset = 2
            else:
                markup += "\\"
                offset = 1

        elif closest_marker == "`":
            m7 = re.search(r'^(`+)([^`]+?)\1(?!`)', text)
            if m7:
                element_text = EscapeEntities(m7.group(2))
                markup += "<literal>" + element_text + "</literal>"
                offset = len(m7.group(0))
            else:
                markup += "`"
                offset = 1

        elif closest_marker == "@":
            # Convert '@param()'
            # FIXME: we could make those also links ($symbol.$2), but that would be less
            # useful as the link target is a few lines up or down
            m7 = re.search(r'^(\A|[^\\])\@(\w+((\.|->)\w+)*)\s*\(\)', text)
            m8 = re.search(r'^(\A|[^\\])\@(\w+((\.|->)\w+)*)', text)
            m9 = re.search(r'^\\\@', text)
            if m7:
                markup += m7.group(1) + "<parameter>" + m7.group(2) + "()</parameter>\n"
                offset = len(m7.group(0))
            elif m8:
                # Convert '@param', but not '\@param'.
                markup += m8.group(1) + "<parameter>" + m8.group(2) + "</parameter>\n"
                offset = len(m8.group(0))
            elif m9:
                markup += r"\@"
                offset = len(m9.group(0))
            else:
                markup += "@"
                offset = 1

        elif closest_marker == '#':
            m10 = re.search(r'^(\A|[^\\])#([\w\-:\.]+[\w]+)\s*\(\)', text)
            m11 = re.search(r'^(\A|[^\\])#([\w\-:\.]+[\w]+)', text)
            m12 = re.search(r'^\\#', text)
            if m10:
                # handle #Object.func()
                markup += m10.group(1) + MakeXRef(m10.group(2), tagify(m10.group(2) + "()", "function"))
                offset = len(m10.group(0))
            elif m11:
                # Convert '#symbol', but not '\#symbol'.
                markup += m11.group(1) + MakeHashXRef(m11.group(2), "type")
                offset = len(m11.group(0))
            elif m12:
                markup += '#'
                offset = len(m12.group(0))
            else:
                markup += '#'
                offset = 1

        elif closest_marker == "%":
            m12 = re.search(r'^(\A|[^\\])\%(-?\w+)', text)
            m13 = re.search(r'^\\%', text)
            if m12:
                # Convert '%constant', but not '\%constant'.
                # Also allow negative numbers, e.g. %-1.
                markup += m12.group(1) + MakeXRef(m12.group(2), tagify(m12.group(2), "literal"))
                offset = len(m12.group(0))
            elif m13:
                markup += r"\%"
                offset = len(m13.group(0))
            else:
                markup += "%"
                offset = 1

        if offset > 0:
            text = text[offset:]

    return markup


def MarkDownParseSpanElements(text):
    markers = ["\\", '<', '![', '[', "`", '%', '#', '@']

    text = MarkDownParseSpanElementsInner(text, markers)

    # Convert 'function()' or 'macro()'.
    # if there is abc_*_def() we don't want to make a link to _def()
    # FIXME: also handle abc(def(....)) : but that would need to be done recursively :/
    def f(m):
        return m.group(1) + MakeXRef(m.group(2), tagify(m.group(2) + "()", "function"))
    text = re.sub(r'([^\*.\w])(\w+)\s*\(\)', f, text)
    return text


def EscapeEntities(text):
    return text.replace('&', '&amp;').replace('<', '&lt;').replace('>', '&gt;')


def ReplaceEntities(text):
    entities = [["&lt;", '<'],
                ["&gt;", '>'],
                ["&ast;", '*'],
                ["&num;", '#'],
                ["&percnt;", '%'],
                ["&colon;", ':'],
                ["&quot;", '"'],
                ["&apos;", "'"],
                ["&nbsp;", ' '],
                ["&amp;", '&'],  # Do this last, or the others get messed up.
                ]

    for i in entities:
        text = re.sub(i[0], i[1], text)
    return text


def MarkDownOutputDocBook(blocksref, symbol, context):
    output = ''
    blocks = blocksref

    for block in blocks:
        # $output += "\n<!-- beg type='" . $block->{"type"} . "'-->\n"

        if block["type"] == "paragraph":
            text = MarkDownParseSpanElements(block["text"])
            if context == "li" and output == '':
                if 'interrupted' in block:
                    output += "\n<para>%s</para>\n" % text
                else:
                    output += "<para>%s</para>" % text
                    if len(blocks) > 1:
                        output += "\n"
            else:
                output += "<para>%s</para>\n" % text

        elif block["type"] == "heading":

            title = MarkDownParseSpanElements(block["text"])

            if block["level"] == 1:
                tag = "refsect2"
            else:
                tag = "refsect3"

            text = MarkDownParseLines(block["lines"], symbol, "heading")
            if 'id' in block:
                output += "<%s id=\"%s\">" % (tag, block["id"])
            else:
                output += "<%s>" % tag

            output += "<title>%s</title>%s</%s>\n" % (title, text, tag)
        elif block["type"] == "li":
            tag = "itemizedlist"

            if "first" in block:
                if block["ordered"]:
                    tag = "orderedlist"
                output += "<%s>\n" % tag

            if "interrupted" in block:
                block["lines"].append('')

            text = MarkDownParseLines(block["lines"], symbol, "li")
            output += "<listitem>" + text + "</listitem>\n"
            if 'last' in block:
                if block["ordered"]:
                    tag = "orderedlist"
                output += "</%s>\n" % tag

        elif block["type"] == "quote":
            text = MarkDownParseLines(block["lines"], symbol, "quote")
            output += "<blockquote>\n%s</blockquote>\n" % text
        elif block["type"] == "code":
            tag = "programlisting"

            if "language" in block:
                if block["language"] == "plain":
                    output += "<informalexample><screen><![CDATA[\n"
                    tag = "screen"
                else:
                    output += "<informalexample><programlisting role=\"example\" language=\"%s\"><![CDATA[\n" % block['language']
            else:
                output += "<informalexample><programlisting role=\"example\"><![CDATA[\n"

            logging.debug('listing for %s: [%s]', symbol, '\n'.join(block['lines']))
            for line in block["lines"]:
                output += ReplaceEntities(line) + "\n"

            output += "]]></%s></informalexample>\n" % tag
        elif block["type"] == "markup":
            text = ExpandAbbreviations(symbol, block["text"])
            output += text + "\n"
        else:
            output += block["text"] + "\n"

        # $output += "\n<!-- end type='" . $block->{"type"} . "'-->\n"
    return output


def MarkDownParseLines(lines, symbol, context):
    logging.info('md parse: ctx=%s, [%s]', context, '\n'.join(lines))
    blocks = MarkDownParseBlocks(lines, symbol, context)
    output = MarkDownOutputDocBook(blocks, symbol, context)
    return output


def MarkDownParse(text, symbol):
    """Converts mark down syntax to the respective docbook.

    http://de.wikipedia.org/wiki/Markdown
    Inspired by the design of ParseDown
    http://parsedown.org/
    Copyright (c) 2013 Emanuil Rusev, erusev.com

    SUPPORTED MARKDOWN
    ==================

    Atx-style Headers
    -----------------

    # Header 1

    ## Header 2 ##

    Setext-style Headers
    --------------------

    Header 1
    ========

    Header 2
    --------

    Ordered (unnested) Lists
    ------------------------

    1. item 1

    1. item 2 with loooong
       description

    3. item 3

    Note: we require a blank line above the list items
    """
    # TODO(ensonic): it would be nice to add id parameters to the refsect2 elements

    return MarkDownParseLines(text.splitlines(), symbol, '')
