#!/bin/bash
# gathers device data every 10 seconds.

_ID = "XXXXXX"
FILENAME = /home/ubuntu/results/XXXXX.txt
USER = "ubuntu"
HOST = 192.168.1.1
DESTINATION = "/home/ubuntu/spaceraddata/XXXXXX-stats.log"

while [ true ]; do
  echo -n "$_ID" >> $FILENAME
  echo -n ", " >> $FILENAME
  echo -n $(date + %s) >> $FILENAME
  echo -n ", " >> $FILENAME
  ./home/ubuntu/scripts/diagnostic >> $FILENAME
  echo -n ", " >> $FILENAME
  /opt/vc/bin/vcgencmd measure_temp >> $FILENAME
  echo -n ", " >> $FILENAME
  /opt/vc/bin/vcgencmd measure_volts >> $FILENAME
  echo -n "\n" >> $FILENAME

  # send the updated FILENAME over SCP to the monitor PC
  scp $FILENAME ($USER)@($SERVER):($DESTINATION)

  sleep 10
done
