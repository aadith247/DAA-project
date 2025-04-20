#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <algorithm>
#include <cmath>
#include <filesystem>
namespace fs = std::filesystem;
using namespace std;

vector<string> getNGrams(const string& text, int n) {
    vector<string> words;
    string word, temp;
    for (char ch : text) {
        if (isalnum(ch)) temp += tolower(ch);
        else if (!temp.empty()) {
            words.push_back(temp);
            temp.clear();
        }
    }
    if (!temp.empty()) words.push_back(temp);

    vector<string> ngrams;
    for (size_t i = 0; i + n <= words.size(); ++i) {
        stringstream ss;
        for (int j = 0; j < n; ++j) {
            if (j > 0) ss << " ";
            ss << words[i + j];
        }
        ngrams.push_back(ss.str());
    }
    return ngrams;
}

// Read entire file content
string readFileContent(const string& filename) {
    ifstream file(filename);
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main() {
    string folderPath = "./docs"; 
    int n = 3; 
    unordered_map<string, unordered_set<int>> ngramToDocMap;
    vector<string> documents;
    vector<string> filenames;
    int docID = 0;
    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (entry.path().extension() == ".txt") {
            string content = readFileContent(entry.path().string());
            documents.push_back(content);
            filenames.push_back(entry.path().filename());

            vector<string> ngrams = getNGrams(content, n);
            for (const string& ng : ngrams) {
                ngramToDocMap[ng].insert(docID);
            }
            docID++;
        }
    }
    string queryDoc = readFileContent("query.txt");
    vector<string> queryNGrams = getNGrams(queryDoc, n);

    unordered_map<int, int> matchCount;
    for (const string& ng : queryNGrams) {
        for (int id : ngramToDocMap[ng]) {
            matchCount[id]++;
        }
    }
    vector<pair<int, double>> similarity;
    for (auto& [id, count] : matchCount) {
        double score = (double)count / queryNGrams.size();
        similarity.push_back({id, score});
    }
    sort(similarity.begin(), similarity.end(), [](auto& a, auto& b) {
        return a.second > b.second;
    });
    cout << "Top matching documents:\n";
    for (auto& [id, score] : similarity) {
        cout << "Doc: " << filenames[id] << ", Similarity: " << fixed << setprecision(6) << score << endl;
    }

    return 0;
}
