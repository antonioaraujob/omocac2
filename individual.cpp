#include "individual.h"

//#include "simulation.h"
#include "mainwindow.h"

#include <QTime>

#include "scan.h"


Individual::Individual(int numberOfApsDeployed)
{
    //qsrand((uint)QTime::currentTime().msec());

    // asignar el valor unico del identificador del individuo
    individualId = Simulation::getNewindividualId();

    // se deben crear los 33 parametros
    // C1,Min1,Max1,AP1,C2,Min2,Max2,AP2,...,C11,Min11,Max11,AP11


    // base de datos sqlite
    //QString database("/home/antonio/Copy/2014/pgcomp/ia/gridCells/gridCells/test_18.1.db");
    //QString database("/home/antonio/desarrollo/iaa/git/omocac/test_18.1.db");
    QString database("test_18.1.db");

    // tipo de experimento para extraer las muestras: full -> full scanning
    QString experiment("full");

    Scan scan(database.toStdString(),experiment.toStdString());
    scan.init();

    //Scan::ScanResults results = scan.execute(11, 10, 30);
    Scan::ScanResults results;

    std::cout << results.size() << " results: " << std::endl;

    int randomChannel = 0;
    double minChannelTime = 0;
    double maxChannelTime = 0;

    for (int i=0; i<11; i++)
    {
        randomChannel = getRandomChannel();
        parametersList.append(randomChannel);

        minChannelTime = getRandomMinChannelTime();
        maxChannelTime = getRandomMaxChannelTime();
        parametersList.append(minChannelTime);
        parametersList.append(maxChannelTime);

        //parametersList.append(getAPNumberOnChannel(numberOfApsDeployed, randomChannel));

        //qDebug("**channel: %d, min: %f, max: %f",randomChannel, minChannelTime, maxChannelTime);
        results = scan.execute(randomChannel, minChannelTime, maxChannelTime);
        //qDebug("**numero de APs encontrados en el canal %d: %d",randomChannel, results.size());
        //std::cout << " numero de APs encontrados en el canal: " << randomChannel << ": " << results.size() << std::endl;
        //qDebug("**scan.execute(%d, %f, %f)=%d",randomChannel, minChannelTime, maxChannelTime, results.size());
        parametersList.append(results.size());

        wonMatchesCounter = 0;
    }

    // calcular el valor de desempeno para el individuo
    calculatePerformanceValue();

    // calcular el valor de desempeno para la descubierta
    //setPerformanceDiscovery(getRandomMaxChannelTime());
    calculateDiscoveryValue();

    // calcular el valor de desempeno para la latencia
    //setPerformanceLatency(getRandomMaxChannelTime());
    calculateLatencyValue();
}


Individual::Individual(Individual &p)
{
    for (int i=0; i<44; i++)
    {
        parametersList.append(p.getParameter(i));
    }

    // calcular el valor de desempeno para el individuo
    calculatePerformanceValue();

    // calcular el valor de desempeno para la descubierta
    setPerformanceDiscovery(getRandomMaxChannelTime());

    // calcular el valor de desempeno para la latencia
    setPerformanceLatency(getRandomMaxChannelTime());
}

int Individual::getIndividualId()
{
    return individualId;
}


int Individual::getRandomChannel()
{
/*
    // el rango es 1 <= channel <= 11
    int low = 1;
    int high = 11;
    return qrand() % ((high + 1) - low) + low;
*/

    int low = 1;
    int high = 11;
    int value = 0;

    while(true)
    {
        value = qrand() % ((high + 1) - low) + low;
        if (!channelSequenceSet.contains(value))
        {
            channelSequenceSet.insert(value);
            //qDebug("canal seleccionado: %d", value);
            return value;
        }
    }
}


double Individual::getRandomMinChannelTime()
{
    // el rango es 0 <= MinChannelTime <= 10 en ms
    int low = 0;
    int high = 10;
    return qrand() % ((high + 1) - low) + low;
}

double Individual::getRandomMaxChannelTime(){

    // el rango es 10 <= MaxChannelTime <= 100
    int low = 10;
    int high = 100;
    return qrand() % ((high + 1) - low) + low;
}

double Individual::getAPNumberOnChannel(int numberOfApsDeployed, int channel){

    // la proporcion de APs que operan en cada canal es tomada del articulo
    //
    if (channel == 1)
    {
        return (numberOfApsDeployed * 18)/100;
    }
    if (channel == 2)
    {
        return (numberOfApsDeployed * 1)/100;
    }
    if (channel == 3)
    {
        return (numberOfApsDeployed * 3)/100;
    }
    if (channel == 4)
    {
        return (numberOfApsDeployed * 1)/100;
    }
    if (channel == 5)
    {
        return (numberOfApsDeployed * 1)/100;
    }
    if (channel == 6)
    {
        return (numberOfApsDeployed * 36)/100;
    }
    if (channel == 7)
    {
        return (numberOfApsDeployed * 2)/100;
    }
    if (channel == 8)
    {
        return (numberOfApsDeployed * 1)/100;
    }
    if (channel == 9)
    {
        return (numberOfApsDeployed * 6)/100;
    }
    if (channel == 10)
    {
        return (numberOfApsDeployed * 6)/100;
    }
    if (channel == 11)
    {
        return (numberOfApsDeployed * 25)/100;
    }
}

void Individual::printIndividual()
{
    //qDebug("El Individual creado es el siguiente:");
    QString individualString("   ");
    for (int j=0;j<44;j++)
    {
        individualString.append(QString::number(parametersList.at(j)));
        if (j!=43)
            individualString.append("-");
    }
    individualString.append("|");
    individualString.append(QString::number(getPerformanceDiscovery()));
    individualString.append("|");
    individualString.append(QString::number(getPerformanceLatency()));
    qDebug(qPrintable(individualString));
    //qDebug("Fo:%f",getPerformanceValue());
}

