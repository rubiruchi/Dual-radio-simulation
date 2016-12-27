/*
 * Copyright (c) 2010, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include <stdio.h>
#include <string.h>

#include "contiki.h"

#include "sys/cooja_mt.h"
#include "lib/simEnvChange.h"

#include "net/packetbuf.h"
#include "net/rime/rimestats.h"
#include "net/netstack.h"

#include "dev/radio.h"
#include "dev/cooja-radio.h"

/* JOONKI */
#include <stdio.h>

#if DUAL_RADIO
#include "../dual_conf.h"
#endif

#define COOJA_RADIO_BUFSIZE PACKETBUF_SIZE
#define CCA_SS_THRESHOLD -95

#define WITH_TURNAROUND 1
#define WITH_SEND_CCA 1

/* COOJA */
char simReceiving = 0;
char simReceivingLR = 0;
char simInDataBuffer[COOJA_RADIO_BUFSIZE];
char simInDataBufferLR[COOJA_RADIO_BUFSIZE];
int simInSize = 0;
int simInSizeLR = 0;
char simOutDataBuffer[COOJA_RADIO_BUFSIZE];
char simOutDataBufferLR[COOJA_RADIO_BUFSIZE];
int simRadioChannel = 26;
int simRadioChannelLR = 27;
int simOutSize = 0;
int simOutSizeLR = 0;

char simRadioHWOn = 1;
char simRadioHWOnLR = 1;

int simSignalStrength = -100;
int simSignalStrengthLR = -100;

/* Not used variable */
int simLastSignalStrength = -100;
int simLastSignalStrengthLR = -100;

char simPower = 100;
char simPowerLR = 100;

int simLQI = 105;
int simLQILR = 105;

int LongRangeTransmit = 0;
int LongRangeReceiving = 0;
static const void *pending_data;

PROCESS(cooja_radio_process, "cooja radio process");

/* JOONKI */
//extern mac_callback_t * global_sent;
//extern FILE *debugfp;

