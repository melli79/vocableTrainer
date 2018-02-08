//
// Created by Melchior on 07/12/17.
//

#ifndef MYTABLEMODEL_H
#define MYTABLEMODEL_H

#include <QList>
#include <QAbstractTableModel>

template <typename M>
class ColumnType;

template <typename _RowModel>
class MyTableModel :public QAbstractTableModel {
public:
    typedef _RowModel RowModel;
protected:
    QList<const ColumnType<RowModel>*>  columns;
    QList<RowModel> vocables;
    bool modified;
public:
    template <class Container>
    explicit MyTableModel(const Container&  columns_) :columns(columns_), modified(false) {
    }

    inline void clear() {
        layoutAboutToBeChanged();
        vocables.clear();
        layoutChanged();
    }

    int rowCount(const QModelIndex& parent= QModelIndex()) const override { return vocables.size(); }
    int columnCount(const QModelIndex& parent= QModelIndex()) const override { return columns.size(); }
    const QList<const ColumnType<RowModel>*>& getColumns() const { return columns; };
    QVariant headerData(int index, Qt::Orientation orientation, int role= Qt::DisplayRole) const override;
    int getIndex(const ColumnType<RowModel>& type) const;
    QVariant data(const QModelIndex& index, int role= Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    const ColumnType<RowModel>& getColumnType(unsigned column) const { return *columns[column]; }
    QVariant getValue(int row, int column) const;
    bool setValue(int row, int column, const QVariant& value);
    bool insertRows(int position, int num, const QModelIndex& index= QModelIndex()) override;
    bool removeRows(int position, int num, const QModelIndex& index= QModelIndex()) override;

    bool isModified() const { return modified; }
    void setModified(bool modified_) { this->modified= modified_; }
};

template <typename M>
class ColumnType {
public:
    enum EditorType { None, Default, ComboBox };
    typedef QString (*Getter)(const M& m);
    typedef bool (*Setter)(M& m, const QString& value);

    QString name;
    Getter getter;
    Setter setter;
    EditorType type;
    struct SizePolicy {
        unsigned min;
        int max;
        explicit SizePolicy(unsigned minWidth_, int maxWidth_=-1) :min(minWidth_), max(maxWidth_) {
            Q_ASSERT(max<0||max>=min);
        }
    } width;

    ColumnType(QString name_, Getter getter_, Setter setter_, EditorType editorType_= None, SizePolicy width_= SizePolicy(0))
            :name(std::move(name_)), getter(getter_), setter(setter_),type(editorType_),width(width_) {
        Q_ASSERT(getter_);
        Q_ASSERT(setter_);
    }

    virtual ~ColumnType() = default;

    bool operator==(const ColumnType& column) const {
        return name==column.name&& getter==column.getter&& setter==column.setter;
    }

    // override this method if you use ComboBox
    virtual QStringList getComboBoxItems() const { return QStringList(); }
};

#include "MyTableModel.tcc"

#endif //MYTABLEMODEL_H
