/*
 * Copyright (C) 2015 Ryan Doyle.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 */

#include <ruby.h>
#include <pcp/pmapi.h>

#include "pmapi.h"
#include "pmapi_pmunits.h"
#include "pmapi_pmdesc.h"
#include "pmapi_pmvalue.h"
#include "pmapi_pmvalueblock.h"
#include "pmapi_pmvalueset.h"
#include "pmapi_ctest.h"
#include "pmapi_pmresult.h"
#include "pmapi_pmloglabel.h"

VALUE pcp_module = Qnil;
VALUE pcp_pmapi_class = Qnil;

/* Error classes */
VALUE pcp_pmapi_error = Qnil;
VALUE pcp_pmapi_pmns_error = Qnil;
VALUE pcp_pmapi_no_pmns_error = Qnil;
VALUE pcp_pmapi_dup_pmns_error = Qnil;
VALUE pcp_pmapi_text_error = Qnil;
VALUE pcp_pmapi_app_version_error = Qnil;
VALUE pcp_pmapi_value_error = Qnil;
VALUE pcp_pmapi_timeout_error = Qnil;
VALUE pcp_pmapi_no_data_error = Qnil;
VALUE pcp_pmapi_reset_error = Qnil;
VALUE pcp_pmapi_name_error = Qnil;
VALUE pcp_pmapi_pmid_error = Qnil;
VALUE pcp_pmapi_indom_error = Qnil;
VALUE pcp_pmapi_inst_error = Qnil;
VALUE pcp_pmapi_unit_error = Qnil;
VALUE pcp_pmapi_conv_error = Qnil;
VALUE pcp_pmapi_trunc_error = Qnil;
VALUE pcp_pmapi_sign_error = Qnil;
VALUE pcp_pmapi_profile_error = Qnil;
VALUE pcp_pmapi_ipc_error = Qnil;
VALUE pcp_pmapi_eof_error = Qnil;
VALUE pcp_pmapi_not_host_error = Qnil;
VALUE pcp_pmapi_eol_error = Qnil;
VALUE pcp_pmapi_mode_error = Qnil;
VALUE pcp_pmapi_label_error = Qnil;
VALUE pcp_pmapi_log_rec_error = Qnil;
VALUE pcp_pmapi_not_archive_error = Qnil;
VALUE pcp_pmapi_log_file_error = Qnil;
VALUE pcp_pmapi_no_context_error = Qnil;
VALUE pcp_pmapi_profile_spec_error = Qnil;
VALUE pcp_pmapi_pmid_log_error = Qnil;
VALUE pcp_pmapi_indom_log_error = Qnil;
VALUE pcp_pmapi_inst_log_error = Qnil;
VALUE pcp_pmapi_no_profile_error = Qnil;
VALUE pcp_pmapi_no_agent_error = Qnil;
VALUE pcp_pmapi_permission_error = Qnil;
VALUE pcp_pmapi_connlimit_error = Qnil;
VALUE pcp_pmapi_again_error = Qnil;
VALUE pcp_pmapi_is_conn_error = Qnil;
VALUE pcp_pmapi_not_conn_error = Qnil;
VALUE pcp_pmapi_need_port_error = Qnil;
VALUE pcp_pmapi_non_leaf_error = Qnil;
VALUE pcp_pmapi_type_error = Qnil;
VALUE pcp_pmapi_thread_error = Qnil;
VALUE pcp_pmapi_no_container_error = Qnil;
VALUE pcp_pmapi_bad_store_error = Qnil;
VALUE pcp_pmapi_too_small_error = Qnil;
VALUE pcp_pmapi_too_big_error = Qnil;
VALUE pcp_pmapi_fault_error = Qnil;
VALUE pcp_pmapi_pmda_ready_error = Qnil;
VALUE pcp_pmapi_pmda_not_ready_error = Qnil;
VALUE pcp_pmapi_nyi_error = Qnil;

static const struct pm_mode_to_string {
    int pm_mode;
    const char *string;
} pm_mode_to_string_map[] = {
    {PM_MODE_LIVE, "PM_MODE_LIVE"},
    {PM_MODE_INTERP, "PM_MODE_INTERP"},
    {PM_MODE_FORW, "PM_MODE_FORW"},
    {PM_MODE_BACK, "PM_MODE_BACK"},
    {-1, "UNKNOWN"},
};

static const struct pmapi_to_ruby_exception {
    int pmapi_error;
    VALUE *ruby_exception;
} pmapi_to_ruby_exception_map[] = {
    {PM_ERR_GENERIC, &pcp_pmapi_error},
    {PM_ERR_PMNS, &pcp_pmapi_pmns_error},
    {PM_ERR_NOPMNS, &pcp_pmapi_no_pmns_error},
    {PM_ERR_DUPPMNS, &pcp_pmapi_dup_pmns_error},
    {PM_ERR_TEXT, &pcp_pmapi_text_error},
    {PM_ERR_APPVERSION, &pcp_pmapi_app_version_error},
    {PM_ERR_VALUE, &pcp_pmapi_value_error},
    {PM_ERR_TIMEOUT, &pcp_pmapi_timeout_error},
    {PM_ERR_NODATA, &pcp_pmapi_no_data_error},
    {PM_ERR_RESET, &pcp_pmapi_reset_error},
    {PM_ERR_NAME, &pcp_pmapi_name_error},
    {PM_ERR_PMID, &pcp_pmapi_pmid_error},
    {PM_ERR_INDOM, &pcp_pmapi_indom_error},
    {PM_ERR_INST, &pcp_pmapi_inst_error},
    {PM_ERR_UNIT, &pcp_pmapi_unit_error},
    {PM_ERR_CONV, &pcp_pmapi_conv_error},
    {PM_ERR_TRUNC, &pcp_pmapi_trunc_error},
    {PM_ERR_SIGN, &pcp_pmapi_sign_error},
    {PM_ERR_PROFILE, &pcp_pmapi_profile_error},
    {PM_ERR_IPC, &pcp_pmapi_ipc_error},
    {PM_ERR_EOF, &pcp_pmapi_eof_error},
    {PM_ERR_NOTHOST, &pcp_pmapi_not_host_error},
    {PM_ERR_EOL, &pcp_pmapi_eol_error},
    {PM_ERR_MODE, &pcp_pmapi_mode_error},
    {PM_ERR_LABEL, &pcp_pmapi_label_error},
    {PM_ERR_LOGREC, &pcp_pmapi_log_rec_error},
    {PM_ERR_NOTARCHIVE, &pcp_pmapi_not_archive_error},
    {PM_ERR_LOGFILE, &pcp_pmapi_log_file_error},
    {PM_ERR_NOCONTEXT, &pcp_pmapi_no_context_error},
    {PM_ERR_PROFILESPEC, &pcp_pmapi_profile_spec_error},
    {PM_ERR_PMID_LOG, &pcp_pmapi_pmid_log_error},
    {PM_ERR_INDOM_LOG, &pcp_pmapi_indom_log_error},
    {PM_ERR_INST_LOG, &pcp_pmapi_inst_log_error},
    {PM_ERR_NOPROFILE, &pcp_pmapi_no_profile_error},
    {PM_ERR_NOAGENT, &pcp_pmapi_no_agent_error},
    {PM_ERR_PERMISSION, &pcp_pmapi_permission_error},
    {PM_ERR_CONNLIMIT, &pcp_pmapi_connlimit_error},
    {PM_ERR_AGAIN, &pcp_pmapi_again_error},
    {PM_ERR_ISCONN, &pcp_pmapi_is_conn_error},
    {PM_ERR_NOTCONN, &pcp_pmapi_not_conn_error},
    {PM_ERR_NEEDPORT, &pcp_pmapi_need_port_error},
    {PM_ERR_NONLEAF, &pcp_pmapi_non_leaf_error},
    {PM_ERR_TYPE, &pcp_pmapi_type_error},
    {PM_ERR_THREAD, &pcp_pmapi_thread_error},
    {PM_ERR_NOCONTAINER, &pcp_pmapi_no_container_error},
    {PM_ERR_BADSTORE, &pcp_pmapi_bad_store_error},
    {PM_ERR_TOOSMALL, &pcp_pmapi_too_small_error},
    {PM_ERR_TOOBIG, &pcp_pmapi_too_big_error},
    {PM_ERR_FAULT, &pcp_pmapi_fault_error},
    {PM_ERR_PMDAREADY, &pcp_pmapi_pmda_ready_error},
    {PM_ERR_PMDANOTREADY, &pcp_pmapi_pmda_not_ready_error},
    {PM_ERR_NYI, &pcp_pmapi_nyi_error},
};

