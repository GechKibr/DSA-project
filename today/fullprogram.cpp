#include <iostream>
#include <cstring> // for strcpy
using namespace std;

const int MAX_STATIONS = 100;
const int MAX_CONNECTIONS = 100;
const double INF = 1e9;

class IntStack
{
private:
    int *data;
    int capacity;
    int topIndex;

    void resize()
    {
        int newCapacity = capacity * 2;
        int *newData = new int[newCapacity];
        for (int i = 0; i < capacity; i++)
        {
            newData[i] = data[i];
        }
        delete[] data;
        data = newData;
        capacity = newCapacity;
    }

public:
    IntStack() : capacity(10), topIndex(-1)
    {
        data = new int[capacity];
    }

    ~IntStack()
    {
        delete[] data;
    }

    void push(int value)
    {
        if (topIndex == capacity - 1)
        {
            resize();
        }
        data[++topIndex] = value;
    }

    int pop()
    {
        if (empty())
        {
            throw "Stack is empty";
        }
        return data[topIndex--];
    }

    bool empty() const
    {
        return topIndex == -1;
    }
};

class IntQueue
{
private:
    int *data;
    int capacity;
    int head;
    int tail;
    int count;

    void resize()
    {
        int newCapacity = capacity * 2;
        int *newData = new int[newCapacity];

        for (int i = 0; i < count; i++)
        {
            newData[i] = data[(head + i) % capacity];
        }

        delete[] data;
        data = newData;
        capacity = newCapacity;
        head = 0;
        tail = count;
    }

public:
    IntQueue() : capacity(10), head(0), tail(0), count(0)
    {
        data = new int[capacity];
    }

    ~IntQueue()
    {
        delete[] data;
    }

    void push(int value)
    {
        if (count == capacity)
        {
            resize();
        }
        data[tail] = value;
        tail = (tail + 1) % capacity;
        count++;
    }

    int pop()
    {
        if (empty())
        {
            throw "Queue is empty";
        }
        int value = data[head];
        head = (head + 1) % capacity;
        count--;
        return value;
    }

    bool empty() const
    {
        return count == 0;
    }
};

struct Station
{
    int id;
    char name[50];
    double gasPrice;
};

struct Edge
{
    int destination;
    double distance;
};

class GasStationGraph
{
private:
    Station stations[MAX_STATIONS];
    Edge adjacencyList[MAX_STATIONS][MAX_CONNECTIONS];
    int connectionCounts[MAX_STATIONS];
    int stationCount;

    int findMinDistance(const double distances[], const bool visited[])
    {
        double minDist = INF;
        int minIndex = -1;
        for (int i = 0; i < stationCount; i++)
        {
            if (!visited[i] && distances[i] < minDist)
            {
                minDist = distances[i];
                minIndex = i;
            }
        }
        return minIndex;
    }

public:
    GasStationGraph() : stationCount(0)
    {
        for (int i = 0; i < MAX_STATIONS; i++)
        {
            connectionCounts[i] = 0;
        }
    }

    void addStation(const char *name, double price)
    {
        if (stationCount >= MAX_STATIONS)
        {
            cout << "Error: Maximum number of gas stations (" << MAX_STATIONS << ") reached. Cannot add more.\n";
            return;
        }
        stations[stationCount].id = stationCount;
        strcpy(stations[stationCount].name, name);
        stations[stationCount].gasPrice = price;
        stationCount++;
        cout << "Gas station '" << name << "' added successfully with ID " << stationCount - 1 << " and price $" << price << ".\n";
    }

    void addConnection(int source, int destination, double distance)
    {
        if (source < 0 || source >= stationCount || destination < 0 || destination >= stationCount)
        {
            cout << "Error: Invalid station IDs. Please ensure both source and destination IDs are valid.\n";
            return;
        }
        if (connectionCounts[source] >= MAX_CONNECTIONS || connectionCounts[destination] >= MAX_CONNECTIONS)
        {
            cout << "Error: Maximum connections (" << MAX_CONNECTIONS << ") reached for one or both stations. Cannot add more connections.\n";
            return;
        }

        // Add connection from source to destination
        adjacencyList[source][connectionCounts[source]].destination = destination;
        adjacencyList[source][connectionCounts[source]].distance = distance;
        connectionCounts[source]++;

        // Add reverse connection for undirected graph
        adjacencyList[destination][connectionCounts[destination]].destination = source;
        adjacencyList[destination][connectionCounts[destination]].distance = distance;
        connectionCounts[destination]++;

        cout << "Connection added successfully between '" << stations[source].name << "' and '" << stations[destination].name << "' (" << distance << " miles).\n";
    }

