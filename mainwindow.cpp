#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "individual.h"
#include "simulation.h"

#include <QTime>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QStringListModel>
#include <QSet>
#include <QDoubleValidator>

/**
 * @brief Funcion de comparacion de individuos con respecto al valor de desempeno de latencia
 * @param p1 Individuo 1 a comparar
 * @param p2 Individuo 2 a comparar
 * @return Verdadero si p1 es menor que p2 con respecto a la funcion objetivo de latencia
 */
inline static bool xLessThanLatency(Individual *p1, Individual *p2)
{
    return p1->getPerformanceLatency() < p2->getPerformanceLatency();
}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setFixedSize(880, 813);

    // Validadores para los parametros del algoritmo
    QValidator * validatorPopSize = new QIntValidator(1, 1000, this);
    ui->lineEditPopulationSize->setValidator(validatorPopSize);
    ui->lineEditPopulationSize->setToolTip("[1..1000]");

    QValidator * validatorGenerations = new QIntValidator(1, 10000, this);
    ui->lineEditGenerationNumber->setValidator(validatorGenerations);
    ui->lineEditGenerationNumber->setToolTip("[1..10000]");

    //QValidator * validatorAcceptedPercentage = new QIntValidator(1, 100, this);
    //ui->lineEditAceptationPercentage->setValidator(validatorAcceptedPercentage);
    //ui->lineEditAceptationPercentage->setToolTip("[1..100]");

    QValidator * validatorMutationStd = new QIntValidator(1, 10, this);
    ui->lineEditMutationStd->setValidator(validatorMutationStd);
    ui->lineEditMutationStd->setToolTip("[1..10]");

    QValidator * validatorExternalFileSize = new QIntValidator(1, 100, this);
    ui->lineEditExternalFileSize->setValidator(validatorExternalFileSize);
    ui->lineEditExternalFileSize->setToolTip("[1..100]");

    QValidator * validatorGridSubintervals = new QIntValidator(1, 10, this);
    ui->lineEditGridSubintervals->setValidator(validatorGridSubintervals);
    ui->lineEditGridSubintervals->setToolTip("[1..10]");

    QValidator * validatorGnormative = new QIntValidator(1, 100, this);
    ui->lineEditGnormative->setValidator(validatorGnormative);
    ui->lineEditGnormative->setToolTip("[1..20]");

    int matches = ui->lineEditPopulationSize->text().toInt()/2;
    QValidator * validatorMatchesPerTournament= new QIntValidator(1, matches, this);
    ui->lineEditMatchPerTournament->setValidator(validatorMatchesPerTournament);
    ui->lineEditMatchPerTournament->setText(QString::number(matches));
    ui->lineEditMatchPerTournament->setToolTip("Un valor recomendado es Poblacion/2");

    QDoubleValidator * validatorDirectedMutation = new QDoubleValidator(0.0, 1.0, 2, this);
    validatorDirectedMutation->setNotation(QDoubleValidator::StandardNotation);
    ui->lineEditDirectedMutation->setValidator(validatorDirectedMutation);
    ui->lineEditDirectedMutation->setToolTip("[0..1]");



    connect(ui->pushButtonRun, SIGNAL(clicked()), this, SLOT(executeAlgorithm()));

    connect(ui->pushButtonCompareAlgorithms, SIGNAL(clicked()), this, SLOT(compareAlgorithms()));

    connect(ui->checkBoxDirectedMutation, SIGNAL(stateChanged(int)), this, SLOT(activateDirectedMutation(int)));
    ui->labelDirectedMutation->setEnabled(false);
    ui->lineEditDirectedMutation->setEnabled(false);

    connect(ui->lineEditPopulationSize, SIGNAL(textChanged(const QString & )), this, SLOT(checkPopulationSize(const QString &)));


    connect(ui->checkBoxComparation, SIGNAL(stateChanged(int)), this, SLOT(activateComparationButton(int)));
    ui->pushButtonCompareAlgorithms->setEnabled(false);


    connect(ui->pushButtonRepeatAlgoritm, SIGNAL(clicked()), this, SLOT(repeatAlgorithm()));

    connect(ui->pushButtonCompareExecutions, SIGNAL(clicked()), this, SLOT(compareAlgorithmRepeated()));

    //connect(ui->pushButtonView, SIGNAL(clicked()), this, SLOT(view()));
    connect(ui->pushButtonView, SIGNAL(clicked()), this, SLOT(viewAll()));


    ui->label_AC_generico->setVisible(false);
    ui->acGenericNumber->setVisible(false);
    ui->label_AC_modificado->setVisible(false);
    ui->acModifiedNumber->setVisible(false);
    ui->acGenericTime->setVisible(false);
    ui->acModifiedTime->setVisible(false);
    ui->label_tiempo_generico_2->setVisible(false);
    ui->label_tiempo_modificado_2->setVisible(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::executeAlgorithm()
{
    if (!validateFields())
    {
        return;
    }


    // creacion del objeto simulacion
    simulation = new Simulation(ui->lineEditPopulationSize->text().toInt(),
                                ui->lineEditExternalFileSize->text().toInt(),
                                ui->lineEditGenerationNumber->text().toInt(),
                                ui->lineEditGridSubintervals->text().toInt(),
                                ui->lineEditGnormative->text().toInt(),
                                ui->lineEditPopulationSize->text().toInt()/2,
                                ui->lineEditMutationStd->text().toInt(),
                                /*ui->lineEditApsNumber->text().toInt()*/25,
                                ui->checkBoxDirectedMutation->isChecked(),
                                ui->lineEditDirectedMutation->text().toDouble());

    qsrand((uint)QTime::currentTime().msec());


    QFile file("/tmp/algorithmResult.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
    {
        QMessageBox msg;
        msg.setText("No se pudo abrir el archivo /tmp/algorithmResult.txt para escribir \nresultados de la ejecucion del algoritmo.");
        msg.exec();
        return;
    }
    QTextStream out(&file);
    out << "Inicia ejecucion del algoritmo cultural." <<"\n";

    out << endl;
    out << "Parametros de la ejecucion" << endl;
    out << "Tamano de la poblacion: " << ui->lineEditPopulationSize->text() << endl;
    out << "Tamano del archivo externo: " << ui->lineEditExternalFileSize->text() << endl;
    out << "Maximo numero de generaciones: " << ui->lineEditGenerationNumber->text() << endl;
    out << "Numero de subintervalos para la rejilla: " << ui->lineEditGridSubintervals->text() << endl;
    out << "Numero de generaciones para actualizar parte normativa: " << ui->lineEditGnormative->text() << endl;
    out << "Numero de encuentros por individuo en un torneo: " << QString::number(ui->lineEditPopulationSize->text().toInt()/2) << endl;
    out << "Desviacion estandar de la mutacion gausiana: " << ui->lineEditMutationStd->text() << endl;
    out << "Numero de APs desplegados: " << 25 << endl;

    // inicializar poblacion de tamano P
    simulation->initializePopulation();

    // evaluar poblacion inicial
    simulation->evaluateIndividuals();

    // inicializar parte fenotipica normativa del espacio de creencias
    simulation->initializeNormativePhenotypicPart();
    qDebug("...se acaba de inicializar la parte normativa fenotipica del espacio de creencias");

    // inicializar rejilla del espacio de creencias
    simulation->initializeGrid();
    qDebug("...se acaba de inicializar la grid");


    // lista de individuos no dominados
    QList<Individual *> nonDominatedList;

    // contador de generaciones para la actualizacion de la parte fenotipica normativa
    int countOfGenerations = 1;

    // repetir por el numero maximo de generaciones
    do{
        qDebug("...generacion: %d", simulation->getCurrentGenerationNumber());
        QFile file("/tmp/algorithmResult.txt");
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
        {
            QMessageBox msg;
            msg.setText("No se pudo abrir el archivo /tmp/algorithmResult.txt para escribir \nresultados de la ejecucion del algoritmo.");
            msg.exec();
            return;
        }
        QTextStream out(&file);
        out << endl<< "Generacion: "<< simulation->getCurrentGenerationNumber() <<"\n";

        // mutacion de la poblacion
        simulation->mutatePopulation();
        qDebug("...despues de simulation->mutatePopulation()");


        // evaluar hijos
        simulation->evaluateIndividuals();

        // realizar torneos y seleccionar poblacion
        simulation->selectPopulation();
        qDebug("...despues de simulation->selectPopulation()");

        // imprimir poblacion para depuracion
        simulation->printPopulation();

        // obtener los individuos no dominados
        nonDominatedList = simulation->getNonDominatedPopulationApproach1();
        qDebug("...Numero de individuos en la poblacion no dominada: %d", nonDominatedList.count());


        qDebug("-------INDIVIDUOS no dominados antes de insertarlos en el archivo externo-------");
        simulation->printList(nonDominatedList);
        qDebug("--------------------------------------------------------------------------------");
        qDebug("...despues de obtener los individuos no dominados");

        // agregar los individuos no dominados al archivo externo
        simulation->addNonDominatedIndividualsToExternalFile(nonDominatedList);
        //simulation->addNonDominatedIndividualsToExternalFile(simulation->getPopulationList());

        qDebug("Cantidad de INDIVIDUOS no dominados despues de insertarlos en el archivo externo: %d",
               simulation->getExternalFile()->getExternalFileList().count());


        // actualizar el espacio de creencias con los individos aceptados
        if (countOfGenerations == simulation->getgNormative())
        {
            qDebug("MainWindow.cpp: numero de generaciones transcurridas: %d ahora actualizar parte normativa", countOfGenerations);
            simulation->updateNormativePhenotypicPart();
        }


        // actualizar la rejilla con todos los individuos no dominados recien agregados al archivo externo
        // durante la generación actual
        simulation->updateGrid(simulation->getExternalFile()->getCurrentGenerationIndividualList());
        simulation->getExternalFile()->resetCurrentGenerationIndividualList();
        qDebug("...despues de actualizar la rejilla");

        qDebug("generacion actual: %d", simulation->getCurrentGenerationNumber());

        qDebug("****************************************************************************");
        qDebug("Individuos del archivo externo");
        simulation->printList(simulation->getExternalFile()->getExternalFileList());
        qDebug("****************************************************************************");

        //QMessageBox msg;
        //QString string = "Ver el Archivo externo al final de la generacion ";
        //string.append(QString::number(simulation->getCurrentGenerationNumber()));
        //msg.setText(string);
        //msg.exec();


        simulation->incrementGeneration();

        // incrementar contador de generaciones para actualizar parte fenotipica normativa
        countOfGenerations++;

    }while(!simulation->stopEvolution()); // fin de la repeticion

    qDebug("*********");
    qDebug("TERMINO EL ALGORITMO CULTURAL!");


    // poblar la lista de individuos no dominados del archivo externo
    populateListView();


    if (ui->checkBoxDirectedMutation->isChecked())
    {
        modificatedAlgorithmSolutions = simulation->getExternalFile()->getExternalFileList();
    }
    else
    {
        genericAlgorithmSolutions = simulation->getExternalFile()->getExternalFileList();
    }


    // generar el grafico
    //setupCustomPlot(ui->customPlot);

    // generar el grafico
    plotSolutions();


    //storeExecutionSolution();

    qDebug("antes de salir executeAlgorithm");
}


void MainWindow::populateListView()
{
    QStringList individuals;


    QFile file("/tmp/externalFile.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
        return;
    QTextStream out(&file);
    out << endl << "Poblacion del archivo externo al final del algoritmo cultural: " << "\n";



    QString aux;
    for (int z=simulation->getExternalFile()->getExternalFileList().count()-1; z>=0; z-- )
    {
        aux.append(simulation->getExternalFile()->getExternalFileList().at(z)->getIndividualAsQString());
        individuals << aux;
        aux.clear();
        out << simulation->getExternalFile()->getExternalFileList().at(z)->getIndividualAsQString() << "\n";
    }

    QStringListModel *model = new QStringListModel();
    model->setStringList(individuals);

    ui->listViewBestIndividuals->setModel(model);


    int numberOfIndividuals = simulation->getExternalFile()->getExternalFileList().count();
    ui->labelNonDominatedNUmber->setText(QString::number(numberOfIndividuals));

}


void MainWindow::setupCustomPlot(QCustomPlot *customPlot)
{

    int count = simulation->getExternalFile()->getExternalFileList().count();
    QVector<double> discovery(count), latency(count);

    int i = 0;

    Individual * individual;
    for (int z=simulation->getExternalFile()->getExternalFileList().count()-1; z>=0; z-- )
    {
        individual = simulation->getExternalFile()->getExternalFileList().at(z);
        discovery[i] = individual->getPerformanceDiscovery();
        latency[i] = individual->getPerformanceLatency();
        i++;
    }


    // create graph and assign data to it:
    customPlot->addGraph();
    customPlot->graph(0)->setPen(QPen(Qt::blue)); // line color blue for first graph
    customPlot->graph(0)->setData(discovery, latency);

    customPlot->graph(0)->setLineStyle(QCPGraph::lsLine);
    customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, Qt::red, 4));

    // give the axes some labels:
    customPlot->xAxis->setLabel("Descubierta");
    customPlot->yAxis->setLabel("Latencia");
    // set axes ranges, so we see all data:
    customPlot->xAxis->setRange(0, 75);
    customPlot->yAxis->setRange(0, 300);

    customPlot->yAxis->grid()->setSubGridVisible(true);

    ui->customPlot->replot();

    // show legend:
    //customPlot->legend->setVisible(true);

/*
    // add two new graphs and set their look:
    customPlot->addGraph();
    customPlot->graph(0)->setPen(QPen(Qt::blue)); // line color blue for first graph
    customPlot->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20))); // first graph will be filled with translucent blue
    customPlot->addGraph();
    customPlot->graph(1)->setPen(QPen(Qt::red)); // line color red for second graph
    // generate some points of data (y0 for first, y1 for second graph):
    QVector<double> x(250), y0(250), y1(250);
    for (int i=0; i<250; ++i)
    {
      x[i] = i;
      y0[i] = exp(-i/150.0)*cos(i/10.0); // exponentially decaying cosine
      y1[i] = exp(-i/150.0);             // exponential envelope
    }
    // configure right and top axis to show ticks but no labels:
    // (see QCPAxisRect::setupFullAxesBox for a quicker method to do this)
    customPlot->xAxis2->setVisible(true);
    customPlot->xAxis2->setTickLabels(false);
    customPlot->yAxis2->setVisible(true);
    customPlot->yAxis2->setTickLabels(false);
    // make left and bottom axes always transfer their ranges to right and top axes:
    connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));
    // pass data points to graphs:
    customPlot->graph(0)->setData(x, y0);
    customPlot->graph(1)->setData(x, y1);
    // let the ranges scale themselves so graph 0 fits perfectly in the visible area:
    customPlot->graph(0)->rescaleAxes();
    // same thing for graph 1, but only enlarge ranges (in case graph 1 is smaller than graph 0):
    customPlot->graph(1)->rescaleAxes(true);
    // Note: we could have also just called customPlot->rescaleAxes(); instead
    // Allow user to drag axis ranges with mouse, zoom with mouse wheel and select graphs by clicking:
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

*/

}


void MainWindow::newSetupCustomPlot(QCustomPlot *customPlot)
{
/*
    int count = simulation->getGlobalRepository()->getRepositoryList().count();
    QVector<double> discovery(count), latency(count);

    int i = 0;

    Particle * particle;
    //for (int z=simulation->getExternalFile()->getExternalFileList().count()-1; z>=0; z-- )
    for (int z=simulation->getGlobalRepository()->getRepositoryList().count()-1; z>=0; z-- )
    {
        particle = simulation->getGlobalRepository()->getRepositoryList().at(z);
        discovery[i] = particle->getPerformanceDiscovery();
        latency[i] = particle->getPerformanceLatency();
        i++;
    }
*/

    //customPlot->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom)); // period as decimal separator and comma as thousand separator
    customPlot->legend->setVisible(true);
    QFont legendFont = font();  // start out with MainWindow's font..
    legendFont.setPointSize(9); // and make a bit smaller for legend
    customPlot->legend->setFont(legendFont);
    customPlot->legend->setBrush(QBrush(QColor(255,255,255,230)));
    // by default, the legend is in the inset layout of the main axis rect. So this is how we access it to change legend placement:
    customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom|Qt::AlignRight);


    customPlot->clearGraphs();

    Individual * individual;
    int count = genericAlgorithmSolutions.count();
    QVector<double> discovery(count), latency(count);
    for (int i = 0; i < count; i++)
    {
        individual = genericAlgorithmSolutions.at(i);
        discovery[i] = individual->getPerformanceDiscovery();
        latency[i] = individual->getPerformanceLatency();
    }


    int countModified = modificatedAlgorithmSolutions.count();
    QVector<double> discoveryModified(countModified), latencyModified(countModified);
    if (ui->checkBoxDirectedMutation->isChecked())
    {
        for (int i = 0; i < countModified; i++)
        {
            individual = modificatedAlgorithmSolutions.at(i);
            discoveryModified[i] = individual->getPerformanceDiscovery();
            latencyModified[i] = individual->getPerformanceLatency();
        }
    }

    // create graph and assign data to it:
    customPlot->addGraph();

    customPlot->graph(0)->setPen(QPen(Qt::blue)); // line color blue for first graph

    if (ui->checkBoxDirectedMutation->isChecked())
    {
        customPlot->graph(0)->setName("AC modificado");
        customPlot->graph(0)->setData(discoveryModified, latencyModified);
    }
    else
    {
        customPlot->graph(0)->setName("AC original");
        customPlot->graph(0)->setData(discovery, latency);
    }

    customPlot->graph(0)->setLineStyle(QCPGraph::lsLine);
    customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, Qt::red, 4));


    if (ui->checkBoxComparation->isChecked())
    //if (modified)
    {
        customPlot->addGraph();
        customPlot->graph(1)->setPen(QPen(Qt::green)); // line color green for second graph
        customPlot->graph(1)->setData(discoveryModified, latencyModified);
        customPlot->graph(1)->setLineStyle(QCPGraph::lsLine);
        customPlot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, Qt::yellow, 4));
    }

    // give the axes some labels:
    customPlot->xAxis->setLabel("Descubierta");
    customPlot->yAxis->setLabel("Latencia");
    // set axes ranges, so we see all data:
    customPlot->xAxis->setRange(0, 75);
    customPlot->yAxis->setRange(0, 300);

    customPlot->yAxis->grid()->setSubGridVisible(true);

    ui->customPlot->replot();

}


