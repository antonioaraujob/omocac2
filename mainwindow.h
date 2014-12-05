#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "simulation.h"
#include "qcustomplot.h"


namespace Ui {
class MainWindow;
}

/**
 * @brief Clase que modela la ventana principal del programa
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT
    
private:

    /**
     * @brief Objeto Simulation que abstrae todo el algoritmo cultural
     */
    Simulation * simulation;


    /**
     * @brief Lista de soluciones no dominadas resultantes de la ejecucion del algoritmo cultural original
     */
    QList<Individual *> genericAlgorithmSolutions;

    /**
     * @brief Lista de soluciones no dominadas resultantes de la ejecucion del algoritmo cultural modificado
     *
     * En la modificacion se esta utilizando
     */
    QList<Individual *> modificatedAlgorithmSolutions;


    QList< QList<Individual *> > repeatedSolutionList;


    QList< QList<Individual *> > repeatedOriginalSolutionList;

    QList< QList<Individual *> > repeatedModificatedSolutionList;


public:

    /**
     * @brief Constructor de la clase
     */
    explicit MainWindow(QWidget *parent = 0);

    /**
     * @brief Destructor de la clase
     */
    ~MainWindow();


    /**
     * @brief Completa el widget ListView con los individuos no dominados del
     * archivo externo al final de la ejecución del algoritmo cultural
     */
    void populateListView();

    /**
     * @brief Configura el widget para generar el grafico de los individuos no dominados del
     * archivo externo.
     *  Se esta utilizando QCustomPlot
     *
     * @param customPlot
     */
    void setupCustomPlot(QCustomPlot *customPlot);

    /**
     * @brief Valida los parametros antes de ejecutar el algoritmo
     *
     *  @return Verdadero si todos los campos son validos
     */
    bool validateFields();

    void newSetupCustomPlot(QCustomPlot *customPlot);

    void setupCustomPlot2(QCustomPlot *customPlot);

    void plotSolutions();


    void storeExecutionSolution();

    /**
     * @brief Retorna el numero de individuos no dominados de todas las repeticiones
     * del algoritmo
     * @return
     */
    int getCountOfNonDominatedInRepetitions();

    /**
     * @brief Retorna el numero de individuos no dominados de todas las repeticiones
     * del algoritmo original
     * @return
     */

    int getCountOfNonDominatedInOriginalRepetitions();

    /**
     * @brief Retorna el numero de individuos no dominados de todas las repeticiones
     * del algoritmo modificado
     * @return
     */
    int getCountOfNonDominatedInModificatedRepetitions();



    /**
     * @brief Retorna el promedio de la funcion objetivo de todos los individuos no dominados
     * luego de las repeticiones del algoritmo
     * @param fo identificador de la funcion objetivo: 1 para descubierta, 2 para latencia
     * @return promedio de la funcion objetivo
     */
    double getMeanOfObjectiveFunction(int fo=1);

    /**
     * @brief retorna la desviacion estandar de la funcion objetivo pasada como argumento
     * @param mean media de la funcion objetivo seleccionada
     * @param fo funcion objetivo seleccionada: 1 para descubierta, 2 para latencia
     * @return desviacion estandar de la funcion objetivo pasada como argumento
     */
    double getStandardDeviation(double mean, int fo = 1);


    /**
     * @brief Retorna el tiempo promedio de ejecucion de 30 repeticiones
     * @param l lista de los tiempos de ejecucion de las 30 repeticiones
     * @return tiempo promedio de ejecucion de 30 repeticiones
     */
    double getMeanExecutionTime(QList<double> l);

    /**
     * @brief Retorna la desviacion estandar del tiempo de ejecucion
     * @param l lista de los tiempos de ejecucion de las 30 repeticiones
     * @param mean tiempo promedio
     * @return
     */
    double getStdDeviationExecutionTime(QList<double> l, double mean);



public slots:

    /**
     * @brief Slot para ejecutar el algoritmo cultural al presionar un boton en
     * la interfaz grafica
     */
    void executeAlgorithm();


    /**
     * @brief Slot para habilitar o deshabilitar el campo para introducir el valor de la
     *  probabilidad de mutacion dirigida
     * @param state Estado del check box
     */
    void activateDirectedMutation(int state);

    /**
     * @brief Slot para cambiar el valor del numero de encuentros por individuo en el torneo
     * @param str
     */
    void checkPopulationSize(const QString & str);

    /**
     * @brief Slot para habilitar el boton de comparacion de los algoritmos genericos y modificado
     * @param state
     */
    void activateComparationButton(int state);

    /**
     * @brief Slot para ejecutar el algoritmo PSO modificado al presionar un boton en la
     * interfaz grafica
     */
    void compareAlgorithms();



    void repeatAlgorithm();

    void view();

    void viewAll();

    /**
     * @brief slot para comparacion la ejecucion repetida 30 veces de cada uno de los algoritmos
     *
     * Se ejecuta primero el algorimo original y almacenan todos los individuos no dominados,
     * luego se ejecuta el algoritmo con la mutación dirigida y se
     *
     */
    void compareAlgorithmRepeated();


private:
    Ui::MainWindow *ui;
};


/** \mainpage Optimización Multiobjetivo con un algoritmo Cultural
  *
  * \section intro_sec Introducción
  *
  * En este proyecto se utiliza un algoritmo cultural para abordar un problema de optimización
  * multiobjetivo.
  *
  *
  *
  *
  */


#endif // MAINWINDOW_H
