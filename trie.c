#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "defs.h"


//build trie;
//verify trie;
//free trie;
TrieNode* makeTrieNode(char* data, int isParam, int line) {
    TrieNode* n = (TrieNode*) calloc(1, sizeof(TrieNode));
    for (int i=0; i < MAX_NODES; ++i) {
        n->child[i] = NULL;
    }
    n->isParam = isParam;
    n->val = data;
    n->line = line;
    //printf("MADE TRIE FOR %s VALUE\n", n->val);
    return n;
};


void freeTrie(TrieNode* node) {
    for (int i = 0; i < MAX_NODES; ++i) {
        if (node->child[i] != NULL) {
            free(node->child[i]->val);
            freeTrie(node->child[i]);
        } else {
            continue;
        }
    }
    free(node);
}

TrieNode* insertRoute(TrieNode* root, char* route, int line) {
    TrieNode* temp = root;
    // scan route
    // extract the names between "/"
    // while extracting make nodes with the vaule extracte if has ":"
    // isParam = 1
    // assuming that the first route is binded to the root of the Trie
    int routeCount = 0;
    int wcursor = 0;
    int wordcount = 0;
    char *word = malloc(sizeof(char) * 64);
    while (route[wcursor] != '\0') {
        if (route[wcursor] == '/' && wcursor == 0) {
            wcursor++;
            continue;
        }
        word[wordcount] = route[wcursor];
        if ((route[wcursor] == '/' && wcursor != 0) || route[wcursor + 1] == '\0') {  
            if (word[wordcount] == '/') word[wordcount] = '\0';
            for (int i = 0; i < MAX_NODES; ++i) {
                if (temp->child[i] != NULL) {
                    if (strcmp(temp->child[i]->val, word) == 0) {
                        temp = temp->child[i];
                        break;
                    }
                    continue;
                }
                TrieNode *node = makeTrieNode(word, 0, line);
                if (word[0] == ':') node->isParam = 1;
                if (route[wcursor + 1] == '\0') node->isLeaf = 1;
                temp->child[i] = node;
                temp = node;
                break;
            }
            
            wcursor++;
            word = malloc(sizeof(char) * 64);
            wordcount = 0;
            continue;
        }
        wcursor++;
        wordcount++;
    }
    //temp->isLeaf = 1;
    return root;
};
