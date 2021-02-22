#!/bin/bash
# checks 500 MBs of memory

LED_PATH=/sys/class/leds/led0/

if [ -f /boot/RADIATION/COLLECTDATA ]; then
	sleep 60

	FILENAME=/media/noobs/RADIATION/results/$(date +%F.%H-%M-%S)-mem.log
	echo heartbeat > $LED_PATH/trigger
	echo -e "STARTING MEMORY TEST AT $(date)\n\n" >> $FILENAME
	memtester 800 1 2>&1 >> $FILENAME
	echo 0 > $LED_PATH/brightness
fi
