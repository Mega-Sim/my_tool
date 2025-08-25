/** \file
 * \brief Example code for Simple Open EtherCAT master
 *
 * Usage: firm_update ifname1 slave fname
 * ifname is NIC interface, f.e. eth0
 * slave = slave number in EtherCAT order 1..n
 * fname = binary file to store in slave
 * CAUTION! Using the wrong file can result in a bricked slave!
 *
 * This is a slave firmware update test.
 *
 * (c)Arthur Ketels 2011
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include "ethercat.h"

#define VERSION_MAJOR		2
#define VERSION_MINOR		0

#define RESULT_SUCCESS      1
#define RESULT_FAIL			0
// =======================================================================
#define VERSION_DISPLAY_ON      1	
#define FIRM_UPDATE_ON    	1
#define PARAM_UPDATE_ON    	1
#define PARAM_EXTRACTION_ON	1

#define GET_COMMON_ON		1

#define SET_COMMON_ON		1
#define SET_SAFE_OP_ON		1

// ============ Reading define
#define MAX_DATA 1500
#define MAX_COL  6
#define SET_VALUE_SKIP  -1
#define EXPECTED_FIELD_COUNT 6

#define DEF_Name 		0
#define DEF_ParamNo 	1
#define DEF_Index		2
#define DEF_SubIndex	3
#define DEF_DataType	4
#define DEF_SetValue	5

// ============ Axis
#define AXIS_DRIVING		1
#define AXIS_HOIST			2
#define AXIS_SLIDE			3

// ============ Controller Type
#define CONT_CSD7	1
#define CONT_D8		2


// ============ Data type
// 1byte
#define TYPE_USINT		0
#define TYPE_SINT		1

// 2byte
#define TYPE_UINT		2
#define TYPE_INT		3

// 4byte
#define TYPE_UDINT		4
#define TYPE_DINT		5

// ============ 1 line reading def.
struct line_buf_data {
    char str[MAX_COL][1024];
};

// ============ All line store def.
struct parsing_data {
    char               cName[100];
    char               cParamNo[20];
    unsigned short int ui16_Index;
    unsigned char      ui8_iSubIndex;
    unsigned char      ui8_iDataType;
    int                i_SetValue;
};

struct param_data {
    int iCount;
    struct parsing_data Parsed[MAX_DATA];
};

struct param_data Param_Raw;

// =======================================================================

void remove_spaces(char* s) {
    const char* d = s;
    do {
        // while (*d == ' ') {
        while ((*d == ' ') || (*d == '\n') || (*d == '\r')) {
            ++d;
        }
    } while ((*s++ = *d++));
}

void getfield(char* line, struct line_buf_data* d, int end_idx)
{
    int idx = 0;
    char* token = strtok(line, ",");

    do
    {
        //printf("token: %s\n", token);
        strcpy(d->str[idx++], token);
    } while (idx != end_idx && (token = strtok(NULL, ",")));
}

int parameter_load(char* filename)
{
    FILE* stream = fopen(filename, "r");
    char line[1024];
    struct line_buf_data str_split;
    int iLineCount = 0;

    // -----------------------------------------------------------------
    if (stream == NULL)
    {
        printf("Parameter file not found! Check please!\n");
        return RESULT_FAIL;
    }
	
    printf("### Loaded raw file data.... ### \n");

    while (fgets(line, 1024, stream))
    {
        // 개행 문자 제거
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') 
		{
            line[len - 1] = '\0';
        }

        // 공백만 있는 라인 건너뛰기
        int is_empty = 1;
		for (int i = 0; line[i] != '\0'; i++) 
		{
			char ch = line[i];
			if (ch != ' ' && ch != '\t' && ch != '\n' && ch != '\r' && ch != '\v' && ch != '\f') {
				is_empty = 0;
				break;
			}
		}

        if (is_empty) {
            printf("[INFO] 공백 줄 건너뜀\n");
            continue;
        }

        remove_spaces(line);

        char* tmp = strdup(line);
		if (!tmp) {
			perror("strdup 실패");
			continue;
		}

		// 콤마 개수 검사
		int comma_count = 0;
		for (int i = 0; tmp[i] != '\0'; i++) {
			if (tmp[i] == ',') comma_count++;
		}

		if (comma_count + 1 != EXPECTED_FIELD_COUNT) {
			printf("[ERROR] 잘못된 필드 수 (%d개), 예상: %d개 → 건너뜀: %s\n",
				comma_count + 1, EXPECTED_FIELD_COUNT, tmp);
			free(tmp);
			continue;
		}

        getfield(tmp, &str_split, MAX_COL);

        // contents debug print
        for (int i = 0; i < MAX_COL; i++)
        {
        	if (i == 0)
            	printf("[%30s] \t", str_split.str[i]);
			else
				printf("[%7s] \t", str_split.str[i]);
        }
        printf("\n");

        // ############  data convertion ############
        // (1) Name (str)
        strcpy(Param_Raw.Parsed[iLineCount].cName, str_split.str[DEF_Name]);

        // (2) ParamNo (str)
        strcpy(Param_Raw.Parsed[iLineCount].cParamNo, str_split.str[DEF_ParamNo]);

        // (3) Index (Hexa int)
        Param_Raw.Parsed[iLineCount].ui16_Index = (int)strtol(str_split.str[DEF_Index], NULL, 16);

        // (4) SubIndex (int. but if "-" => 0 set)
        if (strcmp("-", str_split.str[DEF_SubIndex]) == 0)
            Param_Raw.Parsed[iLineCount].ui8_iSubIndex = 0;
        else
            Param_Raw.Parsed[iLineCount].ui8_iSubIndex = atoi(str_split.str[DEF_SubIndex]);

        // (5) DataType (int. data byte length (1/2/4) )
        // 1 byte size
        if (strcmp("USINT", str_split.str[DEF_DataType]) == 0)
            Param_Raw.Parsed[iLineCount].ui8_iDataType = TYPE_USINT;   
        else if (strcmp("SINT", str_split.str[DEF_DataType]) == 0)
            Param_Raw.Parsed[iLineCount].ui8_iDataType = TYPE_SINT;  
		// 2 byte size
        else if (strcmp("UINT", str_split.str[DEF_DataType]) == 0)
            Param_Raw.Parsed[iLineCount].ui8_iDataType = TYPE_UINT;   
        else if (strcmp("INT", str_split.str[DEF_DataType]) == 0)
            Param_Raw.Parsed[iLineCount].ui8_iDataType = TYPE_INT;
		// 4 byte size
        else if (strcmp("UDINT", str_split.str[DEF_DataType]) == 0)
            Param_Raw.Parsed[iLineCount].ui8_iDataType = TYPE_UDINT;   
        else if (strcmp("DINT", str_split.str[DEF_DataType]) == 0)
            Param_Raw.Parsed[iLineCount].ui8_iDataType = TYPE_DINT;
		// error
        else
        {
            printf("[ERROR] Unknown DataType → 건너뜀: %s\n", tmp);
            free(tmp);
            continue;   // exit(0) 대신 건너뛰기
        }

        // (6) SetValue (int. but if "-" => -1 set)
        if (strcmp("-", str_split.str[DEF_SetValue]) == 0) // 
            Param_Raw.Parsed[iLineCount].i_SetValue = SET_VALUE_SKIP;
        else
        {
            // hex process
            if (
                   ((str_split.str[DEF_SetValue][0] == '0') && (str_split.str[DEF_SetValue][1] == 'x'))   // find 0x (Lover case - 0x)
                || ((str_split.str[DEF_SetValue][0] == '0') && (str_split.str[DEF_SetValue][1] == 'X'))   // find 0X (Upper case - 0X)
                )
            {
                Param_Raw.Parsed[iLineCount].i_SetValue = (int)strtol(str_split.str[DEF_SetValue], NULL, 16);
            }
            else  // 10.. process
            {
                Param_Raw.Parsed[iLineCount].i_SetValue = atoi(str_split.str[DEF_SetValue]);
            }
        }	
		
        // ############################################################

        iLineCount++;
        free(tmp);
    }

    Param_Raw.iCount = iLineCount;

    // -----------------------------------------------------------------
    printf("\n\n### Parsed data.... ### \n");
    printf("[Name(S)]\t\t [ParamNo(S)]\t\t [Index(H)]\t\t [SubIndex(I)]\t\t [DataLength(I)]\t\t [SetVal(I)]\n");
    printf("----------------------------------------------------------------------------------------------------\n");

    for (int i = 0; i < Param_Raw.iCount; i++)
    {
        printf("[%30s]\t", Param_Raw.Parsed[i].cName);
        printf("[%7s]\t", Param_Raw.Parsed[i].cParamNo);
        printf("[0x%04x]\t", Param_Raw.Parsed[i].ui16_Index);
        printf("[%2d]\t", Param_Raw.Parsed[i].ui8_iSubIndex);
        printf("[%2d]\t", Param_Raw.Parsed[i].ui8_iDataType);
        printf("[%2d]\n", Param_Raw.Parsed[i].i_SetValue);
    }

    // -----------------------------------------------------------------
    printf("\n### Data loading OK.. \n\n");

	return RESULT_SUCCESS;
}

// =======================================================================



#define FWBUFSIZE (8 * 1024 * 1024)

uint8 ob;
uint16 ow;
uint32 data;
char filename[256];
char filebuffer[FWBUFSIZE]; // 8MB buffer
int filesize;
int j;
uint16 argslave;

int input_bin(char *fname, int *length)
{
    FILE *fp;

    int cc = 0, c;

    fp = fopen(fname, "rb");

    if(fp == NULL)
        return 0;
    while (((c = fgetc(fp)) != EOF) && (cc < FWBUFSIZE))
		filebuffer[cc++] = (uint8)c;
    *length = cc;
    fclose(fp);
    
    return 1;
}
// -------------------------------------------

/* 
	return value : 1 - success, 0 - fail
*/
int boottest(char *ifname, uint16 slave, char *filename)
{
    int iResult = RESULT_FAIL;
	
	printf("Starting firmware update example\n");

	/* initialise SOEM, bind socket to ifname */
	if (ec_init(ifname))
	{
		printf("ec_init on %s succeeded.\n",ifname);
		/* find and auto-config slaves */

	    if ( ec_config_init(FALSE) > 0 )
		{
			printf("%d slaves found and configured.\n",ec_slavecount);

			/* wait for all slaves to reach PRE_OP state */
			// shkim. windows - linux diff
			ec_statecheck(0, EC_STATE_PRE_OP,  EC_TIMEOUTSTATE * 4);

			printf("Request init state for slave %d\n", slave);
			ec_slave[slave].state = EC_STATE_INIT;
			ec_writestate(slave);

			/* wait for slave to reach INIT state */
			ec_statecheck(slave, EC_STATE_INIT,  EC_TIMEOUTSTATE * 4);
			printf("Slave %d state to INIT.\n", slave);

			/* read BOOT mailbox data, master -> slave */
			data = ec_readeeprom(slave, ECT_SII_BOOTRXMBX, EC_TIMEOUTEEP);
			ec_slave[slave].SM[0].StartAddr = (uint16)LO_WORD(data);
			ec_slave[slave].SM[0].SMlength = (uint16)HI_WORD(data);
			/* store boot write mailbox address */
			ec_slave[slave].mbx_wo = (uint16)LO_WORD(data);
			/* store boot write mailbox size */
			ec_slave[slave].mbx_l = (uint16)HI_WORD(data);

			/* read BOOT mailbox data, slave -> master */
			data = ec_readeeprom(slave, ECT_SII_BOOTTXMBX, EC_TIMEOUTEEP);
			ec_slave[slave].SM[1].StartAddr = (uint16)LO_WORD(data);
			ec_slave[slave].SM[1].SMlength = (uint16)HI_WORD(data);
			/* store boot read mailbox address */
			ec_slave[slave].mbx_ro = (uint16)LO_WORD(data);
			/* store boot read mailbox size */
			ec_slave[slave].mbx_rl = (uint16)HI_WORD(data);

			printf(" SM0 A:%4.4x L:%4d F:%8.8x\n", ec_slave[slave].SM[0].StartAddr, ec_slave[slave].SM[0].SMlength, (int)ec_slave[slave].SM[0].SMflags);
			printf(" SM1 A:%4.4x L:%4d F:%8.8x\n", ec_slave[slave].SM[1].StartAddr, ec_slave[slave].SM[1].SMlength, (int)ec_slave[slave].SM[1].SMflags);
			/* program SM0 mailbox in for slave */
			ec_FPWR (ec_slave[slave].configadr, ECT_REG_SM0, sizeof(ec_smt), &ec_slave[slave].SM[0], EC_TIMEOUTRET);
			/* program SM1 mailbox out for slave */
			ec_FPWR (ec_slave[slave].configadr, ECT_REG_SM1, sizeof(ec_smt), &ec_slave[slave].SM[1], EC_TIMEOUTRET);

			printf("Request BOOT state for slave %d\n", slave);
			ec_slave[slave].state = EC_STATE_BOOT;
			ec_writestate(slave);

			/* wait for slave to reach BOOT state */
			if (ec_statecheck(slave, EC_STATE_BOOT,  EC_TIMEOUTSTATE * 10) == EC_STATE_BOOT)
			{
				printf("Slave %d state to BOOT.\n", slave);

				if (input_bin(filename, &filesize))
				{
					printf("File read OK, %d bytes.\n",filesize);
					printf("FoE write....");
					j = ec_FOEwrite(slave, filename, 0x01020304, filesize , &filebuffer, EC_TIMEOUTSTATE);
					printf("result %d.\n",j);

					if(j > 0)
						iResult = RESULT_SUCCESS;	// success
					else
						iResult = RESULT_FAIL;	// fail
					
					printf("Request init state for slave %d\n", slave);
					ec_slave[slave].state = EC_STATE_INIT;
					ec_writestate(slave);
				}
				else
				    printf("File not read OK.\n");
			}

		}
		else
		{
			printf("No slaves found!\n");
		}
		printf("End firmware update example, close socket\n");
		/* stop SOEM, close socket */
		ec_close();
	}
	else
	{
		printf("No socket connection on %s\nExcecute as root\n",ifname);
	}

	return iResult;
}


