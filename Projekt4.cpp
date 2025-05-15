/**
 * @file Projekt4.cpp
 * @brief Načte orientovaný graf ze seznamu hran a vygeneruje histogram vzdáleností od počátečního vrcholu.
 */
#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <string>
#include <fstream>
#include <sstream>
#include <memory>
using namespace std;

class Graph
{
    public: 
    std::vector<std::vector<int>> sousede; ///< Seznam indexů sousedů pro každý vrchol
    std::unordered_map<int, int> id2index; ///< Mapování původního ID vrcholu na interní index
    std::vector<int> index2id; ///< Obrácené mapování indexu zpět na ID
    /**
     * @brief Vrátí existující index pro ID nebo vytvoří nový.
     * @param id Původní ID vrcholu.
     * @return Interní index vrcholu.
     */
    int getOrAddIndex(int id) //zde mi také pomohl chatGPT, leknul jsem se chyby ohledně přetypování a nechtěl jsem v tom mít zmatek
    {
        auto it = id2index.find(id);
        if (it == id2index.end()) {
            int index = static_cast<int>(id2index.size());  // <- správně přetypováno
            id2index[id] = index;
            index2id.push_back(id);
            sousede.emplace_back();
            return index;
        }
        return it->second;
    }
    /**
     * @brief Přidá orientovanou hranu do grafu (u → v).
     * @param u_id Původní ID počátečního vrcholu.
     * @param v_id Původní ID cílového vrcholu.
     */
    void addEdge(int u_id, int v_id) 
    {
        int u = getOrAddIndex(u_id);
        int v = getOrAddIndex(v_id);
        sousede[u].push_back(v);
    }
};
/**
 * @brief Načete graf ze souboru a ihned ho sestaví.
 * @param filename Cesta ke vstupnímu souboru.
 * @param graf Objekt grafu, do kterého se hrany zapisují.
 */
void readGraphFromFile(const string& filename, Graph& graf) 
{
    std::ifstream file(filename);
    if (not file.is_open()) 
    {
        std::cerr << "Unable to open file " << filename << "\n";
        return;
    }

    string line;
    while (std::getline(file, line)) 
    {
        std::stringstream ss(line);
        int u, v;
        if (ss >> u >> v) 
        {
            graf.addEdge(u, v);
        }
    }
}
/**
 * @brief Provede BFS z daného vrcholu a vytvoří histogram vzdáleností.
 * @param graf Graf ve formě seznamu sousedů.
 * @param startIndex Interní index počátečního vrcholu.
 * @return Vektor, kde histogram[d] = počet vrcholů ve vzdálenosti d.
 */
vector<int> BFS_with_histogram(const Graph& graf, int startIndex)
{
    std::vector<int> vzdalenosti(graf.sousede.size(), -1); // -1 = nedosaženo
    std::queue<int> q;

    vzdalenosti[startIndex] = 0;
    q.push(startIndex);

    int maxVzdalenost = 0;

    while (!q.empty()) 
    {
        int current = q.front();
        q.pop();
        int dist = vzdalenosti[current];
        for (int neighbor : graf.sousede[current]) {
            if (vzdalenosti[neighbor] == -1) {
                vzdalenosti[neighbor] = dist + 1;
                maxVzdalenost = std::max(maxVzdalenost, dist + 1);
                q.push(neighbor);
            }
        }
    }

    // histogram[vzdalenost] = počet uzlů
    vector<int> histogram(maxVzdalenost + 1, 0);
    for (int d : vzdalenosti) 
    {
        if (d != -1)
        {
            histogram[d]++;
        }
    }

    return histogram;
}
/**
 * @brief Zapíše histogram vzdáleností do výstupního souboru.
 * @param filename Cesta k výstupnímu souboru.
 * @param histogram Histogram vzdáleností.
 */
void writeHistogramToFile(const std::string& filename, const std::vector<int>& histogram) 
{
    std::ofstream outFile(filename);
    if (!outFile.is_open()) 
    {
        std::cerr << "Unable to open output file: " << filename << "\n";
        return;
    }

    for (size_t vzdalenost = 0; vzdalenost < histogram.size(); ++vzdalenost) 
    {
        int pocet = histogram[vzdalenost];
        if (pocet > 0)
        {
            outFile << vzdalenost << " " << pocet << "\n";
        }
    }
}
/**
 * @brief Hlavní funkce programu. Čte graf, spustí BFS a uloží histogram.
 * @param argc Počet argumentů.
 * @param argv Argumenty: <vstup> <vystup> <start_id>
 * @return 0 při úspěchu, jinak nenulový kód chyby.
 */
int main(int argc, char* argv[])
{
    if (argc != 4) 
    {
        std::cerr << "Použití: " << argv[0] << " <vstup> <vystup> <start>\n";
        return 1;
    }
    
    string input = argv[1];
    string output = argv[2];
    int start = std::stoi(argv[3]);
    Graph graf;
    readGraphFromFile(input, graf);
    if (graf.id2index.find(start) == graf.id2index.end()) 
    {
        std::cerr << "Startovni vrchol nebyl nalezen v grafu\n";
        return 2;
    }
    int startIndex = graf.id2index.at(start);
    vector<int> histogram = BFS_with_histogram(graf, startIndex);
    writeHistogramToFile(output, histogram);

    return 0;
}