/*
INTERFACE D'ACQUISITION POUR WOBULATEUR 0-40MHz
auteur Silicium628
http://www.silicium628.fr
logiciel libre sous licence GNU

*/


#include "mainwindowimpl.h"
#include <stdio.h>
#include <unistd.h>
#include "rs232.h"
#include <math.h>

#include <QPainter>
#include <QMessageBox>
#include <QDir>


QString version = "7.2";

//	numport=16;        /* /dev/ttyUSB0
//	numport=17;        /* /dev/ttyUSB1 
    int  numport=16;
	
	int  bdrate=9600;       /* 9600 baud */
    unsigned char buf[100];

    QList<Acq_Rec> liste_mesures1, liste_mesures2, liste_mesures3, liste_mesures4, liste_mesures5;  	// 512 mesures par balayage
    unsigned char params[1030];
    unsigned char datas[1030];
    QString str_test;

	int num_octet_recu;
	int num_ligne;
    int num_pas_recu;
    int num_pas_desire;
    int digits_F_min[8];
    int digits_F_centrale[8];
    float pas;
    long int Freq_min_desiree, Freq_centrale_desiree, Freq_max_desiree;
//    long int Fmin, Fmax;
    long int Fmarqueur;
    int pos_marqueur, memo_pos_marqueur;
  //  long int graduation_x;
    long int pas_graduation=1000; //en Hz
    int num_pas_graduation;
//    int a_effacer=1;

    int cnx232_ok =0;
    int memo_y;


    QColor couleur_ecran = QColor::fromRgb(1, 92, 65, 255);
    QColor couleur_ligne = QColor::fromRgb(165, 42, 42, 255);
    QColor couleur_trace = QColor::fromRgb(66, 249, 112, 255);

    QColor couleur_memo1 = QColor::fromRgb(255, 85, 0, 255);
    QColor couleur_memo2 = QColor::fromRgb(255, 255, 0, 255);
    QColor couleur_memo3 = QColor::fromRgb(0, 255, 0, 255);
    QColor couleur_memo4 = QColor::fromRgb(0, 170, 255, 255);

    QColor couleur_marqueur = QColor::fromRgb(255, 255, 0, 255);
    QColor couleur_texte = QColor::fromRgb(66, 249, 112, 255);
    QColor vert = QColor::fromRgb(0, 255, 0, 255);
    QColor jaune = QColor::fromRgb(255, 255, 0, 255);


    QPen pen_trace(couleur_trace, 1, Qt::SolidLine);
    QPen pen_memo1(couleur_memo1, 1, Qt::SolidLine);
    QPen pen_memo2(couleur_memo2, 1, Qt::SolidLine);
    QPen pen_memo3(couleur_memo3, 1, Qt::SolidLine);
    QPen pen_memo4(couleur_memo4, 1, Qt::SolidLine);

    QPen pen_reticule(couleur_ligne, 1, Qt::SolidLine);
    QPen pen_marqueur(couleur_marqueur, 1, Qt::SolidLine);