int readVer(char *ifname, uint16 slave)
{
    int iResult = RESULT_FAIL; // 0 = fail, 1 = success
    
	// ---------- save log
	FILE *fp;
	fp = fopen("firm_version.txt", "w");
		
	if(fp == NULL)
	{
		printf("Log file open failed... \n");
		return iResult;
	}
	else
		printf("Firmware version file open... \n");
	// -------------------------	   

	char fwVer[40];
	int fwLength = sizeof(fwVer) - 1;

	/* initialise SOEM, bind socket to ifname */
	if (ec_init(ifname))
	{
		printf("ec_init on %s succeeded.\n",ifname);
		/* find and auto-config slaves */

		if ( ec_config_init(FALSE) > 0 )
		{
			printf("Request PRE OP state for slave %d\n", slave);
			ec_slave[slave].state = EC_STATE_PRE_OP;
			ec_writestate(slave);

			/* wait for slave to reach INIT state */
			ec_statecheck(slave, EC_STATE_PRE_OP,  EC_TIMEOUTSTATE * 4);
			printf("Slave %d state to PREOP.\n", slave);

			if(ec_statecheck(slave, EC_STATE_PRE_OP, EC_TIMEOUTSTATE * 4) == EC_STATE_PRE_OP){
				ec_SDOread(slave, 0x100A, 0x00, FALSE, &fwLength, fwVer, EC_TIMEOUTRXM);
				printf("F/W Version : %s\n",fwVer);

				if(fp != NULL)
					fprintf(fp, "%s\n", fwVer);

				iResult = RESULT_SUCCESS;
			}
		}
		else
		{
			printf("ec_config_init() func fail!! \n");
		}
	}
	else
	{
		printf("ec_init on %s fail!! \n",ifname);
	}	

	if(fp != NULL)
		fclose(fp);

	return iResult;
}

// iDataSize = 1, 2, 4
int readCommon(char *ifname, uint16 slave, uint16 index, uint8 subindex, int iDataType)
{
    int iResult = RESULT_FAIL; // 0 = fail, 1 = success
    int fwLength = 0;	// reading data size (1,2,4 byte)
	
	// ---------- save log
	   FILE *fp;
	   fp = fopen("common_read.txt", "w");
			
	   if(fp == NULL)
	   {
			printf("Common Read file open failed... \n");
			return iResult;
	   }
	   else
			printf("Common Read file open... \n");
	// -------------------------	   
	uint8  		 u_8_int = 0;
	int8   		 s_8_int = 0;
	uint16 		 u_16_int = 0;
    int16  		 s_16_int = 0;
	unsigned int u_32_int = 0;
	int			 s_32_int = 0;


	/* initialise SOEM, bind socket to ifname */
	if (ec_init(ifname))
	{
		printf("ec_init on %s succeeded.\n",ifname);
		/* find and auto-config slaves */

		if ( ec_config_init(FALSE) > 0 )
		{
			printf("Request PRE OP state for slave %d\n", slave);
			ec_slave[slave].state = EC_STATE_PRE_OP;
			ec_writestate(slave);

			/* wait for slave to reach INIT state */
			ec_statecheck(slave, EC_STATE_PRE_OP,  EC_TIMEOUTSTATE * 4);
			printf("Slave %d state to PREOP.\n", slave);

			if(ec_statecheck(slave, EC_STATE_PRE_OP, EC_TIMEOUTSTATE * 4) == EC_STATE_PRE_OP)
			{
				switch(iDataType)
				{
				// 1byte
					case TYPE_USINT:
					{			
						fwLength = sizeof(u_8_int);
						ec_SDOread(slave, index, subindex, FALSE, &fwLength, &u_8_int, EC_TIMEOUTRXM);
						printf("Common read(TYPE_USINT) = %d  0x%08x \n", u_8_int, u_8_int);
						fprintf(fp, "%d \n", u_8_int);
						iResult = RESULT_SUCCESS;
					}
					break;
				
					case TYPE_SINT:
					{		
						fwLength = sizeof(s_8_int);
						ec_SDOread(slave, index, subindex, FALSE, &fwLength, &s_8_int, EC_TIMEOUTRXM);
						printf("Common read(TYPE_SINT) = %d  0x%08x \n", s_8_int, s_8_int);
						fprintf(fp, "%d \n", s_8_int);
						iResult = RESULT_SUCCESS;
					}
					break;
					
				// 2byte
					case TYPE_UINT:
					{			
						fwLength = sizeof(u_16_int);
						ec_SDOread(slave, index, subindex, FALSE, &fwLength, &u_16_int, EC_TIMEOUTRXM);
						printf("Common read(TYPE_UINT) = %d  0x%08x \n", u_16_int, u_16_int);
						fprintf(fp, "%d \n", u_16_int);
						iResult = RESULT_SUCCESS;
					}
					break;
				
					case TYPE_INT:
					{			
						fwLength = sizeof(s_16_int);
						ec_SDOread(slave, index, subindex, FALSE, &fwLength, &s_16_int, EC_TIMEOUTRXM);
						printf("Common read(TYPE_INT) = %d  0x%08x \n", s_16_int, s_16_int);
						fprintf(fp, "%d \n", s_16_int);
						iResult = RESULT_SUCCESS;
					}
					break;
					
				// 4byte
					case TYPE_UDINT:
					{		
						fwLength = sizeof(u_32_int);
						ec_SDOread(slave, index, subindex, FALSE, &fwLength, &u_32_int, EC_TIMEOUTRXM);
						printf("Common read(TYPE_UDINT) = %d  0x%08x \n", u_32_int, u_32_int);
						fprintf(fp, "%d \n", u_32_int);
						iResult = RESULT_SUCCESS;
					}
					break;
				
					case TYPE_DINT:
					{	
						fwLength = sizeof(s_32_int);
						ec_SDOread(slave, index, subindex, FALSE, &fwLength, &s_32_int, EC_TIMEOUTRXM);
						printf("Common read(TYPE_DINT) = %d  0x%08x \n", s_32_int, s_32_int);
						fprintf(fp, "%d \n", s_32_int);
						iResult = RESULT_SUCCESS;
					}
					break;
				}
			}
			else
			{
				printf("ec_config_init() func fail!! \n");
			}
		}
	}
	else
	{
		printf("ec_init on %s fail!! \n",ifname);
	}	

	if(fp != NULL)
		fclose(fp);

	return iResult;
}


