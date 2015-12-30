#ifndef RB_PCP_PMAPI_PMVALUEBLOCK_H
#define RB_PCP_PMAPI_PMVALUEBLOCK_H 1

VALUE pcp_pmapi_pmvalueblock_class;
void init_rb_pmapi_pmvalueblock(VALUE);
VALUE rb_pmapi_pmvalueblock_new(pmValueBlock*);
pmValueBlock *rb_pmapi_pmvalueblock_ptr(VALUE);

#endif
