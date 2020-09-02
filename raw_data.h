#ifndef DAQMT_TC_SERVER_DATA_H
#define DAQMT_TC_SERVER_DATA_H

struct raw_data {
    unsigned int   magic_word;
    unsigned int   data_length;
    unsigned int   trigger_count;
    unsigned short event_data[4*1024]; /* 4: number of channel.  1024: number of samples */
    unsigned int   footer;
};

extern struct raw_data create_data(unsigned int trigger_count);

#endif
