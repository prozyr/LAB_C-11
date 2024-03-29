# Prime Sum Theory Checker

This project explores the theory that every even number can be represented as the sum of two prime numbers. Additionally, it evaluates the performance of the program for both single-threaded and multi-threaded execution.

## Project Structure

The project is implemented in C++ and consists of two main functions (`f1` and `f2`) executed by different threads. The `f2` function generates prime and even numbers within a specified range, while the `f1` function checks the theory for each even number.

## Dependencies

- C++ Compiler
- Windows Operating System (for `system("pause")`)

## How to Build and Run

1. Compile the code using a C++ compiler (e.g., g++).
   ```bash
   g++ -o prime_sum_checker prime_sum_checker.cpp -std=c++11 -pthread

## Configuration
- num_threads: Number of threads used for parallel processing. It is currently set to 10, but you can adjust it in the main function.

- Search struct: Defines the range for prime and even number generation. The default range is from 4 to 1000. You can modify the init and finish values in the Search struct for different ranges.

## Results
- The program outputs the time taken for the entire process in milliseconds. The timing results are printed at the end of the execution.