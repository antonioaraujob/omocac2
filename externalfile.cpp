#include "externalfile.h"

ExternalFile::ExternalFile(int externalFileSize)
{
    maxExternalFileSize = externalFileSize;
}

ExternalFile::ExternalFile()
{
    maxExternalFileSize = 0;
}

ExternalFile::~ExternalFile()
{

}

void ExternalFile::setSize(int size)
{
    maxExternalFileSize = size;
}


int ExternalFile::getSize()
{
    return maxExternalFileSize;
}



double ExternalFile::getLowerF1()
{
    return 1.0;
}


double ExternalFile::getUpperF1()
{
    return 1.0;
}


double ExternalFile::getLowerF2()
{
    return 1.0;
}



double ExternalFile::getUpperF2()
{
    return 1.0;
}


QList<Individual *> ExternalFile::getExternalFileList()
{
    return externalFileNonDominatedList;
}


void ExternalFile::addNonDominatedIndividuals(Individual * ind)
{
    // verificar las condiciones para agregar

    externalFileNonDominatedList.append(ind);
}

/*
void ExternalFile::addNonDominatedIndividuals(QList<Individual *> nonDominatedListToInsert, NormativeGrid *nGrid)
{
    qDebug("ExternalFile::addNonDominatedIndividuals(QList<Individual *> nonDominatedListToInsert, NormativeGrid *nGrid)");

    Individual * newIndividual;

    int indexToReplaceIndividual = 0;

    //Individual * recentIndividual;

    // verificar las condiciones para agregar

    for (int i = 0; i < nonDominatedListToInsert.count(); i++)
    {
        newIndividual = nonDominatedListToInsert.at(i);
        newIndividual->printIndividual();

        // verificar si newIndividual ya existe en el archivo externo
        if (isIndividualInExternalFile(newIndividual))
        {
            continue;
        }


        // 1) Si el individuo que se pretende agregar es dominado por algun individuo
        // del archivo externo, entonces el individuo no se debe agregar
        if ( !isNewIndividualDominatedByExternalFile(newIndividual) )
        {

            if (externalFileNonDominatedList.count() < maxExternalFileSize)
            {

            qDebug(" individuo no esta dominado por el archivo externo; se inserta");
            externalFileNonDominatedList.append(newIndividual);

            // agregar el individuo nuevo en la lista de individuos de la generacion
            currentGenerationIndividualList.append(newIndividual);
            }

        }
        // 2) Si newIndividual domina a algun individuo en el archivo externo, entonces
        // se introduce en su lugar, pero continua comparandose contra todos los demas.
        // Si el mismo individuo, ya agregado, dominara a algun otro, este (el dominado)
        // es eliminado del archivo externo
        else if (newIndividualDominatesAnyoneInExternalFile(newIndividual, indexToReplaceIndividual))
        {
            addNewIndividualAndCheck(newIndividual, indexToReplaceIndividual);
        }
        // 3) si newIndividual no es dominado ni domina a nadie en el archivo externo y
        // el tamano del archivo es menor que q entonces agregarlo
        else if (newIndividualNotDominatedNotDominates(newIndividual))
        {
            if (externalFileNonDominatedList.count() < maxExternalFileSize)
            {
                externalFileNonDominatedList.append(newIndividual);

                // agregar el individuo nuevo en la lista de individuos de la generacion
                currentGenerationIndividualList.append(newIndividual);
            }
            else
            {
                // 4) el tamano del archivo es mayor que q entonces se busca algun individuo
                // del archivo externo cuya celda contenga mas individuos que la celda a la
                // que pertenece el individuo que se pretende agregar, y se reemplaza el
                // individuo anterior con el nuevo.
                checkGridCellAndInsertIndividual(newIndividual, nGrid);

            }
        }
    }
    Individual * ind;
    qDebug("INDIVIDUOS Del archivo externo-------");
    for (int i = 0; i < externalFileNonDominatedList.count(); i++)
    {
        ind = externalFileNonDominatedList.at(i);
        ind->printIndividual();
    }
    qDebug("-------");
}
*/

