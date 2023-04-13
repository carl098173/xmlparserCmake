/*
License Contact: carl25nester@gmail.com

CCI Parsing App

Copyright 2023 Carl Nester

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
documentation files(the “Software”), to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense, and /or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all copies or substantial portions
of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE.

*/
#include "xmlparserCmake.h"
#include <filesystem>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <iomanip>
#include <stdlib.h>
#include <algorithm>
#include <cmath>

#pragma warning(disable : 4996)
#include <chrono>
#include <ctime>   

struct cci_items {
    long id = 0;
    std::string status;             // xml node
    std::string date;               // xml node
    std::string contributor;        // xml node
    std::string definition;         // xml node
    std::string type;               // xml node
    int refCount = 0;               // counts number of reference tag entries
    int refIndex[5] = { -1, -1, -1, -1, -1 };  // holds up to five reference entries per node
};
struct references {
    std::string creator;            // xml reference tag
    std::string title;              // xml reference tag
    int version = 0;                // xml reference tag
    std::string location;           // xml reference tag
    std::string index;              // xml reference tag
};

void CleanUpCache();
void ExecutePythonScript();
void DownloadFile(bool& fileExistsFlag);
bool FileExists();
void displayAllFromID(std::vector<cci_items>& xml, std::vector<references>& ref, std::string& ver, std::string& pub, std::string fileName, std::vector<std::string>& IdCollection);
void displayById_IdCollection(std::vector<cci_items>& xml, std::vector<references>& ref, std::string& ver, std::string& pub, std::string fileName, long id, std::vector<std::string>& IdCollection);
void collectFlags(int argc, char* argv[], bool& endProgram, bool& allFlag, bool& depFlag, bool& draftFlag, long& id, std::string& inputXml, std::string& searchTerm, std::string& indexSearchTerm);
bool loadXml(std::string& inputXml, std::ifstream& inputStream); // loads xml into filestream
void parseXml(std::ifstream& inputStream, std::vector<cci_items>& xml, std::vector<references>& ref, std::string& ver, std::string& pub);  // parses xml from filestream into memory
void displayAll(std::vector<cci_items>& xml, std::vector<references>& ref, std::string& ver, std::string& pub, std::string fileName);  // ALL output spit onto screen
void displayById(std::vector<cci_items>& xml, std::vector<references>& ref, std::string& ver, std::string& pub, std::string fileName, long id); // type in a cc item id and print out only that selection
long displayByIdGetInput();      // get user input for displayById function
int mainMenu(); // main menu helper
void displayProgramTag(); // creates a tag at the top of the screen telling the name of the program
bool checkNumber(std::string str);  // return true if is a number and no alphabet/special characters
void findByDate(std::vector<cci_items>& xml, std::vector<references>& ref, std::string& ver, std::string& pub, std::string fileName, std::vector<std::string>& IdCollection, std::string date); //Search CCI items by publish date.
std::string findByDateHelper(std::string fileName);  // gets user input for date
std::string keyWordSearchHelper(); // gets user input for keyWordSearch
void keyWordSearch(std::vector<cci_items>& xml, std::vector<references>& ref, std::string& ver, std::string& pub, std::string fileName, std::string keyword, std::vector<std::string>& IdCollection, bool draft, bool dep); //Searches CCI items by node description
void printID(std::vector<std::string>& IdCollection);  // prints CCI ids and formats them in rows and columns, ordered by columns
std::string findByIndexHelper(); // gets user input for findByIndex
void findByIndex(std::vector<cci_items>& xml, std::vector<references>& ref, std::string& ver, std::string& pub, std::string fileName, std::string index, std::vector<std::string>& IdCollection, bool draft, bool dep); // Searches reference tags for a specific index
void printFixedLine();      // prints a single fixed witdth break line
void helpMenu(std::vector<std::string>& arguments);            // prints help menu for command line arguments
bool loadXmlMenu(std::string& inputXml, std::ifstream& inputStream);     // helper function to obtain filename from user data
std::string toUpper(std::string input);
void showLicense(); // show license information


