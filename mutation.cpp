#include "mutation.h"

//#include "scan.h"

#include "QMessageBox"

#include <random>
#include <chrono>

/**
 * @brief Funcion de comparacion de celdas con respecto al valor de individuos en la lista
 * @param p1 Celda 1 a comparar
 * @param p2 Celda 2 a comparar
 * @return Verdadero si p1 es menor que p2 con respecto a la funcion objetivo de descubrimiento
 */
inline static bool cell1LessThanCell2(Cell * cell1, Cell * cell2)
{
    return cell1->getCount() < cell2->getCount();
}

Mutation::Mutation()
{
    newPopulation.clear();

    stdDeviation = 0;

    // inicializar el diccionario de canales utilizados en la mutacion en falso
    for (int i=1; i<=11;i++)
    {
        channelsUsedForMutation[i]=false;
    }

}


Mutation::~Mutation()
{

}

void Mutation::doMutation(QList<Individual *> population, double std, int deployedAp)
{
    newPopulation.clear();

    Individual * father;
    Individual * offspring;

    qDebug(" ----- doMutation: tamano inicial de newPopulation %d", newPopulation.count());

    int newParameterValue = 0;

    // recorrer la lista de poblacion
    for (int i=0; i<population.count(); i++)
    {
        father = population.at(i);
        offspring = new Individual(deployedAp);
        //qDebug("===== offspring id: %d", offspring->getIndividualId());

        // crear un individuo (offspring) y mutar todos sus parametros
        for (int i=0; i<father->getNumberOfParameters(); i++)
        {
            newParameterValue = mutateIndividualParameter(i, 0 /*father->getParameter(i)*/,std, father->getParameter(i), offspring);
            offspring->setParameter(i, newParameterValue);

        }
        // se muto el offspring ahora limpiar el diccionario de canales usados
        // asignar el diccionario de canales utilizados en la mutacion en falso
        for (int c=1; c<=11;c++)
        {
            channelsUsedForMutation[c]=false;
        }

        // evaluar el offspring con los nuevos valores de parametros
        offspring->calculateDiscoveryValue();
        offspring->calculateLatencyValue();

        // agregar el individuo padre y el individuo hijo a la lista newPopulation
        // newPopulation sera de tamano 2p
        newPopulation.append(father);
        newPopulation.append(offspring);

        //qDebug(" ----- domutation: tamano de newPopulation %d", newPopulation.count());
    }
    qDebug(" ----- doMutation: tamano final de newPopulation %d", newPopulation.count());
}


void Mutation::doDirectedMutation(QList<Individual *> population, double std,
                                  int deployedAp, double dMutationProbability, NormativeGrid * grid)
{
    qDebug("Mutation::doDirectedMutation con probabilidad %f", dMutationProbability);

    newPopulation.clear();

    double randomNumber = 0;

    Individual * father;

    // recorrer la lista de poblacion
    for (int i=0; i<population.count(); i++)
    {
        father = population.at(i);
        randomNumber = getRandomUniform();

        qDebug("Numero aleatorio: %f", randomNumber);
        qDebug("dMutationProbability: %f", dMutationProbability);

        if (randomNumber < dMutationProbability)
        {
            qDebug("--> directedMutation()");

            setStdDeviation(std);

            // hacer la mutacion dirigida
            // escribir una funcion
            directedMutation(grid, father);

            // agregar el father
            //newPopulation.append(father);
        }
        else
        {
            // hacer la mutacion gausiana con el papa como patron
            // escribir una funcion
            originalMutation(father, std, deployedAp);
        }

    }

}


QList<Individual *> Mutation::getNewPopulation()
{
    return newPopulation;
}

int Mutation::getRandom(int low, int high)
{
    return qrand() % ((high + 1) - low) + low;
}

double Mutation::getRandomUniform()
{
    return qrand()/double(RAND_MAX);
}

