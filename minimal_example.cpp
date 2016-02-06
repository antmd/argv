// Another example of using argv module
//
// This is a typical minimal use or argv
//
// Copyright (C) 2003-2007
// Jeff Perry
// Center for Perceptual Systems
// University of Texas at Austin
//
// jsp Tue May 15 11:19:22 CDT 2007

#include "argv.h"
#include <iostream>
#include <stdexcept>

using namespace jsp;
using namespace std;

int main (int argc, char *argv[])
{
    try
    {
        bool help = false;
        unsigned model_number = 1;
        string inputfn;

        CommandLine cl;
        cl.AddSpec ("help", 'h', help, "Print help message");
        cl.AddSpec ("model", 'm', model_number, "Model Number", "#", "#");
        cl.AddSpec ("", '\0', inputfn, "Input filename", "FN");
        // Group argv's into option groups
        cl.GroupArgs (argc, argv, 1);
        // Convert from strings to their proper type
        cl.ExtractBegin ();
        cl.Extract (help);
        cl.Extract (model_number);
        cl.Extract (inputfn);
        cl.ExtractEnd ();

        if (help || inputfn.empty () || !cl.GetLeftOverArgs ().empty ())
            throw runtime_error ("usage: " + string (argv[0]) + " " + cl.Usage ());

        cout << "inputfn: " << inputfn << endl;
        cout << "model: " << model_number << endl;

        return 0;
    }
    catch (const exception &e)
    {
        cerr << "Exception: " << e.what () << endl;
        return -1;
    }
}
