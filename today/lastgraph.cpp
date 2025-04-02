#include <iostream>
#include <vector>
using namespace std;

// Simple stack implementation
template <typename T>
class SimpleStack {
private:
    vector<T> data;
public:
    void push(T value) { data.push_back(value); }
    T pop() { 
        T value = data.back();
        data.pop_back();
        return value;
    }
    bool empty() { return data.empty(); }
};

// Simple queue implementation
template <typename T>
class SimpleQueue {
private:
    vector<T> data;
    int front = 0;
public:
    void push(T value) { data.push_back(value); }
    T pop() { 
        T value = data[front++];
        if (front > data.size()/2) {
            data.erase(data.begin(), data.begin()+front);
            front = 0;
        }
        return value;
    }
    bool empty() { return front >= data.size(); }
};

class GasStationGraph {
private:
    struct Station {
        int id;
        string name;
        double gasPrice;
    };

    struct Edge {
        int destination;
        double distance;
    };

    vector<Station> stations;
    vector<vector<Edge>> adjacencyList;

public:
    GasStationGraph() {}

    void addStation(int id, string name, double price) {
        Station newStation = {id, name, price};
        stations.push_back(newStation);
        adjacencyList.push_back(vector<Edge>());
    }

    void addConnection(int source, int destination, double distance) {
        if (source >= 0 && source < stations.size() && 
            destination >= 0 && destination < stations.size()) {
            Edge newEdge = {destination, distance};
            adjacencyList[source].push_back(newEdge);
            
            // For undirected graph
            Edge reverseEdge = {source, distance};
            adjacencyList[destination].push_back(reverseEdge);
        }
    }

    void displayNetwork() {
        if (stations.empty()) {
            cout << "No stations in the network yet.\n";
            return;
        }

        cout << "\n===== CURRENT GAS STATION NETWORK =====\n";
        for (int i = 0; i < stations.size(); i++) {
            cout << "Station " << stations[i].id << ": " << stations[i].name 
                 << " (Price: $" << stations[i].gasPrice << ")\n";
            cout << "  Connected to: ";
            
            if (adjacencyList[i].empty()) {
                cout << "No connections";
            } else {
                for (const Edge& edge : adjacencyList[i]) {
                    cout << stations[edge.destination].name << " (" << edge.distance << " miles), ";
                }
            }
            cout << "\n\n";
        }
    }

    void DFS(int startStation) {
        if (stations.empty()) {
            cout << "No stations to traverse.\n";
            return;
        }

        vector<bool> visited(stations.size(), false);
        SimpleStack<int> stk;
        
        stk.push(startStation);
        visited[startStation] = true;
        
        cout << "\nDFS Traversal starting from " << stations[startStation].name << ":\n";
        
        while (!stk.empty()) {
            int current = stk.pop();
            cout << "Visited: " << stations[current].name << "\n";
            
            for (const Edge& edge : adjacencyList[current]) {
                if (!visited[edge.destination]) {
                    visited[edge.destination] = true;
                    stk.push(edge.destination);
                }
            }
        }
    }

    void BFS(int startStation) {
        if (stations.empty()) {
            cout << "No stations to traverse.\n";
            return;
        }

        vector<bool> visited(stations.size(), false);
        SimpleQueue<int> q;
        
        q.push(startStation);
        visited[startStation] = true;
        
        cout << "\nBFS Traversal starting from " << stations[startStation].name << ":\n";
        
        while (!q.empty()) {
            int current = q.pop();
            cout << "Visited: " << stations[current].name << "\n";
            
            for (const Edge& edge : adjacencyList[current]) {
                if (!visited[edge.destination]) {
                    visited[edge.destination] = true;
                    q.push(edge.destination);
                }
            }
        }
    }

    void findCheapestInRange(int startStation, int maxHops) {
        if (stations.empty()) {
            cout << "No stations in the network.\n";
            return;
        }

        vector<bool> visited(stations.size(), false);
        vector<int> hopCount(stations.size(), 0);
        SimpleQueue<int> q;
        
        q.push(startStation);
        visited[startStation] = true;
        Station cheapest = stations[startStation];
        
        while (!q.empty()) {
            int current = q.pop();
            
            if (stations[current].gasPrice < cheapest.gasPrice && 
                hopCount[current] <= maxHops) {
                cheapest = stations[current];
            }
            
            if (hopCount[current] >= maxHops) continue;
            
            for (const Edge& edge : adjacencyList[current]) {
                if (!visited[edge.destination]) {
                    visited[edge.destination] = true;
                    hopCount[edge.destination] = hopCount[current] + 1;
                    q.push(edge.destination);
                }
            }
        }
        
        cout << "\nCheapest station within " << maxHops << " hops from " 
             << stations[startStation].name << " is " << cheapest.name 
             << " with price $" << cheapest.gasPrice << "\n";
    }

    int getStationCount() {
        return stations.size();
    }

    string getStationName(int id) {
        if (id >= 0 && id < stations.size()) {
            return stations[id].name;
        }
        return "Invalid Station ID";
    }
};

void clearInputBuffer() {
    cin.clear();
    // Simple alternative to numeric_limits<streamsize>::max()
    while (cin.get() != '\n') {
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
                    cout << "Need at least 2 stations to create a connection.\n";
                    break;
                }

                int source, dest;
                double distance;

                cout << "Available stations:\n";
                for (int i = 0; i < gasNetwork.getStationCount(); i++) {
                    cout << i << ": " << gasNetwork.getStationName(i) << "\n";
                }

                cout << "Enter source station ID: ";
                while (!(cin >> source) || source < 0 || source >= gasNetwork.getStationCount()) {
                    cout << "Invalid ID. Please enter a valid station ID: ";
                    clearInputBuffer();
                }
                clearInputBuffer();

                cout << "Enter destination station ID: ";
                while (!(cin >> dest) || dest < 0 || dest >= gasNetwork.getStationCount()) {
                    cout << "Invalid ID. Please enter a valid station ID: ";
                    clearInputBuffer();
                }
                clearInputBuffer();

                cout << "Enter distance between stations (miles): ";
                while (!(cin >> distance) || distance <= 0) {
                    cout << "Invalid distance. Please enter a positive number: ";
                    clearInputBuffer();
                }
                clearInputBuffer();

                gasNetwork.addConnection(source, dest, distance);
                cout << "Connection added between " << gasNetwork.getStationName(source) 
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