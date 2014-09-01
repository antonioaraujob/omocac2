#ifndef MUTATION_H
#define MUTATION_H

#include <QList>
#include <QHash>

#include "individual.h"
#include "normativegrid.h"

#include "scan.h"

/**
 * @brief Clase que modela la variacion de la poblacion para cada generacion
 */
class Mutation
{

private:
    /**
     * @brief Lista que mantiene la poblacion original mas los hijos generados
     */
    QList<Individual *> newPopulation;

    /**
     * @brief Diccionario para mantener registro de los canales que ya se han utilizado
     * en un proceso de mutacion.
     *
     * Se utiliza para evitar que en la mutacion de parametros de un individuo se asignen
     * canales que ya se utilizaron previamente
     */
    QHash<int, bool> channelsUsedForMutation;

    /**
     * @brief Desviacion estandar para la mutacion
     */
    double stdDeviation;


public:

    /**
     * @brief Constructor de la clase Mutation
     */
    Mutation();

    /**
     * @brief Destructor de la clase Mutation
     */
    ~Mutation();

    /**
     * @brief Ejecuta el proceso de mutacion sobre la poblacion pasada como argumento.
     *
     * La nueva poblacion de tamano 2P se almacenara en @see newPopulation
     *
     * @param population poblacion sobre la cual se ejecuta el proceso de mutacion
     * @param std desviacion estandar de la variable aleatoria normal
     * @param deployedAp numero de AP desplegados en el entorno
     */
    void doMutation(QList<Individual *> population, double std, int deployedAp);

    /**
     * @brief Ejecuta el proceso de mutacion dirigida sobre la base del conocimiento normativo
     * del espacio de creencias.
     *
     * @param population poblacion sobre la cual se ejecuta el proceso de mutacion
     * @param std desviacion estandar de la variable aleatoria normal
     * @param deployedAp numero de AP desplegados en el entorno
     * @param dMutationProbability
     * @param grid Puntero a la rejilla
     */
    void doDirectedMutation(QList<Individual *> population, double std, int deployedAp, double dMutationProbability, NormativeGrid * grid);

    /**
     * @brief Retorna la nueva poblacion luego de la variacion.
     *
     * Esta poblacion es de tamano 2P
     * @return Lista de tamano 2P con la nueva poblacion luego de ejecutada la mutacion
     */
    QList<Individual *> getNewPopulation();

    /**
     * Retorna un numero aletorio entre a y b
     *
     * @param a limite inferior
     * @param b limite superior
     * @return numero aleatorio entre a y b
     */
    int getRandom(int low, int high);

    /**
     * @brief Retorna un número aleatorio uniforme entre 0 y 1
     * @return número aleatorio uniforme entre 0 y 1
     */
    double getRandomUniform();

    /**
     * @brief Ejecuta una mutacion gausiana de un parametro de un individuo
     * @param index indice del parametro a mutar
     * @param mean media de la distribucion normal
     * @param std desviacion estandar de la distribucion normal
     * @param currentParameterValue valor actual del parametro a mutar
     * @param offspring Individuo hijo sobre el cual se mutan parametros
     *
     * @return valor del parametro mutado
     */
    int mutateIndividualParameter(int index, int mean, double std, double currentParameterValue, Individual *offspring);

    /**
     * @brief Retorna verdadero si el parametro a revisar de un individuo corresponde a un canal
     * @param index indice del parametro a revisar del individuo
     * @return Verdadero si el indice corresponde a un parametro de numero de canal
     */
    bool isThisParameterAChannel(int index);

    /**
     * @brief Retorna verdadero si el parametro a revisar de un individuo corresponde a un MinChannelTime
     * @param index indice del parametro a revisar del individuo
     * @return Verdadero si el indice corresponde a un parametro de MinChannelTime
     */
    bool isThisParameterAMinChannelTime(int index);

    /**
     * @brief Retorna verdadero si el parametro a revisar de un individuo corresponde a un MaxChannelTime
     * @param index indice del parametro a revisar del individuo
     * @return Verdadero si el indice corresponde a un parametro de MaxChannelTime
     */
    bool isThisParameterAMaxChannelTime(int index);

    /**
     * @brief Retorna verdadero si el parametro a revisar de un individuo corresponde a numero de APs
     * @param index indice del parametro a revisar del individuo
     * @return Verdadero si el indice corresponde a un parametro de APs
     */
    bool isThisParameterAPs(int index);

    /**
     * @brief Imprime en una cadena legible al humano la nueva poblacion de tamano 2p
     */
    void printNewPopulation();

    /**
     * @brief Retorna un nuevo valor de descubrimiento de AP de los experimentos
     * @param channel canal en el que se esta escuchando
     * @param minChannelTime minChannelTime
     * @param maxChannelTime maxChannelTime
     * @return
     */
    int getNewParameterAPs(int channel, double minChannelTime, double maxChannelTime);

    /**
     * @brief Ejecuta la mutacion original del articulo utilizando como patron el individuo
     * pasado como argumento
     * @param father individuo a tomar como patron para la mutacion original
     * @param std desviacion estandar de la distribucion normal (parametro del algoritmo)
     * @param deployedAp numero de APs desplegados
     */
    void originalMutation(Individual * father, double std, int deployedAp);

    /**
     * @brief Ejecuta la mutacion dirigida
     *
     * @param grid Puntero a la rejilla para observar las Cell a utilizar en la mutacion
     * @param father Individuo padre para la mutacion
     */
    void directedMutation(NormativeGrid * grid, Individual * father);

    /**
     * @brief Asigna la desviacion estandar para la mutacion
     * @param std desviacion estandar para la mutacion
     */
    void setStdDeviation(double std);

    /**
     * @brief Retorna la desviacion estandar de la mutacion
     * @return desviacion estandar de la mutacion
     */
    double getStdDeviation();

    /**
     * @brief Calcula la media y la deviación estandar del bloque/gen de un individuo
     * pasado como argumento
     *
     * list[0]->media de minChannelTime
     * list[1]->desviación estándar de minChannelTime
     * list[2]->media de maxChannelTime
     * list[3]->desviación estándar de maxChannelTime
     *
     * @param minChannelTimeList lista de valores de minChannelTime
     * @param maxChannelTimeList lista de valores de maxChannelTime
     * @param genIndex indice del gen del individuo
     * @return Lista con media y desviación estándar de minChannelTime y media y desviación estándar de minChannelTime
     */
    QHash<QString, double> calculateMeanAndStdDev(QList<double> minChannelTimeList, QList<double> maxChannelTimeList, int genIndex);


    int getPatternSequence(QList<int> channelList);

};

#endif // MUTATION_H
