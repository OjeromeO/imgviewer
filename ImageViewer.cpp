#include "ImageViewer.h"
#include <QFileInfo>
#include <cmath>
#include <iostream>



//TODO: implementer la fermeture de tabs avec tabCloseRequested(int)
//          (necessite des destructeurs, avec delete...)
//          (utiliser un vector de ImgDescription s'il le faut)
// TODO: au lieu de renvoyer vers le tab de l'image la plus coherente pour la recherche,
//       afficher un bouton style "liste des resultats detailles", ouvrant une fenetre
//       qui liste les X premieres images les plus coherentes

/*
TODO
- permettre d'utiliser plusieurs mots-cles, separes par des virgules ou autre moyen
- initialiser quand meme des composants a l'ouverture de la fenetre pour avoir une vue d'ensemble de la structure ? (exemple: dessiner la structure des histogrammes)
- ajouter le redimensionnement/zoom/dezoom (automatique ou non) des images
    => resize(), ensure(Widget)Visible()
- parfois l'image ne se centre pas
- bouger l'image direct avec la souris au lieu des scrollbar
*/



ImageViewer::ImageViewer() : QMainWindow()
{
    mainWidget = new QWidget(this);
    
    mainTab = new QTabWidget(mainWidget);
    mainTab->setTabsClosable(false);
    
    searchContainer = new QWidget(mainWidget);
    searchField = new QLineEdit(searchContainer);
    closeButton = new QPushButton("X", searchContainer);
    closeButton->setFixedWidth(30);
    searchField->setMaxLength(100);
    searchField->setFixedWidth(350);
    
    QLabel * searchLabel = new QLabel("Search :", searchContainer);
    QHBoxLayout * layout0 = new QHBoxLayout;
    layout0->addWidget(searchLabel);
    layout0->addWidget(searchField);
    layout0->addStretch(0);
    layout0->addWidget(closeButton);
    searchContainer->setLayout(layout0);
    searchContainer->setVisible(false);
    searchContainer->setContentsMargins(0, -10, 0, -10);
    
    connect(closeButton, SIGNAL(clicked()), this, SLOT(closeSearchContainer()));
    connect(searchField, SIGNAL(returnPressed()), this, SLOT(searchImage()));
    
    imgSortTable = new QTableWidget(0, 5, mainWidget);
    imgSortTable->setFixedWidth(20+5*170);
    imgSortTable->setFixedHeight(28);
    imgSortTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    imgSortTable->setHorizontalHeaderItem(0, new QTableWidgetItem("Dark images"));
    imgSortTable->setHorizontalHeaderItem(1, new QTableWidgetItem("Light images"));
    imgSortTable->setHorizontalHeaderItem(2, new QTableWidgetItem("Red dominant images"));
    imgSortTable->setHorizontalHeaderItem(3, new QTableWidgetItem("Green dominant images"));
    imgSortTable->setHorizontalHeaderItem(4, new QTableWidgetItem("Blue dominant images"));
    
    for(int i = 0;i<5;i++)
        imgSortTable->setColumnWidth(i, 170);
    
    QVBoxLayout * layout = new QVBoxLayout;
    
    layout->addWidget(searchContainer);
    layout->addWidget(mainTab);
    layout->addWidget(imgSortTable, 0, Qt::AlignCenter);
    layout->addWidget(new QLabel("<font color=black size=2>Note: most representative images are on the top of their column.</font>", mainWidget), 0, Qt::AlignCenter);
    
    mainWidget->setLayout(layout);
    
    setCentralWidget(mainWidget);
    
    setWindowTitle("Image Viewer");
    
    createMenus();
    
    resize(500, 400);
    
    connect(imgSortTable, SIGNAL(cellClicked(int, int)), this, SLOT(goToImage(int, int)));
}



void ImageViewer::closeSearchContainer()
{
    searchContainer->setVisible(false);
    searchField->clear();
}



int ImageViewer::getImgDescrListSize()
{
    return imgDescrList.size();
}



ImgDescription * ImageViewer::getImgDescrList(int index)
{
    return imgDescrList[index];
}



