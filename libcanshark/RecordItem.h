
#ifndef CANSHARK_RECORDITEM_H
#define CANSHARK_RECORDITEM_H

#include <cstdint>

namespace dd::libcanshark::data {
    enum class CanFrameType : uint16_t {
        STANDARD = 0,
        RTR = 1
    };

    class RecordItem {
    public:
        uint32_t total_size;
        CanFrameType type;
        uint64_t time;
        uint32_t id;
        uint8_t* data;
        size_t canDataLength;
        uint16_t crc16;

        bool operator==(const RecordItem& rhs) const noexcept {
            return rhs.id == this->id;
//            if(this->canDataLength != other.canDataLength)
//                return false;
//
//            return this->total_size == other.total_size &&
//                this->type == other.type &&
//                this->time == other.time &&
//                this->id == other.id &&
//                memcmp(this->data, other.data, this->canDataLength) == 0 &&
//                this->crc16 == other.crc16;
        }

        bool operator==(const RecordItem* rhs) const noexcept {
            return rhs->id == this->id;
        }
    };
}
Q_DECLARE_METATYPE(dd::libcanshark::data::RecordItem)

#endif //CANSHARK_RECORDITEM_H
