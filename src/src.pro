!include(../config.pri):error("Could not find config.pri")

addSubdir(mfu-common)

CONFIG(server) {
message("Building server")
addSubdir(database)
addSubdir(mfu-daemon, mfu-common database)
}

CONFIG(clients) {
message("Building clients")
addSubdir(mfu-client-qml, mfu-common)
}
