#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    printf("Student Registry System\n");

    printf("Enter student name: ");
    char *name = malloc(256); // Allocate 256 bytes of memory for the name
    if (name == NULL) return 1; // If name is NULL, malloc() failed. Therefore, return 1 for error
    fgets(name, 256, stdin); // Read 256 bytes from stdin and write it to buff

    printf("Enter student age: ");
    char *age = malloc(256); // Allocate 256 bytes of memory for the age
    if (age == NULL) return 1; // Return 1 if malloc() fails
    fgets(age, 256, stdin); // Write the age from stdin

    /*
    Strcspn() finds a substring inside a string and returns the length of the string up to that point (the index of the character).
    We use this function to get the index of the newline that's appended when the user hits enter or return in stdin.
    This character is then updated to be the null terminator (because \n will always be at the end, since its triggered by the enture/return key)
    */
    name[strcspn(name, "\n")] = '\0';
    age[strcspn(age, "\n")] = '\0';

    printf("Name: %s\nAge: %s", name, age); // Print the name and age

    free(name); free(age); // Free them both from memory
    return 0;
}