int main(int argc, char* argv[])
{
    std::string inputXml = "U_CCI_List.xml";           // file name
    std::string version;             // cci list version
    std::string publishDate;     // cci list publish date
    std::vector<cci_items> xml(0);             // initializes first vector of node data
    std::vector<references> references(0);     // initializes 1st vector of reference tags
    std::vector<std::string> IdCollection(0);     // temporary vector that holds all items found in search
    long id = -100;                              // temporary cc item holder
    std::string date = " ";                      // temporary date holder
    std::string searchTerm = "";                       // temporary holder for search terms
    std::string indexSearchTerm = "";
    //////////////// FLAGS
    bool endProgram = false;
    bool allFlag = false;
    bool depFlag = false;
    bool draftFlag = false;
    bool fileExistsFlag;
    ///////////////
    std::ifstream inputStream;

    fileExistsFlag = FileExists();
    if (fileExistsFlag)
    {
        // do nothing
    }
    else
    {
        DownloadFile(fileExistsFlag);
        CleanUpCache();
        printf("Press any key to continue...");
        getchar();
    }


    if (argc > 1)       // check if there are more arguments than the 1 produced by the program name
    {
        collectFlags(argc, argv, endProgram, allFlag, depFlag, draftFlag, id, inputXml, searchTerm, indexSearchTerm);
        if (!endProgram)
        {
            if (loadXml(inputXml, inputStream))        //load xml into filestream
            {
                parseXml(inputStream, xml, references, version, publishDate);   // function will parse selected xml

                if (id != -100)
                {
                    displayById(xml, references, version, publishDate, inputXml, id);
                    return 0;
                }
                else if (searchTerm != "")      // search by index if searchTerm is blank
                {
                    keyWordSearch(xml, references, version, publishDate, inputXml, searchTerm, IdCollection, draftFlag, depFlag);
                    if (!allFlag)
                        printID(IdCollection);
                    else
                    {
                        displayAllFromID(xml, references, version, publishDate, inputXml, IdCollection);
                    }
                }
                else if (indexSearchTerm != "")     // search by index if indexSearchTerm is blank
                {
                    findByIndex(xml, references, version, publishDate, inputXml, indexSearchTerm, IdCollection, draftFlag, depFlag);
                    if (!allFlag)
                        printID(IdCollection);
                    else
                        displayAllFromID(xml, references, version, publishDate, inputXml, IdCollection);
                }
            }
        }
    } // // end : check if there are more arguments than the 1 produced by the program name

    if (argc < 2)      // if the only argument passed in is the filename, use the main program
    {
        // collect xml file
        if (!fileExistsFlag)    // if file does not exist, use menu
        {
            if (!loadXmlMenu(inputXml, inputStream))     // function will select xml to load into filestream
            {
                return 1;                             // end program if false
            }
        }
        else
            loadXml(inputXml, inputStream);

        // testing for optimization 
        auto start = std::chrono::system_clock::now();
        // testing for optimization 

        parseXml(inputStream, xml, references, version, publishDate);   // function will parse selected xml

        // testing for optimization 
        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;
        std::time_t end_time = std::chrono::system_clock::to_time_t(end);
        std::cout << "finished computation at " << std::ctime(&end_time)
            << "elapsed time: " << elapsed_seconds.count() << "s"
            << std::endl;
        printf("Press any key to continue...");
        getchar();
        // testing for optimization 

        while (endProgram == false)     // main program process, no command line arguments
        {
            switch (mainMenu())
            {
            case 1:                     // 1.) Select item by CCI Item Id
            {
                displayProgramTag();
                displayById(xml, references, version, publishDate, inputXml, displayByIdGetInput());
                break;
            }

            case 2:                     // 2.) Search CCI items by publish date.
            {
                displayProgramTag();
                findByDate(xml, references, version, publishDate, inputXml, IdCollection, findByDateHelper(inputXml));
                printf("Press any key to continue...");
                getchar();
                break;
            }

            case 3:                     // 3.) Search CCI items by definition keywords.
            {
                displayProgramTag();
                std::cout << "Using file: " << inputXml << std::endl;
                keyWordSearch(xml, references, version, publishDate, inputXml, keyWordSearchHelper(), IdCollection, draftFlag, depFlag);
                printID(IdCollection);
                printf("Press any key to continue...");
                getchar();
                break;
            }
            case 4:                     // 4.) Search by index.
            {
                displayProgramTag();
                std::cout << "Using file: " << inputXml << std::endl;
                findByIndex(xml, references, version, publishDate, inputXml, findByIndexHelper(), IdCollection, draftFlag, depFlag);
                printID(IdCollection);        
                printf("Press any key to continue...");
                getchar();
                break;
            }
            case 5:                     // 5.) Display all entries.
            {
                displayProgramTag();
                std::cout << "Using file: " << inputXml << std::endl;
                displayAll(xml, references, version, publishDate, inputXml);
                printf("Press any key to continue...");
                getchar();break;
            }
            case 6:                     // 6.) Quit
            {
                endProgram = true;
                break;
            }
            default:
            {
                std::cout << "Invalid entry.";
                printf("Press any key to continue...");
                getchar();
                break;
            }

            }
            IdCollection.clear();
        }   // end while loop
        std::cout << "Goodbye..." << std::endl;
        printf("Press any key to continue...");
        getchar();
    }       // end program with no added arguments

}

