/**************************************************************
* Class::  CSC-415-01 Spring 2025
* Name:: Chrissa Lavapie
* Student ID:: 922488460
* GitHub-Name:: chrissalavapie
* Project:: Assignment 6 – Device Driver
*
* File:: translator.c
*
* Description:: My device driver will be a translator from
English to Tagalog. When a user writes something in English,
using the write() function, it will be translated into Tagalog,
or it can ask for a specific word and get its translation. Then
using the read() function, the user space will read the
translation and print it out for the user.
*
**************************************************************/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/string.h>
#include <linux/ioctl.h>

#define DEVICE_NAME "translator"
#define MAX_WORD_LENGTH 50
#define DICTIONARY_LENGTH 70
#define IOCTL_SET_MODE _IOW('a', 1, int)

enum translateMode
{
    AUTO = 0,   // 0
    ENG_TO_TAG, // 1 - English to Tagalog
    TAG_TO_ENG, // 2 - Tagalog to English
};

// auto detects the language
static enum translateMode currMode = AUTO;

// word pair struct
struct translatedEntry
{
    char english[MAX_WORD_LENGTH];
    char tagalog[MAX_WORD_LENGTH];
};

// final translated word per write
static char translatedWord[MAX_WORD_LENGTH] = "";

// dictionary with 70 translated words (english, tagalog)
static struct translatedEntry dict[DICTIONARY_LENGTH] = {
    {"aunt", "tita"},
    {"car", "coche"},
    {"city", "lungsod"},
    {"congratulations", "pagpupugay"},
    {"cousin", "pinsan"},
    {"dad", "tatay"},
    {"delicious", "sarap"},
    {"eat", "kain"},
    {"family", "pamilya"},
    {"food", "pagkain"},
    {"friday", "viernes"},
    {"friend", "kaibigan"},
    {"good afternoon", "mangandang hapon"},
    {"good morning", "magandang umaga"},
    {"goodnight", "magandang gabi"},
    {"grandma", "lola"},
    {"grandpa", "lolo"},
    {"happy birthday", "maligayang bati"},
    {"hello", "kamusta"},
    {"house", "bahay"},
    {"how are you", "kamusta ka"},
    {"i don't know", "hindi ko alam"},
    {"i like this", "gusto ko 'to"},
    {"i love you", "mahal kita"},
    {"i'm fine", "mabuti"},
    {"i'm full", "busog ako"},
    {"i'm hungry", "gutom ako"},
    {"i'm sleepy", "inaantok"},
    {"i'm sorry", "pasensya na"},
    {"is", "ay"},
    {"kitchen", "cocina"},
    {"later", "mamaya"},
    {"let's eat now", "kain na"},
    {"mom", "nanay"},
    {"monday", "lunes"},
    {"my name is", "ang pangalan ko"},
    {"no", "hindi"},
    {"okay", "sige"},
    {"older brother", "kuya"},
    {"older sister", "ate"},
    {"pants", "pantalon"},
    {"please", "pakiusap"},
    {"restroom", "banyo"},
    {"saturday", "sabado"},
    {"school", "paaralan"},
    {"shirt", "camiseta"},
    {"shoes", "sapatos"},
    {"student", "estudiante"},
    {"sunday", "linggo"},
    {"take care", "ingat"},
    {"thank you", "salamat"},
    {"that", "iyon"},
    {"this", "ito"},
    {"thursday", "jueves"},
    {"today", "ngayon"},
    {"tomorrow", "bukas"},
    {"tuesday", "martes"},
    {"uncle", "tito"},
    {"water", "tubig"},
    {"wednesday", "miercoles"},
    {"what", "ano"},
    {"what is that", "ano yan"},
    {"what is this", "ano to"},
    {"what time is it", "anong oras"},
    {"what's your name", "ano pangalan mo"},
    {"who is that", "sino yan"},
    {"yes", "oo"},
    {"yesterday", "kahapon"},
    {"you're welcome", "walang anuman"},
    {"younger sibling", "ading"},
};

