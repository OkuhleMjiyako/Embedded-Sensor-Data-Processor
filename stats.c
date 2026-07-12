//
// Created by mjiyako on 01/07/2026.
//

#include "stats.h"
#include <math.h>

double stats_mean(const float *values, size_t count)
{
    if (count == 0) {
        return 0.0;
    }
    double sum = 0.0;
    for (const float *p = values; p < values + count; p++) {
        sum += *p;
    }
    return sum / (double)count;
}

float stats_min(const float *values, size_t count)
{
    if (count == 0) {
        return 0.0f;
    }
    float min_val = values[0];
    for (const float *p = values + 1; p < values + count; p++) {
        if (*p < min_val) {
            min_val = *p;
        }
    }
    return min_val;
}

float stats_max(const float *values, size_t count)
{
    if (count == 0) {
        return 0.0f;
    }
    float max_val = values[0];
    for (const float *p = values + 1; p < values + count; p++) {
        if (*p > max_val) {
            max_val = *p;
        }
    }
    return max_val;
}

double stats_stddev(const float *values, size_t count, double mean)
{
    if (count == 0) {
        return 0.0;
    }
    double sum_sq_diff = 0.0;
    for (const float *p = values; p < values + count; p++) {
        double diff = (double)(*p) - mean;
        sum_sq_diff += diff * diff;
    }
    double variance = sum_sq_diff / (double)count;
    return sqrt(variance);
}
