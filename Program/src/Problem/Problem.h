#ifndef _PROBLEM_H
#define _PROBLEM_H

#include <vector>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <numeric>

/*
 * Estrutura de dados que representa a instância do problema
 * de Conjunto Dominante (Dominating Set).
 */
struct TProblemData
{
    int n;                             // Tamanho do vetor de chaves aleatórias (número de vértices)
    int nVertices;                     // Quantidade de vértices no grafo
    int nEdges;                        // Quantidade de arestas no grafo
    std::vector<std::vector<int>> adj; // Lista de adjacências (vizinhos de cada vértice)
};

/*
 * Função de leitura da instância do problema.
 * Lê o grafo a partir de um arquivo no formato:
 *   nVertices nEdges
 *   u1 v1
 *   u2 v2
 *   E assim por diante
 * Cada linha após a primeira contém uma aresta (u, v).
 */
void ReadData(char name[], TProblemData &data)
{
    FILE *arq;
    arq = fopen(name, "r");

    if (arq == NULL)
    {
        // Caso o arquivo não seja encontrado
        printf("\nERROR: File (%s) not found!\n", name);
        getchar();
        exit(1);
    }

    // Lê o número de vértices e arestas
    fscanf(arq, "%d %d", &data.nVertices, &data.nEdges);

    // Inicializa a lista de adjacências
    data.adj.clear();
    data.adj.resize(data.nVertices);

    // Lê cada aresta e atualiza a lista de adjacências
    int u, v;
    for (int i = 0; i < data.nEdges; i++)
    {
        if (fscanf(arq, "%d %d", &u, &v) != 2)
        {
            printf("Erro lendo aresta %d do arquivo %s\n", i, name);
            exit(1);
        }
        // Verifica se os vértices são válidos
        if (u < 0 || u >= data.nVertices || v < 0 || v >= data.nVertices)
        {
            printf("Aresta invalida no arquivo %s: %d %d\n", name, u, v);
            exit(1);
        }
        // Grafo não direcionado -> adiciona em ambas as listas
        data.adj[u].push_back(v);
        data.adj[v].push_back(u);
    }

    // O tamanho do vetor de chaves vai ser igual ao número de vértices
    data.n = data.nVertices;

    fclose(arq);
}

/*
 * Função Decoder.
 * Recebe uma solução codificada como vetor de chaves aleatórias (TSol &s),
 * interpreta essa solução e calcula seu custo (tamanho do conjunto dominante).
 *
 * A função retorna um valor double, que é o número de vértices escolhidos
 * no conjunto dominante. Quanto menor, melhor.
 */
double Decoder(TSol &s, const TProblemData &data)
{
    const int n = data.n;

    // Cria um vetor com todos os vértices [0, 1, ..., n-1]
    std::vector<int> order(n);
    std::iota(order.begin(), order.end(), 0);

    // Ordena os vértices de acordo com suas chaves aleatórias (rk)
    std::sort(order.begin(), order.end(), [&](int a, int b)
              { return s.rk[a] < s.rk[b]; });

    // Vetor que guarda quantas vezes cada vértice está dominado
    std::vector<int> coverCount(n, 0);

    // Vetor dos vértices escolhidos no conjunto dominante
    std::vector<int> chosen;
    chosen.reserve(n);

    // Função auxiliar para adicionar um vértice ao conjunto dominante
    auto add_vertex = [&](int v)
    {
        chosen.push_back(v);

        // O vértice domina a si próprio
        coverCount[v]++;

        // E domina todos os seus vizinhos
        for (int u : data.adj[v])
            coverCount[u]++;
    };

    /*
     * Construção gulosa do conjunto dominante:
     * Percorre os vértices em ordem crescente de chave aleatória
     * e adiciona ao conjunto dominante sempre que necessário.
     */
    for (int v : order)
    {
        bool need = (coverCount[v] == 0); // v não está dominado?
        if (!need)
        {
            // Verifica se há vizinhos ainda não dominados
            for (int u : data.adj[v])
                if (coverCount[u] == 0)
                {
                    need = true;
                    break;
                }
        }
        if (need)
            add_vertex(v);
    }

    /*
     * através do prunning, tenta remover vértices redundantes.
     * Se após a remoção todos os vértices continuarem dominados,
     * então o vértice era redundante e pode ser descartado.
     */
    for (int i = (int)chosen.size() - 1; i >= 0; --i)
    {
        int v = chosen[i];
        bool canRemove = (coverCount[v] > 1); // v ainda está dominado por outro?

        if (canRemove)
        {
            for (int u : data.adj[v])
            {
                if (coverCount[u] <= 1)
                {
                    canRemove = false;
                    break;
                }
            }
        }
        if (canRemove)
        {
            // Remove v do conjunto dominante e atualiza coberturas
            coverCount[v]--;
            for (int u : data.adj[v])
                coverCount[u]--;
            chosen[i] = chosen.back();
            chosen.pop_back();
        }
    }

    /*
     * Verificação de segurança: se algum vértice não estiver dominado,
     * retorna um valor muito alto, portanto invalidando a solução.
     */
    for (int i = 0; i < n; ++i)
        if (coverCount[i] == 0)
            return 1e9;

    // Retorna o tamanho do conjunto dominante construído
    return (double)chosen.size();
}

/*
 * Função para liberar memória da instância.
 */
void FreeMemoryProblem(TProblemData &data)
{
    data.adj.clear();
}

#endif
