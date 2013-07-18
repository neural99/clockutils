all: stopwatch eggclock

eggclock: eggclock.8
	8l -o 8.eggclock eggclock.8

eggclock.8:
	8c eggclock.c

stopwatch: stopwatch.8
	8l -o 8.stopwatch stopwatch.8

stopwatch.8:
	8c stopwatch.c

clean:
	rm *.8 && rm 8.*
