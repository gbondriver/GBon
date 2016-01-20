#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <iconv.h>
#include <time.h>
#include <signal.h>

#include "ts.h"
#include "sdt.h"
#include "eit.h"
#include "nit.h"
#include "tdt.h"
#include "bit.h"
#include "ts_ctl.h"

#include "xmldata.c"

typedef struct {
    SVT_CONTROL *svttop;
    int pid;
    SECcache *sec;
    int sdtflg;
    int numsdt;
    EITCHECK chk;
} EDData;



//SVT_CONTROL	*svttop = NULL;
#define		SECCOUNT	10
char	title[1024];
char	subtitle[1024];
char	Category[1024];
char	*extdesc;
char	ServiceName[1024];

static	EITCHECK	chk;

static FILE *outfile;
static char *canma = "";
static void dumpEitJson(EdEit *edeit)
{
    int i;
    char *eitcanma;
    char *eitextcanma;

    fprintf(outfile, "{");
    //fprintf(outfile,"\"id\":\"%s_%d\",",getBSCSGR(svtcur),svtcur->event_id);
    fprintf(outfile, "\"transport_stream_id\":%d,", edeit->transport_stream_id);
    fprintf(outfile, "\"original_network_id\":%d,", edeit->original_network_id);
    fprintf(outfile, "\"service_id\":%d,", edeit->servid);
    //fprintf(outfile,"\"name\":\"%s\",",svtcur->servicename);
    if (edeit->original_network_id < 0x0010) {
        fprintf(outfile, "\"satelliteinfo\":{");
        fprintf(outfile, "\"TP\":\"%s%d\",",
                getTSID2BSCS(edeit->transport_stream_id),
                getTSID2TP(edeit->transport_stream_id));
        fprintf(outfile, "\"SLOT\":%d},",
                getTSID2SLOT(edeit->transport_stream_id));
    }

    fprintf(outfile, "\"table_id\":\"0x%x\",", edeit->table_id);
    fprintf(outfile, "\"section_number\":\"%d\"", edeit->section_number);
    //fprintf(outfile,"\"channel\":\"%s_%d\",",getBSCSGR(svtcur),svtcur->event_id);
    
    EIT_CONTROL *cur;
    eitcanma = "";
    int num_loop2 = 0;
    if (edeit->eittop == NULL) {
        fprintf(stderr, "eittop is null\n");
        fprintf(outfile, ", \"eits\": [");
    } else {
        fprintf(outfile, ", \"eits\": [{");
        for (cur = edeit->eittop; cur; cur = cur->next) {
            fprintf(stderr, "num loop2 %d\n", num_loop2++);
            fprintf(outfile, "%s", eitcanma);
            fprintf(outfile, "\"id\":\"%d_%d\",", cur->servid, cur->event_id);
            fprintf(outfile,"\"desctags\":\"");
            for (i = 0; i < cur->numdesctag; i++) {
                fprintf(outfile, "%x", cur->desctags[i]);
            }
            fprintf(outfile, "\",");
            fprintf(outfile, "\"event_id\":%d,", cur->event_id);
            memset(title, '\0', sizeof(title));
            if (cur->title) strcpy(title, cur->title);
            strrep(title, "\"", "\\\"");
            fprintf(outfile,"\"title\":\"%s\",", title);
            memset(subtitle, '\0', sizeof(subtitle));
            if (cur->subtitle) strcpy(subtitle, cur->subtitle);
            strrep(subtitle, "\"", "\\\"");
            fprintf(outfile,"\"detail\":\"%s\",", subtitle);

            fprintf(outfile,"\"extdetail\":[");
            eitextcanma = "";
            for(i = 0; i < cur->eitextcnt; i++) {
                if (cur->eitextdesc[i].item_description && cur->eitextdesc[i].item) {
                    strcpy(subtitle, cur->eitextdesc[i].item_description);
                    strrep(subtitle, "\"", "\\\"");
                    fprintf(outfile,"%s{\"item_description\":\"%s\",",
                            eitextcanma, subtitle);
                    memset(subtitle, '\0', sizeof(subtitle));
                    strcpy(subtitle, cur->eitextdesc[i].item);
                    strrep(subtitle, "\"", "\\\"");
                    fprintf(outfile,"\"item\":\"%s\"}", subtitle);
                    eitextcanma = ",";
                }
            }
            fprintf(outfile,"],");

            fprintf(outfile, "\"start\":%d,", cur->start_time);
            fprintf(outfile, "\"end\":%d,", cur->start_time + cur->duration);
            fprintf(outfile,"\"duration\":%d,", cur->duration);
    
            eitextcanma = "";
            fprintf(outfile, "\"category\":[");
            for(i = 0; i < cur->numcontent; i++) {
                fprintf(outfile,"%s{", eitextcanma);
                fprintf(outfile,"\"large\": { \"ja_JP\" : \"%s\", \"en\" : \"%s\"},",
                        getContentStr(cur->content[i],
                                      cur->usernibble[i],
                                      CONTENT_LARGE,CONTENT_LANG_JA),
                        getContentStr(cur->content[i],
                                      cur->usernibble[i],
                                      CONTENT_LARGE,CONTENT_LANG_EN));
                fprintf(outfile,"\"middle\": { \"ja_JP\" : \"%s\", \"en\" : \"%s\"}}",
                        getContentStr(cur->content[i],
                                      cur->usernibble[i],
                                      CONTENT_MIDDLE,CONTENT_LANG_JA),
                        getContentStr(cur->content[i],cur->usernibble[i],
                                      CONTENT_MIDDLE,CONTENT_LANG_EN));
                eitextcanma = ",";
            }
            fprintf(outfile,"],");

            eitextcanma = "";
            fprintf(outfile, "\"attachinfo\":[");
            for(i = 0; i < cur->numattachinfo; i++) {
                fprintf(outfile,"%s\"%s\"", eitextcanma,
                        getAttachInfo(cur->attachinfo[i]));
                eitextcanma = ",";
            }
            fprintf(outfile,"],");

            fprintf(outfile,"\"video\":{");
            fprintf(outfile,"\"resolution\":\"%s\",", getVideoResolution(cur->video));
            fprintf(outfile,"\"aspect\":\"%s\"},", getVideoAspect(cur->video));
            
            eitextcanma = "";
            fprintf(outfile, "\"audio\":[");
            for(i = 0; i < 2; i++) {
                if (cur->audiodesc[i].audiotype > 0) {
                    fprintf(outfile,"%s{\"type\":\"%s\",", eitextcanma,
                            getAudioComponentDescStr(cur->audiodesc[i].audiotype));
                    fprintf(outfile,"\"langcode\":\"%s\",", cur->audiodesc[i].langcode);
                    fprintf(outfile,"\"extdesc\":\"%s\"}",
                            cur->audiodesc[i].audiodesc ?
                            cur->audiodesc[i].audiodesc : "");
                    eitextcanma = ",";
                }
            }
            fprintf(outfile, "],");
            
            fprintf(outfile, "\"freeCA\":%s}", cur->freeCA ? "true" : "false");
            eitcanma = ",{\n";
        }
    }
        fprintf(outfile, "]}");
}

