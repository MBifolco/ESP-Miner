#ifndef FAYKSIC_H_
#define FAYKSIC_H_

#include "common.h"
#include "driver/gpio.h"
#include "mining.h"

#define ASIC_FAYKSIC_JOB_FREQUENCY_MS 2000

#define CRC5_MASK 0x1F
#define FAYKSIC_ASIC_DIFFICULTY 256

#define FAYKSIC_SERIALTX_DEBUG false
#define FAYKSIC_SERIALRX_DEBUG false
#define FAYKSIC_DEBUG_WORK false //causes insane amount of debug output
#define FAYKSIC_DEBUG_JOBS false //causes insane amount of debug output

static const uint64_t FAYKSIC_CORE_COUNT = 112;
static const uint64_t FAYKSIC_SMALL_CORE_COUNT = 894;

typedef struct
{
    float frequency;
} fayksicModule;

typedef struct __attribute__((__packed__))
{
    uint8_t job_id;
    uint8_t num_midstates;
    uint8_t starting_nonce[4];
    uint8_t nbits[4];
    uint8_t ntime[4];
    uint8_t merkle_root[32];
    uint8_t prev_block_hash[32];
    uint8_t version[4];
} FAYKSIC_job;

uint8_t FAYKSIC_init(uint64_t frequency, uint16_t asic_count);

void FAYKSIC_send_init(void);
void FAYKSIC_send_work(void * GLOBAL_STATE, bm_job * next_bm_job);
void FAYKSIC_set_job_difficulty_mask(int);
void FAYKSIC_set_version_mask(uint32_t version_mask);
int FAYKSIC_set_max_baud(void);
int FAYKSIC_set_default_baud(void);
void FAYKSIC_send_hash_frequency(float frequency);
task_result * FAYKSIC_proccess_work(void * GLOBAL_STATE);

#endif /* FAYKSIC_H_ */