void collectFlags(int argc, char* argv[], bool& endProgram, bool& allFlag, bool& depFlag, bool& draftFlag, long& id, std::string& inputXml, std::string& searchTerm, std::string& indexSearchTerm)
{
    //////////// FLAG LIST
    // status = false; endProgram = false; allFlag = false; depFlag = false;
    // draftFlag = false;
    std::size_t xmlFound, idFound, keyFound, indexFound;

    std::vector<std::string> arguments;         // collected command-line arguments

    for (int i = 0; i < argc; i++)              // collect command line args and store in a vector for testing
    {
        arguments.push_back(argv[i]);
    }

    for (auto& arg : arguments) // collect all of the valid arguments
    {
        //            found = arg.find(".xml");
        xmlFound = arg.find(".xml");
        idFound = arg.find("--id=");
        keyFound = arg.find("--key=");
        indexFound = arg.find("--index=");

        if (arg == "--help" || arg == "-h")                     // if -help is found, display menu and do not continue
        {
            helpMenu(arguments);                         // open the help menu then quit
            endProgram = true;
            return;
        }
        else if (arg == "-l" || arg == "--license")
        {
            showLicense();
            endProgram = true;
            return;
        }
        else if (xmlFound != std::string::npos)              // parse for ".xml" and assume this is the .xml file
        {
            inputXml = arg;
        }
        else if (idFound != std::string::npos)
        {
            if (!checkNumber(arg.substr(5, arg.length())))  // check to see if this is a number, 
            {
                std::cout << "Invalid cc item id format";
                endProgram = true;
            }
            else
            {
                id = std::stol(arg.substr(5, arg.length()));
            }

        }
        else if (arg == "-a" || arg == "--all")
            allFlag = true;
        else if (arg == "-nod" || arg == "--not_draft")
            draftFlag = true;
        else if (arg == "-nodep" || arg == "--not_deprecated")
            depFlag = true;
        else if (keyFound != std::string::npos)
        {
            searchTerm = arg.substr(6, arg.length());   // takes substring of --index=" to use remainder in keyword search
        }
        else if (indexFound != std::string::npos)
        {
            indexSearchTerm = arg.substr(8, arg.length());  // takes substring of --index=" to use remainder in index search
        }
        else if (arg == (argv[0]))
        {
            // found filename, do nothing
        }
        else
        {
            std::cout << "Invalid argument:" << arg << std::endl;
            endProgram = true;
        }

    } // end : collect all of the valid arguments
}

bool loadXml(std::string& inputXml, std::ifstream& inputStream)     // function will select xml to load into filestream
{
    inputStream.open(inputXml);     // open stream
    if (inputStream)
    {
        std::cout << "The file has been loaded successfully." << std::endl;
        return true;
    }
    else
    {
        return false;
    }
}

bool loadXmlMenu(std::string& inputXml, std::ifstream& inputStream)     // helper function to obtain filename from user data
{
    bool successfulLoad = false;
    std::string path = ".";         // uses current directory as working path
    std::string fileList;
    size_t found;

    while (successfulLoad == false)
    {
        displayProgramTag();

        std::cout << "File list:" << std::endl; // menu helper
        printFixedLine();
        std::cout << std::endl;                 // end menu helper


        for (const auto& entry : std::filesystem::directory_iterator(path))  // outputs directory contents to console
        {
            fileList = entry.path().string();
            if (path == ".")                                        // if using current directory, clean up formatting
                fileList = fileList.substr(2, fileList.size() - 2);
            found = fileList.find(".xml");      // test to see if the file is xml format
            if (found != std::string::npos)         // if (true) ...
            {
                std::cout << fileList << std::endl;
            }
        }

        for (int i = 0; i < 30; i++)            // menu helper
        {
            std::cout << "-";
        }                                       // menu helper
        std::cout << std::endl;
        std::cout << "Enter the file name or type q to quit >> ";
        std::getline(std::cin >> std::ws, inputXml);
        if (inputXml[0] == 'q' || inputXml[0] == 'Q')  // check to see if loop should quit
        {
            std::cout << "Goodbye..." << std::endl;
            printf("Press any key to continue...");
            getchar();
            successfulLoad = false;
            break;
        }
        successfulLoad = loadXml(inputXml, inputStream);     // load xml into filestream if returns true
    }
    if (successfulLoad == false)
    {
        //std::cout << "Unable to load file." << std::endl;
        return false;
    }
    else return true;

}