//#ifdef DEBUG
#include <pcp/impl.h>
//pmDebug = DBG_TRACE_CONTEXT && DBG_TRACE_PMNS;
//#endif

typedef struct {
    int pm_context;
} PmApi_Context;

static void deallocate(void *pmapi_context_raw) {
    PmApi_Context *pmapi_context = (PmApi_Context*) pmapi_context_raw;
    pmDestroyContext(pmapi_context->pm_context);
    xfree(pmapi_context);
}

static VALUE allocate(VALUE self) {
    PmApi_Context *pmapi_context = ALLOC(PmApi_Context);
    return Data_Wrap_Struct(self, 0, deallocate, pmapi_context);
}

static int get_context(VALUE self) {
    PmApi_Context *pmapi_context;
    Data_Get_Struct(self, PmApi_Context, pmapi_context);
    return pmapi_context->pm_context;
}

static void use_context(VALUE self) {
    pmUseContext(get_context(self));
}

static const char* get_pm_mode_string_from_value(int pm_mode) {
    int i, number_of_pm_modes;
    number_of_pm_modes = sizeof(pm_mode_to_string_map) / sizeof(struct pm_mode_to_string);

    for(i=0; i < number_of_pm_modes; i++) {
        if(pm_mode_to_string_map[i].pm_mode == pm_mode) {
            return pm_mode_to_string_map[i].string;
        }
    }
    /* Last element is a value of "UNKNOWN" */
    return pm_mode_to_string_map[number_of_pm_modes-1].string;
}

static VALUE get_exception_from_pmapi_error_code(int error_code) {
    int i, number_of_pmapi_to_ruby_errors;
    number_of_pmapi_to_ruby_errors = sizeof(pmapi_to_ruby_exception_map) / sizeof(struct pmapi_to_ruby_exception);

    for(i=0; i < number_of_pmapi_to_ruby_errors; i++) {
        if(pmapi_to_ruby_exception_map[i].pmapi_error == error_code) {
            return *pmapi_to_ruby_exception_map[i].ruby_exception;
        }
    }
    /* Default to a generic error */
    return pcp_pmapi_error;
}

void rb_pmapi_raise_error_from_pm_error_code(int error_number) {
    char errmsg[PM_MAXERRMSGLEN];
    VALUE exception_to_raise;

    exception_to_raise = get_exception_from_pmapi_error_code(error_number);

    rb_raise(exception_to_raise, (const char *)pmErrStr_r(error_number, (char *)&errmsg, sizeof(errmsg)));
}

static VALUE rb_pmNewContext(VALUE self, VALUE metric_source, VALUE metric_source_argument) {
    int pm_context, pm_metric_source;
    char *pm_metric_source_argument = NULL;
    PmApi_Context *pmapi_context;
    Data_Get_Struct(self, PmApi_Context, pmapi_context);

    pm_metric_source = NUM2INT(metric_source);

    if(TYPE(metric_source_argument) == T_STRING) {
        pm_metric_source_argument = StringValuePtr(metric_source_argument);
    }

    if ((pm_context = pmNewContext(pm_metric_source, pm_metric_source_argument)) < 0) {
        rb_pmapi_raise_error_from_pm_error_code(pm_context);
    }

    pmapi_context->pm_context = pm_context;

    return self;
}
/* call-seq: pmapi.pmGetContextHostName
 *
 * Returns the hostname that the context is connected to
 *
 */
static VALUE rb_pmGetContextHostName(VALUE self) {
    char result_buffer[MAXHOSTNAMELEN];

    pmGetContextHostName_r(get_context(self), (char *)&result_buffer, MAXHOSTNAMELEN);

    return rb_tainted_str_new_cstr(result_buffer);
}

/* call-seq: pmapi.pmGetPMNSLocation => location
 *
 * Returns the location of the PMNS. Either +PCP::PMAPI::PMNS_LOCAL+,
 * +PCP::PMAPI::PMNS_REMOTE+ or +PCP::PMAPI::PMNS_ARCHIVE+ depending
 * how the context was created.
 *
 *     pmapi.pmGetPMNSLocation #=> PMNS_LOCAL
 *
 */
static VALUE rb_pmGetPMNSLocation(VALUE self) {
    int pmns_location;

    use_context(self);

    pmns_location = pmGetPMNSLocation();

    return INT2NUM(pmns_location);
}

/* call-seq: pmapi.pmLoadNameSpace(filename)
 *
 * Load the namespace from a file on disk.
 *
 *     pmapi.pmLoadNameSpace('/path/to/namespace.pmns')
 */
static VALUE rb_pmLoadNameSpace(VALUE self, VALUE filename) {
    int error;

    use_context(self);

    error = pmLoadNameSpace(StringValuePtr(filename));
    if(error < 0 ) {
        rb_pmapi_raise_error_from_pm_error_code(error);
    }

    return Qnil;
}

static VALUE rb_pmUnloadNameSpace(VALUE self) {
    use_context(self);

    pmUnloadNameSpace();

    return Qnil;
}

static VALUE rb_pmTrimNameSpace(VALUE self) {
    use_context(self);

    pmTrimNameSpace();

    return Qnil;
}

static VALUE rb_pmLookupName(VALUE self, VALUE names) {

    int number_of_names, error;
    char ** namelist;
    pmID * pmidlist;
    long i;
    VALUE result;

    use_context(self);

    number_of_names = RARRAY_LENINT(names);
    namelist = ALLOC_N(char*, number_of_names);
    pmidlist = ALLOC_N(pmID, number_of_names);
    result = rb_ary_new2(number_of_names);

    /* Populate name list */
    for(i = 0; i < number_of_names; i++) {
        namelist[i] = RSTRING_PTR(rb_ary_entry(names, i));
    }

    error = pmLookupName(number_of_names, namelist, pmidlist);
    if(error < 0 ) {
        xfree(namelist);
        xfree(pmidlist);
        rb_pmapi_raise_error_from_pm_error_code(error);
    } else {
        /* Build up the array of hashes to return */
        for(i = 0; i < number_of_names; i++) {
            VALUE hash = rb_hash_new();
            VALUE hash_key = rb_ary_entry(names, i);
            VALUE pmid = UINT2NUM(pmidlist[i]);
            rb_hash_aset(hash, hash_key, pmid);
            rb_ary_push(result, hash);
        }
    }

    xfree(namelist);
    xfree(pmidlist);

    return result;
}

static VALUE rb_pmGetChildren(VALUE self, VALUE root_name) {

    char ** offspring;
    int number_of_children, error, i;
    VALUE children_names;

    use_context(self);

    children_names = rb_ary_new();

    if((error = pmGetChildren(StringValueCStr(root_name), &offspring)) < 0 ) {
        rb_pmapi_raise_error_from_pm_error_code(error);
    } else {
        number_of_children = error;
        for(i = 0; i < number_of_children; i++) {
            rb_ary_push(children_names, rb_tainted_str_new_cstr(offspring[i]));
        }
    }

    if(number_of_children > 0) {
        free(offspring);
    }

    return children_names;
}

static VALUE rb_pmGetChildrenStatus(VALUE self, VALUE root_name) {

    char ** offspring;
    int * offspring_leaf_status;
    int number_of_children, error, i;
    VALUE children_names_and_status;

    use_context(self);

    children_names_and_status = rb_ary_new();

    if((error = pmGetChildrenStatus(StringValueCStr(root_name), &offspring, &offspring_leaf_status)) < 0 ) {
        rb_pmapi_raise_error_from_pm_error_code(error);
    } else {
        number_of_children = error;
        for(i = 0; i < number_of_children; i++) {
            VALUE child_hash = rb_hash_new();
            VALUE child_name = rb_tainted_str_new_cstr(offspring[i]);
            VALUE child_status = INT2NUM(offspring_leaf_status[i]);
            rb_hash_aset(child_hash, child_name, child_status);
            rb_ary_push(children_names_and_status, child_hash);
        }
    }

    if(number_of_children > 0) {
        free(offspring);
        free(offspring_leaf_status);
    }

    return children_names_and_status;
}

static VALUE rb_pmNameID(VALUE self, VALUE pmid) {
    int error;
    char *name_result;
    VALUE result;

    use_context(self);

    if((error = pmNameID(NUM2UINT(pmid), &name_result)) < 0) {
        rb_pmapi_raise_error_from_pm_error_code(error);
        return Qnil;
    } else {
        result = rb_tainted_str_new_cstr(name_result);
        /* TODO: Do we need to free tainted strings? */
        free(name_result);
        return result;
    }
}

