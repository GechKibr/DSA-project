#include <iostream>
#include <cstring>
using namespace std;
// Constants
const int MAX_STATIONS = 100;
const int MAX_NAME_LENGTH = 50;

// Gas Station structure
struct GasStation {
    int id;
    char name[MAX_NAME_LENGTH];
    float gas_price;
};


// Edge structure for adjacency list
struct StationConnection {
    int destination;
    float distance;
    StationConnection* next;
};

// Graph structure
struct GasStationGraph {
    GasStation stations[MAX_STATIONS];
    StationConnection* adjacencyList[MAX_STATIONS];
    int stationCount;
    
    GasStationGraph() : stationCount(0) {
        for (int i = 0; i < MAX_STATIONS; ++i) {
            adjacencyList[i] = nullptr;
        }
    }
};

// Function prototypes
void addStation(GasStationGraph& graph, const char* name, float price);
void addConnection(GasStationGraph& graph, int src, int dest, float distance);
void printStations(const GasStationGraph& graph);
void printConnections(const GasStationGraph& graph, int stationId);
void findCheapestGas(const GasStationGraph& graph);
void findPath(const GasStationGraph& graph, int start, int end);
void cleanupGraph(GasStationGraph& graph);

// Add a new gas station to the graph
void addStation(GasStationGraph& graph, const char* name, float price) {
    if (graph.stationCount >= MAX_STATIONS) {
        std::cout << "Maximum number of stations reached.\n";
        return;
    }
    
    GasStation& newStation = graph.stations[graph.stationCount];
    newStation.id = graph.stationCount;
    strncpy(newStation.name, name, MAX_NAME_LENGTH - 1);
    newStation.name[MAX_NAME_LENGTH - 1] = '\0';
    newStation.gas_price = price;
    
    graph.stationCount++;
}

// Add a connection between two stations
void addConnection(GasStationGraph& graph, int src, int dest, float distance) {
    if (src < 0 || src >= graph.stationCount || dest < 0 || dest >= graph.stationCount) {
        std::cout << "Invalid station ID.\n";
        return;
    }
    
    // Add connection from src to dest
    StationConnection* newConnection = new StationConnection;
    newConnection->destination = dest;
    newConnection->distance = distance;
    newConnection->next = graph.adjacencyList[src];
    graph.adjacencyList[src] = newConnection;
    
    // Since roads are bidirectional, add connection from dest to src as well
    newConnection = new StationConnection;
    newConnection->destination = src;
    newConnection->distance = distance;
    newConnection->next = graph.adjacencyList[dest];
    graph.adjacencyList[dest] = newConnection;
}

// Print all gas stations
void printStations(const GasStationGraph& graph) {
    std::cout << "\nList of Gas Stations:\n";
    std::cout << "ID\tName\t\tPrice per gallon\n";
    std::cout << "--------------------------------\n";
    for (int i = 0; i < graph.stationCount; ++i) {
        const GasStation& station = graph.stations[i];
        std::cout << station.id << "\t" << station.name << "\t\t$" << station.gas_price << "\n";
    }
}

// Print all connections for a specific station
void printConnections(const GasStationGraph& graph, int stationId) {
    if (stationId < 0 || stationId >= graph.stationCount) {
        std::cout << "Invalid station ID.\n";
        return;
    }
    
    const GasStation& station = graph.stations[stationId];
    std::cout << "\nConnections for " << station.name << " (ID: " << stationId << "):\n";
    
    StationConnection* current = graph.adjacencyList[stationId];
    if (!current) {
        std::cout << "No connections found.\n";
        return;
    }
    
    while (current) {
        const GasStation& connectedStation = graph.stations[current->destination];
        std::cout << "-> " << connectedStation.name << " (ID: " << current->destination 
                  << "), Distance: " << current->distance << " miles\n";
        current = current->next;
    }
}

// Find and display the station with the cheapest gas
void findCheapestGas(const GasStationGraph& graph) {
    if (graph.stationCount == 0) {
        std::cout << "No stations available.\n";
        return;
    }
    
    int cheapestId = 0;
    for (int i = 1; i < graph.stationCount; ++i) {
        if (graph.stations[i].gas_price < graph.stations[cheapestId].gas_price) {
            cheapestId = i;
        }
    }
    
    const GasStation& cheapest = graph.stations[cheapestId];
    std::cout << "\nCheapest gas found at " << cheapest.name 
              << " (ID: " << cheapest.id << ") for $" << cheapest.gas_price << " per gallon\n";
}

// Simple path finding (Breadth-First Search)
void findPath(const GasStationGraph& graph, int start, int end) {
    if (start < 0 || start >= graph.stationCount || end < 0 || end >= graph.stationCount) {
        std::cout << "Invalid station ID.\n";
        return;
    }
    
    if (start == end) {
        std::cout << "Start and end stations are the same.\n";
        return;
    }
    
    // BFS implementation
    bool visited[MAX_STATIONS] = {false};
    int parent[MAX_STATIONS];
    for (int i = 0; i < MAX_STATIONS; ++i) {
        parent[i] = -1;
    }
    
    int queue[MAX_STATIONS];
    int front = 0, rear = 0;
    
    queue[rear++] = start;
    visited[start] = true;
    
    bool found = false;
    while (front < rear && !found) {
        int current = queue[front++];
        
        StationConnection* neighbor = graph.adjacencyList[current];
        while (neighbor && !found) {
            int neighborId = neighbor->destination;
            
            if (!visited[neighborId]) {
                visited[neighborId] = true;
                parent[neighborId] = current;
                queue[rear++] = neighborId;
                
                if (neighborId == end) {
                    found = true;
                }
            }
            
            neighbor = neighbor->next;
        }
    }
    
    if (!found) {
        std::cout << "No path exists between " << graph.stations[start].name 
                  << " and " << graph.stations[end].name << ".\n";
        return;
    }
    
    // Reconstruct path
    int path[MAX_STATIONS];
    int pathLength = 0;
    int current = end;
    
    while (current != -1) {
        path[pathLength++] = current;
        current = parent[current];
    }
    
    // Print path in reverse order
    std::cout << "\nPath from " << graph.stations[start].name << " to " 
              << graph.stations[end].name << ":\n";
    
    for (int i = pathLength - 1; i >= 0; --i) {
        std::cout << graph.stations[path[i]].name;
        if (i > 0) {
            // Calculate distance between this station and the next in path
            int from = path[i];
            int to = path[i-1];
            StationConnection* conn = graph.adjacencyList[from];
            while (conn && conn->destination != to) {
                conn = conn->next;
            }
            
            if (conn) {
                std::cout << " --(" << conn->distance << " miles)--> ";
            } else {
                std::cout << " -> ";
            }
        }
    }
    std::cout << "\n";
}

// Clean up dynamically allocated memory
void cleanupGraph(GasStationGraph& graph) {
    for (int i = 0; i < MAX_STATIONS; ++i) {
        StationConnection* current = graph.adjacencyList[i];
        while (current) {
            StationConnection* temp = current;
            current = current->next;
            delete temp;
        }
        graph.adjacencyList[i] = nullptr;
    }
}
