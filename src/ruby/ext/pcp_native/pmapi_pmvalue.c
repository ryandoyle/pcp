#include <ruby.h>
#include <pcp/pmapi.h>

#include "pmapi_pmvalue.h"
#include "pmapi.h"
#include "pmapi_pmvalueblock.h"

VALUE pcp_pmapi_pmvalue_class = Qnil;

typedef struct rb_pmapi_pmvalue_wrapper {
    pmValue pm_value;
    int value_format;
} pmValueWrapper;

static pmValueWrapper* get_pmvalue_ptr(VALUE self) {
    pmValueWrapper *pm_value;

    Data_Get_Struct(self, pmValueWrapper, pm_value);

    return pm_value;
}

static void free_pmvalue(pmValueWrapper *pm_value_wrapper) {
    xfree(pm_value_wrapper);
};

static VALUE allocate_pmvalue(VALUE klass) {
    pmValueWrapper *pmvalue_to_wrap = ALLOC(pmValueWrapper);

    return Data_Wrap_Struct(klass, 0 , free_pmvalue, pmvalue_to_wrap);
}

static VALUE initialize(VALUE self, VALUE inst, VALUE value) {
    pmValueWrapper *pm_value_wrapper = get_pmvalue_ptr(self);

    if(CLASS_OF(value) == pcp_pmapi_pmvalueblock_class) {
        pm_value_wrapper->pm_value.value.pval = rb_pmapi_pmvalueblock_ptr(value);
        pm_value_wrapper->value_format = PM_VAL_DPTR;
    } else if(CLASS_OF(value) == rb_cFixnum) {
        pm_value_wrapper->pm_value.value.lval = NUM2INT(value);
        pm_value_wrapper->value_format = PM_VAL_INSITU;
    } else {
        rb_raise(rb_eArgError, "value must be of class PCP::PMAPI::PmValueBlock or Fixnum");
    }

    rb_iv_set(self, "@value", value);

    pm_value_wrapper->pm_value.inst = NUM2INT(inst);

    return self;
}

static VALUE build_value(pmValue pm_value, int value_format) {
    VALUE value = Qnil;

    switch (value_format) {
        case PM_VAL_INSITU:
            value = INT2NUM(pm_value.value.lval);
            break;
        case PM_VAL_DPTR:
            value = rb_pmapi_pmvalueblock_new(pm_value.value.pval);
            break;
        default:
            rb_raise(pcp_pmapi_error, "Value format %d not supported for PmValue", value_format);
    }

    return value;
}

static VALUE get_inst(VALUE self) {
    return INT2NUM(get_pmvalue_ptr(self)->pm_value.inst);
}

VALUE rb_pmapi_pmvalue_new(pmValue pm_value, int value_format) {
    VALUE instance;
    pmValueWrapper *pm_value_from_instance;

    instance = allocate_pmvalue(pcp_pmapi_pmvalue_class);
    Data_Get_Struct(instance, pmValueWrapper, pm_value_from_instance);

    /* Copy over the struct into our allocated struct  */
    memcpy(&pm_value_from_instance->pm_value, &pm_value, sizeof(pmValue));
    pm_value_from_instance->value_format = value_format;

    rb_iv_set(instance, "@value", build_value(pm_value, value_format));

    return instance;
}

pmValue rb_pmapi_pmvalue_ptr(VALUE pm_value) {
    return get_pmvalue_ptr(pm_value)->pm_value;
}

void init_rb_pmapi_pmvalue(VALUE pmapi_class) {
    pcp_pmapi_pmvalue_class = rb_define_class_under(pmapi_class, "PmValue", rb_cObject);

    rb_define_alloc_func(pcp_pmapi_pmvalue_class, allocate_pmvalue);
    rb_define_method(pcp_pmapi_pmvalue_class, "initialize", initialize, 2);
    rb_define_method(pcp_pmapi_pmvalue_class, "inst", get_inst, 0);
    rb_define_attr(pcp_pmapi_pmvalue_class, "value", 1, 0);
}