MainWindowImpl::MainWindowImpl( QWidget * parent, Qt::WindowFlags f)
	: QMainWindow(parent, f)
{
	setupUi(this);
    setWindowTitle("Wobulateur ATmega + AD9850 - Silicium628 - version " + version);
	
    Led1 = new QLed(frame2);
    Led1->setGeometry(QRect(20, 40, 20, 20));
    Led1->setCouleur(QColor (0,255,0));
    Led1->setForme(1);
    Led1->setEtat(0);

    Led2 = new QLed(frame3);
    Led2->setGeometry(QRect(140, 14, 20, 20));
    Led2->setCouleur(QColor (0,255,0));
    Led2->setForme(2);
    Led2->setEtat(0);

    Led3 = new QLed(frame1);
    Led3->setGeometry(QRect(175, 243, 20, 20));
    Led3->setCouleur(QColor (0,255,0));
    Led3->setForme(2);
    Led3->setEtat(0);

    scene = new QGraphicsScene(this);
    scene->setBackgroundBrush(couleur_ecran);
    graphicsView1->setScene(scene);

    groupe_reticule = new QGraphicsItemGroup();
    groupe_trace = new QGraphicsItemGroup();
    scene->addItem(groupe_trace);

    liste_mesures1.clear();
    timer1 = new QTimer(this);

    connect(timer1, SIGNAL(timeout()), this, SLOT(reception_RS232() ));


    comboBox1->addItem("ttyS0 (com1)");
    comboBox1->addItem("ttyS1 (com2)");
	comboBox1->addItem("ttyUSB0");
	comboBox1->addItem("ttyUSB1");

    comboBox1->setCurrentIndex(2);

    BoutonLed1 = new QBoutonLed(frame4);
    BoutonLed1->setGeometry(QRect(20, 20, 40, 40));
    BoutonLed1->setCouleur(QColor (0,255,0));
    connect(BoutonLed1, SIGNAL(toggled(bool)), this, SLOT(on_BoutonLed1_toggled(bool)));
    BoutonLed1->click();
    pushButton->setVisible(false);



    QPalette palette1 = frame1->palette();
    palette1.setColor( backgroundRole(), QColor( 220, 220, 220 ) );
    frame1->setPalette( palette1 );
    frame1->setAutoFillBackground( true );

    QPalette palette2 = frame2->palette();
    palette2.setColor( backgroundRole(), QColor( 220, 220, 220 ) );
    frame2->setPalette( palette2 );
    frame2->setAutoFillBackground( true );

    tracer_marqueur(1);
    spinBox_7->setValue(8);
    spinBox_PAS->setValue(12);
    spinBox2->setValue(6);
    compose_frequence();
    groupe_trace->setZValue(100);

}
//

void vider_buffers()
{
	int n;	
    liste_mesures1.clear();
	for (n=0; n<1024; n++)
	{
		buf[n]=0;
//		mesures[n][0]=0;
//		mesures[n][1]=0;
	}
}


void efface_params()
{
    int n;
    for(n=0; n<100; n++)
    {
        params[n]=0;
    }
}


void MainWindowImpl::tracer_1freq(int x_i, int y_i, QString sti)
{
// affichage 1 trait vertical

    if((x_i>0) && (x_i<512))
    {
        ligne1 = new QGraphicsLineItem(x_i,0,x_i,512);
        ligne1->setPen(pen_reticule);
        groupe_reticule->addToGroup(ligne1);

        texte_frq = new QGraphicsTextItem(sti);
        texte_frq->setDefaultTextColor(couleur_texte);
        texte_frq->setPos(x_i,y_i);
        if(x_i<450) // evite que l'ecriture ne deborde du cadre a droite
        {
            groupe_reticule->addToGroup(texte_frq);
        }
    }
}



void MainWindowImpl::tracer_reticule()
{
    int i,y;
    QString st1;
//ATTENTION: ne pas remplacer les "float" par de "int" (aucuns) sous peine de calculs faux (erreurs d'arrondis)
    float nb_graduations;
    float largeur_graduations; // en pixels
    float deltaF; //en Hz
    float Fmilieu, Fi;


    deltaF=Freq_max_desiree-Freq_min_desiree;
    nb_graduations = deltaF/pas_graduation;


    if ((nb_graduations>12)&&(num_pas_graduation<20))
    {
        num_pas_graduation++;
        spinBox2->setValue(num_pas_graduation);
         calcul_freq_pas_graduation();
         nb_graduations = deltaF/pas_graduation;
     }

    if ((nb_graduations<3)&&(num_pas_graduation>1))
    {
        num_pas_graduation--;
        spinBox2->setValue(num_pas_graduation);
        calcul_freq_pas_graduation();
        nb_graduations = deltaF/pas_graduation;
    }
    calcul_freq_pas_graduation();

// graguation centrale
    Fmilieu= (float)Freq_centrale_desiree;
    st1.setNum(Fmilieu);
    tracer_1freq(255, 490, st1);


//graduations de part et d'autre de la fréquence centrale
    if ((nb_graduations>0) && (nb_graduations <20))
    {
        largeur_graduations = 512 / (nb_graduations);

        for(i=1; i<=nb_graduations/2; i++)
        {
            if((i%2)==0){y=490;} else{y=20;}
            Fi=Fmilieu - i*pas_graduation;
            st1.setNum(Fi);
            tracer_1freq(255-i*largeur_graduations, y, st1);

            Fi=Fmilieu + i*pas_graduation;
            st1.setNum(Fi);
            tracer_1freq(255+i*largeur_graduations,y, st1);
        }
    }

// lignes horizontales
    for (i=0; i<=10; i++)
    {
       y=50*i;
     //  ligne1 = scene->addLine(0, y, 512, y, pen_reticule);
       ligne1 = new QGraphicsLineItem(0, y, 512, y);
       ligne1->setPen(pen_reticule);
       groupe_reticule->addToGroup(ligne1);
    }
    scene->addItem(groupe_reticule);
}



