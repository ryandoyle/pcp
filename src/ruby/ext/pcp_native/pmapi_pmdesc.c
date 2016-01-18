#include <ruby.h>
#include <pcp/pmapi.h>

#include "pmapi_pmdesc.h"
#include "pmapi_pmunits.h"

VALUE pcp_pmapi_pmdesc_class;



static pmDesc*pmdesc_ptr(VALUE self) {
    pmDesc *pm_desc;

    Data_Get_Struct(self, pmDesc, pm_desc);

    return pm_desc;
}

static void rb_pmapi_pmdesc_free(pmDesc *pm_desc) {
    xfree(pm_desc);
}

static VALUE rb_pmapi_pmdesc_alloc(VALUE klass) {
    pmDesc *pm_desc_to_wrap = ALLOC(pmDesc);

    return Data_Wrap_Struct(klass, 0, rb_pmapi_pmdesc_free, pm_desc_to_wrap);
}

static VALUE initialize(VALUE self, VALUE pmid, VALUE type, VALUE indom, VALUE sem, VALUE units) {
    pmDesc *pm_desc = pmdesc_ptr(self);
    pmUnits *pm_units = rb_pmapi_pmunits_ptr(units);

    pm_desc->indom = NUM2UINT(indom);
    pm_desc->pmid = NUM2UINT(pmid);
    pm_desc->sem = NUM2INT(sem);
    pm_desc->type = NUM2INT(type);
    memcpy(&pm_desc->units, pm_units, sizeof(pmUnits));

    return self;
}

static VALUE get_pmid(VALUE self) {
    return UINT2NUM(pmdesc_ptr(self)->pmid);
}

static VALUE get_type(VALUE self) {
    return INT2NUM(pmdesc_ptr(self)->type);
}

static VALUE get_indom(VALUE self) {
    return UINT2NUM(pmdesc_ptr(self)->indom);
}


static VALUE get_sem(VALUE self) {
    return INT2NUM(pmdesc_ptr(self)->sem);
}

static VALUE get_units(VALUE self) {
    VALUE pmunits = rb_iv_get(self, "@units");

    /* Cache the units object of we haven't created one already */
    if(NIL_P(pmunits)) {
        pmunits = rb_pmapi_pmunits_new_from_pmdesc(&pmdesc_ptr(self)->units, self);
        rb_iv_set(self, "@units", pmunits);
    }

    return pmunits;
}

pmDesc* rb_pmapi_pmdesc_ptr(VALUE pm_desc) {
    return pmdesc_ptr(pm_desc);
}

VALUE rb_pmapi_pmdesc_new(pmDesc pm_desc) {
    VALUE instance;
    pmDesc *pm_desc_from_instance;

    instance = rb_pmapi_pmdesc_alloc(pcp_pmapi_pmdesc_class);
    Data_Get_Struct(instance, pmDesc, pm_desc_from_instance);
    memcpy(pm_desc_from_instance, &pm_desc, sizeof(pmDesc));

    return instance;
}

void init_rb_pmapi_pmdesc(VALUE pmapi_class) {

    pcp_pmapi_pmdesc_class = rb_define_class_under(pmapi_class, "PmDesc", rb_cObject);
    rb_define_method(pcp_pmapi_pmdesc_class, "initialize", initialize, 5);
    rb_define_method(pcp_pmapi_pmdesc_class, "pmid", get_pmid, 0);
    rb_define_method(pcp_pmapi_pmdesc_class, "type", get_type, 0);
    rb_define_method(pcp_pmapi_pmdesc_class, "indom", get_indom, 0);
    rb_define_method(pcp_pmapi_pmdesc_class, "sem", get_sem, 0);
    rb_define_method(pcp_pmapi_pmdesc_class, "units", get_units, 0);

    rb_define_alloc_func(pcp_pmapi_pmdesc_class, rb_pmapi_pmdesc_alloc);

}
