#include <ruby/ruby.h>
#include <pcp/pmapi.h>

#include "pmapi_pmvalueblock.h"
#include "pmapi.h"

#define VBUF_BYTES_PREALLOCATED 1


VALUE pcp_pmapi_pmvalueblock_class = Qnil;

/* We need this wrapper as the pmValueBlock pointer could be changed when realloc()ed
 * in increase_vbuf_by() */
typedef struct pmValueBlockWrapper {
    pmValueBlock *pm_value_block_ptr;
} pmValueBlockWrapper;

static void rb_pmapi_pmvalueblock_free(void *pm_value_block_wrapper) {
    pmValueBlockWrapper *wrapper = (pmValueBlockWrapper *)pm_value_block_wrapper;

    xfree(wrapper->pm_value_block_ptr);
    xfree(wrapper);
}

static VALUE rb_pmapi_pmvalueblock_alloc(VALUE klass) {
    pmValueBlockWrapper *pm_valueblock_wrapper = ALLOC(pmValueBlockWrapper);
    /* We don't know the size of the vbuf yet. That will be realloc()ed later */
    pmValueBlock *pm_value_block = ALLOC(pmValueBlock);

    /* Set the size to start off */
    pm_value_block->vlen = PM_VAL_HDR_SIZE + VBUF_BYTES_PREALLOCATED;

    pm_valueblock_wrapper->pm_value_block_ptr = pm_value_block;

    return Data_Wrap_Struct(klass, 0, rb_pmapi_pmvalueblock_free, pm_valueblock_wrapper);
}

static void increase_vbuf_by(pmValueBlockWrapper *pm_value_block_wrapper, size_t size) {
    pmValueBlock *pm_value_block = pm_value_block_wrapper->pm_value_block_ptr;
    size_t bytes_to_add = size - VBUF_BYTES_PREALLOCATED;

    pm_value_block = (pmValueBlock*)xrealloc(pm_value_block, sizeof(pmValueBlock) + bytes_to_add);

    /* Re-set the pointer on the wrapper in case the realloc() assigned a new pointer */
    pm_value_block_wrapper->pm_value_block_ptr = pm_value_block;

    pm_value_block->vlen += bytes_to_add;
}

static void build_type_64(VALUE value, pmValueBlockWrapper *pm_value_block_wrapper) {
    __int64_t long_value = NUM2LL(value);

    increase_vbuf_by(pm_value_block_wrapper, sizeof(__int64_t));
    memcpy((void *) pm_value_block_wrapper->pm_value_block_ptr->vbuf, (void*)&long_value, sizeof(__int64_t));
}

static void build_type_u64(VALUE value, pmValueBlockWrapper *pm_value_block_wrapper) {
    __uint64_t long_value = NUM2ULL(value);

    increase_vbuf_by(pm_value_block_wrapper, sizeof(__uint64_t));
    memcpy((void *) pm_value_block_wrapper->pm_value_block_ptr->vbuf, (void*)&long_value, sizeof(__uint64_t));
}

static void build_type_float(VALUE value, pmValueBlockWrapper *pm_value_block_wrapper) {
    float float_value = (float)NUM2DBL(value);

    increase_vbuf_by(pm_value_block_wrapper, sizeof(float));
    memcpy((void *) pm_value_block_wrapper->pm_value_block_ptr->vbuf, (void*)&float_value, sizeof(float));
}

static void build_type_double(VALUE value, pmValueBlockWrapper *pm_value_block_wrapper) {
    double double_value = NUM2DBL(value);

    increase_vbuf_by(pm_value_block_wrapper, sizeof(double));
    memcpy((void *) pm_value_block_wrapper->pm_value_block_ptr->vbuf, (void*)&double_value, sizeof(double));
}

static void build_type_string(VALUE value, pmValueBlockWrapper *pm_value_block_wrapper) {
    char *string_value = StringValueCStr(value);

    increase_vbuf_by(pm_value_block_wrapper, strlen(string_value)+1);
    strcpy(pm_value_block_wrapper->pm_value_block_ptr->vbuf, string_value);
}

