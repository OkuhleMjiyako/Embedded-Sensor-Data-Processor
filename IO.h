//
// Created by mjiyako on 01/07/2026.
//

#ifndef EMBEDDED_SENSOR_DATA_PROCESSOR_IO_H
#define EMBEDDED_SENSOR_DATA_PROCESSOR_IO_H
#include <stdint.h>
#include <stddef.h>
#include "adc.h"


#pragma pack(push, 1)
typedef struct {
    uint32_t magic;
    uint16_t version;
    uint16_t channel_count;
    uint32_t record_count;
    uint32_t sample_rate_hz;
    uint8_t  reserved[8];
} FileHeader;
#pragma pack(pop)

#define ADC_MAGIC 0xADC1BEEFu
#define ADC_EXPECTED_VERSION 1
#define ADC_EXPECTED_CHANNEL_COUNT 4
#define ADC_EXPECTED_SAMPLE_RATE 1000

int io_read_file(const char *filename, ADCSample **out_samples,
                 size_t *out_count, FileHeader *out_header);

int io_write_results(const char *filename, const FileHeader *header,
                     const ChannelStats channel_stats[4],
                     const FaultCounts fault_counts[4],
                     const IntegrityReport *integrity);




#endif //EMBEDDED_SENSOR_DATA_PROCESSOR_IO_H