void MainWindow::setupCustomPlot2(QCustomPlot *customPlot)
{
/*
    int count = simulation->getGlobalRepository()->getRepositoryList().count();
    QVector<double> discovery(count), latency(count);

    int i = 0;

    Particle * particle;
    //for (int z=simulation->getExternalFile()->getExternalFileList().count()-1; z>=0; z-- )
    for (int z=simulation->getGlobalRepository()->getRepositoryList().count()-1; z>=0; z-- )
    {
        particle = simulation->getGlobalRepository()->getRepositoryList().at(z);
        discovery[i] = particle->getPerformanceDiscovery();
        latency[i] = particle->getPerformanceLatency();
        i++;
    }
*/

    customPlot->legend->setVisible(true);
    QFont legendFont = font();  // start out with MainWindow's font..
    legendFont.setPointSize(9); // and make a bit smaller for legend
    customPlot->legend->setFont(legendFont);
    customPlot->legend->setBrush(QBrush(QColor(255,255,255,230)));
    // by default, the legend is in the inset layout of the main axis rect. So this is how we access it to change legend placement:
    customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom|Qt::AlignRight);

    customPlot->clearGraphs();


    Individual * individual;
    int count = genericAlgorithmSolutions.count();
    QVector<double> discovery(count), latency(count);
    for (int i = 0; i < count; i++)
    {
        individual = genericAlgorithmSolutions.at(i);
        discovery[i] = individual->getPerformanceDiscovery();
        latency[i] = individual->getPerformanceLatency();
    }


    int countModified = modificatedAlgorithmSolutions.count();
    QVector<double> discoveryModified(countModified), latencyModified(countModified);
    //if (ui->checkBoxComparation->isChecked())
    //if (comparation)
    //{
        for (int i = 0; i < countModified; i++)
        {
            individual = modificatedAlgorithmSolutions.at(i);
            discoveryModified[i] = individual->getPerformanceDiscovery();
            latencyModified[i] = individual->getPerformanceLatency();
        }
    //}

    // create graph and assign data to it:
    customPlot->addGraph();
    customPlot->graph(0)->setPen(QPen(Qt::blue)); // line color blue for first graph
    customPlot->graph(0)->setData(discovery, latency);
    customPlot->graph(0)->setLineStyle(QCPGraph::lsLine);
    customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, Qt::red, 4));
    customPlot->graph(0)->setName("AC original");

    customPlot->addGraph();
    customPlot->graph(1)->setPen(QPen(Qt::green)); // line color green for second graph
    customPlot->graph(1)->setData(discoveryModified, latencyModified);
    customPlot->graph(1)->setLineStyle(QCPGraph::lsLine);
    customPlot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, Qt::black, 4));
    customPlot->graph(1)->setName("AC modificado");

    // give the axes some labels:
    customPlot->xAxis->setLabel("Descubierta");
    customPlot->yAxis->setLabel("Latencia");
    // set axes ranges, so we see all data:
    customPlot->xAxis->setRange(0, 75);
    customPlot->yAxis->setRange(0, 300);

    customPlot->yAxis->grid()->setSubGridVisible(true);



    ui->customPlot->replot();

    // show legend:
    //customPlot->legend->setVisible(true);
}

