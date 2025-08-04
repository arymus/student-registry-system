#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure to hold student data
typedef struct Student { // Typedef allows you to add an alias to a struct
    char *fname; // Member
    char *lname; // Member
    int age; // Member
    int id; // Member
} student; // Alias
student database[50]; // Create a database to hold an array of student structures
int student_count = 1; // Initialize a variable to track the amount of students (starts at 1 so the first ID is 1)

void str_slice(char *str, const char *substr) { // Function to remove a substring from a string
    char *match; // Pointer to hold the value of strstr()

    /*
    Strstr() finds the first occurence of a substring inside a string and returns a pointer to it. (A pointer to the substring's first item, like all strings)
    If no match is found, NULL is returned.
    In this expression, match becomes the pointer to the substring and is then compared to NULL (match would be NULL if the substr isn't found).
    Strstr() is evaluated first and match becomes its return value first, as denoted by the parentheses around the expression.
    The while loop runs as long as match is NOT NULL, aka as long as the substring is found in the string (this works because we update the string inside the loop)

    I should note that if you were to print match, it would print the entire string starting at the substring's first letter all the way to the end of the string because the string would continue to be read until the compiler hits the null terminator '\0', which is still at the end of the original string.
    */
    while ((match = strstr(str, substr)) != NULL) {

        /*
        Memmove() moves memory one place to another and handles scenarios where memory overlaps, unlike the memcpy() function. (which isn't used here)
        Let's break down the arguments: dest, n, and src respectively.

        Arg 1: dest (destination) defines where the memory is being moved to.
        The destination is match because we want to update its value by moving the substring to it. (this edits the original string because match points to the substring, which is inside the original string)

        Arg 2: src (source) defines where the memory is being moved from.
        The source is match + strlen(substr). This is a form of pointer arithmetic called pointer addition.
        The basic principle is that a pointer to an array's first item (ex. arr) + an index (ex. n) results in the memory address of the item at that index. (ex. arr + n = &arr[n])
        Since match returns the rest of the string, strlen(substr) just traverses the part that is the substring itself, resulting in the rest of the string.
        ex. char arr[] = "Hello world";
        If the substring is "lo", match would return "lo world".
        Then, match + strlen(substr) is just match + 2 since "lo" is 2 characters. (excluding the null terminator)
        So, match + strlen(substr) would result in " world".
        

        Arg 3: n (number) defines the amount of bytes to be moved.
        strlen(match + strlen(substr)) is n. We already defined that match + strlen(substr) is just the remaining string without the substring,
        so getting the strlen() of that would just result in the length of the remaining string.
        */
        memmove(match, match + strlen(substr), strlen(match + strlen(substr)) + 1); // Overwrite match to be the remaining string after the substring 
    }
}

student add_student(char *fname, char *lname, int age) {
    if (student_count >= 50) {
        printf("Database limit reached.");
        student new_student = {"", "", -1, -1}; // Initialize a student struct with data indicating an error
        return new_student;
    }

    student new_student = {fname, lname, age, student_count}; // Create a student struct with the inputted name and age as its members, as well as an id number
    database[student_count] = new_student; // Add the new student to the database
    student_count++; // Increase the student count by 1
    return new_student; // Return the structure's address
}

int main() {
    printf("Student Registry System\n");

    // Allocate memory to hold stdin data
    char *fname = malloc(256);
    char *lname = malloc(256);
    char *age = malloc(256);
    char *add_another = malloc(5); // 5 bytes because the longest answer is "yes" + \n added by enter/return key + null terminator

    // While true (runs infinitely)
    while (1) {

        // Prompts and memory allocation for stdin handling
        printf("Enter student first name: ");
        if (fname == NULL) return 1; // If fname is NULL, malloc() failed. Therefore, return 1 for error
        fgets(fname, 256, stdin); // Read 256 bytes from stdin and write it to fname

        printf("Enter student last name: ");
        if (lname == NULL) return 1; // Return 1 if malloc() fails
        fgets(lname, 256, stdin); // Write the last name from stdin to lname

        printf("Enter student age: "); // Extra newline to seperate the prompts from the output
        if (age == NULL) return 1; // Return 1 if malloc() fails
        fgets(age, 256, stdin); // Write the age from stdin to age

        printf("\n"); // Print out a newline

        str_slice(fname, " "); str_slice(lname, " "); str_slice(age, " "); // Remove spaces from the name and age   
        str_slice(fname, "\n"); str_slice(lname, "\n"); str_slice(age, "\n"); // Remove all newline characters from the name and age

        int age_int = atoi(age); // Convert age from a string to an integer

        // If the length of age isn't 0, or if age_int returns 0 and the inputted age isn't 0
        if (strlen(age) == 0 && strcmp(age, "0") != 0 && age_int == 0) {
            printf("Please input a valid age.\n"); // Print an error message
            return 1; // Return 1, which indicates an error
        } 
        
        if (strlen(fname) == 0) { // If the first or last name is empty
            printf("Please input a valid first name."); // Print an error message
            return 1; // Return 1
        } else if (strlen(lname) == 0) {
            printf("Please input a valid last name."); // Print an error message
            return 1; // Return 1
        }
        
        student new_student = add_student(fname, lname, age_int); // Create a student structure with the given data

        if (new_student.id > -1) { // If the student's ID is -1 (only occurs when add_student() throws an error)
            printf("New student created!\n");
            printf("Name: %s %s\nAge: %d\nID: %d\n", new_student.fname, new_student.lname, new_student.age, new_student.id); // Print the student's info
        }

        printf("\n"); // Print out a newline

        printf("Create another student? (Y/N, default 'N') "); // Continuation prompt
        fgets(add_another, 5, stdin);
        str_slice(add_another, "\n"); str_slice(add_another, " "); // Remove spaces and newlines from the response

        // If the response is "y" or "yes", case-insensitive
        if (strcasecmp(add_another, "y") == 0 || strcasecmp(add_another, "yes") == 0) {
            continue; // Continue the loop
        } else break; // If else, break the loop
    }
    printf("Exiting program...");
    free(fname); free(lname); free(age); free(add_another); // Free the allocated memory
    return 0; // Return 0, indicating success
}