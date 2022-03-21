#include <stdio.h>
#include <string.h>
#include <ctype.h>
#define IN 1 /* внутри слова */
#define OUT 2 /* вне слова */

int main(int argc, char **argv) {
	FILE *file = fopen(argv[1], "r");

	/* открыть не удалось */
    if (file == NULL) {
       printf("Не удалось открыть файл \"%s\".\n", argv[1]);
       return 0;
    }

	long long counter = 0;
	int str;
	int flag = 0;

	/* посчитать строки */
	if (strcmp(argv[2], "-l") == 0 || strcmp(argv[2], "--lines") == 0) {
		while ((str = (fgetc(file))) != EOF) {
			if (flag == 0) {
				counter++;
				flag = 1;
			} else if (str == '\n') {
				flag = 0;
			}
		}
      printf("Lines: ");
	}

	/* посчитать слова */
	if (strcmp(argv[2], "-w") == 0 || strcmp(argv[2], "--words") == 0) {
		unsigned short int status = OUT;
		while ((str = (fgetc(file))) != EOF) {
    		if (status == OUT && str != ' ' && str != '\n' && str != '\t') {
    			counter++;
    			status = IN;
    		}
    		else if (str == ' ' || str == '\n' || str == '\t') {
    			status = OUT;
        	}
      }
      printf("Words: ");
	}

	/* посчитать байты (все символы) */
	if (strcmp(argv[2], "-c") == 0 || strcmp(argv[2], "--bytes") == 0) {
      while ((str = (fgetc(file))) != EOF) {
          counter++;
      }
	printf("Bytes: ");
	}


	printf("%lld\n", counter);
	return 0;
}
