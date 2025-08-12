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
int student_count; // Initialize a variable to track the amount of students

char *char_to_str(const char character) {
    char *string = malloc(2); // Allocate 2 bytes to hold the character + null terminator
    if (string != NULL) string[0] = character; string[1] = '\0'; // Add the character and a null terminator, creating a string if malloc() is successful (not null)
    return string; // Return a pointer to the string's first item
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

char *get_input(const char *prompt, size_t n) {
    printf("%s", prompt); // Print the prompt
    char *buff = malloc(n); // Allocate n bytes for user input

    if (buff == NULL) { // If the buffer is NULL, malloc() failed
        printf("Failed to get input."); // Print an error message
        return NULL; // Return NULL
    }

    fgets(buff, n, stdin); // Get n bytes from stdin and write it to buff
    str_slice(buff, " "); str_slice(buff, "\n"); // Remove strings and newlines
    return buff; // Return a pointer to the first character of buff
}

void update_external_student_count() {
    FILE *db_file = fopen("database.txt", "r"); // Open the database file with read permissions
    if (db_file == NULL) return; // If db_copy is NULL, end the function (NULL means the fopen() had an error)

    char student_count_str[3]; // Initialize a string to store a 2-digit number at most + 1 null terminator
    if (fgets(student_count_str, sizeof(student_count_str), db_file) == NULL) {
        fclose(db_file); // Close the file stream to db_file
        return; // End the function
    }

    char buff[(sizeof(student) * 50) + 1]; // Large enough buffer for up to 50 students + null terminator
    size_t offset = 0; // Initialize a number to store the number of bytes off from the cursor's current location
    size_t bytes_read; // Initialize a number to store the number of bytes read from the file into buff

    /*
    Constantly get sizeof(buff) - offset bytes (the remaining space left in buff) from db_file,
    and write it to buff + offset, which is just buff[current index + offset].
    This happens until fgets() returns NULL, indicating the end of the file.
    */
    while (fgets(buff + offset, sizeof(buff) - offset, db_file) != NULL) {
        offset = strlen(buff); // Update offset
    } fclose(db_file); // Close the file stream to the database file

    FILE *db_copy = fopen("db_copy.txt", "w"); // Open db_copy with write permissions
    if (db_copy == NULL) return; // If db_copy is NULL, end the function (NULL means the fopen() had an error)

    fprintf(db_copy, "%d\n%s", student_count, buff); // Write new count and old data
    fclose(db_copy); // Close the file stream to the database copy

    // Remove the original database file and rename the copy to database.txt (returns 0 on error)
    if (remove("database.txt") != 0 || rename("db_copy.txt", "database.txt") != 0) printf("File update failed"); // Print error
}

student add_student(char *fname, char *lname, int age) {
    if (student_count >= 50) { // If the student count reaches 50
        printf("Database limit reached."); // Print an error message
        student new_student = {"", "", 0, 0}; // Initialize a student struct with data indicating an error
        return new_student; // Return a struct with the error data
    }
    
    student new_student; // Create a new student struct

    // Assign values. The reason we make duplicates of fname and lname is so that we can transfer ownership of the values to the structure, because we want to free the previously allocated memory containing the values
    new_student.fname = strdup(fname); // Create a duplicate of fname to store in the object
    new_student.lname = strdup(lname); // Create a duplicate of lname to store in the object
    free(fname); free(lname); // Free data from the memory (previously allocated dynamically)
    new_student.age = age;
    new_student.id = student_count;

    FILE *db_file = fopen("database.txt", "a"); // Open database.txt in append mode (adds onto the file insetead of overwriting). If the file does not exist, it is made.
    if (db_file == NULL) { // If the database opening fails
        printf("Database file failed to open."); // Print error message
        exit(1); // Exit with code 1
    };

    fprintf(db_file, "%d %s %s %d\n", student_count, new_student.fname, new_student.lname, new_student.age); // Append student data to the database file
    fflush(db_file); // Automatically add contents to db_file instead of holding it in a buffer
    fclose(db_file); // Close the file

    database[student_count] = new_student; // Add the new student to the database
    student_count++; // Increase the student count by 1
    update_external_student_count(); // Update the student count in the database file
    return new_student; // Return the structure's address
}

student *get_student_data() {

    // Get 256 bytes of input for each prompt
    char *fname = get_input("Enter student first name: ", 256);
    char *lname = get_input("Enter student last name: ", 256);
    char *age = get_input("Enter student age: ", 256);

    // If fname, lname, or age is NULL (means get_input() returned NULL), return NULL because get_name() only returns NULL on error
    if (fname == NULL || lname == NULL || age == NULL) return NULL; 

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
    FILE *db_file = fopen("database.txt", "r"); // Open database.txt with read permissions
    fclose(db_file); // Close the file stream to db_file
}

void print_all_data() { // Function to print data from the entire file
    FILE *db_file = fopen("database.txt", "a+"); // Open database.txt in read & append mode. If the file does not exist, it is made.
    if (db_file == NULL) printf("Database file failed to open."); // If the database opening fails, print error message

    fseek(db_file, 0L, SEEK_END); // Find the end of the file
    long file_size = ftell(db_file); // Get the file size
    char empty_db[] = "\nYou currently have no entries in your database. Please use 2 (add a new student) in the options menu to create an entry.\n\n";
    
    if (file_size == 0) printf("%s", empty_db);

    char *student_count_str = malloc(3); // Allocate 3 bytes (can have at most 50 students, aka 2 digits + 1 null terminator) to store the student count read from the database file 
    char buff[(sizeof(student) * 50) + 1]; // Initialze a buffer that has the same amount of data as 50 students + 1 for the null terminator

    rewind(db_file); // Move the file cursor to the start of the file
    fscanf(db_file, "%[^\n]", student_count_str); // Read until a newline is hit, and store that data in student_count_str
    fseek(db_file, strlen(student_count_str) + 1, SEEK_SET); // Move the cursor to the space after the student_count, skipping both count and the newline

    /*
    fread() is a function that reads an amount of bytes from a file and returns the number of elements that are successfully read.
    Here, we write sizeof(buff) - 1 bytes (50 students worth of data) to buff from db_file, defining each item's size as 1
    */
    size_t bytes_read = fread(buff, 1, sizeof(buff) - 1, db_file); // Number to calculate the amount of bytes read

    buff[bytes_read] = '\0'; // Null terminate the buffer so that the compiler knows where to stop reading
    if (bytes_read == 0) printf("%s", empty_db);
    else { // If the bytes read isn't 0 (there's data to be read)

        fseek(db_file, strlen(student_count_str) + 1, SEEK_SET); // Move the cursor back to where it started

        // fread() automatically progresses the cursor, so it has to run in a while loop until it returns 0, indicating the end of the file
        while ((bytes_read = fread(buff, 1, sizeof(buff) - 1, db_file)) > 0) printf("\n%s\n", buff); // Print the data        
    } fclose(db_file); // Close the database file
    free(student_count_str); // Free student_count_str from memory 
}

int main() {
    printf("Student Registry System\n");
    FILE *db_test = fopen("database.txt", "r"); // Open database.txt with read permission
    if (db_test == NULL) { // If database is NULL, the file doesn't exist
        printf("Database does not exist. Creating new database...\n");
        student_count = 1; // Set the count of students to 1 so ID creation starts at 1 (allows us to use 0 for error checking)

        FILE *db_file = fopen("database.txt", "w+"); // Open the file with read + write permissions, this time the file is created if it doesn't exist
        if (db_file == NULL) {
            printf("Database creation failed."); // Print an error message
            return 1;
        }

        fprintf(db_file, "%d\n", student_count); // Write the student count to the file
        fclose(db_file); // Close the file stream
        printf("Database created successfully!\n\n"); // Print a success message
    } else { // If else (file does exist)
        char *external_student_count = malloc(3); // Allocate 3 bytes (can have at most 50 students, aka 2 digits + 1 null terminator) to store the student count read from the database file
        fscanf(db_test, "%[^\n]", external_student_count); // Scan the file until a newline is encountered
        fclose(db_test); // Close the file stream
        student_count = atoi(external_student_count); // Convert the student count read in the database to an integer and assign it to student_count
        printf("Student count: %d\n", student_count - 1); // Subtract 1 b/c the database stores the student count + 1 so the program can start the ID assignment as the next number after the student count

        if (student_count == 0) { // Student count will never be 0, but atoi() returns 0 if it fails
            student_count = 1; // Change student count to 1
            printf("Retrieving external student count failed. Student count set to 1."); // Print warning message
        }
    }

    char *option = malloc(3); // Allocate 3 bytes of data (option will be 1 char + \n newline + \0 null terminator)
    while (1) { // While true (runs infinitely)
        printf("Options:\n1 > Print all student data\n2 > Add a new student\n3 > Remove a student\n4 > Find a student\n5 > End the program\n"); // Print commands
        option = get_input("What do you want to do? Input 1, 2, 3, 4, or 5: ", 3); // Get 3 byte input from the inserted prompt

        if (strcmp(option, "1") == 0) print_all_data(); // If the option is 1, print all data
        else if (strcmp(option, "2") == 0) { // If the option is 2
            student *new_student = get_student_data(); // Take input for student data and add it to the database
            if (new_student == NULL) return 1; // If new_student is 1, there was an error. Therefore, return 1 to indicate error
            
            if (new_student->id != 0) { // If the student's ID is NOT 0 (only occurs when add_student() throws an error)
                printf("New student created!\n");
                printf("Name: %s %s\nAge: %d\nID: %d\n\n", new_student->fname, new_student->lname, new_student->age, new_student->id); // Print the student's info
            }
        } else if (strcmp(option, "3") == 0) { // If the option is 3
            printf("\nRemoving student...\n");
            /* Function to remove student */
            update_external_student_count(); // Update the student count in the database file
            printf("Student removed!\n\n");
        } else if (strcmp(option, "4") == 0) { // If the option is 4
            char *id_str = get_input("Student ID: ", 256); // Get 256 bytes of input for the inserted prompt
            int id = atoi(id_str); // Convert the ID to a string

            // If ID as an integer is 0 (can't input 0 because IDs start at 1), print an error message. If else, find the student with that ID
            id == 0 ? printf("\nPlease input a valid ID number.\n\n") : print_student_data(id);
        } else if (strcmp(option, "5") == 0) break; // If the option is 4 break the loop, exiting the program sucessfully
        else printf("\nInvalid input, please input 1, 2, 3, 4 or 5.\n\n");
    }

    free(option); // Free the option from memory
    printf("Exiting program...");
    return 0; // Return 0, indicating success
}