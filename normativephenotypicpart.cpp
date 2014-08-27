#include "normativephenotypicpart.h"

NormativePhenotypicPart::NormativePhenotypicPart()
{
    lowerF1 = 0;
    upperF1 = 0;
    lowerF2 = 0;
    upperF2 = 0;
}

NormativePhenotypicPart::NormativePhenotypicPart(int lF1, int uF1, int lF2, int uF2)
{
    lowerF1 = lF1;
    upperF1 = uF1;
    lowerF2 = lF2;
    upperF2 = uF2;
}

NormativePhenotypicPart::~NormativePhenotypicPart()
{

}

void NormativePhenotypicPart::setLowerF1(double lF1)
{
    lowerF1 = lF1;
}

double NormativePhenotypicPart::getLowerF1()
{
    return lowerF1;
}

void NormativePhenotypicPart::setUpperF1(double uF1)
{
    upperF1 = uF1;
}

double NormativePhenotypicPart::getUpperF1()
{
    return upperF1;
}

void NormativePhenotypicPart::setLowerF2(double lF2)
{
    lowerF2 = lF2;
}

double NormativePhenotypicPart::getLowerF2()
{
    return lowerF2;
}

void NormativePhenotypicPart::setUpperF2(double uF2)
{
    upperF2 = uF2;
}

double NormativePhenotypicPart::getUpperF2()
{
    return upperF2;
}

void NormativePhenotypicPart::updateNormativePhenotypicPart(double lF1, double uF1, double lF2, double uF2)
{
    lowerF1 = lF1;
    upperF1 = uF1;

    lowerF2 = lF2;
    upperF2 = uF2;
}

/*
void NormativePhenotypicPart::updateNormativePhenotypicPart(ExternalFile extFile)
{
    lowerF1 = extFile.getLowerF1();
    upperF1 = extFile.getUpperF1();

    lowerF2 = extFile.getLowerF2();
    upperF2 = extFile.getUpperF2();

}
*/



