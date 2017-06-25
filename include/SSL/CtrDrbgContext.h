#ifndef VOCABBOT_CTRDRBGCONTEXT_H
#define VOCABBOT_CTRDRBGCONTEXT_H
#include <mbedtls/ctr_drbg.h>
#include <memory>
#include "EntropyContext.h"

class CtrDrbgContext {
public:
    CtrDrbgContext();
    ~CtrDrbgContext();

    int init();
    
    inline mbedtls_ctr_drbg_context *context() {
        return &context_;
    }
private:
    mbedtls_ctr_drbg_context context_;
    EntropyContext entropy_;
};
typedef std::shared_ptr<CtrDrbgContext> CtrDrbgContextPtr;


#endif //VOCABBOT_CTRDRBGCONTEXT_H
