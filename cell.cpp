#include "cell.h"

Cell::Cell()
{
    cellIndividualList.clear();
}

Cell::~Cell()
{

}

int Cell::getCount()
{
    return cellIndividualList.count();
}

Individual * Cell::getIndividual(int index)
{
    return cellIndividualList.at(index);
}

void Cell::addIndividual(Individual *individual, int indexF1, int indexF2)
{
    cellIndividualList.append(individual);
    f1 = indexF1;
    f2 = indexF2;
}


QList<Individual *> Cell::getIndividualList()
{
    return cellIndividualList;
}


Cell& Cell::operator = (const Cell &cell)
{
    if (&cell != this)
    {
        this->cellIndividualList = cell.cellIndividualList;
    }
    return *this;
}

int Cell::getSubintervalF1()
{
    return f1;
}

int Cell::getSubintervalF2()
{
    return f2;
}
