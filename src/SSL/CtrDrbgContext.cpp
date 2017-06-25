#include "SSL/CtrDrbgContext.h"



CtrDrbgContext::CtrDrbgContext()
{
    mbedtls_ctr_drbg_init(&context_);
}



CtrDrbgContext::~CtrDrbgContext()
{
    mbedtls_ctr_drbg_free(&context_);
}



int CtrDrbgContext::init()
{
    return mbedtls_ctr_drbg_seed(&context_, mbedtls_entropy_func, entropy_.context(),
                                 reinterpret_cast<const unsigned char *>("Altenius"), 8);
}