    int findNearestStation(int stationId)
    {
        if (stationId < 0 || stationId >= stationCount || connectionCounts[stationId] == 0)
        {
            return -1;
        }

        int nearest = adjacencyList[stationId][0].destination;
        double minDist = adjacencyList[stationId][0].distance;

        for (int i = 1; i < connectionCounts[stationId]; i++)
        {
            if (adjacencyList[stationId][i].distance < minDist)
            {
                minDist = adjacencyList[stationId][i].distance;
                nearest = adjacencyList[stationId][i].destination;
            }
        }
        return nearest;
    }

    void findShortestPath(int start, int end)
    {
        if (start < 0 || start >= stationCount || end < 0 || end >= stationCount)
        {
            cout << "Error: Invalid station IDs. Please ensure both start and end IDs are valid.\n";
            return;
        }

        double distances[MAX_STATIONS];
        bool visited[MAX_STATIONS] = {false};
        int previous[MAX_STATIONS];

        for (int i = 0; i < stationCount; i++)
        {
            distances[i] = INF;
            previous[i] = -1;
        }
        distances[start] = 0;

        for (int count = 0; count < stationCount - 1; count++)
        {
            int u = findMinDistance(distances, visited);
            if (u == -1)
                break;

            visited[u] = true;

            for (int i = 0; i < connectionCounts[u]; i++)
            {
                int v = adjacencyList[u][i].destination;
                double dist = adjacencyList[u][i].distance;

                if (!visited[v] && distances[u] + dist < distances[v])
                {
                    distances[v] = distances[u] + dist;
                    previous[v] = u;
                }
            }
        }

        if (distances[end] == INF)
        {
            cout << "Error: No path exists between '" << stations[start].name << "' and '" << stations[end].name << "'.\n";
            return;
        }

        cout << "Shortest path from '" << stations[start].name << "' to '" << stations[end].name << "' is " << distances[end] << " miles:\n";

        // Reconstruct path
        IntStack path;
        for (int at = end; at != -1; at = previous[at])
        {
            path.push(at);
        }

        while (!path.empty())
        {
            cout << stations[path.pop()].name;
            if (!path.empty())
                cout << " -> ";
        }
        cout << "\n";
    }

    int findCheapestStation()
    {
        if (stationCount == 0)
            return -1;

        int cheapest = 0;
        for (int i = 1; i < stationCount; i++)
        {
            if (stations[i].gasPrice < stations[cheapest].gasPrice)
            {
                cheapest = i;
            }
        }
        return cheapest;
    }

    void updateFuelPrice(int stationId, double newPrice)
    {
        if (stationId < 0 || stationId >= stationCount)
        {
            cout << "Error: Invalid station ID. Please provide a valid station ID.\n";
            return;
        }
        stations[stationId].gasPrice = newPrice;
        cout << "Fuel price updated for '" << stations[stationId].name << "' to $" << newPrice << ".\n";
    }

    void deleteStation(int stationId)
    {
        if (stationId < 0 || stationId >= stationCount)
        {
            cout << "Error: Invalid station ID. Please provide a valid station ID.\n";
            return;
        }

        // Remove all connections to this station
        for (int i = 0; i < stationCount; i++)
        {
            if (i == stationId)
                continue;

            for (int j = 0; j < connectionCounts[i]; j++)
            {
                if (adjacencyList[i][j].destination == stationId)
                {
                    // Shift remaining connections left
                    for (int k = j; k < connectionCounts[i] - 1; k++)
                    {
                        adjacencyList[i][k] = adjacencyList[i][k + 1];
                    }
                    connectionCounts[i]--;
                    j--; // Check current position again
                }
            }
        }

        // Shift stations after the deleted one
        for (int i = stationId; i < stationCount - 1; i++)
        {
            stations[i] = stations[i + 1];
            connectionCounts[i] = connectionCounts[i + 1];

            // Copy connections
            for (int j = 0; j < connectionCounts[i]; j++)
            {
                adjacencyList[i][j] = adjacencyList[i + 1][j];
            }

            // Update IDs in connections
            for (int j = 0; j < stationCount; j++)
            {
                for (int k = 0; k < connectionCounts[j]; k++)
                {
                    if (adjacencyList[j][k].destination == i + 1)
                    {
                        adjacencyList[j][k].destination = i;
                    }
                }
            }
        }

        stationCount--;
        cout << "Gas station '" << stations[stationId].name << "' (ID " << stationId << ") deleted successfully.\n";
    }