bool MainWindow::validateFields()
{
    bool validate = true;

    QMessageBox msg;
    QString message;
    message.append("Los siguientes campos deben estar asignados:\n");

    if (ui->lineEditPopulationSize->text().isEmpty())
    {
        message.append("Tamaño de la población\n");
        validate = false;
    }
    if (ui->lineEditGenerationNumber->text().isEmpty())
    {
        message.append("Número máximo de generaciones\n");
        validate = false;
    }
    if (ui->lineEditMutationStd->text().isEmpty())
    {
        message.append("Desviación estándar\n");
        validate = false;
    }
    if (ui->lineEditExternalFileSize->text().isEmpty())
    {
        message.append("Tamaño máximo del archivo externo\n");
        validate = false;
    }
    if (ui->lineEditGridSubintervals->text().isEmpty())
    {
        message.append("Número de subintervalos de rejilla\n");
        validate = false;
    }
    if (ui->lineEditGnormative->text().isEmpty())
    {
        message.append("Frecuencia de actualización de parte normativa\n");
        validate = false;
    }
    if (ui->lineEditMatchPerTournament->text().isEmpty())
    {
        message.append("Número de encuentros por individuo\n");
        validate = false;
    }
    if (ui->checkBoxDirectedMutation->isChecked())
    {
        if (ui->lineEditDirectedMutation->text().isEmpty())
        {
            message.append("Probabilidad de mutación dirigida\n");
            validate = false;
        }
    }

    msg.setText(message);

    if (!validate)
        msg.exec();

    return validate;
}

void MainWindow::activateDirectedMutation(int state)
{
    qDebug("MainWindow::activateDirectedMutation: state: %d",state);

    if (state == 0)
    {
        ui->labelDirectedMutation->setEnabled(false);
        ui->lineEditDirectedMutation->setEnabled(false);
    }
    else if(state == 2)
    {
        ui->labelDirectedMutation->setEnabled(true);
        ui->lineEditDirectedMutation->setEnabled(true);
    }
}

void MainWindow::checkPopulationSize(const QString & str)
{
    int matches = str.toInt()/2;
    ui->lineEditMatchPerTournament->setText(QString::number(matches));

}




