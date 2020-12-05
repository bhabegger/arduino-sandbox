PORT=/dev/ttyUSB0

upload: HttpServerIndex.h
	mkdir -p build
	arduino -v ESP32-HtttpServer.ino --pref build.path=build --upload --board esp32:esp32:esp32cam --port ${PORT}

HttpServerIndex.h: www/index.min.html
	echo -n "#define INDEX_HTML \"" > $@
	< $< perl -pe 's/"/\\"/g;s/\n/\\n/gs' >> $@
	echo "\"" >> $@


%.min.html : %.html
	htmlmin $< > $@