void parseXml(std::ifstream& inputStream, std::vector<cci_items>& xml, std::vector<references>& ref, std::string& ver, std::string& pub)
{
    const int ID_LENGTH = 10;                   // # of chars in id : eg - CCI-000001
    std::string line;                           // will hold a single line from stream to process
    int refIndex = 0;                          // index for references held in cci.refIndex[] and mirrored in references[]
    std::size_t found;      // can store the maximum size of a theoretically possible object of any type -- used for testing
    size_t pos1 = 0, pos2 = 0;                     // keeps track of where to pull substring
    cci_items currentCCI;                       // creates a temp cci_items to push to main vector
    references currentRef;
    bool startFile = true;
    bool foundMetadata = false;

    if (inputStream) // if stream opened successfully, read line-by-line
    {
        while (!foundMetadata)
        {
            std::getline(inputStream, line);

            ///////////////// block finds version

            found = line.find("<version>");      // test to see if this holds version metadata
            if (found != std::string::npos)         // if (true) ...
            {
                pos1 = line.find("<version>") + 9;  // pos1 = where we found the line + the number of chars in const "<status>" -- 8
                pos2 = line.find("</version>");
                ver = line.substr(pos1, pos2 - pos1);
            }
            ///////////////// END block that finds version

            ///////////////// block finds publishdate

            found = line.find("<publishdate>");      // test to see if this holds publish date metadata
            if (found != std::string::npos)         // if (true) ...
            {
                pos1 = line.find("<publishdate>") + 13;  // pos1 = where we found the line + the number of chars in const "<status>" -- 8
                pos2 = line.find("</publishdate>");
                pub = line.substr(pos1, pos2 - pos1);
                foundMetadata = true;
            }
            ///////////////// END block that finds publishdate
        }
    }

    if (inputStream)    // if stream opened successfully, read line-by-line
    {
        while (std::getline(inputStream, line))
        {
            ///////////////// block finds xml.id

            found = line.find("<cci_item id=\"");      // test to see if this holds cci item id
            if (found != std::string::npos)         // if (true) ...
            {
                pos1 = line.find("<cci_item id=\"CCI-") + 18;  // pos1 = where we found the line + the number of chars in const "<status>" -- 8
                pos2 = line.find("\">");
                currentCCI.id = stoi(line.substr(pos1, pos2 - pos1));
            }
            ///////////////// END block that finds xml.id

            found = NULL;

            ///////////////// block finds xml.status
            found = line.find("<status>");      // test to see if this holds cci item id
            if (found != std::string::npos)         // if (true) ...
            {
                pos1 = line.find("<status>") + 8;  // pos1 = where we found the line + the number of chars in const "<status>" -- 8
                pos2 = line.find("</status>");
                currentCCI.status = line.substr(pos1, pos2 - pos1);
            }
            ///////////////// block finds xml.status

            ///////////////// block finds xml.date

            found = line.find("<publishdate>");      // test to see if this holds cci item id
            if (found != std::string::npos)         // if (true) ...
            {
                pos1 = line.find("<publishdate>") + 13;  // pos1 = where we found the line + the number of chars in const "<status>" -- 8
                pos2 = line.find("</publishdate>");
                currentCCI.date = line.substr(pos1, pos2 - pos1);
            }
            ///////////////// block ends xml.date

            found = NULL;

            ///////////////// block finds xml.contributor

            found = line.find("<contributor>");      // test to see if this holds cci item id
            if (found != std::string::npos)         // if (true) ...
            {
                pos1 = line.find("<contributor>") + 13;  // pos1 = where we found the line + the number of chars in const "<contributor>" -- 13
                pos2 = line.find("</contributor>");
                currentCCI.contributor = line.substr(pos1, pos2 - pos1);
            }

            ///////////////// block ends xml.contributor

            found = NULL;

            ///////////////// block finds xml.definition

            found = line.find("<definition>");      // test to see if this holds cci item id
            if (found != std::string::npos)         // if (true) ...
            {
                pos1 = line.find("<definition>") + 12;  // pos1 = where we found the line + the number of chars in const "<definition>" -- 12
                pos2 = line.find("</definition>");
                currentCCI.definition = line.substr(pos1, pos2 - pos1);
            }

            ///////////////// block ends xml.definition

            found = NULL;

            ///////////////// block finds xml.type

            found = line.find("<type>");      // test to see if this holds cci item id
            if (found != std::string::npos)         // if (true) ...
            {
                pos1 = line.find("<type>") + 12;  // pos1 = where we found the line + the number of chars in const "<type>" -- 6
                pos2 = line.find("</type>");
                currentCCI.type = line.substr(pos1, pos2 - pos1);
            }

            ///////////////// block ends xml.type

            found = NULL;

            ///////////////// block finds all xml reference tags

            found = line.find("<references>");
            if (found != std::string::npos)         // if (true) ...
            {
                // make a loop that gets new lines until </references> is found
                while (true)
                {
                    std::getline(inputStream, line);  // feed in the next line


                    ///////////////// block finds creator tag

                    found = line.find("<reference creator=");      // test to see if this holds "<reference creator="
                    if (found != std::string::npos)         // if (true) ...
                    {
                        pos1 = line.find("<reference creator=") + 20;  // pos1 = where we found the line + the number of chars in const "<type>" -- 6
                        pos2 = line.find("\" title");
                        currentRef.creator = line.substr(pos1, pos2 - pos1);
                    }

                    ///////////////// end of block finds creator tag

                    ///////////////// block finds title tag

                    found = line.find("title=");      // test to see if this holds "title="
                    if (found != std::string::npos)         // if (true) ...
                    {
                        pos1 = line.find("title=\"") + 7;  // pos1 = where we found the line + the number of chars in const "<type>" -- 6
                        pos2 = line.find("\" version");
                        currentRef.title = line.substr(pos1, pos2 - pos1);
                    }

                    ///////////////// end of block finds title tag

                    ///////////////// block finds version tag

                    found = line.find("version=\"");      // test to see if this holds "version="
                    if (found != std::string::npos)         // if (true) ...
                    {
                        pos1 = line.find("version=\"") + 9;  // pos1 = where we found the line + the number of chars in const "<type>" -- 6
                        pos2 = line.find("\" location");
                        currentRef.version = stoi(line.substr(pos1, pos2 - pos1)); // ALSO converts to int
                    }

                    ///////////////// end of block finds title tag

                    ///////////////// block finds location tag

                    found = line.find("location=\"");      // test to see if this holds "version="
                    if (found != std::string::npos)         // if (true) ...
                    {
                        pos1 = line.find("location=\"") + 10;  // pos1 = where we found the line + the number of chars in const "<type>" -- 6
                        pos2 = line.find("\" index");
                        currentRef.location = line.substr(pos1, pos2 - pos1);
                    }

                    ///////////////// end of block finds location tag

                    ///////////////// block finds index tag

                    found = line.find("index=\"");      // test to see if this holds "version="
                    if (found != std::string::npos)         // if (true) ...
                    {
                        pos1 = line.find("index=\"") + 7;  // pos1 = where we found the line + the number of chars in const "<type>" -- 6
                        pos2 = line.find("\" />");
                        currentRef.index = line.substr(pos1, pos2 - pos1);

                        // found all reference tags
                        // if the index tag is missing this will break the program here!!!!
                        ref.push_back(currentRef);      // adds references to vector

                        ///////////////  only need to do these once--- if they find a tag
                        currentCCI.refIndex[currentCCI.refCount] = refIndex; // tags current node with reference id to connect the two vectors
                        currentCCI.refCount++;              // increments reference count (ie 1 to 3/4/5)
                        refIndex++;                         // increments reference index (sum of all reference indexes)

                        ///////////////

                    }

                    ///////////////// end of block finds index tag



                    //  BREAK STATEMENT
                    found = line.find("</references>");      // test to see if this holds "version="
                    if (found != std::string::npos)     // end of index;; clean-up goes here
                    {
                        xml.push_back(currentCCI);      // pushes temp struct to vector
                        currentCCI.refCount = 0;          // resets refCount
                        break;                          // break at end of reference entries
                    }

                }

                ///////////////// end of block finds all xml reference tags
            }


            found = NULL;

        }
        inputStream.close();
    }
}