static int parse(EDData *ed, unsigned char *buf, int buf_size,
                SECcache secs[], int sec_size) {
    SVT_CONTROL *svtcur;
    EdEit *edeit;
    int ret;

    memset(&chk,0,sizeof(EITCHECK));
    chk.waitend = time(NULL) + 10;

    while((ed->sec = EDReadTSFromBuffer(buf, buf_size,
                                        secs, sec_size)) != NULL) {
        ed->pid = ed->sec->pid & 0xFF;
        switch (ed->pid) {
        case 0x10:  //NIT
            fprintf(stderr, "NIT\n");
            dumpNIT(ed->sec->buf, ed->svttop);
            break;

        case 0x11: // SDT
            fprintf(stderr, "SDT %d\n", ed->sdtflg);
            if (ed->sdtflg==0) { 
                ed->sdtflg=1;
                dumpSDT(ed->sec->buf, ed->svttop);
                
                ed->numsdt=0;
                svtcur = ed->svttop->next;
                while(svtcur) {
                    if (svtcur->eit == NULL) {
                        svtcur->eit = calloc(1, sizeof(EIT_CONTROL));
                    }
                    svtcur = svtcur->next;
                    ed->numsdt++;
                }
                
            }
            break;
        case 0x12: // EIT
        case 0x26: // EIT(地デジ)
        case 0x27: // EIT(地デジ)
            //fprintf(stderr, "EIT\n");
            /*
            if (ed->sdtflg) {
                old_dumpEIT2(ed->sec->buf, ed->svttop, &chk);
                if (ret == 0) ed->sdtflg = 0;
            }
            */
            edeit = Ed_dump_Eit(ed->sec->buf);
            if (edeit) {
                fprintf(outfile, "%s", canma);
                dumpEitJson(edeit);
                fprintf(outfile, "\n");
                canma = ", ";
            }
            Ed_free_Eit(edeit);
            break;
        case 0x14: // TDT
            fprintf(stderr, "TDT\n");
            dumpTDT(ed->sec->buf, &ed->chk);
            if (ed->chk.starttime > 0  && ed->chk.maxcycle > 0) {
                if (ed->chk.starttime + ed->chk.maxcycle < ed->chk.tdttime) {
#ifdef DEBUG
                    printf("start %s cycle %d\n",strTime(ed->chk.starttime,"%Y/%m/%d %H:%M:%S"),chk.maxcycle);
                    printf("tdt %s\n",strTime(ed->chk.tdttime,"%Y/%m/%d %H:%M:%S"));
#endif
                    return 0;
                }
            }
            break;
        case 0x23: // SDTT
            fprintf(stderr, "SDTT\n");
            //	ret = dumpSDTT(sec->buf);
            break;
        case 0x13: // RST
            printf("RST\n");
            break;
        case 0x24: // BIT
            fprintf(stderr, "BIT\n");
            if (ed->chk.maxcycle == 0 && ed->chk.tdttime > 0) {
                dumpBIT(ed->sec->buf, &ed->chk.maxcycle);
                ed->chk.starttime = ed->chk.tdttime;
                /* XXX 環境変数等でオーバーライド可能にしたい
                 *     周期どおりで基本番組情報は十分だが、拡張のほうは
                 *     なかなか埋まらない
                 */
                ed->chk.maxcycle = ed->chk.maxcycle * 1.6;
            }
            break;
        }
        if (ed->numsdt == 0 && time(NULL) > ed->chk.waitend) return 1;
    }
}


