#include <err.h>    
#include <stdio.h>
#include <string.h>

FILE *Infile;
int Line;
char post[] = "Post";
char get[] = "Get";
char delete[] = "Delete";
char put[] = "Put";
char patch[] = "Patch";
char controller[] = "Controller";

static int next() {
  int c;

  c = fgetc(Infile);            // Read from input file
  if ('\n' == c)
    Line++;                     // Increment line count
  return c;
}


int skip() {
    int c;

    c = next();
    while (' ' == c || '\t' == c || '\n' == c || '\r' == c || '\f' == c) {
        c = next();
    }
    return (c);
}

int routext() {
    int c;
    char route[32];
    int counter = 0;
    //printf("Start: %c", c);
    c = skip();
    if (c == ')') {
        route[counter] = '.';
        route[counter + 1] = '\0';
    }
    while ( c != ')') {
        if (c == '"' || c == '\'') {
            c = skip();
            continue;
        };
        route[counter] = c;
        route[counter + 1] = '\0';
        c = skip();
        counter++;
    }
    printf(" Route -> %s", route);
    return 0;

}

int scanln() {
    int c;
    char phrase[128];
    
    int counter = 0;
    int cLine = Line;
    //printf("Line: %d\t", Line);
    while ((c = skip()) && cLine == Line) {
        //printf("%c", c);
        if (strcmp(phrase, post) == 0 ||
            strcmp(phrase, get) == 0 ||
            strcmp(phrase, delete) == 0 ||
            strcmp(phrase, put) == 0 ||
            strcmp(phrase, patch) == 0 ||
            strcmp(phrase, controller) == 0 
        ) {
            printf("FINDED ->\t%s", phrase);
            routext();
        }
        phrase[counter] = c;
        phrase[counter + 1] = '\0';
        counter++;
    }
    return 0;    
}

int main(int argc, char *argv[]) {
    
    printf("\nSTART SCANNING: %s\n\n", argv[1]);
    size_t len;
    int character;
    char *check;

    Infile = fopen(argv[1], "r");
    if (Infile == NULL)
        return 0;

    while ((character = skip()) != EOF) {
        /*DO whatever you need to do for each line here.*/
        // for each line skip spaces
        if (character == '@') {
            printf("\n");
            scanln(); 
            //printf("%c", character);
        }
    
    }
    if (!feof(Infile))
        err(1, "fgetln");

    return 0;
}
