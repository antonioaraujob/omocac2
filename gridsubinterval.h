#ifndef GRIDSUBINTERVAL_H
#define GRIDSUBINTERVAL_H

/**
 * @brief Clase GridSubInterval que modela un subintervalo de la rejilla del
 * espacio de creencias
 */
class GridSubInterval
{

private:
    /**
     * @brief Limite inferior del subintervalo
     */
    double lowerBound;

    /**
     * @brief Limite superior del subintervalo
     */
    double upperBound;

public:

    /**
     * @brief Constructor de la clase GridSubInterval
     */
    GridSubInterval(int l, int u);

    /**
     * @brief Destructor de la clase GridSubInterval
     */
    ~GridSubInterval();

    /**
     * @brief Retorna si el valor pasado como argumento pertenece al subintervalo
     *
     * La pertenencia a un intervalo es de la siguiente manera: [lowerBound, upperBound)
     * el limite inferior del intervalo esta incluido pero no el limite superior.
     *
     * @param value valor que se desea consultar si pertenece
     * @return verdadero si el argumento pertenece al intervalo
     */
    bool belongsToSubinterval(double value, bool last=false);

    /**
     * @brief Retorna el limite inferior del intervalo
     * @return Limite inferior del intervalo
     */
    double getLowerBound();

    /**
     * @brief Retorna el limite superior del intervalo
     * @return Limite superior del intervalo
     */
    double getUpperBound();

    /**
     * @brief Imprime los valores del subintervalo
     */
    void printSubInterval();

};

#endif // GRIDSUBINTERVAL_H
