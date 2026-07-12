//
// Created by mjiyako on 01/07/2026.
//

#ifndef EMBEDDED_SENSOR_DATA_PROCESSOR_ADC_H
#define EMBEDDED_SENSOR_DATA_PROCESSOR_ADC_H

#include <stdint.h>
#include <stddef.h>

typedef struct {
    float    timestamp;
    uint8_t  channel_id;
    uint16_t raw_value;
    float    voltage;
    int16_t  temperature;
    uint8_t  status_flags;
    uint32_t sequence_number;
} ADCSample;

#define ADC_VREF 3.3
#define ADC_MAX_RAW_VALUE 4095.0

#define OVERVOLTAGE_THRESHOLD  3.0f
#define UNDERVOLTAGE_THRESHOLD 0.3f
#define STATUS_FLAG_FAULT_BIT  0x01u
#define STATUS_FLAG_OOR_BIT    0x02u

typedef struct {
    uint8_t channel;
    double  mean_voltage;
    float   min_voltage;
    float   max_voltage;
    double  stddev_voltage;
    size_t  sample_count;
} ChannelStats;

typedef struct {
    uint8_t channel;
    size_t  overvoltage_count;
    size_t  undervoltage_count;
    size_t  status_fault_count;
} FaultCounts;

typedef struct {
    size_t gap_count;
    size_t missing_records;
    uint32_t first_gap_after_seq;
} IntegrityReport;

void adc_raw_to_voltage(ADCSample *sample);

void adc_compute_channel_stats(const ADCSample *samples, size_t count,
                               uint8_t channel, ChannelStats *out);

void adc_detect_faults(const ADCSample *samples, size_t count,
                       uint8_t channel, FaultCounts *out);

void adc_check_integrity(const ADCSample *samples, size_t count,
                         IntegrityReport *out);

#endif //EMBEDDED_SENSOR_DATA_PROCESSOR_ADC_H