void ImageViewer::openImage()
{
    QStringList filesList = QFileDialog::getOpenFileNames(this, "Open an image", QString(), "Images (*.bmp *.gif *.jpg *.jpeg *.png *.pbm *.pgm *.tiff *.xbm *.xpm)");
    
    for(int i = 0;i < filesList.size();i++)
    {
        QString file(filesList.at(i));
        
        if (file.isEmpty())
            continue;
        
        QImage image(file);
        
        QFileInfo imgInfo(file);
        
        if (image.isNull())
        {
            QMessageBox::critical(this, "Image Viewer", "Can't load "+imgInfo.fileName());
            continue;
        }
        
        for(int i = 0;i<mainTab->count();i++)
        {
            if (mainTab->tabText(i) == imgInfo.fileName())
                mainTab->setCurrentIndex(i);
        }
        
        if (mainTab->tabText(mainTab->currentIndex()) == imgInfo.fileName())
            continue;
        
        imgDescrList.push_back(new ImgDescription(mainTab, file));
        
        int tab = mainTab->addTab(imgDescrList[imgDescrList.size()-1], imgInfo.fileName());
        
        mainTab->setCurrentIndex(tab);
        
        // adding the columns in the previous images' QWTableWidget
        // and computing/inserting the euclidean distances between histograms
        for(unsigned int i = 0;i < imgDescrList.size()-1;i++)
        {
            imgDescrList[i]->addCompTableColumn();
        
            Histogram * h2 = imgDescrList[imgDescrList.size()-1]->getHistogram();
            int a, b, c, d = 0;
        
            Histogram * h1 = imgDescrList[i]->getHistogram();
            
            a = avgEuclDistance(h1, h2, Qt::gray, 0, 255);
            b = avgEuclDistance(h1, h2, Qt::gray, 171, 255);
            c = avgEuclDistance(h1, h2, Qt::gray, 0, 84);
            d = avgEuclDistance(h1, h2, Qt::gray, 85, 170);
            
            imgDescrList[i]->fillCompTableColumn(Qt::gray, imgDescrList.size()-2, imgDescrList[imgDescrList.size()-1]->getImgName(), a, b, c, d);
            
            a = avgEuclDistance(h1, h2, Qt::red, 0, 255);
            b = avgEuclDistance(h1, h2, Qt::red, 171, 255);
            c = avgEuclDistance(h1, h2, Qt::red, 0, 84);
            d = avgEuclDistance(h1, h2, Qt::red, 85, 170);
            
            imgDescrList[i]->fillCompTableColumn(Qt::red, imgDescrList.size()-2, imgDescrList[imgDescrList.size()-1]->getImgName(), a, b, c, d);
            
            a = avgEuclDistance(h1, h2, Qt::green, 0, 255);
            b = avgEuclDistance(h1, h2, Qt::green, 171, 255);
            c = avgEuclDistance(h1, h2, Qt::green, 0, 84);
            d = avgEuclDistance(h1, h2, Qt::green, 85, 170);
            
            imgDescrList[i]->fillCompTableColumn(Qt::green, imgDescrList.size()-2, imgDescrList[imgDescrList.size()-1]->getImgName(), a, b, c, d);
            
            a = avgEuclDistance(h1, h2, Qt::blue, 0, 255);
            b = avgEuclDistance(h1, h2, Qt::blue, 171, 255);
            c = avgEuclDistance(h1, h2, Qt::blue, 0, 84);
            d = avgEuclDistance(h1, h2, Qt::blue, 85, 170);
            
            imgDescrList[i]->fillCompTableColumn(Qt::blue, imgDescrList.size()-2, imgDescrList[imgDescrList.size()-1]->getImgName(), a, b, c, d);
        }
        
        // adding the columns in the new image's QWTableWidget
        imgDescrList[imgDescrList.size()-1]->addCompTableColumn(imgDescrList.size()-1);
        
        // TODO: optimiser le truc, puisqu'on met les meme valeurs dans les
        // colonnes correspondantes des images, or la on recalcule une 2eme fois
        // la meme chose...
        
        // computing/inserting the euclidean distances between histograms
        for(unsigned int i = 0;i < imgDescrList.size()-1;i++)
        {
            Histogram * h1 = imgDescrList[imgDescrList.size()-1]->getHistogram();
            Histogram * h2 = imgDescrList[i]->getHistogram();
            int a, b, c, d = 0;
            
            a = avgEuclDistance(h1, h2, Qt::gray, 0, 255);
            b = avgEuclDistance(h1, h2, Qt::gray, 171, 255);
            c = avgEuclDistance(h1, h2, Qt::gray, 0, 84);
            d = avgEuclDistance(h1, h2, Qt::gray, 85, 170);
            
            imgDescrList[imgDescrList.size()-1]->fillCompTableColumn(Qt::gray, i, imgDescrList[i]->getImgName(), a, b, c, d);
            
            a = avgEuclDistance(h1, h2, Qt::red, 0, 255);
            b = avgEuclDistance(h1, h2, Qt::red, 171, 255);
            c = avgEuclDistance(h1, h2, Qt::red, 0, 84);
            d = avgEuclDistance(h1, h2, Qt::red, 85, 170);
            
            imgDescrList[imgDescrList.size()-1]->fillCompTableColumn(Qt::red, i, imgDescrList[i]->getImgName(), a, b, c, d);
            
            a = avgEuclDistance(h1, h2, Qt::green, 0, 255);
            b = avgEuclDistance(h1, h2, Qt::green, 171, 255);
            c = avgEuclDistance(h1, h2, Qt::green, 0, 84);
            d = avgEuclDistance(h1, h2, Qt::green, 85, 170);
            
            imgDescrList[imgDescrList.size()-1]->fillCompTableColumn(Qt::green, i, imgDescrList[i]->getImgName(), a, b, c, d);
            
            a = avgEuclDistance(h1, h2, Qt::blue, 0, 255);
            b = avgEuclDistance(h1, h2, Qt::blue, 171, 255);
            c = avgEuclDistance(h1, h2, Qt::blue, 0, 84);
            d = avgEuclDistance(h1, h2, Qt::blue, 85, 170);
            
            imgDescrList[imgDescrList.size()-1]->fillCompTableColumn(Qt::blue, i, imgDescrList[i]->getImgName(), a, b, c, d);
        }
        
        addToImgSortTable(imgInfo.fileName(), imgDescrList.size()-1);
    }
}