// translate function
static const char *translate(const char *input)
{
    for (int i = 0; i < DICTIONARY_LENGTH; i++)
    {
        // traverses through array until it finds the matching english word
        // if the user sets it to english -> tagalog mode or auto
        if (currMode == ENG_TO_TAG || currMode == AUTO)
        {
            if (strcmp(input, dict[i].english) == 0)
            {
                // returns the corresponding tagalog word
                return dict[i].tagalog;
            }
        }

        // if the user sets it to tagalog -> english mode or auto
        if (currMode == TAG_TO_ENG || currMode == AUTO)
        {
            // traverses through array until it finds the matching tagalog word
            if (strcmp(input, dict[i].tagalog) == 0)
            {
                // returns the corresponding english word
                return dict[i].english;
            }
        }
    }
    // if the word or phrase was not found
    return "unknown";
}

// opening the translator
static int translatorOpen(struct inode *inode, struct file *file)
{
    printk("Tagalog Translator Device opened\n");
    return 0;
}

// closing the device
static int translatorRelease(struct inode *inode, struct file *file)
{
    printk("Tagalog Translator Device closed\n");
    return 0;
}

// read returns the translation back to user
static ssize_t translatorRead(struct file *file, char __user *buf,
                              size_t count, loff_t *pos)
{
    size_t len = strlen(translatedWord);

    if (*pos > 0)
    {
        return 0; // EOF, it should be at 0 to restart word
    }

    // null terminate just in case
    translatedWord[MAX_WORD_LENGTH - 1] = '\0';

    // truncate in case the word length is too long
    if (count < len)
    {
        len = count;
    }

    // send translated word back to user
    if (copy_to_user(buf, translatedWord, len))
    {
        return -EFAULT;
    }

    /*
    // update read position to not read again
    *pos += len;
    */

    printk("translatorRead called. *pos=%lld, translatedWord='%s'\n",
           *pos, translatedWord);
    return len;
}

// gets word from user and translates it
static ssize_t translatorWrite(struct file *file, const char __user *buf,
                               size_t count, loff_t *pos)
{
    char input[MAX_WORD_LENGTH];

    // if the word is too long
    if (count > MAX_WORD_LENGTH - 1)
    {
        count = MAX_WORD_LENGTH - 1;
    }

    // get the word from the user
    if (copy_from_user(input, buf, count))
    {
        return -EFAULT;
    }
    input[count] = '\0';

    // translate the word
    const char *result = translate(input);

    // clear the translated word buffer to ensure it doesn't print the old word
    memset(translatedWord, 0, MAX_WORD_LENGTH);
    strncpy(translatedWord, result, MAX_WORD_LENGTH - 1);
    translatedWord[MAX_WORD_LENGTH - 1] = '\0';

    // printk("Input: '%s', translation: '%s'\n", input, translatedWord);

    // update position so we can read into the buffer properly on the next call
    *pos = 0;
    return count;
}

// ioctl function
static long translatorIoctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    switch (cmd)
    {
    case IOCTL_SET_MODE: // when the user wants to set a translation mode
        if (arg == ENG_TO_TAG || arg == TAG_TO_ENG || arg == AUTO)
        {
            // changes the number value the user put in into a translateMode value
            currMode = (enum translateMode)arg;
            printk("Translation mode: %d\n", currMode);
            return 0;
        }
        else
        {
            return -EINVAL; // invalid
        }
    default:
        return -ENOTTY; // unknown ioctl command
    }
}

// device operations
static const struct file_operations translatorFops = {
    .owner = THIS_MODULE,
    .open = translatorOpen,
    .release = translatorRelease,
    .read = translatorRead,
    .write = translatorWrite,
    .unlocked_ioctl = translatorIoctl,
};

// misc device structure
static struct miscdevice translatorDevice = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = &translatorFops,
};

// module initialization
static int __init translatorInit(void)
{
    int ret;

    // register the device
    ret = misc_register(&translatorDevice);
    if (ret)
    {
        printk(KERN_ERR "Failed to register the device\n");
        return ret;
    }

    printk(KERN_INFO "Tagalog Translator Device registered\n");
    return 0;
}

// module being removed
static void __exit translatorExit(void)
{
    misc_deregister(&translatorDevice);
    printk(KERN_INFO "Tagalog Translator Device unregistered\n");
}

module_init(translatorInit);
module_exit(translatorExit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A Tagalog-English Translator Device Driver");
MODULE_AUTHOR("Chrissa Lavapie");
