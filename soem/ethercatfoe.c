/*
 * Licensed under the GNU General Public License version 2 with exceptions. See
 * LICENSE file in the project root for full license information
 */

/** \file
 * \brief
 * File over EtherCAT (FoE) module.
 *
 * SDO read / write and SDO service functions
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "osal.h"
#include "oshw.h"
#include "ethercattype.h"
#include "ethercatbase.h"
#include "ethercatmain.h"
#include "ethercatfoe.h"

#define EC_MAXFOEDATA 512

/** FOE structure.
 * Used for Read, Write, Data, Ack and Error mailbox packets.
 */
PACKED_BEGIN
typedef struct PACKED
{
   ec_mbxheadert MbxHeader;
   uint8         OpCode;
   uint8         Reserved;
   union
   {
      uint32        Password;
      uint32        PacketNumber;
      uint32        ErrorCode;
   };
   union
   {
      char          FileName[EC_MAXFOEDATA];
      uint8         Data[EC_MAXFOEDATA];
      char          ErrorText[EC_MAXFOEDATA];
   };
} ec_FOEt;
PACKED_END

/** FoE progress hook.
 *
 * @param[in]  context        = context struct
 * @param[in]     hook       = Pointer to hook function.
 * @return 1
 */
int ecx_FOEdefinehook(ecx_contextt *context, void *hook)
{
  context->FOEhook = hook;
  return 1;
}

/** FoE read, blocking.
 *
 * @param[in]  context        = context struct
 * @param[in]     slave      = Slave number.
 * @param[in]     filename   = Filename of file to read.
 * @param[in]     password   = password.
 * @param[in,out] psize      = Size in bytes of file buffer, returns bytes read from file.
 * @param[out]    p          = Pointer to file buffer
 * @param[in]     timeout    = Timeout per mailbox cycle in us, standard is EC_TIMEOUTRXM
 * @return Workcounter from last slave response
 */
int ecx_FOEread(ecx_contextt *context, uint16 slave, char *filename, uint32 password, int *psize, void *p, int timeout)
{
   ec_FOEt *FOEp, *aFOEp;
   int wkc;
   int32 dataread = 0;
   int32 buffersize, packetnumber, prevpacket = 0;
   uint16 fnsize, maxdata, segmentdata;
   ec_mbxbuft MbxIn, MbxOut;
   uint8 cnt;
   boolean worktodo;

   buffersize = *psize;
   ec_clearmbx(&MbxIn);
   /* Empty slave out mailbox if something is in. Timeout set to 0 */
   wkc = ecx_mbxreceive(context, slave, (ec_mbxbuft *)&MbxIn, 0);
   ec_clearmbx(&MbxOut);
   aFOEp = (ec_FOEt *)&MbxIn;
   FOEp = (ec_FOEt *)&MbxOut;
   fnsize = (uint16)strlen(filename);
   maxdata = context->slavelist[slave].mbx_l - 12;
   if (fnsize > maxdata)
   {
      fnsize = maxdata;
   }
   FOEp->MbxHeader.length = htoes(0x0006 + fnsize);
   FOEp->MbxHeader.address = htoes(0x0000);
   FOEp->MbxHeader.priority = 0x00;
   /* get new mailbox count value, used as session handle */
   cnt = ec_nextmbxcnt(context->slavelist[slave].mbx_cnt);
   context->slavelist[slave].mbx_cnt = cnt;
   FOEp->MbxHeader.mbxtype = ECT_MBXT_FOE + MBX_HDR_SET_CNT(cnt); /* FoE */
   FOEp->OpCode = ECT_FOE_READ;
   FOEp->Password = htoel(password);
   /* copy filename in mailbox */
   memcpy(&FOEp->FileName[0], filename, fnsize);
   /* send FoE request to slave */
   wkc = ecx_mbxsend(context, slave, (ec_mbxbuft *)&MbxOut, EC_TIMEOUTTXM);
   if (wkc > 0) /* succeeded to place mailbox in slave ? */
   {
      do
      {
         worktodo = FALSE;
         /* clean mailboxbuffer */
         ec_clearmbx(&MbxIn);
         /* read slave response */
         wkc = ecx_mbxreceive(context, slave, (ec_mbxbuft *)&MbxIn, timeout);
         if (wkc > 0) /* succeeded to read slave response ? */
         {
            /* slave response should be FoE */
            if ((aFOEp->MbxHeader.mbxtype & 0x0f) == ECT_MBXT_FOE)
            {
               if(aFOEp->OpCode == ECT_FOE_DATA)
               {
                  segmentdata = etohs(aFOEp->MbxHeader.length) - 0x0006;
                  packetnumber = etohl(aFOEp->PacketNumber);
                  if ((packetnumber == ++prevpacket) && (dataread + segmentdata <= buffersize))
                  {
                     memcpy(p, &aFOEp->Data[0], segmentdata);
                     dataread += segmentdata;
                     p = (uint8 *)p + segmentdata;
                     if (segmentdata == maxdata)
                     {
                        worktodo = TRUE;
                     }
                     FOEp->MbxHeader.length = htoes(0x0006);
                     FOEp->MbxHeader.address = htoes(0x0000);
                     FOEp->MbxHeader.priority = 0x00;
                     /* get new mailbox count value */
                     cnt = ec_nextmbxcnt(context->slavelist[slave].mbx_cnt);
                     context->slavelist[slave].mbx_cnt = cnt;
                     FOEp->MbxHeader.mbxtype = ECT_MBXT_FOE + MBX_HDR_SET_CNT(cnt); /* FoE */
                     FOEp->OpCode = ECT_FOE_ACK;
                     FOEp->PacketNumber = htoel(packetnumber);
                     /* send FoE ack to slave */
                     wkc = ecx_mbxsend(context, slave, (ec_mbxbuft *)&MbxOut, EC_TIMEOUTTXM);
                     if (wkc <= 0)
                     {
                        worktodo = FALSE;
                     }
                     if (context->FOEhook)
                     {
                        context->FOEhook(slave, packetnumber, dataread);
                     }
                  }
                  else
                  {
                     /* FoE error */
                     wkc = -EC_ERR_TYPE_FOE_BUF2SMALL;
                  }
               }
               else
               {
                  if(aFOEp->OpCode == ECT_FOE_ERROR)
                  {
                     /* FoE error */
                     wkc = -EC_ERR_TYPE_FOE_ERROR;
                  }
                  else
                  {
                     /* unexpected mailbox received */
                     wkc = -EC_ERR_TYPE_PACKET_ERROR;
                  }
               }
            }
            else
            {
               /* unexpected mailbox received */
               wkc = -EC_ERR_TYPE_PACKET_ERROR;
            }
            *psize = dataread;
         }
      } while (worktodo);
   }

   return wkc;
}