QString Individual::getIndividualAsQString()
{
    //qDebug("El Individual creado es el siguiente:");
    QString individualString("");
    for (int j=0;j<44;j++)
    {
        individualString.append(QString::number(parametersList.at(j)));
        if (j!=43)
            individualString.append("-");
    }
    individualString.append("|");
    individualString.append(QString::number(getPerformanceDiscovery()));
    individualString.append("|");
    individualString.append(QString::number(getPerformanceLatency()));
    return individualString;
}

void Individual::setPerformanceValue(double performance)
{
    performanceValue = performance;
}


void Individual::calculatePerformanceValue()
{
    performanceValue = parametersList.at(2)+parametersList.at(5)+parametersList.at(8)+parametersList.at(11)+parametersList.at(14)+
            parametersList.at(17)+parametersList.at(20)+parametersList.at(23)+parametersList.at(26)+parametersList.at(29)+parametersList.at(32);

    // se deben calcular los valores de las funciones objetivo para el individuo


    // D = Sumatoria i=1,11 Pi*di
    performanceDiscovery = 0;


    performanceLatency = 0;


}

void Individual::calculateDiscoveryValue()
{

    double api = 0;
    double discovery = 0;

    for (int i=0; i<11; i++)
    {
        api = parametersList.at((i*4)+3);
        discovery = discovery + api;
    }
    performanceDiscovery = discovery;
}

void Individual::calculateLatencyValue()
{
    int channel = 0;
    double minChannelTime = 0;
    double maxChannelTime = 0;
    double latency = 0;

    for (int i=0; i<11; i++)
    {
        channel = parametersList.at((i*4));
        minChannelTime = parametersList.at((i*4)+1);
        maxChannelTime = parametersList.at((i*4)+2);
        latency = latency + minChannelTime + (probabilityDelayLessThanMinCT(minChannelTime)*probabilityExistAtLeastOneAp(channel)*maxChannelTime);
    }
    performanceLatency = latency;
}


double Individual::getPerformanceValue()
{
    return performanceValue;
}


void Individual::setPerformanceDiscovery(double performance)
{
    performanceDiscovery = performance;
}


double Individual::getPerformanceDiscovery() const
{
    return performanceDiscovery;
}

void Individual::setPerformanceLatency(double performance)
{
    performanceLatency = performance;
}


double Individual::getPerformanceLatency() const
{
    return performanceLatency;
}




void Individual::setParameter(int i, double value)
{
    parametersList.replace(i,value);
}

double Individual::getParameter(int i) const
{
    return parametersList.at(i);
}

int Individual::getNumberOfParameters() const
{
    return parametersList.count();
}

void Individual::setWonMatchesCounter(int value)
{
    wonMatchesCounter = value;
}

int Individual::getWonMatchesCounter()
{
    return wonMatchesCounter;
}

void Individual::incrementWonMatchesCounter()
{
    wonMatchesCounter++;
}

/*
Individual& Individual::operator = (const Individual &ind)
{
    for (int i=0;i<ind.getNumberOfParameters();i++)
    {
        this->setParameter(i,ind.getParameter(i));
    }
    this->setPerformanceDiscovery(ind.getPerformanceDiscovery());
    this->setPerformanceLatency(ind.getPerformanceLatency());

    return *this;
}
*/

double Individual::probabilityExistAtLeastOneAp(int channel)
{
    //qDebug("Individual::probabilityExistAtLeastOneAp");
    double probability = 0;

    if (channel == 1)
        probability = 0.82;
    else if (channel == 2)
        probability = 0.09;
    else if (channel == 3)
        probability = 0.35;
    else if (channel == 4)
        probability = 0.1;
    else if (channel == 5)
        probability = 0.11;
    else if (channel == 6)
        probability = 0.92;
    else if (channel == 7)
        probability = 0.12;
    else if (channel == 8)
        probability = 0.13;
    else if (channel == 9)
        probability = 0.45;
    else if (channel == 10)
        probability = 0.4;
    else if (channel == 11)
        probability = 0.83;
    else
        probability = 0;

    return probability;
}

double Individual::probabilityDelayLessThanMinCT(double delay)
{
    //qDebug("Individual::probabilityDelayLessThanMinCT");

    double probability = 0;

    if (delay == 0)
        probability = 0;
    else if (delay == 1)
        probability = 0.01;
    else if (delay == 2)
        probability = 0.03;
    else if (delay == 3)
        probability = 0.5;
    else if (delay == 4)
        probability = 0.68;
    else if (delay == 5)
        probability = 0.72;
    else if (delay == 6)
        probability = 0.79;
    else if (delay == 7)
        probability = 0.82;
    else if (delay == 8)
        probability = 0.84;
    else if (delay == 9)
        probability = 0.88;
    else if (delay == 10)
        probability = 0.89;
    else if (delay == 11)
        probability = 0.9;
    else if (delay == 12)
        probability = 0.91;
    else if (delay == 13)
        probability = 0.92;
    else if (delay == 14)
        probability = 0.93;
    else if (delay == 15)
        probability = 0.94;
    else if (delay == 16)
        probability = 0.95;
    else if (delay == 17)
        probability = 0.96;
    else if (delay == 18)
        probability = 0.97;
    else if (delay == 19)
        probability = 0.98;
    else if (delay == 20)
        probability = 0.99;
    else if (delay == 21)
        probability = 0.99;
    else if (delay == 22)
        probability = 0.99;
    else if (delay == 23)
        probability = 0.99;
    else if (delay == 24)
        probability = 0.99;
    else if (delay == 25)
        probability = 1;
    else if (delay > 25)
        probability =  1;
    else
        probability = 0;

    return probability;
}