int Mutation::mutateIndividualParameter(int index, int mean, double std, double currentParameterValue, Individual * offspring)
{
    // mean representa el parametro sobre el cual se va a mutar
    // std la desviacion estandar de la distribucion normal

    // tomado de http://www.cplusplus.com/reference/random/normal_distribution/

    std::default_random_engine generator;
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    generator.seed(seed);

    std::normal_distribution<double>  distribution(mean,std);

    double yi = distribution(generator);

    //qDebug("--Mutar parametro de individuo--");
    //qDebug("   valor de la distribucion normal: %d, %d", mean, std);
    //qDebug(qPrintable(QString::number(yi)));

    // redondear el yi
    int intYi = qRound(yi);

    if (isThisParameterAChannel(index))
    {
        //qDebug("   isThisParameterAChannel(index)");
        intYi = getRandom(1,11);

        // verificar que el canal no se haya utilizado en mutaciones anteriores
        while (channelsUsedForMutation.value(intYi))
        {
            // seleccionar otro canal que no se haya seleccionado
            intYi = getRandom(1,11);
        }
        channelsUsedForMutation[intYi]=true;
        //qDebug(qPrintable("   channel despues de mutado: "+QString::number(intYi)));
    }
    else if (isThisParameterAMinChannelTime(index))
    {
        //qDebug("   isThisParameterAMinChannelTime(index)");
        intYi = intYi + currentParameterValue;
        if (intYi < 0)
        {
            intYi = 0;
            //qDebug("   el minChannelTime mutado esta por debajo del limite (index %d)", index);
        }
        else if (intYi > 10)
        {
            intYi = 10;
            //qDebug("   el minChannelTime mutado esta por encima del limite (index %d)", index);
        }

        //qDebug(qPrintable("   minChannelTime despues de mutado: "+QString::number(intYi)));
    }
    else if (isThisParameterAMaxChannelTime(index))
    {
        //qDebug("   isThisParameterAMaxChannelTime(index)");
        intYi = intYi + currentParameterValue;
        if (intYi < 10)
        {
            intYi = 10;
            //qDebug("   el maxChannelTime mutado esta por debajo del limite (index %d)", index);
        }
        else if (intYi > 100)
        {
            intYi = 100;
            //qDebug("   el maxChannelTime mutado esta por encima del limite (index %d)", index);
        }

        //qDebug(qPrintable("   maxChannelTime despues de mutado: "+QString::number(intYi)));
    }
    else if (isThisParameterAPs(index))
    {
        //qDebug("   isThisParameterAPs(index)");
        //if (intYi<0)
        //{
        //    intYi = 0;
        //}

        intYi = getNewParameterAPs(offspring->getParameter(index-3),
                                   offspring->getParameter(index-2),
                                   offspring->getParameter(index-1));



        //qDebug(qPrintable("   APs despues de mutado: "+QString::number(intYi)));
    }

    //qDebug("----return----");
    return intYi;
}


