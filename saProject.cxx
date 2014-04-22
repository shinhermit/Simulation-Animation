///////////////////////////////////////////////////////////////////////////////
//  File          :   saProject.cxx                                          //
//  Author        :   Wilfrid LEFER                                          //
//  Comment       :   Interface pour le projet de SA                         //
//  Related files :   saProject.h, wlAnimatedMesh.{h,cxx}                    //
//                    W. Lefer - 2010                                        //
///////////////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include <ctime>

#include <saProject.h>
#include <saViewer.h>
#include <wlCage.h>
#include <wlGround.h>
#include <wlAnimatedSphere.h>
#include <QApplication>
#include <QAction>
#include <QFileDialog>
#include <QToolBar>
#include <QMenuBar>
#include <QStatusBar>
#include <QStringList>
#include <QGroupBox>
#include <QComboBox>
#include <QPushButton>
#include <QEvent>
#include <QDockWidget>
#include <QLineEdit>
#include <QIntValidator>
#include <QDoubleValidator>

// la marge des layouts, qui par defaut est beaucoup trop importante sur MaxOS
#define LAYOUT_MARGIN 0
// la position initiale des balles
#define PX1   -2.0
#define PY1    0.0
#define PZ1   20.0
#define PX2    2.0
#define PY2    0.0
#define PZ2   20.0
// la vitesse initiale des balles
#define VX1   8.0
#define VY1   0.0
#define VZ1   0.0
#define VX2  -8.0
#define VY2   0.0
#define VZ2   0.0
// coefficient d'attenuation du rebond lors d'une reaction cinematique pure
#define k1 0.8
#define k2 0.6
// resistance des ressorts utilises lors d'une reaction a handicap
#define Ks 1.0
// masse associee aux points lors d'une reaction a handicap
#define M 1.0

saProject::saProject(QString title, QSize size, int debug, bool gpuMode)
    : wlCore(debug)
{
    this->Trace("-> saProject()");
    this->setWindowTitle(title);

    saViewer *viewer = new saViewer();
//    viewer->setFixedSize(size);
//    viewer->setAxisIsDrawn();
    viewer->show();

    unsigned int nbItems = 27;
    _configOpenCL(gpuMode, nbItems);
    _createParticles(nbItems, debug);
    _setSimulator(viewer, debug);

    _createGUI(viewer);

    this->Trace("<- saProject");
}

void saProject::_createParticles(unsigned int nbItems, int debug)
{
    wlAnimatedMesh * ball;
    float Xp, Yp, Zp, zOffset;
    unsigned int index;

    double itemsPerSide = ::pow(nbItems, 1./3);
    float step = 3;
    zOffset = 50;

    for(unsigned int i=0; i < itemsPerSide; ++i)
    {
        Xp = i*step;
        for(unsigned int j=0; j < itemsPerSide; ++j)
        {
            Yp = j*step;
            for(unsigned int k=0; k < itemsPerSide; ++k)
            {
                Zp = k*step + zOffset;

                ball = new Particle(this->balls, debug, NULL, "sphere.off");
                ball->SetPosition(Xp, Yp, Zp);
                ball->SetVelocity(0, 0, 0);

                if(gpuMode)
                {
                    index = i + j*itemsPerSide + k*itemsPerSide*itemsPerSide;
                    this->openClInput[index] = Xp;
                    this->openClInput[index+1] = Yp;
                    this->openClInput[index+2] = Zp;

                    this->openClInput[index+3] = 0;
                    this->openClInput[index+4] = 0;
                    this->openClInput[index+5] = 0;
                }

                ball->SetReaction(PURE_KINEMATIC);
                ball->SetAttenuationCoefficientForPureKinematicReaction(k1);
                ball->Reset();
                this->balls << ball;
            }
        }
    }
}

void saProject::_setSimulator(saViewer *viewer, int debug)
{
    wlSimulationEnvironment *env = new wlGround(debug);

//    this->simulator = new wlSimulator(debug, viewer, env, &this->balls);
    ParticleSimulator * ps = new ParticleSimulator(debug, viewer, env, &this->balls);
    ps->setSmoothingTolerance(10.);
    ps->setPressureToDensityGradientProportionnality(20.);
    this->simulator = ps;

    if(this->gpuMode)
    {
        this->simulator->setOpenClContext(&this->openClContext, &this->openClInput);
    }
}

void saProject::_configOpenCL(bool gpuMode, unsigned int nbItems)
{
    this->gpuMode = gpuMode;
    unsigned int kinSize = 6*nbItems; //OpenCL: taille du tableau des valeurs cinématiques

    if(gpuMode)
    {
        if (!this->openClContext.create())
        {
            std::cerr << "Could not create OpenCL context for the GPU\n" << std::endl;
            throw std::runtime_error("saProject::saProject: Could not create OpenCL context for the GPU\n");
        }

        this->openClInput = this->openClContext.createVector<float>(kinSize);
    }
}

