
#ifndef CANSHARK_RECORDITEM_H
#define CANSHARK_RECORDITEM_H

#include <cstdint>

namespace dd::libcanshark::data {
    typedef enum CanFrameType : uint16_t {
        STANDARD = 0,
        RTR = 1
    } CanFrameType;

    typedef struct RecordItem {
        uint32_t total_size;
        CanFrameType type;
        uint64_t time;
        uint32_t id;
        uint8_t* data;
        size_t canDataLength;
        uint16_t crc16;
    } RecordItem;
}
Q_DECLARE_METATYPE(dd::libcanshark::data::RecordItem)

#endif //CANSHARK_RECORDITEM_H
