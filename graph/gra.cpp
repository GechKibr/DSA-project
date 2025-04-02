// Gas Station Network using Graph Data Structure
// No standard headers used - raw implementation

// Vertex representation: Each gas station is a vertex with an ID
struct GasStation {
    int id;             // Unique identifier for the gas station
    char* name;         // Name of the gas station
    float fuelPrice;    // Current fuel price at this station
    
    GasStation(int i, char* n, float price) : id(i), fuelPrice(price) {
        // Manual string copy since we can't use string.h
        name = new char[50]; // Assuming max 50 chars for name
        for(int j = 0; n[j] != '\0' && j < 49; j++) {
            name[j] = n[j];
        }
    }
    
    ~GasStation() {
        delete[] name;
    }
};

// Edge representation: Connection between gas stations with distance
struct RoadConnection {
    int destinationId;  // ID of destination gas station
    float distance;     // Distance between stations in km
    RoadConnection* next; // Pointer to next connection in adjacency list
    
    RoadConnection(int dest, float dist) 
        : destinationId(dest), distance(dist), next(nullptr) {}
};

// Graph class to represent the gas station network
class GasStationNetwork {
private:
    GasStation** stations;      // Array of gas station pointers
    RoadConnection** adjList;   // Adjacency list for connections
    int capacity;               // Maximum number of stations
    int size;                  // Current number of stations
    
    // Helper function to print numbers (replaces std::cout)
    void printNumber(int num) {
        if(num == 0) {
            putChar('0');
            return;
        }
        
        if(num < 0) {
            putChar('-');
            num = -num;
        }
        
        char buffer[20];
        int i = 0;
        
        while(num != 0) {
            buffer[i++] = '0' + (num % 10);
            num /= 10;
        }
        
        for(int j = i-1; j >= 0; j--) {
            putChar(buffer[j]);
        }
    }
    
    // Helper function to print floats
    void printFloat(float num) {
        int intPart = (int)num;
        printNumber(intPart);
        putChar('.');
        int fracPart = (int)((num - intPart) * 100); // 2 decimal places
        printNumber(fracPart);
    }
    
    // Helper function to print strings
    void printString(const char* str) {
        while(*str != '\0') {
            putChar(*str);
            str++;
        }
    }
    
    // System-dependent character output (placeholder)
    void putChar(char c) {
        // In a real implementation, this would use platform-specific output
        // For example: write(1, &c, 1) on Unix-like systems
        // Here we'll just simulate it
        asm volatile("" : : "r"(c)); // Does nothing, just for demonstration
    }
    
public:
    // Constructor
    GasStationNetwork(int maxStations) : capacity(maxStations), size(0) {
        stations = new GasStation*[capacity];
        adjList = new RoadConnection*[capacity];
        for(int i = 0; i < capacity; i++) {
            stations[i] = nullptr;
            adjList[i] = nullptr;
        }
    }
    
    // Destructor
    ~GasStationNetwork() {
        // Delete all stations
        for(int i = 0; i < size; i++) {
            delete stations[i];
        }
        delete[] stations;
        
        // Delete all adjacency lists
        for(int i = 0; i < capacity; i++) {
            RoadConnection* current = adjList[i];
            while(current != nullptr) {
                RoadConnection* temp = current;
                current = current->next;
                delete temp;
            }
        }
        delete[] adjList;
    }
    
    // Add a new gas station (vertex) to the network
    void addStation(int id, char* name, float fuelPrice) {
        if(size >= capacity) {
            printString("Network at full capacity!\n");
            return;
        }
        
        // Check if ID already exists
        for(int i = 0; i < size; i++) {
            if(stations[i]->id == id) {
                printString("Station ID already exists!\n");
                return;
            }
        }
        
        stations[size] = new GasStation(id, name, fuelPrice);
        size++;
    }
    
    // Add a road connection (edge) between two stations
    void addConnection(int srcId, int destId, float distance) {
        int srcIndex = -1, destIndex = -1;
        
        // Find indices of source and destination stations
        for(int i = 0; i < size; i++) {
            if(stations[i]->id == srcId) srcIndex = i;
            if(stations[i]->id == destId) destIndex = i;
        }
        
        if(srcIndex == -1 || destIndex == -1) {
            printString("One or both stations not found!\n");
            return;
        }
        
        // Add to adjacency list (both directions for undirected graph)
        RoadConnection* newConn = new RoadConnection(destId, distance);
        newConn->next = adjList[srcIndex];
        adjList[srcIndex] = newConn;
        
        newConn = new RoadConnection(srcId, distance);
        newConn->next = adjList[destIndex];
        adjList[destIndex] = newConn;
    }
    
