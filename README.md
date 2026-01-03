IPC-based Sensor Data Processing Pipeline
Overview

This project is a simple system-level exercise implemented in C on Linux to demonstrate inter-process communication, basic multithreading, and buffering under unreliable upload conditions.

The goal was to receive sensor data at a fixed rate, process it, and prepare it for periodic IoT upload while ensuring data is not lost even if uploads are slow or blocking.

Architecture

The system consists of two Linux processes:

SensorProcess

Simulates sensor data generation at ~100 Hz

Sends data to another process using a named pipe (FIFO)

DistanceCalcProcess

Reads sensor data continuously from the FIFO

Groups samples into fixed 1-second chunks (100 samples)

Uses a circular queue to pass chunks to a separate upload thread

Inside the consumer process, a dedicated upload thread is created so that slow or blocking upload operations do not affect continuous data ingestion.

IPC Mechanism

A named pipe (FIFO) is used for inter-process communication.

FIFO is created at runtime using:

mkfifo IPC


The FIFO file itself is not tracked in version control.

Buffering Strategy

Sensor rate: 100 samples per second

Data is grouped into 1-second chunks (100 samples per chunk)

Worst-case upload delay assumed: 10 seconds

This results in 10 chunks being queued in the worst case

Circular queue size is set to 20 chunks to provide headroom for delays and retries

This allows continuous data reception even when uploads are slow.

Current Status / Limitations

Circular queue and upload thread logic are implemented in a basic form

Mutex protection, retry logic, and disk persistence are not fully implemented due to time constraints

Upload behavior is simulated and intended to represent blocking IoT communication

Learning Context

I am new to Linux system programming and learned POSIX IPC, processes, threads, and basic buffering concepts very recently. This implementation reflects my understanding so far and focuses on clear architecture and correct separation of concerns rather than production-level completeness.

How to Run

Create the FIFO:

mkfifo IPC


Compile both programs:

gcc SensorProcess.c -o SensorProcess
gcc DistanceCalcProcess.c -o DistanceCalcProcess -lpthread


Run DistanceCalcProcess first, then SensorProcess.
