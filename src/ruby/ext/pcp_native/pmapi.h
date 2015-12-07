#ifndef RB_PCP_PMAPI_H
#define RB_PCP_PMAPI_H 1

VALUE pcp_pmapi_error;
void rb_pmapi_raise_error_from_pm_error_code(int error_number);

#endif