    // Remove a connection between two stations
    void removeConnection(int srcId, int destId) {
        int srcIndex = -1, destIndex = -1;
        
        // Find indices of source and destination stations
        for(int i = 0; i < size; i++) {
            if(stations[i]->id == srcId) srcIndex = i;
            if(stations[i]->id == destId) destIndex = i;
        }
        
        if(srcIndex == -1 || destIndex == -1) {
            printString("One or both stations not found!\n");
            return;
        }
        
        // Remove from source's adjacency list
        RoadConnection* prev = nullptr;
        RoadConnection* current = adjList[srcIndex];
        
        while(current != nullptr) {
            if(current->destinationId == destId) {
                if(prev == nullptr) {
                    adjList[srcIndex] = current->next;
                } else {
                    prev->next = current->next;
                }
                delete current;
                break;
            }
            prev = current;
            current = current->next;
        }
        
        // Remove from destination's adjacency list (undirected graph)
        prev = nullptr;
        current = adjList[destIndex];
        
        while(current != nullptr) {
            if(current->destinationId == srcId) {
                if(prev == nullptr) {
                    adjList[destIndex] = current->next;
                } else {
                    prev->next = current->next;
                }
                delete current;
                break;
            }
            prev = current;
            current = current->next;
        }
    }
    
    // Remove a gas station and all its connections
    void removeStation(int id) {
        int index = -1;
        
        // Find the station
        for(int i = 0; i < size; i++) {
            if(stations[i]->id == id) {
                index = i;
                break;
            }
        }
        
        if(index == -1) {
            printString("Station not found!\n");
            return;
        }
        
        // First remove all connections to this station
        for(int i = 0; i < size; i++) {
            if(i == index) continue;
            
            RoadConnection* prev = nullptr;
            RoadConnection* current = adjList[i];
            
            while(current != nullptr) {
                if(current->destinationId == id) {
                    if(prev == nullptr) {
                        adjList[i] = current->next;
                    } else {
                        prev->next = current->next;
                    }
                    RoadConnection* temp = current;
                    current = current->next;
                    delete temp;
                } else {
                    prev = current;
                    current = current->next;
                }
            }
        }
        
        // Delete the station's own adjacency list
        RoadConnection* current = adjList[index];
        while(current != nullptr) {
            RoadConnection* temp = current;
            current = current->next;
            delete temp;
        }
        adjList[index] = nullptr;
        
        // Remove the station from the array
        delete stations[index];
        
        // Shift remaining elements
        for(int i = index; i < size-1; i++) {
            stations[i] = stations[i+1];
            adjList[i] = adjList[i+1];
        }
        
        size--;
        stations[size] = nullptr;
        adjList[size] = nullptr;
    }
    
    // Breadth-First Traversal starting from a given station
    void BFS(int startId) {
        int startIndex = -1;
        
        // Find the starting station
        for(int i = 0; i < size; i++) {
            if(stations[i]->id == startId) {
                startIndex = i;
                break;
            }
        }
        
        if(startIndex == -1) {
            printString("Starting station not found!\n");
            return;
        }
        
        // Create a queue for BFS (using a circular array)
        int* queue = new int[size];
        int front = 0, rear = 0;
        
        // Visited array
        bool* visited = new bool[size];
        for(int i = 0; i < size; i++) visited[i] = false;
        
        // Mark current station as visited and enqueue
        visited[startIndex] = true;
        queue[rear++] = startIndex;
        rear %= size;
        
        printString("BFS Traversal: ");
        
        while(front != rear) {
            // Dequeue a station and print it
            int current = queue[front++];
            front %= size;
            
            printNumber(stations[current]->id);
            printString(" ");
            
            // Get all adjacent stations
            RoadConnection* conn = adjList[current];
            while(conn != nullptr) {
                // Find the index of the adjacent station
                int adjIndex = -1;
                for(int i = 0; i < size; i++) {
                    if(stations[i]->id == conn->destinationId) {
                        adjIndex = i;
                        break;
                    }
                }
                
                if(adjIndex != -1 && !visited[adjIndex]) {
                    visited[adjIndex] = true;
                    queue[rear++] = adjIndex;
                    rear %= size;
                }
                
                conn = conn->next;
            }
        }
        
        printString("\n");
        
        delete[] queue;
        delete[] visited;
    }
    
