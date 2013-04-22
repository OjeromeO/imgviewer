#include "ImgDescription.h"
#include <iostream>



ImgDescription::ImgDescription(QWidget * parent, QString & imgname) : QWidget(parent)
{
    if (imgname.isEmpty())
        return;
    
    QImage image(imgname);
    QFileInfo imgInfo(imgname);
    
    if (image.isNull())
    {
        QMessageBox::critical(this, "Image Viewer", "Can't load "+imgInfo.fileName());
        return;
    }
    
    imgName = imgInfo.fileName();
    
    groupColumn1 = -1;
    groupColumn2 = -1;
    
    QSplitter * splitter = new QSplitter(this);
    
    imageTab = new QTabWidget(this);
    
    imageTab->setMinimumHeight(500);
    imageTab->setMinimumWidth(500);
    
    descrContainer = new QWidget(this);
    
    kwContainer = new QWidget(descrContainer);
    keyWord = new QLineEdit(/*"define a keyword", */kwContainer);
    
    keyWord->setMaxLength(100);
    keyWord->setFixedWidth(350);
    QHBoxLayout * layoutk = new QHBoxLayout;
    layoutk->addWidget(new QLabel("<strong>Keyword: </strong>", kwContainer));
    layoutk->addWidget(keyWord);
    layoutk->addStretch(0);
    
    kwContainer->setLayout(layoutk);
    
    scrollArea1 = new QScrollArea(imageTab);
    scrollArea2 = new QScrollArea(imageTab);
    scrollArea3 = new QScrollArea(descrContainer);
    scrollArea4 = new QScrollArea(descrContainer);
    
    tableContainer = new QWidget(scrollArea4);
    
    hstgrmLabel = new Histogram(scrollArea3);
    
    scrollArea3->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    hstgrmLabel->setFixedSize(400, 1350);
    scrollArea3->setFixedWidth(400);
    scrollArea3->setMinimumHeight(330);
    
    imageLabel = new QLabel(scrollArea1);
    bwImageLabel = new QLabel(scrollArea2);
    
    QTableWidget * tab[4];
    
    for(int i = 0;i<4;i++)
    {
        tab[i] = new QTableWidget(4, 0, tableContainer);
        
        tab[i]->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tab[i]->setVerticalHeaderItem(0, new QTableWidgetItem("Average"));
        tab[i]->setVerticalHeaderItem(1, new QTableWidgetItem("Light colours"));
        tab[i]->setVerticalHeaderItem(2, new QTableWidgetItem("Dark colours"));
        tab[i]->setVerticalHeaderItem(3, new QTableWidgetItem("Median colours"));
        
        tab[i]->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tab[i]->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }
    
    bwCompTable = tab[0];
    redCompTable = tab[1];
    greenCompTable = tab[2];
    blueCompTable = tab[3];
    
    scrollArea1->setWidget(imageLabel);
    scrollArea2->setWidget(bwImageLabel);
    scrollArea3->setWidget(hstgrmLabel);
    scrollArea4->setWidget(tableContainer);
    
    imageTab->addTab(scrollArea1, "Standard image");
    imageTab->addTab(scrollArea2, "Black and White image");
    imageTab->setTabPosition(QTabWidget::South);

    QHBoxLayout * layout = new QHBoxLayout;
    
    splitter->addWidget(imageTab);
    splitter->addWidget(descrContainer);
    
    layout->addWidget(splitter);
    
    QVBoxLayout * layout2 = new QVBoxLayout;
    layout2->addWidget(scrollArea3, 0, Qt::AlignCenter);
    layout2->addWidget(kwContainer);
    layout2->addWidget(scrollArea4);
    
    QVBoxLayout * layout3 = new QVBoxLayout;
    layout3->addWidget(new QLabel("<strong><font color=black>Euclidean distances between gray levels histograms :</font></strong>", tableContainer));
    layout3->addWidget(bwCompTable);
    layout3->addWidget(new QLabel("<strong><font color=black>Euclidean distances between red component histograms :</font></strong>", tableContainer));
    layout3->addWidget(redCompTable);
    layout3->addWidget(new QLabel("<strong><font color=black>Euclidean distances between green component histograms :</font></strong>", tableContainer));
    layout3->addWidget(greenCompTable);
    layout3->addWidget(new QLabel("<strong><font color=black>Euclidean distances between blue component histograms :</font></strong>", tableContainer));
    layout3->addWidget(blueCompTable);
    layout3->addWidget(new QLabel("<font color=black size=2>Note: euclidean distances are between 0 and 1000.</font>", tableContainer));
    
    tableContainer->setLayout(layout3);
    
    descrContainer->setLayout(layout2);
    
    tableContainer->adjustSize();
    tableContainer->setFixedHeight(750);
    
    bwCompTable->setFixedWidth(120);
    redCompTable->setFixedWidth(120);
    greenCompTable->setFixedWidth(120);
    blueCompTable->setFixedWidth(120);
    
    scrollArea4->setMinimumHeight(205);
    scrollArea4->setMinimumWidth(470);
    
    bwCompTable->setFixedHeight(145);
    redCompTable->setFixedHeight(145);
    greenCompTable->setFixedHeight(145);
    blueCompTable->setFixedHeight(145);
    
    setLayout(layout);
    
    hstgrmLabel->computeHstgrms(image);
    
    imageLabel->setPixmap(QPixmap::fromImage(image));
    imageLabel->setMinimumSize(image.size());
    
    scrollArea1->setAlignment(Qt::AlignCenter);
    
    for(int i = 0;i<image.height();i++)
    {
        for(int j = 0;j<image.width();j++)
        {
            int p = qGray(image.pixel(j, i));
            image.setPixel(j, i, qRgb(p, p, p));
        }
    }
    
    bwImageLabel->setPixmap(QPixmap::fromImage(image));
    bwImageLabel->setMinimumSize(image.size());
    
    scrollArea2->setAlignment(Qt::AlignCenter);
}



