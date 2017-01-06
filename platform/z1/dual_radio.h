#include "platform-conf.h"
#include "cc1200.h"
#include "cc2420.h"
#include "net/rpl/rpl-private.h"

#ifndef SHORT_RADIO
#define SHORT_RADIO 1
#endif

#ifndef LONG_RADIO
#define LONG_RADIO 2
#endif

/* Radio drivers */
extern struct radio_driver NETSTACK_CONF_RADIO;
extern struct radio_driver NETSTACK_RADIO;

/* Functions */
int dual_radio_switch(int radio);
int dual_radio_change(void);
int dual_radio_received(int radio);
int radio_received_is_longrange(void);
int sending_in_LR(void);

/* Process */
int dio_broadcast(rpl_instance_t* instance);
int dis_broadcast(void);

/* Global variable for Dual RPL */
int long_range_radio;
int radio_received;
