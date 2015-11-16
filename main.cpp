#include <iostream>

#include <math.h>

#include "simplexdata.h"
#include "twophase.h"

using namespace std;

// Test data for homework 1-3

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


// Test data for homework 2-1

int m_2a = 2;
int n_2a = 2;

double line_2a_1[] = {1, 1, GT, 6};
double line_2a_2[] = {2, 3, LT, 4};
double line_2a_3[] = {-1, 0, NO, 0};
double *tab_2a[] = {line_2a_1, line_2a_2, line_2a_3};

int m_2b = 2;
int n_2b = 3;

double line_2b_1[] = {2, 1, 1, EQ, 4};
double line_2b_2[] = {1, 1, 2, EQ, 2};
double line_2b_3[] = {1, 1, 0, NO, 0};
double *tab_2b[] = {line_2b_1, line_2b_2, line_2b_3};

int main_13(void)
{
    SimplexData d;
    d.setup(m_4a, n_4a, tab_4a);
    d.setup(m_4b, n_4b, tab_4b);
    d.setup(m_4c, n_4c, tab_4c);
    d.setup(m_w, n_w, tab_w);
    d.doSimplex();
    return 0;
}

int main_21(void)
{
    TwoPhase t;
    t.setup(m_2a, n_2a, tab_2a);
    //t.setup(m_2b, n_2b, tab_2b);
    t.printInitialTable();
    t.addSlacks();
    t.addFakes();
    t.newObjective();
    return 0;
}

int main(void)
{
    return main_21();
}
