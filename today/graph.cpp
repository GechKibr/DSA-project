#include <iostream>
#include <cstring> // For strcpy, strcmp
#include <limits>  // Required for numeric_limits if using INF that way, but 1e9 is fine.
#include <algorithm> // For std::swap (optional, manual swap is used)

using namespace std;

const int MAX_STATIONS = 20;
const double INF = 1e9; // Use a large number for infinity

struct GasStation {
    int id;
    char subCity[20];
    char exactLocation[100];
    char name[50];
    double price;
};

struct Edge {
    int toNode;
    double weight;
};

// Dijkstra's algorithm (from Code B - slightly cleaner with INF)
double dijkstra(Edge adjLists[][MAX_STATIONS], int start, int end, int numStations, int path[], int& pathLength) {
    double distances[MAX_STATIONS];
    int previous[MAX_STATIONS];
    bool visited[MAX_STATIONS];

    for (int i = 0; i < numStations; i++) {
        distances[i] = INF;
        previous[i] = -1;
        visited[i] = false;
    }

    distances[start] = 0;

    for (int count = 0; count < numStations; count++) { // Iterate potentially numStations times
        double minDistance = INF;
        int minIndex = -1;

        // Find the unvisited node with the smallest distance
        for (int v = 0; v < numStations; v++) {
            if (!visited[v] && distances[v] <= minDistance) {
                minDistance = distances[v];
                minIndex = v;
            }
        }

        // If no reachable node is found, or we reached the end node efficiently
        if (minIndex == -1 || minIndex == end && distances[end] != INF) break;

        int current = minIndex;
        visited[current] = true;

        // If the current node is unreachable, skip its neighbors
        if (distances[current] == INF) continue;

        // Update distances of adjacent nodes
        for (int v = 0; v < numStations; v++) {
             // Check if there is an edge (weight > 0 implicitly means connected in this setup)
            if (adjLists[current][v].weight > 0 && !visited[v]) {
                 // Check if a shorter path is found
                 if (distances[current] + adjLists[current][v].weight < distances[v]) {
                    distances[v] = distances[current] + adjLists[current][v].weight;
                    previous[v] = current;
                 }
            }
        }
    }

    // Reconstruct the path
    pathLength = 0;
    int temp = end;
    // Check if a path was actually found
    if (distances[end] == INF) {
        return INF; // Return infinity if no path exists
    }

    while (temp != -1) {
        // Ensure we don't exceed array bounds if path is unexpectedly long
        if (pathLength < MAX_STATIONS) {
           path[pathLength++] = temp;
        } else {
            // Handle error: Path is too long for the buffer
             cerr << "Error: Path reconstruction exceeded MAX_STATIONS." << endl;
             return INF; // Indicate error or impossibly long path
        }
        // Prevent infinite loop if previous array isn't set correctly (e.g., start node)
        if(temp == start) break;
        temp = previous[temp];
    }

     // Add the start node if the loop broke because temp == start and start wasn't the end
     if (pathLength > 0 && path[pathLength-1] != start && start != end) {
        if (pathLength < MAX_STATIONS) {
             path[pathLength++] = start;
        } else {
            cerr << "Error: Path reconstruction exceeded MAX_STATIONS when adding start node." << endl;
            return INF;
        }
     }


    // Reverse the path to get it from start to end
    for (int i = 0; i < pathLength / 2; i++) {
        // Use std::swap or manual swap
        int tempPath = path[i];
        path[i] = path[pathLength - 1 - i];
        path[pathLength - 1 - i] = tempPath;
       // swap(path[i], path[pathLength - 1 - i]); // Alternative using <algorithm>
    }

    return distances[end];
}

// Function to find the nearest gas station (modified to use Code A's dynamic distance logic)
void findNearestGasStation(GasStation stations[], Edge adjLists[][MAX_STATIONS], int userNodeId, int numStations) {
    double minDistance = INF;
    int nearestStationId = -1; // Store ID instead of copying struct
    int path[MAX_STATIONS];
    int pathLength = 0;
    bool found = false;

    // Iterate through all actual gas stations (exclude the abstract userNodeId)
    for (int i = 0; i < numStations; i++) {
        // Don't calculate distance from user node to itself
        if (i != userNodeId) {
            // Use Dijkstra from the abstract userNodeId to station 'i'
            // The adjLists should have been updated just before calling this function (in case 2)
            // based on the user's selected sub-city.
            double distance = dijkstra(adjLists, userNodeId, i, numStations, path, pathLength);

            // Check if this station is reachable and closer than the current minimum
            if (distance < minDistance) {
                minDistance = distance;
                nearestStationId = i;
                found = true;
            }
        }
    }

    if (found) {
        cout << "\nNearest Gas Station: " << stations[nearestStationId].name
             << " (" << stations[nearestStationId].subCity << ")\n";
        cout << "Distance: " << minDistance << " km\n";
        // Optionally, print the path found by the last successful Dijkstra call
        // Note: The 'path' array currently holds the path to the *last station checked*,
        // not necessarily the path to the *nearest* one. You'd need to store the path
        // when minDistance is updated if you want to display the correct path.
        // For simplicity, we'll omit the path display here.

    } else {
        cout << "\nNo reachable gas stations found from your location.\n";
    }
}


