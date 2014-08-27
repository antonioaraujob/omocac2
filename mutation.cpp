#include "mutation.h"

//#include "scan.h"

#include <random>
#include <chrono>

Mutation::Mutation()
{
    newPopulation.clear();


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
                                  int deployedAp, double dMutationProbability)
{
    qDebug("Mutation::doDirectedMutation con probabilidad %f", dMutationProbability);




}


QList<Individual *> Mutation::getNewPopulation()
{
    return newPopulation;
}

int Mutation::getRandom(int low, int high)
{
    return qrand() % ((high + 1) - low) + low;
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