static int GetSDTEITInfo(EDData *ed, FILE *infile, SECcache *secs,int sec_size)
{
    memset(ed, 0, sizeof(EDData));
    ed->svttop = calloc(1, sizeof(SVT_CONTROL));
    ed->chk.waitend = time(NULL) + 10;

#define BUF_SIZE 188 * 1024
    unsigned char buf[BUF_SIZE];
    int nr;

    while((nr = fread(buf, 1, BUF_SIZE, infile)) > 0) {
        parse(ed, buf, nr, secs, sec_size);
    }
    return 0;
}

void dumpJSON(EDData *ed, FILE *outfile)
{
	SVT_CONTROL	*svtcur ;
	EIT_CONTROL	*eitcur ;
	int i;
	char *svtcanma="";
	char *eitcanma;
	char *eitextcanma;

	fprintf(outfile,"[");
	svtcur = ed->svttop->next;
	while(svtcur != NULL) {
		if (!svtcur->haveeitschedule) {
			svtcur = svtcur->next;
			continue;
		}
		fprintf(outfile,"%s{",svtcanma);
                fprintf(outfile,"\"id\":\"%s_%d\",",getBSCSGR(svtcur),svtcur->event_id);
		fprintf(outfile,"\"transport_stream_id\":%d,",svtcur->transport_stream_id);
		fprintf(outfile,"\"original_network_id\":%d,",svtcur->original_network_id);
		fprintf(outfile,"\"service_id\":%d,",svtcur->event_id);
		fprintf(outfile,"\"name\":\"%s\",",svtcur->servicename);
		if (svtcur->original_network_id < 0x0010) {
			fprintf(outfile,"\"satelliteinfo\":{");
			fprintf(outfile,"\"TP\":\"%s%d\",",getTSID2BSCS(svtcur->transport_stream_id),getTSID2TP(svtcur->transport_stream_id));
			fprintf(outfile,"\"SLOT\":%d},",getTSID2SLOT(svtcur->transport_stream_id));
		}
		eitcur = svtcur->eit;
		fprintf(outfile,"\"programs\":[");
		eitcanma="";
		while(eitcur != NULL){
			if(!eitcur->servid){
				eitcur = eitcur->next ;
				continue ;
			}
			fprintf(outfile,"%s{",eitcanma);
                        fprintf(outfile,"\"table_id\":\"0x%x\",", eitcur->table_id);
                        fprintf(outfile,"\"section_number\":\"%d\",", eitcur->section_number);
			fprintf(outfile,"\"channel\":\"%s_%d\",",getBSCSGR(svtcur),svtcur->event_id);
			memset(title, '\0', sizeof(title));
			if (eitcur->title) strcpy(title, eitcur->title);
			strrep(title, "\"", "\\\"");
			fprintf(outfile,"\"title\":\"%s\",",title);
			memset(subtitle, '\0', sizeof(subtitle));
			if (eitcur->subtitle) strcpy(subtitle, eitcur->subtitle);
			strrep(subtitle, "\"", "\\\"");
			fprintf(outfile,"\"detail\":\"%s\",",subtitle);

			fprintf(outfile,"\"extdetail\":[");
			eitextcanma="";
			for(i=0;i<eitcur->eitextcnt;i++) {
				if (eitcur->eitextdesc[i].item_description && eitcur->eitextdesc[i].item) {
					strcpy(subtitle, eitcur->eitextdesc[i].item_description);
					strrep(subtitle, "\"", "\\\"");
					fprintf(outfile,"%s{\"item_description\":\"%s\",",eitextcanma,subtitle);
			        memset(subtitle, '\0', sizeof(subtitle));
					strcpy(subtitle, eitcur->eitextdesc[i].item);
					strrep(subtitle, "\"", "\\\"");
					fprintf(outfile,"\"item\":\"%s\"}",subtitle);
					eitextcanma=",";
				}
			}
			fprintf(outfile,"],");

			fprintf(outfile,"\"start\":%d0000,",eitcur->start_time);
			fprintf(outfile,"\"end\":%d0000,",eitcur->start_time+eitcur->duration);
			fprintf(outfile,"\"duration\":%d,",eitcur->duration);

			eitextcanma="";
			fprintf(outfile,"\"category\":[");
			for(i=0;i<eitcur->numcontent;i++) {
				fprintf(outfile,"%s{",eitextcanma);
				fprintf(outfile,"\"large\": { \"ja_JP\" : \"%s\", \"en\" : \"%s\"},",getContentStr(eitcur->content[i],eitcur->usernibble[i],CONTENT_LARGE,CONTENT_LANG_JA),getContentStr(eitcur->content[i],eitcur->usernibble[i],CONTENT_LARGE,CONTENT_LANG_EN));
				fprintf(outfile,"\"middle\": { \"ja_JP\" : \"%s\", \"en\" : \"%s\"}}",getContentStr(eitcur->content[i],eitcur->usernibble[i],CONTENT_MIDDLE,CONTENT_LANG_JA),getContentStr(eitcur->content[i],eitcur->usernibble[i],CONTENT_MIDDLE,CONTENT_LANG_EN));
				eitextcanma=",";
			}
			fprintf(outfile,"],");

			eitextcanma="";
			fprintf(outfile,"\"attachinfo\":[");
			for(i=0;i<eitcur->numattachinfo;i++) {
				fprintf(outfile,"%s\"%s\"",eitextcanma,getAttachInfo(eitcur->attachinfo[i]));
				eitextcanma=",";
			}
			fprintf(outfile,"],");

			fprintf(outfile,"\"video\":{");
			fprintf(outfile,"\"resolution\":\"%s\",",getVideoResolution(eitcur->video));
			fprintf(outfile,"\"aspect\":\"%s\"},",getVideoAspect(eitcur->video));

			eitextcanma="";
			fprintf(outfile,"\"audio\":[");
			for(i=0;i<2;i++) {
			if (eitcur->audiodesc[i].audiotype > 0) {
					fprintf(outfile,"%s{\"type\":\"%s\",",eitextcanma,getAudioComponentDescStr(eitcur->audiodesc[i].audiotype));
					fprintf(outfile,"\"langcode\":\"%s\",", eitcur->audiodesc[i].langcode);
					fprintf(outfile,"\"extdesc\":\"%s\"}", eitcur->audiodesc[i].audiodesc?eitcur->audiodesc[i].audiodesc:"");
					eitextcanma=",";
				}
			}
			fprintf(outfile,"],");

			fprintf(outfile,"\"freeCA\":%s,",eitcur->freeCA?"true":"false");
			fprintf(outfile,"\"event_id\":%d",eitcur->event_id);
			eitcur = eitcur->next ;
			fprintf(outfile,"}");
			eitcanma=",";
		}
		fprintf(outfile,"]");
		svtcur  = svtcur->next;
		fprintf(outfile,"}");
		svtcanma=",";
	}
	fprintf(outfile,"]");
}


