#ifndef TWOPHASE_H
#define TWOPHASE_H

#define GT 1
#define LT 2
#define EQ 3
#define NO 4

#include "simplexdata.h"

class TwoPhase
{
public:
    TwoPhase();

    // This is not simple simplex ready
    // m is no of constraints
    // n is no of variables
    // tab has m+1 lines to allow for z's expression
    // and n+2 columns, n being the sign of the constraint and n+1 the b value
    int m, n;
    double **tab;
    int *collabels;
    int *rowlabels;

    int slack_count;
    SimplexData after_slacks;
    int fake_count;
    SimplexData after_fakes;
    SimplexData after_objective;

    void setup(int _m, int _n, double **_tab);
    void addSlacks(void);
    void addFakes(void);
    void newObjective(void);
    void printInitialTable(void);
    void printTable(void);
};

#endif // TWOPHASE_H
