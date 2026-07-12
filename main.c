#include <stdio.h>
#include <stdlib.h>
#include "adc.h"
#include "IO.h"

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <adc_sensor_log.bin>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *input_filename = argv[1];

    ADCSample *samples = NULL;
    size_t sample_count = 0;
    FileHeader header;

    if (io_read_file(input_filename, &samples, &sample_count, &header) != 0) {
        return EXIT_FAILURE;
    }

    printf("Loaded %zu records from '%s'.\n", sample_count, input_filename);

    ChannelStats channel_stats[4];
    FaultCounts fault_counts[4];

    for (uint8_t ch = 0; ch < 4; ch++) {
        adc_compute_channel_stats(samples, sample_count, ch, &channel_stats[ch]);
        adc_detect_faults(samples, sample_count, ch, &fault_counts[ch]);
    }

    IntegrityReport integrity;
    adc_check_integrity(samples, sample_count, &integrity);

    const char *output_filename = "results.txt";
    if (io_write_results(output_filename, &header, channel_stats,
                         fault_counts, &integrity) != 0) {
        free(samples);
        return EXIT_FAILURE;
    }

    printf("Results written to '%s'.\n", output_filename);

    free(samples);
    samples = NULL;

    return EXIT_SUCCESS;
}