int test_main(char *infilename, char *outfilename)
{
	FILE *infile = stdin;
	outfile = stdout;

	int   inclose = 0;
	int   outclose = 0;
	int	ret;
	SECcache   secs[SECCOUNT];


	/* 興味のあるpidを指定 */
	memset(secs, 0,  sizeof(SECcache) * SECCOUNT);
	secs[0].pid = 0x11; /* SDT */
	secs[1].pid = 0x12; /* EIT */
	secs[2].pid = 0x14; /* TDT */
	secs[3].pid = 0x23; /* SDTT */
	secs[4].pid = 0x28; /* SDTT */
	secs[5].pid = 0x10; /* NIT */
	secs[6].pid = 0x13; /* RST */
	secs[7].pid = 0x24; /* BIT */

        if(strcmp(infilename, "-")) {
            infile = fopen(infilename, "r");
            inclose = 1;
        }
        if(infile == NULL){
            fprintf(stderr, "Can't open file: %s\n", infilename);
            return 1;
	}

        if(strcmp(outfilename, "-")) {
            outfile = fopen(outfilename, "w+");
            outclose = 1;
        }
	//svttop = calloc(1, sizeof(SVT_CONTROL));
        fprintf(outfile, "[\n");
        EDData ed;
	ret = GetSDTEITInfo(&ed, infile, secs, SECCOUNT);
        fprintf(outfile, "]\n");

        //dumpJSON(&ed, outfile);

	if(inclose) fclose(infile);
	if(outclose) fclose(outfile);

	return ret;
}
