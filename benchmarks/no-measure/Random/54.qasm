OPENQASM 2.0;
include "qelib1.inc";
qreg qubits[54];

z qubits[51];
cz qubits[29], qubits[10];
x qubits[5];
ry(pi/2) qubits[39];
ccx qubits[6], qubits[8], qubits[28];
y qubits[45];
ry(pi/2) qubits[38];
cz qubits[25], qubits[21];
z qubits[46];
y qubits[9];
rx(pi/2) qubits[50];
h qubits[26];
z qubits[47];
t qubits[5];
x qubits[47];
cx qubits[0], qubits[19];
x qubits[45];
cx qubits[27], qubits[51];
cz qubits[1], qubits[11];
x qubits[30];
x qubits[47];
y qubits[32];
y qubits[48];
rx(pi/2) qubits[19];
z qubits[40];
ccx qubits[16], qubits[17], qubits[10];
rx(pi/2) qubits[38];
z qubits[19];
cz qubits[16], qubits[38];
cx qubits[8], qubits[4];
rx(pi/2) qubits[21];
s qubits[12];
z qubits[47];
t qubits[43];
x qubits[7];
cx qubits[50], qubits[1];
h qubits[16];
s qubits[29];
rx(pi/2) qubits[23];
rx(pi/2) qubits[44];
ccx qubits[47], qubits[6], qubits[14];
s qubits[39];
s qubits[50];
ccx qubits[35], qubits[7], qubits[30];
h qubits[36];
rx(pi/2) qubits[23];
h qubits[30];
h qubits[48];
h qubits[22];
ry(pi/2) qubits[4];
ccx qubits[18], qubits[49], qubits[17];
z qubits[20];
y qubits[37];
h qubits[46];
ry(pi/2) qubits[3];
cz qubits[28], qubits[0];
cx qubits[9], qubits[23];
x qubits[1];
ccx qubits[19], qubits[40], qubits[46];
x qubits[11];
ry(pi/2) qubits[26];
ccx qubits[13], qubits[46], qubits[33];
y qubits[42];
x qubits[35];
y qubits[36];
s qubits[37];
ccx qubits[31], qubits[2], qubits[21];
ccx qubits[18], qubits[7], qubits[46];
ccx qubits[2], qubits[47], qubits[9];
rx(pi/2) qubits[33];
cx qubits[33], qubits[7];
s qubits[6];
ry(pi/2) qubits[34];
cx qubits[36], qubits[27];
h qubits[49];
cz qubits[26], qubits[47];
x qubits[25];
ccx qubits[36], qubits[28], qubits[5];
cz qubits[46], qubits[29];
ry(pi/2) qubits[50];
cz qubits[5], qubits[6];
z qubits[1];
cz qubits[48], qubits[8];
z qubits[16];
rx(pi/2) qubits[40];
cx qubits[46], qubits[30];
ccx qubits[41], qubits[38], qubits[37];
ccx qubits[27], qubits[9], qubits[35];
ccx qubits[53], qubits[40], qubits[28];
z qubits[15];
z qubits[4];
rx(pi/2) qubits[8];
rx(pi/2) qubits[39];
cx qubits[50], qubits[33];
cz qubits[2], qubits[11];
s qubits[5];
cz qubits[20], qubits[35];
cx qubits[23], qubits[35];
rx(pi/2) qubits[20];
z qubits[45];
t qubits[49];
h qubits[4];
cx qubits[13], qubits[35];
cx qubits[33], qubits[6];
y qubits[5];
ry(pi/2) qubits[5];
ry(pi/2) qubits[40];
ry(pi/2) qubits[36];
cz qubits[12], qubits[41];
ry(pi/2) qubits[48];
x qubits[37];
ry(pi/2) qubits[19];
cz qubits[16], qubits[6];
s qubits[33];
s qubits[48];
ry(pi/2) qubits[3];
cx qubits[35], qubits[46];
cx qubits[41], qubits[42];
x qubits[32];
rx(pi/2) qubits[4];
h qubits[48];
t qubits[31];
ccx qubits[27], qubits[31], qubits[16];
z qubits[30];
s qubits[46];
h qubits[12];
t qubits[53];
rx(pi/2) qubits[16];
ccx qubits[45], qubits[35], qubits[17];
x qubits[27];
h qubits[29];
y qubits[22];
ry(pi/2) qubits[42];
ry(pi/2) qubits[3];
t qubits[12];
cz qubits[21], qubits[1];
s qubits[37];
t qubits[4];
y qubits[37];
s qubits[7];
z qubits[15];
h qubits[29];
ry(pi/2) qubits[22];
ry(pi/2) qubits[3];
x qubits[0];
h qubits[1];
cx qubits[24], qubits[40];
z qubits[50];
h qubits[51];
y qubits[1];
x qubits[42];
t qubits[1];
cx qubits[48], qubits[13];
h qubits[26];
rx(pi/2) qubits[29];
s qubits[51];
y qubits[27];
cz qubits[48], qubits[47];
cz qubits[48], qubits[24];
x qubits[18];
y qubits[3];
cz qubits[31], qubits[0];