void ImageViewer::displaySearchField()
{
    searchContainer->setVisible(true);
}



void ImageViewer::displayHstgrm()
{
    QDialog * hstgrmDialog = new QDialog(this);
    QScrollArea * scrollArea = new QScrollArea(hstgrmDialog);
    QString imgName(mainTab->tabText(mainTab->currentIndex()));
    Histogram * hstgrm = NULL;
    
    hstgrmDialog->setWindowTitle(imgName);
    
    for(unsigned int i = 0;i < imgDescrList.size();i++)
    {
        if (imgDescrList[i]->getImgName() == imgName)
            hstgrm = new Histogram(scrollArea, imgDescrList[i]->getHistogram());
    }
    
    if (hstgrm == NULL)
        return;
    
    
    scrollArea->setWidget(hstgrm);
    
    QHBoxLayout * layout = new QHBoxLayout;
    layout->addWidget(scrollArea);
    hstgrmDialog->setLayout(layout);
    
    hstgrm->resize(400, 1350);
    hstgrmDialog->setFixedWidth(420);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->adjustSize();
    
    hstgrmDialog->show();
}



void ImageViewer::goToImage(int row, int column)
{
    if (imgSortTable->item(row, column) == 0)
        return;
    
    QString imgName(imgSortTable->item(row, column)->text());
    
    if (mainTab->tabText(mainTab->currentIndex()) == imgName)
        return;
    
    for(int i = 0;i < mainTab->count();i++)
    {
        if (mainTab->tabText(i) == imgName)
            mainTab->setCurrentIndex(i);
    }
}



void ImageViewer::insertInImgSortTable(QString imgname, int column)
{
    if (imgSortTable->rowCount() == 0)
    {
        imgSortTable->insertRow(0);
        imgSortTable->setItem(0, column, new QTableWidgetItem(imgname));
        imgSortTable->setFixedHeight(28+30*1);
    }
    else
    {
        int lastUsed = -1;
        
        // search the column where to put the new image name
        for(int i = 0;i<imgSortTable->rowCount();i++)
        {
            if (imgSortTable->item(i,column) != 0)
                lastUsed = i;
        }
        
        if (lastUsed+1 >= imgSortTable->rowCount())
        {
            imgSortTable->insertRow(lastUsed+1);
            imgSortTable->setItem(lastUsed+1, column, new QTableWidgetItem(imgname));
            imgSortTable->setFixedHeight(28+30*imgSortTable->rowCount());
        }
        else
            imgSortTable->setItem(lastUsed+1, column, new QTableWidgetItem(imgname));
    }
}



