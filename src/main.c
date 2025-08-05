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

    // Allocate memory to hold stdin data
    char *fname = malloc(256);
    char *lname = malloc(256);
    char *age = malloc(256);
    char *add_another = malloc(5); // 5 bytes because the longest answer is "yes" + \n added by enter/return key + null terminator

    // If the memory is still NULL, malloc() failed somehow.
    if (fname == NULL) return NULL;
    if (lname == NULL) return NULL;
    if (age == NULL) return NULL;

    // Prompts and stdin handling
    printf("Enter student first name: ");
    fgets(fname, 255, stdin); // Read 255 bytes from stdin and write it to fname

    printf("Enter student last name: ");
    fgets(lname, 255, stdin); // Write the last name from stdin to lname

    printf("Enter student age: "); // Extra newline to seperate the prompts from the output
    fgets(age, 255, stdin); // Write the age from stdin to age

    printf("\n"); // Print out a newline

    str_slice(fname, " "); str_slice(lname, " "); str_slice(age, " "); // Remove spaces from the name and age   
    str_slice(fname, "\n"); str_slice(lname, "\n"); str_slice(age, "\n"); // Remove all newline characters from the name and age
    int age_int = atoi(age); // Convert age from a string to an integer
    
    // If the length of age isn't 0, or if age_int returns 0 and the inputted age isn't 0
    if (strlen(age) == 0 && strcmp(age, "0") != 0 && age_int == 0) {
        printf("Please input a valid age.\n"); // Print an error message
        return NULL;
    } 
    free(age); // Free the age (separate from other values because we don't need it after this)
    
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
    printf("");
}

void print_all_data() { // Function to print data from the entire file
    FILE *db_file = fopen("database.txt", "a+"); // Open database.txt in read & append mode. If the file does not exist, it is made.
    if (db_file == NULL) { // If the database opening fails
        printf("Database file failed to open."); // Print error message
    };
    
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
        printf("\n"); // Print out 2 newlines
    } fclose(db_file); // Close the database file
}

int main() {
    printf("Student Registry System\n");
    
    char *option = malloc(3); // Allocate 256 bytes of data
    
    while (1) { // While true (runs infinitely)
        printf("Options:\n1 > Print all student data\n2 > Add a new student\n3 > Remove a student\n4 > End the program\n"); // Print commands
        printf("What do you want to do? Input 1, 2, 3, or 4: "); // Print an input prompt
        fgets(option, 3, stdin); // Write 3 bytes from stdin to option (1 number input, 1 \n newline, 1 \0 null terminator)
        str_slice(option, " "); str_slice(option, "\n"); // Remove all spaces and newlines from option

        if (strcmp(option, "1") == 0) print_all_data(); // If the option is 1, print all data
        else if (strcmp(option, "2") == 0) { // If the option is 2
            student *new_student = get_student_data();
            if (new_student == NULL) return 1; // If new_student is 1, there was an error. Therefore, return 1 to indicate error
            
            if (new_student->id > -1) { // If the student's ID is -1 (only occurs when add_student() throws an error)
                printf("New student created!\n");
                printf("Name: %s %s\nAge: %d\nID: %d\n\n", new_student->fname, new_student->lname, new_student->age, new_student->id); // Print the student's info
            }
        } else if (strcmp(option, "3") == 0) { // If the option is 3
            printf("Hai!");
        } else if (strcmp(option, "4") == 0) break; // If the option is 4 break the loop, exiting the program sucessfully
        else { // If else
            printf("Invalid input, please input 1, 2, 3, or 4.");
            return 1;
        }
    }

    printf("Exiting program...");
    return 0; // Return 0, indicating success
}