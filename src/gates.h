/**
 * @file gates.h
 * @brief Gate operations on the classic MTBDD
 */

#include <stdbool.h>
#include "mtbdd.h"
#include <lace.h>
// #include "qparam.h"

#ifndef GATES_H
#define GATES_H


/**
 * Permutation based implementation of the X gate on the given MTBDD.
 */
// TASK_DECL_2(MTBDD, _gate_h, MTBDD, uint64_t);
// TASK_DECL_2(MTBDD, _gate_s, MTBDD, uint64_t);
// TASK_DECL_2(MTBDD, _gate_x, MTBDD, uint64_t);
// TASK_DECL_2(MTBDD, _gate_z, MTBDD, uint64_t);

LACE_TYPEDEF_CB(MTBDD, mtbdd_apply_gate_op, MTBDD, uint32_t);
TASK_DECL_3(MTBDD, mtbdd_apply_gate_x, MTBDD, mtbdd_apply_gate_op, uint32_t);
/**
 * Apply a gate operation <op> to <dd>. Custom apply needed because xt nodes may not be present in the reduced <dd>.
 * Otherwise it's basically the standard uapply.
 */
#define my_mtbdd_apply_gate_x(dd, op, param) RUN(mtbdd_apply_gate_x, dd, op, param)

LACE_TYPEDEF_CB(MTBDD, mtbdd_apply_gate_op, MTBDD, uint32_t);
TASK_DECL_3(MTBDD, mtbdd_apply_gate, MTBDD, mtbdd_apply_gate_op, uint32_t);
/**
 * Apply a gate operation <op> to <dd>. Custom apply needed because xt nodes may not be present in the reduced <dd>.
 * Otherwise it's basically the standard uapply.
 */
#define my_mtbdd_apply_gate(dd, op, param) RUN(mtbdd_apply_gate, dd, op, param)

// TASK_DECL_4(MTBDD, mtbdd_apply_cgate, MTBDD, mtbdd_apply_gate_op, uint32_t, uint32_t);
// /**
//  * Apply a controlled gate rotation operation <op> to <dd>. Custom apply needed because xc, xt nodes may not be present in the reduced <dd>.
//  * Otherwise it's basically the standard uapply. Assumes xc < xt.
//  * 
//  * Note that in the current implementation will work only with a single controlled rotation operation because
//  * of insufficient number of arguments for Sylvan caching.
//  */
// #define my_mtbdd_apply_cgate(dd, op, xt, xc) RUN(mtbdd_apply_cgate, dd, op, xt, xc)

// /**
//  * Returns the probability the given qubit's state will be 1.
//  * This implementation supports only a measurement of all the qubits at the end of the circuit.
//  * 
//  * @param p_t pointer to an MTBDD
//  * 
//  * @param xt target qubit index
//  * 
//  * @param curr_state current state vector (determined by previous measurements)
//  * 
//  * @param n number of qubits in the circuit
//  * 
//  */
// prob_t measure(MTBDD *p_t, uint32_t xt, char *curr_state, int n);


// /**
//  * Get the low or highe child of the next level of the MTBDD
//  * 
//  * @param node current node
//  * 
//  * @param current_var current variable
//  * 
//  * @return low child of the next level
//  */
MTBDD get_next_level_low(MTBDD node, uint32_t current_var);
MTBDD get_next_level_high(MTBDD node, uint32_t current_var);


// /**
//  * Apply quantum gate X on the state vector.
//  * 
//  * @param p_t pointer to an MTBDD
//  * 
//  * @param xt target qubit index
//  * 
//  */
// LACE_TYPEDEF_CB(MTBDD, mtbdd_apply_gate_op, MTBDD, uint32_t);
TASK_DECL_2(MTBDD, _gate_x, MTBDD, uint32_t);
void gate_x(MTBDD *p_t, uint32_t xt);

// /**
//  * Apply quantum gate Y on the state vector.
//  * 
//  * @param p_t pointer to an MTBDD
//  * 
//  * @param xt target qubit index
//  * 
//  */
TASK_DECL_2(MTBDD, _gate_y, MTBDD, uint32_t);
void gate_y(MTBDD *p_t, uint32_t xt);

// /**
//  * Apply quantum gate Z on the state vector.
//  * 
//  * @param p_t pointer to an MTBDD
//  * 
//  * @param xt target qubit index
//  * 
//  */
TASK_DECL_2(MTBDD, _gate_z, MTBDD, uint32_t);
void gate_z(MTBDD *p_t, uint32_t xt);
// /**
//  * Apply quantum gate S on the state vector.
//  * 
//  * @param p_t pointer to an MTBDD
//  * 
//  * @param xt target qubit index
//  * 
//  */
TASK_DECL_2(MTBDD, _gate_s, MTBDD, uint32_t);
void gate_s(MTBDD *p_t, uint32_t xt);

// /**
//  * Apply quantum gate T on the state vector.
//  * 
//  * @param p_t pointer to an MTBDD
//  * 
//  * @param xt target qubit index
//  * 
//  */
TASK_DECL_2(MTBDD, _gate_t, MTBDD, uint32_t);
void gate_t(MTBDD *p_t, uint32_t xt);

