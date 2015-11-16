#include "twophase.h"
#include "simplexdata.h"

#include <stdio.h>

TwoPhase::TwoPhase()
{
}

void TwoPhase::setup(int _m, int _n, double **_tab)
{
    m = _m;
    n = _n;
    tab = new double *[m + 1];
    for (int i=0; i<m+1; ++i)
    {
        tab[i] = new double[n + 2];
        for (int j=0; j<n+2; ++j)
            tab[i][j] = _tab[i][j];
    }
    rowlabels = new int[m];
    for (int i=0; i<m; ++i)
        rowlabels[i] = i + n + 1;
    collabels = new int[n];
    for (int i=0; i<n; ++i)
        collabels[i] = i + 1;
}

void TwoPhase::addSlacks(void)
{
    int slacks_needed = 0;
    for (int i=0; i<m; ++i)
        if ((tab[i][n] == GT) || (tab[i][n] == LT))
            ++slacks_needed;
    if (slacks_needed > 0)
        printf("I need %d slack variables!\n", slacks_needed);
    else
        printf("I don't need any slack variables!\n");
    // It turns it
    int new_m = m;
    int new_n = n + slacks_needed;
    double **new_tab;
    int cur_slack = n;
    new_tab = new double *[m+1];
    for(int i=0; i<m; ++i)
    {
        new_tab[i] = new double[new_n + 1];
        for(int j=0;j<n; ++j)
            new_tab[i][j] = tab[i][j];
        for(int j=n; j<new_n; ++j)
            new_tab[i][j] = 0;
        if ((tab[i][n] == GT) || (tab[i][n] == LT))
        {
            if (tab[i][n] == GT)
                new_tab[i][cur_slack] = -1;
            else
                new_tab[i][cur_slack] = 1;
            ++cur_slack;
        }
        new_tab[i][new_n] = tab[i][n+1];
    }
    new_tab[m] = new double[new_n+1];
    for(int j=0; j<new_n+1; ++j)
        new_tab[m][j] = 0;
    for(int j=0; j<n; ++j)
        new_tab[m][j] = tab[m][j];

    slack_count = slacks_needed;

    after_slacks.setup(new_m, new_n - new_m, new_tab);
    printf("Tableau after adding slack vars, if needed:\n");
    after_slacks.printTable();
}

void TwoPhase::addFakes(void)
{
    int new_m = after_slacks.m;
    // One fake per constraint - formula looks silly, it's like this because the simple simplex expects to get
    // m == no of constraints and n == no of non basic vars
    int new_n = after_slacks.n + after_slacks.m * 2;
    double **new_tab = new double *[new_m + 1];
    int cur_fake = after_slacks.n + after_slacks.n;
    for(int i=0; i<new_m; ++i)
    {
        new_tab[i] = new double[new_n + 1];
        for(int j=0; j<after_slacks.n + after_slacks.m; ++j)
            new_tab[i][j] = after_slacks.tab[i][j];
        for(int j=after_slacks.n + after_slacks.m; j<new_n; ++j)
        {
            if (j == cur_fake)
                new_tab[i][j] = 1;
            else
                new_tab[i][j] = 0;
        }
        new_tab[i][new_n] = after_slacks.tab[i][after_slacks.n + after_slacks.m];
        ++cur_fake;
    }
    new_tab[new_m] = new double[new_n + 1];
    for(int j=0; j<new_n+1; ++j)
        new_tab[new_m][j] = 0;
    for(int j=0; j<after_slacks.n + after_slacks.m; ++j)
        new_tab[new_m][j] = after_slacks.tab[after_slacks.m][j];

    fake_count = new_m; // This doesn't really change, but making a copy so the code is more clear

    after_fakes.setup(new_m, new_n - new_m, new_tab);
    printf("Tableau after adding artificial vars:\n");
    after_fakes.printTable();
}

void TwoPhase::newObjective(void)
{
    printf("ENTER newObjective()\n");
    after_fakes.copyTo(after_objective);
    for(int j=0; j<after_fakes.m + after_fakes.n + 1; ++j)
    {
        after_objective.tab[after_objective.m][j] = 0;
        for(int i=0; i<after_fakes.m; ++i)
            after_objective.tab[after_objective.m][j] -= after_fakes.tab[i][j];
    }
    printf("HERE\n");
    for(int j=0; j<fake_count; ++j)
        after_objective.tab[after_objective.m][after_objective.m + after_objective.n - 1 - j] = 0;
    printf("Tableau after adjusting objective:\n");
    after_objective.printTable();
    printf("EXIT newObjective()\n");
}

void TwoPhase::runFirstPhase(void)
{
    SimplexData::StepResult res = after_objective.doSimplex();

}

void TwoPhase::printTable(void)
{
    // This will crash! Don't call!
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

void TwoPhase::printInitialTable(void)
{
    // Header
    printf ("         |");
    for (int j=0; j<n; ++j)
        printf("     x% 2d ", collabels[j]);
    printf("   TYPE   ");
    printf("   RHS\n");
    for (int i=0; i<m+1; ++i)
    {
        if (i<m)
            printf("     x% 2d |", rowlabels[i]);
        else
            printf("       z |");
        for (int j=0; j<n; ++j)
            printf("% 8.3f ", tab[i][j]);
        if (tab[i][n] == GT)
            printf("   >=    ");
        else if (tab[i][n] == LT)
            printf("   <=    ");
        else if (tab[i][n] == EQ)
            printf("    =    ");
        else
            printf("         ");
        printf("% 8.3f ", tab[i][n+1]);
        printf("\n");
    }
}
