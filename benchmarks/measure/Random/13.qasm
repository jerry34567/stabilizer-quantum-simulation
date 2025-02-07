OPENQASM 2.0;
include "qelib1.inc";
qreg qubits[13];
creg c[13];

z qubits[8];
t qubits[4];
cx qubits[10], qubits[2];
x qubits[12];
s qubits[5];
ry(pi/2) qubits[8];
h qubits[4];
cx qubits[3], qubits[5];
h qubits[10];
rx(pi/2) qubits[6];
ry(pi/2) qubits[4];
cx qubits[8], qubits[1];
y qubits[2];
cz qubits[11], qubits[7];
ccx qubits[4], qubits[0], qubits[1];
ry(pi/2) qubits[2];
h qubits[3];
ry(pi/2) qubits[2];
ry(pi/2) qubits[3];
t qubits[7];
cx qubits[6], qubits[1];
x qubits[3];
ry(pi/2) qubits[9];
ry(pi/2) qubits[8];
t qubits[5];
x qubits[9];
t qubits[9];
y qubits[3];
s qubits[0];
x qubits[6];
rx(pi/2) qubits[8];
ccx qubits[8], qubits[3], qubits[6];
h qubits[8];
s qubits[8];
s qubits[2];
y qubits[0];
cz qubits[1], qubits[9];
h qubits[9];
s qubits[9];
measure qubits[0] -> c[0];
measure qubits[1] -> c[1];
measure qubits[2] -> c[2];
measure qubits[3] -> c[3];
measure qubits[4] -> c[4];
measure qubits[5] -> c[5];
measure qubits[6] -> c[6];
measure qubits[7] -> c[7];
measure qubits[8] -> c[8];
measure qubits[9] -> c[9];
measure qubits[10] -> c[10];
measure qubits[11] -> c[11];
measure qubits[12] -> c[12];
