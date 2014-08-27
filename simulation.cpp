#include "simulation.h"

#include <QFile>
#include <QMessageBox>
#include <QTextStream>

/**
 * @brief Funcion de comparacion de individuos con respecto al valor de desempeno de descubrimiento
 * @param p1 Individuo 1 a comparar
 * @param p2 Individuo 2 a comparar
 * @return Verdadero si p1 es menor que p2 con respecto a la funcion objetivo de descubrimiento
 */
inline static bool xLessThanF1(Individual *p1, Individual *p2)
{
    return p1->getPerformanceDiscovery() < p2->getPerformanceDiscovery();
}


/**
 * @brief Funcion de comparacion de individuos con respecto al valor de desempeno de latencia
 * @param p1 Individuo 1 a comparar
 * @param p2 Individuo 2 a comparar
 * @return Verdadero si p1 es menor que p2 con respecto a la funcion objetivo de latencia
 */
inline static bool xLessThanF2(Individual *p1, Individual *p2)
{
    return p1->getPerformanceLatency() < p2->getPerformanceLatency();
}

/**
 * @brief Define e inicializa el miembro estatico individualIdCounter
 */
int Simulation::individualIdCounter = 0;


Simulation::Simulation(int population, int extFileSize, int generations, int subintervalsGrid, int genNormative,
                       int matches, int stdDev, int aps, bool dMutation, double dMutationProbability)
{
    populationSize = population;

    externalFileSize = extFileSize;

    generationsMax = generations;

    currentGeneration = 1;

    gridSubintervalsNumber = subintervalsGrid;

    gNormative = genNormative;

    matchesPerIndividuals = matches;

    stdDeviation = stdDev;

    deployedAPs = aps;

    directedMutation = dMutation;

    directedMutationProbability = dMutationProbability;

    normativePhenotipicPart = new NormativePhenotypicPart();

    externalFile = new ExternalFile(extFileSize);

    mutation = new Mutation();

    selection = new Selection();

    qDebug("Simulation:");
    qDebug("    tamano de la poblacion: %d", populationSize);
    qDebug("    numero de generaciones: %d", generationsMax);
    qDebug("    Gnormative: %d", gNormative);
    qDebug("    numero de torneos por individuo: %d", matchesPerIndividuals);
    qDebug("    desviacion estandar: %d", stdDeviation);
    qDebug("    numero de APs desplegados: %d", deployedAPs);
}


Simulation::~Simulation()
{
    delete normativePhenotipicPart;
    delete nGrid;
    delete externalFile;
    delete mutation;
}

int Simulation::getNewindividualId()
{
    int newId = individualIdCounter++;
    return newId;
}

