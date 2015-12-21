#ifndef RB_PCP_PMAPI_PMUNITS_H
#define RB_PCP_PMAPI_PMUNITS_H 1

void init_rb_pmapi_pmunits(VALUE);
VALUE rb_pmapi_pmunits_new_from_pmdesc(pmUnits*, VALUE);
pmUnits rb_pmapi_pmunits_get(VALUE);

#endif