long displayByIdGetInput()      // get user input for displayById function
{
    std::string input;
    long id = 0;
    std::cout << "Enter the CCI-Item ID that you would like to look up." << std::endl;
    std::cout << "CCI# >> ";
    std::getline(std::cin >> std::ws, input);
    if (checkNumber(input))                     // if input contains a number, continue to print details
    {
        id = std::stol(input);                  // convert input to long
        printFixedLine();
        return id;
    }
    else
    {
        std::cout << "Not a valid CCI item.";
        return -1;
    }
}

void displayById_IdCollection(std::vector<cci_items>& xml, std::vector<references>& ref, std::string& ver, std::string& pub, std::string fileName, long id, std::vector<std::string>& IdCollection)
{
    bool found = false;

    if (id != -1)       // if input checks haven't failed 
    {
        for (auto& a : xml)
        {
            if (id == a.id)
            {
                std::cout << "Using file \"" << fileName << "\"" << std::endl;
                found = true;
                std::cout << "Version " << ver << std::endl;
                std::cout << "Publish Date " << pub << std::endl;
                std::cout << "CCI Item ID: CCI-" << std::right << std::setw(6) << std::setfill('0') << a.id << "\t\t\t";
                std::cout << "Status:" << a.status << std::endl;
                std::cout << "Contributor: " << a.contributor << "\t\t\t";
                std::cout << "Date:" << a.date << std::endl;
                std::cout << a.definition << std::endl;
                std::cout << a.type << std::endl;
                std::cout << "References:";
                for (int i = 0; i < a.refCount; i++)
                {
                    if (i != 0)
                        std::cout << std::setw(20) << std::setfill(' ');
                    else
                        std::cout << std::right << std::setw(9) << std::setfill(' ');
                    std::cout << ref[i].creator;
                    std::cout << ref[i].title << " ";
                    std::cout << "(v" << ref[i].version << "): ";
                    std::cout << ref[i].index;
                    std::cout << std::endl;
                }
            }
        }
    }
}

void displayById(std::vector<cci_items>& xml, std::vector<references>& ref, std::string& ver, std::string& pub, std::string fileName, long id)
{

    bool found = false;

    if (id != -1)       // if input checks haven't failed 
    {
        for (auto& a : xml)
        {
            if (id == a.id)
            {
                std::cout << "Using file \"" << fileName << "\"" << std::endl;
                found = true;
                std::cout << "Version " << ver << std::endl;
                std::cout << "Publish Date " << pub << std::endl;
                std::cout << "CCI Item ID: CCI-" << std::right << std::setw(6) << std::setfill('0') << a.id << "\t\t\t";
                std::cout << "Status:" << a.status << std::endl;
                std::cout << "Contributor: " << a.contributor << "\t\t\t";
                std::cout << "Date:" << a.date << std::endl;
                std::cout << a.definition << std::endl;
                std::cout << a.type << std::endl;
                std::cout << "References:";
                for (int i = 0; i < a.refCount; i++)
                {
                    if (i != 0)
                        std::cout << std::setw(20) << std::setfill(' ');
                    else
                        std::cout << std::right << std::setw(9) << std::setfill(' ');
                    std::cout << ref[i].creator;
                    std::cout << ref[i].title << " ";
                    std::cout << "(v" << ref[i].version << "): ";
                    std::cout << ref[i].index;
                    std::cout << std::endl;
                }
            }
        }
    }


    if (found == false)             // did not find the cci item
        std::cout << "Entry does not exist." << std::endl;
        printf("Press any key to continue...");
        getchar();
}