/*---------------------------------------------------------------------------*/
void
radio_set_channel(int channel)
{
  simRadioChannel = channel;
	simRadioChannelLR = channel;
}
/*---------------------------------------------------------------------------*/
void
radio_set_txpower(unsigned char power)
{
  /* 1 - 100: Number indicating output power */
  simPower = power;
	simPowerLR = power;
}
/*---------------------------------------------------------------------------*/
int
radio_signal_strength_last(void)
{
	/* Not used function */
  return simLastSignalStrength;
}
/*---------------------------------------------------------------------------*/
int
radio_signal_strength_current(void)
{
	/* Not used function */
  return simSignalStrength;
}
/*---------------------------------------------------------------------------*/
int
radio_LQI(void)
{
	/* Not used function */
	return simLQI;
}
/*---------------------------------------------------------------------------*/
static int
radio_on(void)
{
  simRadioHWOn = 1;
  return 1;
}
/*---------------------------------------------------------------------------*/
static int
radio_off(void)
{
  simRadioHWOn = 0;
  return 1;
}
/*---------------------------------------------------------------------------*/
static void
doInterfaceActionsBeforeTick(void)
{
  if(!simRadioHWOn) {
    simInSize = 0;
    return;
  }	else if(!simRadioHWOnLR) {
    simInSizeLR = 0;
    return;
  }
  if(simReceiving) {
    simLastSignalStrength = simSignalStrength;
    return;
  }	else if(simReceivingLR) {
    simLastSignalStrengthLR = simSignalStrengthLR;
    return;
  }
//  printf("simInsize\n");
  if(simInSize > 0 || simInSizeLR > 0) {
	  printf("In the simInsize\n");
	    process_poll(&cooja_radio_process);
  }
//  printf("simInsize2\n");
}
/*---------------------------------------------------------------------------*/
static void
doInterfaceActionsAfterTick(void)
{
}
/*---------------------------------------------------------------------------*/
static int
// radio_read(void *buf, unsigned short bufsize)
radio_read(void *buf, unsigned short bufsize)
{
	printf("radio read\n");
	int tmp = simInSize;

  if(simInSize == 0 && simInSizeLR == 0) {
    return 0;
  }	
  if(bufsize < simInSize || bufsize <simInSizeLR ) {
    simInSize = 0; /* rx flush */
		simInSizeLR = 0;
    RIMESTATS_ADD(toolong);
    return 0;
  }

#if DUAL_RADIO
			/*JOONKI*/
			if (LongRangeReceiving > 0)	{
				dual_radio_received(LONG_RADIO);
			}	else {
				dual_radio_received(SHORT_RADIO);
			}
#endif

	/*	if (sent != NULL){
		fprintf(debugfp,"INSIDE READ BEFORE memcpy: cooja-radio_driver/radio_read sent: %x\n\n", *sent);
		fflush(debugfp);
	}	*/
#if DUAL_RADIO
	if(radio_received_is_longrange() == LONG_RADIO)	{
		memcpy(buf, simInDataBufferLR, simInSizeLR);
		tmp = simInSizeLR;
		// fprintf(debugfp, "BUFFER address : %x, simInSizeLR : %d, simInSizeSR : %d\n\n",buf,simInSizeLR,simInSize);
		// fflush(debugfp);
		simInSizeLR = 0;
	}	else {
		memcpy(buf, simInDataBuffer, simInSize);
		// fprintf(debugfp, "BUFFER address : %x, simInSizeLR : %d, simInSizeSR : %d\n\n",buf,simInSizeLR,simInSize);
		// fflush(debugfp);
		simInSize = 0;
	}
#else
		memcpy(buf, simInDataBuffer, simInSize);
		simInSize = 0;
#endif
	/* if (sent != NULL){
		fprintf(debugfp,"INSIDE READ AFTER memcpy: cooja-radio_driver/radio_read sent: %x\n\n", *sent);
		fflush(debugfp);
	}	*/
#if DUAL_RADIO
	if(radio_received_is_longrange() == LONG_RADIO) {
	  packetbuf_set_attr(PACKETBUF_ATTR_RSSI, simSignalStrengthLR);
	  packetbuf_set_attr(PACKETBUF_ATTR_LINK_QUALITY, simLQILR);
	} else {
		packetbuf_set_attr(PACKETBUF_ATTR_RSSI, simSignalStrength);
		packetbuf_set_attr(PACKETBUF_ATTR_LINK_QUALITY, simLQI);
	}
#else
		packetbuf_set_attr(PACKETBUF_ATTR_RSSI, simSignalStrength);
		packetbuf_set_attr(PACKETBUF_ATTR_LINK_QUALITY, simLQI);
#endif
	/* if (sent != NULL){
		fprintf(debugfp,"INSIDE READ AFTER packetbuf: cooja-radio_driver/radio_read sent: %x\n\n", *sent);
		fflush(debugfp);
	}	*/
  return tmp;
}
/*---------------------------------------------------------------------------*/
static int
channel_clear(void)
{
  if(simSignalStrength > CCA_SS_THRESHOLD) {
    return 0;
  } else if(simSignalStrengthLR > CCA_SS_THRESHOLD) {
    return 0;
  }
  return 1;
}
/*---------------------------------------------------------------------------*/
static int
radio_send(const void *payload, unsigned short payload_len)
{
  int radiostate = simRadioHWOn;

  /* Simulate turnaround time of 2ms for packets, 1ms for acks*/
#if WITH_TURNAROUND
  simProcessRunValue = 1;
  cooja_mt_yield();
  if(payload_len > 3) {
    simProcessRunValue = 1;
    cooja_mt_yield();
  }
#endif /* WITH_TURNAROUND */

  if(!simRadioHWOn) {
    /* Turn on radio temporarily */
    simRadioHWOn = 1;
  }
  if(payload_len > COOJA_RADIO_BUFSIZE) {
    return RADIO_TX_ERR;
  }
  if(payload_len == 0) {
    return RADIO_TX_ERR;
  }
	printf("COOJA RADIO: Sending packet in cooja driver\n");

#if DUAL_RADIO
/* IN CASE OF LONG RADIO */
	if (sending_in_LR() == LONG_RADIO){
	  if(simOutSizeLR > 0) {
	    return RADIO_TX_ERR;
	  }
		printf("$$$$$$$$$$$$$$$ Sending in LR ------------------------>\n");
		// printf("LongRangeTransmit : %d\n",LongRangeTransmit); 
  	/* Transmit on CCA */
#if WITH_SEND_CCA
	  if(!channel_clear()) {
  	  return RADIO_TX_COLLISION;
	  }	
#endif /* WITH_SEND_CCA */
	  /* Copy packet data to temporary storage */
	  memcpy(simOutDataBufferLR, payload, payload_len);
		simOutSizeLR = payload_len;
		}	
/* IN CASE OF SHORT RADIO */
	else {	
#endif /* DUAL_RADIO */
		printf("$$$$$$$$$$$$$$$$ Sending in SR ------->\n");
		// printf("LongRangeTransmit : %d\n",LongRangeTransmit); 
	  if(simOutSize > 0) {
	    return RADIO_TX_ERR;
	  }

	  /* Transmit on CCA */
#if WITH_SEND_CCA
	  if(!channel_clear()) {
	    return RADIO_TX_COLLISION;
	  }
#endif /* WITH_SEND_CCA */
	  /* Copy packet data to temporary storage */
 	 memcpy(simOutDataBuffer, payload, payload_len);
 	 simOutSize = payload_len;

#if DUAL_RADIO
	}
#endif

  /* Transmit */
  while(simOutSize > 0 || simOutSizeLR > 0) {
    cooja_mt_yield();
  }

  simRadioHWOn = radiostate;
  return RADIO_TX_OK;
}
/*---------------------------------------------------------------------------*/
static int
prepare_packet(const void *data, unsigned short len)
{
  pending_data = data;
  return 0;
}
/*---------------------------------------------------------------------------*/
static int
transmit_packet(unsigned short len)
{
  int ret = RADIO_TX_ERR;
  if(pending_data != NULL) {
    ret = radio_send(pending_data, len);
  }
  return ret;
}
/*---------------------------------------------------------------------------*/
static int
receiving_packet(void)
{
	return simReceiving|simReceivingLR;
}
/*---------------------------------------------------------------------------*/
static int
pending_packet(void)
{
  return (!simReceiving && simInSize > 0)|(!simReceivingLR && simInSizeLR > 0);
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(cooja_radio_process, ev, data)
{
  int len;

  PROCESS_BEGIN();

  while(1) {
    PROCESS_YIELD_UNTIL(ev == PROCESS_EVENT_POLL);
    packetbuf_clear();
		// if (simInSize >0 && simInSizeLR > 0)
		//	dual_radio_received(SHORT_RADIO);

    len = radio_read(packetbuf_dataptr(), PACKETBUF_SIZE);
		if(len > 0) {
      packetbuf_set_datalen(len);
   	  NETSTACK_RDC.input();
   	}
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
static int
init(void)
{
  process_start(&cooja_radio_process, NULL);
  return 1;
}
/*---------------------------------------------------------------------------*/
static radio_result_t
get_value(radio_param_t param, radio_value_t *value)
{
  return RADIO_RESULT_NOT_SUPPORTED;
}
/*---------------------------------------------------------------------------*/
static radio_result_t
set_value(radio_param_t param, radio_value_t value)
{
  return RADIO_RESULT_NOT_SUPPORTED;
}
/*---------------------------------------------------------------------------*/
static radio_result_t
get_object(radio_param_t param, void *dest, size_t size)
{
  return RADIO_RESULT_NOT_SUPPORTED;
}
/*---------------------------------------------------------------------------*/
static radio_result_t
set_object(radio_param_t param, const void *src, size_t size)
{
  return RADIO_RESULT_NOT_SUPPORTED;
}
/*---------------------------------------------------------------------------*/
const struct radio_driver cooja_radio_driver =
{
    init,
    prepare_packet,
    transmit_packet,
    radio_send,
    radio_read,
    channel_clear,
    receiving_packet,
    pending_packet,
    radio_on,
    radio_off,
    get_value,
    set_value,
    get_object,
    set_object
};
/*---------------------------------------------------------------------------*/
SIM_INTERFACE(radio_interface,
	      doInterfaceActionsBeforeTick,
	      doInterfaceActionsAfterTick);
