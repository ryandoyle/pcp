#include <ruby.h>
#include <pcp/pmapi.h>

#include "pmapi_ctest.h"
#include "pmapi_pmvalueblock.h"
#include "pmapi_pmvalue.h"
#include "pmapi_pmvalueset.h"
#include "pmapi_pmunits.h"
#include "pmapi_pmdesc.h"
#include "pmapi_pmresult.h"

VALUE pcp_pmapi_ctest_module = Qnil;

/* Various static methods to test going from Ruby to C */

static VALUE pmvalueblock_get_vbuf(VALUE self, VALUE pm_value_block_rb) {
    pmValueBlock *pm_value_block = rb_pmapi_pmvalueblock_ptr(pm_value_block_rb);

    return rb_tainted_str_new_cstr(pm_value_block->vbuf);
}

static VALUE pmvalueblock_get_vlen(VALUE self, VALUE pm_value_block_rb) {
    pmValueBlock *pm_value_block = rb_pmapi_pmvalueblock_ptr(pm_value_block_rb);

    return UINT2NUM(pm_value_block->vlen);
}

static VALUE pmvalueblock_get_vtype(VALUE self, VALUE pm_value_block_rb) {
    pmValueBlock *pm_value_block = rb_pmapi_pmvalueblock_ptr(pm_value_block_rb);

    return UINT2NUM(pm_value_block->vtype);
}

static VALUE pmvalue_get_inst(VALUE self, VALUE pm_value_rb) {
    pmValue pm_value = rb_pmapi_pmvalue_ptr(pm_value_rb);

    return INT2NUM(pm_value.inst);
}

static VALUE pmvalue_get_insitu_value(VALUE self, VALUE pm_value_rb) {
    pmValue pm_value = rb_pmapi_pmvalue_ptr(pm_value_rb);

    return INT2NUM(pm_value.value.lval);
}

static VALUE pmvalue_get_pmvalueblock_vbuf_as_string(VALUE self, VALUE pm_value_rb) {
    pmValue pm_value = rb_pmapi_pmvalue_ptr(pm_value_rb);
    pmValueBlock *pm_value_block = pm_value.value.pval;

    return rb_tainted_str_new_cstr(pm_value_block->vbuf);
}

static VALUE pmvalueset_get_pmid(VALUE self, VALUE pm_value_set_rb) {
    pmValueSet *pm_value_set = rb_pmapi_pmvalueset_ptr(pm_value_set_rb);

    return UINT2NUM(pm_value_set->pmid);
}

static VALUE pmvalueset_get_valfmt(VALUE self, VALUE pm_value_set_rb) {
    pmValueSet *pm_value_set = rb_pmapi_pmvalueset_ptr(pm_value_set_rb);

    return INT2NUM(pm_value_set->valfmt);
}

static VALUE pmvalueset_get_numval(VALUE self, VALUE pm_value_set_rb) {
    pmValueSet *pm_value_set = rb_pmapi_pmvalueset_ptr(pm_value_set_rb);

    return INT2NUM(pm_value_set->numval);
}

static VALUE pmvalueset_get_pmvalue_for_insitu(VALUE self, VALUE pm_value_set_rb, VALUE pmvalue_index_rb) {
    pmValueSet *pm_value_set = rb_pmapi_pmvalueset_ptr(pm_value_set_rb);
    int pmvalue_index = NUM2INT(pmvalue_index_rb);

    return INT2NUM(pm_value_set->vlist[pmvalue_index].value.lval);
}

static VALUE pmunits_get_dimSpace(VALUE self, VALUE pm_units_rb) {
    pmUnits *pm_units = rb_pmapi_pmunits_ptr(pm_units_rb);

    return INT2NUM(pm_units->dimSpace);
}

static VALUE pmunits_get_dimTime(VALUE self, VALUE pm_units_rb) {
    pmUnits *pm_units = rb_pmapi_pmunits_ptr(pm_units_rb);

    return INT2NUM(pm_units->dimTime);
}

static VALUE pmunits_get_dimCount(VALUE self, VALUE pm_units_rb) {
    pmUnits *pm_units = rb_pmapi_pmunits_ptr(pm_units_rb);

    return INT2NUM(pm_units->dimCount);
}

static VALUE pmunits_get_scaleSpace(VALUE self, VALUE pm_units_rb) {
    pmUnits *pm_units = rb_pmapi_pmunits_ptr(pm_units_rb);

    return UINT2NUM(pm_units->scaleSpace);
}

static VALUE pmunits_get_scaleTime(VALUE self, VALUE pm_units_rb) {
    pmUnits *pm_units = rb_pmapi_pmunits_ptr(pm_units_rb);

    return UINT2NUM(pm_units->scaleTime);
}

static VALUE pmunits_get_scaleCount(VALUE self, VALUE pm_units_rb) {
    pmUnits *pm_units = rb_pmapi_pmunits_ptr(pm_units_rb);

    return INT2NUM(pm_units->scaleCount);
}

static VALUE pmdesc_get_pmid(VALUE self, VALUE pm_desc_rb) {
    pmDesc *pm_desc = rb_pmapi_pmdesc_ptr(pm_desc_rb);

    return UINT2NUM(pm_desc->pmid);
}