int mainMenu()
{
    bool endProgram = false;
    bool invalidEntry = false;
    std::string input;

    while (endProgram == false)
    {
        displayProgramTag();

        std::cout << "Menu choices:" << std::endl;
        std::cout << "\t 1.) Select item by CCI Item Id" << std::endl;
        std::cout << "\t 2.) List all CCI items by publish date." << std::endl;
        std::cout << "\t 3.) Search CCI items by definition keywords." << std::endl;
        std::cout << "\t 4.) Search by index." << std::endl;
        std::cout << "\t 5.) Display all entries.  ***WARNING! 5000+ entries***" << std::endl;
        std::cout << "\t 6.) Quit" << std::endl;
        if (invalidEntry == true)
        {
            std::cout << "**********************     Invalid entry   ***********************" << std::endl;
            std::cout << "********************** Retry with a number ***********************" << std::endl;
            std::cout << "********************** Or choose 6 to quit ***********************" << std::endl;
        }
        std::cout << "Enter your selection >> ";
        std::getline(std::cin >> std::ws, input);
        if (checkNumber(input) == true)
        {
            endProgram = true;
            return std::stoi(input);
        }
        else
        {
            invalidEntry = true;
        }
    }

    return 0;
}

void displayProgramTag()
{
    /// <summary>
    /// Function flushes filestream and prints the program label to the top of the console
    /// </summary>

    std::cout << std::flush;                // flushes iostream
    std::system("CLS");                     // clears screen

    printFixedLine();
    std::cout << std::setfill(' ') << std::left;;
    std::cout << std::setw(60) << "-\tCONTROL CORRELATION IDENTIFIER (CCI) SPECIFICATION" << "-" << std::endl;
    std::cout << std::setw(60) << "-\tHelper Program" << "-" << std::endl;
    std::cout << std::setw(60) << "-\t-h or --help for command-line options" << "-" << std::endl;
    printFixedLine();
}

void displayAll(std::vector<cci_items>& xml, std::vector<references>& ref, std::string& ver, std::string& pub, std::string fileName)
{
    std::cout << "Using file \"" << fileName << "\"" << std::endl;
    std::cout << "Version " << ver << std::endl;
    std::cout << "Publish Date " << pub << std::endl;
    printFixedLine();
    for (auto& a : xml)
    {
        std::cout << "CCI Item ID: CCI-" << std::setw(6) << std::setfill('0') << a.id << "\t\t\t";
        std::cout << "Status:" << a.status << std::endl;
        std::cout << "Contributor: " << a.contributor << "\t\t\t";
        std::cout << "Date:" << a.date << std::endl;
        std::cout << a.definition << std::endl;
        std::cout << a.type << std::endl;
        std::cout << "References:";
        for (int i = 0; i < a.refCount; i++)
        {
            if (i != 0)
                std::cout << std::setw(20) << std::setfill(' ');
            else
                std::cout << std::right << std::setw(9) << std::setfill(' ');
            std::cout << ref[i].creator;
            std::cout << ref[i].title << " ";
            std::cout << "(v" << ref[i].version << "): ";
            std::cout << ref[i].index;
            std::cout << std::endl;
        }
    }
}

void displayAllFromID(std::vector<cci_items>& xml, std::vector<references>& ref, std::string& ver, std::string& pub, std::string fileName, std::vector<std::string>& IdCollection)
{
    long id;
    std::string input;

    if (IdCollection.size() > 10)
    {
        std::cout << "Over 10 entries found!" << std::endl;
        std::cout << "Would you like to display as a list instead?" << std::endl;
        std::cout << "[y/n] or other entry to quit >> ";
        std::getline(std::cin >> std::ws, input);
        if (input == "y" || input == "Y")
        {
            // continue
        }
        else
        {
            return;
        }
    }
    std::cout << "Using file \"" << fileName << "\"" << std::endl;
    std::cout << "Version " << ver << std::endl;
    std::cout << "Publish Date " << pub << std::endl;
    printFixedLine();
    for (auto& b : IdCollection)  // outer loop idcollection
    {
        for (auto& a : xml)         // inner loop xml
        {
            id = std::stol(b.substr(4, b.length()));
            if (id == a.id)
            {
                std::cout << "CCI Item ID: CCI-" << std::setw(6) << std::setfill('0') << a.id << "\t\t\t";
                std::cout << "Status:" << a.status << std::endl;
                std::cout << "Contributor: " << a.contributor << "\t\t\t";
                std::cout << "Date:" << a.date << std::endl;
                std::cout << a.definition << std::endl;
                std::cout << a.type << std::endl;
                std::cout << "References:";
                for (int i = 0; i < a.refCount; i++)
                {
                    if (i != 0)
                        std::cout << std::setw(20) << std::setfill(' ');
                    else
                        std::cout << std::right << std::setw(9) << std::setfill(' ');
                    std::cout << ref[i].creator;
                    std::cout << ref[i].title << " ";
                    std::cout << "(v" << ref[i].version << "): ";
                    std::cout << ref[i].index;
                    std::cout << std::endl;
                }
            }       // endif

        }            // inner loop xml
        printFixedLine();
    }               // outer loop idcollection

}

