#ifndef BOUTONLED_H
#define BOUTONLED_H

#include <QPushButton>
#include "qframe.h"

class QBoutonLed : public QAbstractButton
{
    Q_OBJECT

    QPixmap *led;
    QFrame *frame1;
    QColor couleur;


protected:
    void  paintEvent( QPaintEvent * );



public:
    explicit QBoutonLed(QWidget *parent = 0);

    void setCouleur(QColor couleur_i);
    
signals:
    
public slots:



    
};

#endif // BOUTONLED_H