void MainWindow::activateComparationButton(int state)
{

    if (state == 0)
    {
        ui->pushButtonCompareAlgorithms->setEnabled(false);

        ui->pushButtonRun->setEnabled(true);



        ui->labelExternalFile->setVisible(true);
        ui->labelNonDominatedNUmber->setVisible(true);
        ui->listViewBestIndividuals->setVisible(true);


        ui->label_AC_generico->setVisible(false);
        ui->acGenericNumber->setVisible(false);
        ui->label_AC_modificado->setVisible(false);
        ui->acModifiedNumber->setVisible(false);

        ui->acGenericTime->setVisible(false);
        ui->acModifiedTime->setVisible(false);
        ui->label_tiempo_generico_2->setVisible(false);
        ui->label_tiempo_modificado_2->setVisible(false);

    }
    else if(state == 2)
    {
        ui->pushButtonCompareAlgorithms->setEnabled(true);

        ui->pushButtonRun->setEnabled(false);

        ui->labelExternalFile->setVisible(false);
        ui->labelNonDominatedNUmber->setVisible(false);
        ui->listViewBestIndividuals->setVisible(false);


        ui->label_AC_generico->setVisible(true);
        ui->acGenericNumber->setVisible(true);
        ui->label_AC_modificado->setVisible(true);
        ui->acModifiedNumber->setVisible(true);

        ui->acGenericTime->setVisible(true);
        ui->acModifiedTime->setVisible(true);

        ui->label_tiempo_generico_2->setVisible(true);
        ui->label_tiempo_modificado_2->setVisible(true);

    }

}

void MainWindow::compareAlgorithms()
{

    ui->checkBoxDirectedMutation->setChecked(false);

    QTime timer;
    timer.start();

    executeAlgorithm();

    int runtimeGeneric = timer.elapsed(); //gets the runtime in ms
    ui->acGenericTime->setText(QString::number(runtimeGeneric)+" ms");



    QMessageBox msg;
    msg.setText("Termino el algoritmo generico");
    //msg.exec();

    ui->checkBoxDirectedMutation->setChecked(true);


    timer.start();
    executeAlgorithm();
    int runtimeModified = timer.elapsed(); //gets the runtime in ms
    ui->acModifiedTime->setText(QString::number(runtimeModified)+" ms");

    //ui->checkBoxGrid->setChecked(false);
    ui->checkBoxDirectedMutation->setChecked(false);

    msg.setText("Termino el algoritmo modificado");
    //msg.exec();


    setupCustomPlot2(ui->customPlot);


    ui->acGenericNumber->setText(QString::number(genericAlgorithmSolutions.count()));
    ui->acModifiedNumber->setText(QString::number(modificatedAlgorithmSolutions.count()));



}



/*
    // secuencia de scanning aleatoria
    int marcados = 0;

    int low = 1;
    int high = 11;
    int value = 0;

    QSet<int> set;
    QString seq;

    while(marcados < 11)
    {
        value = qrand() % ((high + 1) - low) + low;
        if (!set.contains(value))
        {
            set.insert(value);
            marcados++;
            seq.append(QString::number(value));
            seq.append(",");
        }
    }
    qDebug("secuencia de scanning:");
    qDebug(qPrintable(seq));
*/

void MainWindow::plotSolutions()
{
    if (!ui->checkBoxComparation->isChecked())
    {
        newSetupCustomPlot(ui->customPlot);
    }
}


void MainWindow::repeatAlgorithm()
{
    qDebug("MainWindow::repeatAlgorithm()");

    QTime timer;
    QList<int> executionTimeList;

    for (int i=0; i<30; i++)
    {
        timer.start();
        executeAlgorithm();
        executionTimeList.append(timer.elapsed());

        if (ui->checkBoxDirectedMutation->isChecked())
        { // mutacion dirigida
            QList<Individual*> list(modificatedAlgorithmSolutions);
            repeatedSolutionList.append(list);
        }
        else // mutacion generica
        {
            QList<Individual*> list(genericAlgorithmSolutions);
            repeatedSolutionList.append(list);
        }

        //repeatedSolutionList.append(list);
    }

    qDebug("mira la lista");

    qDebug("tamano de la lista de repeticiones: ");
    qDebug(qPrintable(QString::number(repeatedSolutionList.count())));

    qDebug("valor del primer individuo de la lista");
    //qDebug(qPrintable(QString::number(repeatedSolutionList[0].at(0)->getIndividualId())));


    qDebug("Promedio del tiempo de ejecucion ");

    int time = 0;
    for (int j=0; j<executionTimeList.count(); j++)
    {
        time = time + executionTimeList.at(j);
    }
    double mean = time/executionTimeList.count();
    qDebug(qPrintable(QString::number(mean)+" ms"));

    qDebug("Promedio de numero de individuos no dominados por ejecucion");

    int totalIndividuals = 0;
    for (int j=0; j<executionTimeList.count(); j++)
    {
        totalIndividuals = totalIndividuals + repeatedSolutionList.at(j).count();
    }
    double individualMean = totalIndividuals/repeatedSolutionList.count();
    qDebug(qPrintable(QString::number(individualMean)+" individuos"));

/*
    // calculo del promedio de funciones objetivo
    QList<Individual *> listOfNonDominated;
    Individual * individual;

    int counter = getCountOfNonDominatedInRepetitions();
    double sumOfDiscovery = 0;
    double sumOfLatency = 0;

    for (int j=0; j<repeatedSolutionList.count(); j++)
    {
        listOfNonDominated = repeatedSolutionList.at(j);
        for (int k=0; k<listOfNonDominated.count(); k++)
        {
            individual = listOfNonDominated.at(k);
            sumOfDiscovery = sumOfDiscovery + individual->getPerformanceDiscovery();
            sumOfLatency = sumOfLatency + individual->getPerformanceLatency();

        }
    }
*/

    double meanF1 = getMeanOfObjectiveFunction(1, repeatedSolutionList, 0);
    qDebug("Promedio de Fo1: %s", qPrintable(QString::number(meanF1)));
    qDebug("STD de Fo1: %s", qPrintable(QString::number(getStandardDeviation(meanF1,1, repeatedSolutionList, 0))));



    double meanF2 = getMeanOfObjectiveFunction(2, repeatedSolutionList, 0);
    qDebug("Promedio de Fo2: %s", qPrintable(QString::number(meanF2)));
    qDebug("STD de Fo1: %s", qPrintable(QString::number(getStandardDeviation(meanF2,2, repeatedSolutionList, 0))));



}


void MainWindow::storeExecutionSolution()
{
    Individual * individual;
    Individual * auxIndividual;
    int count = genericAlgorithmSolutions.count();

    QList<Individual*> list;

    for (int i = 0; i < count; i++)
    {
        auxIndividual = genericAlgorithmSolutions.at(i);
        individual = new Individual(*auxIndividual);
        list.append(individual);
    }

    repeatedSolutionList.append(list);
}

