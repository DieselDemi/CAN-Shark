#include <QTimer>
#include <sstream>
#include <QPushButton>
#include <QSignalMapper>
#include <QTableView>
#include <iostream>
#include "RecordTableModel.h"

namespace dd::forms::models {
    RecordTableModel::RecordTableModel(QObject *parent)
            : QAbstractTableModel(parent) {

    }

    int RecordTableModel::rowCount(const QModelIndex &parent) const {
        return static_cast<int>(this->rowsList.count());
    }

    int RecordTableModel::columnCount(const QModelIndex &parent) const {
        return 8;
    }

    QVariant RecordTableModel::data(const QModelIndex &index, int role) const {
        if (role == Qt::DisplayRole) {
            switch (index.column()) {
                //RTR
                case 0:
                    return QString("0"); //TODO: Get the RTR address
                    //Size
                case 1:
                    return QString::number(rowsList.at(index.row()).total_size);
                    //Frame Type
                case 2: {
                    switch (rowsList.at(index.row()).type) {
                        case data::STANDARD:
                            return QString("STANDARD");
                        case data::RTR:
                            return QString("REMOTE");
                        default:
                            return QString("Unknown");
                    }
                }
                    //Time
                case 3:
                    return QString::number(rowsList.at(index.row()).time);
                    //Can ID
                case 4:
                    return QString::number(rowsList.at(index.row()).id);
                    //Data
                case 5: {
                    data::RecordItem item = rowsList.at(index.row());
                    std::stringstream output;

                    for (size_t i = 0; i < item.total_size; i++) {
                        output << std::hex << item.data[i] << " ";
                    }

                    return QString::fromStdString(output.str());
                }
                    //CRC
                case 6:
                    return QString::number(rowsList.at(index.row()).crc16);

                    //Default do nothing
                default:
                {
                    return "";
                }
            }
        }
//            return QString("Row%1, Column%2")
//                    .arg(index.row() + 1)
//                    .arg(index.column() +1);

        return {};
    }

    void RecordTableModel::addRow(data::RecordItem &item) {
        beginInsertRows(QModelIndex(), static_cast<int>(rowsList.count() + 1), static_cast<int>(rowsList.count() + 1));
        rowsList.append(item);
        endInsertRows();

        auto *signalMapper = new QSignalMapper(this);
        auto qModelIndex = this->index(static_cast<int>(this->rowsList.count() - 1), 7);
        auto *button = new QPushButton("Inspect");
        ((QTableView*)this->parent())->setIndexWidget(qModelIndex, button);
        signalMapper->setMapping(button, static_cast<int>(this->rowsList.count() - 1));
        connect(button, SIGNAL(clicked(bool)), signalMapper, SLOT(map()));
        connect(signalMapper, &QSignalMapper::mappedInt, this, &RecordTableModel::inspectButtonClicked);
    }

    QVariant RecordTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
        if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
            switch (section) {
                case 0:
                    return QString("RTR");
                case 1:
                    return QString("Size");
                case 2:
                    return QString("Frame Type");
                case 3:
                    return QString("Time");
                case 4:
                    return QString("CAN ID");
                case 5:
                    return QString("Data");
                case 6:
                    return QString("CRC");
                case 7:
                    return QString("Inspect");
                default:
                    return QString("");
            }
        }

        return {};
    }

    bool RecordTableModel::insertColumns(int column, int count, const QModelIndex &parent) {
        return true;
    }

    bool RecordTableModel::removeColumns(int column, int count, const QModelIndex &parent) {
        return true;
    }

    bool RecordTableModel::moveColumns(const QModelIndex &sourceParent, int sourceColumn, int count,
                                       const QModelIndex &destinationParent, int destinationChild) {
        return true;
    }

    data::RecordItem &RecordTableModel::getRecord(int row) const {
        return const_cast<data::RecordItem &>(rowsList.at(row));
    }

    void RecordTableModel::inspectButtonClicked(int row) {
        std::cout << rowsList.at(row).id << std::endl;

    }

} // models