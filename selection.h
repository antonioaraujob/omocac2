#ifndef SELECTION_H
#define SELECTION_H

#include <QMap>
#include <QMultiMap>
#include <QMapIterator>

#include "individual.h"
#include "normativephenotypicpart.h"
#include "normativegrid.h"



/**
 * @brief Clase Selection que abstrae el operador de seleccion.
 *
 * Se aplican los torneos segun el conjunto de reglas definidas en el articulo
 * "Un Algoritmo Cultural para Optimizacion Evolutiva Multiobjetivo" de Ricardo Landa y
 * Carlos Coello.
 *
 */
class Selection
{

private:

    /**
     * @brief Lista de la nueva poblacion despues de realizada la seleccion
     */
    QList<Individual *> selectedPopulation;

    /**
     * @brief Contenedor de ganadores de torneos.
     *
     * La clave corresponde al indice del individuo en la poblacion 2p
     * El valor corresponde al numero de victorias del individuo
     */
    QMap<int, int> tournamentsWinners;


    /**
     * @brief Lista de individuos que caen fuera de la grid y se deben mantener porque generaran
     * una nueva porcion del frente de Pareto
     */
    QList<Individual *> outOfGridList;


public:

    /**
     * @brief Constructor de la clase Selection.
     */
    Selection();

    /**
     * @brief Destructor de la clase Selection.
     */
    ~Selection();

    /**
     * @brief Retorna la nueva poblacion seleccionada despues de los torneos
     * @return Poblacion seleccionada despues de los torneos
     */
    QList<Individual *> getSelectedPopulation();

    /**
     * @brief Ejecuta el proceso de seleccion por torneos de la poblacion de tamano 2p
     * @param population2p Lista de individuos de tamano 2p producto del proceso de mutacion
     * sobre la poblacion original
     * @param matches Numero de torneos por cada individuo de la poblacion
     * @param nGrid Rejilla del espacio de creencias
     */
    void doSelection(QList<Individual *> population2p, int matches, NormativeGrid * nGrid);

    /**
     * @brief Ejecuta los torneos entre el individuo y los adversarios de la lista de adversarios
     * segun las reglas del algoritmo
     *
     * @param individualIndex indice del individuo en la poblacion 2p
     * @param individual Individuo que se enfrentara en torneos a los adversarios
     * @param adversaryList Lista de adversarios para los torneos
     * @param nGrid Rejilla del espacio de creencias
     */
    void makeTournaments(int individualIndex, Individual * individual, QList<Individual *> adversaryList,
                         NormativeGrid *nGrid);

    /**
     * Retorna si el individuo xj domina al individuo xi
     * @param xj Individuo 1
     * @param xi Individuo 2
     * @return Verdadero si individuo xj domina a xi
     */
    bool individualDominate(Individual * xj, Individual * xi);

    /**
     * @brief Retorna un numero aletorio entre 1 y el valor pasado como argumento
     * @param high limite superior para seleccionar el numero aleatorio
     * @return Numero aleatorio entre 1 y high
     */
    int getRandom(int high);

    /**
     * @brief Retorna verdadero si los individuos pasados como argumento son no comparables.
     *
     * Los individuos son no comparables si el individuo a no domina al b ni b domina al a.
     *
     * @param a
     * @param b
     * @return Verdaderon si los individuos pasados como argumento son no comparables.
     */
    bool nonComparableIndividuals(Individual * a, Individual * b);

    /**
     * @brief Retorna la lista de individuos que estan fuera de la grid y se deben mantener
     * @return Lista de individuos que estan fuera de la grid y se deben mantener
     */
    QList<Individual *> getOutOfGridList();

    /**
     * @brief Agrega el individuo pasado como argumento a la lista de individuos que caen fuera de la
     * rejilla y se deben conservar para generar una nueva porcion del frente de Pareto
     *
     * @param outOfGridIndividual Individuo que se debe agregar a la lista
     */
    void addIndividualToOutOfGridIndividualList(Individual * outOfGridIndividual);

};

#endif // SELECTION_H
