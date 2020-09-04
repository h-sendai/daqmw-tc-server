#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

#include <arpa/inet.h> /* for htons etc */
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "raw_data.h"

unsigned short create_sin_data(int w, int ch, int trigger_count)
{
    double x;
    int y;

    double pi     = 3.1416;
    double amp    = 2000.0;
    double offset = 2048;
    double a      = 0.1*trigger_count;

    x = (double) w / 1024.0 * 2.0 * pi;
    y = (unsigned short) amp * sin(x - a) + offset;

    return y;
}

struct raw_data create_data(unsigned int trigger_count)
{
    struct raw_data raw_data;

    raw_data.magic_word    = htonl(0x01234567);
    raw_data.data_length   = htonl(4*2*1024);
    raw_data.trigger_count = htonl(trigger_count);
    raw_data.footer        = htonl(0x89abcdef);

    unsigned short n_ch = 4;
    int n_sample        = 1024;

    unsigned short data = 0;

    for (int sample_num = 0; sample_num < n_sample; ++sample_num) { 
        for (unsigned char ch = 0; ch < n_ch; ++ch) {
            unsigned short tmp = (ch << 12);
            /* change this data value to write another graph line (sine curve etc) */
            if (strcmp(data_type, "sin") == 0) { /* data_type: sin */
                data = create_sin_data(sample_num, ch, trigger_count);
            }
            else { /* data_type: const */
                /* 
                 * trivial data (horizontal graph line)
                 * increase 128 each triggers'
                 * 0 for trigger_count 0
                 * 128 for trigger_count 1
                 * 256 for trigger_count 2
                 * :
                 */
                data = (trigger_count * 128) % 4096;
            }
            tmp = htons(tmp + data);
            raw_data.event_data[sample_num*n_ch + ch] = tmp;
        }
    }

    /* 
     * return struct
     * about returning struct
     * see "The C Programming Language: Chapter 6, Section 2"
     * "Copy and assignment include passing arguments to functions and returning
     * values from function as well"
     */
    return raw_data;
}

#if 0
int main(int argc, char *argv[])
{
    struct raw_data raw_data;

    FILE *fp = fopen("data", "w");
    for (int i = 0; i < 8192; ++i) {
        raw_data = create_data(i);
        fwrite(&raw_data, 1, sizeof(raw_data), fp);
    }
    
    return 0;
}
#endif
