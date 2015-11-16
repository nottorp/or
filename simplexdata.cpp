#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "simplexdata.h"

SimplexData::SimplexData()
{
    m = n = 0;
    tab = NULL;
    rowlabels = collabels = NULL;
}

SimplexData::SimplexData(const SimplexData &other)
{
    other.copyTo(*this);
}

SimplexData::~SimplexData()
{
    cleanup();
}

void SimplexData::setup(int _m, int _n, double **_tab)
{
    cleanup();
    m = _m;
    n = _n;
    tab = new double *[m + 1];
    for (int i=0; i<m+1; ++i)
    {
        tab[i] = new double[m + n + 1];
        for (int j=0; j<m+n+1; ++j)
            tab[i][j] = _tab[i][j];
    }
    rowlabels = new int[m];
    for (int i=0; i<m; ++i)
        rowlabels[i] = i + n + 1;
    collabels = new int[m + n];
    for (int i=0; i<m+n; ++i)
        collabels[i] = i + 1;
}

void SimplexData::copyTo(SimplexData &dest) const
{
    dest.cleanup();
    dest.setup(m, n, tab);
    for (int i=0; i<m; ++i)
        dest.rowlabels[i] = rowlabels[i];
    for (int i=0; i<m+n; ++i)
        dest.collabels[i] = collabels[i];
}

void SimplexData::cleanup(void)
{
    if (tab == NULL)
        return;
    for(int i=0; i<m+1; ++i)
        delete[] tab[i];
    delete[] tab;
    delete[] rowlabels;
    delete[] collabels;
    m = n = 0;
    tab = NULL;
    rowlabels = collabels = NULL;
}

void SimplexData::printTable(void)
{
    // Header
    printf ("         |");
    for (int j=0; j<m+n; ++j)
        printf("     x% 2d ", collabels[j]);
    printf("   RHS\n");
    for (int i=0; i<m+1; ++i)
    {
        if (i<m)
            printf("     x% 2d |", rowlabels[i]);
        else
            printf("       z |");
        for (int j=0; j<m+n+1; ++j)
            printf("% 8.3f ", tab[i][j]);
        printf("\n");
    }
}

bool SimplexData::isOptimal(void)
{
    for (int j=0; j<m+n+1; ++j)
    {
        if (tab[m][j] < 0)
            return false;
    }
    return true;
}

int SimplexData::pickPivotColumn(void)
{
    // Bland's rule - pick the leftmost one that's < 0
    for (int j=0; j<m+n; ++j)
    {
        if (tab[m][j] < 0)
            return j;
    }
    return -1;
}

bool SimplexData::isUnbounded(int pivotcol)
{
    for (int i=0; i<m; ++i)
        if (tab[i][pivotcol] > 0)
            return false;
    return true;
}

int SimplexData::pickPivotRow(int pivotcol)
{
    double bestratio = INFINITY;
    int bestindex = -1;
    for (int k=0; k<m; ++k)
    {
        // Don't swap a variable with itself...
        printf("rowlabels[k]=%d, pivotcol+1=%d\n", rowlabels[k], pivotcol+1);
        //if ((rowlabels[k] != (pivotcol + 1)) && (tab[k][pivotcol] > 0))
        if ((rowlabels[k] != (pivotcol + 1)) && (tab[k][pivotcol] > 0))
        {
            double curratio = tab[k][m+n] / tab[k][pivotcol];
            printf("curratio=%4.2f, bestratio=%4.2f\n", curratio, bestratio);
            if (
                    (curratio < bestratio) ||
                    ((curratio == bestratio) && (rowlabels[k] < rowlabels[bestindex])) // Bland's rule - pick the variable with the smallest index
               )
            {
                bestratio = curratio;
                bestindex = k;
            }
        }
    }
    return bestindex;
}



SimplexData& SimplexData::doPivot(int pivotrow, int pivotcol)
{
    SimplexData *res = new SimplexData();
    // Make a copy
    copyTo(*res);
    // Do the pivot operation in the copy
    for (int i=0; i<m+1; ++i)
    {
        if (i != pivotrow)
        {
            for (int j=0; j<m+n+1; ++j)
            {
                if (j != pivotcol)
                {
                    res->tab[i][j] =
                            (tab[i][j] * tab[pivotrow][pivotcol] - tab[i][pivotcol] * tab[pivotrow][j])
                            / tab[pivotrow][pivotcol];

                }
            }
        }
    }
    for (int i=0; i<m+1; ++i)
    {
        if (i != pivotrow)
            res->tab[i][pivotcol] = -1 * tab[i][pivotcol] / tab[pivotrow][pivotcol];
    }
    for (int j=0; j<m+n+1; ++j)
    {
        if (j != pivotcol)
            res->tab[pivotrow][j] = tab[pivotrow][j] / tab[pivotrow][pivotcol];
    }
    res->tab[pivotrow][pivotcol] = 1;
    // Adjust row label in the copy as well
    res->rowlabels[pivotrow] = collabels[pivotcol];
    // ... and return the copy
    return *res;
}

SimplexData& SimplexData::doPivot2(int pivotrow, int pivotcol)
{
    SimplexData *res = new SimplexData();
    // Make a copy
    copyTo(*res);
    res->printTable();
    // Do the pivot operation in the copy
    for (int i=0; i<m+1; ++i)
    {
        if (i != pivotrow)
        {
            for (int j=0; j<m+n+1; ++j)
            {
                res->tab[i][j] = tab[i][j] - tab[pivotrow][j] * tab[i][pivotcol] / tab[pivotrow][pivotcol];
            }
        }
    }
    for (int j=0; j<m+n+1; ++j)
    {
        if (j != pivotcol)
            res->tab[pivotrow][j] = tab[pivotrow][j] / tab[pivotrow][pivotcol];
    }
    res->tab[pivotrow][pivotcol] = 1;
    // Adjust row label in the copy as well
    res->rowlabels[pivotrow] = collabels[pivotcol];
    res->printTable();
    // ... and return the copy
    return *res;
}


SimplexData::StepResult SimplexData::simpleSimplexStep(void)
{
    printf("*** BEGIN STEP ***\n");
    printTable();
    if (isOptimal())
    {
        printf("Optimal, stopping.\n");
        return Optimal;
    }
    int pivotcol = pickPivotColumn();
    if (pivotcol < 0)
    {
        printf("Internal error picking pivot column, should never get here!\n");
        return InternalError;
    }
    printf("Pivot column picked: %d, entering variable x%d\n", pivotcol + 1, collabels[pivotcol]);
    if (isUnbounded(pivotcol))
    {
        printf("Unbounded, stopping.\n");
        return Unbounded;
    }
    int pivotrow = pickPivotRow(pivotcol);
    if (pivotrow < 0)
    {
        printf("Internal error picking pivot row, should never get here!\n");
        return InternalError;
    }
    printf ("Pivot row picked: %d, exit variable x%d\n", pivotrow + 1, rowlabels[pivotrow]);
    *this = doPivot(pivotrow, pivotcol);
    printTable();
    printf("*** END STEP ***\n");
    return Continue;
}

SimplexData::StepResult SimplexData::doSimplex(void)
{
    StepResult res = Continue;
    while (res == Continue)
    {
        res = simpleSimplexStep();
    }
    switch (res)
    {
    case Unbounded:
        printf("Problem is unbounded, no result!\n");
        break;
    case InternalError:
        printf("Bug!\n");
        break;
    case Optimal:
        printf("Optimal value is % 6.3f\n", -1 * tab[m][m+n]);

        break;
    }
    return res;
}
