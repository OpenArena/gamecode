<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
<xsl:output method="text"/>

<xsl:template match="/">
<xsl:text>switch( headid ) {
</xsl:text>
<xsl:for-each select="Challenges/Group">
case <xsl:value-of select = "Id"/>: 
//<xsl:value-of select = "Name"/><xsl:text>
</xsl:text>
<xsl:for-each select="Challenge[not(HideInMenu)]">
<xsl:text>challenges.entry[</xsl:text>
<xsl:value-of select = "position()-1"/>
<xsl:text>].string = "</xsl:text>
<xsl:value-of select = "Name"/>
<xsl:text>:";
challenges.entryInt[</xsl:text>
<xsl:value-of select = "position()-1"/>
<xsl:text>] = getChallenge(</xsl:text>
<xsl:choose>
	<xsl:when test="DefineName">
		<xsl:value-of select = "../DefineName"/><xsl:value-of select = "DefineName"/>
	</xsl:when>
	<xsl:otherwise>
		<xsl:value-of select = "AltDefineName"/>
	</xsl:otherwise>
</xsl:choose>
<xsl:text>);
</xsl:text>
<xsl:if test="position()=last()">
challenges.numberOfEntries = <xsl:value-of select = "last()"/>;
break;
</xsl:if>
</xsl:for-each>

</xsl:for-each>
default:
challenges.numberOfEntries = 0;
};
</xsl:template>
</xsl:stylesheet>
