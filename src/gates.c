#include "gates.h"
#include "sylvan_int.h" // Because of 'mtbddnode_t'
#include "sylvan_stats.h"

// Add these cache IDs at the top of the file
// #define GATE_CNOT_CACHE_ID 106
// #define GATE_CZ_CACHE_ID 107
// #define GATE_TOFFOLI_CACHE_ID 108
// #define GATE_CCZ_CACHE_ID 109

// Custom cache functions for gates with multiple controls
static inline int __attribute__((unused))
gate_cache_get4(uint64_t opid, uint64_t dd, uint64_t d2, uint64_t d3, uint64_t d4, uint64_t *res)
{
    // Pack d3 and d4 into a single value
    uint64_t packed = (d3 & 0xFFFFFFFF) | ((d4 & 0xFFFFFFFF) << 32);
    return cache_get3(opid, dd, d2, packed, res);
}

static inline int __attribute__((unused))
gate_cache_put4(uint64_t opid, uint64_t dd, uint64_t d2, uint64_t d3, uint64_t d4, uint64_t res)
{
    // Pack d3 and d4 into a single value
    uint64_t packed = (d3 & 0xFFFFFFFF) | ((d4 & 0xFFFFFFFF) << 32);
    return cache_put3(opid, dd, d2, packed, res);
}

TASK_IMPL_3(MTBDD, mtbdd_apply_gate_x, MTBDD, dd, mtbdd_apply_gate_op, op, uint32_t, xt)
{
    sylvan_gc_test();
    // sylvan_stats_count(MTBDD_UAPPLY);

    // Check cache
    MTBDD result;
    if (cache_get3(mtbdd_apply_gate_x_id, dd, (size_t)op, xt, &result)) {
        // sylvan_stats_count(MTBDD_UAPPLY_CACHED);
        return result;
    }

    // Every gate on 0 returns 0
    if (dd == mtbdd_false) {
        return mtbdd_false;
    }

    mtbddnode_t n = MTBDD_GETNODE(dd);
    MTBDD target_dd = dd;
    // Add the missing node if needed
    if (mtbddnode_isleaf(n) || 2*xt+1 < mtbddnode_getvariable(n)) {
        target_dd = _mtbdd_makenode(2*xt+1, dd, dd);
    }

    // Check if not xt
    result = WRAP(op, target_dd, xt);
    if (result != mtbdd_invalid) {
        if (cache_put3(mtbdd_apply_gate_x_id, dd, (size_t)op, xt, result)) {
            // sylvan_stats_count(MTBDD_UAPPLY_CACHEDPUT);
        }
        return result;
    }
    // Else recursion
    MTBDD low = node_getlow(dd, n); // can use dd as if the target dd is different, it means xt is the root
    MTBDD high = node_gethigh(dd, n);

    mtbdd_refs_spawn(SPAWN(mtbdd_apply_gate_x, high, op, xt));
    MTBDD new_low = mtbdd_refs_push(CALL(mtbdd_apply_gate_x, low, op, xt));
    MTBDD new_high = mtbdd_refs_sync(SYNC(mtbdd_apply_gate_x));
    mtbdd_refs_pop(1);
    result = mtbdd_makenode(mtbddnode_getvariable(n), new_low, new_high);

    if (cache_put3(mtbdd_apply_gate_x_id, dd, (size_t)op, xt, result)) {
        // sylvan_stats_count(MTBDD_UAPPLY_CACHEDPUT);
    }

    return result;
}

TASK_IMPL_3(MTBDD, mtbdd_apply_gate, MTBDD, dd, mtbdd_apply_gate_op, op, uint32_t, xt)
{
    sylvan_gc_test();
    // sylvan_stats_count(MTBDD_UAPPLY);

    // Check cache
    MTBDD result;
    if (cache_get3(mtbdd_apply_gate_id, dd, (size_t)op, xt, &result)) {
        // sylvan_stats_count(MTBDD_UAPPLY_CACHED);
        return result;
    }

    // Every gate on 0 returns 0
    if (dd == mtbdd_false) {
        return mtbdd_false;
    }

    mtbddnode_t n = MTBDD_GETNODE(dd);
    MTBDD target_dd = dd;
    // Add the missing node if needed
    if (mtbddnode_isleaf(n) || 2*xt < mtbddnode_getvariable(n)) {
        target_dd = _mtbdd_makenode(2*xt, dd, dd);
    }

    // Check if not xt
    result = WRAP(op, target_dd, xt);
    if (result != mtbdd_invalid) {
        if (cache_put3(mtbdd_apply_gate_id, dd, (size_t)op, xt, result)) {
            // sylvan_stats_count(MTBDD_UAPPLY_CACHEDPUT);
        }
        return result;
    }
    // Else recursion
    MTBDD low = node_getlow(dd, n); // can use dd as if the target dd is different, it means xt is the root
    MTBDD high = node_gethigh(dd, n);

    mtbdd_refs_spawn(SPAWN(mtbdd_apply_gate, high, op, xt));
    MTBDD new_low = mtbdd_refs_push(CALL(mtbdd_apply_gate, low, op, xt));
    MTBDD new_high = mtbdd_refs_sync(SYNC(mtbdd_apply_gate));
    mtbdd_refs_pop(1);
    result = mtbdd_makenode(mtbddnode_getvariable(n), new_low, new_high);

    if (cache_put3(mtbdd_apply_gate_id, dd, (size_t)op, xt, result)) {
        // sylvan_stats_count(MTBDD_UAPPLY_CACHEDPUT);
    }

    return result;
}

TASK_IMPL_2(MTBDD, _gate_z, MTBDD, dd, uint32_t, xt)
{
    // sylvan_gc_test();

    // // Check cache
    // MTBDD result;
    // if (cache_get3(GATE_Z_CACHE_ID, dd, (size_t)xt, 0, &result)) {
    //     return result;
    // }

    // // Original computation
    // MTBDD xt_1_zt_0 = my_mtbdd_neg(my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_mul(dd, 2*xt), 2*xt+1));
    // mtbdd_protect(&xt_1_zt_0);

    // result = my_mtbdd_plus(my_mtbdd_plus(dd, xt_1_zt_0), xt_1_zt_0);
    // mtbdd_protect(&result);

    // // Store in cache
    // cache_put3(GATE_Z_CACHE_ID, dd, (size_t)xt, 0, result);

    // mtbdd_unprotect(&xt_1_zt_0);
    // mtbdd_unprotect(&result);
    // return result;

    // dd has to be a nonterminal because of the apply
    uint32_t var = mtbdd_getvar(dd);

    if (var == 2*xt) {
        MTBDD dd_high = mtbdd_gethigh(dd);
        MTBDD dd_low = mtbdd_getlow(dd);

        MTBDD high_high;
        MTBDD high_low;

        if (dd_high == mtbdd_false) {
            high_high = mtbdd_false;
            high_low = mtbdd_false;
        }
        else if (mtbdd_getvar(dd_high) == 2*xt+1) {
            high_high = mtbdd_gethigh(dd_high);
            high_low = mtbdd_getlow(dd_high);
        }
        else {
            high_high = dd_high;
            high_low = dd_high;
        }


        return mtbdd_makenode(2*xt, dd_low, mtbdd_makenode(2*xt+1, my_mtbdd_neg(high_low), high_high));
    }

    return mtbdd_invalid; // Recurse deeper
}

TASK_IMPL_2(MTBDD, _gate_x, MTBDD, dd, uint32_t, xt)
{
    // sylvan_gc_test();

    // // Check cache
    // MTBDD result;
    // if (cache_get3(GATE_X_CACHE_ID, dd, (size_t)xt, 0, &result)) {
    //     return result;
    // }


    // // Original computation
    // MTBDD zt_1 = my_mtbdd_neg(my_mtbdd_b_xt_mul(dd, 2*xt+1));
    // mtbdd_protect(&zt_1);
    // MTBDD zt_0 = my_mtbdd_b_xt_comp_mul(dd, 2*xt+1);
    // mtbdd_protect(&zt_0);
    
    // result = my_mtbdd_plus(zt_0, zt_1);
    // mtbdd_protect(&result);

    // // Store in cache
    // cache_put3(GATE_X_CACHE_ID, dd, (size_t)xt, 0, result);

    // mtbdd_unprotect(&zt_1);
    // mtbdd_unprotect(&zt_0);
    // mtbdd_unprotect(&result);    

    // return result;

    // dd has to be a nonterminal because of the apply
    uint32_t var = mtbdd_getvar(dd);
    
    if (var == 2*xt+1) {
        MTBDD high = mtbdd_gethigh(dd);
        MTBDD low = mtbdd_getlow(dd);

        return mtbdd_makenode(2*xt+1, low, my_mtbdd_neg(high));
    }

    return mtbdd_invalid; // Recurse deeper


}

TASK_IMPL_2(MTBDD, _gate_y, MTBDD, dd, uint32_t, xt)
{
    // sylvan_gc_test();

    // MTBDD result;
    // if (cache_get3(GATE_Y_CACHE_ID, dd, (size_t)xt, 0, &result)) {
    //     return result;
    // }

    // MTBDD xt_0_zt_1 = my_mtbdd_neg(my_mtbdd_b_xt_mul(my_mtbdd_b_xt_comp_mul(dd, 2*xt), 2*xt+1));
    // mtbdd_protect(&xt_0_zt_1);
    // MTBDD xt_1_zt_0 = my_mtbdd_neg(my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_mul(dd, 2*xt), 2*xt+1));
    // mtbdd_protect(&xt_1_zt_0);
    // MTBDD xt_1_zt_1 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_mul(dd, 2*xt), 2*xt+1);
    // mtbdd_protect(&xt_1_zt_1);
    // MTBDD xt_0_zt_0 = my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(dd, 2*xt), 2*xt+1);
    // mtbdd_protect(&xt_0_zt_0);

    // MTBDD temp1 = my_mtbdd_plus(xt_0_zt_1, xt_1_zt_0);
    // mtbdd_protect(&temp1);
    // MTBDD temp2 = my_mtbdd_plus(xt_1_zt_1, xt_0_zt_0);
    // mtbdd_protect(&temp2);

    // result = my_mtbdd_plus(temp1, temp2);
    // mtbdd_protect(&result);

    // cache_put3(GATE_Y_CACHE_ID, dd, (size_t)xt, 0, result);

    // mtbdd_unprotect(&xt_0_zt_1);
    // mtbdd_unprotect(&xt_1_zt_0);
    // mtbdd_unprotect(&xt_1_zt_1);
    // mtbdd_unprotect(&xt_0_zt_0);
    // mtbdd_unprotect(&temp1);
    // mtbdd_unprotect(&temp2);
    // mtbdd_unprotect(&result);

    // return result;

    // dd has to be a nonterminal because of the apply
    uint32_t var = mtbdd_getvar(dd);

    if (var == 2*xt) {
        MTBDD dd_high = mtbdd_gethigh(dd);
        MTBDD dd_low = mtbdd_getlow(dd);

        MTBDD high_high;
        MTBDD high_low;
        MTBDD low_high;
        MTBDD low_low;

        if (dd_high == mtbdd_false) {
            high_high = mtbdd_false;
            high_low = mtbdd_false;
        }
        else if (mtbdd_getvar(dd_high) == 2*xt+1) {
            high_high = mtbdd_gethigh(dd_high);
            high_low = mtbdd_getlow(dd_high);
        }
        else {
            high_high = dd_high;
            high_low = dd_high;
        }

        if (dd_low == mtbdd_false) {
            low_high = mtbdd_false;
            low_low = mtbdd_false;
        }
        else if (mtbdd_getvar(dd_low) == 2*xt+1) {
            low_high = mtbdd_gethigh(dd_low);
            low_low = mtbdd_getlow(dd_low);
        }
        else {
            low_high = dd_low;
            low_low = dd_low;
        }

        MTBDD new_high = mtbdd_makenode(2*xt+1, my_mtbdd_neg(high_high), high_low);
        MTBDD new_low = mtbdd_makenode(2*xt+1, low_low, my_mtbdd_neg(low_high));

        return mtbdd_makenode(2*xt, new_low, new_high);
    }
    
    return mtbdd_invalid; // Recurse deeper
}

TASK_IMPL_2(MTBDD, _gate_s, MTBDD, dd, uint32_t, xt)
{
    // sylvan_gc_test();

    // MTBDD result;
    // if (cache_get3(GATE_S_CACHE_ID, dd, (size_t)xt, 0, &result)) {
    //     return result;
    // }

    // MTBDD xt_0 = my_mtbdd_b_xt_comp_mul(dd, 2*xt);
    // mtbdd_protect(&xt_0);

    // MTBDD xt_1_zt_0 = my_mtbdd_neg(my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_mul(dd, 2*xt), 2*xt+1));
    // mtbdd_protect(&xt_1_zt_0);
    // MTBDD xt_1_zt_1 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_mul(dd, 2*xt), 2*xt+1);
    // mtbdd_protect(&xt_1_zt_1);

    // MTBDD temp1 = my_mtbdd_plus(xt_1_zt_0, xt_1_zt_1);
    // mtbdd_protect(&temp1);
    // result = my_mtbdd_plus(temp1, xt_0);
    // mtbdd_protect(&result);

    // cache_put3(GATE_S_CACHE_ID, dd, (size_t)xt, 0, result);

    // mtbdd_unprotect(&xt_0);
    // mtbdd_unprotect(&xt_1_zt_0);
    // mtbdd_unprotect(&xt_1_zt_1);
    // mtbdd_unprotect(&temp1);
    // mtbdd_unprotect(&result);

    // return result;

    // dd has to be a nonterminal because of the apply
    uint32_t var = mtbdd_getvar(dd);

    if (var == 2*xt) {
        MTBDD dd_high = mtbdd_gethigh(dd);
        MTBDD dd_low = mtbdd_getlow(dd);

        MTBDD high_high;
        MTBDD high_low;

        if (dd_high == mtbdd_false) {
            high_high = mtbdd_false;
            high_low = mtbdd_false;
        }
        else if (mtbdd_getvar(dd_high) == 2*xt+1) {
            high_high = mtbdd_gethigh(dd_high);
            high_low = mtbdd_getlow(dd_high);
        }
        else {
            high_high = dd_high;
            high_low = dd_high;
        }

        return mtbdd_makenode(2*xt, dd_low, mtbdd_makenode(2*xt+1, my_mtbdd_neg(high_high), high_low));
    }

    return mtbdd_invalid; // Recurse deeper
}

TASK_IMPL_2(MTBDD, _gate_t, MTBDD, dd, uint32_t, xt)
{
    // sylvan_gc_test();

    // MTBDD result;
    // if (cache_get3(GATE_T_CACHE_ID, dd, (size_t)xt, 0, &result)) {
    //     return result;
    // }

    // MTBDD xt_0 = my_mtbdd_b_xt_comp_mul(dd, 2*xt);
    // mtbdd_protect(&xt_0);
    // MTBDD xt_1 = my_mtbdd_b_xt_mul(dd, 2*xt);
    // mtbdd_protect(&xt_1);
    // MTBDD orig_xt_1_zt_0 = my_mtbdd_b_xt_comp_mul(xt_1, 2*xt+1);
    // mtbdd_protect(&orig_xt_1_zt_0);
    // MTBDD orig_xt_1_zt_1 = my_mtbdd_b_xt_mul(xt_1, 2*xt+1);
    // mtbdd_protect(&orig_xt_1_zt_1);
    // MTBDD orig_xt_1_zt_0_neg = my_mtbdd_b_xt_mul(my_mtbdd_t_xt_comp(xt_1, 2*xt+1), 2*xt+1);
    // mtbdd_protect(&orig_xt_1_zt_0_neg);
    // MTBDD orig_xt_1_zt_1_neg = my_mtbdd_b_xt_comp_mul(my_mtbdd_t_xt(xt_1, 2*xt+1), 2*xt+1);
    // mtbdd_protect(&orig_xt_1_zt_1_neg);

    // MTBDD res_xt_1_zt_0 = my_mtbdd_div_root2(my_mtbdd_minus(orig_xt_1_zt_0, orig_xt_1_zt_1_neg));
    // mtbdd_protect(&res_xt_1_zt_0);
    // MTBDD res_xt_1_zt_1 = my_mtbdd_div_root2(my_mtbdd_plus(orig_xt_1_zt_1, orig_xt_1_zt_0_neg));
    // mtbdd_protect(&res_xt_1_zt_1);

    // result = my_mtbdd_plus(xt_0, my_mtbdd_plus(res_xt_1_zt_0, res_xt_1_zt_1));
    // mtbdd_protect(&result);

    // cache_put3(GATE_T_CACHE_ID, dd, (size_t)xt, 0, result);

    // mtbdd_unprotect(&xt_0);
    // mtbdd_unprotect(&xt_1);
    // mtbdd_unprotect(&orig_xt_1_zt_0);
    // mtbdd_unprotect(&orig_xt_1_zt_1);
    // mtbdd_unprotect(&orig_xt_1_zt_0_neg);
    // mtbdd_unprotect(&orig_xt_1_zt_1_neg);
    // mtbdd_unprotect(&res_xt_1_zt_0);
    // mtbdd_unprotect(&res_xt_1_zt_1);
    // mtbdd_unprotect(&result);

    // return result;

    // dd has to be a nonterminal because of the apply
    uint32_t var = mtbdd_getvar(dd);

    if (var == 2*xt) {
        MTBDD dd_high = mtbdd_gethigh(dd);
        MTBDD dd_low = mtbdd_getlow(dd);

        MTBDD high_high;
        MTBDD high_low;

        if (dd_high == mtbdd_false) {
            high_high = mtbdd_false;
            high_low = mtbdd_false;
        }
        else if (mtbdd_getvar(dd_high) == 2*xt+1) {
            high_high = mtbdd_gethigh(dd_high);
            high_low = mtbdd_getlow(dd_high);
        }
        else {
            high_high = dd_high;
            high_low = dd_high;
        }

        MTBDD new_high_low = my_mtbdd_div_root2(my_mtbdd_minus(high_high, high_low));
        MTBDD new_high_high = my_mtbdd_div_root2(my_mtbdd_plus(high_high, high_low));
        mtbdd_protect(&new_high_low);
        mtbdd_protect(&new_high_high);

        MTBDD result = mtbdd_makenode(2*xt, dd_low, mtbdd_makenode(2*xt+1, new_high_low, new_high_high));

        mtbdd_unprotect(&new_high_low);
        mtbdd_unprotect(&new_high_high);
        return result;
    }

    return mtbdd_invalid; // Recurse deeper
}

