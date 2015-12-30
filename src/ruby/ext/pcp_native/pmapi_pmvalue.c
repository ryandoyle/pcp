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

static pmValueWrapper* rb_pmapi_pmvalue_ptr(VALUE self) {
    pmValueWrapper *pm_value;

    Data_Get_Struct(self, pmValueWrapper, pm_value);

    return pm_value;
}

static void rb_pmapi_pmvalue_free(pmValueWrapper *pm_value_wrapper) {
    xfree(pm_value_wrapper);
};

static VALUE rb_pmapi_pmvalue_alloc(VALUE klass) {
    pmValueWrapper *pmvalue_to_wrap = ALLOC(pmValueWrapper);

    return Data_Wrap_Struct(klass, 0 , rb_pmapi_pmvalue_free, pmvalue_to_wrap);
}

static VALUE value(VALUE self) {
    pmValueWrapper *pm_value_wrapper = rb_pmapi_pmvalue_ptr(self);
    int value_format = pm_value_wrapper->value_format;

    VALUE value = rb_iv_get(self, "@value");

    if(NIL_P(value)) {
        switch (value_format) {
            case PM_VAL_INSITU:
                value = INT2NUM(pm_value_wrapper->pm_value.value.lval);
                break;
            case PM_VAL_DPTR:
                value = rb_pmapi_pmvalueblock_new(pm_value_wrapper->pm_value.value.pval);
                break;
            default:
                rb_raise(pcp_pmapi_error, "Value format %d not supported for PmValue", value_format);
        }
        rb_iv_set(self, "@value", value);
    }

    return value;
}

static VALUE rb_pmapi_pmvalue_inst_set(VALUE self, VALUE inst) {
    rb_pmapi_pmvalue_ptr(self)->pm_value.inst = NUM2INT(inst);
    return Qnil;
}

static VALUE rb_pmapi_pmvalue_inst(VALUE self) {
    return INT2NUM(rb_pmapi_pmvalue_ptr(self)->pm_value.inst);
}

VALUE rb_pmapi_pmvalue_new(pmValue pm_value, int value_format) {
    VALUE instance;
    pmValueWrapper *pm_value_from_instance;

    instance = rb_pmapi_pmvalue_alloc(pcp_pmapi_pmvalue_class);
    Data_Get_Struct(instance, pmValueWrapper, pm_value_from_instance);

    /* Copy over the struct into our allocated struct  */
    memcpy(&pm_value_from_instance->pm_value, &pm_value, sizeof(pmValue));
    pm_value_from_instance->value_format = value_format;


    return instance;
}

void init_rb_pmapi_pmvalue(VALUE pmapi_class) {
    pcp_pmapi_pmvalue_class = rb_define_class_under(pmapi_class, "PmValue", rb_cObject);

    rb_define_alloc_func(pcp_pmapi_pmvalue_class, rb_pmapi_pmvalue_alloc);
    rb_define_method(pcp_pmapi_pmvalue_class, "inst=", rb_pmapi_pmvalue_inst_set, 1);
    rb_define_method(pcp_pmapi_pmvalue_class, "inst", rb_pmapi_pmvalue_inst, 0);
    rb_define_method(pcp_pmapi_pmvalue_class, "value", value, 0);
}