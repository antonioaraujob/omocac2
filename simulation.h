#ifndef SIMULATION_H
#define SIMULATION_H

#include <QList>

#include "individual.h"
#include "normativephenotypicpart.h"
#include "gridsubinterval.h"
#include "normativegrid.h"
#include "externalfile.h"
#include "mutation.h"
#include "selection.h"


/**
 * @brief Clase que modela la simulacion para el algoritmo cultural
 */
class Simulation
{

private:

    /**
     * @brief Tamano de la poblacion
     */
    int populationSize;

    /**
     * @brief Contador de identificador de individuo
     *
     * Usado para asignar un identificador unico para cada individuo que se creo en la simulacion.
     */
    static int individualIdCounter;

    /**
     * @brief Tamano maximo del archivo externo de soluciones
     */
    int externalFileSize;

    /**
     * @brief Numero maximo de generaciones
     */
    int generationsMax;

    /**
     * @brief Numero de la generacion actual
     */
    int currentGeneration;

    /**
     * @brief Numero de subintervalos para la grid
     */
    int gridSubintervalsNumber;

    /**
     * @brief Frecuencia con la que se actualiza la parte fenotipica normativa.
     * Cada gNormative generaciones se debe actualizar la parte fenotipica
     * normativa.
     */
    int gNormative;

    /**
     * @brief Numero de encuentros por individuo para torneos.
     * Entre más cercano sea este numero a 2p, mas parecido sera a
     * una jerarquizacion completa, requiriendo, en consecuencia, mas
     * tiempo de máquina. p/2 es un buen valor.
     */
    int matchesPerIndividuals;

    /**
     * @brief Desviacion estandar para el proceso de mutacion gausiana
     */
    int stdDeviation;

    /**
     * @brief Numero de APs desplegados en la simulacion
     */
    int deployedAPs;

    /**
     * @brief Lista de la poblacion de individuos
     */
    QList<Individual *> populationList;

    /**
     * @brief Lista de la poblacion de individuos mutados
     */
    QList<Individual *> mutatedPopulationList;

    /**
     * @brief Parte Normativa Fenotipica del espacio de creencias
     */
    NormativePhenotypicPart * normativePhenotipicPart;

    /**
     * @brief Rejilla del espacio de creencias
     */
    NormativeGrid * nGrid;

    /**
     * @brief Archivo externo de soluciones no dominadas
     *
     * Este archivo tendra como tamano maximo el valor de externalFileSize
     * @see externalFileSize
     */
    ExternalFile * externalFile;

    /**
     * @brief Verdadero si se utilizara la mutacion dirigida
     */
    bool directedMutation;

    /**
     * @brief Probabilidad de ocurrencia de la mutacion dirigida
     */
    double directedMutationProbability;

    /**
     * @brief Mutacion de la poblacion.
     *
     * Este objeto se encargara de crear una nueva poblacion de tamano 2p a partir
     * de la poblacion de tamano p. Se utiliza la mutacion gausiana.
     */
    Mutation * mutation;

    /**
     * @brief Seleccion de la poblacion
     *
     *  Este objeto se encarga de realizar la seleccion de P individuos a partir de la
     * poblacion 2P considerando los torneos y el espacio de creencias.
     */
    Selection * selection;


    /**
     * @brief Lista de individuos no dominados seleccionados del torneo que caen fuera de los
     * intervalos de la rejilla
     */
    QList<Individual *> outOfGridIndividualList;

public:

    /**
     * @brief Constructor de la clase Simulacion
     *
     * @param population tamano de la poblacion
     * @param extFileSize tamano del archivo externo
     * @param generations numero de generaciones
     * @param subintervalsGrid numero de subintervalos para la rejilla
     * @param genNormative numero de generacion para ejecutar la actualizacion de la parte
     * normativa fenotipica
     * @param matches numero de torneos por individuo
     * @param stdDev desviacion estandar de la distribucion normal para la mutacion gausiana
     * @param aps numero de APs desplegados en el entorno
     * @param directedMutation Verdadero si se utilizara la mutacion dirigida
     * @param directedMutationProbability Probabilidad de ocurrencia de la mutacion dirigida
     *
     *
     */
    Simulation(int population, int extFileSize, int generations, int subintervalsGrid, int genNormative,
               int matches, int stdDev, int aps, bool dMutation, double dMutationProbability);

    /**
     * @brief Destructor de la clase
     */
    ~Simulation();

    /**
     * @brief Retorna un nuevo identificador para un individuo creado
     * @return Retorna un nuevo identificador para un individuo creado
     */
    static int getNewindividualId();

    // generar poblacion e inicializarla
    /**
     * @brief Inicializa la poblacion de la simulacion
     */
    void initializePopulation();

    /**
     * @brief Retorna la lista de la poblacion de invididuos
     * @return
     */
    QList<Individual *> getPopulationList();

    // evaluar poblacion

    // inicializar el espacio de creencias
    /**
     * @brief Inicializa la parte normativa fenotipica.
     *
     * La inicialización de la parte normativa fenotípica del espacio de creencias
     * consiste en encontrar los valores extremos de cada función objetivo que se
     * encuentren en los individuos no dominados de la población inicial.
     *
     * Ejecutar un ordenamiento de los individuos no dominados con respecto a la
     * función objetivo 1 y seleccionar el de menor valor para lowerF1 y el de mayor
     * valor para upperF1. Este mismo procedimiento se realiza para la función objetivo 2.
     */
    void initializeNormativePhenotypicPart();