QString ImgDescription::getImgName()
{
    return imgName;
}



QString ImgDescription::getKeyWord()
{
    return keyWord->text();
}



int ImgDescription::getGroupColumn1()
{
    return groupColumn1;
}



int ImgDescription::getGroupColumn2()
{
    return groupColumn2;
}



void ImgDescription::setGroupColumn1(int column)
{
    groupColumn1 = column;
}



void ImgDescription::setGroupColumn2(int column)
{
    groupColumn2 = column;
}



void ImgDescription::addCompTableColumn()
{
    bwCompTable->insertColumn(bwCompTable->columnCount());
    bwCompTable->setFixedWidth(120+100*bwCompTable->columnCount());

    redCompTable->insertColumn(redCompTable->columnCount());
    redCompTable->setFixedWidth(120+100*redCompTable->columnCount());

    greenCompTable->insertColumn(greenCompTable->columnCount());
    greenCompTable->setFixedWidth(120+100*greenCompTable->columnCount());

    blueCompTable->insertColumn(blueCompTable->columnCount());
    blueCompTable->setFixedWidth(120+100*blueCompTable->columnCount());
    
    tableContainer->adjustSize();
}



void ImgDescription::addCompTableColumn(int count)
{
    for(int i = 0;i < count;i++)
        addCompTableColumn();
}



int ImgDescription::getCompTableSize(Qt::GlobalColor component)
{
    switch(component)
    {
        case Qt::gray :
            return bwCompTable->columnCount();
            break;
        
        case Qt::red :
            return redCompTable->columnCount();
            break;
        
        case Qt::green :
            return greenCompTable->columnCount();
            break;
        
        case Qt::blue :
            return blueCompTable->columnCount();
            break;
        
        default :
            return -1;
    }
}



Histogram * ImgDescription::getHistogram()
{
    return hstgrmLabel;
}



void ImgDescription::fillCompTableColumn(Qt::GlobalColor component, int column, QString imgName, int row1, int row2, int row3, int row4)
{
    QTableWidget * table = NULL;
    
    switch(component)
    {
        case Qt::gray :
            table = bwCompTable;
            break;
        
        case Qt::red :
            table = redCompTable;
            break;
        
        case Qt::green :
            table = greenCompTable;
            break;
        
        case Qt::blue :
            table = blueCompTable;
            break;
        
        default :
            return;
    }
    
    if (column < 0 || column > table->columnCount())
        return;
    
    table->setHorizontalHeaderItem(column, new QTableWidgetItem(imgName));
    
    table->resizeColumnToContents(column);
    
    if (table->columnWidth(column) > 100)
        table->horizontalHeaderItem(column)->setTextAlignment(Qt::AlignLeft);
    
    table->setColumnWidth(column, 100);
    
    if (row1 < 0)
        table->setItem(0, column, new QTableWidgetItem("err"));
    else
        table->setItem(0, column, new QTableWidgetItem(QString::number(row1)));
    
    if (row2 < 0)
        table->setItem(1, column, new QTableWidgetItem("err"));
    else
        table->setItem(1, column, new QTableWidgetItem(QString::number(row2)));
    
    if (row3 < 0)
        table->setItem(2, column, new QTableWidgetItem("err"));
    else
        table->setItem(2, column, new QTableWidgetItem(QString::number(row3)));
    
    if (row4 < 0)
        table->setItem(3, column, new QTableWidgetItem("err"));
    else
        table->setItem(3, column, new QTableWidgetItem(QString::number(row4)));
    
    table->item(0, column)->setTextAlignment(Qt::AlignCenter);
    table->item(1, column)->setTextAlignment(Qt::AlignCenter);
    table->item(2, column)->setTextAlignment(Qt::AlignCenter);
    table->item(3, column)->setTextAlignment(Qt::AlignCenter);
}