    void deleteConnection(int source, int destination)
    {
        if (source < 0 || source >= stationCount || destination < 0 || destination >= stationCount)
        {
            cout << "Error: Invalid station IDs. Please provide valid station IDs.\n";
            return;
        }

        bool found = false;

        // Remove source->destination connection
        for (int i = 0; i < connectionCounts[source]; i++)
        {
            if (adjacencyList[source][i].destination == destination)
            {
                // Shift remaining connections left
                for (int j = i; j < connectionCounts[source] - 1; j++)
                {
                    adjacencyList[source][j] = adjacencyList[source][j + 1];
                }
                connectionCounts[source]--;
                found = true;
                break;
            }
        }

        // Remove destination->source connection (undirected graph)
        for (int i = 0; i < connectionCounts[destination]; i++)
        {
            if (adjacencyList[destination][i].destination == source)
            {
                // Shift remaining connections left
                for (int j = i; j < connectionCounts[destination] - 1; j++)
                {
                    adjacencyList[destination][j] = adjacencyList[destination][j + 1];
                }
                connectionCounts[destination]--;
                found = true;
                break;
            }
        }

        if (found)
        {
            cout << "Connection between '" << stations[source].name << "' and '" << stations[destination].name << "' deleted.\n";
        }
        else
        {
            cout << "Error: No connection found between '" << stations[source].name << "' and '" << stations[destination].name << "'.\n";
        }
    }

    const char *getStationName(int id) const
    {
        if (id >= 0 && id < stationCount)
        {
            return stations[id].name;
        }
        return "Invalid Station";
    }

    double getStationPrice(int id) const
    {
        if (id >= 0 && id < stationCount)
        {
            return stations[id].gasPrice;
        }
        return -1.0;
    }

    void displayAllStations() const
    {
        if (stationCount == 0)
        {
            cout << "No stations in the network.\n";
            return;
        }

        cout << "\n===== ALL GAS STATIONS =====\n";
        for (int i = 0; i < stationCount; i++)
        {
            cout << "ID: " << stations[i].id << " | Name: " << stations[i].name
                 << " | Price: $" << stations[i].gasPrice << "\n";
        }
    }

    void displayAllConnections()
    {
        bool anyConnections = false;
        for (int i = 0; i < stationCount; i++)
        {
            if (connectionCounts[i] > 0)
            {
                anyConnections = true;
                break;
            }
        }

        if (!anyConnections)
        {
            cout << "No connections in the network.\n";
            return;
        }

        cout << "\n===== ALL CONNECTIONS =====\n";
        for (int i = 0; i < stationCount; i++)
        {
            if (connectionCounts[i] > 0)
            {
                cout << stations[i].name << " (" << i << ") connects to:\n";
                for (int j = 0; j < connectionCounts[i]; j++)
                {
                    cout << "  " << stations[adjacencyList[i][j].destination].name
                         << " (" << adjacencyList[i][j].distance << " miles)\n";
                }
            }
        }
    }

    void saveData()
    {
        // Implementation would save to file
        cout << "Data would be saved in a real implementation.\n";
    }

    void loadData()
    {
        // Implementation would load from file
        cout << "Data would be loaded in a real implementation.\n";
    }

    void displayNetwork()
    {
        if (stationCount == 0)
        {
            cout << "No stations in the network.\n";
            return;
        }

        cout << "\n===== CURRENT GAS STATION NETWORK =====\n";
        for (int i = 0; i < stationCount; i++)
        {
            cout << "Station " << stations[i].id << ": " << stations[i].name
                 << " (Price: $" << stations[i].gasPrice << ")\n";
            cout << "  Connected to: ";

            if (connectionCounts[i] == 0)
            {
                cout << "No connections";
            }
            else
            {
                for (int j = 0; j < connectionCounts[i]; j++)
                {
                    cout << stations[adjacencyList[i][j].destination].name << " ("
                         << adjacencyList[i][j].distance << " miles)";
                    if (j < connectionCounts[i] - 1)
                        cout << ", ";
                }
            }
            cout << "\n\n";
        }
    }

    void DFS(int startStation)
    {
        if (startStation < 0 || startStation >= stationCount)
        {
            cout << "Error: Invalid station ID.\n";
            return;
        }

        bool visited[MAX_STATIONS] = {false};
        IntStack stk;

        stk.push(startStation);
        visited[startStation] = true;

        cout << "\nDFS Traversal starting from " << stations[startStation].name << ":\n";

        while (!stk.empty())
        {
            int current = stk.pop();
            cout << "Visited: " << stations[current].name << "\n";

            for (int i = 0; i < connectionCounts[current]; i++)
            {
                int neighbor = adjacencyList[current][i].destination;
                if (!visited[neighbor])
                {
                    visited[neighbor] = true;
                    stk.push(neighbor);
                }
            }
        }
    }

