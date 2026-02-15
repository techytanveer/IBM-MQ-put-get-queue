[![IBM MQ C++ CI Pipeline](https://github.com/techytanveer/IBM-MQ-put-get-queue/actions/workflows/pipeline.yml/badge.svg)](https://github.com/techytanveer/IBM-MQ-put-get-queue/actions/workflows/pipeline.yml)

# IBM MQ C++ Put/Get Queue Demo

A high-performance C++ implementation for interacting with IBM MQ. This project demonstrates how to connect to a Queue Manager, put a message into a local queue, and retrieve it using a separate consumer.



## 🚀 Features
* **Producer (`ibmMQ`):** Connects to `QM1` and puts a test message into `TEST.QUEUE`.
* **Consumer (`mq_get`):** Connects to `QM1` and retrieves the message.
* **Automated CI/CD:** Integrated with GitHub Actions to compile and test against a live IBM MQ container.

---

## 🛠 Prerequisites

To build and run this locally, you need:
1.  **IBM MQ Client/Server:** Installed in `/opt/mqm`.
2.  **C++ Compiler:** `gcc-c++`.
3.  **Library Access:** `LD_LIBRARY_PATH` must include the MQ `lib64` directory.

---

## 📦 Project Structure

* `main.cpp` - The Producer application (Put).
* `get.cpp` - The Consumer application (Get).
* `include/` - Vendored IBM MQ headers (`cmqc.h`, etc.) for portable builds.
* `.github/workflows/` - CI pipeline configuration.

---

## 🔨 Build Instructions

Use the following commands to compile the applications. We include the `./include` directory to ensure the compiler finds the necessary MQ headers.

```bash
# Compile Producer
g++ -I./include main.cpp -L/opt/mqm/lib64 -lmqm_r -o ibmMQ

# Compile Consumer
g++ -I./include get.cpp -L/opt/mqm/lib64 -lmqm_r -o mq_get
```
## 🏃 Local Compilation
```
g++ -I/opt/mqm/inc main.cpp -L/opt/mqm/lib64 -lmqm_r -o ibmMQ
g++ -I/opt/mqm/inc get.cpp -L/opt/mqm/lib64 -lmqm_r -o mq_get
```

## 🏃 Local Execution
Before running, ensuring the environment is configured and a Queue Manager is running.

Set Environment Variables:

```
export LD_LIBRARY_PATH=/opt/mqm/lib64
Setup Queue Manager (Local Server):


crtmqm QM1
strmqm QM1

echo "DEFINE QLOCAL('TEST.QUEUE') REPLACE" | runmqsc QM1

Run the Demo:


./ibmMQ      # Puts the message
./mq_get     # Gets the message
```

## 🤖 CI/CD Pipeline
This project uses GitHub Actions to maintain code quality. Every push triggers a workflow that:

Spins up an IBM MQ UBI container.

Compiles the C++ source code.

Creates a temporary Queue Manager.

Executes a functional test to verify the Put/Get cycle.

## What I've achieved:
`Environment Stability:` By vendoring the `.h` files, I have removed the brittle dependency on external downloads.

`Configuration Alignment:` Matched the infrastructure (`QM1`) to the application code.

`Security Orchestration:` Learned how to bypass MQ's connection authentication (`CONNAUTH`) specifically for automated testing environments.

---

## 🔄 Change Management

1. **proof-of-concept.tar** - CI/CD pipeline is fully functional: it compiles against vendored headers and successfully executes a "Put" and "Get" against a live (containerized) IBM MQ instance.
2. **async-waiting-feature.tar**- Moving from "one-shot" execution to Asynchronous Waiting. Instead of the consumer checking the queue and immediately quitting if it's empty, we’ll tell it to "hang on the line" for a message to arrive
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
4. **In Progress** - Syncpointing feature  
