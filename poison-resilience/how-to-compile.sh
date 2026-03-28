# The producer
g++ -I./include main.cpp -L/opt/mqm/lib64 -lmqm_r -o ibmMQ
# The Consumer
g++ -I./include get.cpp -L/opt/mqm/lib64 -lmqm_r -o mq_get
# The poison message injector
g++ -I/opt/mqm/inc mq_poison.cpp -L/opt/mqm/lib64 -lmqm_r -o mq_poison
# Clear DLQ
g++ -I/opt/mqm/inc mq_clear_dlq.cpp -L/opt/mqm/lib64 -lmqm_r -o mq_clear_dlq
