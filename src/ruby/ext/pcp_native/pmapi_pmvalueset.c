#include <ruby.h>
#include <pcp/pmapi.h>

#include "pmapi_pmvalueset.h"
#include "pmapi_pmvalue.h"
#include "pmapi.h"

VALUE pcp_pmapi_pmvalueset_class = Qnil;

static void rb_pmapi_pmvalueset_free(pmValueSet *pm_value_set) {
    xfree(pm_value_set);
}

static VALUE rb_pmapi_pmvalueset_alloc(VALUE klass) {
    pmValueSet *pm_value_set_to_wrap = ALLOC(pmValueSet);

    return Data_Wrap_Struct(klass, 0, rb_pmapi_pmvalueset_free, pm_value_set_to_wrap);
}

static pmValueSet *rb_pmapi_pmvalueset_ptr(VALUE self) {
    pmValueSet *pm_value_set;

    Data_Get_Struct(self, pmValueSet, pm_value_set);

    return pm_value_set;
}

static VALUE rb_pmapi_pmvalueset_pmid_set(VALUE self, VALUE pmid) {
    rb_pmapi_pmvalueset_ptr(self)->pmid = NUM2UINT(pmid);
    return Qnil;
}

static VALUE rb_pmapi_pmvalueset_pmid(VALUE self) {
    return UINT2NUM(rb_pmapi_pmvalueset_ptr(self)->pmid);
}

static VALUE rb_pmapi_pmvalueset_valfmt_set(VALUE self, VALUE valfmt) {
    rb_pmapi_pmvalueset_ptr(self)->valfmt = NUM2INT(valfmt);
    return Qnil;
}

static VALUE rb_pmapi_pmvalueset_valfmt(VALUE self) {
    return INT2NUM(rb_pmapi_pmvalueset_ptr(self)->valfmt);
}


static VALUE rb_pmapi_pmvalueset_get_pmvalues(pmValueSet *pm_value_set) {
    int i, error;
    VALUE pm_values;
    pmDesc metric_description;

    /* Create our array of pmValue(s) */
    pm_values = rb_ary_new2(pm_value_set->numval);

    /* We've got to get the metric description so we know how to decode the result */
    if((error = pmLookupDesc(pm_value_set->pmid, &metric_description)) < 0 ) {
        rb_pmapi_raise_error_from_pm_error_code(error);
        return Qnil;
    }

    for(i = 0; i < pm_value_set->numval; i++) {
        rb_ary_push(pm_values, rb_pmapi_pmvalue_new(pm_value_set->vlist[i], pm_value_set->valfmt, metric_description.type));
    }

    return pm_values;
}

VALUE rb_pmapi_pmvalueset_new(pmValueSet *pm_value_set) {
    VALUE instance;
    pmValueSet *pm_value_set_for_instance;

    instance = rb_pmapi_pmvalueset_alloc(pcp_pmapi_pmvalueset_class);
    Data_Get_Struct(instance, pmValueSet, pm_value_set_for_instance);

    /* Use the original pmValueSet as we are not copying all the value sets when wrapping the pmValueSet struct */
    rb_iv_set(instance, "@vlist", rb_pmapi_pmvalueset_get_pmvalues(pm_value_set));

    /* Only copy over the start of the pmValueSet. Don't worry about all the pmValues
     * as they are handled by rb_pmapi_pmvalueset_get_pmvalues() */
    memcpy(pm_value_set_for_instance, pm_value_set, sizeof(pmValueSet));

    return instance;
}

void init_rb_pmapi_pmvalueset(VALUE pmapi_class) {
    pcp_pmapi_pmvalueset_class = rb_define_class_under(pmapi_class, "PmValueSet", rb_cObject);

    rb_define_alloc_func(pcp_pmapi_pmvalueset_class, rb_pmapi_pmvalueset_alloc);
    rb_define_method(pcp_pmapi_pmvalueset_class, "pmid=", rb_pmapi_pmvalueset_pmid_set, 1);
    rb_define_method(pcp_pmapi_pmvalueset_class, "pmid", rb_pmapi_pmvalueset_pmid, 0);
    rb_define_method(pcp_pmapi_pmvalueset_class, "valfmt=", rb_pmapi_pmvalueset_valfmt_set, 1);
    rb_define_method(pcp_pmapi_pmvalueset_class, "valfmt", rb_pmapi_pmvalueset_valfmt, 0);
}
