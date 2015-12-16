#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

EXTERNC typedef struct _CBon {
    void *hModule;
    void *pIBon;
    void *pIBon2;
    void *pIBon3;
}CBon;

EXTERNC CBon *cbon_load_module(char *driverFilename);
EXTERNC void cbon_close_module(CBon *cbon);

EXTERNC int cbon_create_driver(CBon *cbon);
EXTERNC int cbon_open_tuner(CBon *cbon);
EXTERNC int cbon_set_channel(CBon *cbon, unsigned char ch);
EXTERNC int cbon_set_channel2(CBon *cbon, unsigned char space, unsigned char ch);
EXTERNC int cbon_get_ts_stream(CBon *cbon, unsigned char **dest,
                               unsigned int *size, unsigned int *remain);
EXTERNC void cbon_close_tuner(CBon *cbon);
EXTERNC void cbon_release(CBon *cbon);


