#include <iostream>
#include <string>
#include <climits>
#include <limits> // For numeric_limits

using namespace std;

// ------------------- OPTIMIZED DATA STRUCTURES -------------------
struct GasStation {
    int id;
    string subCity;
    string exactLocation;
    string name;
    double price;
};

struct Edge {
    int toNode;
    double weight;
    Edge* next;

    Edge(int to, double w) : toNode(to), weight(w), next(nullptr) {}
};

struct AdjacencyList {
    Edge* head;
    Edge* tail;

    AdjacencyList() : head(nullptr), tail(nullptr) {}

    void addEdge(int to, double weight) {
        Edge* newEdge = new Edge(to, weight);
        if (!head) {
            head = tail = newEdge;
        } else {
            tail->next = newEdge;
            tail = newEdge;
        }
    }

    ~AdjacencyList() {
        Edge* current = head;
        while (current) {
            Edge* temp = current;
            current = current->next;
            delete temp;
        }
    }
};

struct Graph {
    GasStation* stations;
    AdjacencyList* adjLists;
    int capacity;
    int size;

    Graph(int cap) : capacity(cap), size(0) {
        stations = new GasStation[capacity];
        adjLists = new AdjacencyList[capacity];
    }

    ~Graph() {
        delete[] stations;
        delete[] adjLists;
    }

    void addStation(const GasStation& station) {
        if (size >= capacity) return;
        stations[size] = station;
        size++;
    }

    void addEdge(int from, int to, double weight) {
        if (from >= size || to >= size) return;
        adjLists[from].addEdge(to, weight);
        adjLists[to].addEdge(from, weight);
    }

    GasStation* getStation(int id) {
        for (int i = 0; i < size; i++) {
            if (stations[i].id == id) {
                return &stations[i];
            }
        }
        return nullptr;
    }
};

// ------------------- OPTIMIZED PRIORITY QUEUE -------------------
struct PQNode {
    int stationId;
    double distance;
    PQNode* next;

    PQNode(int id, double dist) : stationId(id), distance(dist), next(nullptr) {}
};

struct PriorityQueue {
    PQNode* head;

    PriorityQueue() : head(nullptr) {}

    ~PriorityQueue() {
        while (head) {
            PQNode* temp = head;
            head = head->next;
            delete temp;
        }
    }

    void push(int id, double dist) {
        PQNode* newNode = new PQNode(id, dist);

        if (!head || dist < head->distance) {
            newNode->next = head;
            head = newNode;
        } else {
            PQNode* current = head;
            while (current->next && current->next->distance <= dist) {
                current = current->next;
            }
            newNode->next = current->next;
            current->next = newNode;
        }
    }

    pair<int, double> pop() {
        if (!head) return {-1, -1};
        pair<int, double> result = {head->stationId, head->distance};
        PQNode* temp = head;
        head = head->next;
        delete temp;
        return result;
    }

    bool empty() {
        return head == nullptr;
    }
};

// ------------------- CORE FUNCTIONS -------------------
double dijkstra(const Graph& graph, int start, int end) {
    double* distances = new double[graph.size];
    bool* visited = new bool[graph.size];

    for (int i = 0; i < graph.size; i++) {
        distances[i] = INT_MAX;
        visited[i] = false;
    }
    distances[start] = 0;

    PriorityQueue pq;
    pq.push(start, 0);

    while (!pq.empty()) {
        auto current = pq.pop();
        int u = current.first;
        double dist = current.second;

        if (u == end) {
            double result = distances[u];
            delete[] distances;
            delete[] visited;
            return result;
        }

        if (visited[u]) continue;
        visited[u] = true;

        Edge* edge = graph.adjLists[u].head;
        while (edge) {
            int v = edge->toNode;
            double alt = dist + edge->weight;

            if (alt < distances[v]) {
                distances[v] = alt;
                pq.push(v, alt);
            }
            edge = edge->next;
        }
    }

    delete[] distances;
    delete[] visited;
    return INT_MAX;
}

void findNearestGasStation(const Graph& graph, int userNodeId) {
    double minDistance = INT_MAX;
    GasStation* nearestStation = nullptr;

    for (int i = 0; i < graph.size; i++) {
        if (i != userNodeId) {
            double distance = dijkstra(graph, userNodeId, i);
            if (distance < minDistance) {
                minDistance = distance;
                nearestStation = &graph.stations[i];
            }
        }
    }

    if (nearestStation) {
        cout << "\nNearest Gas Station: " << nearestStation->name
             << " (" << nearestStation->subCity << ")\n";
        cout << "Distance: " << minDistance << " km\n";
    } else {
        cout << "\nNo gas stations found.\n";
    }
}

// ------------------- USER INTERFACE -------------------
void addGasStation(Graph& graph, int userNodeId, int& nextId) {
    string subCity, exactLocation, name;
    double price;

    cout << "\nEnter Gas Station Name: ";
    cin.ignore();
    getline(cin, name);

    cout << "Enter Sub-City (Woreda): ";
    getline(cin, subCity);

    cout << "Enter Exact Location: ";
    getline(cin, exactLocation);

    cout << "Enter Price: ";
    cin >> price;

    GasStation newStation = {nextId++, subCity, exactLocation, name, price};
    graph.addStation(newStation);
    graph.addEdge(userNodeId, newStation.id, 10.0);

    cout << "\nGas Station '" << name << "' added successfully.\n";
}

