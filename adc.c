//
// Created by mjiyako on 01/07/2026.
//

#include "adc.h"
#include "stats.h"
#include <stdlib.h>

void adc_raw_to_voltage(ADCSample *sample)
{
    sample->voltage = (float)((sample->raw_value / ADC_MAX_RAW_VALUE) * ADC_VREF);
}

void adc_compute_channel_stats(const ADCSample *samples, size_t count,
                               uint8_t channel, ChannelStats *out)
{
    out->channel = channel;

    size_t channel_count = 0;
    for (const ADCSample *p = samples; p < samples + count; p++) {
        if (p->channel_id == channel) {
            channel_count++;
        }
    }

    out->sample_count = channel_count;
    if (channel_count == 0) {
        out->mean_voltage = 0.0;
        out->min_voltage = 0.0f;
        out->max_voltage = 0.0f;
        out->stddev_voltage = 0.0;
        return;
    }

    float *voltages = malloc(channel_count * sizeof(float));
    if (voltages == NULL) {
        out->mean_voltage = 0.0;
        out->min_voltage = 0.0f;
        out->max_voltage = 0.0f;
        out->stddev_voltage = 0.0;
        out->sample_count = 0;
        return;
    }

    size_t j = 0;
    for (const ADCSample *p = samples; p < samples + count; p++) {
        if (p->channel_id == channel) {
            voltages[j] = p->voltage;
            j++;
        }
    }

    out->mean_voltage   = stats_mean(voltages, channel_count);
    out->min_voltage    = stats_min(voltages, channel_count);
    out->max_voltage    = stats_max(voltages, channel_count);
    out->stddev_voltage = stats_stddev(voltages, channel_count, out->mean_voltage);

    free(voltages);
}

void adc_detect_faults(const ADCSample *samples, size_t count,
                       uint8_t channel, FaultCounts *out)
{
    out->channel = channel;
    out->overvoltage_count = 0;
    out->undervoltage_count = 0;
    out->status_fault_count = 0;

    for (const ADCSample *p = samples; p < samples + count; p++) {
        if (p->channel_id != channel) {
            continue;
        }
        if (p->voltage > OVERVOLTAGE_THRESHOLD) {
            out->overvoltage_count++;
        }
        if (p->voltage < UNDERVOLTAGE_THRESHOLD) {
            out->undervoltage_count++;
        }
        if (p->status_flags & STATUS_FLAG_FAULT_BIT) {
            out->status_fault_count++;
        }
    }
}

void adc_check_integrity(const ADCSample *samples, size_t count,
                         IntegrityReport *out)
{
    out->gap_count = 0;
    out->missing_records = 0;
    out->first_gap_after_seq = 0;

    if (count < 2) {
        return;
    }

    for (size_t i = 1; i < count; i++) {
        uint32_t prev_seq = (samples + i - 1)->sequence_number;
        uint32_t curr_seq = (samples + i)->sequence_number;

        if (curr_seq != prev_seq + 1) {
            out->gap_count++;
            if (curr_seq > prev_seq) {
                out->missing_records += (curr_seq - prev_seq - 1);
            }
            if (out->first_gap_after_seq == 0) {
                out->first_gap_after_seq = prev_seq;
            }
        }
    }
}