TASK_IMPL_2(MTBDD, _gate_h, MTBDD, dd, uint32_t, xt)
{
    // sylvan_gc_test();

    // MTBDD result;
    // if (cache_get3(GATE_H_CACHE_ID, dd, (size_t)xt, 0, &result)) {
    //     return result;
    // }

    // MTBDD xt_1_zt_1 = my_mtbdd_neg(my_mtbdd_b_xt_mul(my_mtbdd_b_xt_mul(dd, 2*xt), 2*xt+1));
    // mtbdd_protect(&xt_1_zt_1);
    // MTBDD xt_0_zt_0 = my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(dd, 2*xt), 2*xt+1);
    // mtbdd_protect(&xt_0_zt_0);

    // // deal with the 10 to 01
    // MTBDD xt_1_zt_0 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_comp_mul(my_mtbdd_t_xt_comp(my_mtbdd_t_xt(dd, 2*xt), 2*xt+1), 2*xt), 2*xt+1);
    // mtbdd_protect(&xt_1_zt_0);
    // MTBDD xt_0_zt_1 = my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_mul(my_mtbdd_t_xt(my_mtbdd_t_xt_comp(dd, 2*xt), 2*xt+1), 2*xt), 2*xt+1);
    // mtbdd_protect(&xt_0_zt_1);

    // MTBDD temp1 = my_mtbdd_plus(xt_1_zt_0, xt_0_zt_1);
    // mtbdd_protect(&temp1);

    // MTBDD temp2 = my_mtbdd_plus(xt_1_zt_1, xt_0_zt_0);
    // mtbdd_protect(&temp2);

    // result = my_mtbdd_plus(temp1, temp2);
    // mtbdd_protect(&result);

    // cache_put3(GATE_H_CACHE_ID, dd, (size_t)xt, 0, result);

    // mtbdd_unprotect(&xt_1_zt_1);
    // mtbdd_unprotect(&xt_0_zt_0);
    // mtbdd_unprotect(&xt_1_zt_0);
    // mtbdd_unprotect(&xt_0_zt_1);
    // mtbdd_unprotect(&temp1);
    // mtbdd_unprotect(&temp2);
    // mtbdd_unprotect(&result);

    // return result;

    // dd has to be a nonterminal because of the apply
    uint32_t var = mtbdd_getvar(dd);
    
    if (var == 2*xt) {
        MTBDD dd_high = mtbdd_gethigh(dd);
        MTBDD dd_low = mtbdd_getlow(dd);
        MTBDD high_low;
        MTBDD high_high;
        MTBDD low_low;
        MTBDD low_high;
        if (dd_high == mtbdd_false) {
            high_low = mtbdd_false;
            high_high = mtbdd_false;
        }
        else if (mtbdd_getvar(dd_high) == 2*xt+1) {
            high_low = mtbdd_getlow(dd_high);
            high_high = mtbdd_gethigh(dd_high);
        }
        else {
            high_low = dd_high;
            high_high = dd_high;
        }

        if (dd_low == mtbdd_false) {
            low_low = mtbdd_false;
            low_high = mtbdd_false;
        }
        else if (mtbdd_getvar(dd_low) == 2*xt+1) {
            low_low = mtbdd_getlow(dd_low);
            low_high = mtbdd_gethigh(dd_low);
        }
        else {
            low_low = dd_low;
            low_high = dd_low;
        }

        MTBDD low = mtbdd_makenode(2*xt+1, low_low, high_low);
        MTBDD high = mtbdd_makenode(2*xt+1, low_high, my_mtbdd_neg(high_high));
        mtbdd_protect(&low);
        mtbdd_protect(&high);

        MTBDD result = mtbdd_makenode(2*xt, low, high);
        mtbdd_unprotect(&low);
        mtbdd_unprotect(&high);

        return result;
    }

    return mtbdd_invalid; // Recurse deeper
}

TASK_IMPL_4(MTBDD, mtbdd_cnot_apply, MTBDD, dd, mtbdd_cnot_apply_op, op, uint32_t, xt, uint32_t, xc)
{
    sylvan_gc_test();

    // Check cache
    MTBDD result;
    if (cache_get3(mtbdd_apply_cx_gate_id, dd, (size_t)xt, (size_t)xc, &result)) {
        return result;
    }

    // Apply the operation directly
    result = WRAP(op, dd, xt, xc);

    // Store in cache
    cache_put3(mtbdd_apply_cx_gate_id, dd, (size_t)xt, (size_t)xc, result);

    return result;
}

TASK_IMPL_3(MTBDD, _gate_cnot, MTBDD, dd, uint32_t, xt, uint32_t, xc)
{
    MTBDD result;
    MTBDD xc_0 = my_mtbdd_b_xt_comp_mul(dd, 2*xc);
    mtbdd_protect(&xc_0);

    MTBDD xc_0_tzc_1 = my_mtbdd_t_xt(xc_0, 2*xc+1);
    mtbdd_protect(&xc_0_tzc_1);
    MTBDD xc_0_tzc_1_zt_1 = my_mtbdd_b_xt_mul(xc_0_tzc_1, 2*xt+1);
    mtbdd_protect(&xc_0_tzc_1_zt_1);
    MTBDD xc_0_tzc_1_zt_1_inv = my_mtbdd_b_xt_comp_mul(xc_0_tzc_1_zt_1, 2*xc+1);
    mtbdd_protect(&xc_0_tzc_1_zt_1_inv);
    MTBDD xc_0_zc_0 = my_mtbdd_b_xt_comp_mul(xc_0, 2*xc+1);
    mtbdd_protect(&xc_0_zc_0);
    MTBDD xc_0_zc_0_orig = my_mtbdd_b_xt_comp_mul(xc_0_zc_0, 2*xt+1);
    mtbdd_protect(&xc_0_zc_0_orig);
    MTBDD first = my_mtbdd_plus(xc_0_zc_0_orig, xc_0_tzc_1_zt_1_inv);
    mtbdd_protect(&first);

    MTBDD xc_0_tzc_0 = my_mtbdd_t_xt_comp(xc_0, 2*xc+1);
    mtbdd_protect(&xc_0_tzc_0);
    MTBDD xc_0_tzc_0_zt_1 = my_mtbdd_b_xt_mul(xc_0_tzc_0, 2*xt+1);
    mtbdd_protect(&xc_0_tzc_0_zt_1);
    MTBDD xc_0_tzc_0_zt_1_inv = my_mtbdd_b_xt_mul(xc_0_tzc_0_zt_1, 2*xc+1);
    mtbdd_protect(&xc_0_tzc_0_zt_1_inv);
    MTBDD xc_0_zc_1 = my_mtbdd_b_xt_mul(xc_0, 2*xc+1);
    mtbdd_protect(&xc_0_zc_1);
    MTBDD xc_0_zc_1_orig = my_mtbdd_b_xt_comp_mul(xc_0_zc_1, 2*xt+1);
    mtbdd_protect(&xc_0_zc_1_orig);
    MTBDD second = my_mtbdd_plus(xc_0_zc_1_orig, xc_0_tzc_0_zt_1_inv);
    mtbdd_protect(&second);

    MTBDD res1 = my_mtbdd_plus(first, second);
    mtbdd_protect(&res1);

    mtbdd_unprotect(&xc_0);
    mtbdd_unprotect(&xc_0_zc_0);
    mtbdd_unprotect(&xc_0_zc_1);
    mtbdd_unprotect(&xc_0_zc_0_orig);
    mtbdd_unprotect(&xc_0_zc_1_orig);
    mtbdd_unprotect(&xc_0_tzc_0);
    mtbdd_unprotect(&xc_0_tzc_0_zt_1);
    mtbdd_unprotect(&xc_0_tzc_0_zt_1_inv);
    mtbdd_unprotect(&xc_0_tzc_1);
    mtbdd_unprotect(&xc_0_tzc_1_zt_1);
    mtbdd_unprotect(&xc_0_tzc_1_zt_1_inv);
    mtbdd_unprotect(&first);
    mtbdd_unprotect(&second);

    MTBDD xc_1 = my_mtbdd_b_xt_mul(dd, 2*xc);
    mtbdd_protect(&xc_1);

    MTBDD xc_1_tzc_1 = my_mtbdd_t_xt(xc_1, 2*xc+1);
    mtbdd_protect(&xc_1_tzc_1);
    MTBDD xc_1_tzc_1_zt_1 = my_mtbdd_b_xt_mul(xc_1_tzc_1, 2*xt+1);
    mtbdd_protect(&xc_1_tzc_1_zt_1);
    MTBDD xc_1_tzc_1_zt_1_inv = my_mtbdd_b_xt_comp_mul(xc_1_tzc_1_zt_1, 2*xc+1);
    mtbdd_protect(&xc_1_tzc_1_zt_1_inv);
    MTBDD xc_1_zc_0 = my_mtbdd_b_xt_comp_mul(xc_1, 2*xc+1);
    mtbdd_protect(&xc_1_zc_0); 
    MTBDD xc_1_zc_0_orig = my_mtbdd_b_xt_comp_mul(xc_1_zc_0, 2*xt+1);
    mtbdd_protect(&xc_1_zc_0_orig);
    MTBDD left_bracket = my_mtbdd_plus(xc_1_zc_0_orig, xc_1_tzc_1_zt_1_inv);
    mtbdd_protect(&left_bracket);
    
    MTBDD xc_1_tzc_0 = my_mtbdd_t_xt_comp(xc_1, 2*xc+1);
    mtbdd_protect(&xc_1_tzc_0);
    MTBDD xc_1_tzc_0_zt_1 = my_mtbdd_b_xt_mul(xc_1_tzc_0, 2*xt+1);
    mtbdd_protect(&xc_1_tzc_0_zt_1);
    MTBDD xc_1_tzc_0_zt_1_inv = my_mtbdd_b_xt_mul(xc_1_tzc_0_zt_1, 2*xc+1);
    mtbdd_protect(&xc_1_tzc_0_zt_1_inv);
    MTBDD xc_1_zc_1 = my_mtbdd_b_xt_mul(xc_1, 2*xc+1);
    mtbdd_protect(&xc_1_zc_1);
    MTBDD xc_1_zc_1_orig = my_mtbdd_b_xt_comp_mul(xc_1_zc_1, 2*xt+1);
    mtbdd_protect(&xc_1_zc_1_orig);
    MTBDD right_bracket = my_mtbdd_plus(xc_1_zc_1_orig, xc_1_tzc_0_zt_1_inv);
    mtbdd_protect(&right_bracket);

    MTBDD temp = my_mtbdd_plus(left_bracket, right_bracket);
    mtbdd_protect(&temp);
    // deal with the xt inv
    MTBDD temp_xt_1 = my_mtbdd_t_xt(temp, 2*xt);
    mtbdd_protect(&temp_xt_1);
    MTBDD temp_xt_1_inv = my_mtbdd_b_xt_comp_mul(temp_xt_1, 2*xt);
    mtbdd_protect(&temp_xt_1_inv);

    MTBDD temp_xt_0 = my_mtbdd_t_xt_comp(temp, 2*xt);
    mtbdd_protect(&temp_xt_0);
    MTBDD temp_xt_0_inv = my_mtbdd_b_xt_mul(temp_xt_0, 2*xt);
    mtbdd_protect(&temp_xt_0_inv);

    MTBDD res2 = my_mtbdd_plus(temp_xt_0_inv, temp_xt_1_inv);
    mtbdd_protect(&res2);

    mtbdd_unprotect(&xc_1);
    mtbdd_unprotect(&xc_1_zc_0);
    mtbdd_unprotect(&xc_1_zc_1);
    mtbdd_unprotect(&xc_1_zc_0_orig);
    mtbdd_unprotect(&xc_1_zc_1_orig);
    mtbdd_unprotect(&xc_1_tzc_0);
    mtbdd_unprotect(&xc_1_tzc_0_zt_1);
    mtbdd_unprotect(&xc_1_tzc_0_zt_1_inv);
    mtbdd_unprotect(&xc_1_tzc_1);
    mtbdd_unprotect(&xc_1_tzc_1_zt_1);
    mtbdd_unprotect(&xc_1_tzc_1_zt_1_inv);
    mtbdd_unprotect(&temp);
    mtbdd_unprotect(&temp_xt_0);
    mtbdd_unprotect(&temp_xt_0_inv);
    mtbdd_unprotect(&temp_xt_1);
    mtbdd_unprotect(&temp_xt_1_inv);
    mtbdd_unprotect(&left_bracket);
    mtbdd_unprotect(&right_bracket);

    MTBDD temp_res = my_mtbdd_plus(res1, res2);
    mtbdd_protect(&temp_res);

    mtbdd_unprotect(&res1);
    mtbdd_unprotect(&res2);

    MTBDD xc_1_xt_1_zc_1_zt_1 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_mul(my_mtbdd_b_xt_mul(my_mtbdd_b_xt_mul(temp_res, 2*xc), 2*xc+1), 2*xt), 2*xt+1);
    mtbdd_protect(&xc_1_xt_1_zc_1_zt_1);

    MTBDD xc_1_xt_0_zc_0_zt_1 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_mul(temp_res, 2*xc), 2*xc+1), 2*xt), 2*xt+1);
    mtbdd_protect(&xc_1_xt_0_zc_0_zt_1);

    MTBDD temp2 = my_mtbdd_plus(xc_1_xt_1_zc_1_zt_1, xc_1_xt_0_zc_0_zt_1);
    // temp2 = my_mtbdd_neg(temp2);
    mtbdd_protect(&temp2);

    result = my_mtbdd_minus(my_mtbdd_minus(temp_res, temp2), temp2);
    mtbdd_protect(&result);
    mtbdd_unprotect(&temp_res);
    mtbdd_unprotect(&temp2);
    mtbdd_unprotect(&xc_1_xt_1_zc_1_zt_1);
    mtbdd_unprotect(&xc_1_xt_0_zc_0_zt_1);

    mtbdd_unprotect(&result);

    return result;
}

// cz gate
TASK_IMPL_4(MTBDD, mtbdd_cz_apply, MTBDD, dd, mtbdd_cz_apply_op, op, uint32_t, xt, uint32_t, xc)
{
    sylvan_gc_test();

    // Check cache
    MTBDD result;
    if (cache_get3(mtbdd_apply_cz_gate_id, dd, (size_t)xt, (size_t)xc, &result)) {
        return result;
    }

    // Apply the operation directly
    result = WRAP(op, dd, xt, xc);

    // Store in cache
    cache_put3(mtbdd_apply_cz_gate_id, dd, (size_t)xt, (size_t)xc, result);

    return result;
}

TASK_IMPL_3(MTBDD, _gate_cz, MTBDD, dd, uint32_t, xt, uint32_t, xc) {

    MTBDD xc_0_xt_0 = my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(dd, 2*xc), 2*xt);
    mtbdd_protect(&xc_0_xt_0);
    MTBDD xc_0_xt_1 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_comp_mul(dd, 2*xc), 2*xt);
    mtbdd_protect(&xc_0_xt_1);
    MTBDD bracket_left = my_mtbdd_b_xt_mul(my_mtbdd_t_xt_comp(xc_0_xt_1, 2*xc+1), 2*xc+1);
    mtbdd_protect(&bracket_left);
    MTBDD bracket_right = my_mtbdd_b_xt_comp_mul(my_mtbdd_t_xt(xc_0_xt_1, 2*xc+1), 2*xc+1);
    mtbdd_protect(&bracket_right);
    MTBDD inter_res = my_mtbdd_plus(bracket_left, bracket_right);
    mtbdd_protect(&inter_res);
    MTBDD inter_res_2 = my_mtbdd_plus(inter_res, xc_0_xt_0);
    mtbdd_protect(&inter_res_2);
    
    MTBDD xc_1_xt_0 = my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_mul(dd, 2*xc), 2*xt);
    mtbdd_protect(&xc_1_xt_0);
    MTBDD bracket_left_2 = my_mtbdd_b_xt_mul(my_mtbdd_t_xt_comp(xc_1_xt_0, 2*xt+1), 2*xt+1);
    mtbdd_protect(&bracket_left_2);
    MTBDD bracket_right_2 = my_mtbdd_b_xt_comp_mul(my_mtbdd_t_xt(xc_1_xt_0, 2*xt+1), 2*xt+1);
    mtbdd_protect(&bracket_right_2);
    MTBDD inter_res_3 = my_mtbdd_plus(bracket_left_2, bracket_right_2);
    mtbdd_protect(&inter_res_3);

    MTBDD xc_1_xt_1 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_mul(dd, 2*xc), 2*xt);
    mtbdd_protect(&xc_1_xt_1);
    MTBDD bracket_left_3 = my_mtbdd_b_xt_mul(my_mtbdd_t_xt_comp(my_mtbdd_b_xt_mul(my_mtbdd_t_xt_comp(xc_1_xt_1, 2*xt+1), 2*xt+1), 2*xc+1), 2*xc+1);
    mtbdd_protect(&bracket_left_3);
    MTBDD bracket_right_3 = my_mtbdd_b_xt_comp_mul(my_mtbdd_t_xt(my_mtbdd_b_xt_comp_mul(my_mtbdd_t_xt(xc_1_xt_1, 2*xt+1), 2*xt+1), 2*xc+1), 2*xc+1);
    mtbdd_protect(&bracket_right_3);
    MTBDD inter_res_5 = my_mtbdd_plus(bracket_left_3, bracket_right_3);
    mtbdd_protect(&inter_res_5);

    MTBDD bracket_left_4 = my_mtbdd_neg(my_mtbdd_b_xt_mul(my_mtbdd_t_xt_comp(my_mtbdd_b_xt_comp_mul(my_mtbdd_t_xt(xc_1_xt_1, 2*xt+1), 2*xt+1), 2*xc+1), 2*xc+1));
    mtbdd_protect(&bracket_left_4);
    MTBDD bracket_right_4 = my_mtbdd_neg(my_mtbdd_b_xt_comp_mul(my_mtbdd_t_xt(my_mtbdd_b_xt_mul(my_mtbdd_t_xt_comp(xc_1_xt_1, 2*xt+1), 2*xt+1), 2*xc+1), 2*xc+1));
    mtbdd_protect(&bracket_right_4);
    MTBDD inter_res_6 = my_mtbdd_plus(bracket_left_4, bracket_right_4);
    mtbdd_protect(&inter_res_6);
    MTBDD inter_res_7 = my_mtbdd_plus(inter_res_6, inter_res_5);
    mtbdd_protect(&inter_res_7);
    MTBDD inter_res_8 = my_mtbdd_plus(inter_res_7, inter_res_3);
    mtbdd_protect(&inter_res_8);
    MTBDD res = my_mtbdd_plus(inter_res_8, inter_res_2);
    mtbdd_protect(&res);

    mtbdd_unprotect(&xc_0_xt_0);
    mtbdd_unprotect(&xc_0_xt_1);
    mtbdd_unprotect(&bracket_left);
    mtbdd_unprotect(&bracket_right);
    mtbdd_unprotect(&inter_res);
    mtbdd_unprotect(&inter_res_2);
    mtbdd_unprotect(&xc_1_xt_0);
    mtbdd_unprotect(&bracket_left_2);
    mtbdd_unprotect(&bracket_right_2);
    mtbdd_unprotect(&inter_res_3);
    mtbdd_unprotect(&xc_1_xt_1);
    mtbdd_unprotect(&bracket_left_3);
    mtbdd_unprotect(&bracket_right_3);
    mtbdd_unprotect(&inter_res_5);
    mtbdd_unprotect(&bracket_left_4);
    mtbdd_unprotect(&bracket_right_4);
    mtbdd_unprotect(&inter_res_6);  
    mtbdd_unprotect(&inter_res_7);
    mtbdd_unprotect(&inter_res_8);
    mtbdd_unprotect(&res);

    return res;
}

