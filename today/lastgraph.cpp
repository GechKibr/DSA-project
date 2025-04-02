#include <iostream>
#include <string>
using namespace std;

class IntStack {
private:
    int* data;
    int capacity;
    int topIndex;
    
    void resize() {
        int newCapacity = capacity * 2;
        int* newData = new int[newCapacity];
        for (int i = 0; i < capacity; i++) {
            newData[i] = data[i];
        }
        delete[] data;
        data = newData;
        capacity = newCapacity;
    }

public:
    IntStack() : capacity(10), topIndex(-1) {
        data = new int[capacity];
    }
    
    ~IntStack() {
        delete[] data;
    }
    
    void push(int value) {
        if (topIndex == capacity - 1) {
            resize();
        }
        data[++topIndex] = value;
    }
    
    int pop() {
        if (empty()) {
            throw "Stack is empty";
        }
        return data[topIndex--];
    }
    
    bool empty() const {
        return topIndex == -1;
    }
};

class IntQueue {
private:
    int* data;
    int capacity;
    int head;
    int tail;
    int count;
    
    void resize() {
        int newCapacity = capacity * 2;
        int* newData = new int[newCapacity];
        
        for (int i = 0; i < count; i++) {
            newData[i] = data[(head + i) % capacity];
        }
        
        delete[] data;
        data = newData;
        capacity = newCapacity;
        head = 0;
        tail = count;
    }

public:
    IntQueue() : capacity(10), head(0), tail(0), count(0) {
        data = new int[capacity];
    }
    
    ~IntQueue() {
        delete[] data;
    }
    
    void push(int value) {
        if (count == capacity) {
            resize();
        }
        data[tail] = value;
        tail = (tail + 1) % capacity;
        count++;
    }
    
    int pop() {
        if (empty()) {
            throw "Queue is empty";
        }
        int value = data[head];
        head = (head + 1) % capacity;
        count--;
        return value;
    }
    
    bool empty() const {
        return count == 0;
    }
};

class GasStationGraph {
private:
    struct Station {
        int id;
        std::string name;
        double gasPrice;
    };

    struct Edge {
        int destination;
        double distance;
        Edge* next;
    };

    struct StationNode {
        Station station;
        Edge* edges;
        StationNode* next;
    };

    StationNode* head;
    int stationCount;

    StationNode* findStation(int id) {
        StationNode* current = head;
        while (current != nullptr) {
            if (current->station.id == id) {
                return current;
            }
            current = current->next;
        }
        return nullptr;
    }

public:
    GasStationGraph() : head(nullptr), stationCount(0) {}

    ~GasStationGraph() {
        StationNode* current = head;
        while (current != nullptr) {
            Edge* edge = current->edges;
            while (edge != nullptr) {
                Edge* temp = edge;
                edge = edge->next;
                delete temp;
            }
            StationNode* temp = current;
            current = current->next;
            delete temp;
        }
    }

    void addStation(int id, std::string name, double price) {
        StationNode* newNode = new StationNode();
        newNode->station = {id, name, price};
        newNode->edges = nullptr;
        newNode->next = head;
        head = newNode;
        stationCount++;
    }

    void addConnection(int source, int destination, double distance) {
        StationNode* srcNode = findStation(source);
        StationNode* destNode = findStation(destination);
        
        if (srcNode && destNode) {
            Edge* newEdge = new Edge();
            newEdge->destination = destination;
            newEdge->distance = distance;
            newEdge->next = srcNode->edges;
            srcNode->edges = newEdge;
            
            Edge* reverseEdge = new Edge();
            reverseEdge->destination = source;
            reverseEdge->distance = distance;
            reverseEdge->next = destNode->edges;
            destNode->edges = reverseEdge;
        }
    }

    void displayNetwork() {
        if (head == nullptr) {
            std::cout << "No stations in the network yet.\n";
            return;
        }

        cout << "\n===== CURRENT GAS STATION NETWORK =====\n";
        StationNode* current = head;
        while (current != nullptr) {
            cout << "Station " << current->station.id << ": " << current->station.name 
                     << " (Price: $" << current->station.gasPrice << ")\n";
            cout << "  Connected to: ";
            
            Edge* edge = current->edges;
            if (edge == nullptr) {
                cout << "No connections";
            } else {
                while (edge != nullptr) {
                    StationNode* dest = findStation(edge->destination);
                    if (dest) {
                        cout << dest->station.name << " (" << edge->distance << " miles), ";
                    }
                    edge = edge->next;
                }
            }
            cout << "\n\n";
            current = current->next;
        }
    }

