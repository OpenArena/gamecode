# OpenArena gamecode 
[![Build Status](https://travis-ci.org/OpenArena/gamecode.svg?branch=master)](https://travis-ci.org/OpenArena/gamecode)

## Description ##
This is the game code part of OpenArena. In mod form it is referred as OpenArenaExpanded (OAX).

## Building ##

You need a C-compiler (tested with gcc and clang) and GNU make then just type 
```
make 
```
and the qvm-files will be build. Ready to be packed into a pk3-file. 

See https://github.com/OpenArena/gamecode/wiki/Build-instruction for more details.

See http://openarena.wikia.com/wiki/OpenArena_eXpanded for alternative build options

## Extracting entities ##
It is possible to extract entity definition for use with GtkRadiant 1.6 like this:

```
cd code/game
./extract_entities.sh > openarena.def
```

Note that these entities will not work with GtkRadiant 1.5 and NetRadiant 1.5 because they use a XML-based format (although there are not that much XML information in it)

## Links ##
Development documentation is located here: https://github.com/OpenArena/gamecode/wiki

The development board on the OpenArena forum: http://openarena.ws/board/index.php?board=30.0

In particular the Open Arena Expanded topic: http://openarena.ws/board/index.php?topic=1908.0

