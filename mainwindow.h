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


    void checkPopulationSize(const QString & str);

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
