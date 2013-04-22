#ifndef _SEARCHDIALOG_H_
#define _SEARCHDIALOG_H_

#include <QtGui>



class SearchDialog : public QDialog
{
    Q_OBJECT
    
    public:
        SearchDialog(QWidget * parent);
        
    public slots:
        void searchImage();
    
    private:
        int levDistance(QString kw1, QString kw2);
        int minDistance(int d1, int d2, int d3);
        QLineEdit * searchField;
        QPushButton * result1;
        QPushButton * result2;
        QPushButton * result3;
        QPushButton * closeButton;
        QPushButton * searchButton;
        QWidget * resultContainer;
};



#endif

