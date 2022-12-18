#include "CanSharkPro.h"

namespace dd::libcanshark::drivers {
    bool CanSharkPro::startRecording(size_t max_messages) {
        return false;
    }

    bool CanSharkPro::stopRecording() {
        return false;
    }

    bool CanSharkPro::updateFirmware(const QString &firmwareUpdateFileName) {
        return false;
    }
} // drivers