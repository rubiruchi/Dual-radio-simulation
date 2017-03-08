#ifndef RESIDUAL_H__
#define RESIDUAL_H__
#define RESIDUAL_ENERGY_MAX 100000

#include <stdint.h>
#include "contiki-conf.h"
typedef int32_t energy_t;


#ifdef COOJA
energy_t COOJA_radioOn;
energy_t COOJA_radioTx;
energy_t COOJA_radioRx;
energy_t COOJA_radioOnLong;
energy_t COOJA_radioTxLong;
energy_t COOJA_radioRxLong;
energy_t COOJA_duration;
#endif

#ifdef ZOLERTIA_Z1
#include "energest.h"
#endif

energy_t get_residual_energy(void);


#endif