void ExternalFile::addNonDominatedIndividuals(QList<Individual *> nonDominatedListToInsert, NormativeGrid *nGrid)
{
    qDebug("ExternalFile::addNonDominatedIndividuals(QList<Individual *> nonDominatedListToInsert, NormativeGrid *nGrid)");

    Individual * newIndividual;

    int indexToReplaceIndividual = 0;

    //Individual * recentIndividual;

    // verificar las condiciones para agregar

    for (int i = 0; i < nonDominatedListToInsert.count(); i++)
    {
        newIndividual = nonDominatedListToInsert.at(i);
        newIndividual->printIndividual();

        // verificar si newIndividual ya existe en el archivo externo
        if (isIndividualInExternalFile(newIndividual))
        {
            continue;
        }


        // 1) Si el individuo que se pretende agregar es dominado por algun individuo
        // del archivo externo, entonces el individuo no se debe agregar
        if ( !isNewIndividualDominatedByExternalFile(newIndividual) )
        {

            // 2) Si newIndividual domina a algun individuo en el archivo externo, entonces
            // se introduce en su lugar, pero continua comparandose contra todos los demas.
            // Si el mismo individuo, ya agregado, dominara a algun otro, este (el dominado)
            // es eliminado del archivo externo
            if (newIndividualDominatesAnyoneInExternalFile(newIndividual, indexToReplaceIndividual))
            {
                addNewIndividualAndCheck(newIndividual, indexToReplaceIndividual);
            }
            // 3) si newIndividual no es dominado ni domina a nadie en el archivo externo y
            // el tamano del archivo es menor que q entonces agregarlo
            else if (externalFileNonDominatedList.count() < maxExternalFileSize)
            {
                externalFileNonDominatedList.append(newIndividual);

                // agregar el individuo nuevo en la lista de individuos de la generacion
                currentGenerationIndividualList.append(newIndividual);
            }
            else
            {
                // 4) el tamano del archivo es mayor que q entonces se busca algun individuo
                // del archivo externo cuya celda contenga mas individuos que la celda a la
                // que pertenece el individuo que se pretende agregar, y se reemplaza el
                // individuo anterior con el nuevo.
                checkGridCellAndInsertIndividual(newIndividual, nGrid);
            }
        }
    }
    Individual * ind;
    qDebug("INDIVIDUOS Del archivo externo-------");
    for (int i = 0; i < externalFileNonDominatedList.count(); i++)
    {
        ind = externalFileNonDominatedList.at(i);
        ind->printIndividual();
    }
    qDebug("-------");
}


bool ExternalFile::isNewIndividualDominatedByExternalFile(Individual * newIndividual)
{
    qDebug("ExternalFile::isNewIndividualDominatedByExternalFile");
    bool isDominated = false;

    Individual * nonDominatedindividual;

    for (int i = 0; i < externalFileNonDominatedList.count(); i++)
    {
        qDebug("   dentro del for");
        nonDominatedindividual = externalFileNonDominatedList.at(i);
        if (individualDominate(nonDominatedindividual, newIndividual))
        {
            qDebug("  newIndivual dominado por el archivo externo");
            isDominated = true;
            break;
        }
    }
    qDebug("   salio del for");
    return isDominated;
}

bool ExternalFile::individualDominate(Individual * xj, Individual * xi)
{
    //qDebug("... ... Simulation::individualDominate");

    // a solution xj is said to dominate another solution xi, and we write xj <| xi if both
    // the following conditions are true:
    //
    // condition a: Fi(xj) <= Fi(xi) to i E 1,2
    //
    // confition b: Exists j 1, 2 such that Fj(xj) < Fj(xi)

    bool conditionA = false;
    bool conditionB = false;


    // condition a
    if ( (xj->getPerformanceDiscovery() >= xi->getPerformanceDiscovery()) &&
         (xj->getPerformanceLatency() <= xi->getPerformanceLatency()) )
    {
        conditionA = true;
    }

    // condition b
    if ( (xj->getPerformanceDiscovery() > xi->getPerformanceDiscovery()) ||
         (xj->getPerformanceLatency() < xi->getPerformanceLatency()) )
    {
        conditionB = true;
    }

    if ( (conditionA) && (conditionB) )
    {
        return true;
    }else
    {
        return false;
    }
}


