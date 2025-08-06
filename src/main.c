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

char *char_to_str(const char character) {
    char *string = malloc(2); // Allocate 2 bytes to hold the character + null terminator
    if (string != NULL) string[0] = character; string[1] = '\0'; // Add the character and a null terminator, creating a string if malloc() is successful (not null)
    return string; // Return a pointer to the string's first item
}

char *get_input(const char *prompt, size_t n) {
    printf("%s", prompt); // Print the prompt
    char *buff = malloc(n); // Allocate n bytes for user input

    if (buff == NULL) { // If the buffer is NULL, malloc() failed
        printf("Failed to get input."); // Print an error message
        return 1; // Return 1
    }

    fgets(buff, n, stdin); // Get n bytes from stdin and write it to buff
    str_slice(buff, " "); str_slice(buff, "\n"); // Remove strings and newlines
    return buff; // Return a pointer to the first character of buff
}

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
    if (student_count >= 50) { // If the student count reaches 50
        printf("Database limit reached."); // Print an error message
        student new_student = {"", "", -1, -1}; // Initialize a student struct with data indicating an error
        return new_student; // Return a struct with the error data
    }
    
    student new_student; // Create a new student struct

    // Assign values. The reason we make duplicates of fname and lname is so that we can transfer ownership of the values to the structure, because we want to free the previously allocated memory containing the values
    new_student.fname = strdup(fname); // Create a duplicate of fname to store in the object
    new_student.lname = strdup(lname); // Create a duplicate of lname to store in the object
    new_student.age = age;
    new_student.id = student_count;

    FILE *db_file = fopen("database.txt", "a"); // Open database.txt in append mode (adds onto the file insetead of overwriting). If the file does not exist, it is made.
    if (db_file == NULL) { // If the database opening fails
        printf("Database file failed to open."); // Print error message
        exit(1); // Exit with code 1
    };

    fprintf(db_file, "%d %s %s %d\n", student_count, fname, lname, age); // Append student data to the database file
    fclose(db_file); // Close the file

    database[student_count] = new_student; // Add the new student to the database
    student_count++; // Increase the student count by 1

    free(fname); free(lname); ; // Free data from the memory (previously allocated dynamically)
    return new_student; // Return the structure's address
}

student *get_student_data() {

    // Get 256 bytes of input for each prompt
    char *fname = get_input("Enter student first name: ", 256);
    char *lname = get_input("Enter student last name: ", 256);
    char *age = get_input("Enter student age: ", 256);

    // If fname, lname, or age is 1 (means get_input() returned 1), return NULL because get_name() only returns 1 on error
    if (fname == 1 || lname == 1 || age == 1) return NULL; 

    printf("\n"); // Print out a newline
    int age_int = atoi(age); // Convert age from a string to an integer
    
    // If the length of age isn't 0, or if age_int returns 0 and the inputted age isn't 0
    if (strlen(age) == 0 && strcmp(age, "0") != 0 && age_int == 0) {
        printf("Please input a valid age.\n"); // Print an error message
        return NULL;
    }
    free(age); // Free the age from memory (separate from other values because we don't need it after this)
    
    // If the first or last name is empty
    if (strlen(fname) == 0) {
        printf("Please input a valid first name."); // Print an error message
        return NULL;
    } else if (strlen(lname) == 0) {
        printf("Please input a valid last name."); // Print an error message
        return NULL;
    }

    student *new_student = malloc(sizeof(student)); // Allocate memory for 1 student
    *new_student = add_student(fname, lname, age_int); // Create a student structure with the given data
    
    return new_student; // Return a pointer to the struct
}

