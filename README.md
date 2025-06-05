# Parallel-Firewall

## Overview
Parallel-Firewall is a multithreaded application designed to process network packets efficiently using a producer-consumer model. It leverages a ring buffer for inter-thread communication and applies filtering rules to determine whether packets should be passed or dropped. The application supports parallel processing with up to 32 consumer threads, ensuring high performance and scalability for packet processing tasks.

This project is ideal for scenarios where large volumes of network packets need to be processed in real-time, such as intrusion detection systems, traffic monitoring, or firewall applications.

## Features
- **Producer-Consumer Model**: Implements a ring buffer for efficient communication between producer and consumer threads, minimizing contention and maximizing throughput.
- **Packet Filtering**: Filters packets based on predefined source ranges, ensuring only valid packets are processed further.
- **Multithreading**: Supports up to 32 consumer threads for parallel processing, leveraging modern multi-core architectures.
- **Logging**: Logs packet processing results in a sorted order based on timestamps, ensuring accurate and organized output.
- **Serial Processing**: Includes a standalone application for serial packet processing, useful for debugging or environments without multithreading support.
- **Robust Error Handling**: Ensures proper handling of truncated packets and invalid inputs, improving reliability.

## File Structure
- **`consumer.c`**: Implements consumer threads that process packets, apply filtering rules, and log results in a sorted order.
- **`consumer.h`**: Header file defining consumer-related structures and functions, including thread creation and synchronization mechanisms.
- **`producer.c`**: Implements the producer that reads packets from an input file and enqueues them into the ring buffer for processing.
- **`producer.h`**: Header file for producer-related functions, including the `publish_data` function.
- **`packet.c`**: Contains packet-related utilities, such as hashing and filtering logic, which are essential for packet validation and processing.
- **`packet.h`**: Header file defining the packet structure (`so_packet_t`) and related functions, including `packet_hash` and `process_packet`.
- **`ring_buffer.c`**: Implements the ring buffer used for inter-thread communication, ensuring thread-safe enqueue and dequeue operations.
- **`ring_buffer.h`**: Header file defining the ring buffer structure (`so_ring_buffer_t`) and its associated functions.
- **`firewall.c`**: Main application file that initializes the ring buffer, creates consumer threads, and coordinates packet processing and logging.
- **`serial.c`**: A standalone application for serial packet processing, useful for environments where multithreading is not feasible.
- **`Makefile`**: Build script for compiling the project, including dependencies and linking options.
- **`README.md`**: Documentation for the project, providing an overview, features, and usage instructions.

## Build Instructions
To build the project, use the provided `Makefile`. Run the following command:

```sh
make