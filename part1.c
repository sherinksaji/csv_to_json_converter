#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "part1.h"

int is_csv_file(const char *filename)
{
    size_t len = strlen(filename);
    if (len < 4)
    {
        return 0;
    }
    return strcmp(filename + len - 4, ".csv") == 0;
}

void read_filename(char *filename)
{
    int n;
    do
    {
        printf("Enter filename: ");
        n = scanf("%s", filename);
        if (n == 0)
        {
            printf("Invalid input. Please try again.\n");
            scanf("%*[^\n]");
        }
    } while (n == 0);
}

int read_row(char *line, char ***columns_ptr)
{

    char *token;
    char **columns;
    int num_columns = 0;

    if (line[0] == '\xEF' && line[1] == '\xBB' && line[2] == '\xBF')
    {
        memmove(line, line + 3, strlen(line) - 2);
    }

    token = strtok(line, ",");
    columns = malloc(10 * sizeof(char *));
    while (token != NULL)
    {

        columns[num_columns] = malloc(strlen(token) + 1);
        strcpy(columns[num_columns], token);

        num_columns++;
        token = strtok(NULL, ",");
    }

    *columns_ptr = columns;
    return num_columns;
}

void free_headers(char **headers)
{
    int i = 0;
    for (i = 0; i < 10; i++)
    {
        if (headers[i] != NULL)
        {
            free(headers[i]);
            headers[i] = NULL;

        }

        
    }
    if (headers != NULL)
    {
        free(headers);
        headers = NULL;

    }
}

int count_lines(char *filename)
{
    FILE *fp;

    char buffer[1024];
    int count = 0;

    fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("Error opening file.\n");
        return 0;
    }

    while (fgets(buffer, 1024, fp))
    {
        count++;
    }

    fclose(fp);

    return count;
}

char *remove_last_newline(char *str)
{
    size_t len = strlen(str);


    if (len > 0 && (str[len - 1] == '\n' && str[len - 2] == '\r'))
    {
        str[len - 1] = '\0';
        str[len - 2] = '\0';
    }

    if (len > 0 && (str[len - 1] == '\r' && str[len - 2] == '\n'))
    {
        str[len - 1] = '\0';
        str[len - 2] = '\0';
    }

    if (len > 0 && (str[len - 1] == '\r'))
    {
        str[len - 1] = '\0';
    }

    if (len > 0 && (str[len - 1] == '\n'))
    {
        str[len - 1] = '\0';
    }


    return str;
}

void add_to_JSON(FILE *fp, char **row, int num_columns, int last_row, char **headers)
{
    int i = 0;

    char *end_header;
    char *end_row;

    fprintf(fp, "\t{\n");

    for (i = 0; i < num_columns; i++)
    {

        if (i == num_columns - 1)
        {

            end_header = remove_last_newline(headers[i]);
            end_row = remove_last_newline(row[i]);

            fprintf(fp, "\t\t\"%s\": \"%s\"\n", end_header, end_row);
        }
        else
        {

            fprintf(fp, "\t\t\"%s\": \"%s\",\n", headers[i], row[i]);
        }
    }
    if (last_row)
    {
        fprintf(fp, "\t}\n");
    }
    else
    {
        fprintf(fp, "\t},\n");
    }
}

int main()
{
    char *filename = malloc(MAX_FILENAME_LEN + 1);
    FILE *fp;
    FILE *jsonFp;
    char **headers;
    char ***rows;
    char *line;
    int i = 0;
    int j = 0;

    int num_columns = 0;
    int num_rows = 0;
    int max_line_length = 0;
    char *outputfilename;

    while (1)
    {
        read_filename(filename);
        if (is_csv_file(filename))
        {
            break;
        }
        printf("Invalid filename: must end with .csv\n");
    }

    printf("Opening file: %s\n", filename);

    fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("The file does not exist.\n");
        return 0;
    }
    fclose(fp);

    num_rows = count_lines(filename) - 1;

    fp = fopen(filename, "r");

    max_line_length = MAX_ROW_LENGTH;

    line = malloc(max_line_length + 1);

    fgets(line, max_line_length, fp);

    num_columns = read_row(line, &headers);
    if (num_columns > 10)
    {
        printf("Error:Too many columns.");
        fclose(fp);
        free(line);
        free(filename);
        line = NULL;
        filename = NULL;

        return 0;
    }

    if (num_columns == 0)
    {
        printf("Error: unable to read headers\n");
        fclose(fp);
        free(line);
        free(filename);
        line = NULL;
        filename = NULL;

        return 0;
    }


    rows = malloc(num_rows * sizeof(char **));

    for (i = 0; i < num_rows; i++)
    {

        fgets(line, max_line_length, fp);

        read_row(line, &rows[i]);
    }

    fclose(fp);


    outputfilename = strtok(filename, ".");
    strcat(outputfilename, ".json");

    jsonFp = fopen(outputfilename, "w");
    fprintf(jsonFp, "[\n");

    for (i = 0; i < num_rows; i++)
    {
        if (i == num_rows - 1)
        {
            add_to_JSON(jsonFp, rows[i], num_columns, 1, headers);
        }
        else
        {
            add_to_JSON(jsonFp, rows[i], num_columns, 0, headers);
        }
    }
    fprintf(jsonFp, "]\n");

    fclose(jsonFp);


    /* free malloced memories*/
    if (line != NULL)
    {
        free(line);
        line = NULL;
    }

    if(filename != NULL)
    {
        free(filename);
        filename = NULL;
    }

    free_headers (headers);

    for (i = 0; i < num_rows; i++){
        for (j = 0; j < num_columns; j++) {
            if (rows[i][j] != NULL)
            {
                free (rows[i][j]);
                rows[i][j] = NULL;
            }
        }
        if(rows[i] != NULL)
        {
            free(rows[i]);
            rows[i] = NULL;
        }

    }
    if(rows != NULL)
    {
        free(rows);
        rows = NULL;
    }


    printf("Done\n");
    return 0;
}