// Implementation example for Toffoli gate
TASK_IMPL_5(MTBDD, mtbdd_toffoli_apply, MTBDD, dd, mtbdd_toffoli_apply_op, op, uint32_t, xt, uint32_t, xc1, uint32_t, xc2)
{
    sylvan_gc_test();

    static int toffoli_count = 0;
    toffoli_count++;
    // if (toffoli_count >= 28) {
    //     // mtbdd_traverse_nodes(dd);
    // }
    // else {
        // printf("toffoli_count: %d\n", toffoli_count);
    // }

    // mtbdd_traverse_nodes(dd);
    // Check cache using our custom cache function
    MTBDD result;
    if (gate_cache_get4(mtbdd_apply_ccx_gate_id, dd, (size_t)xt, (size_t)xc1, (size_t)xc2, &result)) {
        return result;
    }

    // Apply the operation directly
    result = WRAP(op, dd, xt, xc1, xc2);
    // mtbdd_traverse_nodes(result);

    // Store in cache using our custom cache function
    gate_cache_put4(mtbdd_apply_ccx_gate_id, dd, (size_t)xt, (size_t)xc1, (size_t)xc2, result);
    // mtbdd_traverse_nodes(result);

    return result;
}

TASK_IMPL_4(MTBDD, _gate_toffoli, MTBDD, dd, uint32_t, xt, uint32_t, xc1, uint32_t, xc2)
{
    mtbdd_protect(&dd);
    MTBDD res = my_mtbdd_div_2(dd);
    mtbdd_unprotect(&dd);
    mtbdd_protect(&res);

    // 分段計算，每段後檢查結果
    MTBDD xc1_0_xc2_0_zt_0 = my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(dd, 2*xc1), 2*xc2), 2*xt+1);
    mtbdd_protect(&xc1_0_xc2_0_zt_0);
    
    MTBDD xc1_0_xc2_0_zt_0_div2 = my_mtbdd_div_2(xc1_0_xc2_0_zt_0);
    mtbdd_protect(&xc1_0_xc2_0_zt_0_div2);

    MTBDD without_xc1_0_xc2_0_zt_0 = my_mtbdd_minus(dd, xc1_0_xc2_0_zt_0);
    mtbdd_protect(&without_xc1_0_xc2_0_zt_0);

    mtbdd_unprotect(&res);
    res = my_mtbdd_plus(res, xc1_0_xc2_0_zt_0_div2);
    mtbdd_protect(&res);
    MTBDD xt_inv_1 = my_mtbdd_b_xt_comp_mul(my_mtbdd_t_xt(without_xc1_0_xc2_0_zt_0, 2*xt), 2*xt);
    mtbdd_protect(&xt_inv_1);
    MTBDD xt_inv_0 = my_mtbdd_b_xt_mul(my_mtbdd_t_xt_comp(without_xc1_0_xc2_0_zt_0, 2*xt), 2*xt);
    mtbdd_protect(&xt_inv_0);
    MTBDD xt_inv = my_mtbdd_plus(xt_inv_0, xt_inv_1);
    mtbdd_protect(&xt_inv);

    MTBDD zc1_inv_1 = my_mtbdd_b_xt_comp_mul(my_mtbdd_t_xt(without_xc1_0_xc2_0_zt_0, 2*xc1+1), 2*xc1+1);
    mtbdd_protect(&zc1_inv_1);
    MTBDD zc1_inv_0 = my_mtbdd_b_xt_mul(my_mtbdd_t_xt_comp(without_xc1_0_xc2_0_zt_0, 2*xc1+1), 2*xc1+1);
    mtbdd_protect(&zc1_inv_0);
    MTBDD zc1_inv = my_mtbdd_plus(zc1_inv_0, zc1_inv_1);
    mtbdd_protect(&zc1_inv);

    MTBDD zc2_inv_1 = my_mtbdd_b_xt_comp_mul(my_mtbdd_t_xt(without_xc1_0_xc2_0_zt_0, 2*xc2+1), 2*xc2+1);
    mtbdd_protect(&zc2_inv_1);
    MTBDD zc2_inv_0 = my_mtbdd_b_xt_mul(my_mtbdd_t_xt_comp(without_xc1_0_xc2_0_zt_0, 2*xc2+1), 2*xc2+1);
    mtbdd_protect(&zc2_inv_0);
    MTBDD zc2_inv = my_mtbdd_plus(zc2_inv_0, zc2_inv_1);
    mtbdd_protect(&zc2_inv);

    MTBDD zc1_zc2_inv_1 = my_mtbdd_b_xt_comp_mul(my_mtbdd_t_xt(zc1_inv, 2*xc2+1), 2*xc2+1);
    mtbdd_protect(&zc1_zc2_inv_1);
    MTBDD zc1_zc2_inv_0 = my_mtbdd_b_xt_mul(my_mtbdd_t_xt_comp(zc1_inv, 2*xc2+1), 2*xc2+1);
    mtbdd_protect(&zc1_zc2_inv_0);
    MTBDD zc1_zc2_inv = my_mtbdd_plus(zc1_zc2_inv_0, zc1_zc2_inv_1);
    mtbdd_protect(&zc1_zc2_inv);

    MTBDD zc1_xt_inv_1 = my_mtbdd_b_xt_comp_mul(my_mtbdd_t_xt(xt_inv, 2*xc1+1), 2*xc1+1);
    mtbdd_protect(&zc1_xt_inv_1);
    MTBDD zc1_xt_inv_0 = my_mtbdd_b_xt_mul(my_mtbdd_t_xt_comp(xt_inv, 2*xc1+1), 2*xc1+1);
    mtbdd_protect(&zc1_xt_inv_0);
    MTBDD zc1_xt_inv = my_mtbdd_plus(zc1_xt_inv_0, zc1_xt_inv_1);
    mtbdd_protect(&zc1_xt_inv);

    MTBDD zc2_xt_inv_1 = my_mtbdd_b_xt_comp_mul(my_mtbdd_t_xt(xt_inv, 2*xc2+1), 2*xc2+1);
    mtbdd_protect(&zc2_xt_inv_1);
    MTBDD zc2_xt_inv_0 = my_mtbdd_b_xt_mul(my_mtbdd_t_xt_comp(xt_inv, 2*xc2+1), 2*xc2+1);
    mtbdd_protect(&zc2_xt_inv_0);
    MTBDD zc2_xt_inv = my_mtbdd_plus(zc2_xt_inv_0, zc2_xt_inv_1);
    mtbdd_protect(&zc2_xt_inv);

    MTBDD zc1_zc2_xt_inv_1 = my_mtbdd_b_xt_comp_mul(my_mtbdd_t_xt(zc2_xt_inv, 2*xc1+1), 2*xc1+1);
    mtbdd_protect(&zc1_zc2_xt_inv_1);
    MTBDD zc1_zc2_xt_inv_0 = my_mtbdd_b_xt_mul(my_mtbdd_t_xt_comp(zc2_xt_inv, 2*xc1+1), 2*xc1+1);
    mtbdd_protect(&zc1_zc2_xt_inv_0);
    MTBDD zc1_zc2_xt_inv = my_mtbdd_plus(zc1_zc2_xt_inv_0, zc1_zc2_xt_inv_1);
    mtbdd_protect(&zc1_zc2_xt_inv);

    // first ite(zt, 0, 1/2) f(xt')
    MTBDD ite_1 = my_mtbdd_div_2(my_mtbdd_b_xt_comp_mul(xt_inv, 2*xt+1));
    mtbdd_protect(&ite_1);
    // second ite(xc1, 0, 1/2) f(zc1')
    MTBDD ite_2 = my_mtbdd_div_2(my_mtbdd_b_xt_comp_mul(zc1_inv, 2*xc1));
    mtbdd_protect(&ite_2);
    // third ite(xc2, 0, 1/2) f(zc2')
    MTBDD ite_3 = my_mtbdd_div_2(my_mtbdd_b_xt_comp_mul(zc2_inv, 2*xc2));
    mtbdd_protect(&ite_3);

    // fourth ite(xc1 xor xc2, 0, -1/2) ite(zc1 == zc2 and xc1, -1, 1) f(zc1', zc2')
    MTBDD ite4_temp1 = my_mtbdd_neg(my_mtbdd_div_2(zc1_zc2_inv));
    mtbdd_protect(&ite4_temp1);
    MTBDD ite4_temp1_0_0 = my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(ite4_temp1, 2*xc1), 2*xc2); 
    mtbdd_protect(&ite4_temp1_0_0);
    MTBDD ite4_temp1_1_1 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_mul(ite4_temp1, 2*xc1), 2*xc2);
    mtbdd_protect(&ite4_temp1_1_1);
    MTBDD ite4_temp1_1_1_equiv_0 = my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(ite4_temp1_1_1, 2*xc1+1), 2*xc2+1);
    mtbdd_protect(&ite4_temp1_1_1_equiv_0);
    MTBDD ite4_temp1_1_1_equiv_1 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_mul(ite4_temp1_1_1, 2*xc1+1), 2*xc2+1);
    mtbdd_protect(&ite4_temp1_1_1_equiv_1);
    MTBDD ite4_temp1_1_1_equiv = my_mtbdd_plus(ite4_temp1_1_1_equiv_0, ite4_temp1_1_1_equiv_1);
    mtbdd_protect(&ite4_temp1_1_1_equiv);
    MTBDD ite_4 = my_mtbdd_minus(my_mtbdd_minus(my_mtbdd_plus(ite4_temp1_0_0, ite4_temp1_1_1), ite4_temp1_1_1_equiv), ite4_temp1_1_1_equiv);
    mtbdd_protect(&ite_4);

    // fifth ite(xc2 and xt, 0, -1/2) ite(zc2 == xt and xc2, -1, 1) f(zc2', xt')
    MTBDD ite5_temp1 = my_mtbdd_neg(my_mtbdd_div_2(zc2_xt_inv));
    mtbdd_protect(&ite5_temp1);
    MTBDD ite5_temp1_0_0 = my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(ite5_temp1, 2*xc2), 2*xt+1);
    mtbdd_protect(&ite5_temp1_0_0);
    MTBDD ite5_temp1_1_1 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_mul(ite5_temp1, 2*xc2), 2*xt+1);
    mtbdd_protect(&ite5_temp1_1_1);
    MTBDD ite5_temp1_1_1_equiv_0 = my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(ite5_temp1_1_1, 2*xc2+1), 2*xt);
    mtbdd_protect(&ite5_temp1_1_1_equiv_0);
    MTBDD ite5_temp1_1_1_equiv_1 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_mul(ite5_temp1_1_1, 2*xc2+1), 2*xt);
    mtbdd_protect(&ite5_temp1_1_1_equiv_1);
    MTBDD ite5_temp1_1_1_equiv = my_mtbdd_plus(ite5_temp1_1_1_equiv_0, ite5_temp1_1_1_equiv_1);
    mtbdd_protect(&ite5_temp1_1_1_equiv);
    MTBDD ite_5 = my_mtbdd_minus(my_mtbdd_minus(my_mtbdd_plus(ite5_temp1_0_0, ite5_temp1_1_1), ite5_temp1_1_1_equiv), ite5_temp1_1_1_equiv);
    mtbdd_protect(&ite_5);

    // sixth ite(xc1 and xt, 0, -1/2) ite(zc1 == xt and xc1, -1, 1) f(zc1', xt')
    MTBDD ite6_temp1 = my_mtbdd_neg(my_mtbdd_div_2(zc1_xt_inv));
    mtbdd_protect(&ite6_temp1);
    MTBDD ite6_temp1_0_0 = my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(ite6_temp1, 2*xc1), 2*xt+1);
    mtbdd_protect(&ite6_temp1_0_0);
    MTBDD ite6_temp1_1_1 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_mul(ite6_temp1, 2*xc1), 2*xt+1);
    mtbdd_protect(&ite6_temp1_1_1);
    MTBDD ite6_temp1_1_1_equiv_0 = my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(ite6_temp1_1_1, 2*xc1+1), 2*xt);
    mtbdd_protect(&ite6_temp1_1_1_equiv_0);
    MTBDD ite6_temp1_1_1_equiv_1 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_mul(ite6_temp1_1_1, 2*xc1+1), 2*xt);
    mtbdd_protect(&ite6_temp1_1_1_equiv_1);
    MTBDD ite6_temp1_1_1_equiv = my_mtbdd_plus(ite6_temp1_1_1_equiv_0, ite6_temp1_1_1_equiv_1);
    mtbdd_protect(&ite6_temp1_1_1_equiv);
    MTBDD ite_6 = my_mtbdd_minus(my_mtbdd_minus(my_mtbdd_plus(ite6_temp1_0_0, ite6_temp1_1_1), ite6_temp1_1_1_equiv), ite6_temp1_1_1_equiv);
    mtbdd_protect(&ite_6);

    // seventh ite(xc1 xor xc2 xor xt, 0, 1/2) ite((xc1 == xc2 and zc1 == zc2 and xc1) or (xc1 == xt and zc1 == xt and xc1) or (xc2 == xt and zc2 == xt and xc2), -1, 1) f(zc1', zc2', xt')
    MTBDD ite7_temp1 = my_mtbdd_div_2(zc1_zc2_xt_inv);
    mtbdd_protect(&ite7_temp1);
    MTBDD ite7_temp1_0_0_0 = my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(ite7_temp1, 2*xc1), 2*xc2), 2*xt+1);
    mtbdd_protect(&ite7_temp1_0_0_0);
    MTBDD ite7_temp1_0_1_1 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_mul(my_mtbdd_b_xt_comp_mul(ite7_temp1, 2*xc1), 2*xc2), 2*xt+1);
    mtbdd_protect(&ite7_temp1_0_1_1);
    MTBDD ite7_temp1_1_0_1 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_mul(my_mtbdd_b_xt_comp_mul(ite7_temp1, 2*xc2), 2*xt+1), 2*xc1);
    mtbdd_protect(&ite7_temp1_1_0_1);
    MTBDD ite7_temp1_1_1_0 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_mul(ite7_temp1, 2*xc2), 2*xt+1), 2*xc1);
    mtbdd_protect(&ite7_temp1_1_1_0);
    MTBDD ite7_temp1_0_1_1_equiv_0 = my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(ite7_temp1_0_1_1, 2*xc2+1), 2*xt);
    mtbdd_protect(&ite7_temp1_0_1_1_equiv_0);
    MTBDD ite7_temp1_0_1_1_equiv_1 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_mul(ite7_temp1_0_1_1, 2*xc2+1), 2*xt);
    mtbdd_protect(&ite7_temp1_0_1_1_equiv_1);
    MTBDD ite7_temp1_0_1_1_equiv = my_mtbdd_neg(my_mtbdd_plus(ite7_temp1_0_1_1_equiv_0, ite7_temp1_0_1_1_equiv_1));
    mtbdd_protect(&ite7_temp1_0_1_1_equiv);
    MTBDD ite7_temp1_1_0_1_equiv_0 = my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(ite7_temp1_1_0_1, 2*xt), 2*xc1+1);
    mtbdd_protect(&ite7_temp1_1_0_1_equiv_0);
    MTBDD ite7_temp1_1_0_1_equiv_1 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_mul(ite7_temp1_1_0_1, 2*xt), 2*xc1+1);
    mtbdd_protect(&ite7_temp1_1_0_1_equiv_1);
    MTBDD ite7_temp1_1_0_1_equiv = my_mtbdd_neg(my_mtbdd_plus(ite7_temp1_1_0_1_equiv_0, ite7_temp1_1_0_1_equiv_1));
    mtbdd_protect(&ite7_temp1_1_0_1_equiv);
    MTBDD ite7_temp1_1_1_0_equiv_0 = my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(ite7_temp1_1_1_0, 2*xc1+1), 2*xc2+1);
    mtbdd_protect(&ite7_temp1_1_1_0_equiv_0);
    MTBDD ite7_temp1_1_1_0_equiv_1 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_mul(ite7_temp1_1_1_0, 2*xc1+1), 2*xc2+1);
    mtbdd_protect(&ite7_temp1_1_1_0_equiv_1);
    MTBDD ite7_temp1_1_1_0_equiv = my_mtbdd_neg(my_mtbdd_plus(ite7_temp1_1_1_0_equiv_0, ite7_temp1_1_1_0_equiv_1));
    mtbdd_protect(&ite7_temp1_1_1_0_equiv);
    MTBDD ite7_frac1 = my_mtbdd_minus(my_mtbdd_minus(ite7_temp1_0_1_1, ite7_temp1_0_1_1_equiv), ite7_temp1_0_1_1_equiv);
    mtbdd_protect(&ite7_frac1);
    MTBDD ite7_frac2 = my_mtbdd_minus(my_mtbdd_minus(ite7_temp1_1_0_1, ite7_temp1_1_0_1_equiv), ite7_temp1_1_0_1_equiv);
    mtbdd_protect(&ite7_frac2);
    MTBDD ite7_frac3 = my_mtbdd_minus(my_mtbdd_minus(ite7_temp1_1_1_0, ite7_temp1_1_1_0_equiv), ite7_temp1_1_1_0_equiv);
    mtbdd_protect(&ite7_frac3);
    MTBDD ite_7 = my_mtbdd_plus(my_mtbdd_plus(ite7_temp1_0_0_0, ite7_frac1), my_mtbdd_plus(ite7_frac2, ite7_frac3));
    mtbdd_protect(&ite_7);

    // final result up to ite_3
    // MTBDD temp_res = my_mtbdd_plus(ite_1, my_mtbdd_plus(ite_2, ite_3));

    MTBDD temp_res = my_mtbdd_plus(ite_1, my_mtbdd_plus(ite_2, my_mtbdd_plus(ite_3, my_mtbdd_plus(ite_4, my_mtbdd_plus(ite_5, my_mtbdd_plus(ite_6, ite_7))))));
    mtbdd_protect(&temp_res);
    
    // 在新的加法操作前，先解除對舊的 res 的保護
    mtbdd_unprotect(&res);
    res = my_mtbdd_plus(res, temp_res);
    mtbdd_protect(&res);

    mtbdd_unprotect(&xc1_0_xc2_0_zt_0);
    mtbdd_unprotect(&without_xc1_0_xc2_0_zt_0);
    mtbdd_unprotect(&xc1_0_xc2_0_zt_0_div2);
    mtbdd_unprotect(&xt_inv_1);
    mtbdd_unprotect(&xt_inv_0);
    mtbdd_unprotect(&xt_inv);
    mtbdd_unprotect(&zc1_inv_1);
    mtbdd_unprotect(&zc1_inv_0);
    mtbdd_unprotect(&zc1_inv);
    mtbdd_unprotect(&zc2_inv_1);
    mtbdd_unprotect(&zc2_inv_0);
    mtbdd_unprotect(&zc2_inv);
    mtbdd_unprotect(&zc1_zc2_inv_1);
    mtbdd_unprotect(&zc1_zc2_inv_0);
    mtbdd_unprotect(&zc1_zc2_inv);
    mtbdd_unprotect(&zc1_xt_inv_1);
    mtbdd_unprotect(&zc1_xt_inv_0);
    mtbdd_unprotect(&zc1_xt_inv);
    mtbdd_unprotect(&zc2_xt_inv_1);
    mtbdd_unprotect(&zc2_xt_inv_0);
    mtbdd_unprotect(&zc2_xt_inv);
    mtbdd_unprotect(&zc1_zc2_xt_inv_1);
    mtbdd_unprotect(&zc1_zc2_xt_inv_0);
    mtbdd_unprotect(&zc1_zc2_xt_inv);
    mtbdd_unprotect(&ite_1);
    mtbdd_unprotect(&ite_2);
    mtbdd_unprotect(&ite_3);
    mtbdd_unprotect(&ite4_temp1);
    mtbdd_unprotect(&ite4_temp1_0_0);
    mtbdd_unprotect(&ite4_temp1_1_1);
    mtbdd_unprotect(&ite4_temp1_1_1_equiv_0);
    mtbdd_unprotect(&ite4_temp1_1_1_equiv_1);
    mtbdd_unprotect(&ite4_temp1_1_1_equiv);
    mtbdd_unprotect(&ite_4);
    mtbdd_unprotect(&ite5_temp1);
    mtbdd_unprotect(&ite5_temp1_0_0);
    mtbdd_unprotect(&ite5_temp1_1_1);
    mtbdd_unprotect(&ite5_temp1_1_1_equiv_0);
    mtbdd_unprotect(&ite5_temp1_1_1_equiv_1);
    mtbdd_unprotect(&ite5_temp1_1_1_equiv);
    mtbdd_unprotect(&ite_5);
    mtbdd_unprotect(&ite6_temp1);
    mtbdd_unprotect(&ite6_temp1_0_0);
    mtbdd_unprotect(&ite6_temp1_1_1);
    mtbdd_unprotect(&ite6_temp1_1_1_equiv_0);
    mtbdd_unprotect(&ite6_temp1_1_1_equiv_1);
    mtbdd_unprotect(&ite6_temp1_1_1_equiv);
    mtbdd_unprotect(&ite_6);
    mtbdd_unprotect(&ite7_temp1);
    mtbdd_unprotect(&ite7_temp1_0_0_0);
    mtbdd_unprotect(&ite7_temp1_0_1_1);
    mtbdd_unprotect(&ite7_temp1_1_0_1);
    mtbdd_unprotect(&ite7_temp1_1_1_0);
    mtbdd_unprotect(&ite7_temp1_0_1_1_equiv_0);
    mtbdd_unprotect(&ite7_temp1_0_1_1_equiv_1);
    mtbdd_unprotect(&ite7_temp1_0_1_1_equiv);
    mtbdd_unprotect(&ite7_temp1_1_0_1_equiv_0);
    mtbdd_unprotect(&ite7_temp1_1_0_1_equiv_1);
    mtbdd_unprotect(&ite7_temp1_1_0_1_equiv);
    mtbdd_unprotect(&ite7_temp1_1_1_0_equiv_0);
    mtbdd_unprotect(&ite7_temp1_1_1_0_equiv_1);
    mtbdd_unprotect(&ite7_temp1_1_1_0_equiv);
    mtbdd_unprotect(&ite7_frac1);
    mtbdd_unprotect(&ite7_frac2);
    mtbdd_unprotect(&ite7_frac3);
    mtbdd_unprotect(&ite_7);
    mtbdd_unprotect(&temp_res);
    mtbdd_unprotect(&res);

    return res;
}

