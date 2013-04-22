#ifndef _HISTOGRAM_H_
#define _HISTOGRAM_H_

#include <QtGui>



class Histogram : public QLabel
{
    public:
        Histogram(QWidget * parent);
        Histogram(QWidget * parent, Histogram * hstgrm);
        void computeHstgrms(QImage & img);
        void paintEvent(QPaintEvent * event);
        void drawBwHstgrm(int xBase, int yBase, int height);
        void drawRedHstgrm(int xBase, int yBase, int height);
        void drawGreenHstgrm(int xBase, int yBase, int height);
        void drawBlueHstgrm(int xBase, int yBase, int height);
        int getBwHstgrm(int index);
        int getRedHstgrm(int index);
        int getGreenHstgrm(int index);
        int getBlueHstgrm(int index);
        
    private:
        // index 0 to 255 => count of image's pixels for this value
        // index 256 => maximum value
        // index 257 => total value of the dark component
        // index 258 => total value of the light component
        int bwHstgrm[259];
        
        // index 0 to 255 => count of image's pixels for this value
        // index 256 => maximum value
        // index 257 => total value of the component
        int redHstgrm[258];
        int greenHstgrm[258];
        int blueHstgrm[258];
};



#endif

