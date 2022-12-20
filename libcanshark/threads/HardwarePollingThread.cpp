
#include "HardwarePollingThread.h"
#include <CanShark.h>

namespace dd::libcanshark::threads {
    /**
     * Basic thread
     */
    void HardwarePollingThread::run() {
        while (isRunning()) {
            bool changed = false;

            auto checkList = drivers::CanShark::getAvailablePorts();

            if (checkList.size() != currentPorts.size()) {
                changed = true;
            } else {
                for (qsizetype i = 0; i < checkList.size(); i++) {
                    if (std::get<1>(checkList[i]) != std::get<1>(currentPorts[i]))
                        changed = true;
                }
            }

            if (changed) {
                emit hardwareChanged();
            }

            currentPorts = drivers::CanShark::getAvailablePorts();
            QThread::msleep(100);
        }
    }
} // threads