int writeCommon(char *ifname, uint16 slave, uint16 index, uint8 subindex, int iDataType, int iWriteVal)
{
	// -------------------------	   
	uint8  		 u_8_int = 0;
	int8   		 s_8_int = 0;
	uint16 		 u_16_int = 0;
    int16  		 s_16_int = 0;
	unsigned int u_32_int = 0;
	int			 s_32_int = 0;

	int fwLength = 0;	// reading data size (1,2,4 byte)
	int iResult = RESULT_FAIL; // 0 = fail, 1 = success
	
	/* initialise SOEM, bind socket to ifname */
	if (ec_init(ifname))
	{
		printf("ec_init on %s succeeded.\n",ifname);
		/* find and auto-config slaves */

		if ( ec_config_init(FALSE) > 0 )
		{
			printf("Request PRE OP state for slave %d\n", slave);
			ec_slave[slave].state = EC_STATE_PRE_OP;
			ec_writestate(slave);

			/* wait for slave to reach INIT state */
			ec_statecheck(slave, EC_STATE_PRE_OP,  EC_TIMEOUTSTATE * 4);
			printf("Slave %d state to PREOP.\n", slave);

			if(ec_statecheck(slave, EC_STATE_PRE_OP, EC_TIMEOUTSTATE * 4) == EC_STATE_PRE_OP)
			{
				switch(iDataType)
				{
				// 1byte
					case TYPE_USINT:
					{			
						fwLength = sizeof(u_8_int);
						u_8_int = (uint8)iWriteVal;
						ec_SDOwrite(slave, index, subindex, FALSE, fwLength, &u_8_int, EC_TIMEOUTRXM);						
						printf("Common write (TYPE_USINT) = %d  0x%08x \n", u_8_int, u_8_int);
						iResult = RESULT_SUCCESS;
					}
					break;
				
					case TYPE_SINT:
					{		
						fwLength = sizeof(s_8_int);
						s_8_int = (int8)iWriteVal;
						ec_SDOwrite(slave, index, subindex, FALSE, fwLength, &s_8_int, EC_TIMEOUTRXM);
						printf("Common write (TYPE_SINT) = %d  0x%08x \n", s_8_int, s_8_int);						
						iResult = RESULT_SUCCESS;
					}
					break;
					
				// 2byte
					case TYPE_UINT:
					{			
						fwLength = sizeof(u_16_int);
						u_16_int = (uint16)iWriteVal;
						ec_SDOwrite(slave, index, subindex, FALSE, fwLength, &u_16_int, EC_TIMEOUTRXM);
						printf("Common write (TYPE_UINT) = %d  0x%08x \n", u_16_int, u_16_int);						
						iResult = RESULT_SUCCESS;
					}
					break;
				
					case TYPE_INT:
					{			
						fwLength = sizeof(s_16_int);
						s_16_int = (int16)iWriteVal;
						ec_SDOwrite(slave, index, subindex, FALSE, fwLength, &s_16_int, EC_TIMEOUTRXM);
						printf("Common write (TYPE_INT) = %d	0x%08x \n", s_16_int, s_16_int);						
						iResult = RESULT_SUCCESS;
					}
					break;
					
				// 4byte
					case TYPE_UDINT:
					{		
						fwLength = sizeof(u_32_int);
						u_32_int = (unsigned int)iWriteVal;
						ec_SDOwrite(slave, index, subindex, FALSE, fwLength, &u_32_int, EC_TIMEOUTRXM);
						printf("Common write (TYPE_UDINT) = %d  0x%08x \n", u_32_int, u_32_int);						
						iResult = RESULT_SUCCESS;
					}
					break;
				
					case TYPE_DINT:
					{	
						fwLength = sizeof(s_32_int);
						s_32_int = (int)iWriteVal;
						ec_SDOwrite(slave, index, subindex, FALSE, fwLength, &s_32_int, EC_TIMEOUTRXM);
						printf("Common write (TYPE_DINT) = %d  0x%08x \n", s_32_int, s_32_int);						
						iResult = RESULT_SUCCESS;
					}
					break;
				}				
			}		 // 	
		}
	}

	return iResult;
}


char IOmap[4096];

int writeSafeOp(char *ifname, uint16 slave, uint16 index, uint8 subindex, int iDataType, int iWriteVal)
{
	// -------------------------	   
	uint8  		 u_8_int = 0;
	int8   		 s_8_int = 0;
	uint16 		 u_16_int = 0;
    int16  		 s_16_int = 0;
	unsigned int u_32_int = 0;
	int			 s_32_int = 0;

	int fwLength = 0;	// reading data size (1,2,4 byte)
	int iResult = RESULT_FAIL; // 0 = fail, 1 = success

    printf(">>>>>>>>>>>>>>> Start Safe OP Test.... \n");
    printf("[STATE] [all] = %04X  [Drive] = %04X  [Hoist] = %04X  [Slide] = %04X  \n",
				ec_slave[0].state, ec_slave[1].state, ec_slave[2].state, ec_slave[3].state);
		
	/* initialise SOEM, bind socket to ifname */
	if (ec_init(ifname))
	{
	   printf("ec_init on %s succeeded.\n",ifname);
       printf("[STATE] [all] = %04X  [Drive] = %04X  [Hoist] = %04X  [Slide] = %04X  \n",
				ec_slave[0].state, ec_slave[1].state, ec_slave[2].state, ec_slave[3].state);	   
	   /* find and auto-config slaves */

		if ( ec_config_init(FALSE) > 0 )
		{
			printf("%d slaves found and configured.\n",ec_slavecount);
			printf("[STATE] [all] = %04X  [Drive] = %04X  [Hoist] = %04X  [Slide] = %04X  \n",
						ec_slave[0].state, ec_slave[1].state, ec_slave[2].state, ec_slave[3].state);

			ec_config_map(&IOmap);
			ec_configdc();

			printf("Slaves mapped, state to SAFE_OP.\n");
			/* wait for all slaves to reach SAFE_OP state */
			ec_statecheck(0, EC_STATE_SAFE_OP,	EC_TIMEOUTSTATE * 4);
			printf("[STATE] [all] = %04X  [Drive] = %04X  [Hoist] = %04X  [Slide] = %04X  \n",
						ec_slave[0].state, ec_slave[1].state, ec_slave[2].state, ec_slave[3].state);


		    // Write data
			switch(iDataType)
			{
			// 1byte
				case TYPE_USINT:
				{			
					fwLength = sizeof(u_8_int);
					u_8_int = (uint8)iWriteVal;
					ec_SDOwrite(slave, index, subindex, FALSE, fwLength, &u_8_int, EC_TIMEOUTRXM);						
					printf("Common write (TYPE_USINT) = %d	0x%08x \n", u_8_int, u_8_int);
					iResult = RESULT_SUCCESS;
				}
				break;
			
				case TYPE_SINT:
				{		
					fwLength = sizeof(s_8_int);
					s_8_int = (int8)iWriteVal;
					ec_SDOwrite(slave, index, subindex, FALSE, fwLength, &s_8_int, EC_TIMEOUTRXM);
					printf("Common write (TYPE_SINT) = %d  0x%08x \n", s_8_int, s_8_int);						
					iResult = RESULT_SUCCESS;
				}
				break;
				
			// 2byte
				case TYPE_UINT:
				{			
					fwLength = sizeof(u_16_int);
					u_16_int = (uint16)iWriteVal;
					ec_SDOwrite(slave, index, subindex, FALSE, fwLength, &u_16_int, EC_TIMEOUTRXM);
					printf("Common write (TYPE_UINT) = %d  0x%08x \n", u_16_int, u_16_int); 					
					iResult = RESULT_SUCCESS;
				}
				break;
			
				case TYPE_INT:
				{			
					fwLength = sizeof(s_16_int);
					s_16_int = (int16)iWriteVal;
					ec_SDOwrite(slave, index, subindex, FALSE, fwLength, &s_16_int, EC_TIMEOUTRXM);
					printf("Common write (TYPE_INT) = %d	0x%08x \n", s_16_int, s_16_int);						
					iResult = RESULT_SUCCESS;
				}
				break;
				
			// 4byte
				case TYPE_UDINT:
				{		
					fwLength = sizeof(u_32_int);
					u_32_int = (unsigned int)iWriteVal;
					ec_SDOwrite(slave, index, subindex, FALSE, fwLength, &u_32_int, EC_TIMEOUTRXM);
					printf("Common write (TYPE_UDINT) = %d	0x%08x \n", u_32_int, u_32_int);						
					iResult = RESULT_SUCCESS;
				}
				break;
			
				case TYPE_DINT:
				{	
					fwLength = sizeof(s_32_int);
					s_32_int = (int)iWriteVal;
					ec_SDOwrite(slave, index, subindex, FALSE, fwLength, &s_32_int, EC_TIMEOUTRXM);
					printf("Common write (TYPE_DINT) = %d  0x%08x \n", s_32_int, s_32_int); 					
					iResult = RESULT_SUCCESS;
				}
				break;
			} // switch 	

			printf("\n (1) Request init state for all slaves\n");
			ec_slave[0].state = EC_STATE_INIT;
       		printf("[STATE] [all] = %04X  [Drive] = %04X  [Hoist] = %04X  [Slide] = %04X  \n",
						ec_slave[0].state, ec_slave[1].state, ec_slave[2].state, ec_slave[3].state);	
			
			/* request INIT state for all slaves */
			printf("\n (2) request INIT state for all slaves \n");
			ec_writestate(0);

       		printf("[STATE] [all] = %04X  [Drive] = %04X  [Hoist] = %04X  [Slide] = %04X  \n",
						ec_slave[0].state, ec_slave[1].state, ec_slave[2].state, ec_slave[3].state);	

		}
		else
		{
			printf("No slaves found!\n");
		}
		printf("End simple test, close socket\n");
		/* stop SOEM, close socket */
		ec_close();


	}
	else
	{
	   printf("No socket connection on %s\nExecute as root\n",ifname);
	}

	   

#if 0	
	/* initialise SOEM, bind socket to ifname */
	if (ec_init(ifname))
	{
		printf("ec_init on %s succeeded.\n",ifname);
		/* find and auto-config slaves */

		if ( ec_config_init(FALSE) > 0 )
		{
			printf("Request PRE OP state for slave %d\n", slave);
			ec_slave[slave].state = EC_STATE_PRE_OP;
			ec_writestate(slave);

			/* wait for slave to reach INIT state */
			ec_statecheck(slave, EC_STATE_PRE_OP,  EC_TIMEOUTSTATE * 4);
			printf("Slave %d state to PREOP.\n", slave);

			if(ec_statecheck(slave, EC_STATE_PRE_OP, EC_TIMEOUTSTATE * 4) == EC_STATE_PRE_OP)
			{
				// -------------------------------------------------
				//			Change to Safe OP mode  
				// -------------------------------------------------
				printf("Request SAFE OP state for slave %d\n", slave);
				ec_slave[slave].state = EC_STATE_SAFE_OP;
				ec_writestate(slave);

				/* wait for slave to reach INIT state */
				ec_statecheck(slave, EC_STATE_SAFE_OP,  EC_TIMEOUTSTATE * 4);
				printf("Slave %d state to Safe OP.\n", slave);

				if(ec_statecheck(slave, EC_STATE_PRE_OP, EC_TIMEOUTSTATE * 4) == EC_STATE_SAFE_OP)
				{
				// -------------------------------------------------
			
					switch(iDataType)
					{
					// 1byte
						case TYPE_USINT:
						{			
							fwLength = sizeof(u_8_int);
							u_8_int = (uint8)iWriteVal;
							ec_SDOwrite(slave, index, subindex, FALSE, fwLength, &u_8_int, EC_TIMEOUTRXM);						
							printf("Common write (TYPE_USINT) = %d  0x%08x \n", u_8_int, u_8_int);
							iResult = RESULT_SUCCESS;
						}
						break;
					
						case TYPE_SINT:
						{		
							fwLength = sizeof(s_8_int);
							s_8_int = (int8)iWriteVal;
							ec_SDOwrite(slave, index, subindex, FALSE, fwLength, &s_8_int, EC_TIMEOUTRXM);
							printf("Common write (TYPE_SINT) = %d  0x%08x \n", s_8_int, s_8_int);						
							iResult = RESULT_SUCCESS;
						}
						break;
						
					// 2byte
						case TYPE_UINT:
						{			
							fwLength = sizeof(u_16_int);
							u_16_int = (uint16)iWriteVal;
							ec_SDOwrite(slave, index, subindex, FALSE, fwLength, &u_16_int, EC_TIMEOUTRXM);
							printf("Common write (TYPE_UINT) = %d  0x%08x \n", u_16_int, u_16_int);						
							iResult = RESULT_SUCCESS;
						}
						break;
					
						case TYPE_INT:
						{			
							fwLength = sizeof(s_16_int);
							s_16_int = (int16)iWriteVal;
							ec_SDOwrite(slave, index, subindex, FALSE, fwLength, &s_16_int, EC_TIMEOUTRXM);
							printf("Common write (TYPE_INT) = %d	0x%08x \n", s_16_int, s_16_int);						
							iResult = RESULT_SUCCESS;
						}
						break;
						
					// 4byte
						case TYPE_UDINT:
						{		
							fwLength = sizeof(u_32_int);
							u_32_int = (unsigned int)iWriteVal;
							ec_SDOwrite(slave, index, subindex, FALSE, fwLength, &u_32_int, EC_TIMEOUTRXM);
							printf("Common write (TYPE_UDINT) = %d  0x%08x \n", u_32_int, u_32_int);						
							iResult = RESULT_SUCCESS;
						}
						break;
					
						case TYPE_DINT:
						{	
							fwLength = sizeof(s_32_int);
							s_32_int = (int)iWriteVal;
							ec_SDOwrite(slave, index, subindex, FALSE, fwLength, &s_32_int, EC_TIMEOUTRXM);
							printf("Common write (TYPE_DINT) = %d  0x%08x \n", s_32_int, s_32_int);						
							iResult = RESULT_SUCCESS;
						}
						break;
					} // switch		
				} // EC_STATE_SAFE_OP
				else
				{
					printf("Change to SAFE OP : Time out!!. check please!! \n ");
				}
			} // EC_STATE_PRE_OP 	
		}
	}
#endif

	return iResult;
}

