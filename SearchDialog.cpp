#include "SearchDialog.h"
#include "ImageViewer.h"
#include <iostream>



SearchDialog::SearchDialog(QWidget * parent) : QDialog(parent)
{
    QWidget * searchContainer = new QWidget(this);
    resultContainer = new QWidget(this);
    QWidget * buttonsContainer = new QWidget(this);
    
    searchField = new QLineEdit(searchContainer);
    closeButton = new QPushButton("Close", buttonsContainer);
    searchButton = new QPushButton("Search", buttonsContainer);
    
    result1 = new QPushButton(resultContainer);
    result2 = new QPushButton("toto", resultContainer);
    result3 = new QPushButton(resultContainer);
    
    QHBoxLayout * layout2 = new QHBoxLayout;
    layout2->addWidget(new QLabel("Search :", searchContainer));
    layout2->addWidget(searchField);
    searchContainer->setLayout(layout2);
    
    QHBoxLayout * layout3 = new QHBoxLayout;
    layout3->addStretch(0);
    layout3->addWidget(closeButton);
    layout3->addWidget(searchButton);
    buttonsContainer->setLayout(layout3);
    
    QVBoxLayout * layout4 = new QVBoxLayout;
    layout4->addWidget(new QLabel("Best results :", resultContainer));
    layout4->addWidget(result1);
    layout4->addWidget(result2);
    layout4->addWidget(result3);
    resultContainer->setLayout(layout4);
    
    QVBoxLayout * layout1 = new QVBoxLayout;
    layout1->addWidget(searchContainer);
    layout1->addStretch(0);
    layout1->addWidget(resultContainer);
    layout1->addStretch(0);
    layout1->addWidget(buttonsContainer);
    setLayout(layout1);
    
    setWindowTitle("Find an image");
    setFixedSize(400, 300);
    
    resultContainer->setVisible(false);
    
    connect(closeButton, SIGNAL(clicked()), this, SLOT(accept()));
    
    
    
    connect(searchButton, SIGNAL(clicked()), this, SLOT(searchImage()));
    connect(searchField, SIGNAL(returnPressed()), this, SLOT(searchImage()));
}

//TODO: la recherche doit renvoyer les 3 resultats les plus coherents
//      elle les affiche ds 1 qpushbutton chacun, le clic devant provoquer l'affichage du tab correspondant
void SearchDialog::searchImage()
{
    QString rech(searchField->text());
    
    ImageViewer * parent = (ImageViewer *)this->parentWidget();
    
    QString res1;
    QString res2;
    QString res3;
    
    for(int i = 0;i < parent->getImgDescrListSize();i++)
    {
        if (!parent->getImgDescrList(i)->getKeyWord().isEmpty())
        {
            int dist = levDistance(rech, parent->getImgDescrList(i)->getKeyWord());
            
            // recalcul des 3 meilleurs resultats
        }
    }
    
    // mise en place des label des qpushbutton
}



int SearchDialog::levDistance(QString kw1, QString kw2)
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



int SearchDialog::minDistance(int d1, int d2, int d3)
{
    int min = 0;
    
    min = (d1 < d2) ? d1 : d2;
    
    return (d3 < min) ? d3 : min;
}