// ccz gate
TASK_IMPL_5(MTBDD, mtbdd_ccz_apply, MTBDD, dd, mtbdd_ccz_apply_op, op, uint32_t, xt, uint32_t, xc1, uint32_t, xc2)
{
    sylvan_gc_test();

    // Check cache using our custom cache function
    MTBDD result;
    if (gate_cache_get4(mtbdd_apply_ccz_gate_id, dd, (size_t)xt, (size_t)xc1, (size_t)xc2, &result)) {
        return result;
    }

    // Apply the operation directly
    result = WRAP(op, dd, xt, xc1, xc2);

    // Store in cache using our custom cache function
    gate_cache_put4(mtbdd_apply_ccz_gate_id, dd, (size_t)xt, (size_t)xc1, (size_t)xc2, result);

    return result;
}


TASK_IMPL_4(MTBDD, _gate_ccz, MTBDD, dd, uint32_t, xt, uint32_t, xc1, uint32_t, xc2)
{
    mtbdd_protect(&dd);
    MTBDD res = my_mtbdd_div_2(dd);
    mtbdd_unprotect(&dd);
    mtbdd_protect(&res);

    MTBDD xc1_0_xc2_0_xt_0 = my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(dd, 2*xc1), 2*xc2), 2*xt);
    mtbdd_protect(&xc1_0_xc2_0_xt_0);

    MTBDD xc1_0_xc2_0_xt_0_div2 = my_mtbdd_div_2(xc1_0_xc2_0_xt_0);
    mtbdd_protect(&xc1_0_xc2_0_xt_0_div2);

    MTBDD without_xc1_0_xc2_0_xt_0 = my_mtbdd_minus(dd, xc1_0_xc2_0_xt_0);
    mtbdd_protect(&without_xc1_0_xc2_0_xt_0);

    // MTBDD without_xc1_0_xc2_0_xt_0_div2 = my_mtbdd_div_2(without_xc1_0_xc2_0_xt_0);
    // mtbdd_protect(&without_xc1_0_xc2_0_xt_0_div2);
    
    mtbdd_unprotect(&res);
    res = my_mtbdd_plus(res, xc1_0_xc2_0_xt_0_div2);
    mtbdd_protect(&res);

    MTBDD zt_inv_1 = my_mtbdd_b_xt_comp_mul(my_mtbdd_t_xt(without_xc1_0_xc2_0_xt_0, 2*xt+1), 2*xt+1);
    mtbdd_protect(&zt_inv_1);
    MTBDD zt_inv_0 = my_mtbdd_b_xt_mul(my_mtbdd_t_xt_comp(without_xc1_0_xc2_0_xt_0, 2*xt+1), 2*xt+1);
    mtbdd_protect(&zt_inv_0);
    MTBDD zt_inv = my_mtbdd_plus(zt_inv_0, zt_inv_1);
    mtbdd_protect(&zt_inv);

    MTBDD zc1_inv_1 = my_mtbdd_b_xt_comp_mul(my_mtbdd_t_xt(without_xc1_0_xc2_0_xt_0, 2*xc1+1), 2*xc1+1);
    mtbdd_protect(&zc1_inv_1);
    MTBDD zc1_inv_0 = my_mtbdd_b_xt_mul(my_mtbdd_t_xt_comp(without_xc1_0_xc2_0_xt_0, 2*xc1+1), 2*xc1+1);
    mtbdd_protect(&zc1_inv_0);
    MTBDD zc1_inv = my_mtbdd_plus(zc1_inv_0, zc1_inv_1);
    mtbdd_protect(&zc1_inv);

    MTBDD zc2_inv_1 = my_mtbdd_b_xt_comp_mul(my_mtbdd_t_xt(without_xc1_0_xc2_0_xt_0, 2*xc2+1), 2*xc2+1);
    mtbdd_protect(&zc2_inv_1);
    MTBDD zc2_inv_0 = my_mtbdd_b_xt_mul(my_mtbdd_t_xt_comp(without_xc1_0_xc2_0_xt_0, 2*xc2+1), 2*xc2+1);
    mtbdd_protect(&zc2_inv_0);
    MTBDD zc2_inv = my_mtbdd_plus(zc2_inv_0, zc2_inv_1);
    mtbdd_protect(&zc2_inv);

    MTBDD zc1_zc2_inv_1 = my_mtbdd_b_xt_comp_mul(my_mtbdd_t_xt(zc1_inv, 2*xc2+1), 2*xc2+1);
    mtbdd_protect(&zc1_zc2_inv_1);
    MTBDD zc1_zc2_inv_0 = my_mtbdd_b_xt_mul(my_mtbdd_t_xt_comp(zc1_inv, 2*xc2+1), 2*xc2+1);
    mtbdd_protect(&zc1_zc2_inv_0);
    MTBDD zc1_zc2_inv = my_mtbdd_plus(zc1_zc2_inv_0, zc1_zc2_inv_1);
    mtbdd_protect(&zc1_zc2_inv);

    MTBDD zc1_zt_inv_1 = my_mtbdd_b_xt_comp_mul(my_mtbdd_t_xt(zt_inv, 2*xc1+1), 2*xc1+1);
    mtbdd_protect(&zc1_zt_inv_1);
    MTBDD zc1_zt_inv_0 = my_mtbdd_b_xt_mul(my_mtbdd_t_xt_comp(zt_inv, 2*xc1+1), 2*xc1+1);
    mtbdd_protect(&zc1_zt_inv_0);
    MTBDD zc1_zt_inv = my_mtbdd_plus(zc1_zt_inv_0, zc1_zt_inv_1);
    mtbdd_protect(&zc1_zt_inv);

    MTBDD zc2_zt_inv_1 = my_mtbdd_b_xt_comp_mul(my_mtbdd_t_xt(zt_inv, 2*xc2+1), 2*xc2+1);
    mtbdd_protect(&zc2_zt_inv_1);
    MTBDD zc2_zt_inv_0 = my_mtbdd_b_xt_mul(my_mtbdd_t_xt_comp(zt_inv, 2*xc2+1), 2*xc2+1);
    mtbdd_protect(&zc2_zt_inv_0);
    MTBDD zc2_zt_inv = my_mtbdd_plus(zc2_zt_inv_0, zc2_zt_inv_1);
    mtbdd_protect(&zc2_zt_inv);

    MTBDD zc1_zc2_zt_inv_1 = my_mtbdd_b_xt_comp_mul(my_mtbdd_t_xt(zc2_zt_inv, 2*xc1+1), 2*xc1+1);
    mtbdd_protect(&zc1_zc2_zt_inv_1);
    MTBDD zc1_zc2_zt_inv_0 = my_mtbdd_b_xt_mul(my_mtbdd_t_xt_comp(zc2_zt_inv, 2*xc1+1), 2*xc1+1);
    mtbdd_protect(&zc1_zc2_zt_inv_0);
    MTBDD zc1_zc2_zt_inv = my_mtbdd_plus(zc1_zc2_zt_inv_0, zc1_zc2_zt_inv_1);
    mtbdd_protect(&zc1_zc2_zt_inv);

    // first ite(xt, 0, 1/2) f(zt')
    MTBDD ite_1 = my_mtbdd_div_2(my_mtbdd_b_xt_comp_mul(zt_inv, 2*xt));
    mtbdd_protect(&ite_1);
    // second ite(xc1, 0, 1/2) f(zc1')
    MTBDD ite_2 = my_mtbdd_div_2(my_mtbdd_b_xt_comp_mul(zc1_inv, 2*xc1));
    mtbdd_protect(&ite_2);
    // third ite(xc2, 0, 1/2) f(zc2')
    MTBDD ite_3 = my_mtbdd_div_2(my_mtbdd_b_xt_comp_mul(zc2_inv, 2*xc2));
    mtbdd_protect(&ite_3);

    // fourth ite(xc1 xor xc2, 0, -1/2) ite(zc1 == zc2 and xc1, -1, 1) f(zc1', zc2')
    MTBDD ite4_temp1 = my_mtbdd_neg(my_mtbdd_div_2(zc1_zc2_inv));
    mtbdd_protect(&ite4_temp1);
    MTBDD ite4_temp1_0_0 = my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(ite4_temp1, 2*xc1), 2*xc2); 
    mtbdd_protect(&ite4_temp1_0_0);
    MTBDD ite4_temp1_1_1 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_mul(ite4_temp1, 2*xc1), 2*xc2);
    mtbdd_protect(&ite4_temp1_1_1);
    MTBDD ite4_temp1_1_1_equiv_0 = my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(ite4_temp1_1_1, 2*xc1+1), 2*xc2+1);
    mtbdd_protect(&ite4_temp1_1_1_equiv_0);
    MTBDD ite4_temp1_1_1_equiv_1 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_mul(ite4_temp1_1_1, 2*xc1+1), 2*xc2+1);
    mtbdd_protect(&ite4_temp1_1_1_equiv_1);
    MTBDD ite4_temp1_1_1_equiv = my_mtbdd_plus(ite4_temp1_1_1_equiv_0, ite4_temp1_1_1_equiv_1);
    mtbdd_protect(&ite4_temp1_1_1_equiv);
    MTBDD ite_4 = my_mtbdd_minus(my_mtbdd_minus(my_mtbdd_plus(ite4_temp1_0_0, ite4_temp1_1_1), ite4_temp1_1_1_equiv), ite4_temp1_1_1_equiv);
    mtbdd_protect(&ite_4);

    // fifth ite(xc2 and xt, 0, -1/2) ite(zc2 == zt and xc2, -1, 1) f(zc2', zt')
    MTBDD ite5_temp1 = my_mtbdd_neg(my_mtbdd_div_2(zc2_zt_inv));
    mtbdd_protect(&ite5_temp1);
    MTBDD ite5_temp1_0_0 = my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(ite5_temp1, 2*xc2), 2*xt);
    mtbdd_protect(&ite5_temp1_0_0);
    MTBDD ite5_temp1_1_1 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_mul(ite5_temp1, 2*xc2), 2*xt);
    mtbdd_protect(&ite5_temp1_1_1);
    MTBDD ite5_temp1_1_1_equiv_0 = my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(ite5_temp1_1_1, 2*xc2+1), 2*xt+1);
    mtbdd_protect(&ite5_temp1_1_1_equiv_0);
    MTBDD ite5_temp1_1_1_equiv_1 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_mul(ite5_temp1_1_1, 2*xc2+1), 2*xt+1);
    mtbdd_protect(&ite5_temp1_1_1_equiv_1);
    MTBDD ite5_temp1_1_1_equiv = my_mtbdd_plus(ite5_temp1_1_1_equiv_0, ite5_temp1_1_1_equiv_1);
    mtbdd_protect(&ite5_temp1_1_1_equiv);
    MTBDD ite_5 = my_mtbdd_minus(my_mtbdd_minus(my_mtbdd_plus(ite5_temp1_0_0, ite5_temp1_1_1), ite5_temp1_1_1_equiv), ite5_temp1_1_1_equiv);
    mtbdd_protect(&ite_5);

    // sixth ite(xc1 and xt, 0, -1/2) ite(zc1 == zt and xc1, -1, 1) f(zc1', zt')
    MTBDD ite6_temp1 = my_mtbdd_neg(my_mtbdd_div_2(zc1_zt_inv));
    mtbdd_protect(&ite6_temp1);
    MTBDD ite6_temp1_0_0 = my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(ite6_temp1, 2*xc1), 2*xt);
    mtbdd_protect(&ite6_temp1_0_0);
    MTBDD ite6_temp1_1_1 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_mul(ite6_temp1, 2*xc1), 2*xt);
    mtbdd_protect(&ite6_temp1_1_1);
    MTBDD ite6_temp1_1_1_equiv_0 = my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(ite6_temp1_1_1, 2*xc1+1), 2*xt+1);
    mtbdd_protect(&ite6_temp1_1_1_equiv_0);
    MTBDD ite6_temp1_1_1_equiv_1 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_mul(ite6_temp1_1_1, 2*xc1+1), 2*xt+1);
    mtbdd_protect(&ite6_temp1_1_1_equiv_1);
    MTBDD ite6_temp1_1_1_equiv = my_mtbdd_plus(ite6_temp1_1_1_equiv_0, ite6_temp1_1_1_equiv_1);
    mtbdd_protect(&ite6_temp1_1_1_equiv);
    MTBDD ite_6 = my_mtbdd_minus(my_mtbdd_minus(my_mtbdd_plus(ite6_temp1_0_0, ite6_temp1_1_1), ite6_temp1_1_1_equiv), ite6_temp1_1_1_equiv);
    mtbdd_protect(&ite_6);

    // seventh ite(xc1 xor xc2 xor xt, 0, 1/2) ite((xc1 == xc2 and zc1 == zc2 and xc1) or (xc1 == xt and zc1 == zt and xc1) or (xc2 == xt and zc2 == zt and xc2), -1, 1) f(zc1', zc2', zt')
    MTBDD ite7_temp1 = my_mtbdd_div_2(zc1_zc2_zt_inv);
    mtbdd_protect(&ite7_temp1);
    MTBDD ite7_temp1_0_0_0 = my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(ite7_temp1, 2*xc1), 2*xc2), 2*xt);
    mtbdd_protect(&ite7_temp1_0_0_0);
    MTBDD ite7_temp1_0_1_1 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_mul(my_mtbdd_b_xt_comp_mul(ite7_temp1, 2*xc1), 2*xc2), 2*xt);
    mtbdd_protect(&ite7_temp1_0_1_1);
    MTBDD ite7_temp1_1_0_1 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_mul(my_mtbdd_b_xt_comp_mul(ite7_temp1, 2*xc2), 2*xt), 2*xc1);
    mtbdd_protect(&ite7_temp1_1_0_1);
    MTBDD ite7_temp1_1_1_0 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_mul(ite7_temp1, 2*xc2), 2*xt), 2*xc1);
    mtbdd_protect(&ite7_temp1_1_1_0);
    MTBDD ite7_temp1_0_1_1_equiv_0 = my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(ite7_temp1_0_1_1, 2*xc2+1), 2*xt+1);
    mtbdd_protect(&ite7_temp1_0_1_1_equiv_0);
    MTBDD ite7_temp1_0_1_1_equiv_1 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_mul(ite7_temp1_0_1_1, 2*xc2+1), 2*xt+1);
    mtbdd_protect(&ite7_temp1_0_1_1_equiv_1);
    MTBDD ite7_temp1_0_1_1_equiv = my_mtbdd_neg(my_mtbdd_plus(ite7_temp1_0_1_1_equiv_0, ite7_temp1_0_1_1_equiv_1));
    mtbdd_protect(&ite7_temp1_0_1_1_equiv);
    MTBDD ite7_temp1_1_0_1_equiv_0 = my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(ite7_temp1_1_0_1, 2*xt+1), 2*xc1+1);
    mtbdd_protect(&ite7_temp1_1_0_1_equiv_0);
    MTBDD ite7_temp1_1_0_1_equiv_1 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_mul(ite7_temp1_1_0_1, 2*xt+1), 2*xc1+1);
    mtbdd_protect(&ite7_temp1_1_0_1_equiv_1);
    MTBDD ite7_temp1_1_0_1_equiv = my_mtbdd_neg(my_mtbdd_plus(ite7_temp1_1_0_1_equiv_0, ite7_temp1_1_0_1_equiv_1));
    mtbdd_protect(&ite7_temp1_1_0_1_equiv);
    MTBDD ite7_temp1_1_1_0_equiv_0 = my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(ite7_temp1_1_1_0, 2*xc1+1), 2*xc2+1);
    mtbdd_protect(&ite7_temp1_1_1_0_equiv_0);
    MTBDD ite7_temp1_1_1_0_equiv_1 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_mul(ite7_temp1_1_1_0, 2*xc1+1), 2*xc2+1);
    mtbdd_protect(&ite7_temp1_1_1_0_equiv_1);
    MTBDD ite7_temp1_1_1_0_equiv = my_mtbdd_neg(my_mtbdd_plus(ite7_temp1_1_1_0_equiv_0, ite7_temp1_1_1_0_equiv_1));
    mtbdd_protect(&ite7_temp1_1_1_0_equiv);
    MTBDD ite7_frac1 = my_mtbdd_minus(my_mtbdd_minus(ite7_temp1_0_1_1, ite7_temp1_0_1_1_equiv), ite7_temp1_0_1_1_equiv);
    mtbdd_protect(&ite7_frac1);
    MTBDD ite7_frac2 = my_mtbdd_minus(my_mtbdd_minus(ite7_temp1_1_0_1, ite7_temp1_1_0_1_equiv), ite7_temp1_1_0_1_equiv);
    mtbdd_protect(&ite7_frac2);
    MTBDD ite7_frac3 = my_mtbdd_minus(my_mtbdd_minus(ite7_temp1_1_1_0, ite7_temp1_1_1_0_equiv), ite7_temp1_1_1_0_equiv);
    mtbdd_protect(&ite7_frac3);
    MTBDD ite_7 = my_mtbdd_plus(my_mtbdd_plus(ite7_temp1_0_0_0, ite7_frac1), my_mtbdd_plus(ite7_frac2, ite7_frac3));
    mtbdd_protect(&ite_7);

    // final result up to ite_3
    // MTBDD temp_res = my_mtbdd_plus(ite_1, my_mtbdd_plus(ite_2, ite_3));

    MTBDD temp_res = my_mtbdd_plus(ite_1, my_mtbdd_plus(ite_2, my_mtbdd_plus(ite_3, my_mtbdd_plus(ite_4, my_mtbdd_plus(ite_5, my_mtbdd_plus(ite_6, ite_7))))));
    mtbdd_protect(&temp_res);
    mtbdd_unprotect(&res);
    res = my_mtbdd_plus(res, temp_res);
    mtbdd_protect(&res);

    mtbdd_unprotect(&xc1_0_xc2_0_xt_0);
    mtbdd_unprotect(&without_xc1_0_xc2_0_xt_0);
    mtbdd_unprotect(&xc1_0_xc2_0_xt_0_div2);
    mtbdd_unprotect(&zt_inv_1);
    mtbdd_unprotect(&zt_inv_0);
    mtbdd_unprotect(&zt_inv);
    mtbdd_unprotect(&zc1_inv_1);
    mtbdd_unprotect(&zc1_inv_0);
    mtbdd_unprotect(&zc1_inv);
    mtbdd_unprotect(&zc2_inv_1);
    mtbdd_unprotect(&zc2_inv_0);
    mtbdd_unprotect(&zc2_inv);
    mtbdd_unprotect(&zc1_zc2_inv_1);
    mtbdd_unprotect(&zc1_zc2_inv_0);
    mtbdd_unprotect(&zc1_zc2_inv);
    mtbdd_unprotect(&zc1_zt_inv_1);
    mtbdd_unprotect(&zc1_zt_inv_0);
    mtbdd_unprotect(&zc1_zt_inv);
    mtbdd_unprotect(&zc2_zt_inv_1);
    mtbdd_unprotect(&zc2_zt_inv_0);
    mtbdd_unprotect(&zc2_zt_inv);
    mtbdd_unprotect(&zc1_zc2_zt_inv_1);
    mtbdd_unprotect(&zc1_zc2_zt_inv_0);
    mtbdd_unprotect(&zc1_zc2_zt_inv);
    mtbdd_unprotect(&ite_1);
    mtbdd_unprotect(&ite_2);
    mtbdd_unprotect(&ite_3);
    mtbdd_unprotect(&ite4_temp1);
    mtbdd_unprotect(&ite4_temp1_0_0);
    mtbdd_unprotect(&ite4_temp1_1_1);
    mtbdd_unprotect(&ite4_temp1_1_1_equiv_0);
    mtbdd_unprotect(&ite4_temp1_1_1_equiv_1);
    mtbdd_unprotect(&ite4_temp1_1_1_equiv);
    mtbdd_unprotect(&ite_4);
    mtbdd_unprotect(&ite5_temp1);
    mtbdd_unprotect(&ite5_temp1_0_0);
    mtbdd_unprotect(&ite5_temp1_1_1);
    mtbdd_unprotect(&ite5_temp1_1_1_equiv_0);
    mtbdd_unprotect(&ite5_temp1_1_1_equiv_1);
    mtbdd_unprotect(&ite5_temp1_1_1_equiv);
    mtbdd_unprotect(&ite_5);
    mtbdd_unprotect(&ite6_temp1);
    mtbdd_unprotect(&ite6_temp1_0_0);
    mtbdd_unprotect(&ite6_temp1_1_1);
    mtbdd_unprotect(&ite6_temp1_1_1_equiv_0);
    mtbdd_unprotect(&ite6_temp1_1_1_equiv_1);
    mtbdd_unprotect(&ite6_temp1_1_1_equiv);
    mtbdd_unprotect(&ite_6);
    mtbdd_unprotect(&ite7_temp1);
    mtbdd_unprotect(&ite7_temp1_0_0_0);
    mtbdd_unprotect(&ite7_temp1_0_1_1);
    mtbdd_unprotect(&ite7_temp1_1_0_1);
    mtbdd_unprotect(&ite7_temp1_1_1_0);
    mtbdd_unprotect(&ite7_temp1_0_1_1_equiv_0);
    mtbdd_unprotect(&ite7_temp1_0_1_1_equiv_1);
    mtbdd_unprotect(&ite7_temp1_0_1_1_equiv);
    mtbdd_unprotect(&ite7_temp1_1_0_1_equiv_0);
    mtbdd_unprotect(&ite7_temp1_1_0_1_equiv_1);
    mtbdd_unprotect(&ite7_temp1_1_0_1_equiv);
    mtbdd_unprotect(&ite7_temp1_1_1_0_equiv_0);
    mtbdd_unprotect(&ite7_temp1_1_1_0_equiv_1);
    mtbdd_unprotect(&ite7_temp1_1_1_0_equiv);
    mtbdd_unprotect(&ite7_frac1);
    mtbdd_unprotect(&ite7_frac2);
    mtbdd_unprotect(&ite7_frac3);
    mtbdd_unprotect(&ite_7);
    mtbdd_unprotect(&temp_res);
    mtbdd_unprotect(&res);

    return res;
}

