#ifndef CELL_H
#define CELL_H

#include <QList>

#include "individual.h"

/**
 * @brief Clase Cell que abstrae una celda de la rejilla del espacio de creencias.
 *
 * Una Cell almacenará una lista de individuos de acuerdo a los valores de las funciones
 * objetivo en unos subintervalos específicos.
 */
class Cell
{

private:

    /**
    * @brief Lista de individuos cuyos valores de funciones objetivo caen en los
    * subintervalos correspondiente a la celda.
    */
    QList<Individual *> cellIndividualList;

    /**
     * @brief Coordenada x de la celda (subintervalo de descubierta)
     */
    int f1;

    /**
     * @brief Coordenada y de la celda (subintervalo de latencia)
     */
    int f2;

public:

    /**
     * @brief Constructor de la clase Cell
     */
    Cell();

    /**
     * @brief Desstructor de la clase Cell
     */
    ~Cell();

    /**
     * @brief Retorna el número de individuos de la celda
     *
     * @return Número de individuos de la celda
     */
    int getCount();

    /**
     * @brief Retorna el individuo de la lista con indice el valor pasado como argumento.
     * @param index indice del individuo en la lista de individuos de Cell
     * @return Individuo con indice pasado como argumento
     */
    Individual * getIndividual(int index);


    /**
     * @brief Agrega un individuo a la lista de individuos de la celda
     * @param individual individuo a agregar a la lista.
     * @param indexF1 indice del subintervalo para F1
     * @param indexF1 indice del subintervalo para F2
     */
    void addIndividual(Individual * individual,  int indexF1, int indexF2);

    /**
     * @brief Retorna la lista de individuos de la Cell
     *
     * @return Lista de individuos de la Cell
     */
    QList<Individual *> getIndividualList();

    /**
     * @brief Sobrecarga del operador de asignacion para una celda
     * @param cell a asignar
     * @return Cell asignada
     */
    Cell& operator = (const Cell &cell);

    /**
    * @brief Retorna el valor del indice del subintervalo de F1 dentro de la rejilla
    * @return valor del indice del subintervalo de F1 dentro de la rejilla
    */
    int getSubintervalF1();

    /**
    * @brief Retorna el valor del indice del subintervalo de F2 dentro de la rejilla
    * @return valor del indice del subintervalo de F1 dentro de la rejilla
    */
    int getSubintervalF2();

};

#endif // CELL_H
