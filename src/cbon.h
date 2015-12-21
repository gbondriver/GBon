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
int cbon_open_tuner(CBon *cbon);
int cbon_set_channel(CBon *cbon, unsigned char ch);
int cbon_set_channel2(CBon *cbon, unsigned char space, unsigned char ch);
int cbon_get_ts_stream(CBon *cbon, unsigned char **dest,
                        unsigned int *size, unsigned int *remain);
void cbon_close_tuner(CBon *cbon);
void cbon_release(CBon *cbon);


#ifdef __cplusplus
}
#endif