void MainWindow::view()
{

    qDebug("MainWindow::view()");
    qDebug("tamano de la lista de repeticiones: ");
    qDebug(qPrintable(QString::number(repeatedSolutionList.count())));

    for (int i=0; i<repeatedSolutionList.count(); i++)
    {
        qDebug("tamano de la lista de individuos no dominados: ");
        qDebug(qPrintable(QString::number(repeatedSolutionList[i].count())));
    }



    ui->customPlotExecutions->legend->setVisible(true);
    QFont legendFont = font();  // start out with MainWindow's font..
    legendFont.setPointSize(9); // and make a bit smaller for legend
    ui->customPlotExecutions->legend->setFont(legendFont);
    ui->customPlotExecutions->legend->setBrush(QBrush(QColor(255,255,255,230)));
    // by default, the legend is in the inset layout of the main axis rect. So this is how we access it to change legend placement:
    ui->customPlotExecutions->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom|Qt::AlignRight);

    ui->customPlotExecutions->clearGraphs();


    Individual * individual;

    QList<Individual *> list1 = repeatedSolutionList.at(0);

    int count = list1.count();
    qDebug(qPrintable(QString::number(count)));

    QVector<double> discovery(count), latency(count);
    for (int i = 0; i < count; i++)
    {
        individual = list1.at(i);
        discovery[i] = individual->getPerformanceDiscovery();
        latency[i] = individual->getPerformanceLatency();
    }


    QList<Individual *> list2 = repeatedSolutionList.at(1);

    int countModified = list2.count();
    qDebug(qPrintable(QString::number(countModified)));

    QVector<double> discoveryModified(countModified), latencyModified(countModified);

    for (int i = 0; i < countModified; i++)
    {
        individual = list2.at(i);
        discoveryModified[i] = individual->getPerformanceDiscovery();
        latencyModified[i] = individual->getPerformanceLatency();
    }

    QList<Individual *> list3 = repeatedSolutionList.at(2);

    int count2 = list3.count();
    qDebug(qPrintable(QString::number(count2)));

    QVector<double> discovery3(count2), latency3(count2);

    for (int i = 0; i < count2; i++)
    {
        individual = list3.at(i);
        discovery3[i] = individual->getPerformanceDiscovery();
        latency3[i] = individual->getPerformanceLatency();
    }

    QList<Individual *> list4 = repeatedSolutionList.at(3);

    int count4 = list4.count();
    qDebug(qPrintable(QString::number(count4)));

    QVector<double> discovery4(count4), latency4(count4);

    for (int i = 0; i < count4; i++)
    {
        individual = list4.at(i);
        discovery4[i] = individual->getPerformanceDiscovery();
        latency4[i] = individual->getPerformanceLatency();
    }

    QList<Individual *> list5 = repeatedSolutionList.at(4);

    int count5 = list5.count();
    qDebug(qPrintable(QString::number(count5)));

    QVector<double> discovery5(count5), latency5(count5);

    for (int i = 0; i < count5; i++)
    {
        individual = list5.at(i);
        discovery5[i] = individual->getPerformanceDiscovery();
        latency5[i] = individual->getPerformanceLatency();
    }


    // create graph and assign data to it:
    ui->customPlotExecutions->addGraph();
    ui->customPlotExecutions->graph(0)->setPen(QPen(Qt::blue)); // line color blue for first graph
    ui->customPlotExecutions->graph(0)->setData(discovery, latency);
    ui->customPlotExecutions->graph(0)->setLineStyle(QCPGraph::lsLine);
    ui->customPlotExecutions->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, Qt::red, 4));
    ui->customPlotExecutions->graph(0)->setName("Ejecucion 1");


    ui->customPlotExecutions->addGraph();
    ui->customPlotExecutions->graph(1)->setPen(QPen(Qt::green)); // line color green for second graph
    ui->customPlotExecutions->graph(1)->setData(discoveryModified, latencyModified);
    ui->customPlotExecutions->graph(1)->setLineStyle(QCPGraph::lsLine);
    ui->customPlotExecutions->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, Qt::black, 4));
    ui->customPlotExecutions->graph(1)->setName("Ejecucion 2");

    ui->customPlotExecutions->addGraph();
    ui->customPlotExecutions->graph(2)->setPen(QPen(Qt::yellow)); // line color green for second graph
    ui->customPlotExecutions->graph(2)->setData(discovery3, latency3);
    ui->customPlotExecutions->graph(2)->setLineStyle(QCPGraph::lsLine);
    ui->customPlotExecutions->graph(2)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, Qt::black, 4));
    ui->customPlotExecutions->graph(2)->setName("Ejecucion 3");


    ui->customPlotExecutions->addGraph();
    ui->customPlotExecutions->graph(3)->setPen(QPen(Qt::magenta)); // line color green for second graph
    ui->customPlotExecutions->graph(3)->setData(discovery4, latency4);
    ui->customPlotExecutions->graph(3)->setLineStyle(QCPGraph::lsLine);
    ui->customPlotExecutions->graph(3)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, Qt::black, 4));
    ui->customPlotExecutions->graph(3)->setName("Ejecucion 4");

    ui->customPlotExecutions->addGraph();
    ui->customPlotExecutions->graph(4)->setPen(QPen(Qt::cyan)); // line color green for second graph
    ui->customPlotExecutions->graph(4)->setData(discovery5, latency5);
    ui->customPlotExecutions->graph(4)->setLineStyle(QCPGraph::lsLine);
    ui->customPlotExecutions->graph(4)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, Qt::black, 4));
    ui->customPlotExecutions->graph(4)->setName("Ejecucion 5");


    // give the axes some labels:
    ui->customPlotExecutions->xAxis->setLabel("Descubierta");
    ui->customPlotExecutions->yAxis->setLabel("Latencia");
    // set axes ranges, so we see all data:
    ui->customPlotExecutions->xAxis->setRange(0, 75);
    ui->customPlotExecutions->yAxis->setRange(0, 300);

    ui->customPlotExecutions->yAxis->grid()->setSubGridVisible(true);



    ui->customPlotExecutions->replot();


    repeatedSolutionList.clear();


}


void MainWindow::viewAll()
{

    qDebug("MainWindow::view()");
    qDebug("tamano de la lista de repeticiones: ");
    qDebug(qPrintable(QString::number(repeatedSolutionList.count())));

    for (int i=0; i<repeatedSolutionList.count(); i++)
    {
        qDebug("tamano de la lista de individuos no dominados: ");
        qDebug(qPrintable(QString::number(repeatedSolutionList[i].count())));
    }



    ui->customPlotExecutions->legend->setVisible(true);
    QFont legendFont = font();  // start out with MainWindow's font..
    legendFont.setPointSize(9); // and make a bit smaller for legend
    ui->customPlotExecutions->legend->setFont(legendFont);
    ui->customPlotExecutions->legend->setBrush(QBrush(QColor(255,255,255,230)));
    // by default, the legend is in the inset layout of the main axis rect. So this is how we access it to change legend placement:
    ui->customPlotExecutions->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom|Qt::AlignRight);

    ui->customPlotExecutions->clearGraphs();


    // identificar el número total de individuos de todas las ejecuciones
    int counter = getCountOfNonDominatedInRepetitions();
/*
    for (int x=0; x<repeatedSolutionList.count(); x++)
    {
        counter = counter + repeatedSolutionList.at(x).count();
    }
*/
    qDebug("counter: %s", qPrintable(QString::number(counter)));


    Individual * individual;
    QList<Individual *> list1; // = repeatedSolutionList.at(0);
    QVector<double> discovery(counter), latency(counter);
    int pos = 0;

    for (int j=0; j<repeatedSolutionList.count(); j++)
    {
        list1 = repeatedSolutionList.at(j);
        for (int k=0; k<list1.count(); k++)
        {
            individual = list1.at(k);
            discovery[pos] = individual->getPerformanceDiscovery();
            latency[pos] = individual->getPerformanceLatency();
            pos++;

            //qDebug("tamano del vector discovery: %s", qPrintable(QString::number(discovery.count())));
            //qDebug("tamano del vector latency: %s", qPrintable(QString::number(latency.count())));
        }
    }

    qDebug("tamano del vector discovery: %s", qPrintable(QString::number(discovery.count())));
    qDebug("tamano del vector latency: %s", qPrintable(QString::number(latency.count())));