static VALUE rb_pmNameAll(VALUE self, VALUE pmid) {
    int error, number_of_names, i;
    char ** name_result;
    VALUE result;

    use_context(self);

    if((error = pmNameAll(NUM2UINT(pmid), &name_result)) < 0) {
        rb_pmapi_raise_error_from_pm_error_code(error);
        return Qnil;
    } else {
        number_of_names = error;
        result = rb_ary_new2(number_of_names);
        for(i = 0; i < number_of_names; i++) {
            rb_ary_push(result, rb_tainted_str_new_cstr(name_result[i]));
        }
    }

    free(name_result);
    return result;
}

static void dometric(const char * name) {
    rb_yield(rb_tainted_str_new_cstr(name));
}

static VALUE rb_pmTraversePMNS(VALUE self, VALUE name) {
    int error;

    use_context(self);

    if((error = pmTraversePMNS(StringValueCStr(name), dometric)) < 0) {
        rb_pmapi_raise_error_from_pm_error_code(error);
    }

    return Qnil;
}

static VALUE rb_pmLookupDesc(VALUE self, VALUE pmid) {
    int error;
    pmDesc pmDesc;

    use_context(self);

    if((error = pmLookupDesc(NUM2UINT(pmid), &pmDesc)) < 0) {
        rb_pmapi_raise_error_from_pm_error_code(error);
        return Qnil;
    }

    return rb_pmapi_pmdesc_new(pmDesc);
}


static VALUE pmLookupInDom_with_lookup_function(VALUE self, VALUE indom, VALUE name, int(*indom_lookup_function)(pmInDom, const char *)) {
    int error_or_result;

    use_context(self);

    if((error_or_result = indom_lookup_function(NUM2UINT(indom), StringValueCStr(name))) < 0) {
        rb_pmapi_raise_error_from_pm_error_code(error_or_result);
        return Qnil;
    }

    return INT2NUM(error_or_result);
}

static VALUE rb_pmLookupInDom(VALUE self, VALUE indom, VALUE name) {
    return pmLookupInDom_with_lookup_function(self, indom, name, pmLookupInDom);
}

static VALUE rb_pmLookupInDomArchive(VALUE self, VALUE indom, VALUE name) {
    return pmLookupInDom_with_lookup_function(self, indom, name, pmLookupInDomArchive);
}

static VALUE pmNameInDom_with_lookup_function(VALUE self, VALUE indom, VALUE instance_id, int(*indom_lookup_function)(pmInDom, int, char **)) {
    int error;
    char *name;
    VALUE result;

    use_context(self);

    if((error = indom_lookup_function(NUM2UINT(indom), NUM2INT(instance_id), &name)) < 0) {
        rb_pmapi_raise_error_from_pm_error_code(error);
        return Qnil;
    }

    result = rb_tainted_str_new_cstr(name);
    free(name);

    return result;
}

static VALUE rb_pmNameInDom(VALUE self, VALUE indom, VALUE instance_id) {
    return pmNameInDom_with_lookup_function(self, indom, instance_id, pmNameInDom);
}

static VALUE rb_pmNameInDomArchive(VALUE self, VALUE indom, VALUE instance_id) {
    return pmNameInDom_with_lookup_function(self, indom, instance_id, pmNameInDomArchive);
}

static VALUE pmGetInDom_with_lookup_function(VALUE self, VALUE indom, int(*indom_lookup_function)(pmInDom, int **, char ***)) {
    int error_or_number_of_instances, i;
    int *instance_ids;
    char **instance_names;
    VALUE result;

    use_context(self);

    if((error_or_number_of_instances = indom_lookup_function(NUM2UINT(indom), &instance_ids, &instance_names)) < 0) {
        rb_pmapi_raise_error_from_pm_error_code(error_or_number_of_instances);
        return Qnil;
    }

    result = rb_ary_new2(error_or_number_of_instances);

    for(i=0; i<error_or_number_of_instances; i++) {
        VALUE instance_hash = rb_hash_new();
        VALUE instance_name = rb_tainted_str_new_cstr(instance_names[i]);
        VALUE instance_id = INT2NUM(instance_ids[i]);
        rb_hash_aset(instance_hash, instance_id, instance_name);
        rb_ary_push(result, instance_hash);

    }

    free(instance_ids);
    free(instance_names);

    return result;
}

static VALUE rb_pmGetInDom(VALUE self, VALUE indom) {
    return pmGetInDom_with_lookup_function(self, indom, pmGetInDom);
}

static VALUE rb_pmGetInDomArchive(VALUE self, VALUE indom) {
    return pmGetInDom_with_lookup_function(self, indom, pmGetInDomArchive);
}

static VALUE rb_pmWhichContext(VALUE self) {
    int error_or_context_number;

    use_context(self);

    if((error_or_context_number = pmWhichContext()) < 0) {
        rb_pmapi_raise_error_from_pm_error_code(error_or_context_number);
        return Qnil;
    }

    return INT2NUM(error_or_context_number);
}

static VALUE rb_pmDestroyContext() {
    rb_raise(rb_eNotImpError, "pmDestroyContext is controlled by object lifecycle");
    return Qnil;
}

static VALUE rb_pmDupContext() {
    rb_raise(rb_eNotImpError, "pmDupContext not supported");
    return Qnil;
}

static VALUE rb_pmUseContext() {
    rb_raise(rb_eNotImpError, "pmUseContext not supported");
    return Qnil;
}

static VALUE rb_pmReconnectContext(VALUE self) {
    int error;

    if((error = pmReconnectContext(get_context(self))) < 0) {
        rb_pmapi_raise_error_from_pm_error_code(error);
    }

    return Qnil;
}

static VALUE rb_pmDelProfile(VALUE self, VALUE indom, VALUE instance_identifiers) {
    int error, number_of_instance_identifiers, i;
    int *instlist = NULL;

    use_context(self);

    number_of_instance_identifiers = RARRAY_LENINT(instance_identifiers);

    if(number_of_instance_identifiers > 0) {
        instlist = ALLOC_N(int, number_of_instance_identifiers);
        for(i = 0; i < number_of_instance_identifiers; i++) {
            instlist[i] = NUM2INT(rb_ary_entry(instance_identifiers, i));
        }
    }

    if((error = pmDelProfile(NUM2UINT(indom), number_of_instance_identifiers, instlist)) < 0) {
        rb_pmapi_raise_error_from_pm_error_code(error);
    }

    if(instlist) {
        xfree(instlist);
    }

    return Qnil;
}

static VALUE rb_pmAddProfile(VALUE self, VALUE indom, VALUE instance_identifiers) {
    int error, number_of_instance_identifiers, i;
    int *instlist = NULL;

    use_context(self);

    number_of_instance_identifiers = RARRAY_LENINT(instance_identifiers);

    if(number_of_instance_identifiers > 0) {
        instlist = ALLOC_N(int, number_of_instance_identifiers);
        for(i = 0; i < number_of_instance_identifiers; i++) {
            instlist[i] = NUM2INT(rb_ary_entry(instance_identifiers, i));
        }
    }

    if((error = pmAddProfile(NUM2UINT(indom), number_of_instance_identifiers, instlist)) < 0) {
        if(instlist) {
            xfree(instlist);
        }
        rb_pmapi_raise_error_from_pm_error_code(error);
    }

    if(instlist) {
        xfree(instlist);
    }

    return Qnil;
}

static VALUE rb_pmFetch(VALUE self, VALUE pmids) {
    int number_of_pmids, i, error;
    pmID *pmidlist;
    pmResult *pm_fetch_result;
    VALUE result;

    use_context(self);

    if(TYPE(pmids) != T_ARRAY) {
        rb_raise(rb_eArgError, "PMIDs must be an array");
    }

    number_of_pmids = RARRAY_LENINT(pmids);
    pmidlist = ALLOC_N(pmID, number_of_pmids);

    for(i = 0; i < number_of_pmids; i++) {
        pmidlist[i] = NUM2UINT(rb_ary_entry(pmids, i));
    }

    if((error = pmFetch(number_of_pmids, pmidlist, &pm_fetch_result)) < 0) {
        xfree(pmidlist);
        rb_pmapi_raise_error_from_pm_error_code(error);
        return Qnil;
    }


    result = rb_pmapi_pmresult_new(pm_fetch_result);

    pmFreeResult(pm_fetch_result);
    xfree(pmidlist);

    return result;
}

static VALUE rb_pmFetchArchive(VALUE self) {
    int error;
    pmResult *pm_fetch_archive_result;
    VALUE result;

    use_context(self);

    if((error = pmFetchArchive(&pm_fetch_archive_result)) < 0) {
        rb_pmapi_raise_error_from_pm_error_code(error);
        return Qnil;
    }

    result = rb_pmapi_pmresult_new(pm_fetch_archive_result);

    pmFreeResult(pm_fetch_archive_result);

    return result;
}

