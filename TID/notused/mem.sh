#!/bin/bash
# checks 500 MBs of memory

#Doesn't actually push to remote server.

sleep 60

FILENAME=/home/ubuntu/results/dev1-mem.log

echo -e "STARTING MEMORY TEST AT $(date)\n\n" >> $FILENAME
memtester 800 1 2>&1 >> $FILENAME