static VALUE pmdesc_get_type(VALUE self, VALUE pm_desc_rb) {
    pmDesc *pm_desc = rb_pmapi_pmdesc_ptr(pm_desc_rb);

    return INT2NUM(pm_desc->type);
}

static VALUE pmdesc_get_indom(VALUE self, VALUE pm_desc_rb) {
    pmDesc *pm_desc = rb_pmapi_pmdesc_ptr(pm_desc_rb);

    return UINT2NUM(pm_desc->indom);
}

static VALUE pmdesc_get_sem(VALUE self, VALUE pm_desc_rb) {
    pmDesc *pm_desc = rb_pmapi_pmdesc_ptr(pm_desc_rb);

    return INT2NUM(pm_desc->sem);
}

static VALUE pmdesc_get_units_dimSpace(VALUE self, VALUE pm_desc_rb) {
    pmDesc *pm_desc = rb_pmapi_pmdesc_ptr(pm_desc_rb);

    return INT2NUM(pm_desc->units.dimSpace);
}

static VALUE pmresult_get_timestamp(VALUE self, VALUE pm_result_rb) {
    pmResult *pm_result = rb_pmapi_pmresult_ptr(pm_result_rb);

    return rb_time_new(pm_result->timestamp.tv_sec, pm_result->timestamp.tv_usec);
}

static VALUE pmresult_get_numpmid(VALUE self, VALUE pm_result_rb) {
    pmResult *pm_result = rb_pmapi_pmresult_ptr(pm_result_rb);

    return INT2NUM(pm_result->numpmid);
}

static VALUE pmresult_get_pmid_for_nth_vset(VALUE self, VALUE pm_result_rb, VALUE vset_index_rb) {
    int vset_index = NUM2INT(vset_index_rb);
    pmResult *pm_result = rb_pmapi_pmresult_ptr(pm_result_rb);

    return UINT2NUM(pm_result->vset[vset_index]->pmid);
}

void init_rb_pmapi_ctest(VALUE pmapi_class) {
    pcp_pmapi_ctest_module = rb_define_module_under(pmapi_class, "CTest");
    rb_define_method(pcp_pmapi_ctest_module, "pmvalueblock_get_vbuf", pmvalueblock_get_vbuf, 1);
    rb_define_method(pcp_pmapi_ctest_module, "pmvalueblock_get_vlen", pmvalueblock_get_vlen, 1);
    rb_define_method(pcp_pmapi_ctest_module, "pmvalueblock_get_vtype", pmvalueblock_get_vtype, 1);

    rb_define_method(pcp_pmapi_ctest_module, "pmvalue_get_inst", pmvalue_get_inst, 1);
    rb_define_method(pcp_pmapi_ctest_module, "pmvalue_get_insitu_value", pmvalue_get_insitu_value, 1);
    rb_define_method(pcp_pmapi_ctest_module, "pmvalue_get_pmvalueblock_vbuf_as_string", pmvalue_get_pmvalueblock_vbuf_as_string, 1);

    rb_define_method(pcp_pmapi_ctest_module, "pmvalueset_get_pmid", pmvalueset_get_pmid, 1);
    rb_define_method(pcp_pmapi_ctest_module, "pmvalueset_get_valfmt", pmvalueset_get_valfmt, 1);
    rb_define_method(pcp_pmapi_ctest_module, "pmvalueset_get_numval", pmvalueset_get_numval, 1);
    rb_define_method(pcp_pmapi_ctest_module, "pmvalueset_get_pmvalue_for_insitu", pmvalueset_get_pmvalue_for_insitu, 2);

    rb_define_method(pcp_pmapi_ctest_module, "pmunits_get_dimSpace", pmunits_get_dimSpace, 1);
    rb_define_method(pcp_pmapi_ctest_module, "pmunits_get_dimTime", pmunits_get_dimTime, 1);
    rb_define_method(pcp_pmapi_ctest_module, "pmunits_get_dimCount", pmunits_get_dimCount, 1);
    rb_define_method(pcp_pmapi_ctest_module, "pmunits_get_scaleSpace", pmunits_get_scaleSpace, 1);
    rb_define_method(pcp_pmapi_ctest_module, "pmunits_get_scaleTime", pmunits_get_scaleTime, 1);
    rb_define_method(pcp_pmapi_ctest_module, "pmunits_get_scaleCount", pmunits_get_scaleCount, 1);

    rb_define_method(pcp_pmapi_ctest_module, "pmdesc_get_pmid", pmdesc_get_pmid, 1);
    rb_define_method(pcp_pmapi_ctest_module, "pmdesc_get_type", pmdesc_get_type, 1);
    rb_define_method(pcp_pmapi_ctest_module, "pmdesc_get_indom", pmdesc_get_indom, 1);
    rb_define_method(pcp_pmapi_ctest_module, "pmdesc_get_sem", pmdesc_get_sem, 1);
    rb_define_method(pcp_pmapi_ctest_module, "pmdesc_get_units_dimSpace", pmdesc_get_units_dimSpace, 1);

    rb_define_method(pcp_pmapi_ctest_module, "pmresult_get_timestamp", pmresult_get_timestamp, 1);
    rb_define_method(pcp_pmapi_ctest_module, "pmresult_get_numpmid", pmresult_get_numpmid, 1);
    rb_define_method(pcp_pmapi_ctest_module, "pmresult_get_pmid_for_nth_vset", pmresult_get_pmid_for_nth_vset, 2);

}