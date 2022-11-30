

#ifndef CANSHARK_CANSHARKMINI_H
#define CANSHARK_CANSHARKMINI_H

#include <QObject>
#include <QSerialPort>
#include "../DataParserThread.h"
#include "CanShark.h"

namespace dd::libcanshark::drivers {
    class CanSharkMini : public CanShark {
    public:
        explicit CanSharkMini(libcanshark::threads::DataParserThread* dataThread,
                              QObject* parent = nullptr);

        bool openConnection(QString const& portName) override;
        bool closeConnection() override;
        bool startRecording(size_t max_messages) override;
        bool stopRecording() override;
        bool updateFirmware(QString const& firmwareUpdateFileName) override;
    };

} // drivers

#endif //CANSHARK_CANSHARKMINI_H