void saProject::_createGUI(saViewer *viewer)
{

    //
    // Menus
    //
    QMenu *file_menu = this->menuBar()->addMenu("&File");
    QAction *openAct = new QAction("&Open file...", this);
    openAct->setShortcut(QKeySequence(QString("Ctrl+F")));
    openAct->setStatusTip("Charger un fichier de donnees");
    connect(openAct, SIGNAL(triggered()), this, SLOT(Open()));
    file_menu->addAction(openAct);
    file_menu->addSeparator();
    QAction *quitAct = new QAction("&Quit", this);
    quitAct->setShortcut(QKeySequence(QString("Ctrl+Q")));
    quitAct->setStatusTip("Quitter l'application");
    connect(quitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));
    file_menu->addAction(quitAct);
    //
    // Button bar
    //
    QToolBar *tb = new QToolBar();
    this->addToolBar(tb);
    QWidget *W = new QWidget();
    tb->addWidget(W);
    QVBoxLayout *Layout = new QVBoxLayout();
    Layout->setMargin(LAYOUT_MARGIN);
    Layout->setSpacing(LAYOUT_MARGIN);
    W->setLayout(Layout);

    // la position initiale des balles
//    for (int i=0 ; i<this->balls.size() ; i++) {
//        QWidget *w = new QWidget;
//        Layout->addWidget(w);
//        QHBoxLayout *layout = new QHBoxLayout();
//        w->setLayout(layout);
//        {
//            QLabel *l = new QLabel("Position initiale");
//            layout->addWidget(l);
//        }
//        {
//            QLabel *l = new QLabel("Px : ");
//            l->setWhatsThis(QString::fromLocal8Bit("Composante X de la position initiale"));
//            layout->addWidget(l);
//            QLineEdit *le = new QLineEdit(QString::number(i==0 ? PX1 : PX2));
//            layout->addWidget(le);
//            le->setMaximumWidth(40);
//            QObject::connect(le, SIGNAL(textChanged(QString)), this->balls[i], SLOT(SetPositionX(QString)));
//        }
//        {
//            QLabel *l = new QLabel("Py : ");
//            l->setWhatsThis(QString::fromLocal8Bit("Composante Y de la position initiale"));
//            layout->addWidget(l);
//            QLineEdit *le = new QLineEdit(QString::number(i==0 ? PY1 : PY2));
//            layout->addWidget(le);
//            le->setMaximumWidth(40);
//            QObject::connect(le, SIGNAL(textChanged(QString)), this->balls[i], SLOT(SetPositionY(QString)));
//        }
//        {
//            QLabel *l = new QLabel("Pz : ");
//            l->setWhatsThis(QString::fromLocal8Bit("Composante Z de la position initiale"));
//            layout->addWidget(l);
//            QLineEdit *le = new QLineEdit(QString::number(i==0 ? PZ1 : PZ2));
//            layout->addWidget(le);
//            le->setMaximumWidth(40);
//            QObject::connect(le, SIGNAL(textChanged(QString)), this->balls[i], SLOT(SetPositionZ(QString)));
//        }
//    }

