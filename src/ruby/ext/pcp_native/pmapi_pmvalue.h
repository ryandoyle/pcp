#ifndef RB_PCP_PMAPI_PMVALUE_H
#define RB_PCP_PMAPI_PMVALUE_H 1

VALUE pcp_pmapi_pmvalue_class;
VALUE rb_pmapi_pmvalue_new(pmValue, int);
pmValue rb_pmapi_pmvalue_ptr(VALUE pm_value);
void init_rb_pmapi_pmvalue(VALUE);

#endif
