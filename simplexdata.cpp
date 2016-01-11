#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "util.h"
#include "simplexdata.h"

#undef BLANDS
#define BLANDS

#undef ALTERNATE_PIVOT
//#define ALTERNATE_PIVOT

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

void SimplexData::setup(int _m, int _n, double **_tab, int fake_count)
{
    cleanup();
    m = _m;
    n = _n - fake_count;
    tab = new double *[m + 1];
    for (int i=0; i<m+1; ++i)
    {
        tab[i] = new double[m + n + 1];
        //for (int j=0; j<m+n+1; ++j)
        //    tab[i][j] = _tab[i][j];
        // Copy the columns corresponding to non fake variables
        for (int j=0; j<m+n; ++j)
            tab[i][j] = _tab[i][j];
        // And the RHS column which may be at a different position if fake_count > 0
        tab[i][m+n] = _tab[i][m+_n];
    }
    rowlabels = new int[m];
    for (int i=0; i<m; ++i)
        rowlabels[i] = i + n + 1;
    collabels = new int[m + n];
    for (int i=0; i<m+n; ++i)
        collabels[i] = i + 1;
}

void SimplexData::copyTo(SimplexData &dest, int fake_count) const
{
    dest.cleanup();
    dest.setup(m, n, tab, fake_count);
    for (int i=0; i<m; ++i)
        dest.rowlabels[i] = rowlabels[i];
    for (int i=0; i<m+n-fake_count; ++i)
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
    for (int j=0; j<m+n; ++j)
    {
        if (tab[m][j] < 0)
            return false;
    }
    return true;
}

int SimplexData::pickPivotColumn(void)
{
#ifdef BLANDS
    // Bland's rule - pick the leftmost one that's < 0
    for (int j=0; j<n+m; ++j)
    {
        if (tab[m][j] < 0)
            return j;
    }
    return -1;
#else
    double curmin = 0;
    int curindex = -1;
    for (int j=0; j<m+n; ++j)
    {
        if ((tab[m][j] < 0) && (tab[m][j] < curmin))
        {
            curmin = tab[m][j];
            curindex = j;
        }
    }
    return curindex;
#endif
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
                    (curratio < bestratio)
#ifdef BLANDS
                    ||
                    ((curratio == bestratio) && (rowlabels[k] < rowlabels[bestindex])) // Bland's rule - pick the variable with the smallest index
#endif
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
            res->tab[i][pivotcol] = 0/*-1 * tab[i][pivotcol] / tab[pivotrow][pivotcol]*/;
    }
    for (int j=0; j<m+n+1; ++j)
    {
        if (j != pivotcol)
            res->tab[pivotrow][j] = tab[pivotrow][j] / tab[pivotrow][pivotcol];
    }
    res->tab[pivotrow][pivotcol] = 1;
    // Adjust row label in the copy as well
    res->rowlabels[pivotrow] = collabels[pivotcol];
    // Fix zeros, those -0.0 are ugly as hell
    res->fixZeros();
    // ... and return the copy
    return *res;
}

SimplexData& SimplexData::doPivot2(int pivotrow, int pivotcol)
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
#ifdef ALTERNATE_PIVOT
    *this = doPivot2(pivotrow, pivotcol);
#else
    *this = doPivot(pivotrow, pivotcol);
#endif
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
        double opt = -tab[m][m+n];
        if (isZero(opt)) opt=fabs(opt);
        printf("Optimal value is % 6.3f\n", opt);

        break;
    }
    return res;
}

void SimplexData::getFakesOut(int fake_count)
{
    //printf("Entering getFakesOut(%d)\n", fake_count);
    int fake_start = m + n - fake_count;
    bool did_pivot = true;
    while (did_pivot)
    {
        did_pivot = false;
        for (int i=0; i<m; ++i)
        {
            // Do we have a fake var on a row?
            // Row and col labels are 1 based for human consumption!
            if ((rowlabels[i] - 1) >= fake_start)
            {
                did_pivot = true;
                printf("Should get fake %d on row %d out of tableau\n", rowlabels[i], i);
                // Find a non fake that's not already in to pivot in
                int non_fake = -1;
                for (int j=0; j<fake_start; ++j)
                {
                    bool is_in = false;
                    for (int k=0; k<m; ++k)
                    {
                        //printf("rowlabels[%d] == %d, j == %d\n", k, rowlabels[k], j);
                        if ((rowlabels[k] - 1) == j)
                            is_in = true;
                    }
                    if ((!is_in) && (!isZero(tab[i][j])))
                    {
                        non_fake = j;
                        break;
                    }
                }
                if (non_fake < 0)
                {
                    //TODO: instead, remove the row here - it means all the possible pivots are zero
                    printf("Internal error, could not find non-fake to pivot in place of fake %d on row %d!\n", rowlabels[i], i);
                    return;
                }
                else
                {
                    printf("Pivoting row %d and col %d\n", i, non_fake);
                    *this = doPivot(i, non_fake);
                    printTable();
                    break; // OUT OF THE FOR!
                }
            }
        }
    }
}

void SimplexData::fixZeros(void)
{
    for (int i=0; i<m+1; ++i)
        for (int j=0; j<m+n+1; ++j)
            if (isZero(tab[i][j]))
                tab[i][j] = 0;
}
