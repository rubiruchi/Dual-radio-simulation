/* Adding simple remaining energy and alpha JJH*/
#define INITIAL_ENERGY 200
#define ALPHA 1

/* RPL MODES */
#define RPL_ENERGY_MODE 0
#define RPL_LIFETIME_MAX_MODE 1

/* Distributed weight update problem solutions */
#define MODE_DIO_WEIGHT_UPDATED 0
#define MODE_DAP_WEIGHT	0 // DAP (DODAG Ask Parent's weight)
#define MODE_PARENT_UPDATE_IN_ROUND 0 // Update parent only when round is synchronized

#if RPL_ENERGY_MODE
uint8_t remaining_energy;
uint8_t alpha;
#define LONG_ETX_PENALTY 5
#elif RPL_LIFETIME_MAX_MODE
uint8_t my_weight;
#define DATA_PKT_SIZE 10 // 'B' in theory
#define SHORT_TX_COST 1
#define SHORT_RX_COST 1
#define LONG_TX_COST 9
#define LONG_RX_COST 3
#endif