int param_extract(char *ifname, uint16 slave)
{
        uint8            u_8_int = 0;
        int8             s_8_int = 0;
        uint16           u_16_int = 0;
        int16            s_16_int = 0;
        unsigned int     u_32_int = 0;
        int              s_32_int = 0;

        int iResult = RESULT_FAIL;

        FILE *fp;
        fp = fopen("current_param_extract.txt", "w");

        if(fp == NULL)
        {
                printf("Parameter extract file open failed... \n");
                return iResult;
        }

        if (Param_Raw.iCount == 0)
        {
			if (parameter_load("param_driving_cable.txt") == RESULT_FAIL)
			{
					printf("Parameter list load failed... \n");
					fclose(fp);
					return iResult;
			}
        }

        fprintf(fp, "--------------------------------------------------------------\n");
        fprintf(fp, "[Name(S)]\t\t [ParamNo(S)]\t\t [Index(H)]\t [SubIndex(I)]\t [DataLength(I)]\t [CurVal(I)] \n");
        fprintf(fp, "--------------------------------------------------------------\n");
        
		// Clear SetValue since extraction only reads current values
        for(int i = 0; i < Param_Raw.iCount; i++)
        {
                Param_Raw.Parsed[i].i_SetValue = -1;
        }

        int fwLength = 0;

#if PARAM_EXTRACTION_ON
        if (ec_init(ifname))
        {
			printf("ec_init on %s succeeded.\n",ifname);

			if ( ec_config_init(FALSE) > 0 )
			{
				printf("Request PRE OP state for slave %d\n", slave);
				ec_slave[slave].state = EC_STATE_PRE_OP;
				ec_writestate(slave);

				ec_statecheck(slave, EC_STATE_PRE_OP,  EC_TIMEOUTSTATE * 4);
				printf("Slave %d state to PREOP.\n", slave);

				if(ec_statecheck(slave, EC_STATE_PRE_OP, EC_TIMEOUTSTATE * 4) == EC_STATE_PRE_OP)
				{
					for(int i = 0; i < Param_Raw.iCount; i++)
					{
						fprintf(fp, "[%30s]\t[%7s]\t[0x%04x]\t[%2d]\t[%2d]\t",
								Param_Raw.Parsed[i].cName,
								Param_Raw.Parsed[i].cParamNo,
								Param_Raw.Parsed[i].ui16_Index,
								Param_Raw.Parsed[i].ui8_iSubIndex,
								Param_Raw.Parsed[i].ui8_iDataType);

						switch(Param_Raw.Parsed[i].ui8_iDataType)
						{
						case TYPE_USINT:
							fwLength = sizeof(u_8_int);
							ec_SDOread(slave, Param_Raw.Parsed[i].ui16_Index, Param_Raw.Parsed[i].ui8_iSubIndex, FALSE, &fwLength, &u_8_int, EC_TIMEOUTRXM);
							fprintf(fp, "%d\n", u_8_int);
							break;

						case TYPE_SINT:
							fwLength = sizeof(s_8_int);
							ec_SDOread(slave, Param_Raw.Parsed[i].ui16_Index, Param_Raw.Parsed[i].ui8_iSubIndex, FALSE, &fwLength, &s_8_int, EC_TIMEOUTRXM);
							fprintf(fp, "%d\n", s_8_int);
							break;

						case TYPE_UINT:
							fwLength = sizeof(u_16_int);
							ec_SDOread(slave, Param_Raw.Parsed[i].ui16_Index, Param_Raw.Parsed[i].ui8_iSubIndex, FALSE, &fwLength, &u_16_int, EC_TIMEOUTRXM);
							fprintf(fp, "%d\n", u_16_int);
							break;

						case TYPE_INT:
							fwLength = sizeof(s_16_int);
							ec_SDOread(slave, Param_Raw.Parsed[i].ui16_Index, Param_Raw.Parsed[i].ui8_iSubIndex, FALSE, &fwLength, &s_16_int, EC_TIMEOUTRXM);
							fprintf(fp, "%d\n", s_16_int);
							break;

						case TYPE_UDINT:
							fwLength = sizeof(u_32_int);
							ec_SDOread(slave, Param_Raw.Parsed[i].ui16_Index, Param_Raw.Parsed[i].ui8_iSubIndex, FALSE, &fwLength, &u_32_int, EC_TIMEOUTRXM);
							fprintf(fp, "%d\n", u_32_int);
							break;

						case TYPE_DINT:
							fwLength = sizeof(s_32_int);
							ec_SDOread(slave, Param_Raw.Parsed[i].ui16_Index, Param_Raw.Parsed[i].ui8_iSubIndex, FALSE, &fwLength, &s_32_int, EC_TIMEOUTRXM);
							fprintf(fp, "%d\n", s_32_int);
							break;

						default:
								fprintf(fp, "0\n");
						}
					}

					iResult = RESULT_SUCCESS;
				}
			}
        }
#endif

	if(fp != NULL)
	{
		fprintf(fp, "--------------------------------------------------------------\n");
		fclose(fp);
	}

	/* 기존 파라미터 정보는 재사용되지 않으므로 메모리를 정리한다. */
	memset(&Param_Raw, 0, sizeof(Param_Raw));

	return iResult;
}

