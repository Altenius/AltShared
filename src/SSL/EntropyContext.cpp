#include "SSL/EntropyContext.h"


EntropyContext::EntropyContext()
{
    mbedtls_entropy_init(&context_);
}


EntropyContext::~EntropyContext()
{
    mbedtls_entropy_free(&context_);
}
