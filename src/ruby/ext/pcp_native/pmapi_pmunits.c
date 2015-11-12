#include <ruby.h>
#include <pcp/pmapi.h>

#include "pmapi_pmunits.h"

VALUE pcp_pmapi_pmunits_class = Qnil;


static void rb_pmapi_pmunits_free(pmUnits *pm_units) {
    free(pm_units);
}

static VALUE rb_pmapi_pmunits_alloc(VALUE klass) {
    pmUnits *units_to_wrap = malloc(sizeof(pmUnits));

    return Data_Wrap_Struct(klass, 0, rb_pmapi_pmunits_free, units_to_wrap);
}

static pmUnits* rb_pmapi_pmunits_ptr(VALUE self) {
    pmUnits *pm_units;

    Data_Get_Struct(self, pmUnits, pm_units);

    return pm_units;
}

static VALUE rb_pmapi_pmunits_dimSpace(VALUE self) {
    return INT2NUM(rb_pmapi_pmunits_ptr(self)->dimSpace);
}

static VALUE rb_pmapi_pmunits_dimSpaceSet(VALUE self, VALUE dimSpace) {
    rb_pmapi_pmunits_ptr(self)->dimSpace = NUM2INT(dimSpace);
    return Qnil;
}

static VALUE rb_pmapi_pmunits_dimTime(VALUE self) {
    return INT2NUM(rb_pmapi_pmunits_ptr(self)->dimTime);
}

static VALUE rb_pmapi_pmunits_dimTimeSet(VALUE self, VALUE dimTime) {
    rb_pmapi_pmunits_ptr(self)->dimTime = NUM2INT(dimTime);
    return Qnil;
}

static VALUE rb_pmapi_pmunits_dimCount(VALUE self) {
    return INT2NUM(rb_pmapi_pmunits_ptr(self)->dimCount);
}

static VALUE rb_pmapi_pmunits_dimCountSet(VALUE self, VALUE dimCount) {
    rb_pmapi_pmunits_ptr(self)->dimCount = NUM2INT(dimCount);
    return Qnil;
}

static VALUE rb_pmapi_pmunits_scaleSpace(VALUE self) {
    return UINT2NUM(rb_pmapi_pmunits_ptr(self)->scaleSpace);
}

static VALUE rb_pmapi_pmunits_scaleSpaceSet(VALUE self, VALUE scaleSpace) {
    rb_pmapi_pmunits_ptr(self)->scaleSpace = NUM2UINT(scaleSpace);
    return Qnil;
}

static VALUE rb_pmapi_pmunits_scaleTime(VALUE self) {
    return UINT2NUM(rb_pmapi_pmunits_ptr(self)->scaleTime);
}

static VALUE rb_pmapi_pmunits_scaleTimeSet(VALUE self, VALUE scaleTime) {
    rb_pmapi_pmunits_ptr(self)->scaleTime = NUM2UINT(scaleTime);
    return Qnil;
}

static VALUE rb_pmapi_pmunits_scaleCount(VALUE self) {
    return INT2NUM(rb_pmapi_pmunits_ptr(self)->scaleCount);
}

static VALUE rb_pmapi_pmunits_scaleCountSet(VALUE self, VALUE scaleCount) {
    rb_pmapi_pmunits_ptr(self)->scaleCount = NUM2INT(scaleCount);
    return Qnil;
}

VALUE rb_pmapi_pmunits_new(pmUnits pm_units) {
    VALUE instance;
    pmUnits *units_from_instance;

    instance = rb_pmapi_pmunits_alloc(pcp_pmapi_pmunits_class);
    Data_Get_Struct(instance, pmUnits, units_from_instance);
    memcpy(units_from_instance, &pm_units, sizeof(pmUnits));

    return instance;
}


void init_rb_pmapi_pmunits(VALUE pmapi_class) {

    pcp_pmapi_pmunits_class = rb_define_class_under(pmapi_class, "PmUnits", rb_cObject);
    rb_define_method(pcp_pmapi_pmunits_class, "dimSpace", rb_pmapi_pmunits_dimSpace, 0);
    rb_define_method(pcp_pmapi_pmunits_class, "dimSpace=", rb_pmapi_pmunits_dimSpaceSet, 1);
    rb_define_method(pcp_pmapi_pmunits_class, "dimTime", rb_pmapi_pmunits_dimTime, 0);
    rb_define_method(pcp_pmapi_pmunits_class, "dimTime=", rb_pmapi_pmunits_dimTimeSet, 1);
    rb_define_method(pcp_pmapi_pmunits_class, "dimCount", rb_pmapi_pmunits_dimCount, 0);
    rb_define_method(pcp_pmapi_pmunits_class, "dimCount=", rb_pmapi_pmunits_dimCountSet, 1);
    rb_define_method(pcp_pmapi_pmunits_class, "scaleSpace", rb_pmapi_pmunits_scaleSpace, 0);
    rb_define_method(pcp_pmapi_pmunits_class, "scaleSpace=", rb_pmapi_pmunits_scaleSpaceSet, 1);
    rb_define_method(pcp_pmapi_pmunits_class, "scaleTime", rb_pmapi_pmunits_scaleTime, 0);
    rb_define_method(pcp_pmapi_pmunits_class, "scaleTime=", rb_pmapi_pmunits_scaleTimeSet, 1);
    rb_define_method(pcp_pmapi_pmunits_class, "scaleCount", rb_pmapi_pmunits_scaleCount, 0);
    rb_define_method(pcp_pmapi_pmunits_class, "scaleCount=", rb_pmapi_pmunits_scaleCountSet, 1);

    rb_define_alloc_func(pcp_pmapi_pmunits_class, rb_pmapi_pmunits_alloc);

}