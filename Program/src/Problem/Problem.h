#ifndef _PROBLEM_H
#define _PROBLEM_H

#include <vector>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <algorithm>

//----------------- STRUCT -----------------
struct TProblemData
{
    int n;                             // tamanho do vetor de chaves
    int nVertices;                     // número de vértices
    int nEdges;                        // número de arestas
    std::vector<std::vector<int>> adj; // lista de adjacência
};

//----------------- LEITURA -----------------
void ReadData(char name[], TProblemData &data)
{
    FILE *arq = fopen(name, "r");
    if (arq == NULL)
    {
        printf("\nERROR: File (%s) not found!\n", name);
        getchar();
        exit(1);
    }

    fscanf(arq, "%d %d", &data.nVertices, &data.nEdges);
    data.adj.assign(data.nVertices, std::vector<int>());

    int u, v;
    for (int i = 0; i < data.nEdges; i++)
    {
        if (fscanf(arq, "%d %d", &u, &v) != 2)
            break;
        data.adj[u].push_back(v);
        data.adj[v].push_back(u);
    }

    data.n = data.nVertices;
    fclose(arq);
}

//----------------- DECODER -----------------
double Decoder(TSol &s, const TProblemData &data)
{
    // ordem dos vértices pelas chaves aleatórias
    std::vector<std::pair<double, int>> order;
    order.reserve(data.n);
    for (int i = 0; i < data.n; i++)
        order.push_back({s.rk[i], i});
    std::sort(order.begin(), order.end()); // menor chave primeiro

    std::vector<int> dominated(data.n, 0);
    std::vector<int> selected(data.n, 0);

    int covered = 0;
    int setSize = 0;

    // escolhe vértices até dominar todo o grafo
    for (auto &p : order)
    {
        int v = p.second;
        if (dominated[v])
            continue;

        selected[v] = 1;
        setSize++;

        if (!dominated[v])
        {
            dominated[v] = 1;
            covered++;
        }
        for (int neigh : data.adj[v])
        {
            if (!dominated[neigh])
            {
                dominated[neigh] = 1;
                covered++;
            }
        }
        if (covered == data.n)
            break;
    }

    // se não conseguiu cobrir tudo, penaliza
    if (covered < data.n)
        return 1e6 + (data.n - covered);

    return setSize;
}

//----------------- LIBERAR MEMÓRIA -----------------
void FreeMemoryProblem(TProblemData &data)
{
    data.adj.clear();
    data.adj.shrink_to_fit();
}

#endif
