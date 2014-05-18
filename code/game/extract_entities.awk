/\{\ \"models/ {
if(model) {
	print "-------- MODEL FOR RADIANT ONLY - DO NOT SET THIS AS A KEY --------";
	print "model=" substr($2,1,length($2)-1);
	model = 0;
}
};
/\/\*QUAKED/ {
blk=1;
model=1
if (mustend) {
	endhere = 1
}
};
/\/\*QUAKED item_\*\*/ {
blk=0
model=0
endhere=0
}
/\*\// {
if(blk) {
	mustend=1;
}
blk=0;
}
{
if (blk && endhere) {
	endhere = 0
	print "*/"
}
}
{ if(blk) print $0}; 

END  { print "*/"}