void gate_x(MTBDD *p_t, uint32_t xt)
{
    // printf("gate_x\n");
    // *p_t = mtbdd_uapply(*p_t, TASK(_gate_x), xt);
    *p_t = my_mtbdd_apply_gate_x(*p_t, TASK(_gate_x), xt);
    // size_t total_nodes = mtbdd_nodecount(*p_t);
    // printf("X gate nodes: %zu\n", total_nodes);

    // MTBDD t = *p_t;
    // mtbdd_protect(&t);
    // MTBDD zt_1 = my_mtbdd_neg(my_mtbdd_b_xt_mul(t, 2*xt+1));
    // mtbdd_protect(&zt_1);
    // MTBDD zt_0 = my_mtbdd_b_xt_comp_mul(t, 2*xt+1);
    // mtbdd_protect(&zt_0);
    
    // MTBDD res = my_mtbdd_plus(zt_0, zt_1);
    // mtbdd_protect(&res);

    // *p_t = res;

    // mtbdd_unprotect(&t);
    // mtbdd_unprotect(&zt_1);
    // mtbdd_unprotect(&zt_0);
    // mtbdd_unprotect(&res);
}

void gate_y(MTBDD *p_t, uint32_t xt)
{
    // printf("gate_y\n");
    // *p_t = mtbdd_uapply(*p_t, TASK(_gate_y), xt);
    // size_t total_nodes = mtbdd_nodecount(*p_t);
    // printf("Y gate nodes: %zu\n", total_nodes);
    *p_t = my_mtbdd_apply_gate(*p_t, TASK(_gate_y), xt);

    // MTBDD t = *p_t;
    // mtbdd_protect(&t);
    // MTBDD xt_0_zt_1 = my_mtbdd_neg(my_mtbdd_b_xt_mul(my_mtbdd_b_xt_comp_mul(t, 2*xt), 2*xt+1));
    // mtbdd_protect(&xt_0_zt_1);
    // MTBDD xt_1_zt_0 = my_mtbdd_neg(my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_mul(t, 2*xt), 2*xt+1));
    // mtbdd_protect(&xt_1_zt_0);
    // MTBDD xt_1_zt_1 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_mul(t, 2*xt), 2*xt+1);
    // mtbdd_protect(&xt_1_zt_1);
    // MTBDD xt_0_zt_0 = my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(t, 2*xt), 2*xt+1);
    // mtbdd_protect(&xt_0_zt_0);

    // MTBDD temp1 = my_mtbdd_plus(xt_0_zt_1, xt_1_zt_0);
    // mtbdd_protect(&temp1);
    // MTBDD temp2 = my_mtbdd_plus(xt_1_zt_1, xt_0_zt_0);
    // mtbdd_protect(&temp2);

    // MTBDD res = my_mtbdd_plus(temp1, temp2);
    // mtbdd_protect(&res);

    // *p_t = res;

    // mtbdd_unprotect(&t);
    // mtbdd_unprotect(&xt_0_zt_1);
    // mtbdd_unprotect(&xt_1_zt_0);
    // mtbdd_unprotect(&xt_1_zt_1);
    // mtbdd_unprotect(&xt_0_zt_0);
    // mtbdd_unprotect(&temp1);
    // mtbdd_unprotect(&temp2);
    // mtbdd_unprotect(&res);
}



// Modify gate functions to use cache
void gate_z(MTBDD *p_t, uint32_t xt)
{
    // printf("gate_z\n");
    // *p_t = mtbdd_uapply(*p_t, TASK(_gate_z), xt);
    // size_t total_nodes = mtbdd_nodecount(*p_t);
    // printf("Z gate nodes: %zu\n", total_nodes);
    *p_t = my_mtbdd_apply_gate(*p_t, TASK(_gate_z), xt);

    // MTBDD t = *p_t;
    // mtbdd_protect(&t);
    
    // // Original computation
    // MTBDD xt_1_zt_0 = my_mtbdd_neg(my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_mul(t, 2*xt), 2*xt+1));
    // mtbdd_protect(&xt_1_zt_0);

    // MTBDD res = my_mtbdd_plus(my_mtbdd_plus(t, xt_1_zt_0), xt_1_zt_0);
    // mtbdd_protect(&res);

    // *p_t = res;

    // mtbdd_unprotect(&t);
    // mtbdd_unprotect(&xt_1_zt_0);
    // mtbdd_unprotect(&res);
}

void gate_s(MTBDD *p_t, uint32_t xt)
{
    //  printf("gate_s\n");
    // *p_t = mtbdd_uapply(*p_t, TASK(_gate_s), xt);
    // size_t total_nodes = mtbdd_nodecount(*p_t);
    *p_t = my_mtbdd_apply_gate(*p_t, TASK(_gate_s), xt);
    // printf("S gate nodes: %zu\n", total_nodes);

    // MTBDD t = *p_t;
    // mtbdd_protect(&t);
    // MTBDD xt_0 = my_mtbdd_b_xt_comp_mul(t, 2*xt);
    // mtbdd_protect(&xt_0);

    // MTBDD xt_1_zt_0 = my_mtbdd_neg(my_mtbdd_b_xt_comp_mul(my_mtbdd_t_xt(my_mtbdd_b_xt_mul(t, 2*xt), 2*xt+1), 2*xt+1));
    // mtbdd_protect(&xt_1_zt_0);
    // MTBDD xt_1_zt_1 = my_mtbdd_b_xt_mul(my_mtbdd_t_xt_comp(my_mtbdd_b_xt_mul(t, 2*xt), 2*xt+1), 2*xt+1);
    // mtbdd_protect(&xt_1_zt_1);

    // MTBDD temp1 = my_mtbdd_plus(xt_1_zt_0, xt_1_zt_1);
    // mtbdd_protect(&temp1);
    // MTBDD res = my_mtbdd_plus(temp1, xt_0);
    // mtbdd_protect(&res);

    // *p_t = res;
    
    // mtbdd_unprotect(&t);
    // mtbdd_unprotect(&xt_0);
    // mtbdd_unprotect(&xt_1_zt_0);
    // mtbdd_unprotect(&xt_1_zt_1);
    // mtbdd_unprotect(&temp1);
    // mtbdd_unprotect(&res);
}

void gate_t(MTBDD *p_t, uint32_t xt)
{
    // printf("gate_t\n");
    // *p_t = mtbdd_uapply(*p_t, TASK(_gate_t), xt);
    // size_t total_nodes = mtbdd_nodecount(*p_t);
    // printf("T gate nodes: %zu\n", total_nodes);
    *p_t = my_mtbdd_apply_gate(*p_t, TASK(_gate_t), xt);

    // MTBDD t = *p_t;
    // mtbdd_protect(&t);
    // MTBDD xt_0 = my_mtbdd_b_xt_comp_mul(t, 2*xt);
    // mtbdd_protect(&xt_0);
    // MTBDD xt_1 = my_mtbdd_b_xt_mul(t, 2*xt);
    // mtbdd_protect(&xt_1);
    // MTBDD orig_xt_1_zt_0 = my_mtbdd_b_xt_comp_mul(xt_1, 2*xt+1);
    // mtbdd_protect(&orig_xt_1_zt_0);
    // MTBDD orig_xt_1_zt_1 = my_mtbdd_b_xt_mul(xt_1, 2*xt+1);
    // mtbdd_protect(&orig_xt_1_zt_1);
    // MTBDD orig_xt_1_zt_0_neg = my_mtbdd_b_xt_mul(my_mtbdd_t_xt_comp(xt_1, 2*xt+1), 2*xt+1);
    // mtbdd_protect(&orig_xt_1_zt_0_neg);
    // MTBDD orig_xt_1_zt_1_neg = my_mtbdd_b_xt_comp_mul(my_mtbdd_t_xt(xt_1, 2*xt+1), 2*xt+1);
    // mtbdd_protect(&orig_xt_1_zt_1_neg);

    // MTBDD res_xt_1_zt_0 = my_mtbdd_div_root2(my_mtbdd_minus(orig_xt_1_zt_0, orig_xt_1_zt_1_neg));
    // mtbdd_protect(&res_xt_1_zt_0);
    // MTBDD res_xt_1_zt_1 = my_mtbdd_div_root2(my_mtbdd_plus(orig_xt_1_zt_1, orig_xt_1_zt_0_neg));
    // mtbdd_protect(&res_xt_1_zt_1);

    // MTBDD res = my_mtbdd_plus(xt_0, my_mtbdd_plus(res_xt_1_zt_0, res_xt_1_zt_1));
    // mtbdd_protect(&res);

    // *p_t = res;

    // mtbdd_unprotect(&t);
    // mtbdd_unprotect(&xt_0);
    // mtbdd_unprotect(&xt_1);
    // mtbdd_unprotect(&orig_xt_1_zt_0);
    // mtbdd_unprotect(&orig_xt_1_zt_1);
    // mtbdd_unprotect(&orig_xt_1_zt_0_neg);
    // mtbdd_unprotect(&orig_xt_1_zt_1_neg);
    // mtbdd_unprotect(&res_xt_1_zt_0);
    // mtbdd_unprotect(&res_xt_1_zt_1);
    // mtbdd_unprotect(&res);
}

void gate_h(MTBDD *p_t, uint32_t xt)
{   
    // printf("gate_h\n");
    // *p_t = mtbdd_uapply(*p_t, TASK(_gate_h), xt);
    *p_t = my_mtbdd_apply_gate(*p_t, TASK(_gate_h), xt);
    // size_t total_nodes = mtbdd_nodecount(*p_t);
    // printf("H gate nodes: %zu\n", total_nodes);

    // MTBDD t = *p_t;
    // mtbdd_protect(&t);
    // MTBDD xt_1_zt_1 = my_mtbdd_neg(my_mtbdd_b_xt_mul(my_mtbdd_b_xt_mul(t, 2*xt), 2*xt+1));
    // mtbdd_protect(&xt_1_zt_1);
    // MTBDD xt_0_zt_0 = my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(t, 2*xt), 2*xt+1);
    // mtbdd_protect(&xt_0_zt_0);

    // // deal with the 10 to 01
    // MTBDD xt_1_zt_0 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_comp_mul(my_mtbdd_t_xt_comp(my_mtbdd_t_xt(t, 2*xt), 2*xt+1), 2*xt), 2*xt+1);
    // mtbdd_protect(&xt_1_zt_0);
    // MTBDD xt_0_zt_1 = my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_mul(my_mtbdd_t_xt(my_mtbdd_t_xt_comp(t, 2*xt), 2*xt+1), 2*xt), 2*xt+1);
    // mtbdd_protect(&xt_0_zt_1);

    // MTBDD temp1 = my_mtbdd_plus(xt_1_zt_0, xt_0_zt_1);
    // mtbdd_protect(&temp1);

    // MTBDD temp2 = my_mtbdd_plus(xt_1_zt_1, xt_0_zt_0);
    // mtbdd_protect(&temp2);

    // MTBDD res = my_mtbdd_plus(temp1, temp2);
    // mtbdd_protect(&res);

    // *p_t = res;

    // mtbdd_unprotect(&t);
    // mtbdd_unprotect(&xt_1_zt_1);
    // mtbdd_unprotect(&xt_0_zt_0);
    // mtbdd_unprotect(&xt_1_zt_0);
    // mtbdd_unprotect(&xt_0_zt_1);
    // mtbdd_unprotect(&temp1);
    // mtbdd_unprotect(&temp2);
    // mtbdd_unprotect(&res);
}

// void gate_rx_pihalf(MTBDD *p_t, uint32_t xt)
// {
//     *p_t = my_mtbdd_apply_gate(*p_t, TASK(_gate_rx_pihalf), xt);
//     mpz_add_ui(c_k, c_k, 1);
// }

// void gate_ry_pihalf(MTBDD *p_t, uint32_t xt)
// {
//     *p_t = my_mtbdd_apply_gate(*p_t, TASK(_gate_ry_pihalf), xt);
//     mpz_add_ui(c_k, c_k, 1);
// }

