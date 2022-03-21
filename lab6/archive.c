#include <stdio.h>
#include <string.h>
#include <stdlib.h>


FILE* create_archive(char* archive_name, int file_count)
{
    FILE* f = fopen(archive_name, "wb");
    fwrite(&file_count, sizeof(int), 1, f);
    // printf("%d", file_count);
    return f;
}

int add_file_to_archive(FILE* archive_name, char* file_name)
{
    FILE* f = fopen(file_name, "rb");

    // ошибка при открытии файла
    if (f == NULL)
        return -1;

    // запись имени файла
    for (unsigned long i=0; i < strlen(file_name); i++)
    {
        char buffer = *(file_name + i);
        fwrite(&buffer, sizeof(char), 1, archive_name);
    }

    // байт конца имени файла
    unsigned char end_of_file = 0xFF;
    fwrite(&end_of_file, sizeof(unsigned char), 1, archive_name);

    // место под длину файла
    char zero_byte = 0x0;
    long pointer_file_size = ftell(archive_name);
    fwrite(&zero_byte, sizeof(unsigned long long), 1, archive_name);

    // подсчет байтов в файле, запись в архив
    unsigned long long bytes_counter = 0;
    char buffer;
    while (fread(&buffer, sizeof(char), 1, f) != 0)
    {
        fwrite(&buffer, sizeof(char), 1, archive_name);
        bytes_counter++;
    }

    // запись длины файла
    //получение указателя на конец файла
    long file_end = ftell(archive_name);
    // перемещение указателя на место для длины файла
    fseek(archive_name, pointer_file_size, SEEK_SET);
    // запись кол-ва байт
    fwrite(&bytes_counter, sizeof(unsigned long long), 1, archive_name);
    // перемещение указателя обратно на конец
    fseek(archive_name, file_end, SEEK_SET);

    return 0;
}

void extract_archive (char* archive_name)
{
    FILE* archive = fopen(archive_name, "rb");

    // кол-во файлов
    int file_count = 0;
    fread(&file_count, sizeof(int), 1, archive);

    // распаковка файлов
    char* buffer_name;
    unsigned long buffer_name_size;
    unsigned char buffer_byte;

    FILE* output_file;
    unsigned long long output_file_size;
    for (int i = 0; i < file_count; i++)
    {
        buffer_name = calloc(1, sizeof(char));
        // *(buffer_name) = '\0';
        buffer_byte = 0;

        fread(&buffer_byte, sizeof(char), 1, archive);
        buffer_name_size = 1;

        // считывание имени файла
        while (buffer_byte != 0xFF)
        {
            realloc(buffer_name, buffer_name_size);
            *(buffer_name + buffer_name_size - 1) = buffer_byte;
            fread(&buffer_byte, sizeof(char), 1, archive);
            buffer_name_size++;
        }
        *(buffer_name + buffer_name_size - 1) = '\0';
        // printf("%s", buffer_name);

        output_file = fopen(buffer_name, "wb");
        // printf("%s", buffer_name);
        free(buffer_name);
        // считываем размер файла
        output_file_size = 0;
        fread(&output_file_size, sizeof(unsigned long long), 1, archive);
        // printf("%lld", output_file_size);
        for (unsigned long long j=0; j<output_file_size; ++j)
        {
            fread(&buffer_byte, sizeof(char), 1, archive);
            fwrite(&buffer_byte, sizeof(char), 1, output_file);
        }
        fclose(output_file);
    }
}

void print_list (char* archive_name)
{
    FILE* archive = fopen(archive_name, "rb");
    if (archive == NULL)
    {
        printf("\"%s\" - archive doesn't exist.\n", archive_name);
        return;
    }

    printf("\"%s\" contains:\n", archive_name);
    int file_count = 0;
    fread(&file_count, sizeof(int), 1, archive);

    unsigned long long buffer_name_size;
    char* buffer_name;
    unsigned char buffer_byte;

    for (int i = 0; i < file_count; i++)
    {
        buffer_name = calloc(1, sizeof(char));
        // *(buffer_name) = '\0';
        buffer_byte = 0;

        fread(&buffer_byte, sizeof(char), 1, archive);
        buffer_name_size = 1;

        // считывание имени файла
        while (buffer_byte != 0xFF)
        {
            realloc(buffer_name, buffer_name_size);
            *(buffer_name + buffer_name_size - 1) = buffer_byte;
            fread(&buffer_byte, sizeof(char), 1, archive);
            buffer_name_size++;
        }
        *(buffer_name + buffer_name_size - 1) = '\0';
        // printf("%s", buffer_name);

        printf("%d. %s\n", i+1, buffer_name);
        free(buffer_name);

        // пропускаем информацию из файла
        unsigned long long file_size=0;
        fread(&file_size, sizeof(unsigned long long), 1, archive);
        fseek(archive, file_size, SEEK_CUR);
    }
}

int main(int argc, char **argv)
{
    char* archive_name;

    if (strcmp(argv[1], "--file") == 0)
    {
        archive_name = malloc(sizeof(argv[2]));
        strcpy(archive_name, argv[2]);
    }
    else
        printf("Invalid usage\n");

    if (strcmp(argv[3], "--list") == 0)
        print_list(archive_name);
    else if (strcmp(argv[3], "--create") == 0)
    {
        FILE* archive = create_archive(archive_name, argc-4); // exe, --file, arc_name, --create
        for (int i = 4; i < argc; ++i)
        {
            add_file_to_archive(archive, argv[i]);
        }
        fclose(archive);
        printf("Archived %d files to \"%s\".\n", argc-4, archive_name);
    }
    else if (strcmp(argv[3], "--extract") == 0)
    {
        extract_archive(archive_name);
        printf("Extracted files from \"%s\".\n", archive_name);
    }
    else
        printf("Invalid usage\n");
    return 0;
}
