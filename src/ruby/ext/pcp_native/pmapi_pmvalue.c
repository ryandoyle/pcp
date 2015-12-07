#include <ruby.h>
#include <pcp/pmapi.h>

#include "pmapi_pmvalue.h"
#include "pmapi.h"

VALUE pcp_pmapi_pmvalue_class = Qnil;

struct rb_pmapi_pmvalue_wrapper {
    pmValue pm_value;
    int value_format;
    int metric_type;
};

static struct rb_pmapi_pmvalue_wrapper* rb_pmapi_pmvalue_ptr(VALUE self) {
    struct rb_pmapi_pmvalue_wrapper *pm_value;

    Data_Get_Struct(self, struct rb_pmapi_pmvalue_wrapper, pm_value);

    return pm_value;
}

static void rb_pmapi_pmvalue_free(struct rb_pmapi_pmvalue_wrapper *pm_value_wrapper) {
    xfree(pm_value_wrapper);
};

static VALUE rb_pmapi_pmvalue_alloc(VALUE klass) {
    struct rb_pmapi_pmvalue_wrapper *pmvalue_to_wrap = ALLOC(struct rb_pmapi_pmvalue_wrapper);

    return Data_Wrap_Struct(klass, 0 , rb_pmapi_pmvalue_free, pmvalue_to_wrap);
}

static VALUE rb_pmapi_pmvalue_build(struct rb_pmapi_pmvalue_wrapper *pm_value_wrapper) {
    pmAtomValue atom_value;
    int error;
    VALUE result = Qnil;

    atom_value.cp = NULL;

    if((error = pmExtractValue(pm_value_wrapper->value_format, &pm_value_wrapper->pm_value, pm_value_wrapper->metric_type, &atom_value, pm_value_wrapper->metric_type))) {
        rb_pmapi_raise_error_from_pm_error_code(error);
        return Qnil;
    }

    switch(pm_value_wrapper->metric_type) {
        case PM_TYPE_32:
            result = LONG2NUM(atom_value.l);
            break;
        case PM_TYPE_U32:
            result = ULONG2NUM(atom_value.ul);
            break;
        case PM_TYPE_64:
            result = LL2NUM(atom_value.ll);
            break;
        case PM_TYPE_U64:
            result = ULL2NUM(atom_value.ull);
            break;
        case PM_TYPE_FLOAT:
            result = DBL2NUM((double)atom_value.f);
            break;
        case PM_TYPE_DOUBLE:
            result = DBL2NUM(atom_value.d);
            break;
        case PM_TYPE_STRING:
            result = rb_tainted_str_new_cstr(atom_value.cp);
            free(atom_value.cp);
            break;
        case PM_TYPE_AGGREGATE:
            /* No support for aggregate but pmExtractValue() will still malloc */
            free(atom_value.vbp);
        case PM_TYPE_AGGREGATE_STATIC:
        case PM_TYPE_EVENT:
        case PM_TYPE_HIGHRES_EVENT:
        case PM_TYPE_UNKNOWN:
        case PM_TYPE_NOSUPPORT:
        default:
            rb_raise(pcp_pmapi_error, "Metric data type %d not supported", pm_value_wrapper->metric_type);
    }

    return result;
}

static VALUE rb_pmapi_pmvalue_inst_set(VALUE self, VALUE inst) {
    rb_pmapi_pmvalue_ptr(self)->pm_value.inst = NUM2INT(inst);
    return Qnil;
}

static VALUE rb_pmapi_pmvalue_inst(VALUE self) {
    return INT2NUM(rb_pmapi_pmvalue_ptr(self)->pm_value.inst);
}

VALUE rb_pmapi_pmvalue_new(pmValue pm_value, int value_format, int metric_type) {
    VALUE instance;
    struct rb_pmapi_pmvalue_wrapper *pm_value_from_instance;

    instance = rb_pmapi_pmvalue_alloc(pcp_pmapi_pmvalue_class);
    Data_Get_Struct(instance, struct rb_pmapi_pmvalue_wrapper, pm_value_from_instance);

    /* Copy over the struct into our allocated struct  */
    memcpy(&pm_value_from_instance->pm_value, &pm_value, sizeof(pmValue));
    pm_value_from_instance->metric_type = metric_type;
    pm_value_from_instance->value_format = value_format;

    rb_iv_set(instance, "@value", rb_pmapi_pmvalue_build(pm_value_from_instance));

    return instance;
}

void init_rb_pmapi_pmvalue(VALUE pmapi_class) {
    pcp_pmapi_pmvalue_class = rb_define_class_under(pmapi_class, "PmValue", rb_cObject);

    rb_define_alloc_func(pcp_pmapi_pmvalue_class, rb_pmapi_pmvalue_alloc);
    rb_define_method(pcp_pmapi_pmvalue_class, "inst=", rb_pmapi_pmvalue_inst_set, 1);
    rb_define_method(pcp_pmapi_pmvalue_class, "inst", rb_pmapi_pmvalue_inst, 0);
}