static VALUE rb_pmGetArchiveLabel(VALUE self) {
    int error;
    pmLogLabel pm_log_label;

    use_context(self);

    if((error = pmGetArchiveLabel(&pm_log_label)) < 0) {
        rb_pmapi_raise_error_from_pm_error_code(error);
        return Qnil;
    }

    return rb_pmapi_pmloglabel_new(pm_log_label);
}

static VALUE rb_pmGetArchiveEnd(VALUE self) {
    int error;
    struct timeval time;

    use_context(self);

    if((error = pmGetArchiveEnd(&time)) < 0) {
        rb_pmapi_raise_error_from_pm_error_code(error);
        return Qnil;
    }

    return rb_time_new(time.tv_sec, time.tv_usec);
}

/* call-seq: pmapi.pmFreeResult
 *
 * Not implemented. Lifecycle of pmResult is controlled by Ruby.
 *
 * Raises a NotImplementedError if called.
 *
 */
static VALUE rb_pmFreeResult() {
    rb_raise(rb_eNotImpError, "pmFreeResult not supported");
    return Qnil;
}

/* call-seq: pmapi.pmFreeHighResResult
 *
 * Not implemented. Lifecycle of pmHighResResult is controlled by Ruby.
 *
 * Raises a NotImplementedError if called.
 *
 */
static VALUE rb_pmFreeHighResResult() {
    rb_raise(rb_eNotImpError, "pmFreeHighResResult not supported");
    return Qnil;
}

static void pm_atom_free_pointer(void *freeable) {
    free(freeable);
}

static void pm_atom_noop_free_pointer(void *freeable) {
    (void)freeable;
}

static VALUE rb_extract_value_from_pmatomvalue(int pm_type, pmAtomValue pm_atom_value,  void(*pm_atom_free_function)(void *)) {
    VALUE result = Qnil;

    switch(pm_type) {
        case PM_TYPE_32:
            result = INT2NUM(pm_atom_value.l);
            break;
        case PM_TYPE_U32:
            result = UINT2NUM(pm_atom_value.ul);
            break;
        case PM_TYPE_64:
            result = LL2NUM(pm_atom_value.ll);
            break;
        case PM_TYPE_U64:
            result = ULL2NUM(pm_atom_value.ull);
            break;
        case PM_TYPE_FLOAT:
            result = DBL2NUM(pm_atom_value.f);
            break;
        case PM_TYPE_DOUBLE:
            result = DBL2NUM(pm_atom_value.d);
            break;
        case PM_TYPE_STRING:
            result = rb_tainted_str_new_cstr(pm_atom_value.cp);
            /* Some usages of extracting values from value blocks don't require freeing memory. Allow the caller
             * to pass in their own version of a freeing function and call that instead */
            pm_atom_free_function(pm_atom_value.cp);
            break;
        case PM_TYPE_AGGREGATE:
            pm_atom_free_function(pm_atom_value.vbp);
        default:
            rb_raise(pcp_pmapi_error, "Metric data type %d not supported", pm_type);
    }

    return result;
}

/* call-seq: pmapi.pmExtractValue
 */
static VALUE rb_pmExtractValue(VALUE self, VALUE value_format_rb, VALUE pm_value_rb, VALUE input_type_rb, VALUE output_type_rb) {
    pmAtomValue pm_atom_value;
    int error;
    int value_format = NUM2INT(value_format_rb);
    pmValue pm_value = rb_pmapi_pmvalue_ptr(pm_value_rb);
    int input_type = NUM2INT(input_type_rb);
    int output_type = NUM2INT(output_type_rb);

    if((error = pmExtractValue(value_format, &pm_value, input_type, &pm_atom_value, output_type)) < 0) {
        rb_pmapi_raise_error_from_pm_error_code(error);
        return Qnil;
    }

    return rb_extract_value_from_pmatomvalue(output_type, pm_atom_value, pm_atom_free_pointer);
}

/* call-seq: pmapi.pmPrintValue
 *
 * Not implemented. Methods that return a pmResult already do the type conversion required
 * to easily print results in Ruby code.
 *
 * Raises a NotImplementedError if called.
 *
 */
static VALUE rb_pmPrintValue() {
    rb_raise(rb_eNotImpError, "pmPrintValue not supported");
    return Qnil;
}

static pmAtomValue rb_build_pmatomvalue(int pm_type, VALUE value_rb) {
    pmAtomValue atom_value;

    switch (pm_type) {
        case PM_TYPE_32:
            atom_value.l = NUM2INT(value_rb);
            break;
        case PM_TYPE_U32:
            atom_value.ul = NUM2UINT(value_rb);
            break;
        case PM_TYPE_64:
            atom_value.ll = NUM2LL(value_rb);
            break;
        case PM_TYPE_U64:
            atom_value.ull = NUM2ULL(value_rb);
            break;
        case PM_TYPE_FLOAT:
            atom_value.f = (float)NUM2DBL(value_rb);
            break;
        case PM_TYPE_DOUBLE:
            atom_value.d = NUM2DBL(value_rb);
            break;
        case PM_TYPE_STRING:
            atom_value.cp = StringValueCStr(value_rb);
            break;
        default:
            rb_raise(pcp_pmapi_error, "Metric data type %d not supported", pm_type);
            break;
    }

    return atom_value;
}

static VALUE rb_pmConvScale(VALUE self, VALUE pm_type_rb, VALUE value_rb, VALUE input_pmunits_rb, VALUE output_pmunits_rb) {
    int error;
    int pm_type = NUM2INT(pm_type_rb);
    pmAtomValue input_atom_value;
    pmAtomValue output_atom_value;
    pmUnits *input_pm_units = rb_pmapi_pmunits_ptr(input_pmunits_rb);
    pmUnits *output_pm_units = rb_pmapi_pmunits_ptr(output_pmunits_rb);

    input_atom_value = rb_build_pmatomvalue(pm_type, value_rb);

    if((error = pmConvScale(pm_type, &input_atom_value, input_pm_units, &output_atom_value, output_pm_units)) < 0) {
        rb_pmapi_raise_error_from_pm_error_code(error);
        return Qnil;
    }

    return rb_extract_value_from_pmatomvalue(pm_type, output_atom_value, pm_atom_noop_free_pointer);
}

static VALUE rb_pmSortInstances() {
    rb_raise(rb_eNotImpError, "pmSortInstances not supported");
    return Qnil;
}

static VALUE rb_pmSetMode(int argc, VALUE *argv, VALUE self) {
    VALUE pm_mode_rb;
    VALUE time_rb;
    VALUE delta_rb;
    VALUE pm_time_rb;
    int pm_mode, delta, error;
    struct timeval time;

    rb_scan_args(argc, argv, "31", &pm_mode_rb, &time_rb, &delta_rb ,&pm_time_rb);

    pm_mode = NUM2INT(pm_mode_rb);
    time = rb_time_timeval(time_rb);
    delta = NUM2INT(delta_rb);

    if(!NIL_P(pm_time_rb)) {
        /* Passing in a PM_TIME_* only applies for PM_MODE_INTERP */
        if(pm_mode != PM_MODE_INTERP) {
            rb_raise(rb_eArgError, "Setting time mode not supported for %s", get_pm_mode_string_from_value(pm_mode));
            return Qnil;
        }
        pm_mode |= PM_XTB_SET(NUM2INT(pm_time_rb));
    }

    use_context(self);

    if((error = pmSetMode(pm_mode, &time, delta)) < 0) {
        rb_pmapi_raise_error_from_pm_error_code(error);
    }

    return Qnil;
}

static VALUE rb_pmStore(VALUE self, VALUE pm_result_rb) {
    const pmResult *pm_result;
    int error;

    use_context(self);

    pm_result = rb_pmapi_pmresult_ptr(pm_result_rb);

    if((error = pmStore(pm_result)) < 0) {
        rb_pmapi_raise_error_from_pm_error_code(error);
    }

    return Qnil;
}

static VALUE rb_pmLookupText(VALUE self, VALUE pmid_rb, VALUE pm_text_type_rb) {
    pmID pmid;
    int pm_text_type, error;
    char *result_buffer;
    VALUE result;

    use_context(self);

    pmid = NUM2UINT(pmid_rb);
    pm_text_type = NUM2INT(pm_text_type_rb);

    if((error = pmLookupText(pmid, pm_text_type, &result_buffer)) < 0) {
        rb_pmapi_raise_error_from_pm_error_code(error);
        return Qnil;
    }

    result = rb_tainted_str_new_cstr(result_buffer);
    free(result_buffer);

    return result;
}

