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
    // Functions assume that, when they receive fake_count > 0, they get a tableau that has had those fakes already
    // added - i.e. no check is made that n is indeed >= fake_count
    // It is only used internally by the two phase simplex
    void setup(int m, int n, double **tab, int fake_count=0);
    void copyTo(SimplexData &dest, int fake_count = 0) const;

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
    void swapCols(int a, int b);

    // This is only used by the two phase simplex who knows how many fake variables we have
    void getFakesOut(int fake_count);

    StepResult doSimplex(void);
};

#endif // SIMPLEXDATA_H