bool Mutation::isThisParameterAChannel(int index)
{
    if ( (index == 0) || (index == 4) || (index == 8) || (index == 12) || (index == 16) ||
         (index == 20) || (index == 24) || (index == 28) || (index == 32) || (index == 36) || (index == 40) )
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Mutation::isThisParameterAMinChannelTime(int index)
{
    if ( (index == 1) || (index == 5) || (index == 9) || (index == 13) || (index == 17) ||
         (index == 21) || (index == 25) || (index == 29) || (index == 33) || (index == 37) || (index == 41) )
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Mutation::isThisParameterAMaxChannelTime(int index)
{
    if ( (index == 2) || (index == 6) || (index == 10) || (index == 14) || (index == 18) ||
         (index == 22) || (index == 26) || (index == 30) || (index == 34) || (index == 38) || (index == 42) )
    {
        return true;
    }
    else
    {
        return false;
    }
}


bool Mutation::isThisParameterAPs(int index)
{
    if ( (index == 3) || (index == 7) || (index == 11) || (index == 15) || (index == 19) ||
         (index == 23) || (index == 27) || (index == 31) || (index == 35) || (index == 39) || (index == 43) )
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Mutation::printNewPopulation()
{
    for (int i = 0; i < newPopulation.count(); i++)
    {
        newPopulation.at(i)->printIndividual();
    }
}


int Mutation::getNewParameterAPs(int channel, double minChannelTime, double maxChannelTime)
{
    //qDebug("Mutation::getNewParameterAPs(%d, %f, %f)", channel, minChannelTime, maxChannelTime);

    // base de datos de experimentos
    QString database("test_18.1.db");
    // tipo de experimento para extraer las muestras: full -> full scanning
    QString experiment("full");
    Scan scan(database.toStdString(),experiment.toStdString());
    scan.init();

    Scan::ScanResults results;

    results = scan.execute(channel, minChannelTime, maxChannelTime);

    //qDebug("** nuevo parametro AP: %d", results.size());
    return results.size();


}


void Mutation::originalMutation(Individual * father, double std, int deployedAp)
{
    qDebug("Mutation::originalMutation(Individual * father)");

    QMessageBox msg;
    msg.setText("*** MUTACION ORIGINAL***");
    //msg.exec();

    //father = population.at(i);

    Individual * offspring;
    offspring = new Individual(deployedAp);
    //qDebug("===== offspring id: %d", offspring->getIndividualId());

    int newParameterValue = 0;
    // crear un individuo (offspring) y mutar todos sus parametros
    for (int i=0; i<father->getNumberOfParameters(); i++)
    {
        newParameterValue = mutateIndividualParameter(i, 0 /*father->getParameter(i)*/,std, father->getParameter(i), offspring);
        offspring->setParameter(i, newParameterValue);

    }
    // se muto el offspring ahora limpiar el diccionario de canales usados
    // asignar el diccionario de canales utilizados en la mutacion en falso
    for (int c=1; c<=11;c++)
    {
        channelsUsedForMutation[c]=false;
    }

    // evaluar el offspring con los nuevos valores de parametros
    offspring->calculateDiscoveryValue();
    offspring->calculateLatencyValue();

    // agregar el individuo padre y el individuo hijo a la lista newPopulation
    // newPopulation sera de tamano 2p
    newPopulation.append(father);
    newPopulation.append(offspring);
}


void Mutation::directedMutation(NormativeGrid *grid, Individual *father)
{
    QMessageBox msg;
    msg.setText("*** MUTACION DIRIGIDA***");
    //msg.exec();
    //return;


    //Identificar las celdas de la rejilla que tienen contadores mayores a cero.
    // Agregar las celdas en una lista de Cells.
    // lista de celdas pobladas
    QList<Cell *> cellList = grid->getPopulatedCellList();
    qDebug("   numero de celdas con individuos: %d", cellList.count());

    //Ordenar la lista de menor a mayor cantidad de individuos por celda.
    qSort(cellList.begin(), cellList.end(), cell1LessThanCell2);


    for (int i=0; i<cellList.count();i++)
    {
        qDebug("Celda (%d-%d) con: %d individuos", cellList.at(i)->getSubintervalF1(),
        cellList.at(i)->getSubintervalF2(), cellList.at(i)->getCount());
    }

    // no hay como ejercer influencia en la mutacion
    if (cellList.count() == 0)
    {
        originalMutation(father, getStdDeviation(), /*deployedAp*/ 10);
        return;
    }
    else // existe al menos una celda con individuo en la rejilla
    {
        int randomCell = getRandom(0,cellList.count()-1);
        Cell * selectedCell = cellList.at(randomCell);

        // la celda tiene solo un individuo
        if (selectedCell->getCount() == 1)
        {
            qDebug("   la celda tiene un solo individuo");
            originalMutation(father, getStdDeviation(), /*deployedAp*/ 10);
            return;
        }
        else // la celda tiene dos o mas individuos
        {
            qDebug("   la celda tiene dos o mas individuos");
            for (int i=0; i<selectedCell->getCount();i++)
            {
                selectedCell->getIndividual(i)->printIndividual();
            }
            qDebug("----------------------------------------------------------------");

            // TODO
            QMessageBox msg;
            msg.setText("*** MUTACION DIRIGIDA***\nLa celda tiene dos o mas individuos!!!\TODO!");
            //msg.exec();



            // ----------------------------------------------------------------
            // primera etapa de la mutacion dirigida

            // TODO: establecer el umbral como un parametro
            double stdDevThreshold = 0.1;

            int numberOfIndividuals = selectedCell->getCount();

            Individual * preOffspring = new Individual(10);

            qDebug("-----preoffspring------");
            preOffspring->printIndividual();
            qDebug("-----------------------");

            QList<double> minList;
            QList<double> maxList;

            QHash<QString,double> meanAndStdDevHash;

            // verlo primero por bloque y luego por individuo...
            // iterar por bloque
            for (int i=0; i<11;i++)
            {
                // iterar por individuo
                for (int j=0; j<numberOfIndividuals;j++)
                {

                    //selectedCell->getIndividual(j)->printIndividual();
                    // obtener los min y max y agregarlos en las listas respectivas
                    minList.append(selectedCell->getIndividual(j)->getParameter((i*4)+1));
                    maxList.append(selectedCell->getIndividual(j)->getParameter((i*4)+2));
                }
                meanAndStdDevHash = calculateMeanAndStdDev(minList, maxList,0);
                minList.clear();
                maxList.clear();

                // verificar si la desviacion del min es menor que umbral
                if (meanAndStdDevHash.value("stdDevMinChannelTime") < stdDevThreshold)
                {
                    preOffspring->setParameter(((i*4)+1),meanAndStdDevHash.value("meanMinChannelTime"));
                }
                else
                {
                    // asignar el valor mutado del parametro del padre
                    preOffspring->setParameter(((i*4)+1),father->getParameter((i*4)+1));

                }
                // verificar si la desviacion del max es menor que umbral
                if (meanAndStdDevHash.value("stdDevMaxChannelTime") < stdDevThreshold)
                {
                    preOffspring->setParameter(((i*4)+2),meanAndStdDevHash.value("meanMaxChannelTime"));
                }
                else
                {
                    // asignar el valor mutado del parametro del padre
                    preOffspring->setParameter(((i*4)+2),father->getParameter((i*4)+2));
                }
                meanAndStdDevHash.clear();
            }
            qDebug("--MIN y MAX MUTADOS---");
            preOffspring->printIndividual();
            qDebug("-----");

            // ----------------------------------------------------------------
            // ----------------------------------------------------------------
            // segunda etapa de la mutacion dirigida
            // identificar la secuencia mas comun de escaneo


            QList <int> channelList;

            QList<int> patternSequence;
            int newChannel = 0;

            // verlo primero por bloque y luego por individuo...
            // iterar por bloque
            for (int i=0; i<11;i++)
            {
                // iterar por individuo
                for (int j=0; j<numberOfIndividuals;j++)
                {
                    // obtener el canal de cada individuo
                    //selectedCell->getIndividual(j)->printIndividual();
                    channelList.append(selectedCell->getIndividual(j)->getParameter((i*4)));
                }
                // operar sobre channelList
                newChannel = getPatternSequence(channelList);
                //qDebug("canal %d del patron: %d",i+1, newChannel);
                channelList.clear();
                patternSequence.append(newChannel);
            }

            qDebug("PATRON DE SECUENCIA: --------------------");
            QString seq;
            for (int i=0;i<patternSequence.count();i++)
            {
                seq.append(QString::number(patternSequence.at(i)));
                seq.append("-");
            }
            qDebug(qPrintable(seq));
            qDebug("------------------------------------------");

            QList<int> finalSequence;

            QHash<int,bool> usedChannels;
            for (int i=1;i<=11;i++)
            {
                usedChannels.insert(i,false);
            }


            for (int i=0; i<patternSequence.count();i++)
            {
                if (usedChannels.value(patternSequence.at(i)))
                {
                    finalSequence.append(0);
                }
                else
                {
                    usedChannels.insert(patternSequence.at(i),true);
                    finalSequence.append(patternSequence.at(i));
                }

            }
            qDebug("Secuencia final con ceros:");
            QString seq2;
            for (int i=0;i<finalSequence.count();i++)
            {
                seq2.append(QString::number(finalSequence.at(i)));
                seq2.append("-");
            }
            qDebug(qPrintable(seq2));
            qDebug("------------------------------------------");

            for (int i=0;i<finalSequence.count();i++)
            {
                if (finalSequence.at(i) == 0)
                {
                    for (int j=0;j<usedChannels.count();j++)
                    {
                        if (usedChannels.value(j+1) == false)
                        {
                            finalSequence.replace(i,j+1);
                            usedChannels.insert(j+1,true);
                            break;
                        }
                    }
                }
            }
            qDebug("Secuencia final despues de quitar repetidos:");
            QString seq3;
            for (int i=0;i<finalSequence.count();i++)
            {
                seq3.append(QString::number(finalSequence.at(i)));
                seq3.append("-");
            }
            qDebug(qPrintable(seq3));
            qDebug("------------------------------------------");


            for (int i=0; i<11; i++)
            {
                preOffspring->setParameter(i*4,finalSequence.at(i));
            }
            qDebug("*******************OFFSPRING*******************************");
            preOffspring->printIndividual();
            qDebug("***********************************************************");

            // ----------------------------------------------------------------

            // TODO: agregar el offspring a newPopulation
            // agregar el individuo padre y el individuo hijo a la lista newPopulation
            // newPopulation sera de tamano 2p
            newPopulation.append(father);
            newPopulation.append(preOffspring);
        }


    }



}


void Mutation::setStdDeviation(double std)
{
    stdDeviation = std;
}

double Mutation::getStdDeviation()
{
    return stdDeviation;
}


QHash<QString, double> Mutation::calculateMeanAndStdDev(QList<double> minChannelTimeList, QList<double> maxChannelTimeList, int genIndex)
{
    // media de minchanneltime

    double minChannelTimeSum = 0.0;
    double maxChannelTimeSum = 0.0;

    double meanMinChannelTime = 0.0;
    double meanMaxChannelTime = 0.0;

    double minChannelTimeStdSum = 0.0;
    double maxChannelTimeStdSum = 0.0;

    double stdDevMinChannelTime = 0.0;
    double stdDevMaxChannelTime = 0.0;

    QHash<QString, double> hash;

    for (int i=0;i<minChannelTimeList.count();i++)
    {
        minChannelTimeSum = minChannelTimeSum + minChannelTimeList.at(i);
    }
    meanMinChannelTime = minChannelTimeSum / minChannelTimeList.count();
    hash.insert("meanMinChannelTime", meanMinChannelTime);


    for (int j=0; j<minChannelTimeList.count(); j++)
    {
        minChannelTimeStdSum = minChannelTimeStdSum + pow((minChannelTimeList.at(j)-meanMinChannelTime),2);
    }
    stdDevMinChannelTime = (sqrt(minChannelTimeStdSum/minChannelTimeList.count()));
    hash.insert("stdDevMinChannelTime", stdDevMinChannelTime);


    maxChannelTimeSum = 0.0;
    for (int k=0;k<maxChannelTimeList.count();k++)
    {
        maxChannelTimeSum = maxChannelTimeSum + maxChannelTimeList.at(k);
    }
    meanMaxChannelTime = maxChannelTimeSum / maxChannelTimeList.count();
    hash.insert("meanMaxChannelTime", meanMaxChannelTime);

    for (int l=0; l<maxChannelTimeList.count(); l++)
    {
        maxChannelTimeStdSum = maxChannelTimeStdSum + pow((maxChannelTimeList.at(l)-meanMaxChannelTime),2);
    }
    stdDevMaxChannelTime = (sqrt(maxChannelTimeStdSum/maxChannelTimeList.count()));
    hash.insert("stdDevMaxChannelTime", stdDevMaxChannelTime);


    //qDebug("meanMin: %f, stdDevMin: %f, meanMax: %f, sdtDevMax: %f",
    //       hash.value("meanMinChannelTime"), hash.value("stdDevMinChannelTime"),
    //       hash.value("meanMaxChannelTime"), hash.value("stdDevMaxChannelTime"));

    return hash;
}


int Mutation::getPatternSequence(QList<int> channelList)
{


    QHash<int, int> channelMap;
    for (int i=1;i<=11;i++)
    {
        channelMap.insert(i,0);
    }



    int readChannel = 0;
    for (int i=0; i<channelList.count();i++)
    {
        readChannel = channelList.at(i);
        if (readChannel == 1)
        {
            channelMap.insert(1,channelMap.value(1)+1);
        }
        else if (readChannel == 2) {
          channelMap.insert(2,channelMap.value(2)+1);
        }
        else if (readChannel == 3) {
          channelMap.insert(3,channelMap.value(3)+1);
        }
        else if (readChannel == 4) {
          channelMap.insert(4,channelMap.value(4)+1);
        }
        else if (readChannel == 5) {
          channelMap.insert(5,channelMap.value(5)+1);
        }
        else if (readChannel == 6) {
          channelMap.insert(6,channelMap.value(6)+1);
        }
        else if (readChannel == 7) {
          channelMap.insert(7,channelMap.value(7)+1);
        }
        else if (readChannel == 8) {
          channelMap.insert(8,channelMap.value(8)+1);
        }
        else if (readChannel == 9) {
          channelMap.insert(9,channelMap.value(9)+1);
        }
        else if (readChannel == 10) {
          channelMap.insert(10,channelMap.value(10)+1);
        }
        else if (readChannel == 11) {
          channelMap.insert(11,channelMap.value(11)+1);
        }
    }

    QList<int> repeated;
    int indexMax = 0;

    int channelToReturn = 0;

    for (int i=1; i<=11;i++)
    {
        if (i==1)
        {
           indexMax = i;
           repeated.append(i);
           continue;
        }
        if (channelMap.value(i) >= channelMap.value(indexMax))
        {
            indexMax = i;
        }
    }
    channelToReturn = indexMax;

    if (channelMap.value(channelToReturn) >= channelList.count()/2)
    {
        return channelToReturn;
    }
    else
    {
        return 0;
    }


}



