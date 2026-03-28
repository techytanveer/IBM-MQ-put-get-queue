[![IBM MQ C++ CI Pipeline](https://github.com/techytanveer/IBM-MQ-put-get-queue/actions/workflows/pipeline.yml/badge.svg)](https://github.com/techytanveer/IBM-MQ-put-get-queue/actions/workflows/pipeline.yml)

# IBM MQ C++ Put/Get Queue

A high-performance C++ implementation for interacting with IBM MQ. This project demonstrates how to connect to a Queue Manager, put a message into a local queue, and retrieve it using a separate consumer.



## Features
* **Producer (`ibmMQ`):** Connects to `QM1` and puts a test message into `TEST.QUEUE`.
* **Consumer (`mq_get`):** Connects to `QM1` and retrieves the message.
* **Poison Injector (`mq_poison`):** Inject a poison/failed message into 'TEST.QUEUE`.
* **DLQ Janitor (`mq_clear_dlq`):** Connects to `DEAD.LETTER.QUEUE`, read every message until the queue is empty.
* **Automated CI/CD:** Integrated with GitHub Actions to compile and test against a live IBM MQ container.

---

## Prerequisites

To build and run this locally, what is needed:
1.  **IBM MQ Client/Server:** Installed in `/opt/mqm`.
2.  **C++ Compiler:** `gcc-c++`.
3.  **Library Access:** `LD_LIBRARY_PATH` must include the MQ `lib64` directory.

---

## Project Structure

* `main.cpp` - The Producer application (Put).
* `get.cpp` - The Consumer application (Get).
* `include/` - Vendored IBM MQ headers (`cmqc.h`, etc.) for portable builds.
* `.github/workflows/` - CI pipeline configuration.

---

## Environment Setup (Local)

```
Download the Ubuntu .deb tarball from the IBM MQ download page.
tar -zxvf ibm-mq-advanced-for-developers-9.4.x.tar.gz
cd MQServer
sudo ./mqlicense.sh -text_only
# Add the directory to apt to handle dependencies automatically
sudo apt install ./ibmmq-runtime_*.deb ./ibmmq-server_*.deb ./ibmmq-sdk_*.deb ./ibmmq-client_*.deb
sudo usermod -aG mqm $USER
crtmqm QM1
strmqm QM1
echo "DEFINE QLOCAL(DEV.QUEUE.1) REPLACE" | runmqsc QM1
echo "DEFINE QLOCAL('DEV.DEAD.LETTER.QUEUE') REPLACE" | runmqsc QM1
```

## Build Instructions

Use the following commands to compile the applications. We include the `./include` directory to ensure the compiler finds the necessary MQ headers.

```bash
# Compile Producer
g++ -I./include main.cpp -L/opt/mqm/lib64 -lmqm_r -o ibmMQ

# Compile Consumer
g++ -I./include get.cpp -L/opt/mqm/lib64 -lmqm_r -o mq_get

# Compile the poison message injector
g++ -I./include mq_poison.cpp -L/opt/mqm/lib64 -lmqm_r -o mq_poison

# Compile DLQ Janitor
g++ -I./include mq_clear_dlq.cpp -L/opt/mqm/lib64 -lmqm_r -o mq_clear_dlq
```

## Local Execution
Before running, ensuring the environment is configured and a Queue Manager is running.

Set Environment Variables:

```
export LD_LIBRARY_PATH=/opt/mqm/lib64
Setup Queue Manager (Local Server):


crtmqm QM1
strmqm QM1

echo "DEFINE QLOCAL('TEST.QUEUE') REPLACE" | runmqsc QM1
echo "DEFINE QLOCAL('DEV.DEAD.LETTER.QUEUE') REPLACE" | runmqsc QM1

Run the Demo:


./ibmMQ        # Puts the message
./mq_get       # Gets the message
./mq_poison    # Inject the poison
./mq_clear_dlq # Clear the DLQ
```

## CI/CD Pipeline
This project uses GitHub Actions to maintain code quality. Every push triggers a workflow that:

Spins up an IBM MQ UBI container.

Compiles the C++ source code.

Creates a temporary Queue Manager.

Executes a functional test to verify the Put/Get cycle.

## What I've achieved:

1. **proof-of-concept** - CI/CD pipeline is fully functional: it compiles against vendored headers and successfully executes a "Put" and "Get" against a live (containerized) IBM MQ instance.
2. **async-waiting-feature**- Moving from "one-shot" execution to Asynchronous Waiting. Instead of the consumer checking the queue and immediately quitting if it's empty, we’ll tell it to "hang on the line" for a message to arrive
```
~/ibmMQ$ ./mq_get
Timed out after 15 seconds. No message arrived.
Get failed. Reason: 2033
~/ibmMQ$ ./ibmMQ
Successfully put message to TEST.QUEUE!
~/ibmMQ$ ./mq_get
Message Received: Hello IBM MQ from Ubuntu 24.04!
~/ibmMQ$
```
4. **syncpointing-feature** -  With Syncpointing (Transactions), the message is only truly removed from the queue once the code explicitly says "I'm done" (`MQCMIT`). We must move from "unreliable messaging" to Guaranteed Delivery.
5. **Latency-timestamp-cal** - By comparing those MQ timestamps to the current system time when the message is received, we can calculate exactly how many milliseconds the message spent traveling through the provider.*
6. **Resilience** - *(The Poison Message/BackoutCount logic)* In a production environment, a **Poison Message** is a message that causes the consumer to fail or crash every time it tries to process it. Without resilience logic, the app will get stuck in an infinite loop: **Get -> Fail -> Rollback -> Get (Same Message) -> Fail...** IBM MQ solves this with the `BackoutCount` field in the `MQMD`.
  - 6a. **Poison Message Resilience** - Fault Injector Chaos Engineering Tool `mq_poison` to simulate application-level failures
  - 6b. **Dead Letter Queue (DLQ) Routing** - The Consumer detects the Poison Message and moves to DEAD.LETTER.QUEUE 
  - 6c. **Message Lifecycle Management** - The janitor tool `mq_clear_dlq` detects the poison message and clear the queue

