#ifndef RB_PCP_PMAPI_PMVALUESET_H
#define RB_PCP_PMAPI_PMVALUESET_H 1
VALUE pcp_pmapi_pmvalueset_class;
VALUE rb_pmapi_pmvalueset_new(pmValueSet *pm_value_set);
pmValueSet *rb_pmapi_pmvalueset_ptr(VALUE pm_value_set);
void init_rb_pmapi_pmvalueset(VALUE pmapi_class);
#endif
