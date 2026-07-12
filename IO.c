//
// Created by mjiyako on 01/07/2026.
//

#include "IO.h"
#include <stdio.h>
#include <stdlib.h>

#pragma pack(push, 1)
typedef struct {
    float    timestamp;
    uint8_t  channel_id;
    uint16_t raw_value;
    int16_t  temperature;
    uint8_t  status_flags;
    uint32_t sequence_number;
    uint8_t  reserved[2];
} RawRecord;
#pragma pack(pop)


int io_read_file(const char *filename, ADCSample **out_samples,
                 size_t *out_count, FileHeader *out_header)
{
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        fprintf(stderr, "Error: could not open '%s' for reading.\n", filename);
        return 1;
    }


    FileHeader header;
    size_t items_read = fread(&header, sizeof(FileHeader), 1, fp);
    if (items_read != 1) {
        fprintf(stderr, "Error: file '%s' is too short to contain a valid header.\n",
                filename);
        fclose(fp);
        return 1;
    }

    if (header.magic != ADC_MAGIC) {
        fprintf(stderr, "Error: bad magic number 0x%08X (expected 0x%08X). "
                        "This does not look like a valid ADC log file.\n",
                header.magic, ADC_MAGIC);
        fclose(fp);
        return 1;
    }
    if (header.version != ADC_EXPECTED_VERSION) {
        fprintf(stderr, "Error: unsupported file version %u (expected %u).\n",
                header.version, ADC_EXPECTED_VERSION);
        fclose(fp);
        return 1;
    }
    if (header.channel_count != ADC_EXPECTED_CHANNEL_COUNT) {
        fprintf(stderr, "Error: unexpected channel count %u (expected %u).\n",
                header.channel_count, ADC_EXPECTED_CHANNEL_COUNT);
        fclose(fp);
        return 1;
    }
    if (header.record_count == 0) {
        fprintf(stderr, "Error: header reports zero records - nothing to process.\n");
        fclose(fp);
        return 1;
    }


    ADCSample *samples = malloc(header.record_count * sizeof(ADCSample));
    if (samples == NULL) {
        fprintf(stderr, "Error: malloc failed - could not allocate memory for "
                        "%u records.\n", header.record_count);
        fclose(fp);
        return 1;
    }


    for (uint32_t i = 0; i < header.record_count; i++) {
        RawRecord raw;
        items_read = fread(&raw, sizeof(RawRecord), 1, fp);
        if (items_read != 1) {
            fprintf(stderr, "Error: file truncated - expected %u records but "
                            "only found %u.\n", header.record_count, i);
            free(samples);
            fclose(fp);
            return 1;
        }


        ADCSample *dest = samples + i;
        dest->timestamp       = raw.timestamp;
        dest->channel_id      = raw.channel_id;
        dest->raw_value       = raw.raw_value;
        dest->temperature     = raw.temperature;
        dest->status_flags    = raw.status_flags;
        dest->sequence_number = raw.sequence_number;
        adc_raw_to_voltage(dest);
    }

    fclose(fp);

    *out_samples = samples;
    *out_count   = header.record_count;
    if (out_header != NULL) {
        *out_header = header;
    }
    return 0;
}

int io_write_results(const char *filename, const FileHeader *header,
                     const ChannelStats channel_stats[4],
                     const FaultCounts fault_counts[4],
                     const IntegrityReport *integrity)
{
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        fprintf(stderr, "Error: could not open '%s' for writing.\n", filename);
        return 1;
    }


    fprintf(fp, " ADC Sensor Log - Analysis Results\n");

    fprintf(fp, "Source file summary:\n");
    fprintf(fp, "  Record count   : %u\n", header->record_count);
    fprintf(fp, "  Sample rate    : %u Hz\n", header->sample_rate_hz);
    fprintf(fp, "  Channel count  : %u\n\n", header->channel_count);

    fprintf(fp, "Per-channel voltage statistics:\n");
    for (int ch = 0; ch < 4; ch++) {
        const ChannelStats *cs = &channel_stats[ch];
        fprintf(fp, "Channel %d (%zu samples)\n", ch, cs->sample_count);
        fprintf(fp, "  Mean voltage   : %.6f V\n", cs->mean_voltage);
        fprintf(fp, "  Min voltage    : %.6f V\n", cs->min_voltage);
        fprintf(fp, "  Max voltage    : %.6f V\n", cs->max_voltage);
        fprintf(fp, "  Std deviation  : %.6f V\n\n", cs->stddev_voltage);
    }

    fprintf(fp, "Fault detection:\n");
    for (int ch = 0; ch < 4; ch++) {
        const FaultCounts *fc = &fault_counts[ch];
        fprintf(fp, "Channel %d\n", ch);
        fprintf(fp, "  Overvoltage (>3.0V)   : %zu\n", fc->overvoltage_count);
        fprintf(fp, "  Undervoltage (<0.3V)  : %zu\n", fc->undervoltage_count);
        fprintf(fp, "  Status fault bit set  : %zu\n\n", fc->status_fault_count);
    }

    fprintf(fp, "Sampling integrity:\n");
    fprintf(fp, "  Gaps detected      : %zu\n", integrity->gap_count);
    fprintf(fp, "  Missing records    : %zu\n", integrity->missing_records);
    if (integrity->gap_count > 0) {
        fprintf(fp, "  First gap after seq: %u\n", integrity->first_gap_after_seq);
    } else {
        fprintf(fp, "  No gaps detected - sequence numbers are continuous.\n");
    }

    fclose(fp);
    return 0;
}