bool ExternalFile::newIndividualDominatesAnyoneInExternalFile(Individual * newIndividual)
{
    bool dominatedAnIndividual = false;

    Individual * nonDominatedindividual;

    for (int i = 0; i < externalFileNonDominatedList.count(); i++)
    {
        nonDominatedindividual = externalFileNonDominatedList.at(i);
        if (individualDominate(newIndividual, nonDominatedindividual))
        {
            dominatedAnIndividual = true;
            break;
        }
    }
    return dominatedAnIndividual;
}


bool ExternalFile::newIndividualDominatesAnyoneInExternalFile(Individual * newIndividual, int &indexToReplace)
{
    bool dominatedAnIndividual = false;

    Individual * nonDominatedindividual;

    for (int i = 0; i < externalFileNonDominatedList.count(); i++)
    {
        nonDominatedindividual = externalFileNonDominatedList.at(i);
        if (individualDominate(newIndividual, nonDominatedindividual))
        {
            dominatedAnIndividual = true;
            indexToReplace = i;
            break;
        }
    }
    return dominatedAnIndividual;
}

void ExternalFile::addNewIndividualAndCheck(Individual * newIndividual)
{
    qDebug("->ExternalFile::addNewIndividualAndCheck");
    Individual * nonDominatedindividual;

    bool newIndividualAlreadyInserted = false;

    // lista para mantener los indices de los individuos dominados que se deben eliminar
    QList<int> markedToRemove;

    for (int i = 0; i < externalFileNonDominatedList.count(); i++)
    {

        nonDominatedindividual = externalFileNonDominatedList.at(i);
        if (newIndividual->getIndividualId() == nonDominatedindividual->getIndividualId())
        {
            continue;
        }


        if (individualDominate(newIndividual, nonDominatedindividual))
        {
            // verificar que no esta insertado
            if (!newIndividualAlreadyInserted)
            {
                externalFileNonDominatedList.replace(i, newIndividual);
                newIndividualAlreadyInserted = true;
                qDebug("+ reemplazo individuo del archivo externo");
            }
            else // newIndividual ya insertado, entonces marcar nonDominatedIndividual
            {
                //externalFileNonDominatedList.removeAt(i);
                markedToRemove.append(i);
                qDebug("marcado individuo duplicado con indice %d", i);
            }
        }
    }

    // eliminar los individuos marcados del archivo externo
    for (int j = 0; j < markedToRemove.count(); j++)
    {
        externalFileNonDominatedList.removeAt(j);
    }
}


void ExternalFile::addNewIndividualAndCheck(Individual * newIndividual, int indexToReplace)
{
    qDebug("-ExternalFile::addNewIndividualAndCheck");
    Individual * nonDominatedindividual;

    // lista para mantener los indices de los individuos dominados que se deben eliminar
    QList<int> markedToRemove;

    // reemplazar el primer individuo domimnado con newIndividual
    externalFileNonDominatedList.replace(indexToReplace, newIndividual);
    qDebug("------> reemplazo de individuo dominado en el archivo externo");

    // agregar el individuo nuevo en la lista de individuos de la generacion
    currentGenerationIndividualList.append(newIndividual);

    int i = indexToReplace+1;

    while (i < externalFileNonDominatedList.count())
    //for (int i = indexToReplace+1; i < externalFileNonDominatedList.count(); i++)
    {
        nonDominatedindividual = externalFileNonDominatedList.at(i);
        if (individualDominate(newIndividual, nonDominatedindividual))
        {
            markedToRemove.append(nonDominatedindividual->getIndividualId());
            qDebug("marcado individuo duplicado con indice %d", i);
        }
        i++;
    }

    // eliminar los individuos marcados del archivo externo
    for (int j=0; j<markedToRemove.count(); j++)
    {
        for (int k=0; k<externalFileNonDominatedList.count(); k++)
        {
            if (externalFileNonDominatedList.at(k)->getIndividualId() == markedToRemove.at(j))
            {
                externalFileNonDominatedList.removeAt(k);
                break;
            }
        }
    }

}


