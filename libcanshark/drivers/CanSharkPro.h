#ifndef CANSHARK_CANSHARKPRO_H
#define CANSHARK_CANSHARKPRO_H

#include "CanShark.h"

namespace dd::libcanshark::drivers {
    class CanSharkPro : public CanShark{
    public:
        bool updateFirmware(const QString &firmwareUpdateFileName, const QString& selectedDevicePortName) override;
    };

} // drivers

#endif //CANSHARK_CANSHARKPRO_H
