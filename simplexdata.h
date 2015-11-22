#ifndef SIMPLEXDATA_H
#define SIMPLEXDATA_H

class SimplexData
{
public:
    enum StepResult {Optimal, InternalError, Unbounded, Continue};

    SimplexData();
    SimplexData(const SimplexData &other);
    ~SimplexData();

    void cleanup(void);
    void setup(int m, int n, double **tab);
    void copyTo(SimplexData &dest) const;

    void printTable(void);

    // m is no of basic variables (and/or constraints)
    // n is no of non-basic variables
    // tableau has m + 1 rows to allow for z's coefs
    // and m + n + 1 columns to allow for all vars and the RHS column
    int m, n;
    double **tab;
    int *rowlabels;
    int *collabels;

    bool isOptimal(void);
    int pickPivotColumn(void);
    bool isUnbounded(int pivotcol);
    int pickPivotRow(int pivotcol);
    SimplexData& doPivot(int pivotrow, int pivotcol);
    SimplexData& doPivot2(int pivotrow, int pivotcol);
    StepResult simpleSimplexStep(void);
    void fixZeros(void);

    // This is only used by the two phase simplex who knows how many fake variables we have
    void getFakesOut(int fake_count);

    StepResult doSimplex(void);
};

#endif // SIMPLEXDATA_H
