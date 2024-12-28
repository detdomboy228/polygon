#include <vector>
#include <math.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <stack>
#include <queue>
#include <iomanip>
#include <cassert>
#include <chrono>

using namespace std;
struct Node {
    double lon, lat;
    vector<pair<Node*, double>> kids;
};

struct Graph {
    vector<Node*> nodes;

    Node* find_closest_node(double lat, double lon) {
        // Функция для поиска в графе узла, который ближе всего находится к указанной точке,
        // которую вы выбрали на карте
        double min_distance = 99999;
        Node* node_found = nullptr;

        for (auto& node : nodes) {
            double distance = sqrt(pow(node->lat - lat, 2) + pow(node->lon - lon, 2));
            if (distance < min_distance) {
                node_found = node;
                min_distance = distance;
            }
        }
        return node_found;
    }
};


static vector<string> customSplit(string str, char separator) {
    int startIndex = 0, endIndex = 0;
    vector <string> strings;
    for (int i = 0; i <= str.size(); i++) {
        if (str[i] == separator || i == str.size()) {
            endIndex = i;
            string temp;
            temp.append(str, startIndex, endIndex - startIndex);
            strings.push_back(temp);
            startIndex = endIndex + 1;
        }
    }
    return strings;
}


Graph make_a_graph(const string& filename) {
    Graph graph;
    string line;
    string path = filename;
    ifstream in(path);
    unordered_map<string, pair<Node*, vector<string>>> rk;
    if (in.is_open())
    {
        while (getline(in, line)) {
            string root;
            root = customSplit(line, ':')[0];
            vector<string> coords = customSplit(root, ',');
            vector<string> empty;
            vector<string> kids = customSplit(customSplit(line, ':')[1], ';');

            ostringstream lon;
            lon << fixed << setprecision(10) << coords[0];

            ostringstream lat;
            lat << fixed << setprecision(10) << coords[1];

            Node* n = new Node{ stod(lon.str()), stod(lat.str()) };

            pair<Node*, vector<string>> pr = { n, empty };
            rk[root] = pr;
            for (auto kid : kids) {
                if (kid == "") continue;
                vector<string> coords = customSplit(kid, ',');

                ostringstream lon;
                lon << fixed << setprecision(10) << coords[0];

                ostringstream lat;
                lat << fixed << setprecision(10) << coords[1];

                Node* n = new Node{ stod(lon.str()), stod(lat.str()) };

                pair<Node*, vector<string>> pr = { n, empty };
                rk[coords[0] + ',' + coords[1]] = pr;
            }
        }
    }
    else cout << "Неверное наименование файла";
    in.close();
    ifstream in2(path);
    if (in2.is_open())
    {
        while (getline(in2, line)) {
            string root;
            Node* n;
            root = customSplit(line, ':')[0];
            n = rk[root].first;
            vector<string> kids = customSplit(customSplit(line, ':')[1], ';');
            vector<pair<Node*, double>> vec;
            for (auto kid : kids) {
                if (kid == "") continue;
                vector<string> data = customSplit(kid, ',');
                Node* n;
                string coords = data[0] + ',' + data[1];
                double weight = stod(data[2]);
                n = rk[coords].first;
                pair<Node*, double> pr;
                pr.first = n;
                pr.second = weight;
                vec.push_back(pr);
            }
            n->kids = vec;
            graph.nodes.push_back(n);
        }
    }
    else cout << "Неверное наименование файла";
    in2.close();
    return graph;
}


tuple<vector<string>, double, int> dfs(Node* s, Node* f) {
    stack<Node*> stack; // mem - O(V) (худший случай)
    unordered_map<Node*, bool> visited; // аналогично
    unordered_map<Node*, Node*> dad; // аналогично
    stack.push(s);
    cout << "Starting DFS..." << endl;
    while (!stack.empty()) { // O(V + E) - худший случай, но на деле - зависит от расположения вершин, может быть гораздо меньше
        Node* n = stack.top();
        stack.pop();
        if (n == f) {
            vector<string> path; // mem/diff O(E)
            int num_edges = 0;
            Node* previous = n;
            double dist = 0;
            for (Node* e = f; e != s; e = dad[e]) { // длина пути - diff O(E) худший случай
                path.push_back(to_string(e->lon) + ',' + to_string(e->lat));
                num_edges += 1;
                for (auto pr : e->kids) { // ~2 в нашем случае, но на бумаге может быть diff O(E)
                    if (pr.first == previous) dist += pr.second;
                }
                previous = e;
            }
            reverse(path.begin(), path.end());
            tuple<vector<string>, double, int> tup = { path, dist, num_edges }; // опять же mem O(E) в худшем
            cout << "Done!" << endl;
            return tup;
        }
        for (auto e : n->kids) { // За весь перебор while пройдем E ребер (в полном dfs, тут - меньше)
            Node* neighbor = e.first;
            if (!visited[neighbor]) {
                stack.push(neighbor);
                visited[neighbor] = true;
                dad[neighbor] = n;
            }
        }
    }
    cout << "Done!" << endl;
    return {};
}