/*
int param_download_to_motion(uint16 slave)
{
	uint8  		 u_8_int = 0;
	int8   		 s_8_int = 0;
	uint16 		 u_16_int = 0;
    int16  		 s_16_int = 0;
	unsigned int u_32_int = 0;
	int			 s_32_int = 0;

	int          bErrCheck = RESULT_SUCCESS;
	int          iErrCount = 0;

	uint8 		u8val[4];

	u8val[0] = 's';
	u8val[1] = 'a';
	u8val[2] = 'v';
	u8val[3] = 'e';

	// int iResult = RESULT_SUCCESS;

	// ---------- save log
	FILE *fp;

	if(slave == AXIS_DRIVING)
		fp = fopen("drive_param_down_log.txt", "w");
	else if(slave == AXIS_HOIST)
		fp = fopen("hoist_param_down_log.txt", "w");	
	else if(slave == AXIS_SLIDE	)
		fp = fopen("slide_param_down_log.txt", "w");
	else
		fp = fopen("unknown_param_down_log.txt", "w");
    
    fprintf(fp, "----------------------------------------------------------------------------------------------------------------- \n");
    fprintf(fp, "[Name(S)]\t\t [ParamNo(S)]\t\t [Index(H)]\t\t [SubIndex(I)]\t\t [DataLength(I)]\t\t [SetVal(I)]\t\t [PreVal(I)]\t\t [GetVal(I)]\t\t [Check] \n");
    fprintf(fp, "----------------------------------------------------------------------------------------------------------------- \n");	

	int fwLength = 0;
	// -----------------------------------------------------------------	

	for(int i = 0; i < Param_Raw.iCount; i++)
	{
		// -------------------- save log ----------------------
        fprintf(fp, "[%30s]\t",   Param_Raw.Parsed[i].cName);
        fprintf(fp, "[%7s]\t", 	  Param_Raw.Parsed[i].cParamNo);
        fprintf(fp, "[0x%04x]\t", Param_Raw.Parsed[i].ui16_Index);
        fprintf(fp, "[%2d]\t",    Param_Raw.Parsed[i].ui8_iSubIndex);
        fprintf(fp, "[%2d]\t",    Param_Raw.Parsed[i].ui8_iDataType);
        fprintf(fp, "[%2d]\t",    Param_Raw.Parsed[i].i_SetValue);
		// ----------------------------------------------------
	
		switch(Param_Raw.Parsed[i].ui8_iDataType)
		{
		// 1byte
			case TYPE_USINT:
			{
				fwLength = sizeof(u_8_int);
				uint8 u_8_int_pre = 0;
				ec_SDOread(slave, Param_Raw.Parsed[i].ui16_Index, Param_Raw.Parsed[i].ui8_iSubIndex,
							FALSE, &fwLength, &u_8_int_pre, EC_TIMEOUTRXM);				
				fprintf(fp, "[%2d]\t",    u_8_int_pre);
	
				u_8_int = (uint8)Param_Raw.Parsed[i].i_SetValue;
				ec_SDOwrite(slave, Param_Raw.Parsed[i].ui16_Index, Param_Raw.Parsed[i].ui8_iSubIndex, 
					        FALSE, sizeof(u_8_int), &u_8_int, EC_TIMEOUTRXM);
				// verify
				//fwLength = sizeof(u_8_int);
				uint8 u_8_int_check = 0;
				ec_SDOread(slave, Param_Raw.Parsed[i].ui16_Index, Param_Raw.Parsed[i].ui8_iSubIndex,
							FALSE, &fwLength, &u_8_int_check, EC_TIMEOUTRXM);
				fprintf(fp, "[%2d]\t",    u_8_int_check);

				if(u_8_int != u_8_int_check)
				{
					fprintf(fp, "Warning \n");
					// iResult = RESULT_FAIL;
 				}
				else
				{
					if(u_8_int_pre != u_8_int)
						fprintf(fp, "Modified \n");
					else	
						fprintf(fp, "OK \n");
				}
			}
			break;

			case TYPE_SINT:
			{
        fwLength = sizeof(s_8_int);
        int8 s_8_int_pre = 0;
        ec_SDOread(slave, Param_Raw.Parsed[i].ui16_Index, Param_Raw.Parsed[i].ui8_iSubIndex,
							FALSE, &fwLength, &s_8_int_pre, EC_TIMEOUTRXM);
				fprintf(fp, "[%2d]\t",    s_8_int_pre);
        
				s_8_int = (int8)Param_Raw.Parsed[i].i_SetValue;
				ec_SDOwrite(slave, Param_Raw.Parsed[i].ui16_Index, Param_Raw.Parsed[i].ui8_iSubIndex, 
					        FALSE, sizeof(s_8_int), &s_8_int, EC_TIMEOUTRXM);

				// verify
				
				int8 s_8_int_check = 0;
				ec_SDOread(slave, Param_Raw.Parsed[i].ui16_Index, Param_Raw.Parsed[i].ui8_iSubIndex,
							FALSE, &fwLength, &s_8_int_check, EC_TIMEOUTRXM);
				fprintf(fp, "[%2d]\t",    s_8_int_check);

				if(s_8_int != s_8_int_check)
				{
					fprintf(fp, "Warning \n");
					// iResult = RESULT_FAIL;
 				}
				else
				{
					if(s_8_int_pre != s_8_int)
						fprintf(fp, "Modified \n");
					else	
						fprintf(fp, "OK \n");
				}				
			}
			break;
			
		// 2byte
			case TYPE_UINT:
			{
        fwLength = sizeof(u_16_int);
				uint16 u_16_int_pre = 0;
				ec_SDOread(slave, Param_Raw.Parsed[i].ui16_Index, Param_Raw.Parsed[i].ui8_iSubIndex,
							FALSE, &fwLength, &u_16_int_pre, EC_TIMEOUTRXM);
				fprintf(fp, "[%2d]\t",    u_16_int_pre);
        
				u_16_int = (uint16)Param_Raw.Parsed[i].i_SetValue;
				ec_SDOwrite(slave, Param_Raw.Parsed[i].ui16_Index, Param_Raw.Parsed[i].ui8_iSubIndex, 
					        FALSE, sizeof(u_16_int), &u_16_int, EC_TIMEOUTRXM);

				// verify
				//fwLength = sizeof(u_16_int);
				uint16 u_16_int_check = 0;
				ec_SDOread(slave, Param_Raw.Parsed[i].ui16_Index, Param_Raw.Parsed[i].ui8_iSubIndex,
							FALSE, &fwLength, &u_16_int_check, EC_TIMEOUTRXM);
				fprintf(fp, "[%2d]\t",    u_16_int_check);

				if(u_16_int != u_16_int_check)
				{
					fprintf(fp, "Warning \n");
					// iResult = RESULT_FAIL;
				}
				else
				{
					if(u_16_int_pre != u_16_int)
						fprintf(fp, "Modified \n");
					else	
						fprintf(fp, "OK \n");
				}	
			}
			break;

			case TYPE_INT:
			{
        fwLength = sizeof(s_16_int);
				int16 s_16_int_pre = 0;
				ec_SDOread(slave, Param_Raw.Parsed[i].ui16_Index, Param_Raw.Parsed[i].ui8_iSubIndex,
							FALSE, &fwLength, &s_16_int_pre, EC_TIMEOUTRXM);
				fprintf(fp, "[%2d]\t",    s_16_int_pre);
      
				s_16_int = (int16)Param_Raw.Parsed[i].i_SetValue;
				ec_SDOwrite(slave, Param_Raw.Parsed[i].ui16_Index, Param_Raw.Parsed[i].ui8_iSubIndex, 
					        FALSE, sizeof(s_16_int), &s_16_int, EC_TIMEOUTRXM);

				// verify
				//fwLength = sizeof(s_16_int);
				int16 s_16_int_check = 0;
				ec_SDOread(slave, Param_Raw.Parsed[i].ui16_Index, Param_Raw.Parsed[i].ui8_iSubIndex,
							FALSE, &fwLength, &s_16_int_check, EC_TIMEOUTRXM);
				fprintf(fp, "[%2d]\t",    s_16_int_check);

				if(s_16_int != s_16_int_check)
				{
					fprintf(fp, "Warning \n");
					// iResult = RESULT_FAIL;
				}
				else
				{
					if(s_16_int_pre != s_16_int)
						fprintf(fp, "Modified \n");
					else	
						fprintf(fp, "OK \n");
				}				
			}
			break;
			
		// 4byte
			case TYPE_UDINT:
			{
        fwLength = sizeof(u_32_int);
				unsigned int u_32_int_pre = 0;
				ec_SDOread(slave, Param_Raw.Parsed[i].ui16_Index, Param_Raw.Parsed[i].ui8_iSubIndex,
							FALSE, &fwLength, &u_32_int_pre, EC_TIMEOUTRXM);
				fprintf(fp, "[%2d]\t",    u_32_int_pre);      
      
				u_32_int = (unsigned int)Param_Raw.Parsed[i].i_SetValue;
				ec_SDOwrite(slave, Param_Raw.Parsed[i].ui16_Index, Param_Raw.Parsed[i].ui8_iSubIndex, 
					        FALSE, sizeof(u_32_int), &u_32_int, EC_TIMEOUTRXM);

				// verify
				//fwLength = sizeof(u_32_int);
				unsigned int u_32_int_check = 0;
				ec_SDOread(slave, Param_Raw.Parsed[i].ui16_Index, Param_Raw.Parsed[i].ui8_iSubIndex,
							FALSE, &fwLength, &u_32_int_check, EC_TIMEOUTRXM);
				fprintf(fp, "[%2d]\t",    u_32_int_check);

				if(u_32_int != u_32_int_check)
				{
					fprintf(fp, "Warning \n");
					// iResult = RESULT_FAIL;
				}
				else
				{
					if(u_32_int_pre != u_32_int)
						fprintf(fp, "Modified \n");
					else	
						fprintf(fp, "OK \n");
				}				
			}
			break;

			case TYPE_DINT:
			{
        fwLength = sizeof(s_32_int);
				int s_32_int_pre = 0;
				ec_SDOread(slave, Param_Raw.Parsed[i].ui16_Index, Param_Raw.Parsed[i].ui8_iSubIndex,
							FALSE, &fwLength, &s_32_int_pre, EC_TIMEOUTRXM);
				fprintf(fp, "[%2d]\t",    s_32_int_pre);
      
				s_32_int = (int)Param_Raw.Parsed[i].i_SetValue;
				ec_SDOwrite(slave, Param_Raw.Parsed[i].ui16_Index, Param_Raw.Parsed[i].ui8_iSubIndex, 
					        FALSE, sizeof(s_32_int), &s_32_int, EC_TIMEOUTRXM);

				// verify
				//fwLength = sizeof(s_32_int);
				int s_32_int_check = 0;
				ec_SDOread(slave, Param_Raw.Parsed[i].ui16_Index, Param_Raw.Parsed[i].ui8_iSubIndex,
							FALSE, &fwLength, &s_32_int_check, EC_TIMEOUTRXM);
				fprintf(fp, "[%2d]\t",    s_32_int_check);

				if(s_32_int != s_32_int_check)
				{
					fprintf(fp, "Warning \n");
					// iResult = RESULT_FAIL;
				}
				else
				{
					if(s_32_int_pre != s_32_int)
						fprintf(fp, "Modified \n");
					else	
						fprintf(fp, "OK \n");
				}				
			}
			break;

		// error
			default:
			{
				printf("Unknown data type! check please!. Data Index = %d  Data Type = %d \n", i, Param_Raw.Parsed[i].ui8_iDataType);
				bErrCheck = RESULT_FAIL;
				iErrCount++;
				// iResult = RESULT_FAIL;
				
			    fprintf(fp, "\n\t ------> ERROR! Count = %d -------- \n", iErrCount);

			}
		}
	}

	if(bErrCheck != RESULT_FAIL)
	{
		// save parameter value
		ec_SDOwrite(slave, 0x1010, 0x04, FALSE, sizeof(u8val), u8val, EC_TIMEOUTRXM);
		printf("\n Parameter file download OK! \n");
	}
	else
		printf("\n Parameter file downloaded. but Error exist. Count = %d \n", iErrCount);


	if(fp != NULL)
	{
		fprintf(fp, "-----------------------------------------------------------------------------------------------------------------\n");
		fclose(fp);	
	}


	return bErrCheck;
}
*/

