/*******************************************
 * File Name: osdshortcutdialog.cpp
 * Date: 2021-03-07
 * Author: Bob.Zhang
 *
 * Description: Implementation of shortcut
 * keys dialog.
 *******************************************/

#include "osdconfig.h"
#include "osdshortcutdialog.h"
#include "ui_osdshortcutdialog.h"
#include <QHeaderView>

OSDShortcutDialog::OSDShortcutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OSDShortcutDialog)
{
    ui->setupUi(this);

    ui->tableWidget->verticalHeader()->hide();
    ui->tableWidget->horizontalHeader()->
            setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->
            setSectionResizeMode(1, QHeaderView::Stretch);

    QTableWidgetItem *item;
    auto keyMap = OSDConfig::getInstance().getKeySequenceMapRef();
    auto itr = keyMap.begin();
    int row = 0;
    ui->tableWidget->setRowCount(keyMap.size());
    for(; itr != keyMap.end(); ++itr, ++row)
    {
        item = new QTableWidgetItem(itr.value().first.toString());
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        ui->tableWidget->setItem(row, 0, item);

        item = new QTableWidgetItem(itr.value().second);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        ui->tableWidget->setItem(row, 1, item);
    }
}

OSDShortcutDialog::~OSDShortcutDialog()
{
    delete ui;
}