/** FoE write, blocking.
 *
 * @param[in]  context        = context struct
 * @param[in]  slave      = Slave number.
 * @param[in]  filename   = Filename of file to write.
 * @param[in]  password   = password.
 * @param[in]  psize      = Size in bytes of file buffer.
 * @param[out] p          = Pointer to file buffer
 * @param[in]  timeout    = Timeout per mailbox cycle in us, standard is EC_TIMEOUTRXM
 * @return Workcounter from last slave response
 */
int ecx_FOEwrite(ecx_contextt *context, uint16 slave, char *filename, uint32 password, int psize, void *p, int timeout)
{
   ec_FOEt *FOEp, *aFOEp;
   int wkc;
   int32 packetnumber, sendpacket = 0;
   uint16 fnsize, maxdata;
   int segmentdata;
   ec_mbxbuft MbxIn, MbxOut;
   uint8 cnt;
   boolean worktodo, dofinalzero;
   int tsize;

   // ---------- save log
   FILE *fp;
   fp = fopen("er.txt", "a");
   if(fp == NULL)
      printf("Log file open failed... \n");
   else
      printf("Firmware update progress  ->  er.txt save... \n");

#define LOGF(...) do { if(fp) fprintf(fp, __VA_ARGS__); } while (0)

   ec_clearmbx(&MbxIn);
   /* Empty slave out mailbox if something is in. Timeout set to 0 */
   wkc = ecx_mbxreceive(context, slave, (ec_mbxbuft *)&MbxIn, 0);
   ec_clearmbx(&MbxOut);
   aFOEp = (ec_FOEt *)&MbxIn;
   FOEp = (ec_FOEt *)&MbxOut;
   dofinalzero = FALSE;
   fnsize = (uint16)strlen(filename);
   maxdata = context->slavelist[slave].mbx_l - 12;
   if (fnsize > maxdata)
   {
	   printf("#(01) fnsize = maxdata; Case.  fnsize = %d  maxdata = %d \n", fnsize, maxdata);
         LOGF("#(01) fnsize = maxdata; Case.  fnsize = %d  maxdata = %d \n", fnsize, maxdata);

      fnsize = maxdata;
   }
   FOEp->MbxHeader.length = htoes(0x0006 + fnsize);
   FOEp->MbxHeader.address = htoes(0x0000);
   FOEp->MbxHeader.priority = 0x00;
   /* get new mailbox count value, used as session handle */
   cnt = ec_nextmbxcnt(context->slavelist[slave].mbx_cnt);
   context->slavelist[slave].mbx_cnt = cnt;
   FOEp->MbxHeader.mbxtype = ECT_MBXT_FOE + MBX_HDR_SET_CNT(cnt); /* FoE */
   FOEp->OpCode = ECT_FOE_WRITE;

     printf(" \n  Log Pass(1) : %x \n", password);
        LOGF(" \n  Log Pass(1) : %x \n", password);

   FOEp->Password = htoel(password);
     printf(" \n  Log Pass(2) : %x \n", FOEp->Password);
        LOGF(" \n  Log Pass(2) : %x \n", FOEp->Password);

   /* copy filename in mailbox */
   memcpy(&FOEp->FileName[0], filename, fnsize);
   /* send FoE request to slave */
   wkc = ecx_mbxsend(context, slave, (ec_mbxbuft *)&MbxOut, EC_TIMEOUTTXM);
   if (wkc > 0) /* succeeded to place mailbox in slave ? */
   {
	   printf("#(02) begin wkc = %d \n", wkc);
         LOGF("#(02) begin wkc = %d \n", wkc);

      do
      {
         worktodo = FALSE;
         /* clean mailboxbuffer */
         ec_clearmbx(&MbxIn);
         /* read slave response */
         wkc = ecx_mbxreceive(context, slave, (ec_mbxbuft *)&MbxIn, timeout);
		   printf("#(03) ecx_mbxreceive Return = %d \n", wkc);
            LOGF("#(03) ecx_mbxreceive Return = %d \n", wkc);


         if (wkc > 0) /* succeeded to read slave response ? */
         {
            /* slave response should be FoE */
            if ((aFOEp->MbxHeader.mbxtype & 0x0f) == ECT_MBXT_FOE)
            {
			      printf("#(04) ECT_MBXT_FOE  \n");
                  LOGF("#(04) ECT_MBXT_FOE  \n");
               switch (aFOEp->OpCode)
               {
                  case ECT_FOE_ACK:
                  {
					      printf("#(05) ECT_FOE_ACK \n");
                     packetnumber = etohl(aFOEp->PacketNumber);

		               printf("#(30) packetnumber = %d sendpacket = %d \n", packetnumber, sendpacket);
                        LOGF("#(30) packetnumber = %d sendpacket = %d \n", packetnumber, sendpacket);

                     if (packetnumber == sendpacket)
                     {
                        if (context->FOEhook)
                        {
                           context->FOEhook(slave, packetnumber, psize);
			                  printf("#(31) slave = %d packetnumber = %d psize = %d  \n", slave, packetnumber, psize);
                              LOGF("#(31) slave = %d packetnumber = %d psize = %d  \n", slave, packetnumber, psize);
                        }

		                     printf("#(32)  psize = %d  maxdata = %d \n",  psize, maxdata);
                              LOGF("#(32)  psize = %d  maxdata = %d \n",  psize, maxdata);

                        tsize = psize;
                        if (tsize > maxdata)
                        {
                           tsize = maxdata;
                        }
                        if(tsize || dofinalzero)
                        {
                           worktodo = TRUE;
                           dofinalzero = FALSE;
                           segmentdata = tsize;

                           printf("#(32) psize = %d  segmentdata = %d maxdata = %d \n", psize, segmentdata, maxdata);
                              LOGF("#(32) psize = %d  segmentdata = %d maxdata = %d \n", psize, segmentdata, maxdata);

                           psize -= segmentdata;
                           /* if last packet was full size, add a zero size packet as final */
                           /* EOF is defined as packetsize < full packetsize */
                           if (!psize && (segmentdata == maxdata))
                           {
                              dofinalzero = TRUE;
                           }
                           FOEp->MbxHeader.length = htoes((uint16)(0x0006 + segmentdata));
                           FOEp->MbxHeader.address = htoes(0x0000);
                           FOEp->MbxHeader.priority = 0x00;
                           /* get new mailbox count value */
                           cnt = ec_nextmbxcnt(context->slavelist[slave].mbx_cnt);
                           context->slavelist[slave].mbx_cnt = cnt;
                           FOEp->MbxHeader.mbxtype = ECT_MBXT_FOE + MBX_HDR_SET_CNT(cnt); /* FoE */
                           FOEp->OpCode = ECT_FOE_DATA;
                           sendpacket++;
                           FOEp->PacketNumber = htoel(sendpacket);
                           memcpy(&FOEp->Data[0], p, segmentdata);
                           p = (uint8 *)p + segmentdata;
                           /* send FoE data to slave */
                           wkc = ecx_mbxsend(context, slave, (ec_mbxbuft *)&MbxOut, EC_TIMEOUTTXM);
			                  printf("#(06) wkc = %d  \n", wkc);
                              LOGF("#(06) wkc = %d  \n", wkc);


                           if (wkc <= 0)
                           {
			                     printf("#(07) wkc <= 0  \n");
                                 LOGF("#(07) wkc <= 0  \n");

                              worktodo = FALSE;
                           }
                        }
                     }
                     else
                     {
                        /* FoE error */
			               printf("#(08) packetnumber <> sendpacket... EC_ERR_TYPE_FOE_PACKETNUMBER!!  \n");
                           LOGF("#(08) packetnumber <> sendpacket... EC_ERR_TYPE_FOE_PACKETNUMBER!!  \n");
                        wkc = -EC_ERR_TYPE_FOE_PACKETNUMBER;
                     }
                     break;
                  }
                  case ECT_FOE_BUSY:
                  {
                     printf("#(09) ECT_FOE_BUSY \n");
                        LOGF("#(09) ECT_FOE_BUSY \n");

                     /* resend if data has been send before */
                     /* otherwise ignore */
                     if (sendpacket)
                     {
						      printf("#(10) sendpacket = %d \n", sendpacket);
                        if (!psize)
                        {
							      printf("#(11) psize = %d \n", psize);
                           dofinalzero = TRUE;
                        }
                        psize += segmentdata;
                        p = (uint8 *)p - segmentdata;
                        --sendpacket;
                     }
                     break;
                  }
                  case ECT_FOE_ERROR:
                  {
		               printf("#(12)  ECT_FOE_ERROR  \n");
                        LOGF("#(12)  ECT_FOE_ERROR  \n");

                     /* FoE error */
                     if (aFOEp->ErrorCode == 0x8001)
                     {
		                  printf("#(13) >> aFOEp->ErrorCode == 0x8001  \n");
                           LOGF("#(13) >> aFOEp->ErrorCode == 0x8001  \n");

                        wkc = -EC_ERR_TYPE_FOE_FILE_NOTFOUND;
                     }
                     else
                     {
			               printf("#(14) >> EC_ERR_TYPE_FOE_ERROR  \n");
                           LOGF("#(14) >> EC_ERR_TYPE_FOE_ERROR  \n");

                        wkc = -EC_ERR_TYPE_FOE_ERROR;
                     }
                     break;
                  }
                  default:
                  {
                     /* unexpected mailbox received */
                     printf("#(15) EC_ERR_TYPE_PACKET_ERROR   \n");
                        LOGF("#(15) EC_ERR_TYPE_PACKET_ERROR   \n");
                     wkc = -EC_ERR_TYPE_PACKET_ERROR;
                     break;
                  }
               }
            }
            else
            {
               /* unexpected mailbox received */
	           printf("#(16) NOT --> slave response should be FoE >> Inverse  \n");
                  LOGF("#(16) NOT --> slave response should be FoE >> Inverse  \n");
               wkc = -EC_ERR_TYPE_PACKET_ERROR;
            }
         }
      } while (worktodo);
   }

   printf("#(20) exit wkr = %d \n", wkc);
   LOGF("#(20) exit wkr = %d \n", wkc);
   if(fp) fclose(fp);

#undef LOGF

   if(wkc > 0)
   {  
      printf("################################################## \n");
      printf("             Firmware Update Success                                   \n");
      printf("################################################## \n");

      if(slave == 1)
         printf("Motion firmware update.... OK! \n");
      else if(slave == 2)
         printf("Hoist firmware update.... OK! \n");
      else if(slave == 3)
      printf("Slide firmware update.... OK! \n");
      else
      printf("(Unknown) firmware update.... OK! \n");

      printf("################################################## \n");
   }
   else
   {
      printf(">>>>>>>>>>  Firmware update fail... ErrCode = %d \n\n", wkc);
   }

   return wkc;
}

#ifdef EC_VER1
int ec_FOEdefinehook(void *hook)
{
   return ecx_FOEdefinehook(&ecx_context, hook);
}

int ec_FOEread(uint16 slave, char *filename, uint32 password, int *psize, void *p, int timeout)
{
   return ecx_FOEread(&ecx_context, slave, filename, password, psize, p, timeout);
}

int ec_FOEwrite(uint16 slave, char *filename, uint32 password, int psize, void *p, int timeout)
{
   return ecx_FOEwrite(&ecx_context, slave, filename, password, psize, p, timeout);
}
#endif