void gate_cnot(MTBDD *p_t, uint32_t xt, uint32_t xc)
{
    // printf("gate_cnot\n");

    *p_t = mtbdd_cnot_apply(*p_t, TASK(_gate_cnot), xt, xc);
    // size_t total_nodes = mtbdd_nodecount(*p_t);
    // printf("CNOT gate nodes: %zu\n", total_nodes);

    // MTBDD t = *p_t;
    // mtbdd_protect(&t);

    // MTBDD xc_0 = my_mtbdd_b_xt_comp_mul(t, 2*xc);
    // mtbdd_protect(&xc_0);

    // MTBDD xc_0_tzc_1 = my_mtbdd_t_xt(xc_0, 2*xc+1);
    // mtbdd_protect(&xc_0_tzc_1);
    // MTBDD xc_0_tzc_1_zt_1 = my_mtbdd_b_xt_mul(xc_0_tzc_1, 2*xt+1);
    // mtbdd_protect(&xc_0_tzc_1_zt_1);
    // MTBDD xc_0_tzc_1_zt_1_inv = my_mtbdd_b_xt_comp_mul(xc_0_tzc_1_zt_1, 2*xc+1);
    // mtbdd_protect(&xc_0_tzc_1_zt_1_inv);
    // MTBDD xc_0_zc_0 = my_mtbdd_b_xt_comp_mul(xc_0, 2*xc+1);
    // mtbdd_protect(&xc_0_zc_0);
    // MTBDD xc_0_zc_0_orig = my_mtbdd_b_xt_comp_mul(xc_0_zc_0, 2*xt+1);
    // mtbdd_protect(&xc_0_zc_0_orig);
    // MTBDD first = my_mtbdd_plus(xc_0_zc_0_orig, xc_0_tzc_1_zt_1_inv);
    // mtbdd_protect(&first);

    // MTBDD xc_0_tzc_0 = my_mtbdd_t_xt_comp(xc_0, 2*xc+1);
    // mtbdd_protect(&xc_0_tzc_0);
    // MTBDD xc_0_tzc_0_zt_1 = my_mtbdd_b_xt_mul(xc_0_tzc_0, 2*xt+1);
    // mtbdd_protect(&xc_0_tzc_0_zt_1);
    // MTBDD xc_0_tzc_0_zt_1_inv = my_mtbdd_b_xt_mul(xc_0_tzc_0_zt_1, 2*xc+1);
    // mtbdd_protect(&xc_0_tzc_0_zt_1_inv);
    // MTBDD xc_0_zc_1 = my_mtbdd_b_xt_mul(xc_0, 2*xc+1);
    // mtbdd_protect(&xc_0_zc_1);
    // MTBDD xc_0_zc_1_orig = my_mtbdd_b_xt_comp_mul(xc_0_zc_1, 2*xt+1);
    // mtbdd_protect(&xc_0_zc_1_orig);
    // MTBDD second = my_mtbdd_plus(xc_0_zc_1_orig, xc_0_tzc_0_zt_1_inv);
    // mtbdd_protect(&second);

    // MTBDD res1 = my_mtbdd_plus(first, second);
    // mtbdd_protect(&res1);

    // mtbdd_unprotect(&xc_0);
    // mtbdd_unprotect(&xc_0_zc_0);
    // mtbdd_unprotect(&xc_0_zc_1);
    // mtbdd_unprotect(&xc_0_zc_0_orig);
    // mtbdd_unprotect(&xc_0_zc_1_orig);
    // mtbdd_unprotect(&xc_0_tzc_0);
    // mtbdd_unprotect(&xc_0_tzc_0_zt_1);
    // mtbdd_unprotect(&xc_0_tzc_0_zt_1_inv);
    // mtbdd_unprotect(&xc_0_tzc_1);
    // mtbdd_unprotect(&xc_0_tzc_1_zt_1);
    // mtbdd_unprotect(&xc_0_tzc_1_zt_1_inv);
    // mtbdd_unprotect(&first);
    // mtbdd_unprotect(&second);

    // MTBDD xc_1 = my_mtbdd_b_xt_mul(t, 2*xc);
    // mtbdd_protect(&xc_1);

    // MTBDD xc_1_tzc_1 = my_mtbdd_t_xt(xc_1, 2*xc+1);
    // mtbdd_protect(&xc_1_tzc_1);
    // MTBDD xc_1_tzc_1_zt_1 = my_mtbdd_b_xt_mul(xc_1_tzc_1, 2*xt+1);
    // mtbdd_protect(&xc_1_tzc_1_zt_1);
    // MTBDD xc_1_tzc_1_zt_1_inv = my_mtbdd_b_xt_comp_mul(xc_1_tzc_1_zt_1, 2*xc+1);
    // mtbdd_protect(&xc_1_tzc_1_zt_1_inv);
    // MTBDD xc_1_zc_0 = my_mtbdd_b_xt_comp_mul(xc_1, 2*xc+1);
    // mtbdd_protect(&xc_1_zc_0); 
    // MTBDD xc_1_zc_0_orig = my_mtbdd_b_xt_comp_mul(xc_1_zc_0, 2*xt+1);
    // mtbdd_protect(&xc_1_zc_0_orig);
    // MTBDD left_bracket = my_mtbdd_plus(xc_1_zc_0_orig, xc_1_tzc_1_zt_1_inv);
    // mtbdd_protect(&left_bracket);
    
    // MTBDD xc_1_tzc_0 = my_mtbdd_t_xt_comp(xc_1, 2*xc+1);
    // mtbdd_protect(&xc_1_tzc_0);
    // MTBDD xc_1_tzc_0_zt_1 = my_mtbdd_b_xt_mul(xc_1_tzc_0, 2*xt+1);
    // mtbdd_protect(&xc_1_tzc_0_zt_1);
    // MTBDD xc_1_tzc_0_zt_1_inv = my_mtbdd_b_xt_mul(xc_1_tzc_0_zt_1, 2*xc+1);
    // mtbdd_protect(&xc_1_tzc_0_zt_1_inv);
    // MTBDD xc_1_zc_1 = my_mtbdd_b_xt_mul(xc_1, 2*xc+1);
    // mtbdd_protect(&xc_1_zc_1);
    // MTBDD xc_1_zc_1_orig = my_mtbdd_b_xt_comp_mul(xc_1_zc_1, 2*xt+1);
    // mtbdd_protect(&xc_1_zc_1_orig);
    // MTBDD right_bracket = my_mtbdd_plus(xc_1_zc_1_orig, xc_1_tzc_0_zt_1_inv);
    // mtbdd_protect(&right_bracket);

    // MTBDD temp = my_mtbdd_plus(left_bracket, right_bracket);
    // mtbdd_protect(&temp);
    // // deal with the xt inv
    // MTBDD temp_xt_1 = my_mtbdd_t_xt(temp, 2*xt);
    // mtbdd_protect(&temp_xt_1);
    // MTBDD temp_xt_1_inv = my_mtbdd_b_xt_comp_mul(temp_xt_1, 2*xt);
    // mtbdd_protect(&temp_xt_1_inv);

    // MTBDD temp_xt_0 = my_mtbdd_t_xt_comp(temp, 2*xt);
    // mtbdd_protect(&temp_xt_0);
    // MTBDD temp_xt_0_inv = my_mtbdd_b_xt_mul(temp_xt_0, 2*xt);
    // mtbdd_protect(&temp_xt_0_inv);

    // MTBDD res2 = my_mtbdd_plus(temp_xt_0_inv, temp_xt_1_inv);
    // mtbdd_protect(&res2);

    // mtbdd_unprotect(&xc_1);
    // mtbdd_unprotect(&xc_1_zc_0);
    // mtbdd_unprotect(&xc_1_zc_1);
    // mtbdd_unprotect(&xc_1_zc_0_orig);
    // mtbdd_unprotect(&xc_1_zc_1_orig);
    // mtbdd_unprotect(&xc_1_tzc_0);
    // mtbdd_unprotect(&xc_1_tzc_0_zt_1);
    // mtbdd_unprotect(&xc_1_tzc_0_zt_1_inv);
    // mtbdd_unprotect(&xc_1_tzc_1);
    // mtbdd_unprotect(&xc_1_tzc_1_zt_1);
    // mtbdd_unprotect(&xc_1_tzc_1_zt_1_inv);
    // mtbdd_unprotect(&temp);
    // mtbdd_unprotect(&temp_xt_0);
    // mtbdd_unprotect(&temp_xt_0_inv);
    // mtbdd_unprotect(&temp_xt_1);
    // mtbdd_unprotect(&temp_xt_1_inv);
    // mtbdd_unprotect(&left_bracket);
    // mtbdd_unprotect(&right_bracket);

    // MTBDD temp_res = my_mtbdd_plus(res1, res2);
    // mtbdd_protect(&temp_res);

    // mtbdd_unprotect(&res1);
    // mtbdd_unprotect(&res2);
    // mtbdd_unprotect(&t);

    // MTBDD xc_1_xt_1_zc_1_zt_1 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_mul(my_mtbdd_b_xt_mul(my_mtbdd_b_xt_mul(temp_res, 2*xc), 2*xc+1), 2*xt), 2*xt+1);
    // mtbdd_protect(&xc_1_xt_1_zc_1_zt_1);

    // MTBDD xc_1_xt_0_zc_0_zt_1 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_mul(temp_res, 2*xc), 2*xc+1), 2*xt), 2*xt+1);
    // mtbdd_protect(&xc_1_xt_0_zc_0_zt_1);

    // MTBDD temp2 = my_mtbdd_plus(xc_1_xt_1_zc_1_zt_1, xc_1_xt_0_zc_0_zt_1);
    // // temp2 = my_mtbdd_neg(temp2);
    // mtbdd_protect(&temp2);

    // MTBDD res = my_mtbdd_minus(my_mtbdd_minus(temp_res, temp2), temp2);
    // mtbdd_protect(&res);
    // mtbdd_unprotect(&temp_res);
    // mtbdd_unprotect(&temp2);
    // mtbdd_unprotect(&xc_1_xt_1_zc_1_zt_1);
    // mtbdd_unprotect(&xc_1_xt_0_zc_0_zt_1);
    // *p_t = res;
    // mtbdd_unprotect(&res);
}

void gate_cz(MTBDD *p_t, uint32_t xt, uint32_t xc)
{
    // printf("gate_cz\n");
    *p_t = mtbdd_cz_apply(*p_t, TASK(_gate_cz), xc, xt);
    // size_t total_nodes = mtbdd_nodecount(*p_t);
    // printf("CZ gate nodes: %zu\n", total_nodes);

    // MTBDD t = *p_t;
    // mtbdd_protect(&t);

    // MTBDD xc_0_xt_0 = my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(t, 2*xc), 2*xt);
    // mtbdd_protect(&xc_0_xt_0);
    // MTBDD xc_0_xt_1 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_comp_mul(t, 2*xc), 2*xt);
    // mtbdd_protect(&xc_0_xt_1);
    // MTBDD bracket_left = my_mtbdd_b_xt_mul(my_mtbdd_t_xt_comp(xc_0_xt_1, 2*xc+1), 2*xc+1);
    // mtbdd_protect(&bracket_left);
    // MTBDD bracket_right = my_mtbdd_b_xt_comp_mul(my_mtbdd_t_xt(xc_0_xt_1, 2*xc+1), 2*xc+1);
    // mtbdd_protect(&bracket_right);
    // MTBDD inter_res = my_mtbdd_plus(bracket_left, bracket_right);
    // mtbdd_protect(&inter_res);
    // MTBDD inter_res_2 = my_mtbdd_plus(inter_res, xc_0_xt_0);
    // mtbdd_protect(&inter_res_2);
    
    // MTBDD xc_1_xt_0 = my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_mul(t, 2*xc), 2*xt);
    // mtbdd_protect(&xc_1_xt_0);
    // MTBDD bracket_left_2 = my_mtbdd_b_xt_mul(my_mtbdd_t_xt_comp(xc_1_xt_0, 2*xt+1), 2*xt+1);
    // mtbdd_protect(&bracket_left_2);
    // MTBDD bracket_right_2 = my_mtbdd_b_xt_comp_mul(my_mtbdd_t_xt(xc_1_xt_0, 2*xt+1), 2*xt+1);
    // mtbdd_protect(&bracket_right_2);
    // MTBDD inter_res_3 = my_mtbdd_plus(bracket_left_2, bracket_right_2);
    // mtbdd_protect(&inter_res_3);

    // MTBDD xc_1_xt_1 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_mul(t, 2*xc), 2*xt);
    // mtbdd_protect(&xc_1_xt_1);
    // MTBDD bracket_left_3 = my_mtbdd_b_xt_mul(my_mtbdd_t_xt_comp(my_mtbdd_b_xt_mul(my_mtbdd_t_xt_comp(xc_1_xt_1, 2*xt+1), 2*xt+1), 2*xc+1), 2*xc+1);
    // mtbdd_protect(&bracket_left_3);
    // MTBDD bracket_right_3 = my_mtbdd_b_xt_comp_mul(my_mtbdd_t_xt(my_mtbdd_b_xt_comp_mul(my_mtbdd_t_xt(xc_1_xt_1, 2*xt+1), 2*xt+1), 2*xc+1), 2*xc+1);
    // mtbdd_protect(&bracket_right_3);
    // MTBDD inter_res_5 = my_mtbdd_plus(bracket_left_3, bracket_right_3);
    // mtbdd_protect(&inter_res_5);

    // MTBDD bracket_left_4 = my_mtbdd_neg(my_mtbdd_b_xt_mul(my_mtbdd_t_xt_comp(my_mtbdd_b_xt_comp_mul(my_mtbdd_t_xt(xc_1_xt_1, 2*xt+1), 2*xt+1), 2*xc+1), 2*xc+1));
    // mtbdd_protect(&bracket_left_4);
    // MTBDD bracket_right_4 = my_mtbdd_neg(my_mtbdd_b_xt_comp_mul(my_mtbdd_t_xt(my_mtbdd_b_xt_mul(my_mtbdd_t_xt_comp(xc_1_xt_1, 2*xt+1), 2*xt+1), 2*xc+1), 2*xc+1));
    // mtbdd_protect(&bracket_right_4);
    // MTBDD inter_res_6 = my_mtbdd_plus(bracket_left_4, bracket_right_4);
    // mtbdd_protect(&inter_res_6);
    // MTBDD inter_res_7 = my_mtbdd_plus(inter_res_6, inter_res_5);
    // mtbdd_protect(&inter_res_7);
    // MTBDD inter_res_8 = my_mtbdd_plus(inter_res_7, inter_res_3);
    // mtbdd_protect(&inter_res_8);
    // MTBDD res = my_mtbdd_plus(inter_res_8, inter_res_2);
    // mtbdd_protect(&res);

    // *p_t = res;

    // mtbdd_unprotect(&xc_0_xt_0);
    // mtbdd_unprotect(&xc_0_xt_1);
    // mtbdd_unprotect(&bracket_left);
    // mtbdd_unprotect(&bracket_right);
    // mtbdd_unprotect(&inter_res);
    // mtbdd_unprotect(&inter_res_2);
    // mtbdd_unprotect(&xc_1_xt_0);
    // mtbdd_unprotect(&bracket_left_2);
    // mtbdd_unprotect(&bracket_right_2);
    // mtbdd_unprotect(&inter_res_3);
    // mtbdd_unprotect(&xc_1_xt_1);
    // mtbdd_unprotect(&bracket_left_3);
    // mtbdd_unprotect(&bracket_right_3);
    // mtbdd_unprotect(&inter_res_5);
    // mtbdd_unprotect(&bracket_left_4);
    // mtbdd_unprotect(&bracket_right_4);
    // mtbdd_unprotect(&inter_res_6);  
    // mtbdd_unprotect(&inter_res_7);
    // mtbdd_unprotect(&inter_res_8);
    // mtbdd_unprotect(&res);
    // mtbdd_unprotect(&t);

}