void MainWindowImpl::effacer_reticule()
{
    foreach( QGraphicsItem *item, scene->items( groupe_reticule->boundingRect() ) )
    {
        if( item->group() == groupe_reticule )  { delete item; }
    }
}



void MainWindowImpl::effacer_trace()
{
    foreach( QGraphicsItem *item, scene->items( groupe_trace->boundingRect() ) )
    {
        if( item->group() == groupe_trace )  { delete item; }
    }
}


void MainWindowImpl::effacer_marqueur()
{
    scene->removeItem(ligne_marqueur);
}



void MainWindowImpl::tracer_marqueur(int x)
{
    ligne_marqueur = new QGraphicsLineItem(x,0,x,512);
    ligne_marqueur->setPen(pen_marqueur);
    scene->addItem(ligne_marqueur);
}


void MainWindowImpl::tracer_1point(int n, QList<Acq_Rec> *liste_mesures_i, QPen pen_i)
{
    Acq_Rec mesure_i;
    int y1,y2;
    int n_max;

    n_max=liste_mesures_i->size(); // =512 apres aquisition 1 balayage complet
    if ((n>1)&&(n<n_max))
    {

        mesure_i=liste_mesures_i->at(n-1);
        y1=(128*mesure_i.Valeur[1]+mesure_i.Valeur[0])/2;    if (y1>1024) { y1=1024;}

        mesure_i=liste_mesures_i->at(n);
        y2=(128*mesure_i.Valeur[1]+mesure_i.Valeur[0])/2;    if (y2>1024) { y2=1024;}

        if ( (y1 != 0) && (y2 != 0))
        {
            segment_trace = new QGraphicsLineItem(n-1,y1,n,y2);
            segment_trace->setPen(pen_i);
            groupe_trace->addToGroup(segment_trace);

        }
    }
}


void MainWindowImpl::save_image_ecran()
{
    QString st1,nom1, filename;
    QImage image1 (512, 512, QImage::Format_RGB32 );

    QPainter painter1(&image1);
    scene->render(&painter1);
    int n =0;

    nom1=lineEdit13->text();

    int ok=0;
    while (ok!=1)
    {
        st1.setNum(n); // conversion num -> txt
        filename = QDir::homePath()+"/"+nom1+st1+".png";
        QFile f( filename );
        if  (!f.exists() ) { ok=1;  } else { n++;   }
    }
    image1.save(filename, "png", 50);
    QMessageBox::information(this, "copie d'ecran", "image cree: "+ filename);
}



void formate_nombre(QString *str_i)
{
    uint n;
    n=str_i->length();
    if (n>3) {str_i->insert(n-3, ' ');}
    if (n>6) {str_i->insert(n-6, ' ');}
    if (n>9) {str_i->insert(n-9, ' ');}
    *str_i+=" Hz";
}




