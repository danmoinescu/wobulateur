#include "boutonled.h"
#include "qpainter.h"


QBoutonLed::QBoutonLed(QWidget *parent) :
    QAbstractButton(parent)
{
    setCheckable(true);
    setGeometry(0,0,40,40);
    frame1 = new QFrame(this);
    frame1->setLineWidth(0);
    frame1->setFrameShape(QFrame::WinPanel);
    frame1->setFrameShadow(QFrame::Raised);
    frame1->setGeometry(0, 0, 30, 20);

    couleur = QColor( 0, 255, 0 );
}


void QBoutonLed::setCouleur(QColor couleur_i)
{
    couleur = couleur_i;
}



void QBoutonLed::paintEvent( QPaintEvent * )
{

    if (isChecked())
    {
        frame1->setLineWidth(2);
        frame1->setFrameShadow(QFrame::Sunken);
        QPalette palette = frame1->palette();
        frame1->setAutoFillBackground( true );
        palette.setColor( backgroundRole(), couleur );
        frame1->setPalette( palette );
    }
    else
    {
        frame1->setLineWidth(2);
        frame1->setFrameShadow(QFrame::Raised);
        QPalette palette = frame1->palette();
        frame1->setAutoFillBackground( true );
        palette.setColor( backgroundRole(), QColor( 200, 200, 200 ) );
        frame1->setPalette( palette );
    }
}



