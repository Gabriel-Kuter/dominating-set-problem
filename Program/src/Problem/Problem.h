// *******************************************************************
//      file with specific functions to solve Dominating Set problem
// *******************************************************************
#ifndef _PROBLEM_H
#define _PROBLEM_H

#include <vector>
#include <algorithm>
#include <cstdio>
#include <cstdlib>

//----------------- DEFINITION OF PROBLEM SPECIFIC TYPES -----------------------

struct TProblemData
{
    int n;                             // size of the RKO vector = number of vertices
    int nVertices;                     // number of vertices
    int nEdges;                        // number of edges
    std::vector<std::vector<int>> adj; // adjacency list
};

//-------------------------- FUNCTIONS OF SPECIFIC PROBLEM --------------------------

/************************************************************************************
 Method: ReadData
 Description: read the input graph
*************************************************************************************/
void ReadData(char name[], TProblemData &data)
{
    FILE *arq;
    arq = fopen(name, "r");

    if (arq == NULL)
    {
        printf("\nERROR: File (%s) not found!\n", name);
        getchar();
        exit(1);
    }

    // read number of vertices and edges
    fscanf(arq, "%d %d", &data.nVertices, &data.nEdges);

    // init adjacency list
    data.adj.clear();
    data.adj.resize(data.nVertices);

    int u, v;
    for (int i = 0; i < data.nEdges; i++)
    {
        fscanf(arq, "%d %d", &u, &v);
        // assume vertices start at 1 in the file → convert to 0-index
        u--;
        v--;
        data.adj[u].push_back(v);
        data.adj[v].push_back(u);
    }

    // define the random-key vector size = number of vertices
    data.n = data.nVertices;

    fclose(arq);
}

/************************************************************************************
 Method: Decoder
 Description: mapping the random-key solution into a problem solution
*************************************************************************************/
double Decoder(TSol &s, const TProblemData &data)
{
    // order vertices by random key (smallest first)
    std::vector<int> order(data.n);
    for (int i = 0; i < data.n; i++)
        order[i] = i;
    std::sort(order.begin(), order.end(), [&](int a, int b)
              { return s.rk[a] < s.rk[b]; });

    // construct dominating set
    std::vector<int> dominated(data.n, 0);
    std::vector<int> solution;
    for (int v : order)
    {
        bool needed = !dominated[v];
        if (!needed)
        {
            for (int u : data.adj[v])
            {
                if (!dominated[u])
                {
                    needed = true;
                    break;
                }
            }
        }
        if (needed)
        {
            solution.push_back(v);
            dominated[v] = 1;
            for (int u : data.adj[v])
                dominated[u] = 1;
        }
    }

    int cost = solution.size();

    // penalize infeasible solutions (shouldn’t happen, but just to be sure)
    for (int i = 0; i < data.n; i++)
    {
        if (!dominated[i])
            cost += 100000;
    }

    // RKO é de minimização, então devolvemos cost
    return (double)cost;
}

/************************************************************************************
 Method: FreeMemoryProblem
 Description: Free local memory allocate by Problem
*************************************************************************************/
void FreeMemoryProblem(TProblemData &data)
{
    data.adj.clear();
}

#endif