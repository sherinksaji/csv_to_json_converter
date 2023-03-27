#define MAX_FILENAME_LEN 255
#define MAX_ROW_LENGTH 10000

int is_csv_file(const char *filename);

void read_filename(char *filename);

int read_row(char *line, char ***columns_ptr);

void free_headers(char **headers);

int count_lines(char *filename);

char *remove_last_newline(char *str);

void add_to_JSON(FILE *fp, char **row, int num_columns, int last_row, char **headers);
