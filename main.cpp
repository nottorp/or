#include <iostream>

#include <math.h>

#include "simplexdata.h"

using namespace std;

// Sweet, sweet C++ and its dynamic two dimensional arrays...
// Test 4a
int m_4a = 3;
int n_4a = 2;
double line_4a_1[] = {-2, 1, 1, 0, 0, 2};
double line_4a_2[] = {-1, 2, 0, 1, 0, 7};
double line_4a_3[] = {1, 0, 0, 0, 1, 3};
double line_4a_4[] = {-1, -1, 0, 0, 0, 0};
double *tab_4a[] = {line_4a_1, line_4a_2, line_4a_3, line_4a_4};

// Test 4b
int m_4b = 2;
int n_4b = 2;
double line_4b_1[] = {-1, 1, 1, 0, 2};
double line_4b_2[] = {-2, 1, 0, 1, 1};
double line_4b_3[] = {-1, -2, 0, 0, 0};
double *tab_4b[] = {line_4b_1, line_4b_2, line_4b_3};

// Test 4c
int m_4c = 2;
int n_4c = 3;
double line_4c_1[] = {4, 5, -2, 1, 0, 22};
double line_4c_2[] = {1, -2, 1, 0, 1, 30};
double line_4c_3[] = {3, -2, -4, 0, 0, 0};
double *tab_4c[] = {line_4c_1, line_4c_2, line_4c_3};

// Wikipedia example
int m_w = 2;
int n_w = 3;
double line_w_1[] = {3, 2, 1, 1, 0, 10};
double line_w_2[] = {2, 5, 3, 0, 1, 15};
double line_w_3[] = {-2, -3, -4, 0, 0, 0};
double *tab_w[] = {line_w_1, line_w_2, line_w_3};

// 4 (a)
#define M 3
#define N 2

double test_table[M + 1][M + N + 1] =
{
    {-2, 1, 1, 0, 0, 2},
    {-1, 2, 0, 1, 0, 7},
    {1, 0, 0, 0, 1, 3},
    {-1, -1, 0, 0, 0, 0}
};

int rowlabel[M] = {3, 4, 5};
int collabel[M + N] = {1, 2, 3, 4, 5};

// 4 (b)
/*
#define M 2
#define N 2

double test_table[M + 1][M + N + 1] =
{
    {-1, 1, 1, 0, 2},
    {-2, 1, 0, 1, 1},
    {-1, -2, 0, 0, 0}
};

int rowlabel[M] = {3, 4};
int collabel[M + N] = {1, 2, 3, 4};
*/

// 1 (a)
/*
#define M 2
#define N 2

double test_table[M + 1][M + N + 1] =
{
    {-5, 2, 1, 0, 30},
    {-3, 1, 0, 1, 12},
    {3, 9, 0, 0, 0}
};

int rowlabel[M] = {1, 2};
int collabel[M + N] = {1, 2, 3, 4};
*/

//TODO: Use these?

double epsilon = 10e-16;

bool isZero(double x)
{
    return (fabs(x) < epsilon);
}

bool equal(double x, double y)
{
    return isZero(x - y);
}

bool gtZero(double x)
{
    return (x > epsilon);
}

bool ltZero(double x)
{
    return (x < epsilon);
}

void printTable(int m, int n, double tab[M + 1][M + N + 1], int *cols, int *rows)
{
    // Header
    printf ("         |");
    for (int j=0; j<m+n; ++j)
        printf("     x% 2d ", cols[j]);
    printf("   RHS\n");
    for (int i=0; i<m+1; ++i)
    {
        if (i<m)
            printf("     x% 2d |", rows[i]);
        else
            printf("       z |");
        for (int j=0; j<m+n+1; ++j)
            printf("% 8.3f ", tab[i][j]);
        printf("\n");
    }
}


bool isOptimal(int m, int n, double tab[M + 1][M + N + 1])
{
    for (int j=0; j<n; ++j)
    {
        if (tab[m][j] < 0)
            return false;
    }
    return true;
}

int pickPivotColumn(int m, int n, double tab[M + 1][M + N + 1])
{
    // Bland's rule - pick the leftmost one that's < 0
    for (int j=0; j<n; ++j)
    {
        if (tab[m][j] < 0)
            return j;
    }
    return -1;
}

bool isUnbounded(int m, int n, double tab[M + 1][M + N + 1], int pivotcol)
{
    for (int i=0; i<m; ++i)
        if (tab[i][pivotcol] > 0)
            return false;
    return true;
}

int pickPivotRow(int m, int n, double tab[M + 1][M + N + 1], int pivotcol, int *rowlabel)
{
    double bestratio = INFINITY;
    int bestindex = -1;
    for (int k=0; k<m; ++k)
    {
        if (tab[k][pivotcol] > 0)
        {
            double curratio = tab[k][m+n] / tab[k][pivotcol];
            if (
                    (curratio < bestratio) ||
                    ((curratio == bestratio) && (rowlabel[k] < rowlabel[bestindex])) // Bland's rule - pick the variable with the smallest index
               )
            {
                bestratio = curratio;
                bestindex = k;
            }
        }
    }
    return bestindex;
}

void doPivot(int m, int n, double tab[M + 1][M + N + 1], int pivotrow, int pivotcol, int *rowlabel, int *collabel)
{

}

int simpleSimplexStep(int m, int n, double tab[M + 1][M + N + 1], int *rowlabel, int *collabel)
{
    printf("*** BEGIN STEP ***\n");
    printTable(m, n, tab, collabel, rowlabel);
    if (isOptimal(m, n, tab))
    {
        printf("Optimal, stopping.\n");
        return 1;
    }
    int pivotcol = pickPivotColumn(m, n, tab);
    if (pivotcol < 0)
    {
        printf("Internal error picking pivot column, should never get here!\n");
        return -1;
    }
    printf("Pivot column picked: %d, entering variable x%d\n", pivotcol + 1, collabel[pivotcol]);
    if (isUnbounded(m, n, tab, pivotcol))
    {
        printf("Unbounded, stopping.\n");
        return 2;
    }
    int pivotrow = pickPivotRow(m, n, tab, pivotcol, rowlabel);
    if (pivotcol < 0)
    {
        printf("Internal error picking pivot row, should never get here!\n");
        return -1;
    }
    printf ("Pivot row picked: %d, exit variable x%d\n", pivotrow + 1, rowlabel[pivotrow]);
    return 0;
}

int main()
{
    SimplexData d;
    //d.setup(m_4a, n_4a, tab_4a);
    //d.setup(m_4b, n_4b, tab_4b);
    //d.setup(m_4c, n_4c, tab_4c);
    d.setup(m_w, n_w, tab_w);
    d.doSimplex();
    //d.simpleSimplexStep();
    //printTable(M, N, test_table, collabel, rowlabel);
    //simpleSimplexStep(M, N, test_table, rowlabel, collabel);
    return 0;
}

