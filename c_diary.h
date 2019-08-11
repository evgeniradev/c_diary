typedef struct {
  char date[11];
  char text[101];
} RECORD;

FILE *fp;

RECORD temp_record;
RECORD *database;
RECORD *existing_record;

bool stop;
int total_records_number;
char temp_date[11];
char temp_text[101];
char file_path[32];

#define DIR_PATH "./c_diary"
#define FILENAME "c_diary_database.bin"

void set_up_database();
char get_command();
void create_record();
void display_records();
void modify_record();
void remove_all_records();
void quit();
void invalid_command();
