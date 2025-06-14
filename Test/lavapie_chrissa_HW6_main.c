/**************************************************************
* Class::  CSC-415-01 Spring 2025
* Name:: Chrissa Lavapie
* Student ID:: 922488460
* GitHub-Name:: chrissalavapie
* Project:: Assignment 6 – Device Driver
*
* File:: lavapie_chrissa_HW6_main.c
*
* Description:: My device driver will be a translator from English
to Tagalog. When a user writes something in English, using the write()
function, it will be translated into Tagalog, or it can ask for a specific
word and get its translation. Then using the read() function, the user space
will read the translation and print it out for the user.
*
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <sys/ioctl.h>

#define DEVICE_PATH "/dev/translator"
#define MAX_WORD_LENGTH 50
#define IOCTL_SET_MODE _IOW('a', 1, int)

enum translateMode
{
    AUTO = 0,   // 0 - detects language
    ENG_TO_TAG, // 1 - English to Tagalog
    TAG_TO_ENG, // 2 - Tagalog to English
};

// asking user what translation mode they want
// returns the mode
enum translateMode setTranslateMode(int fd)
{
    int mode;
    printf("\nChoose a translation mode: \n");
    printf("[1] - English to Tagalog\n");
    printf("[2] - Tagalog to English\n");
    printf("[0] - Detects Language\n");
    printf("Enter choice translation mode: ");
    scanf("%d", &mode);
    getchar(); // consume new line

    // if invalid input is put exit program
    if (mode != ENG_TO_TAG && mode != TAG_TO_ENG && mode != AUTO)
    {
        printf("Invalid input. Exiting...");
        {
            close(fd);
            return -1;
        }
    }

    // set the mode through ioctl
    if (ioctl(fd, IOCTL_SET_MODE, mode) == -1)
    {
        printf("Ioctl operation failed.");
        close(fd);
        return -1;
    }
    else
    {
        printf("Translsation mode set.\n\n");
    }

    return mode;
}

int main()
{
    int fd;
    char input[MAX_WORD_LENGTH];
    char output[MAX_WORD_LENGTH];
    char contInput[MAX_WORD_LENGTH];
    bool restart = true; // if the user wants to keep running the program

    // open the device
    fd = open(DEVICE_PATH, O_RDWR);
    if (fd == -1)
    {
        printf("Failed to open device");
        return -1;
    }

    // obtain the translation mode to print the right word bank
    int mode = setTranslateMode(fd);

    // error with Ioctl
    if (mode == -1)
    {
        return -1;
    }

    // if the mode is english to tagalog or auto
    if (mode == 1 || mode == 0)
    {
        printf("\n\n------------------Tagalog Translator ------------------\n\n");

        printf("Word bank of available English words and phrases: \n");
        printf("-------------------------------------------------------\n");
        printf("aunt              | i'm full        | thank you        \n");
        printf("car               | i'm hungry      | that             \n");
        printf("city              | i'm sleepy      | this             \n");
        printf("congratulations   | i'm sorry       | thursday         \n");
        printf("cousin            | is              | today            \n");
        printf("dad               | kitchen         | tomorrow         \n");
        printf("delicious         | later           | tuesday          \n");
        printf("eat               | let's eat now   | uncle            \n");
        printf("family            | mom             | water            \n");
        printf("food              | monday          | wednesday        \n");
        printf("friday            | my name is      | what             \n");
        printf("friend            | no              | what is that     \n");
        printf("good afternoon    | okay            | what is this     \n");
        printf("good morning      | older brother   | what time is it  \n");
        printf("goodnight         | older sister    | what's your name \n");
        printf("grandma           | pants           | who is that      \n");
        printf("grandpa           | please          | yes              \n");
        printf("happy birthday    | restroom        | yesterday        \n");
        printf("hello             | saturday        | you're welcome   \n");
        printf("house             | school          | younger sibling  \n");
        printf("how are you       | shirt           |                  \n");
        printf("i don't know      | shoes           |                  \n");
        printf("i like this       | student         |                  \n");
        printf("i love you        | sunday          |                  \n");
        printf("i'm fine          | take care       |                  \n\n");
    }
    // if the mode if tagalog to english
    else if (mode == 2)
    {
        printf("\n\n------------------English Translator ------------------\n\n");

        printf("Word bank of available Tagalog words and phrases: \n");
        printf("-------------------------------------------------------\n");
        printf("ading              | juevos          | pamilya     \n");
        printf("akaibigan          | kahapon         | pantalon    \n");
        printf("ang pangalan ko    | kaibigan        | pasenya na  \n");
        printf("ang pangalan mo    | kain na         | pinsan      \n");
        printf("ano to             | kain            | sabado      \n");
        printf("ano yan            | kamusta ka      | salamat     \n");
        printf("ano                | kamusta         | sapatos     \n");
        printf("ating              | kuya            | sarap       \n");
        printf("ay                 | linggo          | sige        \n");
        printf("bahay              | lola            | sino yan    \n");
        printf("banyo              | lunes           | tatay       \n");
        printf("bukas              | manbuti         | tita        \n");
        printf("busog ako          | magandang gabi  | tito        \n");
        printf("camiseta           | magandang umaga | tubig       \n");
        printf("coche              | maligayang bati | wala anuman \n");
        printf("cocina             | mamaya          | lungsod     \n");
        printf("estudiante         | mangandang hapon| mahal kita  \n");
        printf("gusto ko 'to       | martes          | ate         \n");
        printf("gutom ako          | miercoles       | ngayon      \n");
        printf("hindi ko alam      | nanay           | anong oras  \n");
        printf("hindi              | oo              |             \n");
        printf("inaantok           | paaralan        |             \n");
        printf("ingat              | pagkain         |             \n");
        printf("ito                | pagpupugay      |             \n");
        printf("iyon               | pakiusap        |             \n\n");
    }

    // while user wants to continue running program
    while (restart)
    {
        // obtains user word
        printf("\nEnter a word or phrase to translate: ");
        fgets(input, MAX_WORD_LENGTH, stdin);
        input[strcspn(input, "\n")] = '\0'; // remove trailing newline

        // convert input to lowercase
        for (int i = 0; input[i]; i++)
        {
            input[i] = tolower((unsigned char)input[i]);
        }

        // write the input to the translator device
        if (write(fd, input, strlen(input)) == -1)
        {
            printf("Write failed.");
        }

        // reads the translated word back into user space
        ssize_t bytesRead = read(fd, output, MAX_WORD_LENGTH - 1);
        if (bytesRead >= 0)
        {
            output[bytesRead] = '\0';
            printf("Translation: %s\n", output);
        }
        else
        {
            perror("Read failed");
        }

        // Asks the user if they wanna keep using the app
        printf("\nEnter \"CONTINUE\" to continue translating. Enter \"EXIT\" to exit:");
        fgets(contInput, MAX_WORD_LENGTH, stdin);
        contInput[strcspn(contInput, "\n")] = '\0'; // remove trailing newline

        // if the user types exit, exit the program
        if (strcmp("EXIT", contInput) == 0)
        {
            restart = false;
        }
        // if the uset types continue, reprompt them
        else if (strcmp("CONTINUE", contInput) == 0)
        {
            continue;
        }
        // if anything else is enter, exit the program
        else
        {
            printf("Invalid input.");
            restart = false;
        }
    }

    printf("Exiting...\n");

    close(fd);
    return 0;
}
