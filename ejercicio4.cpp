#include <iostream>
#include <map>
#include <vector>
#include <queue>
#include <stack>
#include <set>
#include <memory>
using namespace std;

class Node {
public:
    Node(int id) : id_(id) {}
    int getId() const { return id_; }
    void addNeighbor(int id) { neighbors_.push_back(id); }
    const vector<int>& getNeighbors() const { return neighbors_; }
private:
    int id_;
    vector<int> neighbors_;
};

class Graph {
public:
    void addEdge(int from, int to) {
        getOrCreate(from).addNeighbor(to);
        getOrCreate(to);
    }

    bool hasNode(int id) const { return nodes_.count(id) > 0; }
    const vector<int>& neighborsOf(int id) const { return nodes_.at(id).getNeighbors(); }

private:
    Node& getOrCreate(int id) {
        auto it = nodes_.find(id);
        if (it == nodes_.end()) it = nodes_.emplace(id, Node(id)).first;
        return it->second;
    }
    map<int, Node> nodes_;
};

class GraphIterator {
public:
    virtual ~GraphIterator() {}
    virtual bool hasNext() = 0;
    virtual int next() = 0;
};

class BFSIterator : public GraphIterator {
public:
    BFSIterator(const Graph& g, int start) : graph_(g) {
        if (g.hasNode(start)) {
            pending_.push(start);
            visited_.insert(start);
        }
    }
    bool hasNext() override { return !pending_.empty(); }
    int next() override {
        int current = pending_.front();
        pending_.pop();
        for (int n : graph_.neighborsOf(current)) {
            if (!visited_.count(n)) {
                visited_.insert(n);
                pending_.push(n);
            }
        }
        return current;
    }
private:
    const Graph& graph_;
    queue<int> pending_;
    set<int> visited_;
};

class DFSIterator : public GraphIterator {
public:
    DFSIterator(const Graph& g, int start) : graph_(g) {
        if (g.hasNode(start)) pending_.push(start);
    }
    bool hasNext() override {
        skipVisited();
        return !pending_.empty();
    }
    int next() override {
        skipVisited();
        int current = pending_.top();
        pending_.pop();
        visited_.insert(current);
        const vector<int>& nb = graph_.neighborsOf(current);
        for (auto it = nb.rbegin(); it != nb.rend(); ++it) {
            if (!visited_.count(*it)) pending_.push(*it);
        }
        return current;
    }
private:
    void skipVisited() {
        while (!pending_.empty() && visited_.count(pending_.top())) pending_.pop();
    }
    const Graph& graph_;
    stack<int> pending_;
    set<int> visited_;
};

void traverse(GraphIterator& it) {
    while (it.hasNext()) cout << it.next() << " ";
    cout << endl;
}

int main() {
    Graph g;
    g.addEdge(1, 2);
    g.addEdge(1, 3);
    g.addEdge(2, 4);
    g.addEdge(4, 1);

    unique_ptr<GraphIterator> it = make_unique<BFSIterator>(g, 1);
    cout << "BFS: ";
    traverse(*it);

    it = make_unique<DFSIterator>(g, 1);
    cout << "DFS: ";
    traverse(*it);
}