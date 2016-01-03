#ifndef RB_PCP_PMAPI_PMRESULT_H
#define RB_PCP_PMAPI_PMRESULT_H 1
VALUE rb_pmapi_pmresult_new(pmResult *pm_result);
pmResult* rb_pmapi_pmresult_ptr(VALUE pm_result);
void init_rb_pmapi_pmresult(VALUE pmapi_class);
#endif
