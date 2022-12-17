

#ifndef CANSHARK_CANSHARKMINI_H
#define CANSHARK_CANSHARKMINI_H

#include <QObject>
#include <QSerialPort>
#include "CanShark.h"
#include "../threads/DataParserThread.h"
#include "../threads/FirmwareUpdateThread.h"

namespace dd::libcanshark::drivers {
    class CanSharkMini : public CanShark {
        Q_OBJECT
    public:
        explicit CanSharkMini(libcanshark::threads::DataParserThread* dataThread,
                              QObject* parent = nullptr);

        bool openConnection(QString const& portName) override;
        bool closeConnection() override;
        bool startRecording(size_t max_messages) override;
        bool stopRecording() override;
        bool updateFirmware(QString const& firmwareUpdateFileName) override;

    private:
        threads::FirmwareUpdateThread* ptr_firmwareUpdateThread = nullptr;

    private slots:
        void updateThreadFinished(threads::FirmwareUpdateThreadStatus status, const QString& message);
        void updateThreadProgress(const QString& message);

    };

} // drivers

#endif //CANSHARK_CANSHARKMINI_H
