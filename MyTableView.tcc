//
// Created by Melchior on 13/11/17.
//

#include "MyTableView.h"
#include <QLineEdit>

template <typename RowModel>
void MyTableView<RowModel>::resizeEvent(QResizeEvent* event) {
    auto* tableModel= dynamic_cast<const MyTableModel<RowModel>*>(model());
    if(tableModel== nullptr) {
        QAbstractItemView::resizeEvent(event);
        return;
    }
    unsigned minWidth=0;
    int maxWidth=0;
    const QList<const ColumnType<RowModel>*>& columns = tableModel->getColumns();
    foreach(const ColumnType<RowModel>* column, columns) {
        minWidth += column->width.min;
        if(maxWidth<0) {
            if(column->width.max<0)  maxWidth += column->width.max;
        }else if(column->width.max<0) {
            maxWidth= column->width.max;
        }else maxWidth += column->width.max;
    }
    if(event->size().width()<minWidth) {
        event->ignore();
        return;
    }
    if(maxWidth>=0&& event->size().width()>maxWidth) {
        event->ignore();
        return;
    }
    if(maxWidth<0) {
        if(resizeWithFill(columns, (unsigned)(event->size().width()), minWidth, (unsigned) abs(maxWidth))) event->accept();
        return;
    }
    double stretch= 0;
    if(maxWidth-minWidth>0) stretch= (event->size().width()-minWidth)/((double)maxWidth-minWidth);
    unsigned usedWidth=0;  double sumWidth=0;
    unsigned i=0;
    for (const auto* column :columns) {
        const typename ColumnType<RowModel>::SizePolicy& policy = column->width;
        auto width= (unsigned)((sumWidth+= policy.min +(policy.max-policy.min)*stretch)+0.5-usedWidth);
        setColumnWidth(i++,width);  usedWidth += width;
    }
}

template <typename RowModel>
bool MyTableView<RowModel>::resizeWithFill(const QList<const ColumnType<RowModel>*>& columns, unsigned targetWidth, unsigned minWidth, unsigned totalFill) {
    double stretch= (targetWidth-minWidth)/(double)totalFill;
    unsigned usedWidth=0;  double sumWidth=0;
    unsigned i=0;
    for(const auto& column :columns) {
        unsigned width= column->width.min;  sumWidth+= width;
        int max= column->width.max;
        if(max<0) width += (unsigned)((sumWidth+= -max*stretch)+0.5-width-usedWidth);
        setColumnWidth(i++,width);  usedWidth += width;
    }
    return true;
}

template <typename RowModel>
MyTableView<RowModel>::MyTableView(QWidget* parent_) :QTableView(parent_) {
        setItemDelegate(new MyTableItemDelegate<RowModel>(this));
}

template <typename RowModel>
QWidget* MyTableItemDelegate<RowModel>::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    const ColumnType<RowModel>& column= tableView->getColumnType(index.column());
    switch (column.type) {
    case ColumnType<RowModel>::ComboBox: {
        //auto comboBoxOption = QStyleOptionViewItem(option);
        //comboBoxOption.type = QStyleOption::SO_ComboBox;
        auto* comboBox = new QComboBox(parent);
        comboBox->setEditable(true);
        return comboBox;
    }
    case ColumnType<RowModel>::Default:
        return new QLineEdit(parent);
    case ColumnType<RowModel>::None:
        break;
    }
    return nullptr;
}

template<typename RowModel>
void MyTableItemDelegate<RowModel>::setEditorData(QWidget* editor, const QModelIndex& index) const {
    const ColumnType<RowModel>& column = tableView->getColumnType(index.column());
    switch (column.type) {
    case ColumnType<RowModel>::ComboBox:
        if(auto* comboBox= dynamic_cast<QComboBox*>(editor)) {
            comboBox->clear();
            comboBox->addItems(column.getComboBoxItems());
            comboBox->setCurrentText(tableView->model()->getValue(index.row(),index.column()).toString());
        }
        break;
    case ColumnType<RowModel>::Default:
        if(auto* lineEditor= dynamic_cast<QLineEdit*>(editor)) {
            lineEditor->setText(tableView->model()->getValue(index.row(),index.column()).toString());
        }
        break;
    case ColumnType<RowModel>::None:
        break;
    }
}

template<typename RowModel>
void MyTableItemDelegate<RowModel>::setModelData(QWidget* editor, QAbstractItemModel* model,
                                                 const QModelIndex& index) const {
    ColumnType<RowModel> column = tableView->getColumnType(index.column());
    if(auto* tableModel= dynamic_cast<MyTableModel<RowModel>*>(model)) {
        switch (column.type) {
        case ColumnType<RowModel>::ComboBox:
            if(auto* comboBox= dynamic_cast<QComboBox*>(editor)) {
                tableModel->setValue(index.row(),index.column(),comboBox->currentText());
            }
        case ColumnType<RowModel>::Default:
            if(auto* lineEditor= dynamic_cast<QLineEdit*>(editor)) {
                tableModel->setValue(index.row(),index.column(),lineEditor->text());
            }
            break;
        case ColumnType<RowModel>::None:
            break;
        }
    }
}