int main() {
    const int MAX_STATIONS = 100;
    Graph graph(MAX_STATIONS);
    int nextId = 0;

    // Initialize Gondar gas stations
    graph.addStation({nextId++, "Fasil/Piazza", "Near the main Piazza", "Total Ethiopia", 56.50});
    graph.addStation({nextId++, "Fasil/Arada", "Bahir Dar Highway", "Shell Gondar", 57.75});
    graph.addStation({nextId++, "Arada/Maraki", "Near University", "NOC", 56.60});
    graph.addStation({nextId++, "Azezo", "Debark Road", "Yetebaberut", 58.80});
    graph.addStation({nextId++, "Fasil", "Near Fasil Ghebbi", "Kobil", 58.90});
    graph.addStation({nextId++, "Arada", "Main bus station", "Oilibya", 57.70});
    graph.addStation({nextId++, "Maraki", "Western exit", "Gulf Oil", 58.85});

    // Add connections
    graph.addEdge(0, 1, 5.0);  // Total to Shell
    graph.addEdge(1, 2, 3.0);  // Shell to NOC
    graph.addEdge(2, 3, 10.0); // NOC to Yetebaberut
    graph.addEdge(0, 4, 1.5);  // Total to Kobil
    graph.addEdge(4, 5, 4.0);  // Kobil to Oilibya
    graph.addEdge(5, 6, 7.0);  // Oilibya to Gulf Oil

    // User setup
    string validSubCities[] = {"Fasil", "Arada", "Maraki", "Azezo", "Gondar Town"};
    string userSubCity;

    // Display available sub-cities before anything else
    cout << "Available Sub-Cities (Woredas):\n";
    for (const auto& city : validSubCities) {
        cout << "- " << city << endl;
    }

    // Add user node
    int userNodeId = nextId++;
    graph.addStation({userNodeId, "User Location", "User Location", "User", 0}); // Default location

    // Main menu
    int choice;
    do {
        cout << "\nGondar Gas Station Tracker\n";
        cout << "1. List all stations\n";
        cout << "2. Find nearest station\n";
        cout << "3. Find cheapest gas\n";
        cout << "4. Find path between stations\n";
        cout << "5. Add new station\n";
        cout << "0. Exit\n";
        cout << "Choice: ";
        cin >> choice;

        switch (choice) {
            case 1: {
                cout << "\nList of all Gas Stations:\n";
                for (int i = 0; i < graph.size - 1; i++) {
                    double dist = dijkstra(graph, userNodeId, i);
                    cout << graph.stations[i].id << ": " << graph.stations[i].name
                         << " (" << graph.stations[i].subCity << ") - "
                         << graph.stations[i].price << " ETB, Distance from user: " << dist << " km\n";
                }
                break;
            }
            case 2: {
                cout << "\nEnter your current Sub-City (Woreda): ";
                cin.ignore();
                getline(cin, userSubCity);

                bool validSubCity = false;
                for(const auto& city : validSubCities){
                  if(city == userSubCity){
                    validSubCity = true;
                    break;
                  }
                }
                if(!validSubCity){
                  cout << "Error: Invalid Sub-City entered. Please try again.\n";
                  break;
                }
                graph.stations[userNodeId].subCity = userSubCity;

                for (int i = 0; i < graph.size - 1; i++) {
                    double distance = (graph.stations[i].subCity == userSubCity) ? 2.0 :
                                      (graph.stations[i].subCity == "Azezo" || userSubCity == "Azezo") ? 12.0 : 8.0;
                    graph.addEdge(userNodeId, i, distance);
                }
                findNearestGasStation(graph, userNodeId);
                break;
            }
            case 3: {
                double minPrice = numeric_limits<double>::max();
                GasStation* cheapest = nullptr;
                for (int i = 0; i < graph.size - 1; i++) {
                    if (graph.stations[i].price < minPrice) {
                        minPrice = graph.stations[i].price;
                        cheapest = &graph.stations[i];
                    }
                }
                if (cheapest) {
                    cout << "\nCheapest Gas Station: " << cheapest->name << " at "
                         << cheapest->price << " ETB\n";
                } else {
                  cout << "\nNo Gas stations found.\n";
                }
                break;
            }
            case 4: {
                int start, end;
                cout << "Enter start station ID: ";
                cin >> start;
                cout << "Enter end station ID: ";
                cin >> end;
                if(start < 0 || start >= graph.size || end < 0 || end >= graph.size){
                  cout << "Error: Invalid station IDs.\n";
                  break;
                }
                double dist = dijkstra(graph, start, end);
                if (dist < INT_MAX) {
                    cout << "Distance between station " << start << " and station " << end << " is: " << dist << " km\n";
                } else {
                    cout << "No path exists between station " << start << " and station " << end << ".\n";
                }
                break;
            }
            case 5:
                addGasStation(graph, userNodeId, nextId);
                break;
        }
    } while (choice != 0);

    return 0;
}