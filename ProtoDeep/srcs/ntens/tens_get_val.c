#include "pd_main.h"

float pd_ntens_get_val(pd_ntensor *tensor, ...)
{
    va_list arg;
    va_start(arg, tensor);
    size_t *shape_mult = tensor->shape_m;
    pd_count shape_len = tensor->shape_len;
    float *val = tensor->val;
    for (pd_count i = 0; i < shape_len; ++i)
        val += *shape_mult++ * (size_t)va_arg(arg, unsigned int);
    return *val;
}

float *pd_ntens_get_pval(pd_ntensor *tensor, ...)
{
    va_list arg;
    va_start(arg, tensor);
    size_t *shape_mult = tensor->shape_m;
    pd_count shape_len = tensor->shape_len;
    float *val = tensor->val;
    for (pd_count i = 0; i < shape_len; ++i)
        val += *shape_mult++ * (size_t)va_arg(arg, unsigned int);
    return val;
}