    void BFS(int startStation)
    {
        if (startStation < 0 || startStation >= stationCount)
        {
            cout << "Error: Invalid station ID.\n";
            return;
        }

        bool visited[MAX_STATIONS] = {false};
        IntQueue q;

        q.push(startStation);
        visited[startStation] = true;

        cout << "\nBFS Traversal starting from " << stations[startStation].name << ":\n";

        while (!q.empty())
        {
            int current = q.pop();
            cout << "Visited: " << stations[current].name << "\n";

            for (int i = 0; i < connectionCounts[current]; i++)
            {
                int neighbor = adjacencyList[current][i].destination;
                if (!visited[neighbor])
                {
                    visited[neighbor] = true;
                    q.push(neighbor);
                }
            }
        }
    }

    void findCheapestInRange(int startStation, int maxHops)
    {
        if (startStation < 0 || startStation >= stationCount)
        {
            cout << "Error: Invalid station ID.\n";
            return;
        }

        bool visited[MAX_STATIONS] = {false};
        int hopCount[MAX_STATIONS] = {0};
        IntQueue q;

        q.push(startStation);
        visited[startStation] = true;
        int cheapestId = startStation;

        while (!q.empty())
        {
            int current = q.pop();

            if (stations[current].gasPrice < stations[cheapestId].gasPrice &&
                hopCount[current] <= maxHops)
            {
                cheapestId = current;
            }

            if (hopCount[current] >= maxHops)
                continue;

            for (int i = 0; i < connectionCounts[current]; i++)
            {
                int neighbor = adjacencyList[current][i].destination;
                if (!visited[neighbor])
                {
                    visited[neighbor] = true;
                    hopCount[neighbor] = hopCount[current] + 1;
                    q.push(neighbor);
                }
            }
        }

        cout << "\nCheapest station within " << maxHops << " hops from "
             << stations[startStation].name << " is " << stations[cheapestId].name
             << " with price $" << stations[cheapestId].gasPrice << "\n";
    }
};

void clearInputBuffer()
{
    cin.clear();
    while (cin.get() != '\n')
    {
        continue;
    }
}

