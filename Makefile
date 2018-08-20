test:
	mkdir -p build
	cd build && cmake ..
	cd build && make signupeoseos_unit_tests
	cd build && ./tests/unit_test -p -- --verbose
