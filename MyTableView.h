//
// Created by Melchior on 13/11/17.
//

#ifndef MYTABLEVIEW_H
#define MYTABLEVIEW_H

#include <QTableView>
#include <QResizeEvent>
#include <QComboBox>
#include <QItemDelegate>
#include "MyTableModel.h"

template <typename RowModel>
class MyTableView :public QTableView {
public:
    explicit MyTableView(QWidget* parent_= nullptr);

    void setModel(QAbstractItemModel* tableModel_) override {
        Q_ASSERT(dynamic_cast<MyTableModel<RowModel>*>(tableModel_));
        QTableView::setModel(tableModel_);
    }

    MyTableModel<RowModel>* model() const {
        return static_cast<MyTableModel<RowModel>*>(QTableView::model());
    }

    void resizeEvent(QResizeEvent* event) override;

    const ColumnType<RowModel>& getColumnType(unsigned column) const {
        return static_cast<MyTableModel<RowModel>*>(model())->getColumnType(column);
    }

private:
    bool resizeWithFill(const QList<const ColumnType<RowModel>*>& columns, unsigned targetWidth, unsigned minWidth, unsigned totalFill);
};

template <typename RowModel>
class MyTableItemDelegate :public QItemDelegate {
protected:
    MyTableView<RowModel>* tableView;
public:
    explicit MyTableItemDelegate(MyTableView<RowModel>* tableView_) :tableView(tableView_) {}
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
};

#include "MyTableView.tcc"

#endif //MYTABLEVIEW_H
