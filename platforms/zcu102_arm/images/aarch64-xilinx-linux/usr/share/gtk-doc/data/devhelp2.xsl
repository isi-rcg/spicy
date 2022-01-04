<?xml version='1.0'?> <!--*- mode: xml -*-->
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version='1.0'
                xmlns="http://www.devhelp.net/book"
                xmlns:str="http://exslt.org/strings"
                exclude-result-prefixes="str">

  <xsl:template name="generate.devhelp2">
    <xsl:call-template name="write.chunk">
      <xsl:with-param name="filename">
        <xsl:choose>
          <xsl:when test="$gtkdoc.bookname">
            <xsl:value-of select="$gtkdoc.bookname"/>
          </xsl:when>
          <xsl:otherwise>
            <xsl:text>book</xsl:text>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:text>.devhelp2</xsl:text>
      </xsl:with-param>
      <xsl:with-param name="method" select="'xml'"/>
      <xsl:with-param name="indent" select="'yes'"/>
      <xsl:with-param name="encoding" select="'utf-8'"/>
      <xsl:with-param name="doctype-public"/>
      <xsl:with-param name="content">
        <xsl:call-template name="devhelp2"/>
      </xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <xsl:variable name="gtkdoc.refsect2" select="//refsect2"/>

  <xsl:template name="devhelp2">
    <xsl:variable name="title">
      <xsl:apply-templates select="." mode="generate.devhelp2.toc.title.mode"/>
    </xsl:variable>
    <xsl:variable name="link">
      <xsl:call-template name="href.target"/>
    </xsl:variable>
    <xsl:variable name="author">
      <xsl:if test="articleinfo|bookinfo">
        <xsl:apply-templates mode="generate.devhelp2.authors"
                             select="articleinfo|bookinfo"/>
      </xsl:if>
    </xsl:variable>
    <xsl:variable name="online">
      <xsl:value-of select="/book/bookinfo/releaseinfo/ulink[@role='online-location']/@url"/>
    </xsl:variable>
    <xsl:variable name="toc.nodes" select="part|reference|preface|chapter|
                                           appendix|article|bibliography|
                                           glossary|index|refentry|
                                           bridgehead|sect1"/>

    <book title="{$title}" link="{$link}" author="{$author}" name="{$gtkdoc.bookname}" version="2" language="c">
      <xsl:if test="$online != ''">
        <xsl:attribute name="online"><xsl:value-of select="$online"/></xsl:attribute>
      </xsl:if>
      <xsl:if test="$toc.nodes">
        <chapters>                                    
          <xsl:apply-templates select="$toc.nodes"
                               mode="generate.devhelp2.toc.mode"/>
        </chapters>
      </xsl:if>
      <functions>
        <xsl:apply-templates select="$gtkdoc.refsect2[@role]"
                             mode="generate.devhelp2.index.mode"/>
        <xsl:apply-templates select="$gtkdoc.refsect2/refsect3[@role='enum_members']/informaltable/tgroup/tbody/row[@role='constant']"
                             mode="generate.devhelp2.index.mode"/>
        <xsl:apply-templates select="$gtkdoc.refsect2/refsect3[@role='struct_members']/informaltable/tgroup/tbody/row[@role='member']"
                             mode="generate.devhelp2.index.mode"/>
      </functions>
    </book>
  </xsl:template>

  <xsl:template match="*" mode="generate.devhelp2.toc.mode">
    <xsl:variable name="title">
      <xsl:apply-templates select="." mode="generate.devhelp2.toc.title.mode"/>
    </xsl:variable>
    <xsl:variable name="target">
      <xsl:variable name="anchor" select="title/anchor"/>
      <xsl:choose>
        <xsl:when test="$anchor">
          <xsl:call-template name="href.target">
            <xsl:with-param name="object" select="$anchor"/>
          </xsl:call-template>
        </xsl:when>
        <xsl:otherwise>
          <xsl:call-template name="href.target"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>

    <sub name="{$title}" link="{$target}">
      <xsl:apply-templates select="section|sect1|
                                   refentry|refsect|
                                   bridgehead|part|chapter|glossary|index"
                           mode="generate.devhelp2.toc.mode"/>
    </sub>
  </xsl:template>

  <xsl:template match="*" mode="generate.devhelp2.index.mode">
    <xsl:variable name="title" select="title|
                                       term/literal|
                                       entry[@role='enum_member_name']/para|
                                       entry[@role='struct_member_name']/para/structfield/@id"/>
    <xsl:variable name="anchor" select="title/anchor|
                                        entry[@role='enum_member_name']/para|
                                        entry[@role='struct_member_name']/para/structfield"/>
    <xsl:variable name="type" select="@role"/>
    <xsl:variable name="condition" select="@condition"/>
    <xsl:variable name="target">
      <xsl:choose>
        <xsl:when test="$anchor">
          <xsl:call-template name="href.target">
            <xsl:with-param name="object" select="$anchor"/>
          </xsl:call-template>
        </xsl:when>
        <xsl:otherwise>
          <xsl:call-template name="href.target"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <keyword type="{$type}" name="{$title}" link="{$target}">
      <xsl:if test="$condition">
        <xsl:for-each select="str:split($condition,'|')">
          <xsl:variable name="attrname">
            <xsl:value-of select="substring-before(.,':')"/>
          </xsl:variable>
          <xsl:choose>
            <xsl:when test="string-length($attrname)=0">
              <xsl:variable name="attrname2">
                <xsl:value-of select="."/>
              </xsl:variable>
              <xsl:attribute name="{$attrname2}"/>
            </xsl:when>
            <xsl:otherwise>
              <xsl:attribute name="{$attrname}">
                <xsl:value-of select="substring-after(.,':')"/>
              </xsl:attribute>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:for-each>
     </xsl:if>
    </keyword>
  </xsl:template>

  <!-- get title -->
  <xsl:template match="article" mode="generate.devhelp2.toc.title.mode">
    <xsl:value-of select="articleinfo/title"/>
  </xsl:template>
  <xsl:template match="book" mode="generate.devhelp2.toc.title.mode">
    <xsl:value-of select="bookinfo/title"/>
  </xsl:template>
  <xsl:template match="refentry" mode="generate.devhelp2.toc.title.mode">
    <xsl:value-of select="refmeta/refentrytitle"/>
  </xsl:template>
  <xsl:template match="*" mode="generate.devhelp2.toc.title.mode">
    <xsl:value-of select="title"/>
  </xsl:template>

  <!-- generate list of authors ... -->
  <xsl:template match="articleinfo|bookinfo" mode="generate.devhelp2.authors">
    <xsl:for-each select="authorgroup/author">
      <xsl:value-of select="firstname"/>
      <xsl:text> </xsl:text>
      <xsl:value-of select="surname"/>
      <xsl:if test="not(last())">
        <xsl:text>, </xsl:text>
      </xsl:if>
    </xsl:for-each>
  </xsl:template>

</xsl:stylesheet>