void ImageViewer::addToImgSortTable(QString imgname, int imgDescrIndex)
{
    ImgDescription * imgDescr = imgDescrList[imgDescrIndex];
    Histogram * h = imgDescr->getHistogram();
    
    if (h->getBwHstgrm(257) > h->getBwHstgrm(258))
        imgDescr->setGroupColumn1(0);
    else
        imgDescr->setGroupColumn1(1);
    
    if (h->getRedHstgrm(257) >= h->getGreenHstgrm(257) &&
        h->getRedHstgrm(257) >= h->getBlueHstgrm(257))
        imgDescr->setGroupColumn2(2);
    
    if (h->getGreenHstgrm(257) >= h->getRedHstgrm(257) &&
        h->getGreenHstgrm(257) >= h->getBlueHstgrm(257))
        imgDescr->setGroupColumn2(3);
    
    if (h->getBlueHstgrm(257) >= h->getRedHstgrm(257) &&
        h->getBlueHstgrm(257) >= h->getGreenHstgrm(257))
        imgDescr->setGroupColumn2(4);
    
    insertInImgSortTable(imgname, imgDescr->getGroupColumn1());
    insertInImgSortTable(imgname, imgDescr->getGroupColumn2());
    
    int lastUsed1 = -1;
    int lastUsed2 = -1;
    
    for(int i = 0;i<imgSortTable->rowCount();i++)
    {
        if (imgSortTable->item(i,imgDescr->getGroupColumn1()) != 0)
            lastUsed1 = i;
        
        if (imgSortTable->item(i,imgDescr->getGroupColumn2()) != 0)
            lastUsed2 = i;
    }
    
    if (lastUsed1 <= 0 && lastUsed2 <= 0)
        return;
    
    if (lastUsed1 > 0)
    {
        QString reprImg1 = mostReprImg(imgDescr->getGroupColumn1());
        
        if (reprImg1 != imgSortTable->item(0, imgDescr->getGroupColumn1())->text())
        {
            int imgIndex = -1;
            
            for(int i = 0;i < lastUsed1;i++)
            {
                if (imgSortTable->item(i, imgDescr->getGroupColumn1())->text() == reprImg1)
                    imgIndex = i;
            }
            
            imgSortTable->removeCellWidget(lastUsed1, imgDescr->getGroupColumn1());
            
            imgSortTable->setItem(lastUsed1, imgDescr->getGroupColumn1(), imgSortTable->takeItem(0, imgDescr->getGroupColumn1()));
            
            imgSortTable->setItem(0, imgDescr->getGroupColumn1(), new QTableWidgetItem(reprImg1));
        }
    }
    
    if (lastUsed2 > 0)
    {
        QString reprImg2 = mostReprImg(imgDescr->getGroupColumn2());
        
        if (reprImg2 != imgSortTable->item(0, imgDescr->getGroupColumn2())->text())
        {
            int imgIndex = -1;
            
            for(int i = 0;i < lastUsed2;i++)
            {
                if (imgSortTable->item(i, imgDescr->getGroupColumn2())->text() == reprImg2)
                    imgIndex = i;
            }
            
            imgSortTable->removeCellWidget(lastUsed2, imgDescr->getGroupColumn2());
            
            imgSortTable->setItem(lastUsed2, imgDescr->getGroupColumn2(), imgSortTable->takeItem(0, imgDescr->getGroupColumn2()));
            
            imgSortTable->setItem(0, imgDescr->getGroupColumn2(), new QTableWidgetItem(reprImg2));
        }
    }
}



void ImageViewer::searchImage()
{
    QString rech(searchField->text());
    
    int minDist = 100;
    int imgIndex = 0;
    
    for(unsigned int i = 0;i < imgDescrList.size();i++)
    {
        if (!imgDescrList[i]->getKeyWord().isEmpty())
        {
            int dist = levDistance(rech, imgDescrList[i]->getKeyWord());
            
            if (dist < minDist)
            {
                minDist = dist;
                imgIndex = i;
            }
        }
    }
    
    mainTab->setCurrentIndex(imgIndex);
}



