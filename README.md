# Quantum Circuit Stabilizer Simulator

This is a quantum circuit simulator based on MTBDD (Multi-Terminal Binary Decision Diagram), specifically designed for simulating Stabilizer circuits. This project is modified from [MEDUSA](https://github.com/s-jobra/MEDUSA), developed in C language, and utilizes the [Sylvan](https://trolando.github.io/sylvan/) library for MTBDD operations.

## Dependencies
* `gmp` library
* [`Sylvan`](https://trolando.github.io/sylvan/) and [`Lace`](https://fmt.ewi.utwente.nl/tools/lace/) libraries - can be installed via `make` (requires `git`)

## Installation
1. First, ensure that the `gmp` library (`libgmp-dev`) is installed on your system
2. Install dependencies:
```
make install-deps
```
3. Compile the project:
```
make
```

## Usage
The simulator accepts circuit files in OpenQASM format, which can be provided through program arguments or standard input:

```
./stabilizer --file path/to/circuit.qasm
```
or
```
./stabilizer < path/to/circuit.qasm
```

### Additional Options
* `--info`: Output execution time and memory usage
* `--help`: Display all available options

## Output
Simulation results are written to the `res.dot` file. Since these files can be large, converting them to graphics using tools like [`Graphviz`](https://graphviz.org/) might take some time.

You can generate graphics using the following command:
```
make plot
```

If the results contain large values, the system will use alternative variables in `res.dot`, with the actual values available in `res-vars.txt`.
