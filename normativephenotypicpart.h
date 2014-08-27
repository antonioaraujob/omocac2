#ifndef NORMATIVEPHENOTYPICPART_H
#define NORMATIVEPHENOTYPICPART_H

//#include "externalfile.h"


/**
 * @brief Clase que modela la Parte Normativa Fenotipica del espacio de creencias
 *
 * Mantiene los limites inferior y superior de los intervalos de cada funcion
 * objetivo dentro de los cuales se construira una rejilla (NormativeGrid).
 *
 */
class NormativePhenotypicPart
{

private:


    /**
     * @brief Limite inferior de la funcion objetivo 1
     */
    double lowerF1;

    /**
     * @brief Limite superior de la funcion objetivo 2
     */
    double upperF1;

    /**
     * @brief Limite inferior de la funcion objetivo 2
     */
    double lowerF2;

    /**
     * @brief Limite superior de la funcion objetivo2
     */
    double upperF2;


public:

    /**
     * @brief Constructor de la clase
     */
    NormativePhenotypicPart();

    /**
     * @brief  Constructor de la clase
     * @param lF1 limite inferior de la funcion objetivo 1
     * @param uF1 limite superior de la funcion objetivo 1
     * @param lF2 limite inferior de la funcion objetivo 2
     * @param uF2 limite superior de la funcion objetivo 2
     */
    NormativePhenotypicPart(int lF1, int uF1, int lF2, int uF2);

    /**
     * @brief Destructor de la clase
     */
    ~NormativePhenotypicPart();

    /**
     * @brief Establece el valor lowerF1
     *
     * @param lF1 limite inferior a establecer
     */
    void setLowerF1(double lF1);

    /**
     * @brief Devuelve el valor lowerF1
     *
     * @return el limite inferior de la funcion objetivo 1
     */
    double getLowerF1();

    /**
     * @brief Establece el valor upperF1
     *
     * @param uF1 limite superior a establecer
     */
    void setUpperF1(double uF1);

    /**
     * @brief Devuelve el valor upperF1
     *
     * @return el limite superior de la funcion objetivo 1
     */
    double getUpperF1();

    /**
     * @brief Establece el valor lowerF2
     *
     * @param lF1 limite inferior a establecer
     */
    void setLowerF2(double lF2);

    /**
     * @brief Devuelve el valor lowerF2
     *
     * @return el limite inferior de la funcion objetivo 2
     */
    double getLowerF2();

    /**
     * @brief Establece el valor upperF2
     *
     * @param uF2 limite superior a establecer
     */
    void setUpperF2(double uF2);

    /**
     * @brief Devuelve el valor upperF2
     *
     * @return el limite superior de la funcion objetivo 2
     */
    double getUpperF2();

    /**
     * @brief Actualiza la parte normativa fenotipica
     * @param lF1 limite inferior de la funcion objetivo 1
     * @param uF1 limite superior de la funcion objetivo 1
     * @param lF2 limite inferior de la funcion objetivo 2
     * @param uF2 limite superior de la funcion objetivo 2
     */
    void updateNormativePhenotypicPart(double lF1, double uF1, double lF2, double uF2);

    /**
     * @brief Actualiza la parte normativa fenotipica de acuerdo al contenido del archivo externo
     * pasado como argumento.
     *
     * @param extFile ExternalFile para obtener los limites de las funciones objetivo
     */
    //void updateNormativePhenotypicPart(ExternalFile extFile);



};

#endif // NORMATIVEPHENOTYPICPART_H