void MainWindowImpl::reception_RS232()
{
    Acq_Rec mesure_i;
	int n;
	int i;
    int data_i;
    float Fi;

    QString st1;
//rappel: int PollComport(int comport_number, unsigned char *buf, int size)

    n = PollComport(numport, buf, 10);
    if(n >0)
    {
        for(i=0; i < n; i++)
        {
            datas[num_octet_recu] = buf[i];
            st1.setNum(buf[i]); // conversion num -> txt
  //          str_test+=st1;
            num_octet_recu++;
        }

       if (num_octet_recu>=10)
       {
           if ((datas[0]=='D')&&(datas[1]=='A')&&(datas[2]=='T'))
           {
               Led1->setCouleur(jaune);
               Led1->setEtat(1);
                for(i=0; i < n; i++)
                {
                    data_i = buf[i];


        //les donnees sont transmises sur deux fois 7 bits
        // le 8eme bit (bit7) determine si c'est l'octet de poids faible ou celui de poids fort qui est recu
                    if ((data_i & 128)==0)
                    {
                        st1.setNum(data_i); // conversion num -> txt
                        mesure_i.Valeur[0]=data_i;
                    }
                    else
                    {
                        data_i &= 127; // elimine le bit d'identification de poids

                        Fi = Freq_min_desiree+num_ligne*pas;
                        mesure_i.Freq=(long int)Fi;
                        mesure_i.Valeur[1]=data_i;
                        liste_mesures1<<mesure_i;
                        tracer_1point(num_ligne, &liste_mesures1, pen_trace);

                        progressBar1->setValue(num_ligne);

                        num_ligne++;
                    }

                    num_octet_recu++;


                    if (num_ligne >= 506)
                    {
                        num_octet_recu=0;
                        num_ligne=0;
                        progressBar1->setValue(0);
                        Led1->setCouleur(vert);
                        Led1->setEtat(1);

                    }
                }
            }
        }
    }
}



void MainWindowImpl::on_Btn_connect_pressed()
{
	QString st1; 
	
    Led1->setEtat(0);

    if (comboBox1->currentIndex()==0)
    {
        numport =0;
        st1="ttyS0";
    }
    if (comboBox1->currentIndex()==1)
    {
        numport =1;
        st1="ttyS1";
    }
    if (comboBox1->currentIndex()==2)
	{
		numport =16;	
		st1="ttyUSB0";
	}
    if (comboBox1->currentIndex()==3)
	{
		numport =17;	
		st1="ttyUSB1";
	}		
	num_octet_recu=0;
	num_ligne=0;
  if(OpenComport(numport, bdrate))
  {
    cnx232_ok=0;
    lineEdit1->setText("Ne peut pas ouvrir "+st1);
    pushButton->setVisible(true);
    Led2->setCouleur(QColor (255,0,0));
    Led2->setEtat(1);
  }
  else
  {
    cnx232_ok=1;
  	lineEdit1->setText("port "+st1+ " ouvert\n");
    pushButton->setVisible(false);
    Led2->setCouleur(QColor (0,255,0));
    Led2->setEtat(1);

  	timer1->start(2);

  }
}





void MainWindowImpl::MAJ_marqueur(int x_i)
{
    QString st1;
    //int n;
    effacer_marqueur();
    pos_marqueur = x_i;
    tracer_marqueur(pos_marqueur);

    Fmarqueur = Freq_min_desiree+pos_marqueur*pas;
    st1.setNum(Fmarqueur); // conversion num -> txt
    formate_nombre(&st1);
    lineEdit6->setText(st1);
}



void MainWindowImpl::on_horizontalSlider_sliderMoved(int position)
{
    MAJ_marqueur(position);
}


void MainWindowImpl::on_horizontalSlider_sliderReleased()
{

}


void MainWindowImpl::on_Btn_Go_pressed()
{
    if(cnx232_ok ==1)
    {
        num_octet_recu=0;
        num_ligne=0;
        //clear_tableau();
        vider_buffers();

        effacer_trace();
        Led1->setEtat(0);

        SendByte(numport, 'G');
        SendByte(numport, 'G');
    }
    else {  QMessageBox::information(this, "Erreur", "Pas de connexion RS232"); }
}



