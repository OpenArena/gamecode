#! /usr/bin/pyhton3
import sys

filename = sys.argv[1]

#print("File: "+filename)

quaked=""
description=""
model=""
item=""
blk=False
started=False

def printLine():
    global started
    if (started):
        print(item+","+quaked+",\""+description.strip().replace("\"","\"\"")+"\","+model)
        started=False


with open(filename) as fp:
    for line in fp:
        if "{ \"models/" in line:
            model=line.split('"')[1]
        if "*/" in line:
            blk= False
        elif blk:
            description+=line
        if "/*QUAKED" in line and not "/*QUAKED item_*" in line:
            printLine()
            quaked=line.strip()
            description=""
            item = quaked.split(" ")[1]
            blk = True
            started=True
printLine()
