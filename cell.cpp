#include "cell.h"

Cell::Cell()
{

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

void Cell::addIndividual(Individual *individual)
{
    cellIndividualList.append(individual);
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