int param_download_to_motion(uint16 slave)
{
    // 임시 변수 선언
    uint8   u_8_int    = 0;
    int8    s_8_int    = 0;
    uint16  u_16_int   = 0;
    int16   s_16_int   = 0;
    uint32  u_32_int   = 0;
    int32   s_32_int   = 0;

    int     bErrCheck  = RESULT_SUCCESS;
    int     iErrCount  = 0;
    int     fwLength   = 0;
    uint8   u8val[4]   = { 's','a','v','e' };

    int count = Param_Raw.iCount;
    // 모든 파라미터의 Pre/Set/Check 값을 저장할 배열
    int32_t *preVals   = malloc(count * sizeof(int32_t));
    int32_t *setVals   = malloc(count * sizeof(int32_t));
    int32_t *checkVals = malloc(count * sizeof(int32_t));
    if (!preVals || !setVals || !checkVals) {
        printf("Memory allocation failed\n");
        return RESULT_FAIL;
    }

    // 로그 파일 오픈 및 헤더 출력
    FILE *fp;
    if      (slave == AXIS_DRIVING) fp = fopen("drive_param_down_log.txt",    "w");
    else if (slave == AXIS_HOIST)   fp = fopen("hoist_param_down_log.txt",   "w");
    else if (slave == AXIS_SLIDE)   fp = fopen("slide_param_down_log.txt",   "w");
    else                             fp = fopen("unknown_param_down_log.txt","w");

    fprintf(fp, "-----------------------------------------------------------------------------------------------------------------\n");
    fprintf(fp, "[Name(S)]\t [ParamNo(S)]\t [Index(H)]\t [SubIndex(I)]\t [DataType(I)]\t [SetVal(I)]\t [PreVal(I)]\t [GetVal(I)]\t [Check]\n");
    fprintf(fp, "-----------------------------------------------------------------------------------------------------------------\n");

    // 1) 모든 파라미터의 현재값(Pre)을 한꺼번에 읽기
    for (int i = 0; i < count; i++) {
        switch (Param_Raw.Parsed[i].ui8_iDataType) {
            case TYPE_USINT:
                fwLength = sizeof(u_8_int);
                ec_SDOread(slave, Param_Raw.Parsed[i].ui16_Index, Param_Raw.Parsed[i].ui8_iSubIndex,
                           FALSE, &fwLength, &u_8_int, EC_TIMEOUTRXM);
                preVals[i] = u_8_int;
                break;
            case TYPE_SINT:
                fwLength = sizeof(s_8_int);
                ec_SDOread(slave, Param_Raw.Parsed[i].ui16_Index, Param_Raw.Parsed[i].ui8_iSubIndex,
                           FALSE, &fwLength, &s_8_int, EC_TIMEOUTRXM);
                preVals[i] = s_8_int;
                break;
            case TYPE_UINT:
                fwLength = sizeof(u_16_int);
                ec_SDOread(slave, Param_Raw.Parsed[i].ui16_Index, Param_Raw.Parsed[i].ui8_iSubIndex,
                           FALSE, &fwLength, &u_16_int, EC_TIMEOUTRXM);
                preVals[i] = u_16_int;
                break;
            case TYPE_INT:
                fwLength = sizeof(s_16_int);
                ec_SDOread(slave, Param_Raw.Parsed[i].ui16_Index, Param_Raw.Parsed[i].ui8_iSubIndex,
                           FALSE, &fwLength, &s_16_int, EC_TIMEOUTRXM);
                preVals[i] = s_16_int;
                break;
            case TYPE_UDINT:
                fwLength = sizeof(u_32_int);
                ec_SDOread(slave, Param_Raw.Parsed[i].ui16_Index, Param_Raw.Parsed[i].ui8_iSubIndex,
                           FALSE, &fwLength, &u_32_int, EC_TIMEOUTRXM);
                preVals[i] = u_32_int;
                break;
            case TYPE_DINT:
                fwLength = sizeof(s_32_int);
                ec_SDOread(slave, Param_Raw.Parsed[i].ui16_Index, Param_Raw.Parsed[i].ui8_iSubIndex,
                           FALSE, &fwLength, &s_32_int, EC_TIMEOUTRXM);
                preVals[i] = s_32_int;
                break;
            default:
                // 알 수 없는 데이터 타입
                printf("Unknown data type! Index=%d, Type=%d\n", i, Param_Raw.Parsed[i].ui8_iDataType);
                bErrCheck = RESULT_FAIL;
                iErrCount++;
                preVals[i] = 0;
                break;
        }
    }

    // 2) 모든 파라미터의 설정값(SetValue)을 한꺼번에 쓰기 (setVals 저장)
    for (int i = 0; i < count; i++) {
        int32_t val = Param_Raw.Parsed[i].i_SetValue;
        setVals[i] = val;
        switch (Param_Raw.Parsed[i].ui8_iDataType) {
            case TYPE_USINT:
                u_8_int = (uint8)val;
                ec_SDOwrite(slave, Param_Raw.Parsed[i].ui16_Index, Param_Raw.Parsed[i].ui8_iSubIndex,
                            FALSE, sizeof(u_8_int), &u_8_int, EC_TIMEOUTRXM);
                break;
            case TYPE_SINT:
                s_8_int = (int8)val;
                ec_SDOwrite(slave, Param_Raw.Parsed[i].ui16_Index, Param_Raw.Parsed[i].ui8_iSubIndex,
                            FALSE, sizeof(s_8_int), &s_8_int, EC_TIMEOUTRXM);
                break;
            case TYPE_UINT:
                u_16_int = (uint16)val;
                ec_SDOwrite(slave, Param_Raw.Parsed[i].ui16_Index, Param_Raw.Parsed[i].ui8_iSubIndex,
                            FALSE, sizeof(u_16_int), &u_16_int, EC_TIMEOUTRXM);
                break;
            case TYPE_INT:
                s_16_int = (int16)val;
                ec_SDOwrite(slave, Param_Raw.Parsed[i].ui16_Index, Param_Raw.Parsed[i].ui8_iSubIndex,
                            FALSE, sizeof(s_16_int), &s_16_int, EC_TIMEOUTRXM);
                break;
            case TYPE_UDINT:
                u_32_int = (uint32)val;
                ec_SDOwrite(slave, Param_Raw.Parsed[i].ui16_Index, Param_Raw.Parsed[i].ui8_iSubIndex,
                            FALSE, sizeof(u_32_int), &u_32_int, EC_TIMEOUTRXM);
                break;
            case TYPE_DINT:
                s_32_int = (int32)val;
                ec_SDOwrite(slave, Param_Raw.Parsed[i].ui16_Index, Param_Raw.Parsed[i].ui8_iSubIndex,
                            FALSE, sizeof(s_32_int), &s_32_int, EC_TIMEOUTRXM);
                break;
            default:
                // 이미 에러 처리됨
                break;
        }
    }

    // 3) 모든 파라미터를 다시 읽어서(Check) checkVals 저장
    for (int i = 0; i < count; i++) {
        switch (Param_Raw.Parsed[i].ui8_iDataType) {
            case TYPE_USINT:
                fwLength = sizeof(u_8_int);
                ec_SDOread(slave, Param_Raw.Parsed[i].ui16_Index, Param_Raw.Parsed[i].ui8_iSubIndex,
                           FALSE, &fwLength, &u_8_int, EC_TIMEOUTRXM);
                checkVals[i] = u_8_int;
                break;
            case TYPE_SINT:
                fwLength = sizeof(s_8_int);
                ec_SDOread(slave, Param_Raw.Parsed[i].ui16_Index, Param_Raw.Parsed[i].ui8_iSubIndex,
                           FALSE, &fwLength, &s_8_int, EC_TIMEOUTRXM);
                checkVals[i] = s_8_int;
                break;
            case TYPE_UINT:
                fwLength = sizeof(u_16_int);
                ec_SDOread(slave, Param_Raw.Parsed[i].ui16_Index, Param_Raw.Parsed[i].ui8_iSubIndex,
                           FALSE, &fwLength, &u_16_int, EC_TIMEOUTRXM);
                checkVals[i] = u_16_int;
                break;
            case TYPE_INT:
                fwLength = sizeof(s_16_int);
                ec_SDOread(slave, Param_Raw.Parsed[i].ui16_Index, Param_Raw.Parsed[i].ui8_iSubIndex,
                           FALSE, &fwLength, &s_16_int, EC_TIMEOUTRXM);
                checkVals[i] = s_16_int;
                break;
            case TYPE_UDINT:
                fwLength = sizeof(u_32_int);
                ec_SDOread(slave, Param_Raw.Parsed[i].ui16_Index, Param_Raw.Parsed[i].ui8_iSubIndex,
                           FALSE, &fwLength, &u_32_int, EC_TIMEOUTRXM);
                checkVals[i] = u_32_int;
                break;
            case TYPE_DINT:
                fwLength = sizeof(s_32_int);
                ec_SDOread(slave, Param_Raw.Parsed[i].ui16_Index, Param_Raw.Parsed[i].ui8_iSubIndex,
                           FALSE, &fwLength, &s_32_int, EC_TIMEOUTRXM);
                checkVals[i] = s_32_int;
                break;
            default:
                checkVals[i] = 0;
                break;
        }
    }

    // 4) 로그 출력 (setVals 사용)
    for (int i = 0; i < count; i++) {
        fprintf(fp, "[%30s]\t",   Param_Raw.Parsed[i].cName);
        fprintf(fp, "[%7s]\t",    Param_Raw.Parsed[i].cParamNo);
        fprintf(fp, "[0x%04x]\t", Param_Raw.Parsed[i].ui16_Index);
        fprintf(fp, "[%2d]\t",    Param_Raw.Parsed[i].ui8_iSubIndex);
        fprintf(fp, "[%2d]\t",    Param_Raw.Parsed[i].ui8_iDataType);
        fprintf(fp, "[%2d]\t",    setVals[i]);
        fprintf(fp, "[%2d]\t",    preVals[i]);
        fprintf(fp, "[%2d]\t",    checkVals[i]);

        if (preVals[i] != checkVals[i]) {
            fprintf(fp, "Warning\n");
        }
        else {
            if (preVals[i] != setVals[i])
                fprintf(fp, "Modified\n");
            else
                fprintf(fp, "OK\n");
        }
    }

    // 5) 최종 파라미터 다운로드 커맨드 및 콘솔 메시지
    if (bErrCheck != RESULT_FAIL) {
        ec_SDOwrite(slave, 0x1010, 0x04, FALSE, sizeof(u8val), u8val, EC_TIMEOUTRXM);
        printf("\nParameter file download OK!\n");
    }
    else {
        printf("\nParameter file downloaded, but errors exist. Count = %d\n", iErrCount);
    }

    // 로그 파일 닫기 및 메모리 해제
    fprintf(fp, "-----------------------------------------------------------------------------------------------------------------\n");
    fclose(fp);
    free(preVals);
    free(setVals);
    free(checkVals);

    return bErrCheck;
}

