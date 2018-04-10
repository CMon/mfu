!include(config.pri):error("Could not find config.pri")

addSubdir(modules/rpclib)
addSubdir(src)

OTHER_FILES = \
    .qmake.conf \
    local.pri.example \
    README.md \