// Function to add a gas station (from Code B)
void addGasStation(GasStation stations[], Edge adjLists[][MAX_STATIONS],
                   int userNodeId, int& nextId, char validSubCities[][20], int& numSubCities) {
    // Check if we can add another station
    if (nextId >= MAX_STATIONS) {
        cout << "Error: Maximum number of stations reached (" << MAX_STATIONS << "). Cannot add more." << endl;
        return;
    }

    char subCity[20], exactLocation[100], name[50];
    double price;

    cout << "\nEnter Gas Station Name: ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the input buffer before getline
    cin.getline(name, 50);

    cout << "Enter Sub-City (Woreda): ";
    cin.getline(subCity, 20);

    cout << "Enter Exact Location: ";
    cin.getline(exactLocation, 100);

    cout << "Enter Price (ETB): ";
    while (!(cin >> price) || price < 0) { // Basic input validation
         cout << "Invalid price. Please enter a non-negative number: ";
         cin.clear();
         cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    // Assign data to the new station slot
    stations[nextId].id = nextId;
    strcpy(stations[nextId].subCity, subCity);
    strcpy(stations[nextId].exactLocation, exactLocation);
    strcpy(stations[nextId].name, name);
    stations[nextId].price = price;

    cout << "\nEnter distances from new station '" << name << "' to existing stations:\n";
    // Initialize connections for the new station to 0 or INF
    for (int j = 0; j < MAX_STATIONS; ++j) {
         adjLists[nextId][j] = {j, 0.0}; // Or INF if 0 means no direct connection
         adjLists[j][nextId] = {nextId, 0.0};
    }


    // Connect to all existing stations (including the abstract user node if desired, but
    // Code B's approach connects to real stations + asks for distance to main city implicitly
    // via the userNodeId connection - let's stick to connecting to actual stations).
    for (int i = 0; i < nextId; i++) { // Iterate up to the ID *before* the new one
         // Don't ask for distance to itself or the abstract user node here?
         // Let's ask for distances to *all* pre-existing nodes including the userNodeId
         // as Code B's userNodeId (7) is "Gondar Town", a real reference.

         double distance = -1.0; // Initialize to invalid distance
         cout << "Distance to " << stations[i].name << " (" << stations[i].subCity << "): ";
         while (!(cin >> distance) || distance < 0) { // Basic validation
             cout << "Invalid distance. Please enter a non-negative number: ";
             cin.clear();
             cin.ignore(numeric_limits<streamsize>::max(), '\n');
         }
         // Add edge in both directions
         adjLists[nextId][i] = {i, distance};
         adjLists[i][nextId] = {nextId, distance};
    }

     // Add the sub-city to the list if it's new
     bool subCityExists = false;
     for (int i = 0; i < numSubCities; i++) {
         if (strcmp(validSubCities[i], subCity) == 0) {
             subCityExists = true;
             break;
         }
     }
     // Check if there's space for a new sub-city name
     if (!subCityExists) {
         if (numSubCities < MAX_STATIONS) { // Assuming max sub-cities won't exceed max stations
             strcpy(validSubCities[numSubCities], subCity);
             numSubCities++;
         } else {
             cout << "Warning: Could not add new sub-city '" << subCity << "' to the list (list full)." << endl;
         }
     }


    cout << "\nGas Station '" << name << "' added successfully with ID " << nextId << ".\n";
    nextId++; // Increment the ID for the *next* potential station
}


int main() {
    // Initial Gas Station Data (Using Code A's initial list including User Location)
    GasStation stations[MAX_STATIONS] = {
        {0, "Fasil", "Near the main Piazza", "Total Ethiopia", 56.50},
        {1, "Arada", "Bahir Dar Highway", "Shell Gondar", 57.75},
        {2, "Maraki", "Near University", "NOC", 56.60},
        {3, "Azezo", "Debark Road", "Yetebaberut", 58.80},
        {4, "Fasil", "Near Fasil Ghebbi", "Kobil", 58.90},
        {5, "Arada", "Main bus station", "Oilibya", 57.70},
        {6, "Maraki", "Western exit", "Gulf Oil", 58.85},
        // Abstract User Location Node (from Code A)
        {7, "User Location", "User Location", "User", 0} // Price is irrelevant
    };
    int nextId = 8; // The ID for the *next* station to be added
    int numStations = nextId; // Current number of nodes in the graph (including user node)
    int userNodeId = 7; // The ID of the abstract user location node

    // Adjacency List (Matrix representation here) - Initialize
    Edge adjLists[MAX_STATIONS][MAX_STATIONS];
    for(int i = 0; i < MAX_STATIONS; i++){
        for(int j = 0; j < MAX_STATIONS; j++){
            adjLists[i][j].toNode = j; // Point to the destination node
            adjLists[i][j].weight = (i == j) ? 0.0 : 0.0; // Initialize weights (0 or INF could be used for no direct path)
                                                         // Using 0 for now, implies check > 0 in Dijkstra
        }
    }

    // Initial Connections between actual stations (from Code A/B - they were similar)
    adjLists[0][1] = {1, 3.5}; adjLists[1][0] = {0, 3.5};
    adjLists[1][2] = {2, 2.8}; adjLists[2][1] = {1, 2.8};
    adjLists[2][3] = {3, 8.2}; adjLists[3][2] = {2, 8.2};
    adjLists[0][4] = {4, 1.5}; adjLists[4][0] = {0, 1.5};
    adjLists[4][5] = {5, 3.0}; adjLists[5][4] = {4, 3.0};
    adjLists[5][6] = {6, 7.0}; adjLists[6][5] = {5, 7.0};
    adjLists[3][6] = {6, 7.0}; adjLists[6][3] = {3, 7.0};
    // Note: No initial connections to/from userNodeId (7) are set here.
    // They will be dynamically set in Case 2 based on user's sub-city selection.

    // List of known Sub-Cities
    char validSubCities[][20] = {"Fasil", "Arada", "Maraki", "Azezo", "Gondar Town"};
    int numSubCities = 5;

    int choice;
    do {
        // Update numStations before each menu display, in case a station was added
        numStations = nextId;

        cout << "\n--- Gondar Gas Station Tracker ---";
        cout << "\n1. List all stations (showing current distance from your set location)";
        cout << "\n2. Find nearest station (Set your location first)";
        cout << "\n3. Find cheapest gas station";
        cout << "\n4. Find shortest path between two stations";
        cout << "\n5. Add new gas station";
        cout << "\n0. Exit";
        cout << "\nChoice: ";

        // Input validation for choice
        while (!(cin >> choice)) {
             cout << "Invalid input. Please enter a number: ";
             cin.clear();
             cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        switch (choice) {
            case 1: {
                cout << "\n--- List of all Gas Stations --- \n";
                if (numStations <= 1) { // Only user node exists or no stations
                     cout << "No gas stations available to list." << endl;
                     break;
                }
                cout << "Distances are calculated from your last set location: '"
                     << stations[userNodeId].subCity << "'" << endl;

                for (int i = 0; i < numStations; i++) {
                     // Don't list the abstract user node itself in the primary list
                     if (i == userNodeId) continue;

                     int path[MAX_STATIONS];
                     int pathLength = 0;
                     // Calculate distance from the abstract user node
                     double dist = dijkstra(adjLists, userNodeId, i, numStations, path, pathLength);

                     cout << stations[i].id << ": " << stations[i].name
                          << " (" << stations[i].subCity << ") - Price: " << stations[i].price << " ETB";
                     if (dist < INF) {
                         cout << ", Distance: " << dist << " km\n";
                     } else {
                         cout << ", Distance: Not Reachable\n";
                     }
                }
                break;
            }
            case 2: { // Find nearest station (Incorporates Code A's dynamic distance logic)
                int subCityChoice;
                cout << "\nEnter your current Sub-City (Woreda) to set your location:\n";
                for(int i = 0; i < numSubCities; i++){
                    cout << i + 1 << ". " << validSubCities[i] << endl;
                }
                cout << "Selection: ";

                // Input validation for sub-city choice
                while (!(cin >> subCityChoice) || subCityChoice < 1 || subCityChoice > numSubCities) {
                     cout << "Invalid selection. Please enter a number between 1 and " << numSubCities << ": ";
                     cin.clear();
                     cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }

                // Get the chosen sub-city name
                char currentUserSubCity[20];
                strcpy(currentUserSubCity, validSubCities[subCityChoice - 1]);
                // Update the user node's subCity for reference
                strcpy(stations[userNodeId].subCity, currentUserSubCity);
                cout << "Setting your location reference to: " << currentUserSubCity << endl;


                // --- Crucial Part: Update distances from userNodeId based on sub-city ---
                // (Logic adapted directly from Code A's case 2)
                cout << "Updating temporary distances from your location..." << endl;
                for (int i = 0; i < numStations; i++) {
                    if (i == userNodeId) continue; // Skip connection to self

                    char stationSubCity[20];
                    strcpy(stationSubCity, stations[i].subCity);
                    double distance = INF; // Default to not directly connected based on this logic

                    // Nested if-else structure from Code A to set approximate distances
                    if (strcmp(currentUserSubCity, "Arada") == 0) {
                        if (strcmp(stationSubCity, "Arada") == 0) distance = (strcmp(stations[i].name, "Shell Gondar") == 0) ? 0.8 : 0.5; // Example: closer to Oilibya
                        else if (strcmp(stationSubCity, "Fasil") == 0) distance = 3.5;
                        else if (strcmp(stationSubCity, "Maraki") == 0) distance = 2.8;
                        else if (strcmp(stationSubCity, "Azezo") == 0) distance = 12.0;
                        else if (strcmp(stationSubCity, "Gondar Town") == 0) distance = 0.8; // Assuming Gondar Town is close to Arada
                    }
                    else if (strcmp(currentUserSubCity, "Fasil") == 0) {
                        if (strcmp(stationSubCity, "Arada") == 0) distance = 3.5;
                        else if (strcmp(stationSubCity, "Fasil") == 0) distance = (strcmp(stations[i].name, "Total Ethiopia") == 0) ? 0.5 : 1.2; // Example: Closer to Total
                        else if (strcmp(stationSubCity, "Maraki") == 0) distance = 6.0;
                        else if (strcmp(stationSubCity, "Azezo") == 0) distance = 15.0;
                        else if (strcmp(stationSubCity, "Gondar Town") == 0) distance = 3.2;
                    }
                    else if (strcmp(currentUserSubCity, "Maraki") == 0) {
                        if (strcmp(stationSubCity, "Arada") == 0) distance = 2.8;
                        else if (strcmp(stationSubCity, "Fasil") == 0) distance = 6.0;
                        else if (strcmp(stationSubCity, "Maraki") == 0) distance = (strcmp(stations[i].name, "NOC") == 0) ? 0.7 : 2.5; // Example: Closer to NOC
                        else if (strcmp(stationSubCity, "Azezo") == 0) distance = 8.2;
                        else if (strcmp(stationSubCity, "Gondar Town") == 0) distance = 1.8;
                    }
                     else if (strcmp(currentUserSubCity, "Azezo") == 0) {
                         if (strcmp(stationSubCity, "Arada") == 0) distance = 12.0;
                         else if (strcmp(stationSubCity, "Fasil") == 0) distance = 15.0;
                         else if (strcmp(stationSubCity, "Maraki") == 0) distance = 8.2;
                         else if (strcmp(stationSubCity, "Azezo") == 0) distance = 0.5; // Close if in Azezo
                         else if (strcmp(stationSubCity, "Gondar Town") == 0) distance = 11.5;
                     }
                     else if (strcmp(currentUserSubCity, "Gondar Town") == 0) {
                         // Distances if user is centrally in "Gondar Town"
                         if (strcmp(stationSubCity, "Arada") == 0) distance = 0.8;
                         else if (strcmp(stationSubCity, "Fasil") == 0) distance = 3.2;
                         else if (strcmp(stationSubCity, "Maraki") == 0) distance = 1.8;
                         else if (strcmp(stationSubCity, "Azezo") == 0) distance = 11.5;
                         else if (strcmp(stationSubCity, "Gondar Town") == 0) distance = 0.3; // Very close to other 'Gondar Town' points if any existed
                         // Add distances to specific stations based on Gondar Town being central
                         else if(strcmp(stations[i].name, "Total Ethiopia") == 0) distance = 3.2; // Fasil
                         else if(strcmp(stations[i].name, "Shell Gondar") == 0) distance = 0.8; // Arada
                         else if(strcmp(stations[i].name, "NOC") == 0) distance = 1.8; // Maraki
                         // ... and so on for other stations relative to a central point
                     }


                    // Apply the calculated distance to the adjacency list (temporarily for this search)
                    // Connect userNodeId to station i and vice-versa
                    if (distance < INF) {
                        adjLists[userNodeId][i] = {i, distance};
                        adjLists[i][userNodeId] = {userNodeId, distance};
                         //cout << "  Set distance User <-> " << stations[i].name << " to " << distance << endl; // Debug output
                    } else {
                        // If no specific distance rule applied, maybe set to 0 or keep previous?
                        // Setting to 0 ensures Dijkstra won't use it unless it's the only option,
                        // but might be confusing. Let's set weight to 0.
                        adjLists[userNodeId][i] = {i, 0.0};
                        adjLists[i][userNodeId] = {userNodeId, 0.0};
                         // cout << "  No specific rule for User <-> " << stations[i].name << ". Setting dist to 0." << endl; // Debug output
                    }
                }
                // --- End of dynamic distance update ---

                // Now find the nearest using the updated adjLists from userNodeId
                findNearestGasStation(stations, adjLists, userNodeId, numStations);
                break;
            }
            case 3: { // Find cheapest gas
                double minPrice = INF;
                int cheapestStationId = -1;
                bool found = false;
                for (int i = 0; i < numStations; i++) {
                    // Skip the abstract user node
                    if (i == userNodeId) continue;
                    // Consider only stations with a valid price (> 0 maybe?)
                    if (stations[i].price > 0 && stations[i].price < minPrice) {
                        minPrice = stations[i].price;
                        cheapestStationId = i;
                        found = true;
                    }
                }
                if (found) {
                    cout << "\n--- Cheapest Gas Station --- \n";
                    cout << stations[cheapestStationId].name
                         << " (" << stations[cheapestStationId].subCity << ")"
                         << " at " << stations[cheapestStationId].price << " ETB/Liter\n";
                     // Optionally show distance if user location is set
                     int path[MAX_STATIONS]; int pathLength=0;
                     double dist = dijkstra(adjLists, userNodeId, cheapestStationId, numStations, path, pathLength);
                     if(dist < INF) {
                         cout << "Distance from your location ("<< stations[userNodeId].subCity <<"): " << dist << " km\n";
                     } else {
                         cout << "Distance: Not Reachable from your location ("<< stations[userNodeId].subCity <<")\n";
                     }
                } else {
                    cout << "\nNo Gas stations with valid prices found.\n";
                }
                break;
            }
            case 4: { // Find path between stations
                int startId = -1, endId = -1;
                cout << "\n--- Find Shortest Path --- \n";
                cout << "Available Stations (ID: Name): \n";
                 for(int i=0; i<numStations; ++i){
                     if(i == userNodeId) cout << "  " << i << ": Your Location (" << stations[i].subCity << ")" << endl;
                     else cout << "  " << i << ": " << stations[i].name << endl;
                 }

                cout << "Enter start station ID: ";
                 while (!(cin >> startId) || startId < 0 || startId >= numStations) {
                     cout << "Invalid ID. Enter ID between 0 and " << numStations - 1 << ": ";
                     cin.clear();
                     cin.ignore(numeric_limits<streamsize>::max(), '\n');
                 }

                cout << "Enter end station ID: ";
                while (!(cin >> endId) || endId < 0 || endId >= numStations) {
                    cout << "Invalid ID. Enter ID between 0 and " << numStations - 1 << ": ";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }


                if (startId == endId) {
                    cout << "Start and end stations are the same.\n";
                    break;
                }

                int path[MAX_STATIONS];
                int pathLength = 0;
                double dist = dijkstra(adjLists, startId, endId, numStations, path, pathLength);

                if (dist < INF) {
                    cout << "\nShortest Distance: " << dist << " km\n";
                    cout << "Path: ";
                    for (int i = 0; i < pathLength; i++) {
                         // Use name, fallback to ID if name is missing or it's user node
                         const char* nodeName = (stations[path[i]].id == userNodeId) ? stations[path[i]].subCity : stations[path[i]].name;
                        if (strlen(nodeName) == 0) { // Fallback if name is empty
                             cout << "ID(" << path[i] << ")";
                        } else {
                            cout << nodeName;
                        }

                        if (i < pathLength - 1) cout << " -> ";
                    }
                    cout << endl;
                } else {
                    cout << "\nNo path exists between station " << startId << " and station " << endId << ".\n";
                }
                break;
            }
            case 5: { // Add new station
                 // Use the function from Code B
                 // Pass numStations (current total) to addGasStation, it uses nextId internally
                addGasStation(stations, adjLists, userNodeId, nextId, validSubCities, numSubCities);
                break;
            }
            case 0: {
                cout << "\nExiting Gas Station Tracker. Goodbye!\n";
                break;
            }
            default: {
                cout << "\nInvalid choice. Please try again.\n";
                break;
            }
        } // End switch
         cout << endl; // Add a newline for better readability between menu cycles

    } while (choice != 0);

    return 0;
}