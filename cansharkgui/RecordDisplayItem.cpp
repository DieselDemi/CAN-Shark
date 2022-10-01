#include "RecordDisplayItem.h"
#include "ui_RecordDisplayItem.h"

#include <sstream>
#include <iostream>
#include <iomanip>

namespace dd::forms::widgets {
    RecordDisplayItem::RecordDisplayItem(dd::data::RecordItem& recordData, QWidget *parent) :
            QWidget(parent), ui(new Ui::RecordDisplayItem), data(recordData) {
        ui->setupUi(this);

        connect(this->ui->inspectButton, &QPushButton::released, this, &RecordDisplayItem::inspectButtonClicked);

        if(recordData.total_size < 16)
            return;

//        assert(recordData.total_size > 16);
        //The total_size - 16 is because there are 16 additional bytes sent in the message
        unsigned int dataByteCount = recordData.total_size - 16;

        this->ui->rtrStatusLabel->setText(ConvertIntFieldToQString(0));
        this->ui->sizeLabel->setText(ConvertIntFieldToQString(dataByteCount));
        this->ui->typeLabel->setText(ConvertTypeFieldToQString(recordData.type));
        this->ui->timeLabel->setText(ConvertIntFieldToQString(recordData.time));
        this->ui->canIDLabel->setText(ConvertIntFieldToQString(recordData.id));
        this->ui->dataLabel->setText(ConvertDataFieldToQString(recordData.data, dataByteCount));
        this->ui->crcLabel->setText(ConvertIntFieldToQString(recordData.crc16));
    }

    RecordDisplayItem::~RecordDisplayItem() {
        delete ui;
    }

    void RecordDisplayItem::inspectButtonClicked() {
        //TODO(Demi): Implement inspection window
    }

    QString RecordDisplayItem::ConvertIntFieldToQString(unsigned int field) {
        return QString::fromStdString(std::to_string(field));
    }

    QString RecordDisplayItem::ConvertTypeFieldToQString(dd::data::CanFrameType type) {
        return type == dd::data::CanFrameType::RTR ? "RTR" : "STANDARD";
    }

    QString RecordDisplayItem::ConvertDataFieldToQString(uint8_t *data, size_t size) {
        std::stringstream outStdStream;

        for(size_t i = 0; i < size || i < 4; i++) {
            outStdStream << "0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << data[i] << " ";
        }

        if(size >= 4)
            outStdStream << "...";

        return QString::fromStdString(outStdStream.str());
    }
} // dd::forms::widgets