static VALUE rb_pmLookupInDomText(VALUE self, VALUE indom_rb, VALUE pm_text_type_rb) {
    pmInDom indom;
    int pm_text_type, error;
    char *result_buffer;
    VALUE result;

    use_context(self);

    indom = NUM2UINT(indom_rb);
    pm_text_type = NUM2INT(pm_text_type_rb);

    if((error = pmLookupInDomText(indom, pm_text_type, &result_buffer)) < 0) {
        rb_pmapi_raise_error_from_pm_error_code(error);
        return Qnil;
    }

    result = rb_tainted_str_new_cstr(result_buffer);
    free(result_buffer);

    return result;
}

static VALUE rb_pmIDStr(VALUE self, VALUE pmid_rb) {
    pmID pmid;
    char result_buffer[20];

    pmid = NUM2UINT(pmid_rb);

    pmIDStr_r(pmid, result_buffer, sizeof(result_buffer));

    return rb_tainted_str_new_cstr(result_buffer);
}

static VALUE rb_pmInDomStr(VALUE self, VALUE indom_rb) {
    pmInDom indom;
    char result_buffer[20];

    indom = NUM2UINT(indom_rb);

    pmInDomStr_r(indom, result_buffer, sizeof(result_buffer));

    return rb_tainted_str_new_cstr(result_buffer);
}

static VALUE rb_pmTypeStr(VALUE self, VALUE pmtype_rb) {
    int pmtype;
    char result_buffer[20];

    pmtype = NUM2INT(pmtype_rb);

    pmTypeStr_r(pmtype, result_buffer, sizeof(result_buffer));

    return rb_tainted_str_new_cstr(result_buffer);
}

static VALUE rb_pmUnitsStr(VALUE self, VALUE pmunits_rb) {
    pmUnits *pmunits;
    char result_buffer[60];

    pmunits = rb_pmapi_pmunits_ptr(pmunits_rb);

    pmUnitsStr_r(pmunits, result_buffer, sizeof(result_buffer));

    return rb_tainted_str_new_cstr(result_buffer);
}

static VALUE rb_pmAtomStr(VALUE self, VALUE value_rb, VALUE pm_type_rb) {
    char result_buffer[80];
    pmAtomValue atom_value;
    int pm_type = NUM2INT(pm_type_rb);


    atom_value = rb_build_pmatomvalue(pm_type, value_rb);

    pmAtomStr_r(&atom_value, pm_type, result_buffer, sizeof(result_buffer));

    return rb_tainted_str_new_cstr(result_buffer);
}

static VALUE rb_pmNumberStr(VALUE self, VALUE double_rb) {
    char result_buffer[8];

    pmNumberStr_r(NUM2DBL(double_rb), result_buffer, sizeof(result_buffer));

    return rb_tainted_str_new_cstr(result_buffer);
}

static VALUE rb_pmEventFlagsStr(VALUE self) {
    rb_raise(rb_eNotImpError, "pmEventFlagsStr not supported");
    return Qnil;
}

static VALUE rb_pmParseInterval(VALUE self, VALUE parse_string_rb) {
    char *error_string;
    struct timeval time;
    VALUE error_string_rb;

    if(pmParseInterval(StringValueCStr(parse_string_rb), &time, &error_string) < 0) {
        error_string_rb = rb_str_new_cstr(error_string);
        free(error_string);
        rb_raise(pcp_pmapi_error, StringValueCStr(error_string_rb));
        return Qnil;
    }

    return rb_time_new(time.tv_sec, time.tv_usec);
}

static VALUE rb_pmParseTimeWindow(int argc, VALUE *argv, VALUE self) {
    int error;

    VALUE start_time_str_rb;
    VALUE end_time_str_rb;
    VALUE align_str_rb;
    VALUE offset_str_rb;
    VALUE log_start_rb;
    VALUE log_end_rb;
    VALUE error_str_rb;

    struct timeval log_start;
    struct timeval log_end;

    VALUE result_array;
    struct timeval result_start_time;
    struct timeval result_end_time;
    struct timeval result_offset_time;

    char *error_message = NULL;


    /* log_start and log_end are only relevant for pm log mode */
    rb_scan_args(argc, argv, "42", &start_time_str_rb, &end_time_str_rb, &align_str_rb, &offset_str_rb, &log_start_rb, &log_end_rb);

    /* start of log or current time if not using an archive */
    if(TYPE(log_start_rb) != T_NIL) {
        log_start = rb_time_timeval(log_start_rb);
    } else {
        gettimeofday(&log_start, NULL);
    }
    /* end of log or tv_sec == INT_MAX */
    if(TYPE(log_end_rb) != T_NIL) {
        log_end = rb_time_timeval(log_end_rb);
    } else {
        log_end.tv_sec = INT_MAX;
    }

    error = pmParseTimeWindow(
            TYPE(start_time_str_rb) == T_NIL ? NULL : StringValueCStr(start_time_str_rb),
            TYPE(end_time_str_rb) == T_NIL ? NULL : StringValueCStr(end_time_str_rb),
            TYPE(align_str_rb) == T_NIL ? NULL : StringValueCStr(align_str_rb),
            TYPE(offset_str_rb) == T_NIL ? NULL : StringValueCStr(offset_str_rb),
            &log_start,
            &log_end,
            &result_start_time,
            &result_end_time,
            &result_offset_time,
            &error_message);

    if(error == 0){
        /* Its possible to parse the time but with a warning. Printing to stderr is probably the best thing to do */
        fprintf(stderr, "Warning: %s\n", error_message);
        /* According to the man page, error_message is a static buffer but it does call other functions that malloc */
        if(error_message) {
            free(error_message);
        }
    }
    if(error == -1){
        /* Error */
        error_str_rb = rb_tainted_str_new_cstr(error_message);
        if(error_message) {
            free(error_message);
        }
        rb_raise(pcp_pmapi_error, StringValueCStr(error_str_rb));
    }

    result_array = rb_ary_new2(3);
    rb_ary_push(result_array, rb_time_new(result_start_time.tv_sec, result_start_time.tv_usec));
    rb_ary_push(result_array, rb_time_new(result_end_time.tv_sec, result_end_time.tv_usec));
    rb_ary_push(result_array, rb_time_new(result_offset_time.tv_sec, result_offset_time.tv_usec));

    return result_array;

}