int main()
{
    GasStationGraph gasNetwork;
    int choice;

    do
    {
        cout << "\n===== GAS STATION TRACKER MENU =====\n";
        cout << "1. Add a new gas station\n";
        cout << "2. Add connection between stations\n";
        cout << "3. Find nearest gas station\n";
        cout << "4. Find shortest path between stations\n";
        cout << "5. Find cheapest gas station\n";
        cout << "6. Update fuel price\n";
        cout << "7. Delete gas station\n";
        cout << "8. Delete connection\n";
        cout << "9. Display all gas stations\n";
        cout << "10. Display all connections\n";
        cout << "11. Save data\n";
        cout << "12. Load data\n";
        cout << "13. Display network\n";
        cout << "14. Perform DFS traversal\n";
        cout << "15. Perform BFS traversal\n";
        cout << "16. Find cheapest gas in range\n";
        cout << "0. Exit\n";
        cout << "Enter your choice: ";

        while (!(cin >> choice))
        {
            cout << "Invalid input. Please enter a number: ";
            clearInputBuffer();
        }
        clearInputBuffer();

        switch (choice)
        {
        case 1:
        {
            char name[50];
            double price;

            cout << "Enter station name: ";
            cin.getline(name, 50);
            cout << "Enter gas price: $";
            while (!(cin >> price) || price <= 0)
            {
                cout << "Invalid price. Please enter a positive number: ";
                clearInputBuffer();
            }
            clearInputBuffer();

            gasNetwork.addStation(name, price);
            break;
        }

        case 2:
        {
            int source, dest;
            double distance;

            gasNetwork.displayAllStations();

            cout << "Enter source station ID: ";
            while (!(cin >> source))
            {
                cout << "Invalid ID. Please enter a valid station ID: ";
                clearInputBuffer();
            }
            clearInputBuffer();

            cout << "Enter destination station ID: ";
            while (!(cin >> dest))
            {
                cout << "Invalid ID. Please enter a valid station ID: ";
                clearInputBuffer();
            }
            clearInputBuffer();

            cout << "Enter distance between stations (miles): ";
            while (!(cin >> distance) || distance <= 0)
            {
                cout << "Invalid distance. Please enter a positive number: ";
                clearInputBuffer();
            }
            clearInputBuffer();

            gasNetwork.addConnection(source, dest, distance);
            break;
        }

        case 3:
        {
            int stationId;
            gasNetwork.displayAllStations();

            cout << "Enter station ID to find nearest: ";
            while (!(cin >> stationId))
            {
                cout << "Invalid ID. Please enter a valid station ID: ";
                clearInputBuffer();
            }
            clearInputBuffer();

            int nearest = gasNetwork.findNearestStation(stationId);
            if (nearest == -1)
            {
                cout << "No connected stations found.\n";
            }
            else
            {
                cout << "Nearest station is " << nearest << ": " << gasNetwork.getStationName(nearest) << "\n";
            }
            break;
        }

        case 4:
        {
            int start, end;
            gasNetwork.displayAllStations();

            cout << "Enter starting station ID: ";
            while (!(cin >> start))
            {
                cout << "Invalid ID. Please enter a valid station ID: ";
                clearInputBuffer();
            }
            clearInputBuffer();

            cout << "Enter destination station ID: ";
            while (!(cin >> end))
            {
                cout << "Invalid ID. Please enter a valid station ID: ";
                clearInputBuffer();
            }
            clearInputBuffer();

            gasNetwork.findShortestPath(start, end);
            break;
        }

        case 5:
        {
            int cheapest = gasNetwork.findCheapestStation();
            if (cheapest == -1)
            {
                cout << "No stations in the network.\n";
            }
            else
            {
                cout << "Cheapest gas station is ID " << cheapest << ": "
                     << gasNetwork.getStationName(cheapest) << " ($"
                     << gasNetwork.getStationPrice(cheapest) << ")\n";
            }
            break;
        }

        case 6:
        {
            int stationId;
            double newPrice;

            gasNetwork.displayAllStations();

            cout << "Enter station ID to update: ";
            while (!(cin >> stationId))
            {
                cout << "Invalid ID. Please enter a valid station ID: ";
                clearInputBuffer();
            }
            clearInputBuffer();

            cout << "Enter new gas price: $";
            while (!(cin >> newPrice) || newPrice <= 0)
            {
                cout << "Invalid price. Please enter a positive number: ";
                clearInputBuffer();
            }
            clearInputBuffer();

            gasNetwork.updateFuelPrice(stationId, newPrice);
            break;
        }

        case 7:
        {
            int stationId;
            gasNetwork.displayAllStations();

            cout << "Enter station ID to delete: ";
            while (!(cin >> stationId))
            {
                cout << "Invalid ID. Please enter a valid station ID: ";
                clearInputBuffer();
            }
            clearInputBuffer();

            gasNetwork.deleteStation(stationId);
            break;
        }

        case 8:
        {
            int source, dest;
            gasNetwork.displayAllConnections();

            cout << "Enter source station ID: ";
            while (!(cin >> source))
            {
                cout << "Invalid ID. Please enter a valid station ID: ";
                clearInputBuffer();
            }
            clearInputBuffer();

            cout << "Enter destination station ID: ";
            while (!(cin >> dest))
            {
                cout << "Invalid ID. Please enter a valid station ID: ";
                clearInputBuffer();
            }
            clearInputBuffer();

            gasNetwork.deleteConnection(source, dest);
            break;
        }

        case 9:
            gasNetwork.displayAllStations();
            break;

        case 10:
            gasNetwork.displayAllConnections();
            break;

        case 11:
            gasNetwork.saveData();
            break;

        case 12:
            gasNetwork.loadData();
            break;

        case 13:
            gasNetwork.displayNetwork();
            break;

        case 14:
        {
            int startStation;
            gasNetwork.displayAllStations();

            cout << "Enter starting station ID: ";
            while (!(cin >> startStation))
            {
                cout << "Invalid ID. Please enter a valid station ID: ";
                clearInputBuffer();
            }
            clearInputBuffer();

            gasNetwork.DFS(startStation);
            break;
        }

        case 15:
        {
            int startStation;
            gasNetwork.displayAllStations();

            cout << "Enter starting station ID: ";
            while (!(cin >> startStation))
            {
                cout << "Invalid ID. Please enter a valid station ID: ";
                clearInputBuffer();
            }
            clearInputBuffer();

            gasNetwork.BFS(startStation);
            break;
        }

        case 16:
        {
            int startStation, maxHops;
            gasNetwork.displayAllStations();

            cout << "Enter starting station ID: ";
            while (!(cin >> startStation))
            {
                cout << "Invalid ID. Please enter a valid station ID: ";
                clearInputBuffer();
            }
            clearInputBuffer();

            cout << "Enter maximum number of hops: ";
            while (!(cin >> maxHops) || maxHops < 1)
            {
                cout << "Invalid number. Please enter a positive integer: ";
                clearInputBuffer();
            }
            clearInputBuffer();

            gasNetwork.findCheapestInRange(startStation, maxHops);
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