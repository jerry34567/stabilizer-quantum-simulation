OPENQASM 2.0;
include "qelib1.inc";
qreg qubits[88];
creg c[88];

rx(pi/2) qubits[72];
cz qubits[86], qubits[14];
x qubits[30];
s qubits[73];
t qubits[51];
t qubits[9];
h qubits[28];
z qubits[37];
t qubits[37];
y qubits[71];
h qubits[5];
ccx qubits[21], qubits[2], qubits[15];
cz qubits[67], qubits[41];
z qubits[7];
h qubits[11];
h qubits[15];
ccx qubits[75], qubits[19], qubits[28];
h qubits[46];
cz qubits[55], qubits[61];
s qubits[77];
ccx qubits[8], qubits[26], qubits[86];
x qubits[38];
z qubits[22];
z qubits[70];
z qubits[54];
t qubits[30];
y qubits[32];
x qubits[21];
z qubits[47];
cz qubits[0], qubits[66];
z qubits[46];
h qubits[34];
z qubits[62];
h qubits[2];
rx(pi/2) qubits[75];
t qubits[46];
ccx qubits[19], qubits[26], qubits[41];
x qubits[9];
cx qubits[85], qubits[58];
rx(pi/2) qubits[31];
y qubits[4];
rx(pi/2) qubits[80];
ry(pi/2) qubits[69];
z qubits[6];
t qubits[79];
cz qubits[59], qubits[4];
h qubits[39];
s qubits[29];
s qubits[51];
cz qubits[45], qubits[71];
h qubits[86];
h qubits[23];
t qubits[75];
z qubits[66];
t qubits[36];
s qubits[22];
t qubits[10];
h qubits[38];
t qubits[29];
ry(pi/2) qubits[23];
cz qubits[9], qubits[4];
s qubits[55];
x qubits[18];
cx qubits[84], qubits[63];
x qubits[10];
t qubits[56];
cz qubits[43], qubits[67];
cx qubits[37], qubits[79];
ccx qubits[59], qubits[60], qubits[53];
rx(pi/2) qubits[10];
h qubits[44];
s qubits[68];
cz qubits[24], qubits[48];
y qubits[56];
s qubits[30];
cz qubits[54], qubits[69];
ry(pi/2) qubits[64];
cx qubits[85], qubits[74];
t qubits[40];
ry(pi/2) qubits[35];
cz qubits[84], qubits[79];
x qubits[57];
cz qubits[39], qubits[43];
x qubits[59];
z qubits[46];
y qubits[35];
ry(pi/2) qubits[68];
h qubits[64];
cx qubits[54], qubits[30];
x qubits[83];
t qubits[51];
h qubits[57];
ccx qubits[8], qubits[62], qubits[13];
rx(pi/2) qubits[58];
z qubits[62];
h qubits[64];
x qubits[47];
cz qubits[49], qubits[82];
ccx qubits[81], qubits[5], qubits[72];
rx(pi/2) qubits[9];
z qubits[47];
cx qubits[55], qubits[78];
s qubits[14];
cx qubits[27], qubits[47];
cx qubits[12], qubits[21];
cx qubits[49], qubits[55];
cx qubits[87], qubits[59];
rx(pi/2) qubits[77];
t qubits[47];
h qubits[52];
z qubits[7];
z qubits[72];
x qubits[66];
cx qubits[3], qubits[18];
rx(pi/2) qubits[57];
cz qubits[53], qubits[11];
h qubits[12];
rx(pi/2) qubits[68];
cz qubits[78], qubits[30];
ccx qubits[42], qubits[29], qubits[36];
z qubits[82];
rx(pi/2) qubits[69];
cx qubits[58], qubits[5];
z qubits[27];
x qubits[35];
s qubits[62];
h qubits[0];
z qubits[72];
ry(pi/2) qubits[43];
s qubits[21];
cz qubits[41], qubits[66];
cx qubits[31], qubits[8];
ry(pi/2) qubits[73];
s qubits[30];
ry(pi/2) qubits[32];
y qubits[77];
t qubits[70];
h qubits[72];
ry(pi/2) qubits[24];
rx(pi/2) qubits[31];
cz qubits[10], qubits[7];
cz qubits[58], qubits[36];
rx(pi/2) qubits[5];
y qubits[38];
z qubits[12];
z qubits[77];
cz qubits[73], qubits[38];
x qubits[79];
ccx qubits[65], qubits[3], qubits[10];
h qubits[49];
y qubits[42];
ccx qubits[69], qubits[37], qubits[42];
y qubits[23];
t qubits[54];
z qubits[62];
rx(pi/2) qubits[62];
cx qubits[75], qubits[84];
ry(pi/2) qubits[42];
t qubits[16];
h qubits[63];
t qubits[58];
cx qubits[3], qubits[61];
t qubits[37];
cz qubits[28], qubits[55];
cx qubits[9], qubits[4];
s qubits[52];
s qubits[52];
cx qubits[37], qubits[26];
y qubits[12];
ccx qubits[19], qubits[72], qubits[6];
rx(pi/2) qubits[34];
x qubits[64];
cz qubits[47], qubits[34];
s qubits[26];
t qubits[54];
rx(pi/2) qubits[45];
h qubits[6];
cz qubits[44], qubits[75];
y qubits[8];
x qubits[20];
h qubits[4];
z qubits[35];
h qubits[56];
ccx qubits[65], qubits[38], qubits[27];
cz qubits[60], qubits[67];
t qubits[83];
x qubits[43];
cz qubits[60], qubits[73];
s qubits[17];
cx qubits[20], qubits[47];
cx qubits[7], qubits[15];
rx(pi/2) qubits[62];
x qubits[75];
x qubits[33];
y qubits[59];
ccx qubits[53], qubits[12], qubits[15];
s qubits[87];
ry(pi/2) qubits[36];
s qubits[22];
s qubits[78];
cz qubits[62], qubits[63];
y qubits[56];
rx(pi/2) qubits[54];
z qubits[58];
s qubits[70];
ry(pi/2) qubits[75];
s qubits[6];
ry(pi/2) qubits[26];
t qubits[64];
h qubits[58];
ccx qubits[82], qubits[27], qubits[51];
y qubits[64];
y qubits[67];
x qubits[13];
x qubits[40];
cz qubits[10], qubits[8];
z qubits[40];
cx qubits[82], qubits[77];
y qubits[77];
ry(pi/2) qubits[38];
rx(pi/2) qubits[69];
ry(pi/2) qubits[64];
z qubits[19];
y qubits[10];
z qubits[62];
ry(pi/2) qubits[58];
h qubits[61];
t qubits[80];
rx(pi/2) qubits[14];
ccx qubits[79], qubits[24], qubits[17];
s qubits[64];
h qubits[74];
ry(pi/2) qubits[14];
cx qubits[69], qubits[52];
h qubits[26];
h qubits[34];
s qubits[0];
x qubits[59];
h qubits[18];
cz qubits[72], qubits[8];
z qubits[86];
x qubits[8];
y qubits[9];
x qubits[12];
z qubits[79];
cx qubits[58], qubits[65];
z qubits[72];
ry(pi/2) qubits[14];
z qubits[74];
cx qubits[82], qubits[84];
ccx qubits[58], qubits[41], qubits[36];
rx(pi/2) qubits[35];
ccx qubits[33], qubits[44], qubits[67];
ry(pi/2) qubits[20];
cx qubits[84], qubits[29];
rx(pi/2) qubits[72];
cz qubits[19], qubits[37];
x qubits[85];
s qubits[50];
z qubits[85];
rx(pi/2) qubits[0];
h qubits[33];
ry(pi/2) qubits[79];
z qubits[37];
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
measure qubits[13] -> c[13];
measure qubits[14] -> c[14];
measure qubits[15] -> c[15];
measure qubits[16] -> c[16];
measure qubits[17] -> c[17];
measure qubits[18] -> c[18];
measure qubits[19] -> c[19];
measure qubits[20] -> c[20];
measure qubits[21] -> c[21];
measure qubits[22] -> c[22];
measure qubits[23] -> c[23];
measure qubits[24] -> c[24];
measure qubits[25] -> c[25];
measure qubits[26] -> c[26];
measure qubits[27] -> c[27];
measure qubits[28] -> c[28];
measure qubits[29] -> c[29];
measure qubits[30] -> c[30];
measure qubits[31] -> c[31];
measure qubits[32] -> c[32];
measure qubits[33] -> c[33];
measure qubits[34] -> c[34];
measure qubits[35] -> c[35];
measure qubits[36] -> c[36];
measure qubits[37] -> c[37];
measure qubits[38] -> c[38];
measure qubits[39] -> c[39];
measure qubits[40] -> c[40];
measure qubits[41] -> c[41];
measure qubits[42] -> c[42];
measure qubits[43] -> c[43];
measure qubits[44] -> c[44];
measure qubits[45] -> c[45];
measure qubits[46] -> c[46];
measure qubits[47] -> c[47];
measure qubits[48] -> c[48];
measure qubits[49] -> c[49];
measure qubits[50] -> c[50];
measure qubits[51] -> c[51];
measure qubits[52] -> c[52];
measure qubits[53] -> c[53];
measure qubits[54] -> c[54];
measure qubits[55] -> c[55];
measure qubits[56] -> c[56];
measure qubits[57] -> c[57];
measure qubits[58] -> c[58];
measure qubits[59] -> c[59];
measure qubits[60] -> c[60];
measure qubits[61] -> c[61];
measure qubits[62] -> c[62];
measure qubits[63] -> c[63];
measure qubits[64] -> c[64];
measure qubits[65] -> c[65];
measure qubits[66] -> c[66];
measure qubits[67] -> c[67];
measure qubits[68] -> c[68];
measure qubits[69] -> c[69];
measure qubits[70] -> c[70];
measure qubits[71] -> c[71];
measure qubits[72] -> c[72];
measure qubits[73] -> c[73];
measure qubits[74] -> c[74];
measure qubits[75] -> c[75];
measure qubits[76] -> c[76];
measure qubits[77] -> c[77];
measure qubits[78] -> c[78];
measure qubits[79] -> c[79];
measure qubits[80] -> c[80];
measure qubits[81] -> c[81];
measure qubits[82] -> c[82];
measure qubits[83] -> c[83];
measure qubits[84] -> c[84];
measure qubits[85] -> c[85];
measure qubits[86] -> c[86];
measure qubits[87] -> c[87];