void Init_pcp_native() {
    pcp_module = rb_define_module("PCP");
    pcp_pmapi_class = rb_define_class_under(pcp_module, "PMAPI", rb_cObject);
    init_rb_pmapi_pmunits(pcp_pmapi_class);
    init_rb_pmapi_pmdesc(pcp_pmapi_class);
    init_rb_pmapi_pmvalue(pcp_pmapi_class);
    init_rb_pmapi_pmvalueset(pcp_pmapi_class);
    init_rb_pmapi_pmvalueblock(pcp_pmapi_class);
    init_rb_pmapi_pmresult(pcp_pmapi_class);
    init_rb_pmapi_pmloglabel(pcp_pmapi_class);
    init_rb_pmapi_ctest(pcp_pmapi_class);

    /* Exceptions */
    pcp_pmapi_error = rb_define_class_under(pcp_pmapi_class, "Error", rb_eStandardError);
    pcp_pmapi_pmns_error = rb_define_class_under(pcp_pmapi_class, "PMNSError", pcp_pmapi_error);
    pcp_pmapi_no_pmns_error = rb_define_class_under(pcp_pmapi_class, "NoPMNSError", pcp_pmapi_error);
    pcp_pmapi_dup_pmns_error = rb_define_class_under(pcp_pmapi_class, "DupPMNSError", pcp_pmapi_error);
    pcp_pmapi_text_error = rb_define_class_under(pcp_pmapi_class, "TextError", pcp_pmapi_error);
    pcp_pmapi_app_version_error = rb_define_class_under(pcp_pmapi_class, "AppVersionError", pcp_pmapi_error);
    pcp_pmapi_value_error = rb_define_class_under(pcp_pmapi_class, "ValueError", pcp_pmapi_error);
    pcp_pmapi_timeout_error = rb_define_class_under(pcp_pmapi_class, "TimeoutError", pcp_pmapi_error);
    pcp_pmapi_no_data_error = rb_define_class_under(pcp_pmapi_class, "NoDataError", pcp_pmapi_error);
    pcp_pmapi_reset_error = rb_define_class_under(pcp_pmapi_class, "ResetError", pcp_pmapi_error);
    pcp_pmapi_name_error = rb_define_class_under(pcp_pmapi_class, "NameError", pcp_pmapi_error);
    pcp_pmapi_pmid_error = rb_define_class_under(pcp_pmapi_class, "PMIDError", pcp_pmapi_error);
    pcp_pmapi_indom_error = rb_define_class_under(pcp_pmapi_class, "InDomError", pcp_pmapi_error);
    pcp_pmapi_inst_error = rb_define_class_under(pcp_pmapi_class, "InstError", pcp_pmapi_error);
    pcp_pmapi_unit_error = rb_define_class_under(pcp_pmapi_class, "UnitError", pcp_pmapi_error);
    pcp_pmapi_conv_error = rb_define_class_under(pcp_pmapi_class, "ConvError", pcp_pmapi_error);
    pcp_pmapi_trunc_error = rb_define_class_under(pcp_pmapi_class, "TruncError", pcp_pmapi_error);
    pcp_pmapi_sign_error = rb_define_class_under(pcp_pmapi_class, "SignError", pcp_pmapi_error);
    pcp_pmapi_profile_error = rb_define_class_under(pcp_pmapi_class, "ProfileError", pcp_pmapi_error);
    pcp_pmapi_ipc_error = rb_define_class_under(pcp_pmapi_class, "IPCError", pcp_pmapi_error);
    pcp_pmapi_eof_error = rb_define_class_under(pcp_pmapi_class, "EOFError", pcp_pmapi_error);
    pcp_pmapi_not_host_error = rb_define_class_under(pcp_pmapi_class, "NotHostError", pcp_pmapi_error);
    pcp_pmapi_eol_error = rb_define_class_under(pcp_pmapi_class, "EOLError", pcp_pmapi_error);
    pcp_pmapi_mode_error = rb_define_class_under(pcp_pmapi_class, "ModeError", pcp_pmapi_error);
    pcp_pmapi_label_error = rb_define_class_under(pcp_pmapi_class, "LabelError", pcp_pmapi_error);
    pcp_pmapi_log_rec_error = rb_define_class_under(pcp_pmapi_class, "LogRecError", pcp_pmapi_error);
    pcp_pmapi_not_archive_error = rb_define_class_under(pcp_pmapi_class, "NotArchiveError", pcp_pmapi_error);
    pcp_pmapi_log_file_error = rb_define_class_under(pcp_pmapi_class, "LogFileError", pcp_pmapi_error);
    pcp_pmapi_no_context_error = rb_define_class_under(pcp_pmapi_class, "NoContextError", pcp_pmapi_error);
    pcp_pmapi_profile_spec_error = rb_define_class_under(pcp_pmapi_class, "ProfileSpecError", pcp_pmapi_error);
    pcp_pmapi_pmid_log_error = rb_define_class_under(pcp_pmapi_class, "PMIDLogError", pcp_pmapi_error);
    pcp_pmapi_indom_log_error = rb_define_class_under(pcp_pmapi_class, "InDomLogError", pcp_pmapi_error);
    pcp_pmapi_inst_log_error = rb_define_class_under(pcp_pmapi_class, "InstLogError", pcp_pmapi_error);
    pcp_pmapi_no_profile_error = rb_define_class_under(pcp_pmapi_class, "NoProfileError", pcp_pmapi_error);
    pcp_pmapi_no_agent_error = rb_define_class_under(pcp_pmapi_class, "NoAgentError", pcp_pmapi_error);
    pcp_pmapi_permission_error = rb_define_class_under(pcp_pmapi_class, "PermissionError", pcp_pmapi_error);
    pcp_pmapi_connlimit_error = rb_define_class_under(pcp_pmapi_class, "ConnLimitError", pcp_pmapi_error);
    pcp_pmapi_again_error = rb_define_class_under(pcp_pmapi_class, "AgainError", pcp_pmapi_error);
    pcp_pmapi_is_conn_error = rb_define_class_under(pcp_pmapi_class, "IsConnError", pcp_pmapi_error);
    pcp_pmapi_not_conn_error = rb_define_class_under(pcp_pmapi_class, "NotConnError", pcp_pmapi_error);
    pcp_pmapi_need_port_error = rb_define_class_under(pcp_pmapi_class, "NeedPortError", pcp_pmapi_error);
    pcp_pmapi_non_leaf_error = rb_define_class_under(pcp_pmapi_class, "NonLeafError", pcp_pmapi_error);
    pcp_pmapi_type_error = rb_define_class_under(pcp_pmapi_class, "TypeError", pcp_pmapi_error);
    pcp_pmapi_thread_error = rb_define_class_under(pcp_pmapi_class, "ThreadError", pcp_pmapi_error);
    pcp_pmapi_no_container_error = rb_define_class_under(pcp_pmapi_class, "NoContainerError", pcp_pmapi_error);
    pcp_pmapi_bad_store_error = rb_define_class_under(pcp_pmapi_class, "BadStoreError", pcp_pmapi_error);
    pcp_pmapi_too_small_error = rb_define_class_under(pcp_pmapi_class, "TooSmallError", pcp_pmapi_error);
    pcp_pmapi_too_big_error = rb_define_class_under(pcp_pmapi_class, "TooBigError", pcp_pmapi_error);
    pcp_pmapi_fault_error = rb_define_class_under(pcp_pmapi_class, "FaultError", pcp_pmapi_error);
    pcp_pmapi_pmda_ready_error = rb_define_class_under(pcp_pmapi_class, "PMDAReadyError", pcp_pmapi_error);
    pcp_pmapi_pmda_not_ready_error = rb_define_class_under(pcp_pmapi_class, "PMDANotReadyError", pcp_pmapi_error);
    pcp_pmapi_nyi_error = rb_define_class_under(pcp_pmapi_class, "NYIError", pcp_pmapi_error);

    rb_define_const(pcp_pmapi_class, "PM_SPACE_BYTE", INT2NUM(PM_SPACE_BYTE));
    rb_define_const(pcp_pmapi_class, "PM_SPACE_KBYTE", INT2NUM(PM_SPACE_KBYTE));
    rb_define_const(pcp_pmapi_class, "PM_SPACE_MBYTE", INT2NUM(PM_SPACE_MBYTE));
    rb_define_const(pcp_pmapi_class, "PM_SPACE_GBYTE", INT2NUM(PM_SPACE_GBYTE));
    rb_define_const(pcp_pmapi_class, "PM_SPACE_TBYTE", INT2NUM(PM_SPACE_TBYTE));
    rb_define_const(pcp_pmapi_class, "PM_SPACE_PBYTE", INT2NUM(PM_SPACE_PBYTE));
    rb_define_const(pcp_pmapi_class, "PM_SPACE_EBYTE", INT2NUM(PM_SPACE_EBYTE));

    rb_define_const(pcp_pmapi_class, "PM_TIME_NSEC", INT2NUM(PM_TIME_NSEC));
    rb_define_const(pcp_pmapi_class, "PM_TIME_USEC", INT2NUM(PM_TIME_USEC));
    rb_define_const(pcp_pmapi_class, "PM_TIME_MSEC", INT2NUM(PM_TIME_MSEC));
    rb_define_const(pcp_pmapi_class, "PM_TIME_SEC", INT2NUM(PM_TIME_SEC));
    rb_define_const(pcp_pmapi_class, "PM_TIME_MIN", INT2NUM(PM_TIME_MIN));
    rb_define_const(pcp_pmapi_class, "PM_TIME_HOUR", INT2NUM(PM_TIME_HOUR));

    rb_define_const(pcp_pmapi_class, "PM_TYPE_NOSUPPORT", INT2NUM(PM_TYPE_NOSUPPORT));
    rb_define_const(pcp_pmapi_class, "PM_TYPE_32", INT2NUM(PM_TYPE_32));
    rb_define_const(pcp_pmapi_class, "PM_TYPE_U32", INT2NUM(PM_TYPE_U32));
    rb_define_const(pcp_pmapi_class, "PM_TYPE_64", INT2NUM(PM_TYPE_64));
    rb_define_const(pcp_pmapi_class, "PM_TYPE_U64", INT2NUM(PM_TYPE_U64));
    rb_define_const(pcp_pmapi_class, "PM_TYPE_FLOAT", INT2NUM(PM_TYPE_FLOAT));
    rb_define_const(pcp_pmapi_class, "PM_TYPE_DOUBLE", INT2NUM(PM_TYPE_DOUBLE));
    rb_define_const(pcp_pmapi_class, "PM_TYPE_STRING", INT2NUM(PM_TYPE_STRING));
    rb_define_const(pcp_pmapi_class, "PM_TYPE_AGGREGATE", INT2NUM(PM_TYPE_AGGREGATE));
    rb_define_const(pcp_pmapi_class, "PM_TYPE_AGGREGATE_STATIC", INT2NUM(PM_TYPE_AGGREGATE_STATIC));
    rb_define_const(pcp_pmapi_class, "PM_TYPE_EVENT", INT2NUM(PM_TYPE_EVENT));
    rb_define_const(pcp_pmapi_class, "PM_TYPE_HIGHRES_EVENT", INT2NUM(PM_TYPE_HIGHRES_EVENT));
    rb_define_const(pcp_pmapi_class, "PM_TYPE_UNKNOWN", INT2NUM(PM_TYPE_UNKNOWN));

    rb_define_const(pcp_pmapi_class, "PM_SEM_COUNTER", INT2NUM(PM_SEM_COUNTER));
    rb_define_const(pcp_pmapi_class, "PM_SEM_INSTANT", INT2NUM(PM_SEM_INSTANT));
    rb_define_const(pcp_pmapi_class, "PM_SEM_DISCRETE", INT2NUM(PM_SEM_DISCRETE));

    rb_define_const(pcp_pmapi_class, "PM_CONTEXT_UNDEF", INT2NUM(PM_CONTEXT_UNDEF));
    rb_define_const(pcp_pmapi_class, "PM_CONTEXT_HOST", INT2NUM(PM_CONTEXT_HOST));
    rb_define_const(pcp_pmapi_class, "PM_CONTEXT_ARCHIVE", INT2NUM(PM_CONTEXT_ARCHIVE));
    rb_define_const(pcp_pmapi_class, "PM_CONTEXT_LOCAL", INT2NUM(PM_CONTEXT_LOCAL));
    rb_define_const(pcp_pmapi_class, "PM_CONTEXT_TYPEMASK", INT2NUM(PM_CONTEXT_TYPEMASK));
    rb_define_const(pcp_pmapi_class, "PM_CTXFLAG_SECURE", INT2NUM(PM_CTXFLAG_SECURE));
    rb_define_const(pcp_pmapi_class, "PM_CTXFLAG_COMPRESS", INT2NUM(PM_CTXFLAG_COMPRESS));
    rb_define_const(pcp_pmapi_class, "PM_CTXFLAG_RELAXED", INT2NUM(PM_CTXFLAG_RELAXED));
    rb_define_const(pcp_pmapi_class, "PM_CTXFLAG_AUTH", INT2NUM(PM_CTXFLAG_AUTH));
    rb_define_const(pcp_pmapi_class, "PM_CTXFLAG_CONTAINER", INT2NUM(PM_CTXFLAG_CONTAINER));

    rb_define_const(pcp_pmapi_class, "PMNS_LOCAL", INT2NUM(PMNS_LOCAL));
    rb_define_const(pcp_pmapi_class, "PMNS_REMOTE", INT2NUM(PMNS_REMOTE));
    rb_define_const(pcp_pmapi_class, "PMNS_ARCHIVE", INT2NUM(PMNS_ARCHIVE));

    rb_define_const(pcp_pmapi_class, "PM_VAL_HDR_SIZE", UINT2NUM(PM_VAL_HDR_SIZE));

    rb_define_const(pcp_pmapi_class, "PM_VAL_INSITU", INT2NUM(PM_VAL_INSITU));
    rb_define_const(pcp_pmapi_class, "PM_VAL_DPTR", INT2NUM(PM_VAL_DPTR));
    rb_define_const(pcp_pmapi_class, "PM_VAL_SPTR", INT2NUM(PM_VAL_SPTR));

    rb_define_const(pcp_pmapi_class, "PM_ERR_GENERIC", INT2NUM(PM_ERR_GENERIC));
    rb_define_const(pcp_pmapi_class, "PM_ERR_PMNS", INT2NUM(PM_ERR_PMNS));
    rb_define_const(pcp_pmapi_class, "PM_ERR_NOPMNS", INT2NUM(PM_ERR_NOPMNS));
    rb_define_const(pcp_pmapi_class, "PM_ERR_DUPPMNS", INT2NUM(PM_ERR_DUPPMNS));
    rb_define_const(pcp_pmapi_class, "PM_ERR_TEXT", INT2NUM(PM_ERR_TEXT));
    rb_define_const(pcp_pmapi_class, "PM_ERR_APPVERSION", INT2NUM(PM_ERR_APPVERSION));
    rb_define_const(pcp_pmapi_class, "PM_ERR_VALUE", INT2NUM(PM_ERR_VALUE));
    rb_define_const(pcp_pmapi_class, "PM_ERR_TIMEOUT", INT2NUM(PM_ERR_TIMEOUT));
    rb_define_const(pcp_pmapi_class, "PM_ERR_NODATA", INT2NUM(PM_ERR_NODATA));
    rb_define_const(pcp_pmapi_class, "PM_ERR_RESET", INT2NUM(PM_ERR_RESET));
    rb_define_const(pcp_pmapi_class, "PM_ERR_NAME", INT2NUM(PM_ERR_NAME));
    rb_define_const(pcp_pmapi_class, "PM_ERR_PMID", INT2NUM(PM_ERR_PMID));
    rb_define_const(pcp_pmapi_class, "PM_ERR_INDOM", INT2NUM(PM_ERR_INDOM));
    rb_define_const(pcp_pmapi_class, "PM_ERR_INST", INT2NUM(PM_ERR_INST));
    rb_define_const(pcp_pmapi_class, "PM_ERR_UNIT", INT2NUM(PM_ERR_UNIT));
    rb_define_const(pcp_pmapi_class, "PM_ERR_CONV", INT2NUM(PM_ERR_CONV));
    rb_define_const(pcp_pmapi_class, "PM_ERR_TRUNC", INT2NUM(PM_ERR_TRUNC));
    rb_define_const(pcp_pmapi_class, "PM_ERR_SIGN", INT2NUM(PM_ERR_SIGN));
    rb_define_const(pcp_pmapi_class, "PM_ERR_PROFILE", INT2NUM(PM_ERR_PROFILE));
    rb_define_const(pcp_pmapi_class, "PM_ERR_IPC", INT2NUM(PM_ERR_IPC));
    rb_define_const(pcp_pmapi_class, "PM_ERR_EOF", INT2NUM(PM_ERR_EOF));
    rb_define_const(pcp_pmapi_class, "PM_ERR_NOTHOST", INT2NUM(PM_ERR_NOTHOST));
    rb_define_const(pcp_pmapi_class, "PM_ERR_EOL", INT2NUM(PM_ERR_EOL));
    rb_define_const(pcp_pmapi_class, "PM_ERR_MODE", INT2NUM(PM_ERR_MODE));
    rb_define_const(pcp_pmapi_class, "PM_ERR_LABEL", INT2NUM(PM_ERR_LABEL));
    rb_define_const(pcp_pmapi_class, "PM_ERR_LOGREC", INT2NUM(PM_ERR_LOGREC));
    rb_define_const(pcp_pmapi_class, "PM_ERR_NOTARCHIVE", INT2NUM(PM_ERR_NOTARCHIVE));
    rb_define_const(pcp_pmapi_class, "PM_ERR_LOGFILE", INT2NUM(PM_ERR_LOGFILE));
    rb_define_const(pcp_pmapi_class, "PM_ERR_NOCONTEXT", INT2NUM(PM_ERR_NOCONTEXT));
    rb_define_const(pcp_pmapi_class, "PM_ERR_PROFILESPEC", INT2NUM(PM_ERR_PROFILESPEC));
    rb_define_const(pcp_pmapi_class, "PM_ERR_PMID_LOG", INT2NUM(PM_ERR_PMID_LOG));
    rb_define_const(pcp_pmapi_class, "PM_ERR_INDOM_LOG", INT2NUM(PM_ERR_INDOM_LOG));
    rb_define_const(pcp_pmapi_class, "PM_ERR_INST_LOG", INT2NUM(PM_ERR_INST_LOG));
    rb_define_const(pcp_pmapi_class, "PM_ERR_NOPROFILE", INT2NUM(PM_ERR_NOPROFILE));
    rb_define_const(pcp_pmapi_class, "PM_ERR_NOAGENT", INT2NUM(PM_ERR_NOAGENT));
    rb_define_const(pcp_pmapi_class, "PM_ERR_PERMISSION", INT2NUM(PM_ERR_PERMISSION));
    rb_define_const(pcp_pmapi_class, "PM_ERR_CONNLIMIT", INT2NUM(PM_ERR_CONNLIMIT));
    rb_define_const(pcp_pmapi_class, "PM_ERR_AGAIN", INT2NUM(PM_ERR_AGAIN));
    rb_define_const(pcp_pmapi_class, "PM_ERR_ISCONN", INT2NUM(PM_ERR_ISCONN));
    rb_define_const(pcp_pmapi_class, "PM_ERR_NOTCONN", INT2NUM(PM_ERR_NOTCONN));
    rb_define_const(pcp_pmapi_class, "PM_ERR_NEEDPORT", INT2NUM(PM_ERR_NEEDPORT));
    rb_define_const(pcp_pmapi_class, "PM_ERR_NONLEAF", INT2NUM(PM_ERR_NONLEAF));
    rb_define_const(pcp_pmapi_class, "PM_ERR_TYPE", INT2NUM(PM_ERR_TYPE));
    rb_define_const(pcp_pmapi_class, "PM_ERR_THREAD", INT2NUM(PM_ERR_THREAD));
    rb_define_const(pcp_pmapi_class, "PM_ERR_NOCONTAINER", INT2NUM(PM_ERR_NOCONTAINER));
    rb_define_const(pcp_pmapi_class, "PM_ERR_BADSTORE", INT2NUM(PM_ERR_BADSTORE));
    rb_define_const(pcp_pmapi_class, "PM_ERR_TOOSMALL", INT2NUM(PM_ERR_TOOSMALL));
    rb_define_const(pcp_pmapi_class, "PM_ERR_TOOBIG", INT2NUM(PM_ERR_TOOBIG));
    rb_define_const(pcp_pmapi_class, "PM_ERR_FAULT", INT2NUM(PM_ERR_FAULT));
    rb_define_const(pcp_pmapi_class, "PM_ERR_PMDAREADY", INT2NUM(PM_ERR_PMDAREADY));
    rb_define_const(pcp_pmapi_class, "PM_ERR_PMDANOTREADY", INT2NUM(PM_ERR_PMDANOTREADY));
    rb_define_const(pcp_pmapi_class, "PM_ERR_NYI", INT2NUM(PM_ERR_NYI));

    rb_define_const(pcp_pmapi_class, "PMNS_LEAF_STATUS", INT2NUM(PMNS_LEAF_STATUS));
    rb_define_const(pcp_pmapi_class, "PMNS_NONLEAF_STATUS", INT2NUM(PMNS_NONLEAF_STATUS));

    rb_define_const(pcp_pmapi_class, "PM_ID_NULL", INT2NUM(PM_ID_NULL));

    rb_define_const(pcp_pmapi_class, "PM_INDOM_NULL", UINT2NUM(PM_INDOM_NULL));
    rb_define_const(pcp_pmapi_class, "PM_IN_NULL", INT2NUM(PM_IN_NULL));

    rb_define_const(pcp_pmapi_class, "PM_MODE_LIVE", INT2NUM(PM_MODE_LIVE));
    rb_define_const(pcp_pmapi_class, "PM_MODE_INTERP", INT2NUM(PM_MODE_INTERP));
    rb_define_const(pcp_pmapi_class, "PM_MODE_FORW", INT2NUM(PM_MODE_FORW));
    rb_define_const(pcp_pmapi_class, "PM_MODE_BACK", INT2NUM(PM_MODE_BACK));

    rb_define_const(pcp_pmapi_class, "PM_TEXT_ONELINE", INT2NUM(PM_TEXT_ONELINE));
    rb_define_const(pcp_pmapi_class, "PM_TEXT_HELP", INT2NUM(PM_TEXT_HELP));

    rb_define_alloc_func(pcp_pmapi_class, allocate);
    rb_define_private_method(pcp_pmapi_class, "pmNewContext", rb_pmNewContext, 2);
    rb_define_method(pcp_pmapi_class, "pmGetContextHostName", rb_pmGetContextHostName, 0);
    rb_define_method(pcp_pmapi_class, "pmGetPMNSLocation", rb_pmGetPMNSLocation, 0);
    rb_define_method(pcp_pmapi_class, "pmLoadNameSpace", rb_pmLoadNameSpace, 1);
    rb_define_method(pcp_pmapi_class, "pmUnloadNameSpace", rb_pmUnloadNameSpace, 0);
    rb_define_method(pcp_pmapi_class, "pmTrimNameSpace", rb_pmTrimNameSpace, 0);
    rb_define_method(pcp_pmapi_class, "pmLookupName", rb_pmLookupName, 1);
    rb_define_method(pcp_pmapi_class, "pmGetChildren", rb_pmGetChildren, 1);
    rb_define_method(pcp_pmapi_class, "pmGetChildrenStatus", rb_pmGetChildrenStatus, 1);
    rb_define_method(pcp_pmapi_class, "pmNameID", rb_pmNameID, 1);
    rb_define_method(pcp_pmapi_class, "pmNameAll", rb_pmNameAll, 1);
    rb_define_method(pcp_pmapi_class, "pmTraversePMNS", rb_pmTraversePMNS, 1);
    rb_define_method(pcp_pmapi_class, "pmLookupDesc", rb_pmLookupDesc, 1);
    rb_define_method(pcp_pmapi_class, "pmLookupInDom", rb_pmLookupInDom, 2);
    rb_define_method(pcp_pmapi_class, "pmLookupInDomArchive", rb_pmLookupInDomArchive, 2);
    rb_define_method(pcp_pmapi_class, "pmNameInDom", rb_pmNameInDom, 2);
    rb_define_method(pcp_pmapi_class, "pmNameInDomArchive", rb_pmNameInDomArchive, 2);
    rb_define_method(pcp_pmapi_class, "pmGetInDom", rb_pmGetInDom, 1);
    rb_define_method(pcp_pmapi_class, "pmGetInDomArchive", rb_pmGetInDomArchive, 1);
    rb_define_method(pcp_pmapi_class, "pmWhichContext", rb_pmWhichContext, 0);
    rb_define_method(pcp_pmapi_class, "pmDestroyContext", rb_pmDestroyContext, 0);
    rb_define_method(pcp_pmapi_class, "pmDupContext", rb_pmDupContext, 0);
    rb_define_method(pcp_pmapi_class, "pmUseContext", rb_pmUseContext, 0);
    rb_define_method(pcp_pmapi_class, "pmReconnectContext", rb_pmReconnectContext, 0);
    rb_define_method(pcp_pmapi_class, "pmDelProfile", rb_pmDelProfile, 2);
    rb_define_method(pcp_pmapi_class, "pmAddProfile", rb_pmAddProfile, 2);
    rb_define_method(pcp_pmapi_class, "pmFetch", rb_pmFetch, 1);
    rb_define_method(pcp_pmapi_class, "pmFetchArchive", rb_pmFetchArchive, 0);
    rb_define_method(pcp_pmapi_class, "pmGetArchiveLabel", rb_pmGetArchiveLabel, 0);
    rb_define_method(pcp_pmapi_class, "pmGetArchiveEnd", rb_pmGetArchiveEnd, 0);
    rb_define_method(pcp_pmapi_class, "pmFreeResult", rb_pmFreeResult, 0);
    rb_define_method(pcp_pmapi_class, "pmFreeHighResResult", rb_pmFreeHighResResult, 0);
    rb_define_method(pcp_pmapi_class, "pmExtractValue", rb_pmExtractValue, 4);
    rb_define_method(pcp_pmapi_class, "pmPrintValue", rb_pmPrintValue, 0);
    rb_define_method(pcp_pmapi_class, "pmConvScale", rb_pmConvScale, 4);
    rb_define_method(pcp_pmapi_class, "pmSortInstances", rb_pmSortInstances, 0);
    rb_define_method(pcp_pmapi_class, "pmSetMode", rb_pmSetMode, -1);
    rb_define_method(pcp_pmapi_class, "pmStore", rb_pmStore, 1);
    rb_define_method(pcp_pmapi_class, "pmLookupText", rb_pmLookupText, 2);
    rb_define_method(pcp_pmapi_class, "pmLookupInDomText", rb_pmLookupInDomText, 2);
    rb_define_singleton_method(pcp_pmapi_class, "pmIDStr", rb_pmIDStr, 1);
    rb_define_singleton_method(pcp_pmapi_class, "pmInDomStr", rb_pmInDomStr, 1);
    rb_define_singleton_method(pcp_pmapi_class, "pmTypeStr", rb_pmTypeStr, 1);
    rb_define_singleton_method(pcp_pmapi_class, "pmUnitsStr", rb_pmUnitsStr, 1);
    rb_define_singleton_method(pcp_pmapi_class, "pmAtomStr", rb_pmAtomStr, 2);
    rb_define_singleton_method(pcp_pmapi_class, "pmNumberStr", rb_pmNumberStr, 1);
    rb_define_singleton_method(pcp_pmapi_class, "pmEventFlagsStr", rb_pmEventFlagsStr, 0);
    rb_define_singleton_method(pcp_pmapi_class, "pmParseInterval", rb_pmParseInterval, 1);
    rb_define_singleton_method(pcp_pmapi_class, "pmParseTimeWindow", rb_pmParseTimeWindow, -1);

}