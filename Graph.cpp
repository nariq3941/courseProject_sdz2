//
// Created by mrfarinq on 11.05.17.
//

#include <ctgmath>
#include "Graph.h"
#include <random>
#include <fstream>

Graph::Graph() : maximumWeightOfEdge(9), amountOfEdgesInAdjacencyListOfDirectedGraph(0), adjacencyListForGraph(nullptr),
                 neighborhoodMatrixForGraph(nullptr), edgesOfDirectedGraph(nullptr), edgesOfUndirectedGraph(nullptr) {
    
}

void Graph::DeleteGraph() {
    for (int i = 0; i < amountOfEdgesInDirectedGraph; i++) {
        delete[] edgesOfDirectedGraph[i];
    }
    for (int i = 0; i < amountOfEdgesInUndirectedGraph; i++) {
        delete[] edgesOfUndirectedGraph[i];
    }
    delete[] edgesOfDirectedGraph;
    delete[] edgesOfUndirectedGraph;
    
    delete adjacencyListForGraph;
    delete neighborhoodMatrixForGraph;
    
    adjacencyListForGraph = nullptr;
    neighborhoodMatrixForGraph = nullptr;
    
    edgesOfDirectedGraph = nullptr;
    edgesOfUndirectedGraph = nullptr;
}


Graph::~Graph() {
    DeleteGraph();
}


void Graph::ReadGraphFromFile(std::string path) {
    if (adjacencyListForGraph != nullptr || neighborhoodMatrixForGraph != nullptr || edgesOfDirectedGraph != nullptr ||
        edgesOfUndirectedGraph != nullptr) {
        DeleteGraph();
    }
    std::fstream file(path, std::ios::in);
    if (file.is_open()) {
        file >> amountOfEdgesInDirectedGraph;
        file >> amountOfVertices;
        file >> firstVertex;
        file >> lastVertex;
        if (file.fail()) throw std::error_condition();
        
        edgesOfDirectedGraph = new int *[amountOfEdgesInDirectedGraph];
        adjacencyListForGraph = new AdjacencyListForGraph(amountOfVertices);
        neighborhoodMatrixForGraph = new NeighborhoodMatrixForGraph(amountOfVertices);
        
        int vertex_from, vertex_to, edge_weight;
        for (int i = 0; i < amountOfEdgesInDirectedGraph; i++) {
            file >> vertex_from;
            file >> vertex_to;
            file >> edge_weight;
            
            if (file.fail()) throw std::error_condition();
            
            edgesOfDirectedGraph[i] = new int[3];
            edgesOfDirectedGraph[i][0] = vertex_from;
            edgesOfDirectedGraph[i][1] = vertex_to;
            edgesOfDirectedGraph[i][2] = edge_weight;
            
            adjacencyListForGraph->AddEdgeForDirectedGraph(vertex_from, vertex_to, edge_weight);
            neighborhoodMatrixForGraph->AddEdgeForDirectedGraph(vertex_from, vertex_to, edge_weight);
        }
        
        file.close();
        
        GenerateUndirectedGraph();
        
        std::cout << "Reprezentacja listowa grafu skierowanego:\n\n";
        adjacencyListForGraph->PrintDirectedGraph();
        std::cout << std::endl;
        std::cout << "Reprezentacja macierzowa grafu skierowanego:\n\n";
        neighborhoodMatrixForGraph->PrintDirectedGraph();
        std::cout << std::endl;
        std::cout << "Reprezentacja listowa grafu nieskierowanego:\n\n";
        adjacencyListForGraph->PrintUndirectedGraph();
        std::cout << std::endl;
        std::cout << "Reprezentacja macierzowa grafu nieskierowanego:\n\n";
        neighborhoodMatrixForGraph->PrintUndirectedGraph();
    }
}

void Graph::SaveToFile() {
    std::fstream file("GraphGeneratedByProgram.txt", std::ios::out);
    if (file.good()) {
        file << amountOfEdgesInDirectedGraph << " " << amountOfVertices << " " << firstVertex << " "
             << lastVertex << "\n";
        for (int i = 0; i < amountOfEdgesInDirectedGraph; i++)
            file << edgesOfDirectedGraph[i][0] << " " << edgesOfDirectedGraph[i][1] << " " << edgesOfDirectedGraph[i][2]
                 << "\n";
        file.close();
    }
}

