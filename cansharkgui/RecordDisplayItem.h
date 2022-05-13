#ifndef CANSHARK_RECORDDISPLAYITEM_H
#define CANSHARK_RECORDDISPLAYITEM_H

#include <QWidget>

#include "RecordItem.h"

namespace dd::forms::widgets {
    QT_BEGIN_NAMESPACE
    namespace Ui { class RecordDisplayItem; }
    QT_END_NAMESPACE

    class RecordDisplayItem : public QWidget {
        Q_OBJECT

    public:
        explicit RecordDisplayItem(dd::data::RecordItem& recordData, QWidget *parent = nullptr);

        ~RecordDisplayItem() override;

    private slots:
        void inspectButtonClicked();

    private:
        static QString ConvertIntFieldToQString(unsigned int field);
        static QString ConvertTypeFieldToQString(dd::data::CanFrameType type);
        static QString ConvertDataFieldToQString(uint8_t* data, size_t size);
        Ui::RecordDisplayItem *ui;
        dd::data::RecordItem data;
    };
} // dd::forms::widgets

#endif //CANSHARK_RECORDDISPLAYITEM_H