    // Find shortest path between two stations using BFS (unweighted)
    void shortestPath(int startId, int endId) {
        int startIndex = -1, endIndex = -1;
        
        // Find the stations
        for(int i = 0; i < size; i++) {
            if(stations[i]->id == startId) startIndex = i;
            if(stations[i]->id == endId) endIndex = i;
        }
        
        if(startIndex == -1 || endIndex == -1) {
            printString("One or both stations not found!\n");
            return;
        }
        
        // For tracking path
        int* parent = new int[size];
        for(int i = 0; i < size; i++) parent[i] = -1;
        
        // Create a queue for BFS
        int* queue = new int[size];
        int front = 0, rear = 0;
        
        // Visited array
        bool* visited = new bool[size];
        for(int i = 0; i < size; i++) visited[i] = false;
        
        // Mark current station as visited and enqueue
        visited[startIndex] = true;
        queue[rear++] = startIndex;
        
        bool found = false;
        
        while(front != rear && !found) {
            // Dequeue a station
            int current = queue[front++];
            
            // Get all adjacent stations
            RoadConnection* conn = adjList[current];
            while(conn != nullptr && !found) {
                // Find the index of the adjacent station
                int adjIndex = -1;
                for(int i = 0; i < size; i++) {
                    if(stations[i]->id == conn->destinationId) {
                        adjIndex = i;
                        break;
                    }
                }
                
                if(adjIndex != -1 && !visited[adjIndex]) {
                    visited[adjIndex] = true;
                    parent[adjIndex] = current;
                    queue[rear++] = adjIndex;
                    
                    // Check if we've reached the destination
                    if(adjIndex == endIndex) {
                        found = true;
                    }
                }
                
                conn = conn->next;
            }
        }
        
        if(!found) {
            printString("No path exists between the stations!\n");
        } else {
            // Reconstruct path
            int* path = new int[size];
            int pathLength = 0;
            int current = endIndex;
            
            while(current != -1) {
                path[pathLength++] = current;
                current = parent[current];
            }
            
            printString("Shortest path (by number of stations): ");
            
            // Print path in reverse order
            for(int i = pathLength-1; i >= 0; i--) {
                printNumber(stations[path[i]]->id);
                if(i > 0) printString(" -> ");
            }
            printString("\n");
            
            delete[] path;
        }
        
        delete[] queue;
        delete[] visited;
        delete[] parent;
    }
    
    // Print the entire network
    void printNetwork() {
        printString("Gas Station Network:\n");
        for(int i = 0; i < size; i++) {
            printString("Station ");
            printNumber(stations[i]->id);
            printString(" (");
            printString(stations[i]->name);
            printString(") - Fuel Price: ");
            printFloat(stations[i]->fuelPrice);
            printString("\nConnections: ");
            
            RoadConnection* conn = adjList[i];
            while(conn != nullptr) {
                printNumber(conn->destinationId);
                printString(" (");
                printFloat(conn->distance);
                printString(" km)");
                if(conn->next != nullptr) printString(", ");
                conn = conn->next;
            }
            printString("\n\n");
        }
    }
};

// Example usage
int main() {
    // Create a network with capacity for 10 stations
    GasStationNetwork network(10);
    
    // Add some gas stations
    network.addStation(1, "Shell Downtown", 3.45);
    network.addStation(2, "BP Highway", 3.30);
    network.addStation(3, "Exxon Suburb", 3.55);
    network.addStation(4, "Chevron Mall", 3.60);
    network.addStation(5, "Texaco Industrial", 3.25);
    
    // Add connections between stations
    network.addConnection(1, 2, 5.2);   // Shell to BP, 5.2 km
    network.addConnection(1, 3, 8.7);   // Shell to Exxon, 8.7 km
    network.addConnection(2, 4, 12.1);  // BP to Chevron, 12.1 km
    network.addConnection(3, 4, 6.3);   // Exxon to Chevron, 6.3 km
    network.addConnection(3, 5, 9.8);   // Exxon to Texaco, 9.8 km
    
    // Print the network
    network.printNetwork();
    
    // Perform BFS starting from station 1
    network.BFS(1);
    
    // Find shortest path from station 1 to station 5
    network.shortestPath(1, 5);
    
    // Remove a connection and a station
    network.removeConnection(3, 5);
    network.removeStation(4);
    
    // Print the updated network
    network.printNetwork();
    
    return 0;
}