void MainWindowImpl::on_Btn_envoi_clicked()
{
/*
les valeurs numeriques sont transmises par RS232 digit par digit en clair (codage ascii des chiffres, codes 48..57 pour 0..9
et non par leur valeur directe (15 sera envoyé comme 1 suivi de 5 et non par la valeur 15)
ceci pour eviter des bugs lors de la transmisson de valeurs correspondant a des commandes RS232
*/
    int n;


    Led1->setEtat(0);
    Led3->setEtat(1);

    if(cnx232_ok !=1)  {  on_Btn_connect_pressed(); }

    effacer_trace();
    SendByte(numport, 'P'); //'P' comme 'Parametres'  (pas sur 2 digits puis frequence sur 8 digits)
//    SendByte(numport, '=');
//PAS sur 2 digits
    SendByte(numport, 48 + num_pas_desire /10); // unites (en chiffre 0..9)
    SendByte(numport, 48 + num_pas_desire %10); // dizaines
//FREQUENCE sur 8 digits
    for(n=0; n<8; n++)
    {
        SendByte(numport, 48+ digits_F_min[n]); // (en chiffre 0..9)
    }
 //   QTimer::singleShot(1000, this, SLOT(on_Btn_Go_pressed())); // laisse le temps de transmission puis de calcul pour l'ATmega

}



void MainWindowImpl::compose_frequence()
{
    QString st1;
    int i;
    Freq_centrale_desiree=0;
    for(i=0; i<8; i++)
    {
        Freq_centrale_desiree+= digits_F_centrale[i] * pow(10,i);
    }
    Freq_min_desiree=Freq_centrale_desiree-256*pas;
    if(Freq_min_desiree<0)
    {
        QMessageBox::information(this, "Attention", "Frequence min  < 0");
        Freq_min_desiree=0;
    }

    Freq_max_desiree=Freq_centrale_desiree+256*pas;

    if(Freq_max_desiree>49*1e6)
    {
        QMessageBox::information(this, "Attention", "Frequence max  > 49MHz");
        Freq_max_desiree=49*1e6;
    }

    digits_F_min[0]=(Freq_min_desiree/1) %10;
    digits_F_min[1]=(Freq_min_desiree/10) %10;
    digits_F_min[2]=(Freq_min_desiree/100) %10;
    digits_F_min[3]=(Freq_min_desiree/1000) %10;
    digits_F_min[4]=(Freq_min_desiree/10000) %10;
    digits_F_min[5]=(Freq_min_desiree/100000) %10;
    digits_F_min[6]=(Freq_min_desiree/1000000) %10;
    digits_F_min[7]=(Freq_min_desiree/10000000) %10;


    st1.setNum(Freq_min_desiree); // conversion num -> txt
    formate_nombre(&st1);
    lineEdit10->setText(st1);

    st1.setNum(Freq_centrale_desiree); // conversion num -> txt
    formate_nombre(&st1);
    lineEdit8->setText(st1);

    st1.setNum(Freq_max_desiree); // conversion num -> txt
    formate_nombre(&st1);
    lineEdit11->setText(st1);

    effacer_reticule();
    tracer_reticule();
    MAJ_marqueur(pos_marqueur);

    Led3->setEtat(0);
}




void MainWindowImpl::on_spinBox_8_valueChanged(int arg1)
{
    digits_F_centrale[7]=arg1;
    compose_frequence();
}

void MainWindowImpl::on_spinBox_7_valueChanged(int arg1)
{
    digits_F_centrale[6]=arg1;
    compose_frequence();

}

void MainWindowImpl::on_spinBox_6_valueChanged(int arg1)
{
    digits_F_centrale[5]=arg1;
    compose_frequence();
}

void MainWindowImpl::on_spinBox_5_valueChanged(int arg1)
{
    digits_F_centrale[4]=arg1;
    compose_frequence();
}

