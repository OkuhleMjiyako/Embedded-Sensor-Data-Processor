**Embedded Sensor Data Processor**

A modular C program that parses and analyses a binary ADC (Analog-to-Digital Converter) sensor log file.

The program reads a fixed-format binary log of 4-channel ADC samples, converts raw readings to voltages, computes per-channel statistics, detects out-of-range and fault-flagged samples, checks the sample sequence for gaps, and writes a human-readable report.


****Why the code is split this way
********

stats holds pure, reusable numeric functions that know nothing about ADCs they just operate on arrays of float.
adc holds the domain logic (what counts as overvoltage, what a "fault" is, how raw counts become volts) and uses stats internally.
IO owns everything to do with the binary file format: it is the only place that knows about byte layout, packing, and the file header. It converts the file's private on-disk RawRecord layout into the public ADCSample struct that the rest of the program works with.
main just wires the three modules together it contains no parsing or analysis logic itself.

**How to Open**

Open the project folder directly (CLion will detect CMakeLists.txt and configure it automatically). Before running, set the Program arguments field to the path of the log file, and make sure Working directory is a folder (not the .bin file itself).

**What the analysis does**

For each of the 4 channels:


Voltage conversion: adc_raw_to_voltage maps the raw 12-bit ADC code to a voltage using voltage = (raw_value / 4095.0) * 3.3.
Statistics (adc_compute_channel_stats): mean, min, max, and standard deviation of voltage, computed via the stats module.
Fault detection (adc_detect_faults): counts samples that are:

Overvoltage (> 3.0 V)
Undervoltage (< 0.3 V)
Flagged with the hardware fault bit (status_flags & 0x01)



Sequence integrity (adc_check_integrity): scans sequence_number across all channels combined to detect gaps (missing records) in the log, reporting the number of gaps, total missing records, and the sequence number after which the first gap occurs.


Results for all four channels, plus the integrity summary, are written to results.txt.
