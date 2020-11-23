PORT=/dev/ttyUSB0
upload:
	mkdir -p build
	arduino -v ESP32-HtttpServer.ino --pref build.path=build --upload --board esp32:esp32:esp32cam --port $PORT
