PORT=/dev/ttyUSB0

upload:
	fuser -k ${PORT} || true
	mkdir -p build
	arduino RobotComplete.ino --pref build.path=build --upload --board arduino:avr:uno --port ${PORT}
