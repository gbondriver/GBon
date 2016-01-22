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

void cbon_close_module(CBon *cbon) {
    if (cbon == NULL) {
        PERR("cbon is NULl\n");
        return;
    }
    if (cbon->hModule == NULL) {
        PERR("cbon->hModule is NULL\n");
        return;
    }
    ::dlclose(cbon->hModule);
        
}

int cbon_create_driver(CBon *cbon) {
    if (cbon == NULL) {
        PERR("cbon is NULl\n");
        return -1;
    }
    dlerror(); //clear error
    char *err;
    IBonDriver *(*f)() = (IBonDriver *(*)())::dlsym(cbon->hModule,
                                                    "CreateBonDriver");
    IBonDriver *pIBon;
    IBonDriver2 *pIBon2;
    IBonDriver3 *pIBon3;
    pIBon = pIBon2 = pIBon3 = NULL;
    if ((err = ::dlerror()) == NULL) {
        pIBon = f();
        if (pIBon) {
            pIBon2 = dynamic_cast<IBonDriver2 *>(pIBon);
            if (pIBon2) {
                pIBon3 = dynamic_cast<IBonDriver3 *>(pIBon);
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
    //PERR("%p, %p, %p\n", pIBon, pIBon2, pIBon3);
    return 0;
}

int cbon_open_tuner(CBon *cbon) {
    if (cbon == NULL) {
        PERR("cbon is NULl\n");
        return 0;
    }
    if (cbon->pIBon == NULL) {
        PERR("pIBon is NULL\n");
        return 0;
    }
    IBonDriver *bon = static_cast<IBonDriver *>(cbon->pIBon);
    return bon->OpenTuner();
}

int cbon_set_channel(CBon *cbon, unsigned char ch) {
    if (cbon == NULL) {
        PERR("cbon is NULl\n");
        return 0;
    }
    if (cbon->pIBon == NULL) {
        PERR("pIBon is NULL\n");
        return 0;
    }
    IBonDriver *bon = static_cast<IBonDriver *>(cbon->pIBon);
    return bon->SetChannel(ch);
}

float cbon_get_signal_level(CBon *cbon) {
   if (cbon == NULL) {
        PERR("cbon is NULl\n");
        return -1.0;
    }
    if (cbon->pIBon == NULL) {
        PERR("pIBon is NULL\n");
        return -2.0;
    }
    IBonDriver *bon = static_cast<IBonDriver *>(cbon->pIBon);
    return bon->GetSignalLevel();
}

int cbon_get_ts_stream(CBon *cbon, unsigned char **dest, unsigned int *size,
                       unsigned int *remain) {
    if (cbon == NULL) {
        PERR("cbon is NULl\n");
        return 0;
    }
    if (cbon->pIBon == NULL) {
        PERR("pIBon is NULL\n");
        return 0;
    }
    IBonDriver *bon = static_cast<IBonDriver *>(cbon->pIBon);
    return bon->GetTsStream(dest, size, remain);
}

unsigned char cbon_wait_ts_stream(CBon *cbon, unsigned char timeout) {
    if (cbon == NULL) {
        PERR("cbon is NULl\n");
        return 0x80;
    }
    if (cbon->pIBon == NULL) {
        PERR("pIBon is NULL\n");
        return 0x80;
    }
    IBonDriver *bon = static_cast<IBonDriver *>(cbon->pIBon);
    return bon->WaitTsStream(timeout);
}

unsigned char cbon_get_ready_count(CBon *cbon) {
    if (cbon == NULL) {
        PERR("cbon is NULl\n");
        return 0;
    }
    if (cbon->pIBon == NULL) {
        PERR("pIBon is NULL\n");
        return 0;
    }
    IBonDriver *bon = static_cast<IBonDriver *>(cbon->pIBon);
    return bon->GetReadyCount();
}

void cbon_purge_ts_stream(CBon *cbon) {
    if (cbon == NULL) {
        PERR("cbon is NULl\n");
        return;
    }
    if (cbon->pIBon == NULL) {
        PERR("pIBon is NULL\n");
        return;
    }
    IBonDriver *bon = static_cast<IBonDriver *>(cbon->pIBon);
    bon->PurgeTsStream();
}


const unsigned short* cbon_get_tuner_name(CBon *cbon) {
    if (cbon == NULL) {
        PERR("cbon is NULl\n");
        return NULL;
    }
    if (cbon->pIBon2 == NULL) {
        PERR("pIBon2 is NULL\n");
        return NULL;
    }
    IBonDriver2 *bon = static_cast<IBonDriver2 *>(cbon->pIBon2);
    return bon->GetTunerName();
}

int cbon_is_tuner_opening(CBon *cbon) {
    if (cbon == NULL) {
        PERR("cbon is NULl\n");
        return 0;
    }
    if (cbon->pIBon2 == NULL) {
        PERR("pIBon2 is NULL\n");
        return 0;
    }
    IBonDriver2 *bon = static_cast<IBonDriver2 *>(cbon->pIBon2);
    return bon->IsTunerOpening();
}

const unsigned short* cbon_enum_tuning_space(CBon *cbon, unsigned char space) {
    if (cbon == NULL) {
        PERR("cbon is NULl\n");
        return NULL;
    }
    if (cbon->pIBon2 == NULL) {
        PERR("pIBon2 is NULL\n");
        return NULL;
    }
    IBonDriver2 *bon = static_cast<IBonDriver2 *>(cbon->pIBon2);
    return bon->EnumTuningSpace(space);
}

const unsigned short* cbon_enum_channel_name(CBon *cbon, unsigned char space,
                                             unsigned char ch) {
    if (cbon == NULL) {
        PERR("cbon is NULl\n");
        return NULL;
    }
    if (cbon->pIBon2 == NULL) {
        PERR("pIBon2 is NULL\n");
        return NULL;
    }
    IBonDriver2 *bon = static_cast<IBonDriver2 *>(cbon->pIBon2);
    return bon->EnumChannelName(space, ch);
}

int cbon_set_channel2(CBon *cbon, unsigned char space, unsigned char ch) {
    if (cbon == NULL) {
        PERR("cbon is NULl\n");
        return 0;
    }
    if (cbon->pIBon2 == NULL) {
        PERR("pIBon2 is NULL\n");
        return 0;
    }
    IBonDriver2 *bon = static_cast<IBonDriver2 *>(cbon->pIBon2);
    return bon->SetChannel(space, ch);
}

unsigned char cbon_get_cur_space(CBon *cbon) {
    if (cbon == NULL) {
        PERR("cbon is NULl\n");
        return 0;
    }
    if (cbon->pIBon2 == NULL) {
        PERR("pIBon2 is NULL\n");
        return 0;
    }
    IBonDriver2 *bon = static_cast<IBonDriver2 *>(cbon->pIBon2);
    return bon->GetCurSpace();
}

unsigned char cbon_get_cur_channel(CBon *cbon) {
    if (cbon == NULL) {
        PERR("cbon is NULl\n");
        return 0;
    }
    if (cbon->pIBon2 == NULL) {
        PERR("pIBon2 is NULL\n");
        return 0;
    }
    IBonDriver2 *bon = static_cast<IBonDriver2 *>(cbon->pIBon2);
    return bon->GetCurChannel();
}


unsigned char cbon_get_total_device_num(CBon *cbon) {
    if (cbon == NULL) {
        PERR("cbon is NULl\n");
        return 0;
    }
    if (cbon->pIBon3 == NULL) {
        PERR("pIBon3 is NULL\n");
        return 0;
    }
    IBonDriver3 *bon = static_cast<IBonDriver3 *>(cbon->pIBon3);
    return bon->GetTotalDeviceNum();
}

unsigned char cbon_get_active_device_num(CBon *cbon) {
    if (cbon == NULL) {
        PERR("cbon is NULl\n");
        return 0;
    }
    if (cbon->pIBon3 == NULL) {
        PERR("pIBon3 is NULL\n");
        return 0;
    }
    IBonDriver3 *bon = static_cast<IBonDriver3 *>(cbon->pIBon3);
    return bon->GetActiveDeviceNum();
}

int cbon_set_lnb_power(CBon *cbon, int enable) {
    if (cbon == NULL) {
        PERR("cbon is NULl\n");
        return 0;
    }
    if (cbon->pIBon3 == NULL) {
        PERR("pIBon3 is NULL\n");
        return 0;
    }
    IBonDriver3 *bon = static_cast<IBonDriver3 *>(cbon->pIBon3);
    return bon->SetLnbPower(enable);
}

void cbon_release(CBon *cbon) {
    if (cbon == NULL) {
        PERR("cbon is NULl\n");
        return;
    }
    if (cbon->pIBon == NULL) {
        PERR("pIBon is NULL\n");
        return;
    }
    IBonDriver *bon = static_cast<IBonDriver *>(cbon->pIBon);
    bon->Release();

    if (cbon->pIBon2 && (cbon->pIBon != cbon->pIBon2)) {
        IBonDriver2 *bon2 = static_cast<IBonDriver2 *>(cbon->pIBon2);
        bon2->Release();
    }

    if (cbon->pIBon3 &&
        (cbon->pIBon != cbon->pIBon3 && cbon->pIBon2 != cbon->pIBon3)) {
        IBonDriver3 *bon3 = static_cast<IBonDriver3 *>(cbon->pIBon3);
        bon3->Release();
    }
}

void cbon_close_tuner(CBon *cbon) {
    if (cbon == NULL) {
        PERR("cbon is NULl\n");
        return;
    }
    if (cbon->pIBon == NULL) {
        PERR("pIBon is NULL\n");
        return;
    }
    IBonDriver *bon = static_cast<IBonDriver *>(cbon->pIBon);
    bon->CloseTuner();
}

