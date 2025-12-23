#The model line is the line that contains the path to the model-object that should be used in the editor. This is matched in the actual code.
#Only the first model after a QUAKED-comment should be included this is the reason 
/\{\ \"models/ {
if(model) {
	print "-------- MODEL FOR RADIANT ONLY - DO NOT SET THIS AS A KEY --------";
	print "model=" substr($2,1,length($2)-1);
	model = 0; #Do not match the next model
}
};
#We match /*QUAKED as the comment blocks we must include
/\/\*QUAKED/ {
blk=1;  #While this is 1 we print
model=1 #Include the first model found
if (mustend) {
	endhere = 1
}
};
#We make a special case for the example line in the bg_items.c-file by mathcing "/*QUAKED item_*" where the star is an actual star not a wildcard
/\/\*QUAKED item_\*\*/ {
blk=0
model=0
endhere=0
}
#We match "*/" (end comment)
#In the source files */ are always placed on there own line for QUAKED-comments
/\*\// {
if(blk) {
	#if this line ended the 
	mustend=1;
}
blk=0;
}
{
#If this is not the first entry we must terminate the old one
if (blk && endhere) {
	endhere = 0
	print "*/"
}
}
#As long as blk just echo everything to screen
{ if(blk) print $0}; 

#In the end we must end with a comment end. This would normally have been triggered by the next begin.
END  { print "*/"}
