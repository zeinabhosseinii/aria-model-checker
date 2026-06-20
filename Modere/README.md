# Benchmark Execution Guide

This guide only explains how to run the benchmark models located in `testrebecamodels`.  
It does **not** cover project overview or general usage.

---

## Prerequisites

- **Maven** installed
- **Java** (version 8 or higher)
- **G++** with C++11 and pthread support

---

## Step-by-Step Instructions

### 1. Build the Java Executable
```bash
mvn package -P executable
```

### 2. Run the Tool on Your Desired Model

**Basic execution** (without optimizations):
```bash
java -jar target/rmc-2.14.jar -s src/main/java/org/rebecalang/rmc/testrebecamodels/ImageProcessor.txt -e TIMED_REBECA -v 2.3 -x -o ./tmp/test-rmc
```

**With Partial Order Reduction and Symmetry** (for optimized state space exploration):
```bash
java -jar target/rmc-2.14.jar -s src/main/java/org/rebecalang/rmc/testrebecamodels/ImageProcessor.txt -e TIMED_REBECA -v 2.3 -x -o ./tmp/test-rmc -por -symmetry
```

> **To run other benchmarks:**  
> Replace the file path after `-s` with any other model from the `testrebecamodels` directory.

---

### 3. Compile the Generated C++ Code
```bash
g++ -std=c++11 -pthread -O2 ./tmp/test-rmc/*.cpp -o execute
```

### 4. Run the Benchmark
```bash
./execute -x statespace.xml
```

---

## Important Note

- **Optional flags for Step 2:**
  - `-por` : Enables Partial Order Reduction
  - `-symmetry` : Enables Symmetry Reduction
  - You can use them together or separately as needed.

---

## Quick Command Summary

```bash
# 1. Build
mvn package -P executable

# 2. Generate C++ code (without optimizations)
java -jar target/rmc-2.14.jar -s src/main/java/org/rebecalang/rmc/testrebecamodels/ImageProcessor.txt -e TIMED_REBECA -v 2.3 -x -o ./tmp/test-rmc

# 2b. Generate C++ code (with POR and Symmetry)
java -jar target/rmc-2.14.jar -s src/main/java/org/rebecalang/rmc/testrebecamodels/ImageProcessor.txt -e TIMED_REBECA -v 2.3 -x -o ./tmp/test-rmc -por -symmetry

# 3. Compile
g++ -std=c++11 -pthread -O2 ./tmp/test-rmc/*.cpp -o execute

# 4. Run
./execute -x statespace.xml
```

---

For any issues, verify your input paths and ensure all prerequisites are properly installed.