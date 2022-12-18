#include "CanSharkPro.h"

namespace dd::libcanshark::drivers {

    bool CanSharkPro::openConnection(const QString &portName) {
        return false;
    }

    bool CanSharkPro::closeConnection() {
        return false;
    }

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