bool ExternalFile::newIndividualNotDominatedNotDominates(Individual * newIndividual)
{
    bool newIndividualNotDominated = true;
    bool newIndividualNotDominates = true;

    Individual * nonDominatedindividual;

    // verificar si newIndividual domina a alguien
    for (int i = 0; i < externalFileNonDominatedList.count(); i++)
    {
        nonDominatedindividual = externalFileNonDominatedList.at(i);
        if (individualDominate(newIndividual, nonDominatedindividual))
        {
            newIndividualNotDominates = false;
            break;
        }
    }

    // verificar si alguien domina a newIndividual
    for (int i = 0; i < externalFileNonDominatedList.count(); i++)
    {
        nonDominatedindividual = externalFileNonDominatedList.at(i);
        if (individualDominate(nonDominatedindividual, newIndividual))
        {
            newIndividualNotDominated = false;
            break;
        }
    }

    if ( (newIndividualNotDominates) && (newIndividualNotDominated) )
    {
        return true;
    }
    else
    {
        return false;
    }
}


void ExternalFile::checkGridCellAndInsertIndividual(Individual * newIndividual, NormativeGrid * nGrid)
{
    // 4) el tamano del archivo es mayor que q entonces se busca algun individuo
    // del archivo externo cuya celda contenga mas individuos que la celda a la
    // que pertenece el individuo que se pretende agregar, y se reemplaza el
    // individuo anterior con el nuevo.

    int newIndividualCellCount = nGrid->getCountOfCell(newIndividual);

    Individual * externalFileindividual;

    int externalFileindividualCellCount = 0;


    for (int i = 0; i < externalFileNonDominatedList.count(); i++)
    {
        externalFileindividual = externalFileNonDominatedList.at(i);
        externalFileindividualCellCount = nGrid->getCountOfCell(externalFileindividual);

        if (externalFileindividualCellCount > newIndividualCellCount)
        {
            // reemplazar el individuo con newIndivual
            externalFileNonDominatedList.replace(i, newIndividual);

            // agregar el individuo nuevo en la lista de individuos de la generacion
            currentGenerationIndividualList.append(newIndividual);
        }
    }
}


bool ExternalFile::isIndividualInExternalFile(Individual * individual)
{

    qDebug("->ExternalFile::isIndividualInExternalFile");
    Individual * alreadyInsertedIndividual;

    for (int i = 0; i < externalFileNonDominatedList.count(); i++)
    {
        qDebug("   dentro del for");
        alreadyInsertedIndividual = externalFileNonDominatedList.at(i);
        if (individual->getIndividualId() == alreadyInsertedIndividual->getIndividualId())
        {
            qDebug("---> el individuo EXISTE en el arcvhivo");
            return true;
        }

    }

    qDebug("   antes de salir");
    return false;


/*
    qDebug("->ExternalFile::isIndividualInExternalFile");
    Individual * alreadyInsertedIndividual;

    for (int i = 0; i < externalFileNonDominatedList.count(); i++)
    {
        alreadyInsertedIndividual = externalFileNonDominatedList.at(i);

        // verificar si valores de F1 y F2 son iguales
        if ( (ind->getPerformanceDiscovery() == alreadyInsertedIndividual->getPerformanceDiscovery()) &&
            (ind->getPerformanceLatency() == alreadyInsertedIndividual->getPerformanceLatency()) )
        {
            qDebug("    F1 y F2 son iguales");

            bool parameterFlagDifferent = false;
            for (int j=0; j<44; j++)
            {
                if (ind->getParameter(j) != alreadyInsertedIndividual->getParameter(j))
                {
                    qDebug("    parametro %d del individuo es igual", j);
                    parameterFlagDifferent = true;
                }
                else
                {
                    return false;
                }
            }
        }
        else{
            return false;
        }
    }
*/
}


QList<Individual *> ExternalFile::getCurrentGenerationIndividualList()
{
    return currentGenerationIndividualList;
}


void ExternalFile::resetCurrentGenerationIndividualList()
{
    currentGenerationIndividualList.clear();
}






