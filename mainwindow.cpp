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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setFixedSize(835, 683);

    // Validadores para los parametros del algoritmo
    QValidator * validatorPopSize = new QIntValidator(1, 1000, this);
    ui->lineEditPopulationSize->setValidator(validatorPopSize);
    ui->lineEditPopulationSize->setToolTip("[1..1000]");

    QValidator * validatorGenerations = new QIntValidator(1, 50, this);
    ui->lineEditGenerationNumber->setValidator(validatorGenerations);
    ui->lineEditGenerationNumber->setToolTip("[1..50]");

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

    QValidator * validatorGnormative = new QIntValidator(1, 20, this);
    ui->lineEditGnormative->setValidator(validatorGnormative);
    ui->lineEditGnormative->setToolTip("[1..20]");

    int matches = ui->lineEditPopulationSize->text().toInt()/2;
    QValidator * validatorMatchesPerTournament= new QIntValidator(1, matches, this);
    ui->lineEditMatchPerTournament->setValidator(validatorMatchesPerTournament);
    ui->lineEditMatchPerTournament->setText(QString::number(matches));
    ui->lineEditMatchPerTournament->setToolTip("Un valor recomendado es Poblacion/2");

    QDoubleValidator * validatorDirectedMutation = new QDoubleValidator(0.0, 1.0, 2, validatorDirectedMutation);
    validatorDirectedMutation->setNotation(QDoubleValidator::StandardNotation);
    ui->lineEditDirectedMutation->setValidator(validatorDirectedMutation);
    ui->lineEditDirectedMutation->setToolTip("[0..1]");



    connect(ui->pushButtonRun, SIGNAL(clicked()), this, SLOT(executeAlgorithm()));

    connect(ui->checkBoxDirectedMutation, SIGNAL(stateChanged(int)), this, SLOT(activateDirectedMutation(int)));
    ui->labelDirectedMutation->setEnabled(false);
    ui->lineEditDirectedMutation->setEnabled(false);

    connect(ui->lineEditPopulationSize, SIGNAL(textChanged(const QString & )), this, SLOT(checkPopulationSize(const QString &)));


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
    // TODO

/*
    // encontrar los individuos no dominados de la poblacion
    QList<Individual *> nonDominatedList = simulation->getNonDominatedPopulationApproach1();
    qDebug("Numero de individuos en la poblacion no dominada:");
    qDebug(qPrintable(QString::number(nonDominatedList.count())));
    for (int i = 0; i < nonDominatedList.count(); i++)
    {
        nonDominatedList.at(i)->printIndividual();
    }
*/

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
        // TODO

        // realizar torneos y seleccionar poblacion
        simulation->selectPopulation();
        qDebug("...despues de simulation->selectPopulation()");

        // imprimir poblacion para depuracion
        simulation->printPopulation();

        // obtener los individuos no dominados
        nonDominatedList = simulation->getNonDominatedPopulationApproach1();
        qDebug("...Numero de individuos en la poblacion no dominada: %d", nonDominatedList.count());


        Individual * ind;
        qDebug("INDIVIDUOS no dominados antes de insertarlos en el archivo externo-------");
        for (int i = 0; i < nonDominatedList.count(); i++)
        {
            ind = nonDominatedList.at(i);
            ind->printIndividual();
        }
        qDebug("-------");

        qDebug("...despues de obtener los individuos no dominados");

        // agregar los individuos no dominados al archivo externo
        simulation->addNonDominatedIndividualsToExternalFile(nonDominatedList);
        //simulation->addNonDominatedIndividualsToExternalFile(simulation->getPopulationList());

        qDebug("INDIVIDUOS no dominados despues de insertarlos en el archivo externo: %d",
               simulation->getExternalFile()->getExternalFileList().count());


        // actualizar el espacio de creencias con los individos aceptados
        if (countOfGenerations == simulation->getgNormative())
        {
            qDebug("MainWindow.cpp: numero de generaciones transcurridas: %d ahora actualizar parte normativa", countOfGenerations);
            simulation->updateNormativePhenotypicPart();
        }


        // actualizar la rejilla con todos los individuos no dominados recien agregados al archivo externo
        // durante la generación actual
        //
        // TODO: se necesita una funcion que retorne los individuos recien agregados al archivo externo durante la
        // generacion actual

        simulation->updateGrid(simulation->getExternalFile()->getCurrentGenerationIndividualList());
        simulation->getExternalFile()->resetCurrentGenerationIndividualList();
        //qDebug("...despues de actualizar la rejilla");
/*
        if (countOfGenerations == simulation->getgNormative())
        {
            // actualizar la parte fenotipica normativa en caso de que hayan pasado gNormativa generaciones
            simulation->updateNormativePhenotypicPart();
            qDebug("despues de updateNormativePhenotypicPart()");
            countOfGenerations = 0;
        }
*/


        qDebug("generacion actual: %d", simulation->getCurrentGenerationNumber());

        qDebug("****************************************************************************");
        QString aux;
        for (int z=0; z<simulation->getExternalFile()->getExternalFileList().count(); z++)
        {
            simulation->getExternalFile()->getExternalFileList().at(z)->printIndividual();
        }
        qDebug("****************************************************************************");

        QMessageBox msg;
        QString string = "Ver el Archivo externo al final de la generacion ";
        string.append(QString::number(simulation->getCurrentGenerationNumber()));
        msg.setText(string);
        //msg.exec();


        simulation->incrementGeneration();

        // incrementar contador de generaciones para actualizar parte fenotipica normativa
        countOfGenerations++;

    }while(!simulation->stopEvolution()); // fin de la repeticion

    qDebug("*********");
    qDebug("TERMINO EL ALGORITMO CULTURAL!");


    // poblar la lista de individuos no dominados del archivo externo
    populateListView();

    // generar el grafico
    setupCustomPlot(ui->customPlot);

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
