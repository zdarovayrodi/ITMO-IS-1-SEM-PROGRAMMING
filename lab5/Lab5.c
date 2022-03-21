#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include <limits.h>

#pragma pack(1)

typedef struct BitmapFileHeader
{
    char type[2];
    uint32_t size;
    uint16_t reserv1;
    uint16_t reserv2;
    uint32_t offbits;
} file_header;

typedef struct BitmapInfo {
    uint32_t size;
    uint32_t width;
    uint32_t height;
    uint16_t planes;
    uint16_t bitcount;
    uint32_t compression;
    uint32_t imgsize;
    uint32_t pelpermeter1;
    uint32_t pelpermeter2;
    uint32_t clrused;
    uint32_t clrimportant;
} file_info;

struct color
{
    char r;
    char g;
    char b;
};

char* lltostr (long long i)
{
    int len = 2;
    int svi = i;
    while (i > 10)
    {
        len++;
        i = i%10;
    }
    char* num = (char*)malloc(sizeof(char)*len);
    sprintf(num, "%d", svi);
    // printf("%s\n", num);
    return num;
}

void gen_next_name(char* file_path, char* output_path, char** filename, int gen)
{

    if ((gen == 0) || (strlen(lltostr(gen)) > strlen(lltostr(gen - 1))))
    {
        (*filename) = (char*)realloc((*filename), strlen(*filename)+1);
    }

    sprintf((*filename), "%s%d%s", output_path, gen, file_path);
    printf("%s\n", (*filename));
}

int main(int argc, char **argv)
{
    char* file_path;
    char* output_path;
    long long max_iter = LLONG_MAX;
    unsigned long long dump_freq = 1;
    int gen = 0;


    // парсинг аргументов
    // ./a --input a.bmp --output /test/ --max_iter 1 --dump_freq 1
    for (int i=0; i < argc; i++)
    {
        if (strcmp(argv[i], "--input") == 0)
            file_path = argv[i+1];
        else if (strcmp(argv[i], "--output") == 0)
            output_path = argv[i+1];
        else if (strcmp(argv[i], "--max_iter") == 0)
            max_iter = atoi(argv[i+1]);
        else if (strcmp(argv[i], "--dump_freq") == 0)
            dump_freq = atoi(argv[i+1]);
    }

    printf("%s, %s, %lld, %lld\n", file_path, output_path, max_iter, dump_freq);

    char* fin = file_path;
    char *filename = (char*)malloc(sizeof(char)*(strlen(file_path)+strlen(output_path)-1));
    //sprintf(filename, "%s%s", output_path, file_path);


    FILE* input_file = fopen(fin, "rb");
    file_header* fh;
    fh = (file_header*)malloc(sizeof(file_header));
    fread(fh, sizeof(file_header), 1, input_file);

    file_info* fi;
    fi = (file_info*)malloc(sizeof(file_info));
    fread(fi, sizeof(file_info), 1, input_file);

    printf("%d %d %d\n", fi->width, fi->height, fi->bitcount);

    char **matrix_in, **matrix_out;
    matrix_in = (char**) malloc(sizeof(char*) * (fi->height + 2));
    matrix_out = (char**) malloc(sizeof(char*) * (fi->height + 2));
    for (int i = 0; i < fi->height+2; i++)
    {
        matrix_in[i] = (char*) malloc(sizeof(char) * (fi->width+2));
        memset(matrix_in[i], 0, sizeof(char)*(fi->width+2));
        matrix_out[i] = (char*) malloc(sizeof(char) * (fi->width+2));
        memset(matrix_in[i], 0, sizeof(char)*(fi->width+2));
    }

    struct color* fc;
    fc = (struct color *)malloc(sizeof(struct color));

    for (int i = 1; i <= fi->height; i ++)
    {
        for (int j = 1; j <= fi->width; j ++ )
        {
            fread(fc, sizeof(struct color), 1, input_file);
            if (fc->r == 0 & fc->g == 0 & fc->b == 0) matrix_in[i][j] = 1;
            else matrix_in[i][j] = 0;
        }
        // printf("\n");
        // ((ширина изображения * бит + 31 на пиксель) / 32) * 4 может гарантировать, что каждый столбец выровнен по 4 байта, потому что компьютер использует четыре байта за раз для скорости.
        // Выражение ниже показывает, сколько дополнительных байтов в строке
        int k = (int)(((((fi->bitcount * fi->width) + 31) >> 5) << 2) - ((fi->bitcount * fi->width) >> 3));
        // Затем перемещаем указатель файла из этих байтов заполнения
        fseek(input_file, k, SEEK_CUR);
    }


    FILE * fout;

    while (max_iter > 0)
    {
        gen++;
        // printf("%d\n", gen);
        for (int i = 1; i <= fi->height; i++)
        {
            int counter = 0;
            for (int j = 1; j <= fi->width; j++)
                {
                    counter = 0;
                    if (matrix_in[i-1][j-1] == 1) counter++;
                    if (matrix_in[i][j-1] == 1) counter++;
                    if (matrix_in[i-1][j] == 1) counter++;
                    if (matrix_in[i+1][j+1] == 1) counter++;
                    if (matrix_in[i+1][j] == 1) counter++;
                    if (matrix_in[i][j+1] == 1) counter++;
                    if (matrix_in[i+1][j-1] == 1) counter++;
                    if (matrix_in[i-1][j+1] == 1) counter++;

                    if ((counter == 3) && matrix_in[i][j] == 0) matrix_out[i][j] = 1;
                    else if ((counter == 2 || counter == 3) && matrix_in[i][j] == 1) matrix_out[i][j] = 1;
                    else matrix_out[i][j] = 0;
                }
        }
        gen_next_name(file_path, output_path, &filename, gen);

        if (gen%dump_freq == 0)
        {
            fout = fopen(filename, "wb");
            fseek(fout, 0, SEEK_SET);

            // ?
            fh->type[0] = 'B';
            fwrite(fh, sizeof(file_header), 1, fout);
            fwrite(fi, sizeof(file_info), 1, fout);

            for (int i = 1; i <= fi->height; i++)
            {
                for (int j = 1; j <= fi->width; j++)
                    {
                        if (matrix_out[i][j] == 1)
                        {
                            fputc(0, fout);
                            fputc(0, fout);
                            fputc(0, fout);
                        }
                        else
                        {
                            fputc(255, fout);
                            fputc(255, fout);
                            fputc(255, fout);
                        }
                    }
                int k = (int)(((((fi->bitcount * fi->width) + 31) >> 5) << 2) - ((fi->bitcount * fi->width) >> 3));
                for (; k > 0; k--)
                {
                    fputc(0, fout);
                }
            }

        }

        char** temporary = matrix_out;
        matrix_out = matrix_in;
        matrix_in = temporary;

        max_iter--;

    }
    fclose(fout);
    fclose(input_file);

    return 0;
}