/*

    int count = list1.count();
    qDebug(qPrintable(QString::number(count)));

    QVector<double> discovery(count), latency(count);
    for (int i = 0; i < count; i++)
    {
        individual = list1.at(i);
        discovery[i] = individual->getPerformanceDiscovery();
        latency[i] = individual->getPerformanceLatency();
    }


    QList<Individual *> list2 = repeatedSolutionList.at(1);

    int countModified = list2.count();
    qDebug(qPrintable(QString::number(countModified)));

    QVector<double> discoveryModified(countModified), latencyModified(countModified);

    for (int i = 0; i < countModified; i++)
    {
        individual = list2.at(i);
        discoveryModified[i] = individual->getPerformanceDiscovery();
        latencyModified[i] = individual->getPerformanceLatency();
    }

    QList<Individual *> list3 = repeatedSolutionList.at(2);

    int count2 = list3.count();
    qDebug(qPrintable(QString::number(count2)));

    QVector<double> discovery3(count2), latency3(count2);

    for (int i = 0; i < count2; i++)
    {
        individual = list3.at(i);
        discovery3[i] = individual->getPerformanceDiscovery();
        latency3[i] = individual->getPerformanceLatency();
    }

    QList<Individual *> list4 = repeatedSolutionList.at(3);

    int count4 = list4.count();
    qDebug(qPrintable(QString::number(count4)));

    QVector<double> discovery4(count4), latency4(count4);

    for (int i = 0; i < count4; i++)
    {
        individual = list4.at(i);
        discovery4[i] = individual->getPerformanceDiscovery();
        latency4[i] = individual->getPerformanceLatency();
    }

    QList<Individual *> list5 = repeatedSolutionList.at(4);

    int count5 = list5.count();
    qDebug(qPrintable(QString::number(count5)));

    QVector<double> discovery5(count5), latency5(count5);

    for (int i = 0; i < count5; i++)
    {
        individual = list5.at(i);
        discovery5[i] = individual->getPerformanceDiscovery();
        latency5[i] = individual->getPerformanceLatency();
    }
*/

    // create graph and assign data to it:
    ui->customPlotExecutions->addGraph();
    ui->customPlotExecutions->graph(0)->setPen(QPen(Qt::blue)); // line color blue for first graph
    ui->customPlotExecutions->graph(0)->setData(discovery, latency);
    //ui->customPlotExecutions->graph(0)->setLineStyle(QCPGraph::lsLine);
    ui->customPlotExecutions->graph(0)->setLineStyle(QCPGraph::lsNone);
    ui->customPlotExecutions->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, Qt::red, 4));
    ui->customPlotExecutions->graph(0)->setName("Ejecucion 1");

/*
    ui->customPlotExecutions->addGraph();
    ui->customPlotExecutions->graph(1)->setPen(QPen(Qt::green)); // line color green for second graph
    ui->customPlotExecutions->graph(1)->setData(discoveryModified, latencyModified);
    ui->customPlotExecutions->graph(1)->setLineStyle(QCPGraph::lsLine);
    ui->customPlotExecutions->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, Qt::black, 4));
    ui->customPlotExecutions->graph(1)->setName("Ejecucion 2");

    ui->customPlotExecutions->addGraph();
    ui->customPlotExecutions->graph(2)->setPen(QPen(Qt::yellow)); // line color green for second graph
    ui->customPlotExecutions->graph(2)->setData(discovery3, latency3);
    ui->customPlotExecutions->graph(2)->setLineStyle(QCPGraph::lsLine);
    ui->customPlotExecutions->graph(2)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, Qt::black, 4));
    ui->customPlotExecutions->graph(2)->setName("Ejecucion 3");


    ui->customPlotExecutions->addGraph();
    ui->customPlotExecutions->graph(3)->setPen(QPen(Qt::magenta)); // line color green for second graph
    ui->customPlotExecutions->graph(3)->setData(discovery4, latency4);
    ui->customPlotExecutions->graph(3)->setLineStyle(QCPGraph::lsLine);
    ui->customPlotExecutions->graph(3)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, Qt::black, 4));
    ui->customPlotExecutions->graph(3)->setName("Ejecucion 4");

    ui->customPlotExecutions->addGraph();
    ui->customPlotExecutions->graph(4)->setPen(QPen(Qt::cyan)); // line color green for second graph
    ui->customPlotExecutions->graph(4)->setData(discovery5, latency5);
    ui->customPlotExecutions->graph(4)->setLineStyle(QCPGraph::lsLine);
    ui->customPlotExecutions->graph(4)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, Qt::black, 4));
    ui->customPlotExecutions->graph(4)->setName("Ejecucion 5");
*/

    // give the axes some labels:
    ui->customPlotExecutions->xAxis->setLabel("Descubierta (#APs)");
    ui->customPlotExecutions->yAxis->setLabel("Latencia (ms)");
    // set axes ranges, so we see all data:
    ui->customPlotExecutions->xAxis->setRange(0, 75);
    ui->customPlotExecutions->yAxis->setRange(0, 300);

    ui->customPlotExecutions->yAxis->grid()->setSubGridVisible(true);



    ui->customPlotExecutions->replot();


    repeatedSolutionList.clear();


}



int MainWindow::getCountOfNonDominatedInRepetitions()
{
    // identificar el número total de individuos de todas las ejecuciones
    int counter = 0;
    for (int x=0; x<repeatedSolutionList.count(); x++)
    {
        counter = counter + repeatedSolutionList.at(x).count();
    }
    return counter;
}

double MainWindow::getMeanOfObjectiveFunction(int fo, QList<QList<Individual *> >list, int type)
{
    int counter = 0; //getCountOfNonDominatedInRepetitions();

    if (type==0)
    {
        counter = getCountOfNonDominatedInRepetitions();
    }
    else if (type==1)
    {
        counter = getCountOfNonDominatedInOriginalRepetitions();
    }else if (type==2)
    {
        counter = getCountOfNonDominatedInModificatedRepetitions();
    }

    double sumOfDiscovery = 0;
    double sumOfLatency = 0;

    QList<Individual *> listOfNonDominated;
    Individual * individual;

    //for (int j=0; j<repeatedSolutionList.count(); j++)
    for (int j=0; j<list.count(); j++)
    {
        //listOfNonDominated = repeatedSolutionList.at(j);
        listOfNonDominated = list.at(j);
        for (int k=0; k<listOfNonDominated.count(); k++)
        {
            individual = listOfNonDominated.at(k);
            sumOfDiscovery = sumOfDiscovery + individual->getPerformanceDiscovery();
            sumOfLatency = sumOfLatency + individual->getPerformanceLatency();

        }
    }

    if (fo==1)
    {
        return sumOfDiscovery/counter;
    }
    else if (fo==2)
    {
        return sumOfLatency/counter;
    }
    else
    {
        return 0;
    }

    //qDebug("Promedio de Fo1: %s", qPrintable(QString::number(sumOfDiscovery/counter)));
    //qDebug("Promedio de Fo2: %s", qPrintable(QString::number(sumOfLatency/counter)));
}

double MainWindow::getStandardDeviation(double mean, int fo, QList<QList<Individual *> >list, int type)
{
    double numerator = 0;
    double denominator = 0; // getCountOfNonDominatedInRepetitions();

    if (type==0)
    {
        denominator = getCountOfNonDominatedInRepetitions();
    }
    else if (type==1)
    {
        denominator = getCountOfNonDominatedInOriginalRepetitions();
    }else if (type==2)
    {
        denominator = getCountOfNonDominatedInModificatedRepetitions();
    }


    QList<Individual *> listOfNonDominated;
    Individual * individual;

    //for (int j=0; j<repeatedSolutionList.count(); j++)
    for (int j=0; j<list.count(); j++)
    {
        //listOfNonDominated = repeatedSolutionList.at(j);
        listOfNonDominated = list.at(j);
        for (int k=0; k<listOfNonDominated.count(); k++)
        {
            individual = listOfNonDominated.at(k);

            if (fo == 1)
            {
                numerator = numerator + pow((individual->getPerformanceDiscovery()-mean),2);

            }
            else if (fo == 2)
            {
                numerator = numerator + pow((individual->getPerformanceLatency()-mean),2);

            }
            else
            {
                return 0;
            }
        }
    }
    return sqrt(numerator/denominator);

}



