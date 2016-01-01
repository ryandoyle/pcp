#include <ruby.h>
#include <pcp/pmapi.h>

#include "pmapi_ctest.h"
#include "pmapi_pmvalueblock.h"
#include "pmapi_pmvalue.h"
#include "pmapi_pmvalueset.h"
#include "pmapi_pmunits.h"

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
    pmUnits pm_units = rb_pmapi_pmunits_get(pm_units_rb);

    return INT2NUM(pm_units.dimSpace);
}

static VALUE pmunits_get_dimTime(VALUE self, VALUE pm_units_rb) {
    pmUnits pm_units = rb_pmapi_pmunits_get(pm_units_rb);

    return INT2NUM(pm_units.dimTime);
}

static VALUE pmunits_get_dimCount(VALUE self, VALUE pm_units_rb) {
    pmUnits pm_units = rb_pmapi_pmunits_get(pm_units_rb);

    return INT2NUM(pm_units.dimCount);
}

static VALUE pmunits_get_scaleSpace(VALUE self, VALUE pm_units_rb) {
    pmUnits pm_units = rb_pmapi_pmunits_get(pm_units_rb);

    return UINT2NUM(pm_units.scaleSpace);
}

static VALUE pmunits_get_scaleTime(VALUE self, VALUE pm_units_rb) {
    pmUnits pm_units = rb_pmapi_pmunits_get(pm_units_rb);

    return UINT2NUM(pm_units.scaleTime);
}

static VALUE pmunits_get_scaleCount(VALUE self, VALUE pm_units_rb) {
    pmUnits pm_units = rb_pmapi_pmunits_get(pm_units_rb);

    return INT2NUM(pm_units.scaleCount);
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
}