    void DFS(int startId) {
        StationNode* startNode = findStation(startId);
        if (startNode == nullptr) {
            cout << "Invalid starting station.\n";
            return;
        }

        bool* visited = new bool[stationCount]();
        IntStack stk;
        
        stk.push(startId);
        visited[startId] = true;
        
        cout << "\nDFS Traversal starting from " << startNode->station.name << ":\n";
        
        while (!stk.empty()) {
            int currentId = stk.pop();
            StationNode* current = findStation(currentId);
            cout << "Visited: " << current->station.name << "\n";
            
            Edge* edge = current->edges;
            while (edge != nullptr) {
                if (!visited[edge->destination]) {
                    visited[edge->destination] = true;
                    stk.push(edge->destination);
                }
                edge = edge->next;
            }
        }
        
        delete[] visited;
    }

    void BFS(int startId) {
        StationNode* startNode = findStation(startId);
        if (startNode == nullptr) {
            cout << "Invalid starting station.\n";
            return;
        }

        bool* visited = new bool[stationCount]();
        IntQueue q;
        
        q.push(startId);
        visited[startId] = true;
        
        cout << "\nBFS Traversal starting from " << startNode->station.name << ":\n";
        
        while (!q.empty()) {
            int currentId = q.pop();
            StationNode* current = findStation(currentId);
            cout << "Visited: " << current->station.name << "\n";
            
            Edge* edge = current->edges;
            while (edge != nullptr) {
                if (!visited[edge->destination]) {
                    visited[edge->destination] = true;
                    q.push(edge->destination);
                }
                edge = edge->next;
            }
        }
        
        delete[] visited;
    }

    void findCheapestInRange(int startId, int maxHops) {
        StationNode* startNode = findStation(startId);
        if (startNode == nullptr) {
            std::cout << "Invalid starting station.\n";
            return;
        }

        bool* visited = new bool[stationCount]();
        int* hopCount = new int[stationCount]();
        IntQueue q;
        
        q.push(startId);
        visited[startId] = true;
        Station cheapest = startNode->station;
        
        while (!q.empty()) {
            int currentId = q.pop();
            StationNode* current = findStation(currentId);
            
            if (current->station.gasPrice < cheapest.gasPrice && 
                hopCount[currentId] <= maxHops) {
                cheapest = current->station;
            }
            
            if (hopCount[currentId] >= maxHops) continue;
            
            Edge* edge = current->edges;
            while (edge != nullptr) {
                if (!visited[edge->destination]) {
                    visited[edge->destination] = true;
                    hopCount[edge->destination] = hopCount[currentId] + 1;
                    q.push(edge->destination);
                }
                edge = edge->next;
            }
        }
        
        cout << "\nCheapest station within " << maxHops << " hops from " 
                 << startNode->station.name << " is " << cheapest.name 
                 << " with price $" << cheapest.gasPrice << "\n";
        
        delete[] visited;
        delete[] hopCount;
    }

    int getStationCount() const {
        return stationCount;
    }

    string getStationName(int id) {
        StationNode* node = findStation(id);
        if (node) {
            return node->station.name;
        }
        return "Invalid Station ID";
    }
};

void clearInputBuffer() {
    cin.clear();
    while (std::cin.get() != '\n') {
        continue;
    }
}

