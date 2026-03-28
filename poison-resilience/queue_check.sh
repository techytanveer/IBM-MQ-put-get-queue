#!/bin/bash
while true; do
  echo "-------------- TEST QUEUE ---------------"
  echo " "
  echo "DISPLAY QLOCAL('TEST.QUEUE') CURDEPTH" | runmqsc QM1
  echo " "
  echo "-------------- DEAD LETTER QUEUE --------"
  echo " "
  echo "DISPLAY QLOCAL('DEV.DEAD.LETTER.QUEUE') CURDEPTH" | runmqsc QM1
  echo " "
  echo "========================================="
  echo " "
  sleep 5
done
