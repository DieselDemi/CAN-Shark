
#ifndef CANSHARK_RECORDITEM_H
#define CANSHARK_RECORDITEM_H

#include <cstdint>

namespace dd::data {

    typedef enum CanFrameType {
        STANDARD,
        RTR
    } CanFrameType;

    typedef struct RecordItem {
        uint32_t total_size;
        CanFrameType type;
        uint64_t time;
        uint32_t id;
        uint8_t data[8];
        uint16_t crc16;
    } RecordItem;
}

#endif //CANSHARK_RECORDITEM_H
