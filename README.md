# IPC-based Sensor Data Pipeline

## Overview
This is a simple Linux system-level program written in C to experiment with inter-process communication, multithreading, and buffering.

Sensor data is generated at ~100 Hz, processed in another process, and prepared for periodic upload while handling slow or blocking operations.

## Design
- Two Linux processes are used:
  - **SensorProcess** generates sensor data and sends it via a named pipe (FIFO).
  - **DistanceCalcProcess** reads data from the FIFO and processes it.
- A **separate upload thread** is created inside the consumer process to isolate slow upload behavior from continuous data reception.
- Sensor samples are grouped into **1-second chunks (100 samples)**.
- A **circular queue of chunks** is used between the main thread and the upload thread.

## Buffering
- Sensor rate: 100 samples/second  
- Worst-case upload delay: ~10 seconds  
- Buffer size: **20 chunks** (10 required + headroom)

This allows data ingestion to continue even when uploads are delayed.

## IPC
- Inter-process communication is implemented using a **named pipe (FIFO)**.
- The FIFO is created at runtime using:
  ```bash
  mkfifo IPC
  gcc SensorProcess.c -o SensorProcess
  gcc DistanceCalcProcess.c -o DistanceCalcProcess -lpthread