void print_student_data(int id) {
    student stu = database[id]; // Get the student id from the database
    int stu_found = 0; // Integer telling whether the student is found, 0 is false and 1 is true

    if (stu.id == 0) { // If the student's ID isn't found in the program-scoped database (global structs have all their members set to 0 on initialization)
        FILE *db_file = fopen("database.txt", "a+"); // Open database.txt in read & append mode. If the file does not exist, it is made.
        if (db_file == NULL) printf("Database file failed to open."); // If the database opening fails, print error message

        char buff[sizeof(student) + 5]; // Get the size of a student struct, an extra 4 bytes for spaces, and one more for the null terminator
        char *first_item_str = char_to_str(buff[0]); // Make the first item from a buffer into a string
        while (fgets(buff, sizeof(buff), db_file)) {
            buff[strlen(buff) + 1] = '\0'; // Manually assign a null terminator at the end of the string

            // If the first item (as an integer) is the id
            if (atoi(first_item_str) == id) {
                free(first_item_str); // Free the memory char_to_str() dynamically allocates to create the string
                printf("%s", buff); // Print the data
            } else { // If else
                printf("ID was not found in the database."); // Print error msg
                stu_found = 1; // Change stu_found to 1, indicating truthy
            }
        } fclose(db_file); // Close the file stream
    }

    if (stu_found == 1) printf("%s, %s: Age %d, ID %d", stu.lname, stu.fname, stu.age, stu.id); // If the student is found, print the data
}

void print_all_data() { // Function to print data from the entire file
    FILE *db_file = fopen("database.txt", "a+"); // Open database.txt in read & append mode. If the file does not exist, it is made.
    if (db_file == NULL) printf("Database file failed to open."); // If the database opening fails, print error message

    fseek(db_file, 0L, SEEK_END); // Find the end of the file
    long file_size = ftell(db_file); // Get the file size

    if (file_size == 0) { // If the file is empty
        printf("\nYou currently have no entries in your database. Please use 2 (add a new student) in the options menu to create an entry.\n\n");
    }

    char buff[(sizeof(student) * 50) + 1]; // Initialze a buffer that has the same amount of data as 50 students + 1 for the null terminator
    size_t bytes_read; // Number to calculate the amount of bytes read

    rewind(db_file); // Move the cursor back to the start of the file so we can read it
    while ((bytes_read = fread(buff, 1, sizeof(buff) - 1, db_file)) > 0) {
        buff[bytes_read] = '\0'; // Null terminate the buffer so that the compiler knows where to stop reading
        printf("\n%s", buff); // Print the data
        printf("\n"); // Print out a newline
    } fclose(db_file); // Close the database file
}

int main() {
    printf("Student Registry System\n");
    char *option = malloc(3); // Allocate 3 bytes of data (option will be 1 char + \n newline + \0 null terminator)
    
    while (1) { // While true (runs infinitely)
        printf("Options:\n1 > Print all student data\n2 > Add a new student\n3 > Remove a student\n4 > Find a student\n5 > End the program"); // Print commands
        option = get_input("What do you want to do? Input 1, 2, 3, or 4: ", 3); // Get 3 byte input from the inserted prompt

        if (strcmp(option, "1") == 0) print_all_data(); // If the option is 1, print all data
        else if (strcmp(option, "2") == 0) { // If the option is 2
            student *new_student = get_student_data();
            if (new_student == NULL) return 1; // If new_student is 1, there was an error. Therefore, return 1 to indicate error
            
            if (new_student->id > -1) { // If the student's ID is -1 (only occurs when add_student() throws an error)
                printf("New student created!\n");
                printf("Name: %s %s\nAge: %d\nID: %d\n\n", new_student->fname, new_student->lname, new_student->age, new_student->id); // Print the student's info
            }
        } else if (strcmp(option, "3") == 0) { // If the option is 3
            printf("Removing student...");
            /* Function to remove student */
            printf("Student removed!");
        } else if (strcmp(option, "4") == 0) { // If the option is 4
            char *id_str = get_input("Student ID: ", 256); // Get 256 bytes of input for the inserted prompt
            int id = atoi(id_str); // Convert the ID to a string

            // If ID as an integer is 0 (can't input 0 because IDs start at 1), print an error message. If else, find the student with that ID
            id == 0 ? printf("Please input a valid ID number.") : print_student_data(id);
        } else if (strcmp(option, "5") == 0) break; // If the option is 4 break the loop, exiting the program sucessfully
        else { // If else
            printf("Invalid input, please input 1, 2, 3, 4 or 5.");
            return 1;
        }
    }

    printf("Exiting program...");
    return 0; // Return 0, indicating success
}