OPENQASM 2.0;
include "qelib1.inc";
qreg qubits[26];
creg c[26];
h qubits[0];
h qubits[3];
h qubits[4];
h qubits[7];
h qubits[8];
h qubits[11];
h qubits[12];
h qubits[15];
h qubits[18];
h qubits[19];
h qubits[22];
h qubits[23];
h qubits[4];
ccx qubits[1],qubits[2],qubits[4];
h qubits[4];
h qubits[8];
ccx qubits[5],qubits[6],qubits[8];
h qubits[8];
h qubits[12];
ccx qubits[9],qubits[10],qubits[12];
h qubits[12];
h qubits[15];
ccx qubits[13],qubits[14],qubits[15];
h qubits[15];
h qubits[19];
ccx qubits[16],qubits[17],qubits[19];
h qubits[19];
h qubits[23];
ccx qubits[20],qubits[21],qubits[23];
h qubits[23];
h qubits[0];
ccx qubits[24],qubits[25],qubits[0];
h qubits[0];
cx qubits[1],qubits[2];
cx qubits[5],qubits[6];
cx qubits[9],qubits[10];
cx qubits[13],qubits[14];
cx qubits[16],qubits[17];
cx qubits[20],qubits[21];
cx qubits[24],qubits[25];
h qubits[3];
ccx qubits[2],qubits[6],qubits[3];
h qubits[3];
h qubits[11];
ccx qubits[10],qubits[14],qubits[11];
h qubits[11];
h qubits[18];
ccx qubits[17],qubits[21],qubits[18];
h qubits[18];
h qubits[3];
h qubits[4];
h qubits[11];
h qubits[12];
h qubits[18];
h qubits[19];
h qubits[7];
ccx qubits[3],qubits[11],qubits[7];
h qubits[7];
h qubits[22];
ccx qubits[18],qubits[25],qubits[22];
h qubits[22];
h qubits[8];
ccx qubits[4],qubits[6],qubits[8];
h qubits[8];
h qubits[15];
ccx qubits[12],qubits[14],qubits[15];
h qubits[15];
h qubits[23];
ccx qubits[19],qubits[21],qubits[23];
h qubits[23];
h qubits[7];
h qubits[8];
h qubits[22];
h qubits[23];
h qubits[15];
ccx qubits[8],qubits[11],qubits[15];
h qubits[15];
h qubits[15];
h qubits[0];
ccx qubits[7],qubits[22],qubits[0];
h qubits[0];
h qubits[0];
ccx qubits[25],qubits[23],qubits[0];
h qubits[0];
h qubits[0];
ccx qubits[22],qubits[15],qubits[0];
h qubits[0];
h qubits[0];
h qubits[15];
h qubits[15];
ccx qubits[0],qubits[7],qubits[15];
h qubits[15];
h qubits[15];
h qubits[8];
h qubits[7];
h qubits[4];
h qubits[19];
h qubits[22];
h qubits[23];
h qubits[8];
ccx qubits[0],qubits[3],qubits[8];
h qubits[8];
h qubits[4];
ccx qubits[0],qubits[2],qubits[4];
h qubits[4];
h qubits[7];
ccx qubits[3],qubits[11],qubits[7];
h qubits[7];
h qubits[23];
ccx qubits[15],qubits[18],qubits[23];
h qubits[23];
h qubits[19];
ccx qubits[15],qubits[17],qubits[19];
h qubits[19];
h qubits[22];
ccx qubits[18],qubits[25],qubits[22];
h qubits[22];
h qubits[4];
h qubits[8];
h qubits[19];
h qubits[23];
h qubits[3];
h qubits[11];
h qubits[12];
h qubits[18];
h qubits[3];
ccx qubits[2],qubits[6],qubits[3];
h qubits[3];
h qubits[11];
ccx qubits[10],qubits[14],qubits[11];
h qubits[11];
h qubits[12];
ccx qubits[8],qubits[10],qubits[12];
h qubits[12];
h qubits[18];
ccx qubits[17],qubits[21],qubits[18];
h qubits[18];
h qubits[12];
cx qubits[0],qubits[2];
cx qubits[4],qubits[6];
cx qubits[8],qubits[10];
cx qubits[12],qubits[14];
cx qubits[15],qubits[17];
cx qubits[19],qubits[21];
cx qubits[23],qubits[25];
x qubits[2];
x qubits[6];
x qubits[10];
x qubits[14];
x qubits[17];
x qubits[21];
x qubits[25];
cx qubits[1],qubits[2];
cx qubits[5],qubits[6];
cx qubits[9],qubits[10];
cx qubits[13],qubits[14];
cx qubits[16],qubits[17];
cx qubits[20],qubits[21];
cx qubits[24],qubits[25];
h qubits[12];
h qubits[3];
ccx qubits[2],qubits[6],qubits[3];
h qubits[3];
h qubits[11];
ccx qubits[10],qubits[14],qubits[11];
h qubits[11];
h qubits[12];
ccx qubits[8],qubits[10],qubits[12];
h qubits[12];
h qubits[18];
ccx qubits[17],qubits[21],qubits[18];
h qubits[18];
h qubits[3];
h qubits[11];
h qubits[12];
h qubits[18];
h qubits[4];
h qubits[8];
h qubits[19];
h qubits[23];
h qubits[8];
ccx qubits[0],qubits[3],qubits[8];
h qubits[8];
h qubits[4];
ccx qubits[0],qubits[2],qubits[4];
h qubits[4];
h qubits[7];
ccx qubits[3],qubits[11],qubits[7];
h qubits[7];
h qubits[23];
ccx qubits[15],qubits[18],qubits[23];
h qubits[23];
h qubits[19];
ccx qubits[15],qubits[17],qubits[19];
h qubits[19];
h qubits[22];
ccx qubits[18],qubits[25],qubits[22];
h qubits[22];
h qubits[8];
h qubits[7];
h qubits[4];
h qubits[19];
h qubits[22];
h qubits[23];
h qubits[15];
h qubits[15];
ccx qubits[0],qubits[7],qubits[15];
h qubits[15];
h qubits[0];
h qubits[15];
h qubits[0];
ccx qubits[25],qubits[23],qubits[0];
h qubits[0];
h qubits[0];
ccx qubits[22],qubits[15],qubits[0];
h qubits[0];
h qubits[15];
h qubits[15];
ccx qubits[8],qubits[11],qubits[15];
h qubits[15];
h qubits[7];
h qubits[8];
h qubits[22];
h qubits[23];
h qubits[7];
ccx qubits[3],qubits[11],qubits[7];
h qubits[7];
h qubits[22];
ccx qubits[18],qubits[25],qubits[22];
h qubits[22];
h qubits[8];
ccx qubits[4],qubits[6],qubits[8];
h qubits[8];
h qubits[15];
ccx qubits[12],qubits[14],qubits[15];
h qubits[15];
h qubits[23];
ccx qubits[19],qubits[21],qubits[23];
h qubits[23];
h qubits[3];
h qubits[4];
h qubits[11];
h qubits[12];
h qubits[18];
h qubits[19];
h qubits[3];
ccx qubits[2],qubits[6],qubits[3];
h qubits[3];
h qubits[11];
ccx qubits[10],qubits[14],qubits[11];
h qubits[11];
h qubits[18];
ccx qubits[17],qubits[21],qubits[18];
h qubits[18];
cx qubits[1],qubits[2];
cx qubits[5],qubits[6];
cx qubits[9],qubits[10];
cx qubits[13],qubits[14];
cx qubits[16],qubits[17];
cx qubits[20],qubits[21];
cx qubits[24],qubits[25];
h qubits[4];
ccx qubits[1],qubits[2],qubits[4];
h qubits[4];
h qubits[8];
ccx qubits[5],qubits[6],qubits[8];
h qubits[8];
h qubits[12];
ccx qubits[9],qubits[10],qubits[12];
h qubits[12];
h qubits[15];
ccx qubits[13],qubits[14],qubits[15];
h qubits[15];
h qubits[19];
ccx qubits[16],qubits[17],qubits[19];
h qubits[19];
h qubits[23];
ccx qubits[20],qubits[21],qubits[23];
h qubits[23];
h qubits[0];
ccx qubits[24],qubits[25],qubits[0];
h qubits[0];
h qubits[0];
h qubits[3];
h qubits[4];
h qubits[7];
h qubits[8];
h qubits[11];
h qubits[12];
h qubits[15];
h qubits[18];
h qubits[19];
h qubits[22];
h qubits[23];
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
