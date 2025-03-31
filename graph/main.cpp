#include "graph.hpp"

int main() {
    GasStationGraph graph;
    
    // Adding some sample stations
    addStation(graph, "Shell", 3.45);
    addStation(graph, "BP", 3.50);
    addStation(graph, "Exxon", 3.40);
    addStation(graph, "Chevron", 3.55);
    addStation(graph, "Speedway", 3.38);
    
    // Adding connections
    addConnection(graph, 0, 1, 5.2);  // Shell to BP, 5.2 miles
    addConnection(graph, 0, 2, 3.8);  // Shell to Exxon, 3.8 miles
    addConnection(graph, 1, 3, 7.1);  // BP to Chevron, 7.1 miles
    addConnection(graph, 2, 3, 4.5);  // Exxon to Chevron, 4.5 miles
    addConnection(graph, 2, 4, 2.3);  // Exxon to Speedway, 2.3 miles
    addConnection(graph, 3, 4, 6.0);  // Chevron to Speedway, 6.0 miles
    
    // Menu system
    int choice;
    do {
        std::cout << "\nGas Station Tracker\n";
        std::cout << "1. List all stations\n";
        std::cout << "2. Show station connections\n";
        std::cout << "3. Find cheapest gas\n";
        std::cout << "4. Find path between stations\n";
        std::cout << "0. Exit\n";
        std::cout << "Enter choice: ";
        std::cin >> choice;
        
        switch (choice) {
            case 1:
                printStations(graph);
                break;
            case 2: {
                int stationId;
                std::cout << "Enter station ID: ";
                std::cin >> stationId;
                printConnections(graph, stationId);
                break;
            }
            case 3:
                findCheapestGas(graph);
                break;
            case 4: {
                int start, end;
                std::cout << "Enter start station ID: ";
                std::cin >> start;
                std::cout << "Enter end station ID: ";
                std::cin >> end;
                findPath(graph, start, end);
                break;
            }
            case 0:
                std::cout << "Exiting...\n";
                break;
            default:
                std::cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 0);
    
    cleanupGraph(graph);
    return 0;
}

