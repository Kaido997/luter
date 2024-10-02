#include <stdio.h>
#define MAX_NODES 16
typedef struct TrieNode TrieNode;
struct TrieNode {
    char* val;
    TrieNode* child[MAX_NODES];
    int isParam;
    int isLeaf;
    int line;
};
