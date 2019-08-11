#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <regex.h>
#include "c_diary.h"

static void error_and_quit(char custom_message[]) {
  printf("%s", custom_message);
  printf("The programme will now exit.\n");
  exit(1);
}

static int regex_match(const char *str, char *pattern) {
  int status;
  regex_t re;

  if (regcomp(&re, pattern, REG_EXTENDED|REG_NOSUB) != 0)
    error_and_quit("Regex error:\n");

  status = regexec(&re, str, (size_t) 0, NULL, 0);
  regfree(&re);

  if (status != 0)
    return 0;

  return 1;
}

static bool is_date_valid(char date[]) {
  bool result = false;
  char pattern[] =
    "^"
    "[[:digit:]][[:digit:]]-"
    "[[:digit:]][[:digit:]]-"
    "[[:digit:]][[:digit:]][[:digit:]][[:digit:]]"
    "$";

  if (regex_match(date, pattern) == 1)
    result = true;

  return result;
}

static char* read_line(char str[]) {
  int c, i=0;
  c = getchar();

  while (c != '\n' && c != EOF) {
     str[i] = c;
     c = getchar();
     i++;
  }

  str[i] = '\0';
  return str;
}

static bool is_single_char(char str[]) {
  if (str[1] != '\0')
    return false;

  return true;
}

static void set_up_temp_vars_with_user_input() {
  while (strlen(temp_date) == 0 || !is_date_valid(temp_date)) {
    printf("Enter date in the dd-mm-yyyy format:\n");
    read_line(temp_date);

    if (!is_date_valid(temp_date))
      printf("Date '%s' is not of valid format.\n", temp_date);
  }

  while (strlen(temp_text) == 0 || strlen(temp_text) > 100) {
    printf("Enter text(100 characters max):\n");
    read_line(temp_text);

    if (strlen(temp_text) > 100)
      printf("Text is too long.\n");
  }

  strcpy(temp_record.date, temp_date);
  strcpy(temp_record.text, temp_text);
}

static void empty_temp_vars() {
  temp_date[0] = '\0';
  temp_text[0] = '\0';
  temp_record.date[0] = '\0';
  temp_record.text[0] = '\0';
}

static void estimate_total_records_number() {
  total_records_number = ftell(fp) / sizeof(RECORD);
}

static void load_database() {
  fp = fopen(file_path, "rb");

  if (fp == 0)
    error_and_quit("Cannot load database.\n");

  fseek(fp,0,SEEK_END);
  estimate_total_records_number();
  database = realloc(database, total_records_number * sizeof(RECORD));
  fseek(fp, 0, 0);
  fread(database, sizeof(RECORD), total_records_number, fp);
  fclose(fp);
}

static int find_existing_record_location() {
  int location = -1;
  int i;
  load_database();

  for(i=0; i <= total_records_number - 1; i++) {
    if (strcmp(database[i].date, temp_date) == 0) {
      location =  i * sizeof(RECORD);
      break;
    }
  }

  return location;
}

static void create_database() {
  struct stat st = {0};
  bool success = true;

  if (stat(DIR_PATH, &st) == -1)
    if (mkdir(DIR_PATH, 0777) != 0)
      success = false;

  if (success && access(file_path, F_OK ) == -1 ) {
    fp = fopen(file_path, "wb");

    if (fp != 0) {
      success = true;
      printf("Database created successfully.\n");
    }
  } else {
    success = true;
  }

  if (!success)
    error_and_quit("Failed to create database.\n");
}

void set_up_database() {
  strcpy(file_path, DIR_PATH);
  strcat(file_path, "/");
  strcat(file_path, FILENAME);
  create_database();
}

char get_command() {
  static char command[2];
  read_line(command);

  if (!is_single_char(command))
    return 'z';

  return command[0];
}

void create_record() {
  printf("Creating new record...\n");
  set_up_temp_vars_with_user_input();

  if(find_existing_record_location() < 0) {
    fp = fopen(file_path, "ab");

    if (fp == 0)
      error_and_quit("Cannot load database.\n");

    fwrite(&temp_record, sizeof(RECORD), 1, fp);
    fclose(fp);

    printf("Record created successfully.\n");
  } else {
    printf("Record already exists.\n");
  }

  empty_temp_vars();
}

void display_records() {
  int i;
  load_database();

  for(i=0; i <= total_records_number - 1; i++) {
    printf("Date: %s \n", database[i].date);
    printf("Text: %s \n", database[i].text);
    printf("---------\n");
  }

  printf("Total Records: %d \n", total_records_number);
}

void modify_record() {
  set_up_temp_vars_with_user_input();
  int location = find_existing_record_location();

  if (location < 0){
    printf("Record does not exist.\n");
  } else {
    fp = fopen(file_path, "rb+");

    if (fp == 0)
      error_and_quit("Cannot load database.\n");

    fseek(fp, location, SEEK_SET);
    existing_record = realloc(existing_record, sizeof(RECORD));
    fread(existing_record, sizeof(RECORD), 1, fp);

    if (strcmp(existing_record->date, temp_date) != 0)
      error_and_quit("Database error.");

    fseek(fp, location, SEEK_SET);
    fwrite(&temp_record, sizeof(RECORD), 1, fp);
    printf("Record modified successfully.\n");
    fclose(fp);
  }

  empty_temp_vars();
}

void remove_all_records() {
  static char decision[2];
  printf("Are you sure you want to remove all records?\nEnter 'y' or 'n'.\n");
  read_line(decision);

  if (is_single_char(decision) && (decision[0] == 'n' || decision[0] == 'y')) {
    if (decision[0] == 'y') {
      fp = fopen(file_path, "wb");

      if (fp == 0)
        error_and_quit("Cannot load database.\n");

      printf("All records have been removed.\n");
      fclose(fp);
    }
  } else {
    invalid_command();
  }
}

void quit() {
  stop = true;
  printf("Quitting...\n");
}

void invalid_command() {
  printf("Invalid command.\n");
}