tuple<vector<string>, double, int> bfs(Node* s, Node* f) {
    queue<Node*> q; // mem - O(V) (худший случай)
    unordered_map<Node*, bool> visited; // аналогично
    unordered_map<Node*, Node*> dad; // аналогично
    q.push(s);
    cout << "Starting BFS..." << endl;
    while (!q.empty()) { // diff O(V + E) в худшем случае (аналогично dfs, на деле мы можем не проходить весь граф)
        Node* n = q.front();
        q.pop();

        if (n == f) {
            vector<string> path; // mem/diff O(E)
            int num_edges = 0;
            Node* previous = n;
            double dist = 0;
            for (Node* e = f; e != s; e = dad[e]) { // длина пути - diff O(E) худший случай
                path.push_back(to_string(e->lon) + ',' + to_string(e->lat));
                num_edges += 1;
                for (auto pr : e->kids) { // ~2 в нашем случае, но на бумаге может быть diff O(E)
                    if (pr.first == previous) dist += pr.second;
                }
                previous = e;
            }
            reverse(path.begin(), path.end());
            tuple<vector<string>, double, int> tup = { path, dist, num_edges }; // опять же mem O(E) в худшем
            cout << "Done!" << endl;
            return tup;
        }
        for (auto e : n->kids) { // За весь перебор while пройдем E ребер (в полном bfs, тут - меньше)
            Node* neighbor = e.first;
            if (!visited[neighbor]) {
                q.push(neighbor);
                visited[neighbor] = true;
                dad[neighbor] = n;
            }
        }
    }
    cout << "Done!" << endl;
    return {};
}


tuple<vector<string>, double, int> dijkstra(Graph& graph, Node* s, Node* f) {

    auto cmp = [](pair<Node*, double> left, pair<Node*, double> right) { return
        left.second > right.second; };
    priority_queue<pair<Node*, double>, vector<pair<Node*, double>>, decltype(cmp)>
        pq(cmp); // mem O(V)

    unordered_map<Node*, Node*> dad; // mem O(V) - собираем для каждой вершины
    unordered_map<Node*, double> dist; // аналогично

    dist[s] = 0;

    pq.push({ s , 0});
    cout << "Starting Dijkstra..." << endl;
    while (!pq.empty()) { // diff O((V + E)logV) - худший случай, но мы не обязаны проходить весь граф
        pair<Node*, double> pr = pq.top();
        Node* n = pr.first;
        double curr_dist = pr.second;

        pq.pop(); // mem O(logV) в худшем
        if (n == f) {
            vector<string> path; // O(E) в худшем
            Node* previous = f;
            int num_edges = 0;
            for (Node* e = f; e != s; e = dad[e]) { // O(E), аналогично
                path.push_back(to_string(e->lon) + ',' + to_string(e->lat));
                previous = e;
                num_edges += 1;
            }
            reverse(path.begin(), path.end());
            tuple<vector<string>, double, int> tup = { path, dist[f], num_edges }; // mem O(E)
            cout << "Done!" << endl;
            return tup;
        }
        for (auto e : n->kids) { // в худшем случае суммарно E штук рёбер
            Node* neighbor = e.first;
            double new_dist = curr_dist + e.second;
            if (dist.find(neighbor) == dist.end()) {
                dist[neighbor] = numeric_limits<double>::max();
            }
            if (new_dist < dist[neighbor]) {
                dist[neighbor] = new_dist;
                pq.push({ neighbor, new_dist }); // diff O(logV) 
                dad[neighbor] = n;
            }
        }
    }
    cout << "Done!" << endl;
    return {};
}

double heuristic(Node* a, Node* b) {
    return sqrt(pow(a->lat - b->lat, 2) + pow(a->lon - b->lon, 2));
}


