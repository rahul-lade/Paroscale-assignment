
# Paroscale Internship Programming Test – July 2025

## 👨‍💻 Language Used
All solutions are written in **C**.

---

## 🔹 Assignment 1: Multi-threaded Unique Number Extractor

### Objective:
Read a large file with integers. Divide file across threads. Each thread extracts unique integers from its chunk and inserts into a global list without duplicates.

### How to Run:
```bash
cd assignment-1
gcc unique_numbers.c -o run1 -lpthread
./run1
