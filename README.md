# OpenArena gamecode #

## Description ##
This is the game code part of OpenArena. In mod form it is referred as OpenArenaExpanded (OAX).

## Building ##

See http://openarena.wikia.com/wiki/OpenArena_eXpanded for detailed build instructions

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