int param_update(char *ifname, uint16 slave)
{
	// uint8 u8val;
	// u8val = 0;  // value set

	int iResult = RESULT_FAIL;

	/* initialise SOEM, bind socket to ifname */
	if (ec_init(ifname))
	{
		printf("ec_init on %s succeeded.\n",ifname);
		/* find and auto-config slaves */

		if ( ec_config_init(FALSE) > 0 )
		{
			printf("Request PRE OP state for slave %d\n", slave);
			ec_slave[slave].state = EC_STATE_PRE_OP;
			ec_writestate(slave);

			/* wait for slave to reach INIT state */
			ec_statecheck(slave, EC_STATE_PRE_OP,  EC_TIMEOUTSTATE * 4);
			printf("Slave %d state to PREOP.\n", slave);

			if(ec_statecheck(slave, EC_STATE_PRE_OP, EC_TIMEOUTSTATE * 4) == EC_STATE_PRE_OP) {
				
				// ec_SDOwrite(slave, 0x3010, 0x00, FALSE, sizeof(u8val), &u8val, EC_TIMEOUTRXM);
				// ec_SDOwrite(slave, 0x2005, 0x01, FALSE, sizeof(u8val), &u8val, EC_TIMEOUTRXM);
				// printf("Slave %d Reset.\n", slave);
				iResult = param_download_to_motion(slave);
			}
		}
	}

	return iResult;
}

int resettest(char *ifname, uint16 slave, int iController_Type)
{
	uint8 u8val;
	u8val = 1;
	int iResult = RESULT_FAIL; // 0 = fail, 1 = success

	if(iController_Type == CONT_CSD7)
		printf("CSD7(Single) Controller, Slave No = %d \n", slave);
	else if(iController_Type == CONT_D8)
		printf("D8(Dual) Controller, Slave No = %d \n", slave);
	else
	{
		printf("Unknown Controller, Slave No = %d \n exit...!!! \n", slave);
		exit(-1);
	}

	/* initialise SOEM, bind socket to ifname */
	if (ec_init(ifname))
	{
		printf("ec_init on %s succeeded.\n",ifname);
		/* find and auto-config slaves */

		if ( ec_config_init(FALSE) > 0 )
		{
			printf("Request PRE OP state for slave %d\n", slave);
			ec_slave[slave].state = EC_STATE_PRE_OP;
			ec_writestate(slave);

			/* wait for slave to reach INIT state */
			ec_statecheck(slave, EC_STATE_PRE_OP,  EC_TIMEOUTSTATE * 4);
			printf("Slave %d state to PREOP.\n", slave);

			if(ec_statecheck(slave, EC_STATE_PRE_OP, EC_TIMEOUTSTATE * 4) == EC_STATE_PRE_OP)
			{
				if(iController_Type == CONT_D8)  // Dual moving motion axis
				{
					ec_SDOwrite(slave, 0x2c10, 0x00, FALSE, sizeof(u8val), &u8val, EC_TIMEOUTRXM);
					iResult = RESULT_SUCCESS; // reset success
				}
				else			// Single moving motion
				{
					ec_SDOwrite(slave, 0x3010, 0x00, FALSE, sizeof(u8val), &u8val, EC_TIMEOUTRXM);
					iResult = RESULT_SUCCESS; // reset success
				}
				printf("Slave %d Reset.\n", slave);
			}
		}
	}

	return iResult;
}

void dev_debug(char *ifname, uint16 slave)
{
	uint8 u8val;	
	// u8val = 1;	/* initialise SOEM, bind socket to ifname */	
	if (ec_init(ifname))	
	{		
		printf("ec_init on %s succeeded.\n",ifname);
		/* find and auto-config slaves */		

		if ( ec_config_init(FALSE) > 0 )		
		{			
			printf("Request PRE OP state for slave %d\n", slave);			
			ec_slave[slave].state = EC_STATE_PRE_OP;			
			ec_writestate(slave);			

			/* wait for slave to reach INIT state */			
			ec_statecheck(slave, EC_STATE_PRE_OP,  EC_TIMEOUTSTATE * 4);			
			printf("Slave %d state to PREOP.\n", slave);			

			if(ec_statecheck(slave, EC_STATE_PRE_OP, EC_TIMEOUTSTATE * 4) == EC_STATE_PRE_OP) 
			{		
			    // 1 value write 
			    u8val = 1;
				ec_SDOwrite(slave, 0x2002, 0x04, FALSE, sizeof(u8val), &u8val, EC_TIMEOUTRXM);
				printf("AC line loss check (0x2002, 0x04)  = %d \n", u8val);

				// save run
				uint8 		u8val[4];
				u8val[0] = 's';	u8val[1] = 'a';	u8val[2] = 'v';	u8val[3] = 'e';
				ec_SDOwrite(slave, 0x1010, 0x04, FALSE, sizeof(u8val), u8val, EC_TIMEOUTRXM);

				printf("Test write ok.. \n\n");
		    }		
	    }	
    }
}


void print_usage()
{
    printf("\n\t ########## Pre Setting Step ########## \n");

	printf("\n\t (Step 1) Ecmaster close \n");	
	printf(  "\t       >> ps -ef | grep 'ecmaster' (enter) \n");
    printf(  "\t       >> sudo kill -9 ProcessNo   (enter) \n");

	printf("\n\t (Step 2) Virtual communication port open \n");	
	printf(  "\t       >> echo {} | sudo -S sh -c \"echo 0000:02:00.0 | tee /sys/bus/pci/drivers/igb/bind\" (enter) \n");
	printf(  "\t       >> echo {} | sudo -S sh -c \"echo 0000:03:00.0 | tee /sys/bus/pci/drivers/igb/bind\" (enter) \n");

	printf("\n\t (Step 3) Virtual port check \n");	
	printf(  "\t       >> sudo ifconfig (enter) \n");
	printf(  "\t       (3-1) exist check --> enp2s0 port\n");
	printf(  "\t       (3-2) exist check --> eno1 port\n");

	printf("\n\t (Step 4) If not exist enp2s0, eno1 port... \n");
	printf(  "\t       >> sudo ip link set enp2s0 up \n");	
	printf(  "\t       >> sudo ip link set eno1 up \n");	

	
	printf("\n\t ############ Usage info ############ \n");
	printf(  "\t # [Options]                        # \n");
	printf(  "\t #    (1) /v : Version check        # \n");
	printf(  "\t #    (2) /f : Firmware update      # \n");
	printf(  "\t #    (3) /p : Parameter update     # \n");
	printf(  "\t #    (4-1) /r  : Reset (V8.2 only) # \n");
	printf(  "\t #    (4-2) /rs : Reset (CSD7)      # \n");
	printf(  "\t #    (4-3) /rd : Reset (D8)        # \n");
	printf(  "\t #################################### \n");
	printf(  "\t #    (5) /t : S/W Engeneer mode    # \n");
	printf(  "\t #    (6) /g : Get Common Value     # \n");
	printf(  "\t #    (7) /s : Set Common Value     # \n");
	printf(  "\t #    (8) /e : Set SafeOP Value     # \n");
	printf(  "\t #################################### \n\n");
	
	printf("\n\t (1) Firmware version check : sudo ./firm_update /v  network_name  slave_number \n");
	printf(  "\t     (example) \n");
	printf(  "\t       Motion version check => sudo ./firm_update /v enp2s0  1 \n");
	printf(  "\t       Hoist  version check => sudo ./firm_update /v enp2s0  2 \n");
	printf(  "\t       Slide  version check => sudo ./firm_update /v enp2s0  3 \n\n");

	printf("\n\t (2) Run firmware update : sudo ./firm_update /f  network_name  slave_number firmware_name \n");
	printf(  "\t     (example) \n");
	printf(  "\t       sudo ./firm_update /f enp2s0  1 D8N_V_01_01_60_02.bin \n\n");

	printf("\n\t (3) Run parameter update : firm_update /p  network_name  slave_number param_name \n");
	printf(  "\t     (example) \n");
	printf(  "\t       Motion parameter update => sudo ./firm_update /p enp2s0  1 param_driving.txt \n");
	printf(  "\t       Hoist  parameter update => sudo ./firm_update /p enp2s0  2 param_hoist.txt   \n");
	printf(  "\t       Slide  parameter update => sudo ./firm_update /p enp2s0  3 param_slide.txt   \n\n");	

	printf("\n\t (4) Reset motion controller : firm_update /r	network_name  slave_number \n");
	printf(  "\t	 (example) \n");
	printf(  "\t	   Motion controller reset => sudo ./firm_update /r enp2s0	1 \n");
	printf(  "\t	   Hoist  controller reset => sudo ./firm_update /r enp2s0	2 \n");
	printf(  "\t	   Slide  controller reset => sudo ./firm_update /r enp2s0	3 \n\n"); 

	printf("\n\t (5) S/W Developer Test mode : firm_update /t	network_name  slave_number \n");
	printf(  "\t	 (example) \n");
	printf(  "\t	   Run test func => sudo ./firm_update /t enp2s0	1 \n");

	printf("\n\t (6) Get Common Value : sudo ./firm_update /g  [2]network_name  [3]slave_number [4]addr_index(dec) [5]addr_subindex(dec)  [6]data_length(1,2,4) \n");
	printf(  "\t	 (example) \n");
	printf(  "\t	   Get Driving(Front) ErrCode => sudo ./firm_update /g enp2s0  1 26687 0 2 \n");
	printf(  "\t	     result file ----> common_read.txt \n");

	printf("\n\t (7) Set Common Value : sudo ./firm_update /s  [2]network_name  [3]slave_number [4]addr_index(dec) [5]addr_subindex(dec)  [6]data_length(1,2,4) [7]SetValue(dec)\n");
	printf(  "\t	 (example) \n");
	printf(  "\t	   Hoist Motion Absolute Encoder Reset => sudo ./firm_update /s enp2s0  2 12298 0 1 1 \n");

	printf("\n\t (8) Set SafeOP Value : sudo ./firm_update /e  [2]network_name	[3]slave_number [4]addr_index(dec) [5]addr_subindex(dec)  [6]data_length(1,2,4) [7]SetValue(dec)\n");
	printf(  "\t	 (example) \n");
	printf(  "\t	   sudo ./firm_update /e enp2s0	2 12298 0 1 1 \n");

}

void print_data_type(int iType)
{
	switch(iType)
	{
	// 1byte
		case TYPE_USINT:
		{		
			printf("(TYPE_USINT) 1Byte \n");
		}
		break;
	
		case TYPE_SINT:
		{		
			printf("(TYPE_SINT) 1Byte \n");
		}
		break;
		
	// 2byte
		case TYPE_UINT:
		{			
			printf("(TYPE_UINT) 2Byte \n");
		}
		break;
	
		case TYPE_INT:
		{			
			printf("(TYPE_INT) 2Byte \n");
		}
		break;
		
	// 4byte
		case TYPE_UDINT:
		{		
			printf("(TYPE_UDINT) 4Byte \n");
		}
		break;
	
		case TYPE_DINT:
		{	
			printf("(TYPE_DINT) 4Byte \n");
		}
		break;
	}
}

