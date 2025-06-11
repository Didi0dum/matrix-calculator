#include <stdio.h>
#include <stdlib.h>

#include "headers/crypt.h"
#include "headers/io.h"
#include "headers/matrix.h"

void show_menu() {
  printf("\n=== MENU ===\n");
  printf("1. Enter a new matrix\n");
  printf("2. Show current matrix\n");
  printf("3. Multiply by scalar\n");
  printf("4. Save to plain file\n");
  printf("5. Save to encrypted file\n");
  printf("6. Load from plain file\n");
  printf("7. Load from encrypted file\n");
  printf("8. Exit\n");
  printf("Choose an option: ");
}

int main() {
  Matrix *current = NULL;
  int choice;

  char filename[256];
  char keyfile[256];
  double scalar;

  while (1) {
    show_menu();
    if (scanf("%d", &choice) != 1) {
      fprintf(stderr, "Invalid input. Exiting.\n");
      break;
    }

    switch (choice) {
    case 1:
      if (current) {
        free_matrix(current);
      }
      printf("Enter a new matrix:\n");
      current = input_matrix();
      break;

    case 2:
      if (current) {
        printf("Current matrix:\n");
        print_matrix(current);
      } else {
        printf("No matrix loaded.\n");
      }
      break;

    case 3:
      if (!current) {
        printf("No matrix loaded.\n");
        break;
      }
      printf("Enter scalar: ");
      scanf("%lf", &scalar);
      multiply_by_scalar(current, scalar);
      printf("Matrix after multiplication:\n");
      print_matrix(current);
      break;

    case 4:
      if (!current) {
        printf("No matrix to save.\n");
        break;
      }
      printf("Enter filename to save to: ");
      scanf("%s", filename);
      save_matrix(current, filename);
      printf("Saved to '%s'.\n", filename);
      break;

    case 5:
      if (!current) {
        printf("No matrix to save.\n");
        break;
      }
      printf("Enter encrypted output filename: ");
      scanf("%s", filename);
      printf("Enter key file name (will be generated): ");
      scanf("%s", keyfile);
      generate_key(keyfile);
      save_matrix_enctypted(current, filename, keyfile);
      printf("Encrypted and saved to '%s' with key '%s'.\n", filename, keyfile);
      break;

    case 6:
      if (current) {
        free_matrix(current);
      }
      printf("Enter filename to load from: ");
      scanf("%s", filename);
      current = load_matrix(filename);
      if (current)
        printf("Matrix loaded successfully.\n");
      else
        printf("Failed to load matrix.\n");
      break;

    case 7:
      if (current) {
        free_matrix(current);
      }
      printf("Enter encrypted input filename: ");
      scanf("%s", filename);
      printf("Enter key file name: ");
      scanf("%s", keyfile);
      current = load_matrix_encrypted(filename, keyfile);
      if (current)
        printf("Matrix decrypted and loaded successfully.\n");
      else
        printf("Decryption or loading failed.\n");
      break;

    case 8:
      printf("Exiting.\n");
      if (current)
        free_matrix(current);
      return 0;

    default:
      printf("Invalid choice.\n");
      break;
    }
  }

  return 0;
}
