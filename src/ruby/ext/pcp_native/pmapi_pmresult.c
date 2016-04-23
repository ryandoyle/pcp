#include <pcp/pmapi.h>
#include <ruby/ruby.h>

#include "pmapi_pmresult.h"
#include "pmapi_pmvalueset.h"

VALUE pcp_pmapi_pmresult_class = Qnil;

typedef struct pmResultWrapper {
    pmResult *pm_result;
} pmResultWrapper;


static void pmresult_free(void *pm_result_wrapper_raw) {
    pmResultWrapper *pm_result_wrapper = (pmResultWrapper *)pm_result_wrapper_raw;

    xfree(pm_result_wrapper->pm_result);
    xfree(pm_result_wrapper);
}

static VALUE pmresult_allocate(VALUE klass) {
    pmResultWrapper *pm_result_wrapper = ALLOC(pmResultWrapper);
    pmResult *pm_result = ALLOC(pmResult);

    pm_result_wrapper->pm_result = pm_result;

    return Data_Wrap_Struct(klass, 0, pmresult_free, pm_result_wrapper);
}

static pmResultWrapper* get_pmresult_wrapper(VALUE self) {
    pmResultWrapper *pm_result_wrapper;

    Data_Get_Struct(self, pmResultWrapper, pm_result_wrapper);

    return pm_result_wrapper;
}

static pmResult* get_pmresult(VALUE self) {
    return get_pmresult_wrapper(self)->pm_result;
}

static void build_vset_from_array(VALUE self, pmResultWrapper *pm_result_wrapper, VALUE vset) {
    int i;
    int number_of_vsets = RARRAY_LENINT(vset);
    int vsets_to_allocate = number_of_vsets - 1;
    pmResult *pm_result = pm_result_wrapper->pm_result;

    /* Check all values are PCP::PMAPI::PmValueSets */
    for(i=0; i < number_of_vsets; i++) {
        if(CLASS_OF(rb_ary_entry(vset, i)) != pcp_pmapi_pmvalueset_class) {
            rb_raise(rb_eArgError, "vlist must only contain instances of PCP::PMAPI::PmValueSet");
        }
    }

    /* Reallocate if we have vets that need to be allocated. We already have room for one */
    if(number_of_vsets > 1) {
        pm_result = (pmResult*)xrealloc(pm_result, sizeof(pmResult) + sizeof(pmValueSet) * vsets_to_allocate);
    }
    /* Re-attach to wrapper */
    pm_result_wrapper->pm_result = pm_result;

    /* Fill in the vset */
    for(i=0; i < number_of_vsets; i++) {
        pm_result->vset[i] = rb_pmapi_pmvalueset_ptr(rb_ary_entry(vset, i));
    }

    /* Store the vsets to keep their underlying pointers in scope */
    rb_iv_set(self, "@vset", vset);
}

static VALUE initialize(VALUE self, VALUE timestamp, VALUE vset) {
    pmResultWrapper *pm_result_wrapper;

    if(TYPE(vset) != T_ARRAY) {
        rb_raise(rb_eArgError, "vset must be an array");
    }

    pm_result_wrapper = get_pmresult_wrapper(self);

    pm_result_wrapper->pm_result->timestamp = rb_time_timeval(timestamp);
    pm_result_wrapper->pm_result->numpmid = RARRAY_LENINT(vset);
    build_vset_from_array(self, pm_result_wrapper, vset);

    return self;
}

static VALUE create_vset(pmResult *pm_result) {
    int i;
    VALUE pm_value_sets = rb_ary_new2(pm_result->numpmid);

    for(i = 0; i < pm_result->numpmid; i++) {
        rb_ary_push(pm_value_sets, rb_pmapi_pmvalueset_new(pm_result->vset[i]));
    }

    return pm_value_sets;
}

static VALUE get_vset(VALUE self) {
    VALUE vset;
    pmResult *pm_result;

    vset = rb_iv_get(self, "@vset");

    if(NIL_P(vset)) {
        pm_result = get_pmresult(self);
        vset = create_vset(pm_result);
        rb_iv_set(self, "@vset", vset);
    }

    return vset;
}

static VALUE get_timestamp(VALUE self) {
    pmResult *pm_result = get_pmresult(self);

    return rb_time_new(pm_result->timestamp.tv_sec, pm_result->timestamp.tv_usec);
}

static VALUE get_numpmid(VALUE self) {
    pmResult *pm_result = get_pmresult(self);

    return INT2NUM(pm_result->numpmid);
}

VALUE rb_pmapi_pmresult_new(pmResult *pm_result) {
    VALUE instance;
    pmResultWrapper *pm_result_wrapper;
    pmResult *pm_result_instance;
    size_t pm_result_memory_size;

    pm_result_memory_size = sizeof(pmResult) + (pm_result->numpmid - 1) * sizeof(pmValueSet*);

    /* Allocate wrapper and pmResult */
    pm_result_wrapper = ALLOC(pmResultWrapper);
    pm_result_instance = (pmResult*)xmalloc(pm_result_memory_size);
    pm_result_wrapper->pm_result = pm_result_instance;

    instance = Data_Wrap_Struct(pcp_pmapi_pmresult_class, 0, pmresult_free, pm_result_wrapper);

    /* Copy over pmResult and pmValueSets */
    memcpy(pm_result_instance, pm_result, pm_result_memory_size);

    /* We need to create the @vset upfront as pmFreeResult() ends up freeing the vsets */
    rb_iv_set(instance, "@vset", create_vset(pm_result_instance));

    return instance;
}

pmResult* rb_pmapi_pmresult_ptr(VALUE pm_result) {
    return get_pmresult(pm_result);
}

void init_rb_pmapi_pmresult(VALUE pmapi_class) {
    pcp_pmapi_pmresult_class = rb_define_class_under(pmapi_class, "PmResult", rb_cObject);

    rb_define_alloc_func(pcp_pmapi_pmresult_class, pmresult_allocate);
    rb_define_method(pcp_pmapi_pmresult_class, "initialize", initialize, 2);
    rb_define_method(pcp_pmapi_pmresult_class, "timestamp", get_timestamp, 0);
    rb_define_method(pcp_pmapi_pmresult_class, "numpmid", get_numpmid, 0);
    rb_define_method(pcp_pmapi_pmresult_class, "vset", get_vset, 0);
}