bool checkNumber(std::string str)       // return true if string contains only digits 
{
    for (int i = 0; i < str.length(); i++)
    {
        if (isdigit(str[i]) == false)
            return false;
        else
            return true;
    }
    return false;       // added because all control paths need to return a value. Any calls will never touch this if used properly
}

std::string findByDateHelper(std::string fileName)
{
    std::string date;
    bool dateFound = false;
    std::string tempID = "";

    std::cout << "Using file \"" << fileName << "\"" << std::endl;
    std::cout << "Enter the date: Format YYYY-MM-DD" << std::endl;
    std::cout << "Date >> ";
    std::getline(std::cin >> std::ws, date);
    if (date.size() == 10)
    {
        if                  // check to see if format is correct
            (
                checkNumber(date.substr(0, 4)) &&
                date[4] == '-' &&
                checkNumber(date.substr(5, 2)) &&
                date[7] == '-' &&
                checkNumber(date.substr(8, 2))
                )
        {
            // date is valid
            // do nothing
        }
        else
        {
            // date is invalid, reinitialize date to ""
            date = "";
            std::cout << "Invalid Date Format." << std::endl;
        }
    }
    else
    {
        // date is invalid, reinitialize date to ""
        date = "";
        std::cout << "Invalid Date Format." << std::endl;
    }
    return date;
}

void findByDate(std::vector<cci_items>& xml, std::vector<references>& ref, std::string& ver, std::string& pub, std::string fileName, std::vector<std::string>& IdCollection, std::string date)
{
    std::size_t found;      // can store the maximum size of a theoretically possible object of any type -- used for testing
    bool dateFound = false;
    std::string tempID = "";

    std::cout << "Version " << ver << std::endl;
    std::cout << "Publish Date " << pub << std::endl;
    printFixedLine();
    for (auto& a : xml)
    {
        found = a.date.find(date);      // test to see if this holds "version="
        if (found != std::string::npos)         // if (true) ...
        {
            dateFound = true;
            tempID = std::to_string(a.id);
            while (tempID.length() < 6)          // 6 is CCI-XXXXXX
            {
                tempID.insert(0, "0");          // adds preceding zeros
            }
            tempID.insert(0, "CCI-");
            IdCollection.push_back(tempID);
        }
    }
    if (!dateFound)
        std::cout << "No files found with this date." << std::endl;
    else
    {
        // do nothing
    }

    std::cout << std::endl;

}

std::string keyWordSearchHelper()
{
    std::string keyword;
    std::cout << "Enter a keyword to search for >> ";
    std::getline(std::cin >> std::ws, keyword);
    return keyword;
}

void keyWordSearch(std::vector<cci_items>& xml, std::vector<references>& ref, std::string& ver, std::string& pub, std::string fileName, std::string keyword, std::vector<std::string>& IdCollection, bool draft, bool dep)
{

    std::size_t found;      // can store the maximum size of a theoretically possible object of any type -- used for testing
    bool keywordFound = false;
    std::string tempID = "";
    std::string tempString = "";

    std::cout << "Using file \"" << fileName << "\"" << std::endl;
    std::cout << "Version " << ver << std::endl;
    std::cout << "Publish Date " << pub << std::endl;
    printFixedLine();
    for (auto& a : xml)
    {
        tempString = toUpper(a.definition);
        found = tempString.find(toUpper(keyword));      // test to see if this holds "version="
        if (found != std::string::npos)         // if (true) ...
        {
            keywordFound = true;
            tempID = std::to_string(a.id);
            while (tempID.length() < 6)          // 6 is CCI-XXXXXX
            {
                tempID.insert(0, "0");          // adds preceding zeros
            }
            tempID.insert(0, "CCI-");
            if (IdCollection.size() == 0)       // add if this is the first element
                IdCollection.push_back(tempID); // add this item to tempID
            else if (IdCollection.back() == tempID) // do not add if the new element is the same as the last
            {
                // do nothing
            }
            else
            {
                IdCollection.push_back(tempID); // add this item to tempID
            }
        }
    }
    if (!keywordFound)
        std::cout << "No files found with this keyword." << std::endl;
}

