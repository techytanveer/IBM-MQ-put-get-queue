export LD_LIBRARY_PATH=/opt/mqm/lib64
crtmqm QM1
strmqm QM1
echo "DEFINE QLOCAL('TEST.QUEUE') REPLACE" | runmqsc QM1
echo "DEFINE QLOCAL('DEV.DEAD.LETTER.QUEUE') REPLACE" | runmqsc QM1