void gate_toffoli(MTBDD *p_t, uint32_t xt, uint32_t xc1, uint32_t xc2)
{
    // printf("gate_toffoli\n");
    *p_t = mtbdd_toffoli_apply(*p_t, TASK(_gate_toffoli), xt, xc1, xc2);
    // size_t total_nodes = mtbdd_nodecount(*p_t);
    // printf("Toffoli gate nodes: %zu\n", total_nodes);

    // MTBDD t = *p_t;
    // mtbdd_protect(&t);
    // MTBDD res = my_mtbdd_div_2(t);
    // mtbdd_unprotect(&t);
    // mtbdd_protect(&res);

    // // 分段計算，每段後檢查結果
    // MTBDD xc1_0_xc2_0_zt_0 = my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(t, 2*xc1), 2*xc2), 2*xt+1);
    // mtbdd_protect(&xc1_0_xc2_0_zt_0);
    
    // MTBDD xc1_0_xc2_0_zt_0_div2 = my_mtbdd_div_2(xc1_0_xc2_0_zt_0);
    // mtbdd_protect(&xc1_0_xc2_0_zt_0_div2);

    // MTBDD without_xc1_0_xc2_0_zt_0 = my_mtbdd_minus(t, xc1_0_xc2_0_zt_0);
    // mtbdd_protect(&without_xc1_0_xc2_0_zt_0);

    // mtbdd_unprotect(&res);
    // res = my_mtbdd_plus(res, xc1_0_xc2_0_zt_0_div2);
    // mtbdd_protect(&res);
    // MTBDD xt_inv_1 = my_mtbdd_b_xt_comp_mul(my_mtbdd_t_xt(without_xc1_0_xc2_0_zt_0, 2*xt), 2*xt);
    // mtbdd_protect(&xt_inv_1);
    // MTBDD xt_inv_0 = my_mtbdd_b_xt_mul(my_mtbdd_t_xt_comp(without_xc1_0_xc2_0_zt_0, 2*xt), 2*xt);
    // mtbdd_protect(&xt_inv_0);
    // MTBDD xt_inv = my_mtbdd_plus(xt_inv_0, xt_inv_1);
    // mtbdd_protect(&xt_inv);

    // MTBDD zc1_inv_1 = my_mtbdd_b_xt_comp_mul(my_mtbdd_t_xt(without_xc1_0_xc2_0_zt_0, 2*xc1+1), 2*xc1+1);
    // mtbdd_protect(&zc1_inv_1);
    // MTBDD zc1_inv_0 = my_mtbdd_b_xt_mul(my_mtbdd_t_xt_comp(without_xc1_0_xc2_0_zt_0, 2*xc1+1), 2*xc1+1);
    // mtbdd_protect(&zc1_inv_0);
    // MTBDD zc1_inv = my_mtbdd_plus(zc1_inv_0, zc1_inv_1);
    // mtbdd_protect(&zc1_inv);

    // MTBDD zc2_inv_1 = my_mtbdd_b_xt_comp_mul(my_mtbdd_t_xt(without_xc1_0_xc2_0_zt_0, 2*xc2+1), 2*xc2+1);
    // mtbdd_protect(&zc2_inv_1);
    // MTBDD zc2_inv_0 = my_mtbdd_b_xt_mul(my_mtbdd_t_xt_comp(without_xc1_0_xc2_0_zt_0, 2*xc2+1), 2*xc2+1);
    // mtbdd_protect(&zc2_inv_0);
    // MTBDD zc2_inv = my_mtbdd_plus(zc2_inv_0, zc2_inv_1);
    // mtbdd_protect(&zc2_inv);

    // MTBDD zc1_zc2_inv_1 = my_mtbdd_b_xt_comp_mul(my_mtbdd_t_xt(zc1_inv, 2*xc2+1), 2*xc2+1);
    // mtbdd_protect(&zc1_zc2_inv_1);
    // MTBDD zc1_zc2_inv_0 = my_mtbdd_b_xt_mul(my_mtbdd_t_xt_comp(zc1_inv, 2*xc2+1), 2*xc2+1);
    // mtbdd_protect(&zc1_zc2_inv_0);
    // MTBDD zc1_zc2_inv = my_mtbdd_plus(zc1_zc2_inv_0, zc1_zc2_inv_1);
    // mtbdd_protect(&zc1_zc2_inv);

    // MTBDD zc1_xt_inv_1 = my_mtbdd_b_xt_comp_mul(my_mtbdd_t_xt(xt_inv, 2*xc1+1), 2*xc1+1);
    // mtbdd_protect(&zc1_xt_inv_1);
    // MTBDD zc1_xt_inv_0 = my_mtbdd_b_xt_mul(my_mtbdd_t_xt_comp(xt_inv, 2*xc1+1), 2*xc1+1);
    // mtbdd_protect(&zc1_xt_inv_0);
    // MTBDD zc1_xt_inv = my_mtbdd_plus(zc1_xt_inv_0, zc1_xt_inv_1);
    // mtbdd_protect(&zc1_xt_inv);

    // MTBDD zc2_xt_inv_1 = my_mtbdd_b_xt_comp_mul(my_mtbdd_t_xt(xt_inv, 2*xc2+1), 2*xc2+1);
    // mtbdd_protect(&zc2_xt_inv_1);
    // MTBDD zc2_xt_inv_0 = my_mtbdd_b_xt_mul(my_mtbdd_t_xt_comp(xt_inv, 2*xc2+1), 2*xc2+1);
    // mtbdd_protect(&zc2_xt_inv_0);
    // MTBDD zc2_xt_inv = my_mtbdd_plus(zc2_xt_inv_0, zc2_xt_inv_1);
    // mtbdd_protect(&zc2_xt_inv);

    // MTBDD zc1_zc2_xt_inv_1 = my_mtbdd_b_xt_comp_mul(my_mtbdd_t_xt(zc2_xt_inv, 2*xc1+1), 2*xc1+1);
    // mtbdd_protect(&zc1_zc2_xt_inv_1);
    // MTBDD zc1_zc2_xt_inv_0 = my_mtbdd_b_xt_mul(my_mtbdd_t_xt_comp(zc2_xt_inv, 2*xc1+1), 2*xc1+1);
    // mtbdd_protect(&zc1_zc2_xt_inv_0);
    // MTBDD zc1_zc2_xt_inv = my_mtbdd_plus(zc1_zc2_xt_inv_0, zc1_zc2_xt_inv_1);
    // mtbdd_protect(&zc1_zc2_xt_inv);

    // // first ite(zt, 0, 1/2) f(xt')
    // MTBDD ite_1 = my_mtbdd_div_2(my_mtbdd_b_xt_comp_mul(xt_inv, 2*xt+1));
    // mtbdd_protect(&ite_1);
    // // second ite(xc1, 0, 1/2) f(zc1')
    // MTBDD ite_2 = my_mtbdd_div_2(my_mtbdd_b_xt_comp_mul(zc1_inv, 2*xc1));
    // mtbdd_protect(&ite_2);
    // // third ite(xc2, 0, 1/2) f(zc2')
    // MTBDD ite_3 = my_mtbdd_div_2(my_mtbdd_b_xt_comp_mul(zc2_inv, 2*xc2));
    // mtbdd_protect(&ite_3);

    // // fourth ite(xc1 xor xc2, 0, -1/2) ite(zc1 == zc2 and xc1, -1, 1) f(zc1', zc2')
    // MTBDD ite4_temp1 = my_mtbdd_neg(my_mtbdd_div_2(zc1_zc2_inv));
    // mtbdd_protect(&ite4_temp1);
    // MTBDD ite4_temp1_0_0 = my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(ite4_temp1, 2*xc1), 2*xc2); 
    // mtbdd_protect(&ite4_temp1_0_0);
    // MTBDD ite4_temp1_1_1 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_mul(ite4_temp1, 2*xc1), 2*xc2);
    // mtbdd_protect(&ite4_temp1_1_1);
    // MTBDD ite4_temp1_1_1_equiv_0 = my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(ite4_temp1_1_1, 2*xc1+1), 2*xc2+1);
    // mtbdd_protect(&ite4_temp1_1_1_equiv_0);
    // MTBDD ite4_temp1_1_1_equiv_1 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_mul(ite4_temp1_1_1, 2*xc1+1), 2*xc2+1);
    // mtbdd_protect(&ite4_temp1_1_1_equiv_1);
    // MTBDD ite4_temp1_1_1_equiv = my_mtbdd_plus(ite4_temp1_1_1_equiv_0, ite4_temp1_1_1_equiv_1);
    // mtbdd_protect(&ite4_temp1_1_1_equiv);
    // MTBDD ite_4 = my_mtbdd_minus(my_mtbdd_minus(my_mtbdd_plus(ite4_temp1_0_0, ite4_temp1_1_1), ite4_temp1_1_1_equiv), ite4_temp1_1_1_equiv);
    // mtbdd_protect(&ite_4);

    // // fifth ite(xc2 and xt, 0, -1/2) ite(zc2 == xt and xc2, -1, 1) f(zc2', xt')
    // MTBDD ite5_temp1 = my_mtbdd_neg(my_mtbdd_div_2(zc2_xt_inv));
    // mtbdd_protect(&ite5_temp1);
    // MTBDD ite5_temp1_0_0 = my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(ite5_temp1, 2*xc2), 2*xt+1);
    // mtbdd_protect(&ite5_temp1_0_0);
    // MTBDD ite5_temp1_1_1 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_mul(ite5_temp1, 2*xc2), 2*xt+1);
    // mtbdd_protect(&ite5_temp1_1_1);
    // MTBDD ite5_temp1_1_1_equiv_0 = my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(ite5_temp1_1_1, 2*xc2+1), 2*xt);
    // mtbdd_protect(&ite5_temp1_1_1_equiv_0);
    // MTBDD ite5_temp1_1_1_equiv_1 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_mul(ite5_temp1_1_1, 2*xc2+1), 2*xt);
    // mtbdd_protect(&ite5_temp1_1_1_equiv_1);
    // MTBDD ite5_temp1_1_1_equiv = my_mtbdd_plus(ite5_temp1_1_1_equiv_0, ite5_temp1_1_1_equiv_1);
    // mtbdd_protect(&ite5_temp1_1_1_equiv);
    // MTBDD ite_5 = my_mtbdd_minus(my_mtbdd_minus(my_mtbdd_plus(ite5_temp1_0_0, ite5_temp1_1_1), ite5_temp1_1_1_equiv), ite5_temp1_1_1_equiv);
    // mtbdd_protect(&ite_5);

    // // sixth ite(xc1 and xt, 0, -1/2) ite(zc1 == xt and xc1, -1, 1) f(zc1', xt')
    // MTBDD ite6_temp1 = my_mtbdd_neg(my_mtbdd_div_2(zc1_xt_inv));
    // mtbdd_protect(&ite6_temp1);
    // MTBDD ite6_temp1_0_0 = my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(ite6_temp1, 2*xc1), 2*xt+1);
    // mtbdd_protect(&ite6_temp1_0_0);
    // MTBDD ite6_temp1_1_1 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_mul(ite6_temp1, 2*xc1), 2*xt+1);
    // mtbdd_protect(&ite6_temp1_1_1);
    // MTBDD ite6_temp1_1_1_equiv_0 = my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(ite6_temp1_1_1, 2*xc1+1), 2*xt);
    // mtbdd_protect(&ite6_temp1_1_1_equiv_0);
    // MTBDD ite6_temp1_1_1_equiv_1 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_mul(ite6_temp1_1_1, 2*xc1+1), 2*xt);
    // mtbdd_protect(&ite6_temp1_1_1_equiv_1);
    // MTBDD ite6_temp1_1_1_equiv = my_mtbdd_plus(ite6_temp1_1_1_equiv_0, ite6_temp1_1_1_equiv_1);
    // mtbdd_protect(&ite6_temp1_1_1_equiv);
    // MTBDD ite_6 = my_mtbdd_minus(my_mtbdd_minus(my_mtbdd_plus(ite6_temp1_0_0, ite6_temp1_1_1), ite6_temp1_1_1_equiv), ite6_temp1_1_1_equiv);
    // mtbdd_protect(&ite_6);

    // // seventh ite(xc1 xor xc2 xor xt, 0, 1/2) ite((xc1 == xc2 and zc1 == zc2 and xc1) or (xc1 == xt and zc1 == xt and xc1) or (xc2 == xt and zc2 == xt and xc2), -1, 1) f(zc1', zc2', xt')
    // MTBDD ite7_temp1 = my_mtbdd_div_2(zc1_zc2_xt_inv);
    // mtbdd_protect(&ite7_temp1);
    // MTBDD ite7_temp1_0_0_0 = my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(ite7_temp1, 2*xc1), 2*xc2), 2*xt+1);
    // mtbdd_protect(&ite7_temp1_0_0_0);
    // MTBDD ite7_temp1_0_1_1 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_mul(my_mtbdd_b_xt_comp_mul(ite7_temp1, 2*xc1), 2*xc2), 2*xt+1);
    // mtbdd_protect(&ite7_temp1_0_1_1);
    // MTBDD ite7_temp1_1_0_1 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_mul(my_mtbdd_b_xt_comp_mul(ite7_temp1, 2*xc2), 2*xt+1), 2*xc1);
    // mtbdd_protect(&ite7_temp1_1_0_1);
    // MTBDD ite7_temp1_1_1_0 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_mul(ite7_temp1, 2*xc2), 2*xt+1), 2*xc1);
    // mtbdd_protect(&ite7_temp1_1_1_0);
    // MTBDD ite7_temp1_0_1_1_equiv_0 = my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(ite7_temp1_0_1_1, 2*xc2+1), 2*xt);
    // mtbdd_protect(&ite7_temp1_0_1_1_equiv_0);
    // MTBDD ite7_temp1_0_1_1_equiv_1 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_mul(ite7_temp1_0_1_1, 2*xc2+1), 2*xt);
    // mtbdd_protect(&ite7_temp1_0_1_1_equiv_1);
    // MTBDD ite7_temp1_0_1_1_equiv = my_mtbdd_neg(my_mtbdd_plus(ite7_temp1_0_1_1_equiv_0, ite7_temp1_0_1_1_equiv_1));
    // mtbdd_protect(&ite7_temp1_0_1_1_equiv);
    // MTBDD ite7_temp1_1_0_1_equiv_0 = my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(ite7_temp1_1_0_1, 2*xt), 2*xc1+1);
    // mtbdd_protect(&ite7_temp1_1_0_1_equiv_0);
    // MTBDD ite7_temp1_1_0_1_equiv_1 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_mul(ite7_temp1_1_0_1, 2*xt), 2*xc1+1);
    // mtbdd_protect(&ite7_temp1_1_0_1_equiv_1);
    // MTBDD ite7_temp1_1_0_1_equiv = my_mtbdd_neg(my_mtbdd_plus(ite7_temp1_1_0_1_equiv_0, ite7_temp1_1_0_1_equiv_1));
    // mtbdd_protect(&ite7_temp1_1_0_1_equiv);
    // MTBDD ite7_temp1_1_1_0_equiv_0 = my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(ite7_temp1_1_1_0, 2*xc1+1), 2*xc2+1);
    // mtbdd_protect(&ite7_temp1_1_1_0_equiv_0);
    // MTBDD ite7_temp1_1_1_0_equiv_1 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_mul(ite7_temp1_1_1_0, 2*xc1+1), 2*xc2+1);
    // mtbdd_protect(&ite7_temp1_1_1_0_equiv_1);
    // MTBDD ite7_temp1_1_1_0_equiv = my_mtbdd_neg(my_mtbdd_plus(ite7_temp1_1_1_0_equiv_0, ite7_temp1_1_1_0_equiv_1));
    // mtbdd_protect(&ite7_temp1_1_1_0_equiv);
    // MTBDD ite7_frac1 = my_mtbdd_minus(my_mtbdd_minus(ite7_temp1_0_1_1, ite7_temp1_0_1_1_equiv), ite7_temp1_0_1_1_equiv);
    // mtbdd_protect(&ite7_frac1);
    // MTBDD ite7_frac2 = my_mtbdd_minus(my_mtbdd_minus(ite7_temp1_1_0_1, ite7_temp1_1_0_1_equiv), ite7_temp1_1_0_1_equiv);
    // mtbdd_protect(&ite7_frac2);
    // MTBDD ite7_frac3 = my_mtbdd_minus(my_mtbdd_minus(ite7_temp1_1_1_0, ite7_temp1_1_1_0_equiv), ite7_temp1_1_1_0_equiv);
    // mtbdd_protect(&ite7_frac3);
    // MTBDD ite_7 = my_mtbdd_plus(my_mtbdd_plus(ite7_temp1_0_0_0, ite7_frac1), my_mtbdd_plus(ite7_frac2, ite7_frac3));
    // mtbdd_protect(&ite_7);

    // // final result up to ite_3
    // // MTBDD temp_res = my_mtbdd_plus(ite_1, my_mtbdd_plus(ite_2, ite_3));

    // MTBDD temp_res = my_mtbdd_plus(ite_1, my_mtbdd_plus(ite_2, my_mtbdd_plus(ite_3, my_mtbdd_plus(ite_4, my_mtbdd_plus(ite_5, my_mtbdd_plus(ite_6, ite_7))))));
    // mtbdd_protect(&temp_res);
    
    // // 在新的加法操作前，先解除對舊的 res 的保護
    // mtbdd_unprotect(&res);
    // res = my_mtbdd_plus(res, temp_res);
    // mtbdd_protect(&res);

    // *p_t = res;
    
    // mtbdd_unprotect(&xc1_0_xc2_0_zt_0);
    // mtbdd_unprotect(&without_xc1_0_xc2_0_zt_0);
    // mtbdd_unprotect(&xc1_0_xc2_0_zt_0_div2);
    // mtbdd_unprotect(&xt_inv_1);
    // mtbdd_unprotect(&xt_inv_0);
    // mtbdd_unprotect(&xt_inv);
    // mtbdd_unprotect(&zc1_inv_1);
    // mtbdd_unprotect(&zc1_inv_0);
    // mtbdd_unprotect(&zc1_inv);
    // mtbdd_unprotect(&zc2_inv_1);
    // mtbdd_unprotect(&zc2_inv_0);
    // mtbdd_unprotect(&zc2_inv);
    // mtbdd_unprotect(&zc1_zc2_inv_1);
    // mtbdd_unprotect(&zc1_zc2_inv_0);
    // mtbdd_unprotect(&zc1_zc2_inv);
    // mtbdd_unprotect(&zc1_xt_inv_1);
    // mtbdd_unprotect(&zc1_xt_inv_0);
    // mtbdd_unprotect(&zc1_xt_inv);
    // mtbdd_unprotect(&zc2_xt_inv_1);
    // mtbdd_unprotect(&zc2_xt_inv_0);
    // mtbdd_unprotect(&zc2_xt_inv);
    // mtbdd_unprotect(&zc1_zc2_xt_inv_1);
    // mtbdd_unprotect(&zc1_zc2_xt_inv_0);
    // mtbdd_unprotect(&zc1_zc2_xt_inv);
    // mtbdd_unprotect(&ite_1);
    // mtbdd_unprotect(&ite_2);
    // mtbdd_unprotect(&ite_3);
    // mtbdd_unprotect(&ite4_temp1);
    // mtbdd_unprotect(&ite4_temp1_0_0);
    // mtbdd_unprotect(&ite4_temp1_1_1);
    // mtbdd_unprotect(&ite4_temp1_1_1_equiv_0);
    // mtbdd_unprotect(&ite4_temp1_1_1_equiv_1);
    // mtbdd_unprotect(&ite4_temp1_1_1_equiv);
    // mtbdd_unprotect(&ite_4);
    // mtbdd_unprotect(&ite5_temp1);
    // mtbdd_unprotect(&ite5_temp1_0_0);
    // mtbdd_unprotect(&ite5_temp1_1_1);
    // mtbdd_unprotect(&ite5_temp1_1_1_equiv_0);
    // mtbdd_unprotect(&ite5_temp1_1_1_equiv_1);
    // mtbdd_unprotect(&ite5_temp1_1_1_equiv);
    // mtbdd_unprotect(&ite_5);
    // mtbdd_unprotect(&ite6_temp1);
    // mtbdd_unprotect(&ite6_temp1_0_0);
    // mtbdd_unprotect(&ite6_temp1_1_1);
    // mtbdd_unprotect(&ite6_temp1_1_1_equiv_0);
    // mtbdd_unprotect(&ite6_temp1_1_1_equiv_1);
    // mtbdd_unprotect(&ite6_temp1_1_1_equiv);
    // mtbdd_unprotect(&ite_6);
    // mtbdd_unprotect(&ite7_temp1);
    // mtbdd_unprotect(&ite7_temp1_0_0_0);
    // mtbdd_unprotect(&ite7_temp1_0_1_1);
    // mtbdd_unprotect(&ite7_temp1_1_0_1);
    // mtbdd_unprotect(&ite7_temp1_1_1_0);
    // mtbdd_unprotect(&ite7_temp1_0_1_1_equiv_0);
    // mtbdd_unprotect(&ite7_temp1_0_1_1_equiv_1);
    // mtbdd_unprotect(&ite7_temp1_0_1_1_equiv);
    // mtbdd_unprotect(&ite7_temp1_1_0_1_equiv_0);
    // mtbdd_unprotect(&ite7_temp1_1_0_1_equiv_1);
    // mtbdd_unprotect(&ite7_temp1_1_0_1_equiv);
    // mtbdd_unprotect(&ite7_temp1_1_1_0_equiv_0);
    // mtbdd_unprotect(&ite7_temp1_1_1_0_equiv_1);
    // mtbdd_unprotect(&ite7_temp1_1_1_0_equiv);
    // mtbdd_unprotect(&ite7_frac1);
    // mtbdd_unprotect(&ite7_frac2);
    // mtbdd_unprotect(&ite7_frac3);
    // mtbdd_unprotect(&ite_7);
    // mtbdd_unprotect(&temp_res);
    // mtbdd_unprotect(&res);
}

