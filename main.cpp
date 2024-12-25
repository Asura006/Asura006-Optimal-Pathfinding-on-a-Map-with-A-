#include <graphics.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <sstream>
#include <queue>
#include <climits>
using namespace std;
struct Node {
    int x, y;  
    int id;   
};
vector<Node> nodes;
vector<vector<int> > adjMatrix;
int nodeCount = 0;
string intToString(int num) {
    ostringstream ss;
    ss << num;
    return ss.str();
}
void drawNode(int x, int y, int id) {
    setcolor(WHITE);
    circle(x, y, 10);  
    setfillstyle(SOLID_FILL, BLUE);  
    floodfill(x, y, WHITE);
    outtextxy(x - 5, y - 5, const_cast<char*>(intToString(id).c_str())); 
}
void drawEdge(int x1, int y1, int x2, int y2, int weight) {
    setcolor(YELLOW);
    line(x1, y1, x2, y2);  
    char label[10];
    sprintf(label, "%d", weight);
    outtextxy((x1 + x2) / 2, (y1 + y2) / 2, label);
}
double heuristic(int a, int b) {
    int dx = nodes[a].x - nodes[b].x;
    int dy = nodes[a].y - nodes[b].y;
    return sqrt(dx * dx + dy * dy);
}
vector<int> aStar(int start, int end) {
    int n = nodes.size();
    vector<double> g(n, INT_MAX), f(n, INT_MAX);
    vector<int> prev(n, -1);
    vector<bool> closedSet(n, false);
    g[start] = 0;
    f[start] = heuristic(start, end); 
    priority_queue<pair<double, int>, vector<pair<double, int> >, greater<pair<double, int> > > openSet;
    openSet.push({f[start], start});
    while (!openSet.empty()) {
        int current = openSet.top().second;
        openSet.pop();
        if (current == end) {
            break; 
        }
        if (closedSet[current]) continue;
        closedSet[current] = true;
        for (int neighbor = 0; neighbor < n; ++neighbor) {
            if (adjMatrix[current][neighbor] == 0 || closedSet[neighbor]) continue; 
            double tentativeG = g[current] + adjMatrix[current][neighbor];
            if (tentativeG < g[neighbor]) {
                prev[neighbor] = current;
                g[neighbor] = tentativeG;
                f[neighbor] = g[neighbor] + heuristic(neighbor, end);  
                openSet.push({f[neighbor], neighbor});
            }
        }
    }
    vector<int> path;
    for (int at = end; at != -1; at = prev[at]) {
        path.push_back(at);
    }
    for (int i = 0; i < path.size() / 2; i++) {
        swap(path[i], path[path.size() - i - 1]);
    }
    return path;
}
void drawPath(const vector<int>& path) {
    for (int i = 0; i < path.size() - 1; ++i) {
        int x1 = nodes[path[i]].x, y1 = nodes[path[i]].y;
        int x2 = nodes[path[i + 1]].x, y2 = nodes[path[i + 1]].y;
        setcolor(RED);  
        line(x1, y1, x2, y2);  
    }
}
void createRandomMap() {
    srand(time(0));  
    for (int i = 0; i < 100; ++i) {
        nodes.push_back({rand() % 750 + 50, rand() % 550 + 50, i});  
    }
    nodeCount = nodes.size();
    adjMatrix = vector<vector<int> >(nodeCount, vector<int>(nodeCount, 0));
    int edges = 0;
    while (edges < 150) {
        int u = rand() % nodeCount;
        int v = rand() % nodeCount;
        if (u != v && adjMatrix[u][v] == 0) {  
            int weight = rand() % 20 + 1;  
            adjMatrix[u][v] = adjMatrix[v][u] = weight; 
            edges++;
        }
    }
}
int main() {
    initwindow(800, 600, "A* Pathfinding - Map");
    readimagefile("map.jpg", 0, 0, 800, 600); 
    int errorCode = graphresult();
    if (errorCode != grOk) {
        cout << "Error loading image. Error code: " << errorCode << endl;
    }
    createRandomMap();
    for (int i = 0; i < nodeCount; ++i) {
        drawNode(nodes[i].x, nodes[i].y, i);
    }
    for (int i = 0; i < nodeCount; ++i) {
        for (int j = i + 1; j < nodeCount; ++j) {
            if (adjMatrix[i][j] > 0) {
                drawEdge(nodes[i].x, nodes[i].y, nodes[j].x, nodes[j].y, adjMatrix[i][j]);
            }
        }
    }
    int startNode = -1, endNode = -1;
    while (true) {
        if (GetAsyncKeyState(VK_LBUTTON)) {
            int x = mousex(), y = mousey();
            for (int i = 0; i < nodeCount; ++i) {
                int distX = x - nodes[i].x;
                int distY = y - nodes[i].y;
                if (sqrt(distX * distX + distY * distY) < 20) {  
                    if (startNode == -1) {
                        startNode = i;  
                        cout << "Start Node: " << nodes[startNode].id << endl;
                    } else if (endNode == -1) {
                        endNode = i;  
                        cout << "End Node: " << nodes[endNode].id << endl;
                    }
                    break;
                }
            }
            if (startNode != -1 && endNode != -1) {
                vector<int> path = aStar(startNode, endNode);
                drawPath(path); 
                cout << "Shortest Path from Node " << nodes[startNode].id << " to Node " << nodes[endNode].id << ":" << endl;
                int totalDistance = 0;
                for (int i = 0; i < path.size() - 1; ++i) {
                    cout << "Node " << nodes[path[i]].id << " -> ";
                    totalDistance += adjMatrix[path[i]][path[i + 1]];
                }
                cout << "Node " << nodes[path[path.size() - 1]].id << endl;
                cout << "Distance: " << totalDistance << endl;
                startNode = -1;
                endNode = -1;
            }
        }
        delay(100);  
    }
    closegraph();
    return 0;
}
