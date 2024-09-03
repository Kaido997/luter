#include <err.h>    
#include <stdio.h>

int
main(int argc, char *argv[])
{
    printf("START SCANNING: %s\n\n", argv[1]);
    FILE *f;
    size_t len;
    char *line;

    f = fopen(argv[1], "r");
    if (f == NULL)
        return 0;

    while ((line = fgetln(f, &len))) {
                /*DO whatever you need to do for each line here.*/
        if (line[0] == '@') {
            fwrite(line, len, 1, stdout);
        }
    }
    if (!feof(f))
        err(1, "fgetln");

    return 0;
}
