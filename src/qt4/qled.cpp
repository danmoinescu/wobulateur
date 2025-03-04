#include "qled.h"
#include <QPainter>

QLed::QLed(QWidget *parent) :
    QWidget(parent)
{
    setGeometry(0,0,40,40);

    couleur = QColor( 0, 255, 0 );

}



void QLed::setCouleur(QColor couleur_i)
{
    couleur = couleur_i;
}


void QLed::setEtat(int etat_i)
{
    etat1 = etat_i;
    repaint();
}


void QLed::setForme(int forme_i)
{
    forme1 = forme_i;
    repaint();
}


void QLed::paintEvent( QPaintEvent * )
{
    QPainter painter(this);

    if (forme1 == 1)
    {
        painter.drawRect(0,0,20,20);
        if (etat1==0)  { painter.fillRect(1,1,19,19,QColor( 200, 200, 200 )); }
        else  { painter.fillRect(1,1,19,19, couleur); }
    }
    else if (forme1 == 2)
    {
        QBrush brush1;
        brush1.setStyle(Qt::SolidPattern);
        if (etat1==0) { brush1.setColor(QColor( 200, 200, 200 )); }
        else { brush1.setColor(couleur); }
        painter.setBrush(brush1);
        painter.drawEllipse(1,1,18,18);
    }


}
