#ifndef _IMAGEVIEWER_H_
#define _IMAGEVIEWER_H_

#include <QtGui>
#include <vector>
#include "Histogram.h"
#include "ImgDescription.h"
#include "SearchDialog.h"



class ImageViewer : public QMainWindow
{
    Q_OBJECT
    
    public:
        ImageViewer();
    
    public slots:
        int getImgDescrListSize();
        ImgDescription * getImgDescrList(int index);
        void openImage();
        void displaySearchField();
        void displayHstgrm();
        void goToImage(int row, int column);
        void closeSearchContainer();
        void searchImage();
    
    private:
        int levDistance(QString kw1, QString kw2);
        int minDistance(int d1, int d2, int d3);
        void insertInImgSortTable(QString imgname, int column);
        void addToImgSortTable(QString imgname, int imgDescrIndex);
        QString mostReprImg(int column);
        void createMenus();
        int euclDistance(Histogram * hstgrm1, Histogram * hstgrm2, Qt::GlobalColor component, int value);
        int avgEuclDistance(Histogram * hstgrm1, Histogram * hstgrm2, Qt::GlobalColor component, int min, int max);
        
        QMenu * fileMenu;
        QMenu * imageMenu;
        QMenu * searchMenu;
        QMenu * helpMenu;
        
        QAction * openAction;
        QAction * exitAction;
        QAction * hstgrmsAction;
        QAction * findAction;
        QAction * aboutQtAction;
        
        QTabWidget * mainTab;
        
        std::vector<ImgDescription *> imgDescrList;
        
        QTableWidget * imgSortTable;
        
        QWidget * mainWidget;
        
        QWidget * searchContainer;
        QLineEdit * searchField;
        QPushButton * closeButton;
};



#endif