/**
 * Function implementing quantum Hadamard gate for a given MTBDD.
 * 
 * @param p_t pointer to an MTBDD
 * 
 * @param xt target qubit index
 */
TASK_DECL_2(MTBDD, _gate_h, MTBDD, uint32_t);
void gate_h(MTBDD *p_t, uint32_t xt);

// /**
//  * Function implementing quantum Rx(π/2) gate for a given MTBDD.
//  * 
//  * @param p_t pointer to an MTBDD
//  * 
//  * @param xt target qubit index
//  */
// void gate_rx_pihalf(MTBDD *p_t, uint32_t xt);

// /**
//  * Function implementing quantum Ry(π/2) gate for a given MTBDD.
//  * 
//  * @param p_t pointer to an MTBDD
//  * 
//  * @param xt target qubit index
//  */
// void gate_ry_pihalf(MTBDD *p_t, uint32_t xt);

// /**
//  * Function implementing quantum Controlled NOT gate for a given MTBDD.
//  * 
//  * @param p_t custom MTBDD
//  * 
//  * @param xt target qubit index
//  * 
//  * @param xc control qubit index
//  */
LACE_TYPEDEF_CB(MTBDD, mtbdd_cnot_apply_op, MTBDD, uint32_t, uint32_t);
TASK_DECL_4(MTBDD, mtbdd_cnot_apply, MTBDD, mtbdd_cnot_apply_op, uint32_t, uint32_t);
#define mtbdd_cnot_apply(dd, op, xt, xc) RUN(mtbdd_cnot_apply, dd, op, xt, xc)
TASK_DECL_3(MTBDD, _gate_cnot, MTBDD, uint32_t, uint32_t);
void gate_cnot(MTBDD *p_t, uint32_t xt, uint32_t xc);

// /**
//  * Function implementing quantum Controlled Z gate for a given MTBDD.
//  * 
//  * @param p_t custom MTBDD
//  * 
//  * @param xt target qubit index
//  * 
//  * @param xc control qubit index
//  */
LACE_TYPEDEF_CB(MTBDD, mtbdd_cz_apply_op, MTBDD, uint32_t, uint32_t);
TASK_DECL_4(MTBDD, mtbdd_cz_apply, MTBDD, mtbdd_cz_apply_op, uint32_t, uint32_t);
#define mtbdd_cz_apply(dd, op, xt, xc) RUN(mtbdd_cz_apply, dd, op, xt, xc)
TASK_DECL_3(MTBDD, _gate_cz, MTBDD, uint32_t, uint32_t);
void gate_cz(MTBDD *p_t, uint32_t xt, uint32_t xc);

// /**
//  * Function implementing quantum Toffoli gate for a given MTBDD.
//  * 
//  * @param p_t custom MTBDD
//  * 
//  * @param xt target qubit index
//  * 
//  * @param xc1 first control qubit index
//  * 
//  * @param xc2 second control qubit index
//  */

LACE_TYPEDEF_CB(MTBDD, mtbdd_toffoli_apply_op, MTBDD, uint32_t, uint32_t, uint32_t);
TASK_DECL_5(MTBDD, mtbdd_toffoli_apply, MTBDD, mtbdd_toffoli_apply_op, uint32_t, uint32_t, uint32_t);
#define mtbdd_toffoli_apply(dd, op, xt, xc1, xc2) RUN(mtbdd_toffoli_apply, dd, op, xt, xc1, xc2)
TASK_DECL_4(MTBDD, _gate_toffoli, MTBDD, uint32_t, uint32_t, uint32_t);
void gate_toffoli(MTBDD *p_t, uint32_t xt, uint32_t xc1, uint32_t xc2);

// /**
//  * Function implementing quantum Toffoli gate for a given MTBDD.
//  * 
//  * @param p_t custom MTBDD
//  * 
//  * @param xt target qubit index
//  * 
//  * @param xc1 first control qubit index
//  * 
//  * @param xc2 second control qubit index
//  */
LACE_TYPEDEF_CB(MTBDD, mtbdd_ccz_apply_op, MTBDD, uint32_t, uint32_t, uint32_t);
TASK_DECL_5(MTBDD, mtbdd_ccz_apply, MTBDD, mtbdd_ccz_apply_op, uint32_t, uint32_t, uint32_t);
#define mtbdd_ccz_apply(dd, op, xt, xc1, xc2) RUN(mtbdd_ccz_apply, dd, op, xt, xc1, xc2)
TASK_DECL_4(MTBDD, _gate_ccz, MTBDD, uint32_t, uint32_t, uint32_t);
void gate_ccz(MTBDD *p_t, uint32_t xt, uint32_t xc1, uint32_t xc2);

// /**
//  * Function implementing quantum Multicontrol NOT gate for a given MTBDD.
//  * 
//  * @param p_t custom MTBDD
//  * 
//  * @param qparams list of all the target + control qubit indices (first index is assumed to be the target index)
//  * 
//  */
// void gate_mcx(MTBDD *p_t, qparam_list_t *qparams);

#endif
/* end of "gates.h" */