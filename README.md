# Music Follows You

The idea behind this project is to have a server and some clients. The server will be able to stream the music/audio to the clients. The clients do not know much about themself,
the intelligence is located in the server. The server does know which client is located in which room. So if a user is moving from one room to another the mfu-daemon redirects
(fades?) music from the current room to the new one. This toolset should be combined with an indoor location detection so the whole thing would be automated.

This system should work with playlists, and multiple users that could walk between different rooms.

## Build instructions:

### Server
create local.pri with CONFIG += server and run qmake make combination to build the source

### Clients
create local.pri with CONFIG += clients and run qmake make combination to build the source

## Starting:
To start it from a shadowed build you need to preload the libraries: `export LD_LIBRARY_PATH=../lib/:../modules/rpclib/lib/`