void MainWindow::compareAlgorithmRepeated()
{

    qDebug("MainWindow::compareAlgorithmRepeated()");

    QTime timer;
    QList<double> executionTimeList;



    // ejecuciones del algoritmo original
    for (int i=0; i<30; i++)
    {
        timer.start();
        executeAlgorithm();
        executionTimeList.append(timer.elapsed());
        QList<Individual*> list(genericAlgorithmSolutions);
        repeatedOriginalSolutionList.append(list);
    }
    // calcular el tiempo promedio de ejecucion del algoritmo original
    double meanExecutionTimeOriginal = getMeanExecutionTime(executionTimeList);
    double stdExecutionTimeOriginal = getStdDeviationExecutionTime(executionTimeList, meanExecutionTimeOriginal);
    executionTimeList.clear();

    // ejecuciones del algoritmo modificado
    ui->checkBoxDirectedMutation->setChecked(true);
    for (int i=0; i<30; i++)
    {
        timer.start();
        executeAlgorithm();
        executionTimeList.append(timer.elapsed());
        QList<Individual*> list(modificatedAlgorithmSolutions);
        repeatedModificatedSolutionList.append(list);
    }

    // calcular el tiempo promedio de ejecucion del algoritmo modificado
    double meanExecutionTimeModificated = getMeanExecutionTime(executionTimeList);
    double stdExecutionTimeModificated = getStdDeviationExecutionTime(executionTimeList, meanExecutionTimeModificated);

    qDebug("------");
    qDebug("Promedio de tiempo de ejecución original:");
    qDebug(qPrintable(QString::number(meanExecutionTimeOriginal)+" ms, "+QString::number(stdExecutionTimeOriginal)));

    qDebug("Promedio de tiempo de ejecución modificado:");
    qDebug(qPrintable(QString::number(meanExecutionTimeModificated)+" ms, "+QString::number(stdExecutionTimeModificated)));
    qDebug("------");

    qDebug("Promedio de numero de individuos no dominados algoritmo original:");
    int totalIndividuals = 0;
    for (int j=0; j<repeatedOriginalSolutionList.count(); j++)
    {
        totalIndividuals = totalIndividuals + repeatedOriginalSolutionList.at(j).count();
    }
    double meanNonDominatedIndividuals1 = totalIndividuals/repeatedOriginalSolutionList.count();
    qDebug(qPrintable(QString::number(meanNonDominatedIndividuals1)+" individuos"));

    qDebug("Promedio de numero de individuos no dominados algoritmo modificado:");
    int totalIndividuals2 = 0;
    for (int j=0; j<repeatedModificatedSolutionList.count(); j++)
    {
        totalIndividuals2 = totalIndividuals2 + repeatedModificatedSolutionList.at(j).count();
    }
    double meanNonDominatedIndividuals2 = totalIndividuals2/repeatedModificatedSolutionList.count();
    qDebug(qPrintable(QString::number(meanNonDominatedIndividuals2)+" individuos"));
    qDebug("------");



    double meanF1Original = getMeanOfObjectiveFunction(1, repeatedOriginalSolutionList, 1);
    qDebug("Promedio de Fo1 original: %s", qPrintable(QString::number(meanF1Original)));
    qDebug("STD de Fo1 original: %s", qPrintable(QString::number(getStandardDeviation(meanF1Original, 1, repeatedOriginalSolutionList, 1))));
    double meanF2Original = getMeanOfObjectiveFunction(2, repeatedOriginalSolutionList, 1);
    qDebug("Promedio de Fo2 original: %s", qPrintable(QString::number(meanF2Original)));
    qDebug("STD de Fo2 original: %s", qPrintable(QString::number(getStandardDeviation(meanF2Original, 2, repeatedOriginalSolutionList, 1))));


    double meanF1Modificated = getMeanOfObjectiveFunction(1, repeatedModificatedSolutionList, 2);
    qDebug("Promedio de Fo1 modificada: %s", qPrintable(QString::number(meanF1Modificated)));
    qDebug("STD de Fo1 modificada: %s", qPrintable(QString::number(getStandardDeviation(meanF1Modificated, 1, repeatedModificatedSolutionList, 2))));
    double meanF2Modificated = getMeanOfObjectiveFunction(2, repeatedModificatedSolutionList, 2);
    qDebug("Promedio de Fo2 modificada: %s", qPrintable(QString::number(meanF2Modificated)));
    qDebug("STD de Fo2 modificada: %s", qPrintable(QString::number(getStandardDeviation(meanF2Modificated, 2, repeatedModificatedSolutionList, 2))));


    //---------------------------------------------------------------------------
    // prueba de obtener los no dominados de todas las ejecuciones del algoritmo
    QList<Individual*> myList = getNonDominatedIndivualsFromRepetitions(true);
    qDebug("--------");
    qDebug("individuos no dominados del algoritmo original: %s", qPrintable(QString::number(myList.count())));

    QVector<double> discoveryParetoOriginal(myList.count()), latencyParetoOriginal(myList.count());
    Individual * paretoIndividual;
    int vectorPosition = 0;

    for (int i=0; i<myList.count();i++)
    {
        paretoIndividual = myList.at(i);
        discoveryParetoOriginal[vectorPosition] = paretoIndividual->getPerformanceDiscovery();
        latencyParetoOriginal[vectorPosition] = paretoIndividual->getPerformanceLatency();
        vectorPosition++;
    }

    // escribir en un archivo los individuos del frente de pareto encontrado en un archivo
    reportIndividualAsFile(myList,"individuosFrenteParetoOriginal");


    // colocar las cadenas en la pestana de cadenas de la interfaz grafica
    populateAListView(myList, ui->listViewPFOriginal);



    myList.clear();

    myList = getNonDominatedIndivualsFromRepetitions(false);
    qDebug("--------");
    qDebug("individuos no dominados del algoritmo modificado: %s", qPrintable(QString::number(myList.count())));

    QVector<double> discoveryParetoModificated(myList.count()), latencyParetoModificated(myList.count());
    vectorPosition = 0;

    for (int i=0; i<myList.count();i++)
    {
        paretoIndividual = myList.at(i);
        discoveryParetoModificated[vectorPosition] = paretoIndividual->getPerformanceDiscovery();
        latencyParetoModificated[vectorPosition] = paretoIndividual->getPerformanceLatency();
        vectorPosition++;
    }

    // escribir en un archivo los individuos del frente de pareto encontrado en un archivo
    reportIndividualAsFile(myList,"individuosFrenteParetoModificado");

    // colocar las cadenas en la pestana de cadenas de la interfaz grafica
    populateAListView(myList, ui->listViewPFModificated);

    qDebug("--------");
    //---------------------------------------------------------------------------


    // deseleccionar el check para modificacion
    ui->checkBoxDirectedMutation->setChecked(false);



    // llenar el vector original con los valores de las funciones objetivo
    Individual * individual;
    QList<Individual *> list1;
    int originalCounter = getCountOfNonDominatedInOriginalRepetitions();

    QVector<double> discoveryOriginal(originalCounter), latencyOriginal(originalCounter);
    int pos = 0;

    for (int j=0; j<repeatedOriginalSolutionList.count(); j++)
    {
        list1 = repeatedOriginalSolutionList.at(j);
        for (int k=0; k<list1.count(); k++)
        {
            individual = list1.at(k);
            discoveryOriginal[pos] = individual->getPerformanceDiscovery();
            latencyOriginal[pos] = individual->getPerformanceLatency();
            pos++;

            //qDebug("tamano del vector discovery: %s", qPrintable(QString::number(discovery.count())));
            //qDebug("tamano del vector latency: %s", qPrintable(QString::number(latency.count())));
        }
    }

    // llenar el vector modificado con los valores de las funciones objetivo
    //Individual * individual;
    QList<Individual *> list2;
    int modificatedCounter = getCountOfNonDominatedInModificatedRepetitions();

    QVector<double> discoveryModificated(modificatedCounter), latencyModificated(modificatedCounter);
    pos = 0;

    for (int j=0; j<repeatedModificatedSolutionList.count(); j++)
    {
        list2 = repeatedModificatedSolutionList.at(j);
        for (int k=0; k<list2.count(); k++)
        {
            individual = list2.at(k);
            discoveryModificated[pos] = individual->getPerformanceDiscovery();
            latencyModificated[pos] = individual->getPerformanceLatency();
            pos++;

            //qDebug("tamano del vector discovery: %s", qPrintable(QString::number(discovery.count())));
            //qDebug("tamano del vector latency: %s", qPrintable(QString::number(latency.count())));
        }
    }


    // generar el grafico de todas las soluciones no dominadas para el
    // algoritmo original y el modificado

    ui->customPlotExecutions->legend->setVisible(true);
    QFont legendFont = font();  // start out with MainWindow's font..
    legendFont.setPointSize(9); // and make a bit smaller for legend
    ui->customPlotExecutions->legend->setFont(legendFont);
    ui->customPlotExecutions->legend->setBrush(QBrush(QColor(255,255,255,230)));
    // by default, the legend is in the inset layout of the main axis rect. So this is how we access it to change legend placement:
    ui->customPlotExecutions->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom|Qt::AlignRight);
    ui->customPlotExecutions->clearGraphs();

    // create graph and assign data to it:
    ui->customPlotExecutions->addGraph();
    ui->customPlotExecutions->graph(0)->setPen(QPen(Qt::blue)); // line color blue for first graph
    ui->customPlotExecutions->graph(0)->setData(discoveryOriginal, latencyOriginal);
    //ui->customPlotExecutions->graph(0)->setLineStyle(QCPGraph::lsLine);
    ui->customPlotExecutions->graph(0)->setLineStyle(QCPGraph::lsNone);
    ui->customPlotExecutions->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, Qt::red, 4));
    ui->customPlotExecutions->graph(0)->setName("Unmodified algorithm");


    ui->customPlotExecutions->addGraph();
    ui->customPlotExecutions->graph(1)->setPen(QPen(Qt::green)); // line color green for second graph
    ui->customPlotExecutions->graph(1)->setData(discoveryModificated, latencyModificated);
    ui->customPlotExecutions->graph(1)->setLineStyle(QCPGraph::lsNone);
    ui->customPlotExecutions->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, Qt::green, 4));
    ui->customPlotExecutions->graph(1)->setName("Modified algorithm");

    // puntos del frente de pareto algoritmo original
    ui->customPlotExecutions->addGraph();
    ui->customPlotExecutions->graph(2)->setPen(QPen(Qt::blue)); // line color blue for first graph
    ui->customPlotExecutions->graph(2)->setData(discoveryParetoOriginal, latencyParetoOriginal);
    ui->customPlotExecutions->graph(2)->setLineStyle(QCPGraph::lsLine);
    ui->customPlotExecutions->graph(2)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, Qt::red, 4));
    ui->customPlotExecutions->graph(2)->setName("Unmodified Pareto Front");

    // puntos del frente de pareto algoritmo modificado
    ui->customPlotExecutions->addGraph();
    ui->customPlotExecutions->graph(3)->setPen(QPen(Qt::green)); // line color blue for first graph
    ui->customPlotExecutions->graph(3)->setData(discoveryParetoModificated, latencyParetoModificated);
    ui->customPlotExecutions->graph(3)->setLineStyle(QCPGraph::lsLine);
    ui->customPlotExecutions->graph(3)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, Qt::green, 4));
    ui->customPlotExecutions->graph(3)->setName("Modified Pareto Front");




    // give the axes some labels:
    ui->customPlotExecutions->xAxis->setLabel("Discovery ( Number of APs)");
    ui->customPlotExecutions->yAxis->setLabel("Latency (ms)");
    // set axes ranges, so we see all data:
    ui->customPlotExecutions->xAxis->setRange(0, 75);
    ui->customPlotExecutions->yAxis->setRange(0, 300);

    ui->customPlotExecutions->yAxis->grid()->setSubGridVisible(true);

    ui->customPlotExecutions->replot();

    // imprimir las individuos
    reportIndividualAsFile(getNonDominatedIndividualsFromList(repeatedOriginalSolutionList), "individuosAlgoritmoOriginal");
    reportIndividualAsFile(getNonDominatedIndividualsFromList(repeatedModificatedSolutionList), "individuosAlgoritmoModificado");

    // limpiar las listas para las siguientes ejecuciones
    repeatedOriginalSolutionList.clear();
    repeatedModificatedSolutionList.clear();

}

