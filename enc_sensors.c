#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>

#define GPS_ONLY 0
#define GPS_AND_BATTERY 1

#define OPERARION_MODE GPS_AND_BATTERY

#define MAX_FILENAME 4096
#define CMD_LENGTH 4096


int main(int argc, char *argv[])
{
    if (argc < 1)
    {
        fprintf(stderr, "Syntax: %s input.csv\n", argv[0]);
        return -1;
    }

    char *input_file = argv[1];
    // csv input
    FILE *fin = fopen(input_file, "r");

    // write the payload to .bin file
    char raw_filename[MAX_FILENAME];
    sprintf(raw_filename, "/tmp/enc_sensors.bin.%d", getpid ());
    FILE *fout = fopen(raw_filename, "w");

    uint32_t time_stamp;
    uint8_t soc;
    float lat, lon, vbatt, abatt;
    // skip the first line
    while (getc(fin) != '\n');

    while (fscanf(fin, "%u,%f,%f,%f,%f,%hhu\n", &time_stamp, &lat, &lon, &vbatt, &abatt, &soc) > 0)
    {
        // printf("%u,%f,%f,%f,%f,%hhu\n", time_stamp, lat, lon, vbatt, abatt, soc);
        fwrite(&time_stamp, 4, 1, fout);
        fwrite(&lat, 4, 1, fout);
        fwrite(&lon, 4, 1, fout);
        fwrite(&vbatt, 4, 1, fout);
        fwrite(&abatt, 4, 1, fout);
        fwrite(&soc, 1, 1, fout);
    }
    fclose(fin);
    fclose(fout);
    unlink(input_file);

    // compress the file
    char compressed_payload_filename[MAX_FILENAME];
    sprintf(compressed_payload_filename, "/tmp/enc_sensors.paq.%d", getpid ());
    char cmd_compress[CMD_LENGTH];
    sprintf(cmd_compress, "paq8px -3 %s %s", raw_filename, compressed_payload_filename);
    system(cmd_compress);
    unlink(raw_filename);

    // sent over uux
    char cmd_string[MAX_FILENAME];
    sprintf(cmd_string, "uux -r - gw\\!dec_sensors < %s", compressed_payload_filename);
    printf("%s\n", cmd_string);
    system(cmd_string);

    unlink(compressed_payload_filename);
}