tuple<vector<string>, double, int> a_star(Graph& graph, Node* s, Node* f) {

    auto cmp = [](pair<Node*, double> left, pair<Node*, double> right) { return
        left.second > right.second; };
    priority_queue<pair<Node*, double>, vector<pair<Node*, double>>, decltype(cmp)>
        pq(cmp); // ~ mem O(V)

    unordered_map<Node*, Node*> dad; // mem O(|path|) ~ O(E) в худшем случае

    unordered_map<Node*, double> g_score; // mem O(V)
    unordered_map<Node*, double> f_score; // mem O(V)

    g_score[s] = 0.0;
    f_score[s] = heuristic(s, f);

    pq.push({ s , f_score[s]});
    cout << "Starting A*..." << endl;
    while (!pq.empty()) { // diff O((V + E)logV) в худшем
        pair<Node*, double> pr = pq.top();
        Node* n = pr.first;

        pq.pop(); // diff O(logV)
        if (n == f) {
            vector<string> path; // O(E) в худшем
            Node* previous = f;
            int num_edges = 0;
            double dist = 0;
            for (Node* e = f; e != s; e = dad[e]) { // O(E) по аналогии
                path.push_back(to_string(e->lon) + ',' + to_string(e->lat));
                previous = e;
                num_edges += 1;
            }
            reverse(path.begin(), path.end());
            tuple<vector<string>, double, int> tup = { path, g_score[f], num_edges}; // mem O(E)
            cout << "Done!" << endl;
            return tup;
        }
        for (auto e : n->kids) { // суммарно в худшем случае пробегаем E ребер.
            Node* neighbor = e.first;
            double mb_dist = g_score[n] + e.second;
            if (g_score.find(neighbor) == g_score.end()) {
                g_score[neighbor] = numeric_limits<double>::max();
            }
            if (mb_dist < g_score[neighbor]) {
                g_score[neighbor] = mb_dist;
                f_score[neighbor] = mb_dist + heuristic(neighbor, f);
                pq.push({ neighbor, f_score[neighbor]}); // diff O(logV)
                dad[neighbor] = n;
            }
        }
    }
    cout << "Done!" << endl;
    return {};
}


void test() {
    cout << "Test:" << endl;
    Graph graph = make_a_graph("test.txt");
    int edges;
    double dist;
    vector<string> vec;

    Node* a = graph.find_closest_node(1, 1);
    Node* b = graph.find_closest_node(5, 5);

    tie(vec, dist, edges) = dfs(a, b);
    assert(vec.size() != 0); // для сильно связного графа
    tie(vec, dist, edges) = bfs(a, b);
    assert(vec.size() != 0); // тоже только для сильно связного графа
    tie(vec, dist, edges) = dijkstra(graph, a, b);
    assert(dist == 6);
    tie(vec, dist, edges) = a_star(graph, a, b);
    assert(dist == 6);
    cout << "Test has been completed successfully!" << endl << endl;
}



int main() {
    Graph graph;
    test();
    cout << "Reading the file..." << endl;
    graph = make_a_graph("spb_graph.txt");
    cout << "Success!" << endl << endl;
    vector<double> kronva = { 30.309781 , 59.956335 };
    vector<double> point = { 30.500086 , 59.936629 };
    Node* nkr = graph.find_closest_node(kronva[1], kronva[0]);
    Node* nbs = graph.find_closest_node(point[1], point[0]);

    int edges;
    double dist;
    vector<string> vec;

    auto start_dfs = chrono::high_resolution_clock::now();
    tie(vec, dist, edges) = dfs(nbs, nkr);
    auto end_dfs = chrono::high_resolution_clock::now();
    auto elapsed_dfs = chrono::duration_cast<chrono::duration<double>> (end_dfs - start_dfs);
    cout << "Distanse (DFS): " << dist << endl;
    cout << "Number of edges (DFS): " << edges << endl;
    cout << endl;

    auto start_bfs = chrono::high_resolution_clock::now();
    tie(vec, dist, edges) = bfs(nbs, nkr);
    auto end_bfs = chrono::high_resolution_clock::now();
    auto elapsed_bfs = chrono::duration_cast<chrono::duration<double>> (end_bfs - start_bfs);
    cout << "Distanse (BFS): " << dist << endl;
    cout << "Number of edges (BFS): " << edges << endl;
    cout << endl;

    auto start_dijkstra = chrono::high_resolution_clock::now();
    tie(vec, dist, edges) = dijkstra(graph, nbs, nkr);
    auto end_dijkstra = chrono::high_resolution_clock::now();
    auto elapsed_dijkstra = chrono::duration_cast<chrono::duration<double>> (end_dijkstra - start_dijkstra);
    cout << "Min distanse (Dijkstra): " << dist << endl;
    cout << "Number of edges (Dijkstra): " << edges << endl;
    cout << endl;

    auto start_a_star= chrono::high_resolution_clock::now();
    tie(vec, dist, edges) = a_star(graph, nbs, nkr);
    auto end_a_star = chrono::high_resolution_clock::now();
    auto elapsed_a_star = chrono::duration_cast<chrono::duration<double>> (end_a_star - start_a_star);
    cout << "Min distanse (A*): " << dist << endl;
    cout << "Number of edges (A*): " << edges << endl;
    cout << endl;

    cout << "Time comparasion:" << endl;
    cout << "DFS: " << elapsed_dfs.count() << " sec." << endl;
    cout << "BFS: " << elapsed_bfs.count() << " sec." << endl;
    cout << "Dijkstra: " << elapsed_dijkstra.count() << " sec." << endl;
    cout << "A*: " << elapsed_a_star.count() << " sec." << endl;
}