//    // la vitesse initiale des balles
//    for (int i=0 ; i<this->balls.size() ; i++) {
//        QWidget *w = new QWidget;
//        Layout->addWidget(w);
//        QHBoxLayout *layout = new QHBoxLayout();
//        w->setLayout(layout);
//        {
//            QLabel *l = new QLabel("Vitesse initiale");
//            layout->addWidget(l);
//        }
//        {
//            QLabel *l = new QLabel("Vx : ");
//            l->setWhatsThis(QString::fromLocal8Bit("Composante X de la vitesse initiale"));
//            layout->addWidget(l);
//            QLineEdit *le = new QLineEdit(QString::number(i==0 ? VX1 : VX2));
//            layout->addWidget(le);
//            QDoubleValidator *valid = new QDoubleValidator(-100.0, 100.0, 2, this);
//            le->setValidator(valid);
//            le->setMaximumWidth(40);
//            QObject::connect(le, SIGNAL(textChanged(QString)), this->balls[i], SLOT(SetVelocityX(QString)));
//        }
//        {
//            QLabel *l = new QLabel("Vy : ");
//            l->setWhatsThis(QString::fromLocal8Bit("Composante Y de la vitesse initiale"));
//            layout->addWidget(l);
//            QLineEdit *le = new QLineEdit(QString::number(i==0 ? VY1 : VY2));
//            layout->addWidget(le);
//            QDoubleValidator *valid = new QDoubleValidator(-100.0, 100.0, 2, this);
//            le->setValidator(valid);
//            le->setMaximumWidth(40);
//            QObject::connect(le, SIGNAL(textChanged(QString)), this->balls[i], SLOT(SetVelocityY(QString)));
//        }
//        {
//            QLabel *l = new QLabel("Vz : ");
//            l->setWhatsThis(QString::fromLocal8Bit("Composante Z de la vitesse initiale"));
//            layout->addWidget(l);
//            QLineEdit *le = new QLineEdit(QString::number(i==0 ? VZ1 : VZ2));
//            layout->addWidget(le);
//            QDoubleValidator *valid = new QDoubleValidator(-100.0, 100.0, 2, this);
//            le->setValidator(valid);
//            le->setMaximumWidth(40);
//            QObject::connect(le, SIGNAL(textChanged(QString)), this->balls[i], SLOT(SetVelocityZ(QString)));
//        }
//    }
    // la gestion des collisions
    {
        QWidget *w = new QWidget;
        Layout->addWidget(w);
        this->reaction = new QHBoxLayout();
        w->setLayout(this->reaction);
        // le choix du type de reaction
        {
            QLabel *l = new QLabel("Reaction : ");
            this->reaction->addWidget(l);
            QComboBox *cb = new QComboBox();
            cb->setWhatsThis(QString::fromLocal8Bit("Type de prise en compte de la reaction"));
            this->reaction->addWidget(cb);
            cb->addItem("Cinematique pure");
            cb->addItem("Handicap");
            QObject::connect(cb, SIGNAL(activated(int)), this, SLOT(SetReaction(int)));
            for (int i=0 ; i<this->balls.size() ; i++)
                QObject::connect(cb, SIGNAL(activated(int)), this->balls[i], SLOT(SetReaction(int)));
            this->SetReaction(0);
        }
    }
    // la gestion du timing
    {
        QWidget *w = new QWidget;
        Layout->addWidget(w);
        QHBoxLayout *layout = new QHBoxLayout();
        w->setLayout(layout);
        // le nombre de pas de temps
        {
            QLabel *l = new QLabel("Nb de pas de temps : ");
            l->setWhatsThis(QString::fromLocal8Bit("Nombre de pas de temps de la simulation"));
            layout->addWidget(l);
            QLineEdit *le = new QLineEdit(QString::number(200));
            layout->addWidget(le);
            QIntValidator *valid = new QIntValidator(1, 10000, this);
            le->setValidator(valid);
            le->setMaximumWidth(40);
            QObject::connect(le, SIGNAL(textChanged(QString)), simulator, SLOT(SetNumberOfTimeSteps(QString)));
        }
        {
            QLabel *l = new QLabel("Pas de temps : ");
            l->setWhatsThis(QString::fromLocal8Bit("Pas de temps de l'echantillonnage"));
            layout->addWidget(l);
            QLineEdit *le = new QLineEdit(QString::number(0.1));
            layout->addWidget(le);
            QDoubleValidator *valid = new QDoubleValidator(0.0, 1.0, 5, this);
            le->setValidator(valid);
            le->setMaximumWidth(40);
            QObject::connect(le, SIGNAL(textChanged(QString)), simulator, SLOT(SetTimeStep(QString)));
        }
    }
    // la gestion de l'animation
    {
        QWidget *w = new QWidget;
        Layout->addWidget(w);
        QHBoxLayout *layout = new QHBoxLayout();
        w->setLayout(layout);
        {
            QPushButton *b = new QPushButton(QString::fromLocal8Bit("Reset"));
            layout->addWidget(b);
            b->setWhatsThis(QString::fromLocal8Bit("Reinitialise l'animation"));
            QObject::connect(b, SIGNAL(clicked()), simulator, SLOT(Reset()));
        }
        {
            QPushButton *b = new QPushButton(QString::fromLocal8Bit("Step"));
            layout->addWidget(b);
            b->setWhatsThis(QString::fromLocal8Bit("Calcule un pas de temps supplementaire"));
            QObject::connect(b, SIGNAL(clicked()), simulator, SLOT(Step()));
        }
        {
            QPushButton *b = new QPushButton(QString::fromLocal8Bit("Play"));
            layout->addWidget(b);
            b->setWhatsThis(QString::fromLocal8Bit("Demarre l'animation"));
            QObject::connect(b, SIGNAL(clicked()), simulator, SLOT(Play()));
        }
        {
            QPushButton *b = new QPushButton(QString::fromLocal8Bit("Stop"));
            layout->addWidget(b);
            b->setWhatsThis(QString::fromLocal8Bit("Stoppe l'animation continue"));
            QObject::connect(b, SIGNAL(clicked()), simulator, SLOT(Stop()));
        }
    }
    {
        QWidget *w = new QWidget;
        Layout->addWidget(w);
        QHBoxLayout *layout = new QHBoxLayout();
        w->setLayout(layout);
        {
            QPushButton *b = new QPushButton(QString::fromLocal8Bit("PrintSelf"));
            layout->addWidget(b);
            b->setWhatsThis(QString::fromLocal8Bit("Imprime le contenu de la mesh multiresolution"));
            QObject::connect(b, SIGNAL(clicked()), simulator, SLOT(PrintSelf()));
        }
        {
            QPushButton *b = new QPushButton(QString::fromLocal8Bit("PrintContent"));
            layout->addWidget(b);
            b->setWhatsThis(QString::fromLocal8Bit("Imprime le contenu detaille (incluant les tables S et O) de la mesh multiresolution"));
            QObject::connect(b, SIGNAL(clicked()), simulator, SLOT(PrintContent()));
        }
    }
    //
    // Central area
    //
    this->setCentralWidget(viewer);
    // Status bar
    this->sb = new QStatusBar;
    this->setStatusBar(this->sb);
    this->progress = new QProgressBar();
    //
    // Finally display everything
    //
    this->show();
}