QString ImageViewer::mostReprImg(int column)
{
    std::vector<Histogram *> columnHstgrmList;
    std::vector<QString> columnQStringList;
    
    // adding Histograms for the images of the column
    for(unsigned int i = 0;i < imgDescrList.size();i++)
    {
        if (imgDescrList[i]->getGroupColumn1() == column ||
            imgDescrList[i]->getGroupColumn2() == column)
        {
            columnHstgrmList.push_back(imgDescrList[i]->getHistogram());
            columnQStringList.push_back(imgDescrList[i]->getImgName());
        }
    }
    
    // mean of the histograms
    int moyBwHstgrm[257] = {0};
    int moyRedHstgrm[257] = {0};
    int moyGreenHstgrm[257] = {0};
    int moyBlueHstgrm[257] = {0};
    
    for(int i = 0;i < 256;i++)
    {
        for(unsigned int j = 0;j < columnHstgrmList.size();j++)
        {
            moyBwHstgrm[i] += columnHstgrmList[j]->getBwHstgrm(i);
            moyRedHstgrm[i] += columnHstgrmList[j]->getBwHstgrm(i);
            moyGreenHstgrm[i] += columnHstgrmList[j]->getBwHstgrm(i);
            moyBlueHstgrm[i] += columnHstgrmList[j]->getBwHstgrm(i);
        }
        
        if (moyBwHstgrm[256] < moyBwHstgrm[i])
            moyBwHstgrm[256] = moyBwHstgrm[i];

        if (moyRedHstgrm[256] < moyRedHstgrm[i])
            moyRedHstgrm[256] = moyRedHstgrm[i];

        if (moyGreenHstgrm[256] < moyGreenHstgrm[i])
            moyGreenHstgrm[256] = moyGreenHstgrm[i];

        if (moyBlueHstgrm[256] < moyBlueHstgrm[i])
            moyBlueHstgrm[256] = moyBlueHstgrm[i];
    }
    
    // search the image that represents the best the column
    int imgReprIndex = -1;
    int minDist = -1;
    
    // compute average euclidean distance between the 4 histograms and the 4 mean histograms
    // (for each image)
    for(unsigned int j = 0;j < columnHstgrmList.size();j++)
    {
        int sumBW = 0;
        int sumR = 0;
        int sumG = 0;
        int sumB = 0;
        
        for (int i = 0;i <= 255;i++)
        {
            int v1 = (float)moyBwHstgrm[i]*(1000./(float)moyBwHstgrm[256]);
            int v2 = (float)columnHstgrmList[j]->getBwHstgrm(i)*(1000./(float)columnHstgrmList[j]->getBwHstgrm(i));
            int v3 = (float)moyRedHstgrm[i]*(1000./(float)moyRedHstgrm[256]);
            int v4 = (float)columnHstgrmList[j]->getRedHstgrm(i)*(1000./(float)columnHstgrmList[j]->getBwHstgrm(i));
            int v5 = (float)moyGreenHstgrm[i]*(1000./(float)moyGreenHstgrm[256]);
            int v6 = (float)columnHstgrmList[j]->getGreenHstgrm(i)*(1000./(float)columnHstgrmList[j]->getBwHstgrm(i));
            int v7 = (float)moyBlueHstgrm[i]*(1000./(float)moyBlueHstgrm[256]);
            int v8 = (float)columnHstgrmList[j]->getBlueHstgrm(i)*(1000./(float)columnHstgrmList[j]->getBwHstgrm(i));
            
            sumBW += abs(v1-v2);
            sumR += abs(v3-v4);
            sumG += abs(v5-v6);
            sumB += abs(v7-v8);
        }
        
        int dist = (float)sumBW/(float)256 + (float)sumR/(float)256 + (float)sumG/(float)256 + (float)sumB/(float)256;
        
        if (j==0)
        {
            minDist = dist;
            imgReprIndex = 0;
        }
        
        if (dist < minDist)
        {
            minDist = dist;
            imgReprIndex = j;
        }
    }
    
    return columnQStringList[imgReprIndex];
}



