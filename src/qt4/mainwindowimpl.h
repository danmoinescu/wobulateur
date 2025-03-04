#ifndef MAINWINDOWIMPL_H
#define MAINWINDOWIMPL_H
//
#include <QMainWindow>
#include <QTimer>
#include <QGraphicsView>
#include <QGraphicsTextItem>
#include <QGraphicsLineItem>
#include <QGraphicsItemGroup>
#include "ui_mainwindow.h"
#include "boutonled.h"
#include "qled.h"
//

struct Acq_Rec
{
    unsigned char   Valeur[2];
    long int        Freq;
};


class MainWindowImpl : public QMainWindow, public Ui::MainWindow
{
Q_OBJECT
public:
	MainWindowImpl( QWidget * parent = 0, Qt::WFlags f = 0 );
	QTimer *timer1;

	
private slots:
	void on_Btn_Go_pressed();
    void on_Btn_connect_pressed();
    void reception_RS232();
    void tracer_1point(int n, QList<Acq_Rec> *liste_mesures_i, QPen pen_i);
    void effacer_reticule();
    void tracer_reticule();
    void effacer_trace();
    void effacer_marqueur();
    void tracer_marqueur(int x);
    void MAJ_marqueur(int x_i);
    void tracer_1freq(int x_i, int y_i, QString sti);
    void save_image_ecran();
    void on_horizontalSlider_sliderMoved(int position);
    void on_horizontalSlider_sliderReleased();
    void on_spinBox_PAS_valueChanged(int arg1);
    void on_spinBox2_valueChanged(int arg1);
    void on_Btn_envoi_clicked();
    void compose_frequence();
    void calcul_freq_pas_graduation();
    void on_spinBox_8_valueChanged(int arg1);
    void on_spinBox_7_valueChanged(int arg1);
    void on_spinBox_6_valueChanged(int arg1);
    void on_spinBox_5_valueChanged(int arg1);
    void on_spinBox_4_valueChanged(int arg1);
    void on_spinBox_3_valueChanged(int arg1);
    void on_spinBox_2_valueChanged(int arg1);
    void on_spinBox_1_valueChanged(int arg1);

    void on_Btn1_clicked();
    void on_Btn2_clicked();

    void on_Btn_test_3_clicked();

    void on_BoutonLed1_toggled(bool checked);

    void on_Btn1_2_clicked();
    void on_Btn2_2_clicked();
    void on_Btn1_3_clicked();
    void on_Btn2_3_clicked();
    void on_Btn1_4_clicked();
    void on_Btn2_4_clicked();

    void on_comboBox1_currentIndexChanged(int index);

    void on_pushButton_clicked();

    void on_Btn_save_img_clicked();

private:

    QGraphicsScene *scene;
    QGraphicsItemGroup *groupe_reticule;
    QGraphicsItemGroup *groupe_trace;

    QGraphicsLineItem *ligne1;
    QGraphicsLineItem *segment_trace;
    QGraphicsLineItem *ligne_marqueur;
    QGraphicsTextItem *texte_frq;

    QBoutonLed *BoutonLed1;

    QLed *Led1;
    QLed *Led2;
    QLed *Led3;



};
#endif





