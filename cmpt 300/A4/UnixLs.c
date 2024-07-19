#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <grp.h>
#include <pwd.h>
#include <time.h>
#include <stdbool.h>
#include <ctype.h>

ssize_t bytes_read;

void getAndPrintUserName(uid_t uid)
{

    struct passwd *pw = NULL;
    pw = getpwuid(uid);

    if (pw)
    {
        printf(" %-10s", pw->pw_name);
    }
    else
    {
        perror("Hmm not found???");
    }
}

void getAndPrintGroup(gid_t grpNum)
{
    struct group *grp;

    grp = getgrgid(grpNum);

    if (grp)
    {
        printf(" %-10s ", grp->gr_name);
    }
    else
    {
        printf("No group name for %u found\n", grpNum);
    }
}

void printPermissions(mode_t mode)
{
    printf("%c", S_ISLNK(mode) ? 'l' : S_ISDIR(mode) ? 'd'
                                                     : '-');
    printf((mode & S_IRUSR) ? "r" : "-");
    printf((mode & S_IWUSR) ? "w" : "-");
    printf((mode & S_IXUSR) ? "x" : "-");
    printf((mode & S_IRGRP) ? "r" : "-");
    printf((mode & S_IWGRP) ? "w" : "-");
    printf((mode & S_IXGRP) ? "x" : "-");
    printf((mode & S_IROTH) ? "r" : "-");
    printf((mode & S_IWOTH) ? "w" : "-");
    printf((mode & S_IXOTH) ? "x" : "-");
}

void printL_SLink(struct stat fileStat, char *entryName, char *linkTarget)
{
    printPermissions(fileStat.st_mode);
    printf(" %-10lu", fileStat.st_nlink);
    getAndPrintUserName(fileStat.st_uid);

    getAndPrintGroup(fileStat.st_gid);
    char buff[100];
    strftime(buff, sizeof(buff), "%b %d %Y %H:%M", localtime(&fileStat.st_mtime));

    printf("%-10lld  %-25s %-5s->%-5s \n", (long long)fileStat.st_size, buff, entryName, linkTarget);
}

void printL_normalLink(struct stat fileStat, char *entryName)
{
    printPermissions(fileStat.st_mode);
    printf(" %-10lu", fileStat.st_nlink);
    getAndPrintUserName(fileStat.st_uid);

    getAndPrintGroup(fileStat.st_gid);
    char buff[100];
    strftime(buff, sizeof(buff), "%b %d %Y %H:%M", localtime(&fileStat.st_mtime));

    printf("%-10lld  %-25s %-10s \n", (long long)fileStat.st_size, buff, entryName);
}

void printIAndL(struct stat fileStat, char *entryName, char *linkTarget)
{
    printf(" %-10lu ", fileStat.st_ino);
    if (linkTarget == NULL)
    {
        printL_normalLink(fileStat, entryName);
    }
    else
    {
        printL_SLink(fileStat, entryName, linkTarget);
    }
}

void printFileNamesInFolder(DIR *dir,  struct dirent *entry)
{
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_name[0] == '.')
        {
            continue;
        }
        printf("%-25s \n", entry->d_name);
    }
    return;
}

/// used when the user provides a file name instead of a folder name
void printInfoForFile(char *argv[], int argc, char *filePath, bool iFlag, bool lFlag){
    struct stat fileStat;

// checking if any flags exist in the args
    if (argv[1][0] == '-' && islower(argv[1][1]) > 0)
            { //it means its an option
              // only iFlag
                if (iFlag && !lFlag)
                {
                
                    lstat(filePath, &fileStat);

                    printf("%-25s %10lu \n", filePath, fileStat.st_ino);

                }
              // only lFlag
                else if (lFlag && !iFlag)
                {

                    if (lstat(filePath, &fileStat) == 0)
                        {
                            // check if its a sym link
                            if (S_ISLNK(fileStat.st_mode))
                            {
                                char linkTarget[256];

                                ssize_t len = readlink(filePath, linkTarget, sizeof(linkTarget) - 1);
                                linkTarget[len] = '\0';
                                printL_SLink(fileStat, filePath, linkTarget);
                            }
                            else
                            {
                                printL_normalLink(fileStat, filePath);
                            }
                        }
                        else
                        {
                            printf("error in stat\n");
                        }
                }
              // both iFlag and lFlag
                else if (iFlag && lFlag)
                {

                        if (lstat(filePath, &fileStat) == 0)
                        {
                            if (S_ISLNK(fileStat.st_mode))
                            {
                                char linkTarget[256];
                                ssize_t len = readlink(filePath, linkTarget, sizeof(linkTarget) - 1);
                                linkTarget[len] = '\0';
                                printIAndL(fileStat, filePath, linkTarget);
                            }
                            else
                            {
                                printIAndL(fileStat, filePath, NULL);
                            }
                        }
                        else
                        {
                            printf("error in stat\n");
                        }

                }
                else
                {
                       printf("\n%s\n", filePath);

                }

}
else {
    printf("\n%s\n", filePath);
}
}

