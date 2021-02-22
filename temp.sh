#!/bin/bash
# records the temp every 10 seconds

if [ -f /boot/RADIATION/COLLECTDATA ]; then
	sleep 60 
	FILENAME=/media/noobs/RADIATION/results/$(date +%F.%H-%M-%S)-temp.log

	while [ true ]
	do
		echo -n $(date +%F,%T,) >> $FILENAME
		/opt/vc/bin/vcgencmd measure_temp >> $FILENAME
		sleep 10
	done
fi
