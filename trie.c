#include <stdlib.h>
#include <time.h>
#define MAX_NODES 5
typedef struct TrieNode TrieNode;
struct TrieNode {
    char val;
    TrieNode* child[MAX_NODES];
    int isParam;
}; 


//build trie;
//verify trie;
//free trie;

TrieNode* makeTrieNode(char data, int isParam) {
    TrieNode* n = (TrieNode*) calloc(1, sizeof(TrieNode));
    for (int i=0; i < MAX_NODES; ++i) {
        n->child[i] = NULL;
    }
    n->isParam = isParam;
    n->val = data;
    return n;
};


void freeTrie(TrieNode* node) {
    for (int i = 0; i < MAX_NODES; ++i) {
        if (node->child[i] != NULL) {
            freeTrie(node->child[i]);
        } else {
            continue;
        }
    }
    free(node);
}

TrieNode* insertRoute(TrieNode* root, char* route) {
 return root;  
};
