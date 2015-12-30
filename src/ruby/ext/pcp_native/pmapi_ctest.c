#include <ruby.h>
#include <pcp/pmapi.h>

#include "pmapi_ctest.h"
#include "pmapi_pmvalueblock.h"
#include "pmapi_pmvalue.h"

VALUE pcp_pmapi_ctest_class = Qnil;

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

void init_rb_pmapi_ctest(VALUE pmapi_class) {
    pcp_pmapi_ctest_class = rb_define_module_under(pmapi_class, "CTest");
    rb_define_method(pcp_pmapi_ctest_class, "pmvalueblock_get_vbuf", pmvalueblock_get_vbuf, 1);
    rb_define_method(pcp_pmapi_ctest_class, "pmvalueblock_get_vlen", pmvalueblock_get_vlen, 1);
    rb_define_method(pcp_pmapi_ctest_class, "pmvalueblock_get_vtype", pmvalueblock_get_vtype, 1);

    rb_define_method(pcp_pmapi_ctest_class, "pmvalue_get_inst", pmvalue_get_inst, 1);
    rb_define_method(pcp_pmapi_ctest_class, "pmvalue_get_insitu_value", pmvalue_get_insitu_value, 1);
    rb_define_method(pcp_pmapi_ctest_class, "pmvalue_get_pmvalueblock_vbuf_as_string", pmvalue_get_pmvalueblock_vbuf_as_string, 1);
}
