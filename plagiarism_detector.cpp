#include <iostream>
#include <cstring>
#include <iomanip>

using namespace std;

const int MAX_DOCS = 10;
const int MAX_LEN = 1000;
const int ALPHABET_SIZE = 128;

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

void insertSuffix(TrieNode* root, const char* text, int doc_id) {
    int len = strlen(text);
    for (int i = 0; i < len; i++) {
        TrieNode* node = root;
        for (int j = i; j < len; j++) {
            char ch = text[j];
            if (!node->children[ch])
                node->children[ch] = createNode();
            node = node->children[ch];

            bool found = false;// adding root id
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

void matchDocument(TrieNode* root, const char* new_doc, int num_docs, int* scores) {
    int len = strlen(new_doc);
    for (int i = 0; i < len; i++) {
        TrieNode* node = root;
        for (int j = i; j < len; j++) {
            char ch = new_doc[j];
            if (!node->children[ch]) break;
            node = node->children[ch];

            for (int d = 0; d < node->doc_count; d++) {
                int doc_id = node->doc_ids[d];
                scores[doc_id]++;
            }
        }
    }
}

void calculateSimilarityScores(int* raw_scores, double* similarity_scores, int num_docs, int max_possible_score) {
    for (int i = 0; i < num_docs; i++) {
        if (max_possible_score == 0)
            similarity_scores[i] = 0.0;
        else
            similarity_scores[i] = (raw_scores[i] * 100.0) / max_possible_score;
    }
}

int main() {
    const int num_docs = 4;
    const char docs[MAX_DOCS][MAX_LEN] = {
        "the quick brown fox jumps",
        "the lazy dog sleeps",
        "quick dog runs fast",
        "completely different text"
    };
    
    const char new_doc[MAX_LEN] = "the quick brown fox jumps";

    int scores[MAX_DOCS] = {0};
    double similarity[MAX_DOCS] = {0.0};

    TrieNode* root = createNode();

    for (int i = 0; i < num_docs; i++) {
        insertSuffix(root, docs[i], i);
    }
    matchDocument(root, new_doc, num_docs, scores);//match new doc

    int len = strlen(new_doc);
    int max_possible_score = (len * (len + 1)) / 2;

    calculateSimilarityScores(scores, similarity, num_docs, max_possible_score);

    cout << "Top matching documents:\n";
    for (int i = 0; i < num_docs; i++) {
        cout << "Doc ID: " << i + 1 << ", Similarity: " << fixed << setprecision(2) << similarity[i] << "%\n";
    }
    return 0;
}
