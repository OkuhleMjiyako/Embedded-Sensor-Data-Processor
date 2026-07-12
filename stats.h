//
// Created by mjiyako on 01/07/2026.
//

#ifndef EMBEDDED_SENSOR_DATA_PROCESSOR_STATS_H
#define EMBEDDED_SENSOR_DATA_PROCESSOR_STATS_H

#include <stddef.h>

double stats_mean(const float *values, size_t count);
float  stats_min(const float *values, size_t count);
float  stats_max(const float *values, size_t count);
double stats_stddev(const float *values, size_t count, double mean);

#endif //EMBEDDED_SENSOR_DATA_PROCESSOR_STATS_H
