#include <stdio.h>
#include <Python.h>
#include <filesystem>
#include <iostream>
#include <string>

#ifdef _WIN32
const std::string os = "Windows";
#elif __linux__
const std::string os = "Linux";
#elif __APPLE__
const std::string os = "MacOS";
#else
const std::string os = "Unknown";
#endif

void ExecutePythonScript()
{
    // Initialize the Python interpreter
    Py_Initialize();
    const char* script = "dl.py";
    // Load and execute the Python script
    FILE* file;
if ((file = fopen(script, "r")) != NULL)

    {
        PyRun_SimpleFile(file, script);
        fclose(file);
    }
    else 
    {
        // Handle error opening file
        std::cout << "Missing script: " << script << std::endl;
        Py_Finalize();
        return;
    }

    // Handle any returned values or errors
    PyObject* pName, * pModule, * pFunc;

    // Get the module object
    pName = PyUnicode_FromString("dl");
    pModule = PyImport_Import(pName);
    Py_DECREF(pName);

    if (pModule != NULL) {
        // Get the function object
        pFunc = PyObject_GetAttrString(pModule, "download_files");

        if (pFunc && PyCallable_Check(pFunc)) {
            // Call the Python function and handle any exceptions
            PyObject_CallObject(pFunc, NULL);
            if (PyErr_Occurred()) {
                PyErr_Print();
            }
            Py_DECREF(pFunc);
        }
        else {
            // Handle error getting function object
            if (PyErr_Occurred()) {
                PyErr_Print();
            }
        }
        Py_DECREF(pModule);
    }
    else {
        // Handle error importing module
        if (PyErr_Occurred()) {
            PyErr_Print();
        }
    }

    // Clean up and finalize the Python interpreter
    Py_Finalize();

}

void CleanUpCache()
{
    
    std::string folderPath = "__pycache__"; // Replace with your folder path
    std::string command;

    if(os == "Linux" || os == "MacOS")
         command = "rm -rf " + folderPath; // Linux/Unix command to delete folder recursively
    else if (os == "Windows")
        command = "del /s /q /f " + folderPath;
    

    int result = system(command.c_str()); // Execute the command

    if (os == "Windows")
    {
        command = "rmdir " + folderPath;
        system(command.c_str()); // Execute the command
    }
        
/*
if (result == 0)
    {
        std::cout << "Cache folder deleted successfully." << std::endl;
    }
    else
    {
        std::cout << "Error deleting folder." << std::endl;
    }
*/

}