void Graph::GenerateUndirectedGraph() {
    bool *duplicate;
    duplicate = new bool[amountOfEdgesInDirectedGraph];
    for (int i = 0; i < amountOfEdgesInDirectedGraph; i++)
        duplicate[i] = false;
    edgesOfUndirectedGraph = new int *[amountOfEdgesInDirectedGraph];
    for (int i = 0; i < amountOfEdgesInDirectedGraph; i++)
        edgesOfUndirectedGraph[i] = nullptr;
    
    for (int i = 0; i < amountOfEdgesInDirectedGraph; i++) {
        for (int j = 0; j < i; j++) {
            if ((edgesOfDirectedGraph[i][0] == edgesOfDirectedGraph[j][0] &&
                 edgesOfDirectedGraph[i][1] == edgesOfDirectedGraph[j][1]) ||
                (edgesOfDirectedGraph[i][1] == edgesOfDirectedGraph[j][0] &&
                 edgesOfDirectedGraph[i][0] == edgesOfDirectedGraph[j][1])) {
                duplicate[i] = false;
                break;
            }
            duplicate[i] = true;
        }
    }
    
    int vertex_from, vertex_to, edge_weight;
    duplicate[0] = true;
    amountOfEdgesInUndirectedGraph = 0;
    for (int i = 0; i < amountOfEdgesInDirectedGraph; i++) {
        if (duplicate[i]) {
            edgesOfUndirectedGraph[amountOfEdgesInUndirectedGraph] = new int[3];
            vertex_from = edgesOfUndirectedGraph[amountOfEdgesInUndirectedGraph][0] = edgesOfDirectedGraph[i][0];
            vertex_to = edgesOfUndirectedGraph[amountOfEdgesInUndirectedGraph][1] = edgesOfDirectedGraph[i][1];
            edge_weight = edgesOfUndirectedGraph[amountOfEdgesInUndirectedGraph][2] = edgesOfDirectedGraph[i][2];
            
            adjacencyListForGraph->AddEdgeForUndirectedGraph(vertex_from, vertex_to, edge_weight);
            neighborhoodMatrixForGraph->AddEdgeForUndirectedGraph(vertex_from, vertex_to, edge_weight);
            
            amountOfEdgesInUndirectedGraph++;
        }
    }
    
    delete[] duplicate;
}