// this method goes through all the args, checks if its a directory name or an option, and puts the directory names in a list, and makes appropritae flags true
// then it goes throught he directory list created and performs the prints according to the flags
void processCOmmands(char *argv[], int argc)
{
    DIR *dir = NULL;
    char *directories[argc];
    int dirCount = 0;
    struct dirent *entry;
    struct stat fileStat;
    char buffer[300];
    char *directoryPath = ".";
    bool iFlag = false;
    bool lFlag = false;
    bool foundDir = false;

    // if no options or path is provided print file names in the current working directory
    if (argc == 1)
    {
        printFileNamesInFolder(dir, entry);
        closedir(dir);
        return;
    }

    else
    {
        //check if the argument is option i.e starts with - and lowercase

        for (int i = 1; i < argc; i++)
        { 
            if (!foundDir && (argv[i][0] == '-' && islower(argv[i][1]) > 0))
            {
                // check if its i
                if(strchr(argv[i], 'i') != NULL){
                    iFlag = true;
                    
                }
                // check if its l
                if(strchr(argv[i], 'l') != NULL){
                    lFlag = true;
                }
            }

            else 
            {
                directories[dirCount++] = argv[i];
                foundDir = true;
            }
        }

        if (dirCount == 0)
        {
            directories[dirCount++] = ".";
        }
        // go through the directory list
        for (int i = 0; i < dirCount; ++i)
        {
           
            directoryPath = directories[i];
            lstat(directoryPath, &fileStat);

            // check if its a dir, if not print info about the file without doing readdir
            if(S_ISREG(fileStat.st_mode) == 1){
                printInfoForFile(argv, argc, directoryPath, iFlag, lFlag);
                continue;
            }
            else {
            dir = opendir(directoryPath);  
            }

            if(dirCount > 1){
            printf("\nDirectory: %s\n", directories[i]);
            }
            
            if (dir == NULL)
            {
                // Failed to open directory, print why
                perror("Failed to open directory");
                continue;
            }

            if (argv[1][0] == '-' && islower(argv[1][1]) > 0)
            { //it means its an option

                if (iFlag && !lFlag)
                {
                    while ((entry = readdir(dir)) != NULL)
                    {
                        if (entry->d_name[0] == '.')
                        {
                            continue;
                        }
                        char fullPath[1024];
                        //find full path
                        snprintf(fullPath, 1024, "%s/%s", directoryPath, entry->d_name);
                        lstat(fullPath, &fileStat);

                        printf("%-25s %10lu \n", entry->d_name, fileStat.st_ino);
                    }

                    closedir(dir);
                }
                else if (lFlag && !iFlag)
                {

                    while ((entry = readdir(dir)) != NULL)
                    {
                        if (entry->d_name[0] == '.')
                        {
                            continue;
                        }
                        char fullPath[1024];
                        snprintf(fullPath, 1024, "%s/%s", directoryPath, entry->d_name);
                        if (lstat(fullPath, &fileStat) == 0)
                        {
                            if (S_ISLNK(fileStat.st_mode))
                            {
                                char linkTarget[256];

                                ssize_t len = readlink(fullPath, linkTarget, sizeof(linkTarget) - 1);
                                linkTarget[len] = '\0';
                                printL_SLink(fileStat, entry->d_name, linkTarget);
                            }
                            else
                            {
                                printL_normalLink(fileStat, entry->d_name);
                            }
                        }
                        else
                        {
                            printf("error in stat\n");
                        }
                    }

                    closedir(dir);
                }
                else if (iFlag && lFlag)
                {
                    while ((entry = readdir(dir)) != NULL)

                    {
                        if (entry->d_name[0] == '.')
                        {
                            continue;
                        }
                        char fullPath[1024];
                        snprintf(fullPath, 1024, "%s/%s", directoryPath, entry->d_name);
                        if (lstat(fullPath, &fileStat) == 0)
                        {
                            if (S_ISLNK(fileStat.st_mode))
                            {
                                char linkTarget[256];
                                ssize_t len = readlink(fullPath, linkTarget, sizeof(linkTarget) - 1);
                                linkTarget[len] = '\0';
                                printIAndL(fileStat, entry->d_name, linkTarget);
                            }
                            else
                            {
                                printIAndL(fileStat, entry->d_name, NULL);
                            }
                        }
                        else
                        {
                            printf("error in stat\n");
                        }
                    }

                    closedir(dir);
                }
                else
                {
                    printFileNamesInFolder(dir, entry);
                closedir(dir);
                }
            }
            else
            {
                // if no options are provided just print file names in the folder
                printFileNamesInFolder(dir, entry);
                closedir(dir);
            }
        }
        return;
    }
}

int main(int argc, char *argv[])
{
    // method that does all the ls work
    processCOmmands(argv, argc);
    return 0;
}
