#ifdef __cplusplus
extern "C" {
#endif

typedef struct _CBon {
    void *hModule;
    void *pIBon;
    void *pIBon2;
    void *pIBon3;
}CBon;

    CBon *cbon_load_module(char *driverFilename);
    void cbon_close_module(CBon *cbon);

    int cbon_create_driver(CBon *cbon);

    //IBonDriver.h
    int cbon_open_tuner(CBon *cbon);
    int cbon_set_channel(CBon *cbon, unsigned char ch);
    float cbon_get_signal_level(CBon *cbon);
    int cbon_get_ts_stream(CBon *cbon, unsigned char **dest,
                        unsigned int *size, unsigned int *remain);
    unsigned char cbon_wait_ts_stream(CBon *cbon, unsigned char timeout);
    unsigned char cbon_get_ready_count(CBon *cbon);
    void cbon_purge_ts_stream(CBon *cbon);
    void cbon_release(CBon *cbon);

    //IBonDriver2.h
    const unsigned short* cbon_get_tuner_name(CBon *cbon);
    int cbon_is_tuner_opening(CBon *cbon);
    const unsigned short* cbon_enum_tuning_space(CBon *cbon,
                                                 unsigned char space);
    const unsigned short* cbon_enum_channel_name(CBon *cbon,
                                                 unsigned char space,
                                                 unsigned char ch);
    int cbon_set_channel2(CBon *cbon, unsigned char space, unsigned char ch);
    unsigned char cbon_get_cur_space(CBon *cbon);
    unsigned char cbon_get_cur_channel(CBon *cbon);
    void cbon_release2(CBon *cbon);

    //IBonDriver3.h
    unsigned char cbon_get_total_device_num(CBon *cbon);
    unsigned char cbon_get_active_device_num(CBon *cbon);
    int cbon_set_lnb_power(CBon *cbon, int enable);
    void cbon_release3(CBon *cbon);

    void cbon_close_tuner(CBon *cbon);
#ifdef __cplusplus
}
#endif
