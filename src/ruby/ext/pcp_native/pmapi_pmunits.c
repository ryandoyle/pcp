#include <ruby.h>
#include <pcp/pmapi.h>

#include "pmapi_pmunits.h"

VALUE pcp_pmapi_pmunits_class = Qnil;


static void rb_pmapi_pmunits_free(pmUnits *pm_units) {
    xfree(pm_units);
}

static VALUE rb_pmapi_pmunits_alloc(VALUE klass) {
    pmUnits *units_to_wrap = ALLOC(pmUnits);

    return Data_Wrap_Struct(klass, 0, rb_pmapi_pmunits_free, units_to_wrap);
}

static pmUnits* rb_pmapi_pmunits_ptr(VALUE self) {
    pmUnits *pm_units;

    Data_Get_Struct(self, pmUnits, pm_units);

    return pm_units;
}

static VALUE initialize(VALUE self, VALUE dimSpace, VALUE dimTime, VALUE dimCount, VALUE scaleSpace, VALUE scaleTime, VALUE scaleCount) {
    pmUnits *pm_units = rb_pmapi_pmunits_ptr(self);

    pm_units->dimSpace = NUM2INT(dimSpace);
    pm_units->dimTime = NUM2INT(dimTime);
    pm_units->dimCount = NUM2INT(dimCount);
    pm_units->scaleSpace = NUM2UINT(scaleSpace);
    pm_units->scaleTime = NUM2UINT(scaleTime);
    pm_units->scaleCount = NUM2INT(scaleCount);

    return self;
}

static VALUE rb_pmapi_pmunits_dimSpace(VALUE self) {
    return INT2NUM(rb_pmapi_pmunits_ptr(self)->dimSpace);
}

static VALUE rb_pmapi_pmunits_dimTime(VALUE self) {
    return INT2NUM(rb_pmapi_pmunits_ptr(self)->dimTime);
}

static VALUE rb_pmapi_pmunits_dimCount(VALUE self) {
    return INT2NUM(rb_pmapi_pmunits_ptr(self)->dimCount);
}

static VALUE rb_pmapi_pmunits_scaleSpace(VALUE self) {
    return UINT2NUM(rb_pmapi_pmunits_ptr(self)->scaleSpace);
}

static VALUE rb_pmapi_pmunits_scaleTime(VALUE self) {
    return UINT2NUM(rb_pmapi_pmunits_ptr(self)->scaleTime);
}

static VALUE rb_pmapi_pmunits_scaleCount(VALUE self) {
    return INT2NUM(rb_pmapi_pmunits_ptr(self)->scaleCount);
}

VALUE rb_pmapi_pmunits_new_from_pmdesc(pmUnits *pm_units, VALUE pm_desc) {
    VALUE instance;

    instance = Data_Wrap_Struct(pcp_pmapi_pmunits_class, 0, 0, pm_units);
    /* Keep a reference to pm_desc. The pmUnits struct we are passing in is owned by
     * the pmDesc struct and it manages the memory that this class uses */
    rb_iv_set(instance, "@pmdesc", pm_desc);

    return instance;
}

pmUnits rb_pmapi_pmunits_get(VALUE self) {
    pmUnits pm_units;

    memcpy(&pm_units, rb_pmapi_pmunits_ptr(self), sizeof(pmUnits));

    return pm_units;
}

void init_rb_pmapi_pmunits(VALUE pmapi_class) {

    pcp_pmapi_pmunits_class = rb_define_class_under(pmapi_class, "PmUnits", rb_cObject);
    rb_define_method(pcp_pmapi_pmunits_class, "initialize", initialize, 6);
    rb_define_method(pcp_pmapi_pmunits_class, "dimSpace", rb_pmapi_pmunits_dimSpace, 0);
    rb_define_method(pcp_pmapi_pmunits_class, "dimTime", rb_pmapi_pmunits_dimTime, 0);
    rb_define_method(pcp_pmapi_pmunits_class, "dimCount", rb_pmapi_pmunits_dimCount, 0);
    rb_define_method(pcp_pmapi_pmunits_class, "scaleSpace", rb_pmapi_pmunits_scaleSpace, 0);
    rb_define_method(pcp_pmapi_pmunits_class, "scaleTime", rb_pmapi_pmunits_scaleTime, 0);
    rb_define_method(pcp_pmapi_pmunits_class, "scaleCount", rb_pmapi_pmunits_scaleCount, 0);

    rb_define_alloc_func(pcp_pmapi_pmunits_class, rb_pmapi_pmunits_alloc);

}