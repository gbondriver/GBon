#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "typedef.h"
#include "IBonDriver3.h"
#include "cbon.h"

#define PERR(format, ...) ::fprintf(stderr, "%s: %d: %s: ", __FILE__, __LINE__, __PRETTY_FUNCTION__);::fprintf(stderr, format, ##__VA_ARGS__);

CBon *cbon_load_module(char *driverFilename) {
    CBon *cbon = (CBon *)::malloc(sizeof(CBon));
    ::memset(cbon, 0, sizeof(CBon));
    
    cbon->hModule = ::dlopen(driverFilename, RTLD_LAZY);
    if (!cbon->hModule) {
        PERR("dlopen error: %s\n", ::dlerror());
        free(cbon);
        return NULL;
    }
    return cbon;
}

int cbon_create_driver(CBon *cbon) {
    dlerror(); //clear error
    char *err;
    IBonDriver *(*f)() = (IBonDriver *(*)())::dlsym(cbon->hModule,
                                                    "CreateBonDriver");
    IBonDriver *pIBon, *pIBon2, *pIBon3;
    pIBon = pIBon2 = pIBon3 = NULL;
    if ((err = ::dlerror()) == NULL) {
        pIBon = f();
        if (pIBon) {
            pIBon2 = dynamic_cast<IBonDriver2 *>(pIBon);
            if (pIBon2) {
                pIBon3 = dynamic_cast<IBonDriver3 *>(pIBon2);
            }
        } else {
            PERR("pIBon is NULL 2: %s\n", err);
            return -2;
        }
    } else {
        PERR("dlsym error 3: %s\n", err);
        ::dlclose(cbon->hModule);
        return -3;
    }
    cbon->pIBon = pIBon;
    cbon->pIBon2 = pIBon2;
    cbon->pIBon3 = pIBon3;
    return 0;
}

int cbon_open_tuner(CBon *cbon) {
    if (cbon->pIBon == NULL) {
        PERR("pIBon is NULL\n");
        return 0;
    }
    IBonDriver *bon = static_cast<IBonDriver *>(cbon->pIBon);
    return bon->OpenTuner();
}

int cbon_set_channel(CBon *cbon, unsigned char space, unsigned char ch) {
    if (cbon->pIBon2 == NULL) {
        PERR("pIBon2 is NULL\n");
        return 0;
    }
    IBonDriver2 *bon = static_cast<IBonDriver2 *>(cbon->pIBon2);
    return bon->SetChannel(space, ch);
    //return bon->SetChannel(ch);
}

int cbon_get_ts_stream(CBon *cbon, unsigned char **dest, unsigned int *size,
                       unsigned int *remain) {
    if (cbon->pIBon == NULL) {
        PERR("pIBon is NULL\n");
        return 0;
    }
    IBonDriver *bon = static_cast<IBonDriver *>(cbon->pIBon);
    return bon->GetTsStream(dest, size, remain);
}