int MainWindow::getCountOfNonDominatedInOriginalRepetitions()
{
    // identificar el número total de individuos de todas las ejecuciones
    int counter = 0;
    for (int x=0; x<repeatedOriginalSolutionList.count(); x++)
    {
        counter = counter + repeatedOriginalSolutionList.at(x).count();
    }
    return counter;
}

int MainWindow::getCountOfNonDominatedInModificatedRepetitions()
{
    // identificar el número total de individuos de todas las ejecuciones
    int counter = 0;
    for (int x=0; x<repeatedModificatedSolutionList.count(); x++)
    {
        counter = counter + repeatedModificatedSolutionList.at(x).count();
    }
    return counter;
}

double MainWindow::getMeanExecutionTime(QList<double> l)
{

    double sumOfExecutionTime= 0;

    for (int j=0; j<l.count(); j++)
    {
        sumOfExecutionTime = sumOfExecutionTime + l.at(j);

    }
    return sumOfExecutionTime/l.count();

}


double MainWindow::getStdDeviationExecutionTime(QList<double> l, double mean)
{
    double numerator = 0;
    double denominator = l.count();

    for (int j=0; j<l.count(); j++)
    {
        numerator = numerator + pow((l.at(j)-mean),2);

    }
    return sqrt(numerator/denominator);
}

QList<Individual*> MainWindow::getNonDominatedIndividualsFromList(QList<QList<Individual *> > list)
{
    QList<Individual *> individualList;

    QList<Individual*> auxiliaryList;
    Individual * individual;
    for (int j=0; j<list.count(); j++)
    {
        auxiliaryList = list.at(j);
        for (int k=0; k<auxiliaryList.count(); k++)
        {
            individual = auxiliaryList.at(k);
            individualList.append(individual);
        }
    }
    return individualList;
}


QList<Individual*> MainWindow::getNonDominatedIndivualsFromRepetitions(bool original)
{
    QList<Individual*> auxiliaryList;
    if (original)
    {
        auxiliaryList = getNonDominatedIndividualsFromList(repeatedOriginalSolutionList);
    }
    else
    {
        auxiliaryList = getNonDominatedIndividualsFromList(repeatedModificatedSolutionList);
    }

    QList<Individual*> nonDominatedListToReturn;

    ExternalFile auxiliaryExternalFile;
    int p = auxiliaryList.count();

    Individual * individualI;
    Individual * individualJ;

    for (int i=0; i<p; i++)
    {
        bool dominated = false;
        individualI = auxiliaryList.at(i);

        for (int j=0; ((j<p) && (!dominated)); j++)
        {

            if (i==j)
            {
                continue;
            }
            individualJ = auxiliaryList.at(j);
            if (auxiliaryExternalFile.individualDominate(individualJ, individualI))
            {
                dominated = true;
            }
        }
        if (!dominated)
        {
            nonDominatedListToReturn.append(individualI);
        }
    }
    return nonDominatedListToReturn;


}

void MainWindow::reportIndividualAsFile(QList<Individual*> list, QString fileName)
{
    QFile file("/tmp/"+fileName+".txt");
    if (file.exists())
    {
        file.remove();
    }
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
    {
        QString msg = "No se pudo crear el archivo /tmp/"+fileName+".txt";
        qDebug(qPrintable(msg));
        return;
    }
    QTextStream out(&file);
    out << endl << "/tmp/"+fileName+".txt - Individuos encontrados luego de ejecutar el algoritmo cultural: " << "\n";

    QString aux;
    for(int i=0; i<list.count(); i++)
    {
        out << list.at(i)->getIndividualAsQString() << "\n";
    }
}

void MainWindow::populateAListView(QList<Individual*> list, QListView * listView)
{

    // ordenar la lista en orden ascendente de acuerdo a la latencia (F2)
    qSort(list.begin(), list.end(), xLessThanLatency);


    QStringList individuals;

    QString aux;
    for (int i=0; i<list.count(); i++)
    //for (int z=simulation->getExternalFile()->getExternalFileList().count()-1; z>=0; z-- )
    {
        aux.append(list.at(i)->getIndividualAsQString());
        individuals << aux;
        aux.clear();
    }

    QStringListModel *model = new QStringListModel();
    model->setStringList(individuals);

    //ui->listViewPFO->setModel(model);
    listView->setModel(model);

    if (listView->objectName() == "listViewPFOriginal")
    {
        ui->labelNumberPFOriginal->setText(QString::number(list.count()));
    }else if (listView->objectName() == "listViewPFModificated")
    {
        ui->labelNumberPFModificated->setText(QString::number(list.count()));
    }
}