/*
	argv[0] = running file name
    argv[1] = option
    argv[2] = network
    argv[3] = slave no
    argv[4] = data file name
*/
int main(int argc, char *argv[])
{
    int iResult = RESULT_FAIL; // 0 is fail. 1 is success.
    int iAddr_Index = 0;
	int iAddr_Sub = 0;
	int iData_Length = 0;
	int iWrite_Data = 0;
	
    printf("\n>>> SEMES Firmware update utility. Version : %d.%d \n", VERSION_MAJOR, VERSION_MINOR);

	if((argc == 4) || (argc == 5) || (argc == 7) || (argc == 8))
	{
		argslave = atoi(argv[3]);

		// if argv[3] is not number, atoi function return value is 0.
		if(argslave == 0)
		{
			printf("Slave no is not number! please check!! \n\n");
			print_usage();
			return -1;
		}
		else
		{
		    printf("Slave no check ok = %d \n", argslave);
			
		}
	}

	if (argc == 4)		// firmware version check & controller reset
	{
		if((strcmp(argv[1], "/v") == 0) || (strcmp(argv[1], "/V") == 0))
		{			
#if VERSION_DISPLAY_ON	
			iResult = readVer(argv[2], argslave);
#else
			printf("version view.... [1]opt=%s [2]net=%s [3]slave=%d \n",
					argv[1], argv[2], argslave);

			// ---------- save log dummy 
			   FILE *fp;
			   fp = fopen("firm_version.txt", "w");
			   if(fp == NULL)
			       printf("Log file open failed... \n");					   
			   else
				   printf("Firmware version file open... \n");		   

			   fprintf(fp, "%s\n", "1.01.60.07");
     		   fclose(fp);

#endif			
		}
		else if((strcmp(argv[1], "/r") == 0) || (strcmp(argv[1], "/R") == 0))
		{
			printf("Controller reset(V8.2)... [1]opt=%s [2]net=%s [3]slave=%d \n",
					argv[1], argv[2], argslave);

			if(argslave == 1)
				iResult = resettest(argv[2], argslave, CONT_D8);
			else
				iResult = resettest(argv[2], argslave, CONT_CSD7);

			if(iResult == RESULT_FAIL)
				printf("Controller = %d Reset fail... \n", argslave);
			else
				printf("Controller = %d Reset success! \n", argslave);
		}
		else if((strcmp(argv[1], "/rs") == 0) || (strcmp(argv[1], "/RS") == 0)
			 || (strcmp(argv[1], "/rS") == 0) || (strcmp(argv[1], "/Rs") == 0))
		{
			printf("Controller reset(CSD7)... [1]opt=%s [2]net=%s [3]slave=%d \n",
					argv[1], argv[2], argslave);

			iResult = resettest(argv[2], argslave, CONT_CSD7);

			if(iResult == RESULT_FAIL)
				printf("Controller = %d Reset fail... \n", argslave);
			else
				printf("Controller = %d Reset success! \n", argslave);
		}	
		else if((strcmp(argv[1], "/rd") == 0) || (strcmp(argv[1], "/RD") == 0)
			 || (strcmp(argv[1], "/rS") == 0) || (strcmp(argv[1], "/Rd") == 0))
		{
			printf("Controller reset(D8)... [1]opt=%s [2]net=%s [3]slave=%d \n",
					argv[1], argv[2], argslave);

			iResult = resettest(argv[2], argslave, CONT_D8);

			if(iResult == RESULT_FAIL)
				printf("Controller = %d Reset fail... \n", argslave);
			else
				printf("Controller = %d Reset success! \n", argslave);
		}
		else if((strcmp(argv[1], "/t") == 0) || (strcmp(argv[1], "/T") == 0))
		{
			printf("Developer test... [1]opt=%s [2]net=%s [3]slave=%d \n",
					argv[1], argv[2], argslave);
			dev_debug(argv[2], argslave);	
		}
		else if ((strcmp(argv[1], "/x") == 0) || (strcmp(argv[1], "/X") == 0))
		{
#if PARAM_EXTRACTION_ON
        iResult = param_extract(argv[2], argslave);
#else
		printf("parameter extraction.... [1]opt=%s [2]net=%s [3]slave=%d \n",
						argv[1], argv[2], argslave);
		iResult = RESULT_SUCCESS;
#endif
		}
		else
		{
			printf("\n Command error(4)! Please check command! \n");
			print_usage();
		}
	}

// =================================================================

	else if (argc == 5)	// firmware update OR parameter update
	{		
		if((strcmp(argv[1], "/f") == 0) || (strcmp(argv[1], "/F") == 0))
		{			
#if FIRM_UPDATE_ON			
			iResult = boottest(argv[2], argslave, argv[4]);
#else
			printf("firmware update.... [1]opt=%s [2]net=%s [3]slave=%d [4]file=%s \n",
					argv[1], argv[2], argslave, argv[4]);
#endif
		}
		else if((strcmp(argv[1], "/p") == 0) || (strcmp(argv[1], "/P") == 0))
		{
		    iResult = parameter_load(argv[4]);	// 0 : fail, 1 : success
			if(iResult == RESULT_FAIL)
			{
				printf("Parameter data file format error. Check please! \n");
				return iResult;
			}
			
#if PARAM_UPDATE_ON			
			iResult = param_update(argv[2], argslave);
#else
			printf("param update.... [1]opt=%s [2]net=%s [3]slave=%d [4]file=%s \n",
					argv[1], argv[2], argslave, argv[4]);
			iResult = RESULT_SUCCESS;
#endif
		}
		else
		{
			printf("\n Command error(5)! Please check command! \n");
			print_usage();
		}
	}
	else if (argc == 7)	// get common value
	{
		if((strcmp(argv[1], "/g") == 0) || (strcmp(argv[1], "/G") == 0))
		{	
			iAddr_Index  = atoi(argv[4]);
			iAddr_Sub    = atoi(argv[5]);
			iData_Length = atoi(argv[6]);

			printf("Get common parameter view.... [1]opt=%s [2]net=%s [3]slave=%d [4]Index=0x%04x [5]SubIndex=0x%02x [6]DataLen=%d \n",
					argv[1], argv[2], argslave, iAddr_Index, iAddr_Sub, iData_Length);
			print_data_type(iData_Length);

#if GET_COMMON_ON	
			iResult = readCommon(argv[2], argslave, iAddr_Index, iAddr_Sub, iData_Length);
#else
			// test
			if(argslave == 1)	// motion
			{
				if(iAddr_Index == 0x683F)
					iWrite_Data = 0xE1;
				else
					iWrite_Data = 0xE2;				
			}
			else if(argslave == 2)
				iWrite_Data = 0xF3;
			else if(argslave == 3)
				iWrite_Data = 0xF3;
			else
				iWrite_Data = 0xFF;

			// ---------- save log dummy 
		    FILE *fp;
		    fp = fopen("common_read.txt", "w");
		    if(fp == NULL)
		    {
			   printf("common_read file open failed... \n");
			   iResult = RESULT_FAIL; 
		    }
		    else
		   	{
			   printf("common_read file open... \n");
		   	   fprintf(fp, "%d\n", iWrite_Data);
		       fclose(fp);

			   iResult = RESULT_SUCCESS; 
		    }
#endif
		    printf("Get Common => Idx : 0x%04X (Hex) Sub : 0x%04X (Hex) => result : %d(Dec) 0x%04X (Hex) \n",
		   			iAddr_Index, iAddr_Sub, iResult, iResult);			   
		}
		else
		{
			printf("\n Command error(7)! Please check command! \n");
			print_usage();
		}
	}   
	else if (argc == 8)	// set CommonValue() or set writeSafeOp()
	{
		if((strcmp(argv[1], "/s") == 0) || (strcmp(argv[1], "/S") == 0))
		{
			iAddr_Index  = atoi(argv[4]);
			iAddr_Sub    = atoi(argv[5]);
			iData_Length = atoi(argv[6]);
			iWrite_Data  = atoi(argv[7]);

			printf("Set Common parameter view.... [1]opt=%s [2]net=%s [3]slave=%d [4]Index=0x%04x [5]SubIndex=0x%02x [6]DataLen=%d [7]WriteVal=%d \n",
				argv[1], argv[2], argslave, iAddr_Index, iAddr_Sub, iData_Length, iWrite_Data);
			print_data_type(iData_Length);
			
#if SET_COMMON_ON
			iResult = writeCommon(argv[2], argslave, iAddr_Index, iAddr_Sub, iData_Length, iWrite_Data);
#else 
			iResult = RESULT_SUCCESS; 

/*
			if(argslave == 1)	// motion
			{
				if(iAddr_Index == 0x683F)
					iResult = 0xE1;
				else
					iResult = 0xE2;				
			}
			else if(argslave == 2)
				iResult = 0xF3;
			else if(argslave == 3)
				iResult = 0xF4;
			else
				iResult = 0xFF;	
*/
#endif
		printf("Set Common => Idx : 0x%04X (Hex) Sub : 0x%04X (Hex) => result : %d (Dec)	0x%04X (Hex) \n",
				 iAddr_Index, iAddr_Sub, iResult, iResult); 

		}
		else if((strcmp(argv[1], "/e") == 0) || (strcmp(argv[1], "/E") == 0))
		{
			iAddr_Index  = atoi(argv[4]);
			iAddr_Sub	 = atoi(argv[5]);
			iData_Length = atoi(argv[6]);
			iWrite_Data  = atoi(argv[7]);

			printf("Set Safe Op parameter view.... [1]opt=%s [2]net=%s [3]slave=%d [4]Index=0x%04x [5]SubIndex=0x%02x [6]DataLen=%d [7]WriteVal=%d \n",
				argv[1], argv[2], argslave, iAddr_Index, iAddr_Sub, iData_Length, iWrite_Data);
			print_data_type(iData_Length);
					
#if SET_SAFE_OP_ON
			iResult = writeSafeOp(argv[2], argslave, iAddr_Index, iAddr_Sub, iData_Length, iWrite_Data);
#else 
			iResult = RESULT_SUCCESS; 		
#endif
			printf("Set Safe Op => Idx : 0x%04X (Hex) Sub : 0x%04X (Hex) => result : %d (Dec)	0x%04X (Hex) \n",
					iAddr_Index, iAddr_Sub, iResult, iResult); 

		}
		else
		{
			printf("\n Command error(8)! Please check command! \n");
			print_usage();
		}
	}
	else
	{
		printf("\n Command error(0)! Please check command! \n");
		print_usage();
	}	

	return (iResult);
}

