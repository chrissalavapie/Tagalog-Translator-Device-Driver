# Brief
Device Driver Function:
My device driver will be a translator from English to Tagalog. When a user writes something in English, using the write() function, it will be translated into Tagalog, or it can ask for a specific word and get its translation. Then using the read() function, the user space will read the translation and print it out for the user. An ioctl() command will be used to switch translation modes, from English to Tagalog or vice versa. 


## Commands to Build and Load Device Driver
1. cd csc415-assignment-6-device-driver-chrissalavapie // go into hw folder
2. cd Module // Module folder is where the device driver code is stored 
3. make // compile the device driver code 
4. sudo insmod translator // load it
5. ls -l /dev/translator // optional: check to see if it loaded correctly 
6. sudo chmod 666 /dev/translator // changing the permissions 
7. cd .. // exit module
8. cd Test // enter Test folder that has the user app code 
9. make // compile user app code 

## To Unload Device Driver
1. cd Module
2. sudo rmmod translator 
3. make clean

## How to Interact with User App
1. First you must be in the Test class (cd Test) 
2. Run the command make run 
3. It will prompt you to choose a translation mode (English to Tagalog, Tagalog to English,
   or Auto detecting a language).
4. Once you’ve chosen a mode, a title and word bank should pop up showing you which
   words are available to translate in the dictionary.
6. It will prompt you to choose a word from the list to translate.
7. The program will then output the translation for that word. If the input is not found
   in the list, the program will output “unknown.” If the word is in the incorrect language
   (ex. a Tagalog word, but in the English to Tagalog mode) the program will output “unknown.”
9. It will then ask you if you would like to continue using the program or if you would like
   to exit by entering either “CONTINUE” or “EXIT.”
11. If you continue, you may continue to enter words from the list until you choose to exit
    the program. 
