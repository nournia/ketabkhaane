#include <uihelper.h>

void fillComboBox(QComboBox* combobox, QList<StrPair> data)
{
    combobox->clear();
    for (int i = 0; i < data.size(); i++)
        combobox->addItem(data.at(i).first, data.at(i).second);
}

void customizeTable(QTableView* table, int columns, int width, bool sort, int stretch, bool header)
{
    table->setColumnHidden(0, !header);
    table->setSortingEnabled(sort);

    for (int i = 0; i <= columns; i++)
        table->setColumnWidth(i, width);

    table->horizontalHeader()->setSectionResizeMode(stretch, QHeaderView::Stretch);
    table->verticalHeader()->setVisible(false);
    table->verticalHeader()->setDefaultSectionSize(22);
}