static pmValueBlock* get_pm_value_block_from_wrapper(VALUE self) {
    pmValueBlockWrapper *pm_value_block_wrapper;

    Data_Get_Struct(self, pmValueBlockWrapper, pm_value_block_wrapper);

    return pm_value_block_wrapper->pm_value_block_ptr;
}


static VALUE initialize(VALUE self, VALUE value, VALUE type) {

    pmValueBlockWrapper *pm_value_block_wrapper;
    unsigned int pm_type;

    pm_type = NUM2UINT(type);
    Data_Get_Struct(self, pmValueBlockWrapper, pm_value_block_wrapper);

    pm_value_block_wrapper->pm_value_block_ptr->vtype = pm_type;

    switch(pm_type) {
        case PM_TYPE_64:
            build_type_64(value, pm_value_block_wrapper);
            break;
        case PM_TYPE_U64:
            build_type_u64(value, pm_value_block_wrapper);
            break;
        case PM_TYPE_FLOAT:
            build_type_float(value, pm_value_block_wrapper);
            break;
        case PM_TYPE_DOUBLE:
            build_type_double(value, pm_value_block_wrapper);
            break;
        case PM_TYPE_STRING:
            build_type_string(value, pm_value_block_wrapper);
            break;
        /* TODO: Implement other PM_TYPEs */
        default:
            rb_raise(pcp_pmapi_error, "Metric data type %d not supported for PmValueBlock", pm_type);
    }
    return self;
}

static VALUE vbuf(VALUE self) {
    VALUE result;
    pmValueBlock *pm_value_block = get_pm_value_block_from_wrapper(self);
    char *vbuf = pm_value_block->vbuf;
    unsigned int vtype = pm_value_block->vtype;
    unsigned int length_of_value = pm_value_block->vlen - PM_VAL_HDR_SIZE;

    switch(vtype) {
        case PM_TYPE_64:
            result = LL2NUM(*((__int64_t *)vbuf));
            break;
        case PM_TYPE_U64:
            result = ULL2NUM(*((__uint64_t *)vbuf));
            break;
        case PM_TYPE_FLOAT:
            result = DBL2NUM(*((float *)vbuf));
            break;
        case PM_TYPE_DOUBLE:
            result = DBL2NUM(*((double *)vbuf));
            break;
        case PM_TYPE_STRING:
            /* Strings are /supposed/ to have a NULL terminator as per the Programmers Guide:
             * "By convention PM_TYPE_STRING is interpreted as a classic C-style null byte terminated string.".
             * Given this isn't a guarantee, check if it is the case an construct the string accordingly. */
            if(vbuf[length_of_value-1] == 0x00) {
                /* NULL-terminated, only copy the string values */
                result = rb_tainted_str_new(vbuf, length_of_value-1);
            } else {
                /* Not NULL-terminated, copy all the values */
                result = rb_tainted_str_new(vbuf, length_of_value);
            }
            break;
        default:
            rb_raise(pcp_pmapi_error, "Metric data type %d not supported for PmValueBlock", vtype);
    }

    return result;
}

static VALUE vlen(VALUE self) {
    unsigned int vlen = get_pm_value_block_from_wrapper(self)->vlen;

    return UINT2NUM(vlen);
}

static VALUE vtype(VALUE self) {
    unsigned int vtype = get_pm_value_block_from_wrapper(self)->vtype;

    return UINT2NUM(vtype);
}

void init_rb_pmapi_pmvalueblock(VALUE pmapi_class) {
    pcp_pmapi_pmvalueblock_class = rb_define_class_under(pmapi_class, "PmValueBlock", rb_cObject);

    rb_define_method(pcp_pmapi_pmvalueblock_class, "initialize", initialize, 2);
    rb_define_method(pcp_pmapi_pmvalueblock_class, "vbuf", vbuf, 0);
    rb_define_method(pcp_pmapi_pmvalueblock_class, "vlen", vlen, 0);
    rb_define_method(pcp_pmapi_pmvalueblock_class, "vtype", vtype, 0);

    rb_define_alloc_func(pcp_pmapi_pmvalueblock_class, rb_pmapi_pmvalueblock_alloc);
}
