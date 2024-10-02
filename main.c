#include <err.h>    
#include <stdio.h>
#include <string.h>
#include "decls.h"
#include "defs.h"
#define extern_
#include "data.h"
#undef extern_
#include <stdlib.h>
FILE *Infile;
int Line;
const char post[] = "Post";
const char get[] = "Get";
const char delete[] = "Delete";
const char put[] = "Put";
const char patch[] = "Patch";
const char controller[] = "Controller";

static int next() {
  int c;

  c = fgetc(Infile);            // Read from input file
  if ('\n' == c)
    Line++;                     // Increment line count
  return c;
}


static int skip() {
    int c;

    c = next();
    while (' ' == c || '\t' == c || '\n' == c || '\r' == c || '\f' == c) {
        c = next();
    }
    return (c);
}

TrieNode *root;
static TrieNode* extract(int isController, int line) {
    int c;
    int counter = 0;
    char* route = malloc(sizeof(char) * 32);
    c = skip();

    if (c == ')') {
        route[counter] = '.';
        route[counter + 1] = '\0';
        counter++;
        //printf("ROUTE -> %s\n", route);
    }

    while ( c != ')') {
        if (c == '"' || c == '\'') {
            c = skip();
            continue;
        };
        route[counter] = c;
        c = skip();
        counter++;
    }
    route[counter] = '\0';
    printf("ROUTE -> %s\n", route);
    if (isController == 1) {
        root = makeTrieNode(route, 0, line);
    } else {
        if (root == NULL) exit(1);
        root = insertRoute(root, route, line);
    }
    return root;
}

static void scanln() {
    int c;
    char phrase[128];
    int counter = 0;
    int cLine = Line;
    while ((c = skip()) && cLine == Line) {

        if (strcmp(phrase, post) == 0) {
            extract(0, cLine + 1);
        } else if (strcmp(phrase, get) == 0) {
            extract(0, cLine + 1);
        } else if (strcmp(phrase, delete) == 0) {
            extract(0, cLine + 1);
        } else if (strcmp(phrase, put) == 0) {
            extract(0, cLine + 1);
        } else if (strcmp(phrase, patch) == 0) {
            extract(0, cLine + 1);
        } else if (strcmp(phrase, controller) == 0) { 
            extract(1, cLine + 1);
        }
        phrase[counter] = c;
        phrase[counter + 1] = '\0';
        counter++;
    }
}

void triePrint(TrieNode *root) {
    if (!root) return;
    TrieNode *tmp = root;
    printf("%s -> ", tmp->val);

    for (int i = 0; i < MAX_NODES; i++) {
        triePrint(tmp->child[i]);
    }
    printf("\n");

}

void evalTrie(TrieNode* root) {
    TrieNode* tmp = root;
    if (tmp->isLeaf == 1 && tmp->isParam == 1) {
        printf("ERROR: invalid route %s in line %d\n", tmp->val, tmp->line);
        return;
    }
    for (int i = 0; i < MAX_NODES; i++) {
        if (tmp->child[i] != NULL) {
            if (tmp->child[i]->isParam == 1) {
                evalTrie(tmp->child[i]);
            } 
        }
    }
    return;
}

int main(int argc, char *argv[]) {
    argv[1] = "session.controller.ts";
    printf("\nSTART SCANNING: %s\n\n", argv[1]);
    int character;

    Infile = fopen(argv[1], "r");
    if (Infile == NULL) exit(0);

    while ((character = skip()) != EOF) {
        /*DO whatever you need to do for each line here.*/
        // for each line skip spaces
        if (character == '@') {
            scanln();
        }
    
    }
    printf("\nEvalutating trie for %s\n", argv[1]);
    //triePrint(root);
    //if (!feof(Infile)) err(1, "fgetln");
    evalTrie(root);    
    freeTrie(root);
    exit(0);
}