// Slot: choisis un fichier contenant une mesh au format OFF
void
saProject::Open()
{
    this->Trace("-> slot Open()");
    QString s = QFileDialog::getOpenFileName(this,
                                             "Choisissez un fichier de donnees",
                                             ".",
                                             "Datasets (*.txt)");
    if (!s.isEmpty())
        for (int i=0 ; i<this->balls.size() ; i++)
            this->balls[i]->LoadMesh();
    this->Trace("<- slot Open()");
}

void
saProject::Message(QString text)
{
    this->console->append(text);
}

void
saProject::Start(int nsteps)
{
    this->progress->setRange(0, nsteps);
    this->progress->reset();
    this->GetStatusBar()->addWidget(this->progress);
    this->ns = 0;
    this->exectime = new QTime;
    this->exectime->start();
}

void
saProject::Step()
{
    this->progress->setValue(++this->ns);
    if (this->ns >= this->progress->maximum()) {
        this->GetStatusBar()->removeWidget(this->progress);
        int ms = this->exectime->elapsed();
        delete this->exectime;
        this->exectime = new QTime;
        this->Message(QString("Temps d'execution : ") + this->exectime->addMSecs(ms).toString("hh:mm:ss.zzz"));
    }
}

void
saProject::SetReaction(int index)
{
    static QWidget *w = NULL;
    if (w != NULL) {
        this->reaction->removeWidget(w);
        delete w;
    }
    switch (index) {
    case 0:
    {
        // le parametrage specifique au type cinematique pure
        w = new QWidget;
        QHBoxLayout *layout = new QHBoxLayout();
        w->setLayout(layout);
        QLabel *l = new QLabel("Coefficient d'attenuation : ");
        l->setWhatsThis(QString::fromLocal8Bit("Coefficient d'attenuation du rebond"));
        layout->addWidget(l);
        QLineEdit *le = new QLineEdit(QString::number(k1));
        layout->addWidget(le);
        QDoubleValidator *valid = new QDoubleValidator(0.0, 1.0, 2, this);
        le->setValidator(valid);
        le->setMaximumWidth(40);
        for (int i=0 ; i<this->balls.size() ; i++)
            QObject::connect(le, SIGNAL(textChanged(QString)), this->balls[i], SLOT(SetAttenuationCoefficientForPureKinematicReaction(QString)));
    }
        break;
    case 1:
    {
        // le parametrage specifique au type handicap
        w = new QWidget;
        QHBoxLayout *layout = new QHBoxLayout();
        w->setLayout(layout);
        {
            QLabel *l = new QLabel("Resistance : ");
            l->setWhatsThis(QString::fromLocal8Bit("Resistance des ressorts"));
            layout->addWidget(l);
            QLineEdit *le = new QLineEdit(QString::number(Ks));
            layout->addWidget(le);
            QDoubleValidator *valid = new QDoubleValidator(0.0, 1000.0, 2, this);
            le->setValidator(valid);
            le->setMaximumWidth(40);
            for (int i=0 ; i<this->balls.size() ; i++)
                QObject::connect(le, SIGNAL(textChanged(QString)), this->balls[0], SLOT(SetSpringCoefficientForPenaltyReaction(QString)));
        }
        {
            QLabel *l = new QLabel("Masse : ");
            l->setWhatsThis(QString::fromLocal8Bit("Masse associee aux points"));
            layout->addWidget(l);
            QLineEdit *le = new QLineEdit(QString::number(M));
            layout->addWidget(le);
            QDoubleValidator *valid = new QDoubleValidator(0.0, 1000.0, 2, this);
            le->setValidator(valid);
            le->setMaximumWidth(40);
            for (int i=0 ; i<this->balls.size() ; i++)
                QObject::connect(le, SIGNAL(textChanged(QString)), this->balls[0], SLOT(SetPointWeightForPenaltyReaction(QString)));
        }
    }
        break;
    }
    this->reaction->addWidget(w);
}
