#include <stdio.h>
#include <stdbool.h>
#include "c_diary.h"

static void list_options() {
  printf("Enter a command.\n");
  printf("\nCommands: 'a'=add record, 'd'=display records, 'm'=modify record");
  printf("\n          'r'=remove all records, 'q'=quit\n> ");
}

int main(int argc, char **argv) {
  set_up_database();

  while(!stop) {
    list_options();

    switch(get_command()) {
      case 'a':
        create_record();
        break;
      case 'd':
        display_records();
        break;
      case 'm':
        modify_record();
        break;
      case 'r':
        remove_all_records();
        break;
      case 'q':
        quit();
        break;
      default:
        invalid_command();
        break;
    }
  }
}
