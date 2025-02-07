OPENQASM 2.0;
include "qelib1.inc";
qreg qubits[25];

ccx qubits[24], qubits[23], qubits[21];
cx qubits[24], qubits[23];
ccx qubits[23], qubits[22], qubits[21];
cx qubits[23], qubits[22];
ccx qubits[20], qubits[19], qubits[18];
cx qubits[20], qubits[19];
ccx qubits[19], qubits[21], qubits[18];
cx qubits[19], qubits[21];
ccx qubits[17], qubits[16], qubits[15];
cx qubits[17], qubits[16];
ccx qubits[16], qubits[18], qubits[15];
cx qubits[16], qubits[18];
ccx qubits[14], qubits[13], qubits[12];
cx qubits[14], qubits[13];
ccx qubits[13], qubits[15], qubits[12];
cx qubits[13], qubits[15];
ccx qubits[11], qubits[10], qubits[9];
cx qubits[11], qubits[10];
ccx qubits[10], qubits[12], qubits[9];
cx qubits[10], qubits[12];
ccx qubits[8], qubits[7], qubits[6];
cx qubits[8], qubits[7];
ccx qubits[7], qubits[9], qubits[6];
cx qubits[7], qubits[9];
ccx qubits[5], qubits[4], qubits[3];
cx qubits[5], qubits[4];
ccx qubits[4], qubits[6], qubits[3];
cx qubits[4], qubits[6];
ccx qubits[2], qubits[1], qubits[0];
cx qubits[2], qubits[1];
ccx qubits[1], qubits[3], qubits[0];
cx qubits[1], qubits[3];
