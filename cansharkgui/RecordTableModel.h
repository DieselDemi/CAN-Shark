#ifndef CANSHARK_RECORDTABLEMODEL_H
#define CANSHARK_RECORDTABLEMODEL_H

#include <QAbstractTableModel>
#include <RecordItem.h>
#include "FormInspect.h"

namespace dd::forms::models {
    class RecordTableModel : public QAbstractTableModel {
    public:
        explicit RecordTableModel(QObject* parent);

        [[nodiscard]] int rowCount(const QModelIndex &parent) const override;

        [[nodiscard]] int columnCount(const QModelIndex &parent) const override;

        bool insertColumns(int column, int count, const QModelIndex &parent) override;

        bool removeColumns(int column, int count, const QModelIndex &parent) override;

        bool
        moveColumns(const QModelIndex &sourceParent, int sourceColumn, int count, const QModelIndex &destinationParent,
                    int destinationChild) override;

        [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;

        [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

        void addRow(libcanshark::data::RecordItem& item);
        bool updateRow(libcanshark::data::RecordItem& item);
        bool hasRow(const libcanshark::data::RecordItem& item);
        void clearRows();

        [[nodiscard]] libcanshark::data::RecordItem& getRecord(int row) const;

    private slots:
        void inspectButtonClicked(int row);

    private:
        QList<libcanshark::data::RecordItem> rowsList;
        FormInspect* m_inspectForm = nullptr;
    };
} // models

#endif //CANSHARK_RECORDTABLEMODEL_H
