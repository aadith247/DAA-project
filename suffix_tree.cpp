#include <iostream>
#include <fstream>
#include <cstring>
#include <iomanip>
using namespace std;

#define MAX_DOCS 10
#define MAX_LEN 1000
#define ALPHABET_SIZE 128

struct TrieNode {
    TrieNode* children[ALPHABET_SIZE];
    bool is_end;
    int doc_ids[MAX_DOCS];
    int doc_count;

    TrieNode() {
        is_end = false;
        doc_count = 0;
        memset(children, 0, sizeof(children));
        memset(doc_ids, -1, sizeof(doc_ids));
    }
};

TrieNode* createNode() {
    return new TrieNode();
}

void insertSuffix(TrieNode* root, char* text, int doc_id) {
    int len = strlen(text);
    for (int i = 0; i < len; i++) {
        TrieNode* node = root;
        for (int j = i; j < len; j++) {
            char ch = text[j];
            if (!node->children[ch])
                node->children[ch] = createNode();
            node = node->children[ch];

            bool found = false;
            for (int d = 0; d < node->doc_count; d++) {
                if (node->doc_ids[d] == doc_id) {
                    found = true;
                    break;
                }
            }
            if (!found && node->doc_count < MAX_DOCS) {
                node->doc_ids[node->doc_count++] = doc_id;
            }
        }
        node->is_end = true;
    }
}

void matchDocument(TrieNode* root, char* new_doc, int num_docs, int* scores) {
    int len = strlen(new_doc);
    for (int i = 0; i < len; i++) {
        TrieNode* node = root;
        for (int j = i; j < len; j++) {
            char ch = new_doc[j];
            if (!node->children[ch]) break;
            node = node->children[ch];

            for (int d = 0; d < node->doc_count; d++) {
                int id = node->doc_ids[d];
                scores[id]++;
            }
        }
    }
}

void calculateSimilarityScores(int* raw_scores, double* similarity_scores, int num_docs) {

    int max_score = 0;
    for (int i = 0; i < num_docs; i++) {
        if (raw_scores[i] > max_score) {
            max_score = raw_scores[i];
        }
    }

    for (int i = 0; i < num_docs; i++) {
        if (max_score == 0)
            similarity_scores[i] = 0.0;
        else
            similarity_scores[i] = (raw_scores[i] * 100.0) / max_score;
    }
}

int main() {
    int num_docs = 4;
    char docs_c[MAX_DOCS][MAX_LEN];
    char query_c[MAX_LEN];

    for (int i = 0; i < num_docs; i++) {
        string filename = "doc" + to_string(i + 1) + ".txt";
        ifstream file(filename);
        if (file.is_open()) {
            file.getline(docs_c[i], MAX_LEN);
            file.close();
        } else {
            cerr << "Error opening " << filename << endl;
            return 1;
        }
    }

    ifstream query_file("query.txt");
    if (query_file.is_open()) {
        query_file.getline(query_c, MAX_LEN);
        query_file.close();
    } else {
        cerr << "Error opening query.txt" << endl;
        return 1;
    }

    if (strlen(query_c) == 0) {
        cerr << "query.txt is empty!" << endl;
        cout << "Similarity Results:\n";
        for (int i = 0; i < num_docs; i++) {
            cout << "Doc ID " << (i + 1) << ": 0.0% similarity\n";
        }
        return 0;
    }

    int scores[MAX_DOCS] = {0};
    double similarity[MAX_DOCS] = {0.0};
    TrieNode* root = createNode();

    for (int i = 0; i < num_docs; i++) {
        insertSuffix(root, docs_c[i], i);
    }

    matchDocument(root, query_c, num_docs, scores);

    calculateSimilarityScores(scores, similarity, num_docs);

    cout << "Similarity Results:\n";
    for (int i = 0; i < num_docs; i++) {
        cout << "Doc ID " << (i + 1) << ": " << fixed << setprecision(2) << similarity[i] << "% similarity\n";
    }
    return 0;
}