void Simulation::initializePopulation()
{
    Individual * individuo;


    QFile file("/tmp/algorithmResult.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
    {
        QMessageBox msg;
        msg.setText("Simulation::initializePopulation(): No se pudo abrir el archivo /tmp/algorithmResult.txt\n para escribir resultados de la ejecucion del algoritmo.");
        return;
    }
    QTextStream out(&file);
    out << endl << "Inicializacion de la poblacion." <<"\n";


    // inicializacion de la poblacion
    for (int i = 0; i < populationSize; i++)
    {
        individuo = new Individual(deployedAPs);
        individuo->printIndividual();
        qDebug("individualId: %d", individuo->getIndividualId());
        populationList.append(individuo);

        out << individuo->getIndividualAsQString() << endl;
    }
    qDebug("tamano de la poblacion: %d",populationList.count());
    //return populationList;
}

QList<Individual *>  Simulation::getPopulationList()
{
    return populationList;
}

void Simulation::initializeNormativePhenotypicPart()
{

    // obtener la lista de los individuos no dominados de la poblacion inicial
    QList<Individual *> initialNonDominatedPopulation;

    initialNonDominatedPopulation = getNonDominatedPopulationApproach1();

    // ordenar los no dominados con respecto a la funcion objetivo 1 de menor a mayor
    qSort(initialNonDominatedPopulation.begin(), initialNonDominatedPopulation.end(), xLessThanF1);

    // tomar los limites inferior y superior
    int lF1 = initialNonDominatedPopulation.at(0)->getPerformanceDiscovery();
    int uF1 = initialNonDominatedPopulation.at(initialNonDominatedPopulation.count()-1)->getPerformanceDiscovery();

    // ordenar los no dominados con respecto a la funcion objetivo 2 de menor a mayor
    qSort(initialNonDominatedPopulation.begin(), initialNonDominatedPopulation.end(), xLessThanF2);

    int lF2 = initialNonDominatedPopulation.at(0)->getPerformanceLatency();
    int uF2 = initialNonDominatedPopulation.at(initialNonDominatedPopulation.count()-1)->getPerformanceLatency();

    // asigna los extremos de las funciones objetivo con respecto a los individuos no dominados
    normativePhenotipicPart->updateNormativePhenotypicPart(lF1, uF1, lF2, uF2);

    qDebug("| lF1: %f | uF1: %f | lF2: %f | uF2: %f |",
           normativePhenotipicPart->getLowerF1(),normativePhenotipicPart->getUpperF1(),
           normativePhenotipicPart->getLowerF2(), normativePhenotipicPart->getUpperF2());


    QFile file("/tmp/algorithmResult.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
    {
        QMessageBox msg;
        msg.setText("Simulation::initializeNormativePhenotypicPart(): No se pudo abrir el archivo /tmp/algorithmResult.txt para escribir \nresultados de la ejecucion del algoritmo.");
        return;
    }

    QTextStream out(&file);
    out << endl <<"Inicializacion de la parte normativa fenotipica." <<"\n";

    out << "| lF1: " << normativePhenotipicPart->getLowerF1() <<
           "| uF1: " << normativePhenotipicPart->getUpperF1() <<
           "| lF2: " << normativePhenotipicPart->getLowerF2() <<
           "| uF2: " << normativePhenotipicPart->getUpperF2() << "|" <<endl;


}

void Simulation::initializeGrid()
{
    nGrid = new NormativeGrid(gridSubintervalsNumber, normativePhenotipicPart);

    QFile file("/tmp/algorithmResult.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
    {
        QMessageBox msg;
        msg.setText("Simulation::initializeGrid(): No se pudo abrir el archivo /tmp/algorithmResult.txt para escribir \nresultados de la ejecucion del algoritmo.");
        return;
    }

    QTextStream out(&file);
    out << endl <<"Inicializacion de la rejilla." <<"\n";


}


void Simulation::updateNormativePhenotypicPart()
{
    qDebug("Simulation::updateNormativePhenotypicPart");

    QList<Individual *> extFileNonDominatedPopulation;

    // obtener la lista de individuos no dominados del archivo externo
    extFileNonDominatedPopulation = externalFile->getExternalFileList();

    // crear lista temporal de individuos no dominados del archivo externo unidos con
    // los individuos que cayeron fuera de la rejilla en la generacion actual
    QList<Individual *> extFileAndOutOfGridIndividualList;
    extFileAndOutOfGridIndividualList = externalFile->getExternalFileList();

    for (int i=0; i<getOutOfGridIndividualList().count();i++)
    {
        extFileAndOutOfGridIndividualList.append(getOutOfGridIndividualList().at(i));
    }


    // ordenarlos los no dominados con respecto a la funcion objetivo 1 de menor a mayor
    qSort(extFileAndOutOfGridIndividualList.begin(), extFileAndOutOfGridIndividualList.end(), xLessThanF1);

    // tomar los limites inferior y superior
    int lF1 = extFileAndOutOfGridIndividualList.at(0)->getPerformanceDiscovery();
    int uF1 = extFileAndOutOfGridIndividualList.at(extFileAndOutOfGridIndividualList.count()-1)->getPerformanceDiscovery();

    // ordenarlos los no dominados con respecto a la funcion objetivo 2 de menor a mayor
    qSort(extFileAndOutOfGridIndividualList.begin(), extFileAndOutOfGridIndividualList.end(), xLessThanF2);

    int lF2 = extFileAndOutOfGridIndividualList.at(0)->getPerformanceLatency();
    int uF2 = extFileAndOutOfGridIndividualList.at(extFileAndOutOfGridIndividualList.count()-1)->getPerformanceLatency();

    // asigna los extremos de las funciones objetivo con respecto a los individuos no dominados
    normativePhenotipicPart->updateNormativePhenotypicPart(lF1, uF1, lF2, uF2);

    qDebug("nueva parte fenotipica normativa:");
    qDebug("| lF1: %f | uF1: %f | lF2: %f | uF2: %f |",
           normativePhenotipicPart->getLowerF1(),normativePhenotipicPart->getUpperF1(),
           normativePhenotipicPart->getLowerF2(), normativePhenotipicPart->getUpperF2());

    // Reconstruir la rejilla con los nuevos valores de lowerF1, upperF1, lowerF2, upperF2.
    delete nGrid;

    // Reinicializar todos los contadores de la rejilla en cero.
    nGrid = new NormativeGrid(gridSubintervalsNumber, normativePhenotipicPart);

    nGrid->printGrid();

    // Agregar todos los individuos del archivo externo al contador de su celda correspondiente.
    // De esta manera el espacio de creencias está listo de nuevo para su uso.
    updateGrid(extFileNonDominatedPopulation);

    qDebug("++++++grid despues de actualizada con los individuos del archivo externo");
    nGrid->printGrid();
    qDebug("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");

    extFileAndOutOfGridIndividualList.clear();
    outOfGridIndividualList.clear();



/*
    qDebug("Simulation::updateNormativePhenotypicPart");

    QList<Individual *> extFileNonDominatedPopulation;

    // obtener la lista de individuos no dominados del archivo externo
    extFileNonDominatedPopulation = externalFile->getExternalFileList();

    // ordenarlos los no dominados con respecto a la funcion objetivo 1 de menor a mayor
    qSort(extFileNonDominatedPopulation.begin(), extFileNonDominatedPopulation.end(), xLessThanF1);

    // tomar los limites inferior y superior
    int lF1 = extFileNonDominatedPopulation.at(0)->getPerformanceDiscovery();
    int uF1 = extFileNonDominatedPopulation.at(extFileNonDominatedPopulation.count()-1)->getPerformanceDiscovery();

    // ordenarlos los no dominados con respecto a la funcion objetivo 2 de menor a mayor
    qSort(extFileNonDominatedPopulation.begin(), extFileNonDominatedPopulation.end(), xLessThanF2);

    int lF2 = extFileNonDominatedPopulation.at(0)->getPerformanceLatency();
    int uF2 = extFileNonDominatedPopulation.at(extFileNonDominatedPopulation.count()-1)->getPerformanceLatency();

    // asigna los extremos de las funciones objetivo con respecto a los individuos no dominados
    normativePhenotipicPart->updateNormativePhenotypicPart(lF1, uF1, lF2, uF2);

    qDebug("nueva parte fenotipica normativa:");
    qDebug("| lF1: %f | uF1: %f | lF2: %f | uF2: %f |",
           normativePhenotipicPart->getLowerF1(),normativePhenotipicPart->getUpperF1(),
           normativePhenotipicPart->getLowerF2(), normativePhenotipicPart->getUpperF2());

    // Reconstruir la rejilla con los nuevos valores de lowerF1, upperF1, lowerF2, upperF2.
    delete nGrid;

    // Reinicializar todos los contadores de la rejilla en cero.
    nGrid = new NormativeGrid(gridSubintervalsNumber, normativePhenotipicPart);

    nGrid->printGrid();

    // Agregar todos los individuos del archivo externo al contador de su celda correspondiente.
    // De esta manera el espacio de creencias está listo de nuevo para su uso.
    updateGrid(extFileNonDominatedPopulation);

    qDebug("++++++grid despues de actualizada con los individuos del archivo externo");
    nGrid->printGrid();
    qDebug("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
*/
}


void Simulation::updateGrid(QList<Individual *> nonDominated)
{

    // Para actualizar la rejilla simplemente se incrementan los contadores de los
    // individuos no dominados con todos los individuos recién agregados al
    // archivo externo durante la generación actual.

/*
    Individual * ind = nonDominated.at(0);
    nGrid->addIndividualToGrid(ind);

    ind = nonDominated.at(1);
    nGrid->addIndividualToGrid(ind);
*/
    qDebug("Simulation::updateGrid");
    Individual * auxIndividual;
    for (int i=0; i<nonDominated.count(); i++)
    {
        auxIndividual = nonDominated.at(i);

        if(!nGrid->individualInsideGrid(auxIndividual))
        {
            //qDebug("%%%%%%%% el individuo no pertenece a la grid");
            //auxIndividual->printIndividual();
            //outOfGridIndividualList.append(auxIndividual);
        }
        else
        {
            nGrid->addIndividualToGrid(auxIndividual);
        }
    }
}

void Simulation::printGrid()
{
    nGrid->printGrid();
}

void Simulation::mutatePopulation()
{
    // utilizar la mutación dirigida
//    if (directedMutation)
//    {
//        mutation->doDirectedMutation(populationList, getStdDeviation(), deployedAPs, directedMutationProbability);
//    }
//    else
//    {
        mutation->doMutation(populationList, getStdDeviation(), deployedAPs);
//    }

    mutatedPopulationList = mutation->getNewPopulation();

    mutation->printNewPopulation();

    // agregar resultados a archivo
    QFile file("/tmp/algorithmResult.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
    {
        QMessageBox msg;
        msg.setText("Simulation::mutatePopulation(): No se pudo abrir el archivo /tmp/algorithmResult.txt\n para escribir resultados de la ejecucion del algoritmo.");
        msg.exec();
        return;
    }
    QTextStream out(&file);
    out << endl << "Mutacion de la poblacion." <<"\n";
    out << endl;

    Individual * auxIndividual;
    for (int i=0; i<mutatedPopulationList.count(); i++)
    {
        auxIndividual = mutatedPopulationList.at(i);
        out << auxIndividual->getIndividualAsQString() << endl;
    }
}

void Simulation::selectPopulation()
{
    selection->doSelection(mutatedPopulationList, matchesPerIndividuals, nGrid);

    populationList = selection->getSelectedPopulation();

    //
    outOfGridIndividualList = selection->getOutOfGridList();

    for (int i=0; i<selection->getOutOfGridList().count(); i++)
    {
        outOfGridIndividualList.append(selection->getOutOfGridList().at(i));
    }



    // agregar resultados a archivo
    QFile file("/tmp/algorithmResult.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
    {
        QMessageBox msg;
        msg.setText("Simulation::selectPopulation(): No se pudo abrir el archivo /tmp/algorithmResult.txt\n para escribir resultados de la ejecucion del algoritmo.");
        return;
    }
    QTextStream out(&file);
    out << endl << "Seleccion de poblacion de tamano P." <<"\n";
    out << endl;

    Individual * auxIndividual;
    for (int i=0; i<populationList.count(); i++)
    {
        auxIndividual = populationList.at(i);
        out << auxIndividual->getIndividualAsQString() << endl;
    }

}


void Simulation::addNonDominatedIndividualsToExternalFile(QList<Individual *> ndIndividualList)
{
    externalFile->addNonDominatedIndividuals(ndIndividualList, nGrid);


    // agregar resultados a archivo
    QFile file("/tmp/algorithmResult.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
    {
        QMessageBox msg;
        msg.setText("Simulation::addNonDominatedIndividualsToExternalFile(): No se pudo abrir el archivo /tmp/algorithmResult.txt\n para escribir resultados de la ejecucion del algoritmo.");
        return;
    }
    QTextStream out(&file);
    out << endl << "Individuos del archivo externo." <<"\n";
    out << endl;

    Individual * auxIndividual;
    for (int i=0; i<externalFile->getExternalFileList().count(); i++)
    {
        auxIndividual = externalFile->getExternalFileList().at(i);
        out << auxIndividual->getIndividualAsQString() << endl;
    }

}

void Simulation::incrementGeneration()
{
    currentGeneration++;
}

int Simulation::getCurrentGenerationNumber()
{
    return currentGeneration;
}

bool Simulation::stopEvolution()
{
    if (currentGeneration > generationsMax)
        return true;
    else
        return false;
}



QList<Individual *> Simulation::getNonDominatedPopulationApproach1()
{
    qDebug("...Simulation::getNonDominatedPopulationApproach1");

    // populationList es la lista de individuos
    QList<Individual *> nonDominatedPopulation;
    int p = populationList.count();

    Individual * individualI;
    Individual * individualJ;

    for (int i=0; i<p; i++)
    {
        bool dominated = false;
        individualI = populationList.at(i);

        for (int j=0; ((j<p) && (!dominated)); j++)
        {

            if (i==j)
            {
                continue;
            }
            individualJ = populationList.at(j);
            if (individualDominate(individualJ, individualI))
            {
                dominated = true;
            }
        }
        if (!dominated)
        {
            nonDominatedPopulation.append(individualI);
        }
    }
    return nonDominatedPopulation;


}

QList<Individual *> Simulation::getNonDominatedPopulationApproach2()
{
    qDebug("...Simulation::getNonDominatedPopulationApproach2");

    int p = populationList.count();

    Individual * individualI;
    Individual * individualJ;

    // populationList es la lista de individuos
    QList<Individual *> nonDominatedPopulation;
    nonDominatedPopulation.append(populationList.at(0));

    int nonDP = nonDominatedPopulation.count();

    int i = 1;
    int j;

    while(i < p)
    {
        individualI = populationList.at(i);
        j = 0;

        while (j < nonDP)
        {
            individualJ = populationList.at(j);
            if ( individualDominate(individualI, individualJ) )
            {
                nonDominatedPopulation.removeAt(j);
            }
            else if (individualDominate(individualJ, individualI)) {
                continue;
            }
            j++;
        }
        if (j == nonDP)
        {
            nonDominatedPopulation.append(individualI);
        }
        i++;
    }

    return nonDominatedPopulation;
}


bool Simulation::individualDominate(Individual * xj, Individual * xi)
{
    //qDebug("... ... Simulation::individualDominate");

    // a solution xj is said to dominate another solution xi, and we write xj <| xi if both
    // the following conditions are true:
    //
    // condition a: Fi(xj) <= Fi(xi) to i E 1,2
    //
    // confition b: Exists j 1, 2 such that Fj(xj) < Fj(xi)

    bool conditionA = false;
    bool conditionB = false;


    // condition a
    if ( (xj->getPerformanceDiscovery() >= xi->getPerformanceDiscovery()) &&
         (xj->getPerformanceLatency() <= xi->getPerformanceLatency()) )
    {
        conditionA = true;
    }

    // condition b
    if ( (xj->getPerformanceDiscovery() > xi->getPerformanceDiscovery()) ||
         (xj->getPerformanceLatency() < xi->getPerformanceLatency()) )
    {
        conditionB = true;
    }

    if ( (conditionA) && (conditionB) )
    {
        return true;
    }else
    {
        return false;
    }
}


void Simulation::setExternalFile(ExternalFile * extFile)
{
    externalFile = extFile;
}

ExternalFile * Simulation::getExternalFile()
{
    return externalFile;
}


double Simulation::getStdDeviation()
{
    return stdDeviation;
}


void Simulation::printPopulation()
{
    for (int i = 0; i < populationList.count(); i++)
    {
        populationList.at(i)->printIndividual();
    }
}


int Simulation::getgNormative()
{
    return gNormative;
}

QList<Individual *> Simulation::getOutOfGridIndividualList()
{
    return outOfGridIndividualList;
}


void Simulation::addIndividualToOutOfGridIndividualList(Individual * outOfGridIndividual)
{
    // verificar que el individuo no exista; si no existe se agrega en caso contrario se ignora

    Individual * auxIndividual;

    for (int i=0; i<outOfGridIndividualList.count(); i++)
    {
        auxIndividual = outOfGridIndividualList.at(i);

        if (auxIndividual->getIndividualId() == outOfGridIndividual->getIndividualId())
        {

        }
        else
        {
            outOfGridIndividualList.append(outOfGridIndividual);
        }
    }
}
