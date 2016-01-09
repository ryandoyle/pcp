#include <ruby/ruby.h>
#include <pcp/pmapi.h>

#include "pmapi_pmloglabel.h"

VALUE pcp_pmapi_pmloglabel_class = Qnil;

static pmLogLabel* get_pmloglabel_ptr(VALUE self) {
    pmLogLabel *pm_log_label;
    Data_Get_Struct(self, pmLogLabel, pm_log_label);
    return pm_log_label;
}

static void pmloglabel_free(void *pm_log_label_raw) {
    pmLogLabel *pm_log_label = (pmLogLabel*)pm_log_label_raw;
    xfree(pm_log_label);
}

static VALUE initialize(VALUE self) {
    rb_raise(rb_eNotImpError, "creating PmLogLabel instances in Ruby is not supported");
    return Qnil;
}

static VALUE get_ll_magic(VALUE self) {
    return INT2NUM(get_pmloglabel_ptr(self)->ll_magic);
}

static VALUE get_ll_pid(VALUE self) {
    return INT2NUM(get_pmloglabel_ptr(self)->ll_pid);
}

static VALUE get_ll_start(VALUE self) {
    struct timeval ll_start = get_pmloglabel_ptr(self)->ll_start;

    return rb_time_new(ll_start.tv_sec, ll_start.tv_usec);
}

static VALUE get_ll_hostname(VALUE self) {
    return rb_str_new_cstr(get_pmloglabel_ptr(self)->ll_hostname);
}

static VALUE get_ll_tz(VALUE self) {
    return rb_str_new_cstr(get_pmloglabel_ptr(self)->ll_tz);
}

VALUE rb_pmapi_pmloglabel_new(pmLogLabel pm_log_label) {
    pmLogLabel *pm_log_label_to_wrap = ALLOC(pmLogLabel);
    VALUE instance;

    instance = Data_Wrap_Struct(pcp_pmapi_pmloglabel_class, 0, pmloglabel_free, pm_log_label_to_wrap);

    memcpy(pm_log_label_to_wrap, &pm_log_label, sizeof(pmLogLabel));

    return instance;
}

void init_rb_pmapi_pmloglabel(VALUE pmapi_class) {
    pcp_pmapi_pmloglabel_class = rb_define_class_under(pmapi_class, "PmLogLabel", rb_cObject);

    rb_define_private_method(pcp_pmapi_pmloglabel_class, "initialize", initialize, 0);
    rb_define_method(pcp_pmapi_pmloglabel_class, "ll_magic", get_ll_magic, 0);
    rb_define_method(pcp_pmapi_pmloglabel_class, "ll_pid", get_ll_pid, 0);
    rb_define_method(pcp_pmapi_pmloglabel_class, "ll_start", get_ll_start, 0);
    rb_define_method(pcp_pmapi_pmloglabel_class, "ll_hostname", get_ll_hostname, 0);
    rb_define_method(pcp_pmapi_pmloglabel_class, "ll_tz", get_ll_tz, 0);
}