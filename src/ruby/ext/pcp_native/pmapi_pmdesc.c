#include <ruby.h>
#include <pcp/pmapi.h>

#include "pmapi_pmdesc.h"
#include "pmapi_pmunits.h"

VALUE pcp_pmapi_pmdesc_class;



static pmDesc* rb_pmapi_pmdesc_ptr(VALUE self) {
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

static VALUE rb_pmapi_pmdesc_pmid(VALUE self) {
    return UINT2NUM(rb_pmapi_pmdesc_ptr(self)->pmid);
}

static VALUE rb_pmapi_pmdesc_pmid_set(VALUE self, VALUE pmid) {
    rb_pmapi_pmdesc_ptr(self)->pmid = NUM2UINT(pmid);
    return Qnil;
}

static VALUE rb_pmapi_pmdesc_type(VALUE self) {
    return INT2NUM(rb_pmapi_pmdesc_ptr(self)->type);
}

static VALUE rb_pmapi_pmdesc_type_set(VALUE self, VALUE type) {
    rb_pmapi_pmdesc_ptr(self)->type = NUM2INT(type);
    return Qnil;
}

static VALUE rb_pmapi_pmdesc_indom(VALUE self) {
    return UINT2NUM(rb_pmapi_pmdesc_ptr(self)->indom);
}

static VALUE rb_pmapi_pmdesc_indom_set(VALUE self, VALUE indom) {
    rb_pmapi_pmdesc_ptr(self)->indom = NUM2UINT(indom);
    return Qnil;
}

static VALUE rb_pmapi_pmdesc_sem(VALUE self) {
    return INT2NUM(rb_pmapi_pmdesc_ptr(self)->sem);
}

static VALUE rb_pmapi_pmdesc_sem_set(VALUE self, VALUE sem) {
    rb_pmapi_pmdesc_ptr(self)->sem = NUM2INT(sem);
    return Qnil;
}

VALUE rb_pmapi_pmdesc_new(pmDesc pm_desc) {
    VALUE instance;
    pmDesc *pm_desc_from_instance;

    instance = rb_pmapi_pmdesc_alloc(pcp_pmapi_pmdesc_class);
    Data_Get_Struct(instance, pmDesc, pm_desc_from_instance);
    memcpy(pm_desc_from_instance, &pm_desc, sizeof(pmDesc));

    rb_iv_set(instance, "@units", rb_pmapi_pmunits_new(pm_desc_from_instance->units));

    return instance;
}

void init_rb_pmapi_pmdesc(VALUE pmapi_class) {

    pcp_pmapi_pmdesc_class = rb_define_class_under(pmapi_class, "PmDesc", rb_cObject);
    rb_define_method(pcp_pmapi_pmdesc_class, "pmid", rb_pmapi_pmdesc_pmid, 0);
    rb_define_private_method(pcp_pmapi_pmdesc_class, "pmid=", rb_pmapi_pmdesc_pmid_set, 1);
    rb_define_method(pcp_pmapi_pmdesc_class, "type", rb_pmapi_pmdesc_type, 0);
    rb_define_private_method(pcp_pmapi_pmdesc_class, "type=", rb_pmapi_pmdesc_type_set, 1);
    rb_define_method(pcp_pmapi_pmdesc_class, "indom", rb_pmapi_pmdesc_indom, 0);
    rb_define_private_method(pcp_pmapi_pmdesc_class, "indom=", rb_pmapi_pmdesc_indom_set, 1);
    rb_define_method(pcp_pmapi_pmdesc_class, "sem", rb_pmapi_pmdesc_sem, 0);
    rb_define_private_method(pcp_pmapi_pmdesc_class, "sem=", rb_pmapi_pmdesc_sem_set, 1);



    rb_define_alloc_func(pcp_pmapi_pmdesc_class, rb_pmapi_pmdesc_alloc);

}
