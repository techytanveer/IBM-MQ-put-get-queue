# 🚀 Project Roadmap & Sprints

This document outlines the evolutionary path of the IBM MQ C++ Put/Get project. Each sprint is designed to transition the codebase from a "Proof of Concept" to a "Production-Grade" messaging framework.

---

## 🟢 Sprint 1: Observability & Latency Tracking
**Goal:** Gain deep visibility into the messaging lifecycle and connection health.

* **Heartbeat Logging:** Implement a background "pulse" to log connection health status periodically.
* **Latency Metrics:** Utilize the `MQMD.PutDate` and `PutTime` headers to calculate the time delta between message creation and consumption.
* **Structured Error Handling:** Create a lookup utility to map MQ Reason Codes (e.g., 2033, 2058) to human-readable explanations.
* **CI/CD Artifacts:** Update GitHub Actions to export execution logs as downloadable build artifacts.

---

## 🟡 Sprint 2: Resilience & Poison Message Handling
**Goal:** Build "unbreakable" logic that survives bad data and network instability.

* **Exponential Backoff Reconnect:** Implement a retry strategy if the Queue Manager connection is lost.
* **Poison Message Detection:** Monitor the `MQMD.BackoutCount`. If a message fails processing more than 3 times, take action.
* **Dead Letter Queue (DLQ) Routing:** Programmatically move "poison messages" to a `DEV.DEAD.LETTER.QUEUE` using `MQPUT` to prevent queue blocking.
* **Signal Handling:** Gracefully catch `SIGTERM` or `Ctrl+C` to ensure `MQDISC` (Disconnect) is called even during an interrupted run.



---

## 🔴 Sprint 3: Security & Enterprise Hardening
**Goal:** Align the application with industry-standard security and performance requirements.

* **TLS/SSL Integration:** Move from plain-text communication to encrypted channels using `MQCD` structures and CipherSpecs.
* **Multi-threaded Consumption:** Utilize `std::thread` to implement a "Competing Consumer" pattern, allowing multiple threads to pull from the same queue simultaneously.
* **Message Serialization:** Replace raw string buffers with structured data formats (e.g., Protobuf or JSON) to simulate real-world API payloads.
* **Performance Benchmarking:** Create a high-volume test mode to measure the maximum Messages Per Second (MPS) the C++ client can handle.

---

## ✅ Completed Milestones
* [x] Basic Put/Get Functionality.
* [x] GitHub Actions CI/CD with Containerized IBM MQ.
* [x] Asynchronous Waiting (`MQGMO_WAIT`).
* [x] Transactional Integrity (Syncpointing & `MQCMIT`).
