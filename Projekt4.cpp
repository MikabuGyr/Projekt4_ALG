#include <iostream>
#include <vector>
#include <math.h>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
using namespace std;
vector<vector<int>> parseGraphFile(const string& filename) 
{
    std::ifstream file(filename);
    vector<vector<int>> graphData;

    if (not file.is_open()) 
    {
        std::cerr << "Unable to open file " << filename << "\n";
        return {};
    }

    string line;
    while (std::getline(file, line)) 
    {
        std::stringstream ss(line);
        int u, v;
        if (ss >> u >> v) 
        {
            graphData.push_back({u, v});
        }
    }

    return graphData;
}
class Graph{
    public: 
    unordered_set<int> vrcholy; //
    unordered_map<int, unordered_set<int>> sousede;
    Graph(const vector<vector<int>>& hrany)
    {
        for(const auto& hrana: hrany)
        {
            int u = hrana[0], v = hrana[1];
            vrcholy.insert(u);
            vrcholy.insert(v);
            sousede[u].insert(v);
        }
    }
    void print() const
    {
        for(const auto& [vertex, friends]:sousede)
        {
            cout << vertex << ": ";
            for (const int item: friends) 
            {
                cout << item << " ";
            }
            cout << "\n";
        }
    }
};

unordered_map<int, int> BFS_with_histogram(const Graph& graf, int start)
{
	unordered_map<int, int> pocetVzdalenosti;
	queue<int> fronta;
	unordered_map<int, int> vzdalenosti;
	fronta.push(start);
	vzdalenosti[start] = 0;
	while (!fronta.empty()) //tady mi pomohl ChatGPT, měl jsem tam chybu, kde jsem se dostal out of range
	{
		int aktualni = fronta.front();
		fronta.pop();
		int aktualniVzdalenost = vzdalenosti[aktualni];
		pocetVzdalenosti[aktualniVzdalenost]++;
		auto it = graf.sousede.find(aktualni);
        if (it != graf.sousede.end()) {
            for (int neighbor : it->second) {
                if (vzdalenosti.find(neighbor) == vzdalenosti.end()) {
                    vzdalenosti[neighbor] = aktualniVzdalenost + 1;
                    fronta.push(neighbor);
                }
            }
        }
	}
	return pocetVzdalenosti;
}

void writeHistogramToFile(const std::string& filename, const std::unordered_map<int, int>& histogram) {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Unable to open output file: " << filename << "\n";
        return;
    }

    //Dle zadani (testovano grafem G0) vypisuji v poradi podle vzdalenosti
    std::vector<int> vzdalenosti;
    for (const auto& [dist, _] : histogram) {
        vzdalenosti.push_back(dist);
    }
    std::sort(vzdalenosti.begin(), vzdalenosti.end());

    for (int dist : vzdalenosti) {
        outFile << dist << ": " << histogram.at(dist) << "\n";
    }

    outFile.close();
}

int main(int argc, char* argv[])
{
    if (argc != 4) {
        std::cerr << "Použití: " << argv[0] << " <vstup> <vystup> <start>\n";
        return 1;
    }

    string input = argv[1];
    string output = argv[2];
    int start = std::stoi(argv[3]);

    vector<vector<int>> data = parseGraphFile(input);
    Graph graf(data);
    auto histogram = BFS_with_histogram(graf, start);
    writeHistogramToFile(output, histogram);

    return 0;
}