void MainWindowImpl::on_spinBox_4_valueChanged(int arg1)
{
    digits_F_centrale[3]=arg1;
    compose_frequence();
}

void MainWindowImpl::on_spinBox_3_valueChanged(int arg1)
{
    digits_F_centrale[2]=arg1;
    compose_frequence();
}


void MainWindowImpl::on_spinBox_2_valueChanged(int arg1)
{
    digits_F_centrale[1]=arg1;
    compose_frequence();
}




void MainWindowImpl::on_spinBox_PAS_valueChanged(int arg1)
{
    num_pas_desire=arg1;
    QString st1;
    pas = pow(2,num_pas_desire)/68.7194767;

    st1.setNum(pas); // conversion num -> txt
    if (num_pas_desire>15) {formate_nombre(&st1);}
    lineEdit9->setText(st1);

    compose_frequence(); // pour recalculer les freq min et max;

    effacer_reticule();
    tracer_reticule();
}


void MainWindowImpl::on_spinBox_1_valueChanged(int arg1)
{
    digits_F_centrale[0]=arg1;
    compose_frequence();
}


void MainWindowImpl::calcul_freq_pas_graduation()
{
    QString st1;
    long int liste_pas[21]={
    1,2,5,
    10,20,50,
    100,200,500,
    1000,2000,5000,
    10000,20000,50000,
    100000,200000,500000,
    1000000,2000000,5000000
    };

    pas_graduation = liste_pas[num_pas_graduation];
    st1.setNum(pas_graduation); // conversion num -> txt
    formate_nombre(&st1);
    lineEdit12->setText(st1);

}


void MainWindowImpl::on_spinBox2_valueChanged(int arg1)
{
    num_pas_graduation=arg1;
    calcul_freq_pas_graduation();
    compose_frequence();
}


void MainWindowImpl::on_Btn1_clicked()
{
    liste_mesures2=liste_mesures1;
}


void MainWindowImpl::on_Btn2_clicked()
{
    int n;
    for (n=0; n<512; n++) { tracer_1point(n, &liste_mesures2, pen_memo1);}
}



void MainWindowImpl::on_Btn_test_3_clicked()
{
    effacer_trace();
}




void MainWindowImpl::on_BoutonLed1_toggled(bool checked)
{
    if   (checked)
    {
      tracer_reticule();
      scene->addItem(groupe_reticule);
    }
    else
    {
      foreach( QGraphicsItem *item, scene->items(groupe_reticule->boundingRect() ))
      {
        if( item->group() == groupe_reticule ) {delete item; }
      }
    }
}




void MainWindowImpl::on_Btn1_2_clicked()
{
    liste_mesures3=liste_mesures1;
}

void MainWindowImpl::on_Btn2_2_clicked()
{
    int n;
    for (n=0; n<512; n++) { tracer_1point(n, &liste_mesures3, pen_memo2);}
}

void MainWindowImpl::on_Btn1_3_clicked()
{
    liste_mesures4=liste_mesures1;
}

void MainWindowImpl::on_Btn2_3_clicked()
{
    int n;
    for (n=0; n<512; n++) { tracer_1point(n, &liste_mesures4,pen_memo3);}
}

void MainWindowImpl::on_Btn1_4_clicked()
{
    liste_mesures5=liste_mesures1;
}

void MainWindowImpl::on_Btn2_4_clicked()
{
    int n;
    for (n=0; n<512; n++) { tracer_1point(n, &liste_mesures5,pen_memo4);}
}


void MainWindowImpl::on_comboBox1_currentIndexChanged(int index)
{
    Led2->setEtat(0);
}


void MainWindowImpl::on_pushButton_clicked()
{
    QMessageBox::information(this, "Aide", + "Essayez un autre port \n ou la commande (dans un terminal) \n 'sudo chown <your_user> /dev/ttyxxx' \n (change le proprietaire sous Linux)");
}

void MainWindowImpl::on_Btn_save_img_clicked()
{
    save_image_ecran();
}