    /**
     * @brief Inicializa la rejilla del espacio de creencias.
     *
     * La rejilla se crea tomando como intervalos los valores almacenados en la parte
     * normativa fenotípica y se divide utilizando los parámetros de entrada s1 y s2.
     * Los contadores de los individuos no dominados dentro de cada celda se inicializan en
     * cero.
     *
     * Crear una matriz de dimensión s1xs2 (está reportado que 10 es un buen número) con un
     * contador (int) en cada celda. Para cada celda de la rejilla asignar cero al valor
     * del contador.
     */
    void initializeGrid();

    // actualizar el espacio de creencias
    /**
     * @brief Actualiza la parte fenotipica normativa
     */
    void updateNormativePhenotypicPart();

    /**
     * @brief Actualiza la rejilla del espacio de creencias con los individuos no dominados
     * agregados al archivo externo en la generacion actual
     *
     * @param nonDominated lista de individuos no dominados agregados al archivo externo en la
     * generacion actual
     */
    void updateGrid(QList<Individual *> nonDominated);

    /**
     * @brief Imprime la rejilla del espacio de creencia actual
     */
    void printGrid();

    // generacion hijos
    /**
     * @brief Realiza la mutacion sobre la poblacion de tamano p y genera una nueva
     * poblacion de tamano 2p
     */
    void mutatePopulation();

    // evaluar hijos

    // hacer torneos
    // seleccionar ganadores de torneos
    /**
     * @brief Realiza el proceso de seleccion de P individuos a partir de la poblacion 2P
     * generada con la @see mutatePopulation.
     *
     * Para la seleccion se considera la seleccion por torneos y el espacio de creencias
     */
    void selectPopulation();

    // agregar individuos no dominados al archivo externo
    /**
     * @brief Agrega los individuos no dominados en la lista pasada como argumento al archivo
     * externo de acuerdo a las reglas de insercion
     *
     * @param ndIndividualList Lista de individuos no dominados
     */
    void addNonDominatedIndividualsToExternalFile(QList<Individual *> ndIndividualList);

    /**
     * @brief Incrementa el contador de generaciones de la clase Simulation
     */
    void incrementGeneration();

    /**
     * @brief Retorna el numero de la generacion actual
     * @return Numero de la generacion actual
     */
    int getCurrentGenerationNumber();

    /**
     * @brief Retorna verdadero si alcanzo la generacion establecida por el parametro generationMax
     */
    bool stopEvolution();

    /**
     * @brief Retorna la lista de individuos no dominados de la poblacion de acuerdo al enfoque 1
     * @return Lista de individuos no dominados
     */
    QList<Individual *> getNonDominatedPopulationApproach1();


    /**
     * @brief Retorna la lista de individuos no dominados de la poblacion de acuerdo al enfoque 2
     * @return Lista de individuos no dominados
     */
    QList<Individual *> getNonDominatedPopulationApproach2();

    /**
     * @brief Retorna si el individuo xj domina al individuo xi
     * @param xj Individuo 1
     * @param xi Individuo 2
     * @return Verdadero si individuo xj domina a xi
     */
    bool individualDominate(Individual * xj, Individual * xi);

    /**
     * @brief Asigna el archivo externo a la clase Simulation
     * @param extFile puntero al archivo externo a asignar
     */
    void setExternalFile(ExternalFile * extFile);

    /**
     * @brief Devuelve el archivo externo de la clase Simulation
     * @return Puntero al archivo externo de la clase Simulation
     */
    ExternalFile * getExternalFile();

    /**
     * @brief Retorna la desviacion estandar de una variabla aleatoria distribuida normal.
     *
     * Esta desviacion es un parametro de la simulacion introducido por el usuario
     *
     * @return desviacion estandar
     */
    double getStdDeviation();

    /**
     * @brief Imprime la poblacion actual
     */
    void printPopulation();

    /**
     * @brief Retorna el valor del numero de generaciones que deben pasar para actualizar
     * la parte fenotipica normativa
     * @return Retorna el valor del numero de generaciones que deben pasar para actualizar
     * la parte fenotipica normativa
     */
    int getgNormative();

    /**
     * @brief Retorna la lista de inviduos no dominados que cayeron fuera de la rejilla
     * del espacio de creencias en la generacion actual
     */
    QList<Individual *> getOutOfGridIndividualList();

    /**
     * @brief Agrega el individuo pasado como argumento a la lista de los individuos que
     * caen fuera de la rejilla y se deben conservar dado que generara una nueva porcion
     * del frente de Pareto
     *
     * @param outOfGridIndividual individuo que cae fuera de la rejilla y se debe conservar
     */
    void addIndividualToOutOfGridIndividualList(Individual *outOfGridIndividual);

    /**
     * @brief Evalua los individuos con respecto a las funciones objetivo
     */
    void evaluateIndividuals();

    /**
     * @brief Imprime por pantalla la lista de individuos para propositos de depuracion
     * @param list lista de individuos a imprimir por pantalla
     */
    void printList(QList<Individual*> list);


};

#endif // SIMULATION_H
