#include <ruby.h>
#include <pcp/pmapi.h>

#include "pmapi_pmvalueset.h"
#include "pmapi_pmvalue.h"

VALUE pcp_pmapi_pmvalueset_class = Qnil;

typedef struct pmValueSetWrapper {
    pmValueSet *pm_value_set;
} pmValueSetWrapper;

static void pmvalueset_free(void *pm_value_set) {
    pmValueSetWrapper *pm_value_set_wrapper = (pmValueSetWrapper*)pm_value_set;

    xfree(pm_value_set_wrapper->pm_value_set);
    xfree(pm_value_set_wrapper);
}

static VALUE pmvalueset_allocate(VALUE klass) {
    pmValueSetWrapper *pm_value_set_wrapper = ALLOC(pmValueSetWrapper);
    pmValueSet *pm_value_set = ALLOC(pmValueSet);
    pm_value_set_wrapper->pm_value_set = pm_value_set;

    return Data_Wrap_Struct(klass, 0, pmvalueset_free, pm_value_set_wrapper);
}

static pmValueSet *pmvalueset_ptr(VALUE self) {
    pmValueSetWrapper *pm_value_set_wrapper;

    Data_Get_Struct(self, pmValueSetWrapper, pm_value_set_wrapper);

    return pm_value_set_wrapper->pm_value_set;
}

static void build_vlist_from_array(VALUE self, pmValueSetWrapper *pm_value_set_wrapper, VALUE vlist) {
    int i;
    int number_of_pm_values = RARRAY_LENINT(vlist);
    int pmvalues_to_allocate = number_of_pm_values - 1;
    pmValueSet *pm_value_set = pm_value_set_wrapper->pm_value_set;

    /* Make sure vlist is an array */
    if(TYPE(vlist) != T_ARRAY) {
        rb_raise(rb_eArgError, "vlist must be an array");
    }

    /* Check all values are PCP::PMAPI::PmValues */
    for(i=0; i < number_of_pm_values; i++) {
        if(CLASS_OF(rb_ary_entry(vlist, i)) != pcp_pmapi_pmvalue_class) {
            rb_raise(rb_eArgError, "vlist must only contain instances of PCP::PMAPI::PmValue");
        }
    }

    /* Re-allocate required space. We already have room for one pmValue */
    if(number_of_pm_values > 1) {
        pm_value_set = (pmValueSet*)xrealloc(pm_value_set, sizeof(pmValueSet) + sizeof(pmValue) * pmvalues_to_allocate);
    }
    /* re-attach pointer in case it changed in the xrealloc */
    pm_value_set_wrapper->pm_value_set = pm_value_set;

    /* Fill in the vlist */
    for(i=0; i < number_of_pm_values; i++) {
        pm_value_set->vlist[i] = rb_pmapi_pmvalue_ptr(rb_ary_entry(vlist, i));
    }

    /* Store the vlist to keep the pmValues in scope */
    rb_iv_set(self, "@vlist", vlist);

}

static VALUE initialize(VALUE self, VALUE pmid, VALUE valfmt, VALUE vlist) {
    pmValueSetWrapper *pm_value_set_wrapper;

    Data_Get_Struct(self, pmValueSetWrapper, pm_value_set_wrapper);

    pm_value_set_wrapper->pm_value_set->numval = RARRAY_LENINT(vlist);
    pm_value_set_wrapper->pm_value_set->pmid = NUM2UINT(pmid);
    pm_value_set_wrapper->pm_value_set->valfmt = NUM2INT(valfmt);

    build_vlist_from_array(self, pm_value_set_wrapper, vlist);

    return self;
}

static VALUE get_pmid(VALUE self) {
    return UINT2NUM(pmvalueset_ptr(self)->pmid);
}

static VALUE get_valfmt(VALUE self) {
    return INT2NUM(pmvalueset_ptr(self)->valfmt);
}

static VALUE get_numval(VALUE self) {
    return INT2NUM(pmvalueset_ptr(self)->numval);
}

static VALUE get_vlist(VALUE self) {
    int i;
    VALUE vlist;
    pmValueSet *pm_value_set = pmvalueset_ptr(self);

    vlist = rb_iv_get(self, "@vlist");

    if(NIL_P(vlist)) {
        /* Create our array of pmValue(s) */
        vlist = rb_ary_new2(pm_value_set->numval);

        for (i = 0; i < pm_value_set->numval; i++) {
            rb_ary_push(vlist, rb_pmapi_pmvalue_new(pm_value_set->vlist[i], pm_value_set->valfmt));
        }
        rb_iv_set(self, "@vlist", vlist);
    }

    return vlist;
}

pmValueSet *rb_pmapi_pmvalueset_ptr(VALUE pm_value_set) {
    return pmvalueset_ptr(pm_value_set);
}

VALUE rb_pmapi_pmvalueset_new(pmValueSet *pm_value_set) {
    VALUE instance;
    pmValueSetWrapper *pm_value_set_wrapper;
    pmValueSet *pm_value_set_instance;
    size_t pm_value_set_size;

    /* First pmValue is already allocated */
    pm_value_set_size = sizeof(pmValueSet) + (pm_value_set->numval - 1) * sizeof(pmValue);

    /* Allocate wrapper and pmValueSet */
    pm_value_set_wrapper = ALLOC(pmValueSetWrapper);
    pm_value_set_instance = (pmValueSet*)xmalloc(pm_value_set_size);
    pm_value_set_wrapper->pm_value_set = pm_value_set_instance;

    instance = Data_Wrap_Struct(pcp_pmapi_pmvalueset_class, 0, pmvalueset_free, pm_value_set_wrapper);

    /* Only copy over pmValueSet and all pmValues */
    memcpy(pm_value_set_instance, pm_value_set, pm_value_set_size);

    return instance;
}

void init_rb_pmapi_pmvalueset(VALUE pmapi_class) {
    pcp_pmapi_pmvalueset_class = rb_define_class_under(pmapi_class, "PmValueSet", rb_cObject);

    rb_define_alloc_func(pcp_pmapi_pmvalueset_class, pmvalueset_allocate);
    rb_define_method(pcp_pmapi_pmvalueset_class, "initialize", initialize, 3);
    rb_define_method(pcp_pmapi_pmvalueset_class, "pmid", get_pmid, 0);
    rb_define_method(pcp_pmapi_pmvalueset_class, "valfmt", get_valfmt, 0);
    rb_define_method(pcp_pmapi_pmvalueset_class, "vlist", get_vlist, 0);
    rb_define_method(pcp_pmapi_pmvalueset_class, "numval", get_numval, 0);
}
