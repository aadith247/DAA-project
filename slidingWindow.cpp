#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <unordered_set>
#include <algorithm>
#include <cctype>
#include <cstdio>

using namespace std;

// Structure to hold document info
struct Doc {
    int id;
    string text;
};

// Tokenize text into lowercase alphanumeric words
vector<string> tokenize(const string& text) {
    vector<string> words;
    stringstream ss(text);
    string word;
    while (ss >> word) {
        string clean_word;
        for (char c : word) {
            if (isalnum(c)) clean_word += tolower(c);
        }
        if (!clean_word.empty()) words.push_back(clean_word);
    }
    return words;
}

// Generate k-gram hashes for a document
unordered_set<size_t> get_kgram_hashes(const vector<string>& words, int k) {
    unordered_set<size_t> hashes;
    for (int i = 0; i <= (int)words.size() - k; ++i) {
        string kgram;
        for (int j = 0; j < k; ++j) {
            kgram += words[i + j] + " ";
        }
        hash<string> hasher;
        hashes.insert(hasher(kgram));
    }
    return hashes;
}

// Compute Jaccard similarity between two hash sets
double jaccard_similarity(const unordered_set<size_t>& a, const unordered_set<size_t>& b) {
    int intersection = 0;
    for (const auto& hash : a) {
        if (b.count(hash)) intersection++;
    }
    int union_size = a.size() + b.size() - intersection;
    return union_size == 0 ? 0.0 : static_cast<double>(intersection) / union_size;
}

int main() {
    // Sample dataset
    vector<Doc> dataset = {
        {1, "The quick brown fox jumps over the lazy dog"},
        {2, "A quick brown fox jumped over a lazy dog"},
        {3, "Lorem ipsum dolor sit amet consectetur adipiscing elit"},
        {4, "The lazy dog sleeps in the sunny spot"},
    };

    // New document to compare
    Doc new_doc = {0, "quick brown fox over lazy dog"};

    int k = 3; // k-gram size

    // Tokenize and hash k-grams for new document
    vector<string> new_tokens = tokenize(new_doc.text);
    unordered_set<size_t> new_hashes = get_kgram_hashes(new_tokens, k);

    // Compare with each document in dataset
    vector<pair<int, double>> similarities;
    for (const Doc& doc : dataset) {
        vector<string> tokens = tokenize(doc.text);
        unordered_set<size_t> hashes = get_kgram_hashes(tokens, k);
        double sim = jaccard_similarity(new_hashes, hashes);
        similarities.push_back({doc.id, sim});
    }

    // Sort by similarity (highest first)
    sort(similarities.begin(), similarities.end(), [](const pair<int, double>& a, const pair<int, double>& b) {
        return a.second > b.second;
    });

    // Output results
    cout << "Top matching documents:" << endl;
    for (const auto& [id, sim] : similarities) {
        printf("Doc ID: %d, Similarity: %.6f\n", id, sim);
    }

    return 0;
}