void ImageViewer::createMenus()
{
    fileMenu = new QMenu("&File", this);
    imageMenu = new QMenu("&Image", this);
    searchMenu = new QMenu("&Search", this);
    helpMenu = new QMenu("&Help", this);
    
    openAction = new QAction("&Open..", this);
    exitAction = new QAction("&Quit", this);
    hstgrmsAction = new QAction("&Histograms", this);
    findAction = new QAction("&Find...", this);
    aboutQtAction = new QAction("&About Qt", this);
    
    openAction->setShortcut(QKeySequence(QKeySequence::Open));
    exitAction->setShortcut(QKeySequence(QKeySequence::Quit));
    hstgrmsAction->setShortcut(QKeySequence("Ctrl+H"));
    findAction->setShortcut(QKeySequence(QKeySequence::Find));
    
    fileMenu->addAction(openAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);
    
    imageMenu->addAction(hstgrmsAction);
    
    searchMenu->addAction(findAction);
    
    helpMenu->addAction(aboutQtAction);
    
    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(imageMenu);
    menuBar()->addMenu(searchMenu);
    menuBar()->addMenu(helpMenu);
    
    connect(openAction, SIGNAL(triggered()), this, SLOT(openImage()));
    connect(exitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(hstgrmsAction, SIGNAL(triggered()), this, SLOT(displayHstgrm()));
    connect(findAction, SIGNAL(triggered()), this, SLOT(displaySearchField()));
    connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}



int ImageViewer::euclDistance(Histogram * hstgrm1, Histogram * hstgrm2, Qt::GlobalColor component, int value)
{
    if (value < 0 || value > 255)
        return -1;
    
    if (hstgrm1 == NULL || hstgrm2 == NULL)
        return -2;
    
    int v1, v2 = 0;
    
    switch(component)
    {
        case Qt::gray :
            v1 = (float)hstgrm1->getBwHstgrm(value)*(1000./(float)hstgrm1->getBwHstgrm(256));
            v2 = (float)hstgrm2->getBwHstgrm(value)*(1000./(float)hstgrm2->getBwHstgrm(256));
            return abs(v1-v2);
            break;
        
        case Qt::red :
            v1 = (float)hstgrm1->getRedHstgrm(value)*(1000./(float)hstgrm1->getRedHstgrm(256));
            v2 = (float)hstgrm2->getRedHstgrm(value)*(1000./(float)hstgrm2->getRedHstgrm(256));
            return abs(v1-v2);
            break;
        
        case Qt::green :
            v1 = (float)hstgrm1->getGreenHstgrm(value)*(1000./(float)hstgrm1->getGreenHstgrm(256));
            v2 = (float)hstgrm2->getGreenHstgrm(value)*(1000./(float)hstgrm2->getGreenHstgrm(256));
            return abs(v1-v2);
            break;
        
        case Qt::blue :
            v1 = (float)hstgrm1->getBlueHstgrm(value)*(1000./(float)hstgrm1->getBlueHstgrm(256));
            v2 = (float)hstgrm2->getBlueHstgrm(value)*(1000./(float)hstgrm2->getBlueHstgrm(256));
            return abs(v1-v2);
            break;
        
        default :
            return -3;
    }
}



int ImageViewer::avgEuclDistance(Histogram * hstgrm1, Histogram * hstgrm2, Qt::GlobalColor component, int min, int max)
{
    if (min < 0 || min > 255 || max < 0 || max > 255)
        return -1;
    
    if (max < min)
    {
        int tmp = min;
        min = max;
        max = tmp;
    }
    
    if (hstgrm1 == NULL || hstgrm2 == NULL)
        return -2;
    
    if (component != Qt::gray &&
        component != Qt::red &&
        component != Qt::green &&
        component != Qt::blue)
        return -3;
    
    int sum = 0;
    
    for (int i = min;i <= max;i++)
        sum += euclDistance(hstgrm1, hstgrm2, component, i);
            
    return (int)((float)sum/(float)(max-min+1));
}



int ImageViewer::levDistance(QString kw1, QString kw2)
{
    int l1 = kw1.length();
    int l2 = kw2.length();
    
    std::string kw1_s(kw1.toStdString());
    std::string kw2_s(kw2.toStdString());
    
    std::vector<int> matrix((l1+1)*(l2+1), 0);  // l1+1 rows and l2+1 columns
                                                // l1+1 * l2+1 elements
    
    int cost;
    
    for(int i = 0;i <= l1;i++)
        matrix[i*(l2+1)] = i;
    
    for(int j = 0;j <= l2;j++)
        matrix[j] = j;
    
    for(int i = 1;i <= l1;i++)
    {
        for(int j = 1;j <= l2;j++)
        {
            cost = (kw1_s[i-1] == kw2_s[j-1]) ? 0 : 1;
            
            matrix[i*(l2+1)+j] = minDistance(matrix[(i-1)*(l2+1)+j] + 1,
                                             matrix[i*(l2+1)+j-1] + 1,
                                             matrix[(i-1)*(l2+1)+j-1] + cost);
        }
    }
    
    return matrix[l1*(l2+1)+l2];
}



int ImageViewer::minDistance(int d1, int d2, int d3)
{
    int min = 0;
    
    min = (d1 < d2) ? d1 : d2;
    
    return (d3 < min) ? d3 : min;
}