int main() {
    GasStationGraph gasNetwork;
    int choice;

    do {
        cout << "\n===== GAS STATION TRACKER MENU =====\n";
        cout << "1. Add a new gas station\n";
        cout << "2. Add connection between stations\n";
        cout << "3. Display network\n";
        cout << "4. Perform DFS traversal\n";
        cout << "5. Perform BFS traversal\n";
        cout << "6. Find cheapest gas in range\n";
        cout << "0. Exit\n";
        cout << "Enter your choice: ";
        
        while (!(cin >> choice)) {
            cout << "Invalid input. Please enter a number: ";
            clearInputBuffer();
        }
        clearInputBuffer();

        switch (choice) {
            case 1: {
                string name;
                double price;
                int id = gasNetwork.getStationCount();

                cout << "Enter station name: ";
                getline(cin, name);
                cout << "Enter gas price: $";
                while (!(cin >> price) || price <= 0) {
                    cout << "Invalid price. Please enter a positive number: ";
                    clearInputBuffer();
                }
                clearInputBuffer();

                gasNetwork.addStation(id, name, price);
                cout << "Station added successfully with ID: " << id << "\n";
                break;
            }

            case 2: {
                if (gasNetwork.getStationCount() < 2) {
                    std::cout << "Need at least 2 stations to create a connection.\n";
                    break;
                }

                int source, dest;
                double distance;

                std::cout << "Available stations:\n";
                for (int i = 0; i < gasNetwork.getStationCount(); i++) {
                    std::cout << i << ": " << gasNetwork.getStationName(i) << "\n";
                }

                std::cout << "Enter source station ID: ";
                while (!(std::cin >> source) || source < 0 || source >= gasNetwork.getStationCount()) {
                    std::cout << "Invalid ID. Please enter a valid station ID: ";
                    clearInputBuffer();
                }
                clearInputBuffer();

                std::cout << "Enter destination station ID: ";
                while (!(std::cin >> dest) || dest < 0 || dest >= gasNetwork.getStationCount()) {
                    std::cout << "Invalid ID. Please enter a valid station ID: ";
                    clearInputBuffer();
                }
                clearInputBuffer();

                std::cout << "Enter distance between stations (miles): ";
                while (!(std::cin >> distance) || distance <= 0) {
                    std::cout << "Invalid distance. Please enter a positive number: ";
                    clearInputBuffer();
                }
                clearInputBuffer();

                gasNetwork.addConnection(source, dest, distance);
                std::cout << "Connection added between " << gasNetwork.getStationName(source) 
                     << " and " << gasNetwork.getStationName(dest) << "\n";
                break;
            }

            case 3:
                gasNetwork.displayNetwork();
                break;

            case 4: {
                if (gasNetwork.getStationCount() == 0) {
                    cout << "No stations to traverse.\n";
                    break;
                }

                int start;
                cout << "Available stations:\n";
                for (int i = 0; i < gasNetwork.getStationCount(); i++) {
                    cout << i << ": " << gasNetwork.getStationName(i) << "\n";
                }

                cout << "Enter starting station ID: ";
                while (!(cin >> start) || start < 0 || start >= gasNetwork.getStationCount()) {
                    cout << "Invalid ID. Please enter a valid station ID: ";
                    clearInputBuffer();
                }
                clearInputBuffer();

                gasNetwork.DFS(start);
                break;
            }

            case 5: {
                if (gasNetwork.getStationCount() == 0) {
                    cout << "No stations to traverse.\n";
                    break;
                }

                int start;
                cout << "Available stations:\n";
                for (int i = 0; i < gasNetwork.getStationCount(); i++) {
                    cout << i << ": " << gasNetwork.getStationName(i) << "\n";
                }

                cout << "Enter starting station ID: ";
                while (!(cin >> start) || start < 0 || start >= gasNetwork.getStationCount()) {
                    cout << "Invalid ID. Please enter a valid station ID: ";
                    clearInputBuffer();
                }
                clearInputBuffer();

                gasNetwork.BFS(start);
                break;
            }

            case 6: {
                if (gasNetwork.getStationCount() == 0) {
                    cout << "No stations in the network.\n";
                    break;
                }

                int start, maxHops;
                cout << "Available stations:\n";
                for (int i = 0; i < gasNetwork.getStationCount(); i++) {
                    cout << i << ": " << gasNetwork.getStationName(i) << "\n";
                }

                cout << "Enter starting station ID: ";
                while (!(cin >> start) || start < 0 || start >= gasNetwork.getStationCount()) {
                    cout << "Invalid ID. Please enter a valid station ID: ";
                    clearInputBuffer();
                }
                clearInputBuffer();

                cout << "Enter maximum number of hops: ";
                while (!(cin >> maxHops) || maxHops < 1) {
                    
                cout << "Invalid number. Please enter a positive integer: ";
                    clearInputBuffer();
                }
                clearInputBuffer();

                gasNetwork.findCheapestInRange(start, maxHops);
                break;
            }

            case 0:
                cout << "Exiting program...\n";
                break;

            default:
                cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 0);

    return 0;
}
