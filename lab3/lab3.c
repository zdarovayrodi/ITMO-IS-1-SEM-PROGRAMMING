#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
    int i;
    int n=128, statusError5xxCounter=0;
    char * string;
    string = (char*)malloc(n*sizeof(char));
    char c;
    FILE * file=fopen("NASA_access_log_Jul95", "r");
    FILE * fout=fopen("result.out", "w");

    while((c = fgetc(file)) != EOF)
    {
        i = 0;
           while ((c!='\n') && (c != EOF))
           {
               if(i>n-1)
            {
                n+=128;
                string = (char*)realloc(string, n*sizeof(char));
            }

            string[i] = c;
            i++;
            c = fgetc(file);
        }
        int requestFirstIndex = 0, requestSecondIndex = 0, index = 0;
        while (requestSecondIndex == 0)
        {
            if ((string[index] == '"') && (requestFirstIndex == 0))
            {
                requestFirstIndex = index+1;
            } else if ((string[index] == '"') && (requestSecondIndex == 0))
            {
                requestSecondIndex = index+1;
            }
            index++;
        }
        if (string[requestSecondIndex+1] == '5')
        {
            statusError5xxCounter++;
            for (int j=requestFirstIndex; j<requestSecondIndex-1; j++)
                fprintf(fout, "%c", string[j]);
            fprintf(fout, "\n");
        }
    }
    free(string);
    fprintf(fout, "Number of request resulting in 5xx response: %d\n", statusError5xxCounter);
    fclose(fout);
    fclose(file);
    return 0;
}
