#ifndef _IMGDESCRIPTION_H_
#define _IMGDESCRIPTION_H_

#include <QtGui>
#include "Histogram.h"



class ImgDescription : public QWidget
{
    public:
        ImgDescription(QWidget * parent, QString & imgname);
        void addCompTableColumn();
        void addCompTableColumn(int count);
        int getCompTableSize(Qt::GlobalColor component);
        QString getImgName();
        QString getKeyWord();
        int getGroupColumn1();
        int getGroupColumn2();
        void setGroupColumn1(int column);
        void setGroupColumn2(int column);
        Histogram * getHistogram();
        void fillCompTableColumn(Qt::GlobalColor component, int column, QString imgname, int row1, int row2, int row3, int row4);
        
    private:
        QTabWidget * imageTab;
        
        QLabel * imageLabel;
        QLabel * bwImageLabel;
        
        Histogram * hstgrmLabel;
        QString imgName;
        int groupColumn1;
        int groupColumn2;
        
        QTableWidget * bwCompTable;
        QTableWidget * redCompTable;
        QTableWidget * greenCompTable;
        QTableWidget * blueCompTable;
        
        QWidget * descrContainer;
        QWidget * tableContainer;
        QWidget * kwContainer;
        
        QScrollArea * scrollArea1;
        QScrollArea * scrollArea2;
        QScrollArea * scrollArea3;
        QScrollArea * scrollArea4;
        
        QLineEdit * keyWord;
};



#endif

