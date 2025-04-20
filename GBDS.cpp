#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <algorithm>
#include <iomanip>

using namespace std;

struct Doc {
    int id;
    string text;
};

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

vector<pair<int, double>> detect_plagiarism_ngrams(const string& queryText, const vector<Doc>& docs, int n, int top_k) {
    vector<string> queryNGrams = getNGrams(queryText, n);
    unordered_map<string, unordered_set<int>> ngramToDocMap;

    // Indexing n-grams for all documents
    for (const auto& doc : docs) {
        vector<string> ngrams = getNGrams(doc.text, n);
        for (const string& ng : ngrams) {
            ngramToDocMap[ng].insert(doc.id);
        }
    }

    unordered_map<int, int> matchCount;
    for (const string& ng : queryNGrams) {
        for (int id : ngramToDocMap[ng]) {
            matchCount[id]++;
        }
    }

    // Top-K similarity results using min-heap
    using ScorePair = pair<double, int>; // similarity, doc_id
    priority_queue<ScorePair, vector<ScorePair>, greater<>> pq;

    for (const auto& [id, count] : matchCount) {
        double similarity = (double)count / queryNGrams.size();
        pq.emplace(similarity, id);
        if ((int)pq.size() > top_k) pq.pop();
    }

    vector<pair<int, double>> results;
    while (!pq.empty()) {
        results.emplace_back(pq.top().second, pq.top().first);
        pq.pop();
    }
    reverse(results.begin(), results.end());
    return results;
}

int main() {
    vector<Doc> docs = {
        {1, "The quick brown fox jumps over the lazy dog"},
        {2, "A fox fled from danger to safety"},
        {3, "The lazy dog sleeps while the fox jumps"}
    };

    string queryText = "This is a test with the quick brown fox running";
    int n = 3;
    int top_k = 2;

    auto results = detect_plagiarism_ngrams(queryText, docs, n, top_k);

    cout << "Top matching documents:\n";
    for (const auto& [doc_id, similarity] : results) {
        cout << "Doc ID: " << doc_id << ", Similarity: " << fixed << setprecision(6) << similarity << "\n";
    }

    return 0;
}
