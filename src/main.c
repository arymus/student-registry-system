#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *str_slice(char *str, const char *substr) { // Function to remove a substring from a string
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
    str_slice(name, " "); str_slice(age, " "); // Remove spaces from the name and age (in-place modification)    

    printf("Name: %s\nAge: %s", name, age); // Print the name and age
    
    free(name); free(age); // Free them both from memory
    return 0;
}