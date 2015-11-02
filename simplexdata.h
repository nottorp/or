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

    void doSimplex(void);
};

#endif // SIMPLEXDATA_H