void Graph::CreateGraphWithRandomIntegers() {
    if (adjacencyListForGraph != nullptr || neighborhoodMatrixForGraph != nullptr || edgesOfDirectedGraph != nullptr ||
        edgesOfUndirectedGraph != nullptr) {
        DeleteGraph();
    }
    std::cout << "Podaj ilość wierzchołków: " << std::endl;
    std::cin >> amountOfVertices;
    if (amountOfVertices < 2) {
        throw std::invalid_argument("Liczba wierzchołków nie może być mniejsza od 2.");
    }
    
    int minimumAmountOfEdges, maximumAmountOfEdges;
    minimumAmountOfEdges = amountOfVertices - 1;
    maximumAmountOfEdges = ((amountOfVertices - 1) * amountOfVertices) / 2;
    
    auto minimumDensity = (int) ceil((((minimumAmountOfEdges * 100) / maximumAmountOfEdges)));
    
    double density;
    std::cout << "Podaj gęstość grafu nie mniejszą niż " << std::to_string(minimumDensity) << " %." << std::endl;
    std::cin >> density;
    if (density < minimumDensity) {
        throw std::invalid_argument(
                "Gęstość grafu nie może być mniejsza od gęstości minimalndej - " + std::to_string(minimumDensity) +
                " %.");
    }
    if (density > 100) {
        throw std::invalid_argument("Gęstość grafu nie może być większa od 100 %.");
    }
    
    amountOfEdgesInDirectedGraph = (int) ceil(maximumAmountOfEdges * (density / 100));
    amountOfEdgesInDirectedGraph *= 2;
    edgesOfDirectedGraph = new int *[amountOfEdgesInDirectedGraph];
    
    
    std::random_device r;
    std::mt19937 randomEngine(r());  //GENERATOR LICZB PSEUDOLOSOWYCH
    std::uniform_int_distribution<int> edgeWeight(1, 9);    //ROWNOMIERNY ROZKLAD PRAWDOPODOBIENSTWA
    std::uniform_int_distribution<int> randomVertice(0, amountOfVertices - 1);
    
    int vertex_from, vertex_to, edge_weight;
    int edgesLeft = amountOfEdgesInDirectedGraph;
    
    edgesOfDirectedGraph = new int *[amountOfEdgesInDirectedGraph];
    adjacencyListForGraph = new AdjacencyListForGraph(amountOfVertices);
    neighborhoodMatrixForGraph = new NeighborhoodMatrixForGraph(amountOfVertices);
    
    vertex_from = randomVertice(randomEngine);
    
    int i = 0;
    int j = 0;
    while (j < (amountOfVertices - 1) && edgesLeft > 0) //PIERWSZA PETLA LACZY WSZYSTKIE WIERZCHOLKI ABY GRAF BYL SPOJNY
    {
        do {
            vertex_to = randomVertice(randomEngine);
        } while (adjacencyListForGraph->GetVerticeDegree(vertex_to) > 0 || vertex_to ==
                                                                           vertex_from);  //LOSUJ WIERZCHOLEK DOCELOWY DOPOKI NIE WYLOSUJESZ NIEPOLOCZONEGO ZADNA KRAWEDZIA WIERZCHOLKA ALBO NIE WYLOSUJESZ 2 ROZNYCH WIERCHOLKOW
        edge_weight = edgeWeight(randomEngine);
        
        edgesOfDirectedGraph[i] = new int[3];
        edgesOfDirectedGraph[i][0] = vertex_from;
        edgesOfDirectedGraph[i][1] = vertex_to;
        edgesOfDirectedGraph[i][2] = edge_weight;
        
        adjacencyListForGraph->AddEdgeForDirectedGraph(vertex_from, vertex_to, edge_weight);
        neighborhoodMatrixForGraph->AddEdgeForDirectedGraph(vertex_from, vertex_to, edge_weight);
        
        vertex_from = vertex_to;
        
        --edgesLeft;
        j++;
        i++;
    }
    
    for (int k = 0; k < edgesLeft; ++k) //DRUGA PETLA DODAJE POZOSTALE - LOSOWE KRAWEDZIE
    {
        do {
            vertex_from = randomVertice(randomEngine);
            vertex_to = randomVertice(randomEngine);
            edge_weight = edgeWeight(randomEngine);
        } while (neighborhoodMatrixForGraph->GetWeightOfEdge(vertex_from, vertex_to) > 0 ||
                 vertex_to == vertex_from);   //LOSUJ DOPOKI WYLOSUJESZ KRAWEDZ KTORA NIE ISTNIEJE
        
        edgesOfDirectedGraph[i] = new int[3];
        edgesOfDirectedGraph[i][0] = vertex_from;
        edgesOfDirectedGraph[i][1] = vertex_to;
        edgesOfDirectedGraph[i][2] = edge_weight;
        
        adjacencyListForGraph->AddEdgeForDirectedGraph(vertex_from, vertex_to, edge_weight);
        neighborhoodMatrixForGraph->AddEdgeForDirectedGraph(vertex_from, vertex_to, edge_weight);
        
        i++;
    }
    
    firstVertex = edgesOfDirectedGraph[0][0];
    lastVertex = edgesOfDirectedGraph[amountOfVertices - 1][0];
    
    GenerateUndirectedGraph();
    
    std::cout << "Reprezentacja listowa grafu skierowanego:\n\n";
    adjacencyListForGraph->PrintDirectedGraph();
    std::cout << std::endl;
    std::cout << "Reprezentacja macierzowa grafu skierowanego:\n\n";
    neighborhoodMatrixForGraph->PrintDirectedGraph();
    std::cout << std::endl;
    std::cout << "Reprezentacja listowa grafu nieskierowanego:\n\n";
    adjacencyListForGraph->PrintUndirectedGraph();
    std::cout << std::endl;
    std::cout << "Reprezentacja macierzowa grafu nieskierowanego:\n\n";
    neighborhoodMatrixForGraph->PrintUndirectedGraph();
}

int Graph::GetParamOfEdge(int whichVertex, int whichParam, std::string directOrUndirect) {
    if (directOrUndirect == "direct") {
        return edgesOfDirectedGraph[whichVertex][whichParam];
    } else {
        return edgesOfUndirectedGraph[whichVertex][whichParam];
    }
}

void Graph::PrimsAlgorithmForAdjacencyListGraph() {
    adjacencyListForGraph->PrimsAlgorithm(edgesOfUndirectedGraph, firstVertex, amountOfEdgesInUndirectedGraph);
}









