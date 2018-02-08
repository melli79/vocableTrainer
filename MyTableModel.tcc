//
// Created by Melchior on 07/12/17.
//

#include "MyTableModel.h"

template <typename RowModel>
int MyTableModel<RowModel>::getIndex(const ColumnType<RowModel> &type) const {
    for(unsigned i=0; i<columns.size(); ++i) {
        if(*columns[i]==type) return i;
    }
    return -1;
}

template <typename RowModel>
QVariant MyTableModel<RowModel>::data(const QModelIndex &index, int role) const {
    if(Qt::DisplayRole==role||Qt::EditRole==role) return getValue(index.row(),index.column());
    return QVariant();
}

template <typename RowModel>
QVariant MyTableModel<RowModel>::getValue(int row, int column) const {
    if(row<0||row>=vocables.size()||column<0||column>=columns.size()) return QVariant();
    return (*columns[column]->getter)(vocables[row]);
}

template <typename RowModel>
QVariant MyTableModel<RowModel>::headerData(int index, Qt::Orientation orientation, int role) const {
    if(role==Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            if (index >= 0 && index < columns.size()) return columns[index]->name;
        } else if (orientation == Qt::Vertical) {
            return QString("%1").arg(index+1);
        }
    }
    return QVariant();
}

template <typename RowModel>
bool MyTableModel<RowModel>::setData(const QModelIndex& index, const QVariant& value, int role) {
    if(role==Qt::EditRole) {
        return setValue(index.row(),index.column(),value);
    }
    return QAbstractItemModel::setData(index, value, role);
}

template <typename RowModel>
bool MyTableModel<RowModel>::setValue(int row, int column, const QVariant& value) {
    if(row<0||row>=vocables.size()||column<0||column>=columns.size()) return false;
    bool modified= (*columns[column]->setter)(vocables[row], value.toString());
    if(modified)  setModified(true);
    return modified;
}

template <typename RowModel>
bool MyTableModel<RowModel>::insertRows(int position, int num, const QModelIndex& index) {
    if(position<0||num<=0) return false;
    beginInsertRows(index,position,position+num-1);
    if(position>=vocables.size()) {
        for(unsigned i=0; i<num;i++) vocables.append(RowModel());
    }
    else {
        for(unsigned i=0; i<num;i++) vocables.insert(position, RowModel());
    }
    endInsertRows();
    return true;
}

template <typename RowModel>
bool MyTableModel<RowModel>::removeRows(int position, int num, const QModelIndex&) {
    if(position<0||position>=vocables.size()) return false;
    beginRemoveRows(QModelIndex(), position, position+num-1);
    for(unsigned i=0; i<num; i++)  vocables.removeAt(position);
    endRemoveRows();
    return true;
}

template<typename _RowModel>
Qt::ItemFlags MyTableModel<_RowModel>::flags(const QModelIndex &index) const {
    const ColumnType<RowModel>& column= getColumnType(index.column());
    return (column.type==ColumnType<RowModel>::None? Qt::ItemFlags(0): Qt::ItemIsEditable) | QAbstractTableModel::flags(index);
}
