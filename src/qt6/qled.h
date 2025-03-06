#ifndef QLED_H
#define QLED_H

#include <QObject>
#include <QWidget>
#include <QColor>

class QLed : public QWidget
{
    Q_OBJECT

    int etat1; // 0=eteinte 1=allumee
    int forme1;
    QRect *rect1;
    QColor couleur;

protected:
    void  paintEvent( QPaintEvent * );


public:
    explicit QLed(QWidget *parent = 0);

    void setForme(int forme_i); // 1=rectangulaire   2=ronde
    void setCouleur(QColor couleur_i);
    void setEtat(int etat_i);   // 0=eteinte    1=allumée
    
signals:
    
public slots:
    
};

#endif // QLED_H
