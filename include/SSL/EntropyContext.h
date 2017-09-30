#ifndef VOCABBOT_ENTROPYCONTEXT_H
#define VOCABBOT_ENTROPYCONTEXT_H

#include <mbedtls/entropy.h>

class EntropyContext
{
public:
    EntropyContext();

    ~EntropyContext();


    mbedtls_entropy_context *context()
    {
        return &context_;
    }


private:
    mbedtls_entropy_context context_;
};


#endif // VOCABBOT_ENTROPYCONTEXT_H