void gate_ccz(MTBDD *p_t, uint32_t xt, uint32_t xc1, uint32_t xc2)
{
    // printf("gate_ccz\n");
    *p_t = mtbdd_ccz_apply(*p_t, TASK(_gate_ccz), xt, xc1, xc2);
    // size_t total_nodes = mtbdd_nodecount(*p_t);
    // printf("CCZ gate nodes: %zu\n", total_nodes);

    // MTBDD t = *p_t;
    // mtbdd_protect(&t);
    // MTBDD res = my_mtbdd_div_2(t);
    // mtbdd_unprotect(&t);
    // mtbdd_protect(&res);

    // MTBDD xc1_0_xc2_0_xt_0 = my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(t, 2*xc1), 2*xc2), 2*xt);
    // mtbdd_protect(&xc1_0_xc2_0_xt_0);

    // MTBDD xc1_0_xc2_0_xt_0_div2 = my_mtbdd_div_2(xc1_0_xc2_0_xt_0);
    // mtbdd_protect(&xc1_0_xc2_0_xt_0_div2);

    // MTBDD without_xc1_0_xc2_0_xt_0 = my_mtbdd_minus(t, xc1_0_xc2_0_xt_0);
    // mtbdd_protect(&without_xc1_0_xc2_0_xt_0);

    // // MTBDD without_xc1_0_xc2_0_xt_0_div2 = my_mtbdd_div_2(without_xc1_0_xc2_0_xt_0);
    // // mtbdd_protect(&without_xc1_0_xc2_0_xt_0_div2);
    
    // mtbdd_unprotect(&res);
    // res = my_mtbdd_plus(res, xc1_0_xc2_0_xt_0_div2);
    // mtbdd_protect(&res);

    // MTBDD zt_inv_1 = my_mtbdd_b_xt_comp_mul(my_mtbdd_t_xt(without_xc1_0_xc2_0_xt_0, 2*xt+1), 2*xt+1);
    // mtbdd_protect(&zt_inv_1);
    // MTBDD zt_inv_0 = my_mtbdd_b_xt_mul(my_mtbdd_t_xt_comp(without_xc1_0_xc2_0_xt_0, 2*xt+1), 2*xt+1);
    // mtbdd_protect(&zt_inv_0);
    // MTBDD zt_inv = my_mtbdd_plus(zt_inv_0, zt_inv_1);
    // mtbdd_protect(&zt_inv);

    // MTBDD zc1_inv_1 = my_mtbdd_b_xt_comp_mul(my_mtbdd_t_xt(without_xc1_0_xc2_0_xt_0, 2*xc1+1), 2*xc1+1);
    // mtbdd_protect(&zc1_inv_1);
    // MTBDD zc1_inv_0 = my_mtbdd_b_xt_mul(my_mtbdd_t_xt_comp(without_xc1_0_xc2_0_xt_0, 2*xc1+1), 2*xc1+1);
    // mtbdd_protect(&zc1_inv_0);
    // MTBDD zc1_inv = my_mtbdd_plus(zc1_inv_0, zc1_inv_1);
    // mtbdd_protect(&zc1_inv);

    // MTBDD zc2_inv_1 = my_mtbdd_b_xt_comp_mul(my_mtbdd_t_xt(without_xc1_0_xc2_0_xt_0, 2*xc2+1), 2*xc2+1);
    // mtbdd_protect(&zc2_inv_1);
    // MTBDD zc2_inv_0 = my_mtbdd_b_xt_mul(my_mtbdd_t_xt_comp(without_xc1_0_xc2_0_xt_0, 2*xc2+1), 2*xc2+1);
    // mtbdd_protect(&zc2_inv_0);
    // MTBDD zc2_inv = my_mtbdd_plus(zc2_inv_0, zc2_inv_1);
    // mtbdd_protect(&zc2_inv);

    // MTBDD zc1_zc2_inv_1 = my_mtbdd_b_xt_comp_mul(my_mtbdd_t_xt(zc1_inv, 2*xc2+1), 2*xc2+1);
    // mtbdd_protect(&zc1_zc2_inv_1);
    // MTBDD zc1_zc2_inv_0 = my_mtbdd_b_xt_mul(my_mtbdd_t_xt_comp(zc1_inv, 2*xc2+1), 2*xc2+1);
    // mtbdd_protect(&zc1_zc2_inv_0);
    // MTBDD zc1_zc2_inv = my_mtbdd_plus(zc1_zc2_inv_0, zc1_zc2_inv_1);
    // mtbdd_protect(&zc1_zc2_inv);

    // MTBDD zc1_zt_inv_1 = my_mtbdd_b_xt_comp_mul(my_mtbdd_t_xt(zt_inv, 2*xc1+1), 2*xc1+1);
    // mtbdd_protect(&zc1_zt_inv_1);
    // MTBDD zc1_zt_inv_0 = my_mtbdd_b_xt_mul(my_mtbdd_t_xt_comp(zt_inv, 2*xc1+1), 2*xc1+1);
    // mtbdd_protect(&zc1_zt_inv_0);
    // MTBDD zc1_zt_inv = my_mtbdd_plus(zc1_zt_inv_0, zc1_zt_inv_1);
    // mtbdd_protect(&zc1_zt_inv);

    // MTBDD zc2_zt_inv_1 = my_mtbdd_b_xt_comp_mul(my_mtbdd_t_xt(zt_inv, 2*xc2+1), 2*xc2+1);
    // mtbdd_protect(&zc2_zt_inv_1);
    // MTBDD zc2_zt_inv_0 = my_mtbdd_b_xt_mul(my_mtbdd_t_xt_comp(zt_inv, 2*xc2+1), 2*xc2+1);
    // mtbdd_protect(&zc2_zt_inv_0);
    // MTBDD zc2_zt_inv = my_mtbdd_plus(zc2_zt_inv_0, zc2_zt_inv_1);
    // mtbdd_protect(&zc2_zt_inv);

    // MTBDD zc1_zc2_zt_inv_1 = my_mtbdd_b_xt_comp_mul(my_mtbdd_t_xt(zc2_zt_inv, 2*xc1+1), 2*xc1+1);
    // mtbdd_protect(&zc1_zc2_zt_inv_1);
    // MTBDD zc1_zc2_zt_inv_0 = my_mtbdd_b_xt_mul(my_mtbdd_t_xt_comp(zc2_zt_inv, 2*xc1+1), 2*xc1+1);
    // mtbdd_protect(&zc1_zc2_zt_inv_0);
    // MTBDD zc1_zc2_zt_inv = my_mtbdd_plus(zc1_zc2_zt_inv_0, zc1_zc2_zt_inv_1);
    // mtbdd_protect(&zc1_zc2_zt_inv);

    // // first ite(xt, 0, 1/2) f(zt')
    // MTBDD ite_1 = my_mtbdd_div_2(my_mtbdd_b_xt_comp_mul(zt_inv, 2*xt));
    // mtbdd_protect(&ite_1);
    // // second ite(xc1, 0, 1/2) f(zc1')
    // MTBDD ite_2 = my_mtbdd_div_2(my_mtbdd_b_xt_comp_mul(zc1_inv, 2*xc1));
    // mtbdd_protect(&ite_2);
    // // third ite(xc2, 0, 1/2) f(zc2')
    // MTBDD ite_3 = my_mtbdd_div_2(my_mtbdd_b_xt_comp_mul(zc2_inv, 2*xc2));
    // mtbdd_protect(&ite_3);

    // // fourth ite(xc1 xor xc2, 0, -1/2) ite(zc1 == zc2 and xc1, -1, 1) f(zc1', zc2')
    // MTBDD ite4_temp1 = my_mtbdd_neg(my_mtbdd_div_2(zc1_zc2_inv));
    // mtbdd_protect(&ite4_temp1);
    // MTBDD ite4_temp1_0_0 = my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(ite4_temp1, 2*xc1), 2*xc2); 
    // mtbdd_protect(&ite4_temp1_0_0);
    // MTBDD ite4_temp1_1_1 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_mul(ite4_temp1, 2*xc1), 2*xc2);
    // mtbdd_protect(&ite4_temp1_1_1);
    // MTBDD ite4_temp1_1_1_equiv_0 = my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(ite4_temp1_1_1, 2*xc1+1), 2*xc2+1);
    // mtbdd_protect(&ite4_temp1_1_1_equiv_0);
    // MTBDD ite4_temp1_1_1_equiv_1 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_mul(ite4_temp1_1_1, 2*xc1+1), 2*xc2+1);
    // mtbdd_protect(&ite4_temp1_1_1_equiv_1);
    // MTBDD ite4_temp1_1_1_equiv = my_mtbdd_plus(ite4_temp1_1_1_equiv_0, ite4_temp1_1_1_equiv_1);
    // mtbdd_protect(&ite4_temp1_1_1_equiv);
    // MTBDD ite_4 = my_mtbdd_minus(my_mtbdd_minus(my_mtbdd_plus(ite4_temp1_0_0, ite4_temp1_1_1), ite4_temp1_1_1_equiv), ite4_temp1_1_1_equiv);
    // mtbdd_protect(&ite_4);

    // // fifth ite(xc2 and xt, 0, -1/2) ite(zc2 == zt and xc2, -1, 1) f(zc2', zt')
    // MTBDD ite5_temp1 = my_mtbdd_neg(my_mtbdd_div_2(zc2_zt_inv));
    // mtbdd_protect(&ite5_temp1);
    // MTBDD ite5_temp1_0_0 = my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(ite5_temp1, 2*xc2), 2*xt);
    // mtbdd_protect(&ite5_temp1_0_0);
    // MTBDD ite5_temp1_1_1 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_mul(ite5_temp1, 2*xc2), 2*xt);
    // mtbdd_protect(&ite5_temp1_1_1);
    // MTBDD ite5_temp1_1_1_equiv_0 = my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(ite5_temp1_1_1, 2*xc2+1), 2*xt+1);
    // mtbdd_protect(&ite5_temp1_1_1_equiv_0);
    // MTBDD ite5_temp1_1_1_equiv_1 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_mul(ite5_temp1_1_1, 2*xc2+1), 2*xt+1);
    // mtbdd_protect(&ite5_temp1_1_1_equiv_1);
    // MTBDD ite5_temp1_1_1_equiv = my_mtbdd_plus(ite5_temp1_1_1_equiv_0, ite5_temp1_1_1_equiv_1);
    // mtbdd_protect(&ite5_temp1_1_1_equiv);
    // MTBDD ite_5 = my_mtbdd_minus(my_mtbdd_minus(my_mtbdd_plus(ite5_temp1_0_0, ite5_temp1_1_1), ite5_temp1_1_1_equiv), ite5_temp1_1_1_equiv);
    // mtbdd_protect(&ite_5);

    // // sixth ite(xc1 and xt, 0, -1/2) ite(zc1 == zt and xc1, -1, 1) f(zc1', zt')
    // MTBDD ite6_temp1 = my_mtbdd_neg(my_mtbdd_div_2(zc1_zt_inv));
    // mtbdd_protect(&ite6_temp1);
    // MTBDD ite6_temp1_0_0 = my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(ite6_temp1, 2*xc1), 2*xt);
    // mtbdd_protect(&ite6_temp1_0_0);
    // MTBDD ite6_temp1_1_1 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_mul(ite6_temp1, 2*xc1), 2*xt);
    // mtbdd_protect(&ite6_temp1_1_1);
    // MTBDD ite6_temp1_1_1_equiv_0 = my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(ite6_temp1_1_1, 2*xc1+1), 2*xt+1);
    // mtbdd_protect(&ite6_temp1_1_1_equiv_0);
    // MTBDD ite6_temp1_1_1_equiv_1 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_mul(ite6_temp1_1_1, 2*xc1+1), 2*xt+1);
    // mtbdd_protect(&ite6_temp1_1_1_equiv_1);
    // MTBDD ite6_temp1_1_1_equiv = my_mtbdd_plus(ite6_temp1_1_1_equiv_0, ite6_temp1_1_1_equiv_1);
    // mtbdd_protect(&ite6_temp1_1_1_equiv);
    // MTBDD ite_6 = my_mtbdd_minus(my_mtbdd_minus(my_mtbdd_plus(ite6_temp1_0_0, ite6_temp1_1_1), ite6_temp1_1_1_equiv), ite6_temp1_1_1_equiv);
    // mtbdd_protect(&ite_6);

    // // seventh ite(xc1 xor xc2 xor xt, 0, 1/2) ite((xc1 == xc2 and zc1 == zc2 and xc1) or (xc1 == xt and zc1 == zt and xc1) or (xc2 == xt and zc2 == zt and xc2), -1, 1) f(zc1', zc2', zt')
    // MTBDD ite7_temp1 = my_mtbdd_div_2(zc1_zc2_zt_inv);
    // mtbdd_protect(&ite7_temp1);
    // MTBDD ite7_temp1_0_0_0 = my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(ite7_temp1, 2*xc1), 2*xc2), 2*xt);
    // mtbdd_protect(&ite7_temp1_0_0_0);
    // MTBDD ite7_temp1_0_1_1 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_mul(my_mtbdd_b_xt_comp_mul(ite7_temp1, 2*xc1), 2*xc2), 2*xt);
    // mtbdd_protect(&ite7_temp1_0_1_1);
    // MTBDD ite7_temp1_1_0_1 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_mul(my_mtbdd_b_xt_comp_mul(ite7_temp1, 2*xc2), 2*xt), 2*xc1);
    // mtbdd_protect(&ite7_temp1_1_0_1);
    // MTBDD ite7_temp1_1_1_0 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_mul(ite7_temp1, 2*xc2), 2*xt), 2*xc1);
    // mtbdd_protect(&ite7_temp1_1_1_0);
    // MTBDD ite7_temp1_0_1_1_equiv_0 = my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(ite7_temp1_0_1_1, 2*xc2+1), 2*xt+1);
    // mtbdd_protect(&ite7_temp1_0_1_1_equiv_0);
    // MTBDD ite7_temp1_0_1_1_equiv_1 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_mul(ite7_temp1_0_1_1, 2*xc2+1), 2*xt+1);
    // mtbdd_protect(&ite7_temp1_0_1_1_equiv_1);
    // MTBDD ite7_temp1_0_1_1_equiv = my_mtbdd_neg(my_mtbdd_plus(ite7_temp1_0_1_1_equiv_0, ite7_temp1_0_1_1_equiv_1));
    // mtbdd_protect(&ite7_temp1_0_1_1_equiv);
    // MTBDD ite7_temp1_1_0_1_equiv_0 = my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(ite7_temp1_1_0_1, 2*xt+1), 2*xc1+1);
    // mtbdd_protect(&ite7_temp1_1_0_1_equiv_0);
    // MTBDD ite7_temp1_1_0_1_equiv_1 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_mul(ite7_temp1_1_0_1, 2*xt+1), 2*xc1+1);
    // mtbdd_protect(&ite7_temp1_1_0_1_equiv_1);
    // MTBDD ite7_temp1_1_0_1_equiv = my_mtbdd_neg(my_mtbdd_plus(ite7_temp1_1_0_1_equiv_0, ite7_temp1_1_0_1_equiv_1));
    // mtbdd_protect(&ite7_temp1_1_0_1_equiv);
    // MTBDD ite7_temp1_1_1_0_equiv_0 = my_mtbdd_b_xt_comp_mul(my_mtbdd_b_xt_comp_mul(ite7_temp1_1_1_0, 2*xc1+1), 2*xc2+1);
    // mtbdd_protect(&ite7_temp1_1_1_0_equiv_0);
    // MTBDD ite7_temp1_1_1_0_equiv_1 = my_mtbdd_b_xt_mul(my_mtbdd_b_xt_mul(ite7_temp1_1_1_0, 2*xc1+1), 2*xc2+1);
    // mtbdd_protect(&ite7_temp1_1_1_0_equiv_1);
    // MTBDD ite7_temp1_1_1_0_equiv = my_mtbdd_neg(my_mtbdd_plus(ite7_temp1_1_1_0_equiv_0, ite7_temp1_1_1_0_equiv_1));
    // mtbdd_protect(&ite7_temp1_1_1_0_equiv);
    // MTBDD ite7_frac1 = my_mtbdd_minus(my_mtbdd_minus(ite7_temp1_0_1_1, ite7_temp1_0_1_1_equiv), ite7_temp1_0_1_1_equiv);
    // mtbdd_protect(&ite7_frac1);
    // MTBDD ite7_frac2 = my_mtbdd_minus(my_mtbdd_minus(ite7_temp1_1_0_1, ite7_temp1_1_0_1_equiv), ite7_temp1_1_0_1_equiv);
    // mtbdd_protect(&ite7_frac2);
    // MTBDD ite7_frac3 = my_mtbdd_minus(my_mtbdd_minus(ite7_temp1_1_1_0, ite7_temp1_1_1_0_equiv), ite7_temp1_1_1_0_equiv);
    // mtbdd_protect(&ite7_frac3);
    // MTBDD ite_7 = my_mtbdd_plus(my_mtbdd_plus(ite7_temp1_0_0_0, ite7_frac1), my_mtbdd_plus(ite7_frac2, ite7_frac3));
    // mtbdd_protect(&ite_7);

    // // final result up to ite_3
    // // MTBDD temp_res = my_mtbdd_plus(ite_1, my_mtbdd_plus(ite_2, ite_3));

    // MTBDD temp_res = my_mtbdd_plus(ite_1, my_mtbdd_plus(ite_2, my_mtbdd_plus(ite_3, my_mtbdd_plus(ite_4, my_mtbdd_plus(ite_5, my_mtbdd_plus(ite_6, ite_7))))));
    // mtbdd_protect(&temp_res);
    // mtbdd_unprotect(&res);
    // res = my_mtbdd_plus(res, temp_res);
    // mtbdd_protect(&res);

    // *p_t = res;

    // mtbdd_unprotect(&xc1_0_xc2_0_xt_0);
    // mtbdd_unprotect(&without_xc1_0_xc2_0_xt_0);
    // mtbdd_unprotect(&xc1_0_xc2_0_xt_0_div2);
    // mtbdd_unprotect(&zt_inv_1);
    // mtbdd_unprotect(&zt_inv_0);
    // mtbdd_unprotect(&zt_inv);
    // mtbdd_unprotect(&zc1_inv_1);
    // mtbdd_unprotect(&zc1_inv_0);
    // mtbdd_unprotect(&zc1_inv);
    // mtbdd_unprotect(&zc2_inv_1);
    // mtbdd_unprotect(&zc2_inv_0);
    // mtbdd_unprotect(&zc2_inv);
    // mtbdd_unprotect(&zc1_zc2_inv_1);
    // mtbdd_unprotect(&zc1_zc2_inv_0);
    // mtbdd_unprotect(&zc1_zc2_inv);
    // mtbdd_unprotect(&zc1_zt_inv_1);
    // mtbdd_unprotect(&zc1_zt_inv_0);
    // mtbdd_unprotect(&zc1_zt_inv);
    // mtbdd_unprotect(&zc2_zt_inv_1);
    // mtbdd_unprotect(&zc2_zt_inv_0);
    // mtbdd_unprotect(&zc2_zt_inv);
    // mtbdd_unprotect(&zc1_zc2_zt_inv_1);
    // mtbdd_unprotect(&zc1_zc2_zt_inv_0);
    // mtbdd_unprotect(&zc1_zc2_zt_inv);
    // mtbdd_unprotect(&ite_1);
    // mtbdd_unprotect(&ite_2);
    // mtbdd_unprotect(&ite_3);
    // mtbdd_unprotect(&ite4_temp1);
    // mtbdd_unprotect(&ite4_temp1_0_0);
    // mtbdd_unprotect(&ite4_temp1_1_1);
    // mtbdd_unprotect(&ite4_temp1_1_1_equiv_0);
    // mtbdd_unprotect(&ite4_temp1_1_1_equiv_1);
    // mtbdd_unprotect(&ite4_temp1_1_1_equiv);
    // mtbdd_unprotect(&ite_4);
    // mtbdd_unprotect(&ite5_temp1);
    // mtbdd_unprotect(&ite5_temp1_0_0);
    // mtbdd_unprotect(&ite5_temp1_1_1);
    // mtbdd_unprotect(&ite5_temp1_1_1_equiv_0);
    // mtbdd_unprotect(&ite5_temp1_1_1_equiv_1);
    // mtbdd_unprotect(&ite5_temp1_1_1_equiv);
    // mtbdd_unprotect(&ite_5);
    // mtbdd_unprotect(&ite6_temp1);
    // mtbdd_unprotect(&ite6_temp1_0_0);
    // mtbdd_unprotect(&ite6_temp1_1_1);
    // mtbdd_unprotect(&ite6_temp1_1_1_equiv_0);
    // mtbdd_unprotect(&ite6_temp1_1_1_equiv_1);
    // mtbdd_unprotect(&ite6_temp1_1_1_equiv);
    // mtbdd_unprotect(&ite_6);
    // mtbdd_unprotect(&ite7_temp1);
    // mtbdd_unprotect(&ite7_temp1_0_0_0);
    // mtbdd_unprotect(&ite7_temp1_0_1_1);
    // mtbdd_unprotect(&ite7_temp1_1_0_1);
    // mtbdd_unprotect(&ite7_temp1_1_1_0);
    // mtbdd_unprotect(&ite7_temp1_0_1_1_equiv_0);
    // mtbdd_unprotect(&ite7_temp1_0_1_1_equiv_1);
    // mtbdd_unprotect(&ite7_temp1_0_1_1_equiv);
    // mtbdd_unprotect(&ite7_temp1_1_0_1_equiv_0);
    // mtbdd_unprotect(&ite7_temp1_1_0_1_equiv_1);
    // mtbdd_unprotect(&ite7_temp1_1_0_1_equiv);
    // mtbdd_unprotect(&ite7_temp1_1_1_0_equiv_0);
    // mtbdd_unprotect(&ite7_temp1_1_1_0_equiv_1);
    // mtbdd_unprotect(&ite7_temp1_1_1_0_equiv);
    // mtbdd_unprotect(&ite7_frac1);
    // mtbdd_unprotect(&ite7_frac2);
    // mtbdd_unprotect(&ite7_frac3);
    // mtbdd_unprotect(&ite_7);
    // mtbdd_unprotect(&temp_res);
    // mtbdd_unprotect(&res);
}
/* end of "gates.c" */
