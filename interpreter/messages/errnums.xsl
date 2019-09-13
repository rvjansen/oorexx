<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output  omit-xml-declaration="yes" method="xml" indent="yes"/>
<xsl:preserve-space elements="para"/>
<xsl:template match="Messages">
<section id="errorlist"><title>Error List</title>

<xsl:for-each select="Message[Code &lt; 101]">
<xsl:sort select="Code" data-type="number"/>
<xsl:text>
</xsl:text>
<xsl:element name="section"> <xsl:attribute name="id">ERR<xsl:value-of select="Code"/></xsl:attribute>
<title>Error <xsl:value-of select="Code"/> - <xsl:apply-templates select="Text"/></title>
<xsl:apply-templates select="Explanation"/>
<xsl:if test="Subcodes">
<xsl:text>
</xsl:text>
<para>The associated subcodes are: </para>
<xsl:text>
</xsl:text>
<variablelist>
<xsl:for-each select="Subcodes/SubMessage">
<xsl:sort select="Subcode" data-type="number"/>
<xsl:text>
</xsl:text>
<varlistentry><term><xsl:value-of select="Subcode"/></term>
<listitem>
<para><xsl:apply-templates select="Text"/></para>
</listitem>
</varlistentry>
</xsl:for-each>
</variablelist>
</xsl:if>
<xsl:text>
</xsl:text>
</xsl:element>
</xsl:for-each>
<xsl:text>
</xsl:text>
</section>
</xsl:template>


<xsl:template match="Text">
    <xsl:apply-templates/>
</xsl:template>

<xsl:template match="q">
<xsl:text>&quot;</xsl:text><xsl:apply-templates/><xsl:text>&quot;</xsl:text>
</xsl:template>

<xsl:template match="sq">
<xsl:text>&apos;</xsl:text>
</xsl:template>

<xsl:template match="dq">
<xsl:text>&quot;</xsl:text>
</xsl:template>

<xsl:template match="Sub">
<emphasis role="italic"><xsl:value-of select="@name"/></emphasis>
</xsl:template>

<xsl:template match="Explanation">
<xsl:text>
</xsl:text>
<para><emphasis role="bold">Explanation:</emphasis></para>
<xsl:text>
</xsl:text>
<xsl:copy-of select="*"/>
</xsl:template>

</xsl:stylesheet>
