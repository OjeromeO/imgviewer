#include <QApplication>
#include <QtGui>
#include "ImageViewer.h"



int main(int argc, char ** argv)
{
    QApplication app(argc, argv);
    
    ImageViewer imgviewer;
    imgviewer.show();
    
    QDesktopWidget * dim = QApplication::desktop();
    imgviewer.move(dim->width()/4, dim->height()/4);
    
    return app.exec();
}

