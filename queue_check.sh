#!/bin/bash
while true; do
  echo "DISPLAY QLOCAL('TEST.QUEUE') CURDEPTH" | runmqsc QM1
  sleep 2
done