void printID(std::vector<std::string>& IdCollection)  // prints multiple CCIDs for broad searches
{
    // determine the number of rows and columns
    double num_cols = 6;
    double num_rows = std::ceil(static_cast<double>(IdCollection.size()) / num_cols);
    // output the vector in columns and rows
    for (double i = 0; i < num_rows; i++) {
        for (double j = 0; j < num_cols; j++) {
            double index = j * num_rows + i;
            if (index < IdCollection.size()) {
                std::cout << "|  " << std::setw(12) << std::left << IdCollection[index] << "  "; // compiler warning conversion
            }
            else {
                std::cout << std::setw(12) << " ";
            }
        }
        std::cout << std::endl;
    }
    if (IdCollection.size() == 1)
        std::cout << std::endl << "Retrieved " << IdCollection.size() << " record." << std::endl;
    else
        std::cout << std::endl << "Retrieved " << IdCollection.size() << " records." << std::endl;
}

std::string findByIndexHelper()
{
    std::string index;
    std::cout << "Enter a keyword to search for >> ";
    std::cin.ignore();
    std::getline(std::cin >> std::ws, index);
    return index;
}

void findByIndex(std::vector<cci_items>& xml, std::vector<references>& ref, std::string& ver, std::string& pub, std::string fileName, std::string index, std::vector<std::string>& IdCollection, bool draft, bool dep)
{
    std::string tempString = "";
    std::size_t found;      // can store the maximum size of a theoretically possible object of any type -- used for testing
    bool keywordFound = false;
    std::string tempID = "";

    std::cout << "Version " << ver << std::endl;
    std::cout << "Publish Date " << pub << std::endl;
    printFixedLine();
    for (int i = 0; i < ref.size(); i++) // loop through all references
    {
        tempString = toUpper(ref[i].index);
        found = tempString.find(toUpper(index));      // test to see if this reference [i] equals the user inputted index
        if (found != std::string::npos)         // if (true) ...
        {
            keywordFound = true;

            for (int x = 0; x < xml.size(); x++)    // outer loop through xml
            {
                for (int y = 0; y < xml[x].refCount; y++)  // inner loop through refIndex array to find matching index
                {
                    if (xml[x].refIndex[y] == i)
                    {
                        if ((dep && xml[x].status == "deprecated") || (draft && xml[x].status == "draft"))
                        {
                            // do nothing because one of the items is flagged
                        }
                        else
                        {
                            tempID = std::to_string(xml[x].id);
                            while (tempID.length() < 6)          // 6 is CCI-XXXXXX
                            {
                                tempID.insert(0, "0");          // adds preceding zeros
                            }
                            tempID.insert(0, "CCI-");
                            if (IdCollection.size() == 0)       // add if this is the first element
                                IdCollection.push_back(tempID); // add this item to tempID
                            else if (IdCollection.back() == tempID) // do not add if the new element is the same as the last
                            {
                                // do nothing
                            }
                            else
                            {
                                IdCollection.push_back(tempID); // add this item to tempID
                            }
                        }
                    }
                }
            }
        }
    }
    if (!keywordFound)
        std::cout << "No files found with this keyword." << std::endl;
}

void printFixedLine()
{
    const int MENU_LENGTH = 67;
    for (int i = 0; i < MENU_LENGTH; i++)
        std::cout << '-';
    std::cout << std::endl;

}

void helpMenu(std::vector<std::string>& arguments)
{
    std::string line;
    std::ifstream helpfile;
    helpfile.open("help.man");
    if (helpfile)
    {
        std::cout << "\nCCI Helper App" << std::endl << std::endl;
        std::cout << "USAGE: " << arguments[0] << " ";
        while (!helpfile.eof())
        {
            std::getline(helpfile, line);
            std::cout << line << std::endl;
        }
    }
    else
    {
        std::cout << "Help file is missing!";
    }

}

std::string toUpper(std::string input)
{
    std::transform(input.begin(), input.end(), input.begin(), ::toupper);
    return input;
}

void showLicense()
{
    std::string line;
    std::ifstream file;
    file.open("LICENSE");
    if (file)
    {
        while (!file.eof())
        {
            std::getline(file, line);
            std::cout << line << std::endl;
        }
    }
    else
    {
        std::cout << "License file is missing!";
    }

}

void DownloadFile(bool& fileExistsFlag)
{
    displayProgramTag();
    std::cout << "The file does not exist in the current directory." << std::endl;
    std::cout << "Do you want to execute a python script to download the file for you?" << std::endl;
    std::cout << "(Y/n) >> ";
    std::string input;
    std::getline(std::cin >> std::ws, input);
    if (input == "y" || "Y")
        ExecutePythonScript();
    fileExistsFlag = FileExists();
    if (fileExistsFlag)
        std::cout << "File downloaded successfully." << std::endl;
    else
        std::cout << "Unable to download file." << std::endl;
}

bool FileExists()
{
    std::filesystem::path myFilePath("U_CCI_List.xml");
    if (std::filesystem::exists(myFilePath))
    {
        return true;
    }
    else
    {
        return false;
    }
}
