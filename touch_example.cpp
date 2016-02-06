// An example of using argv module
//
// Copyright (C) 2003-2006
// Jeff Perry
// Center for Perceptual Systems
// University of Texas at Austin
//
// jsp Fri Aug 25 10:46:53 CDT 2006

#include "argv.h"
#include <iostream>
#include <stdexcept>

using namespace jsp;
using namespace std;

// This example will mimic /usr/bin/touch command line arguments:
//
// Usage: touch [OPTION]... FILE...
// Update the access and modification times of each FILE to the current time.
//
// Mandatory arguments to long options are mandatory for short options too.
//   -a                     change only the access time
//   -c, --no-create        do not create any files
//   -d, --date=STRING      parse STRING and use it instead of current time
//   -f                     (ignored)
//   -m                     change only the modification time
//   -r, --reference=FILE   use this file's times instead of current time
//   -t STAMP               use [[CC]YY]MMDDhhmm[.ss] instead of current time
//   --help                 display this help and exit
//   --version              output version information and exit

int main (int argc, char *argv[])
{
    try
    {
        bool a = false;
        bool nocreate = false;
        string date;
        bool f = false;
        bool m = false;
        string reference;
        string t;
        bool help = false;
        bool version = false;
        string fn;

        CommandLine cl;

        cl.AddSpec ("",          'a', a,
            "change only the access time");
        cl.AddSpec ("no-create", 'c', nocreate,
            "do not create any files");
        cl.AddSpec ("date",      'd', date,
            "parse STRING and use it instead of current time", "STRING","");
        cl.AddSpec ("",          'f', f,
            "(ignored)");
        cl.AddSpec ("",          'm', m,
            "change only the modification time");
        cl.AddSpec ("reference", 'r', reference,
            "use this file's times instead of current time", "FILE", "");
        cl.AddSpec ("",          't', t,
            "use [[CC]YY]MMDDhhmm[.ss] instead of current time", "", " STAMP");
        cl.AddSpec ("help",      '\0',  help,
            "display this help and exit");
        cl.AddSpec ("version",   '\0',  version,
            "output version information and exit");
        cl.AddSpec ("",          '\0',  fn,
            "Target file", "FILE...");

        // Group argv's into option groups
        try
        {
            cl.GroupArgs (argc, argv, 1);
        }
        catch (const exception &e)
        {
            cerr << e.what () << endl;
            cerr << "Usage: " << argv[0] << " " << cl.Usage () << endl;
            cerr << "Try `" << argv[0] << " --help' for more information" << endl;
            return -1;
        }

        // Convert from strings to their proper type
        cl.ExtractBegin ();
        cl.Extract (a);
        cl.Extract (nocreate);
        cl.Extract (date);
        cl.Extract (f);
        cl.Extract (m);
        cl.Extract (reference);
        cl.Extract (t);
        cl.Extract (help);
        cl.Extract (version);
        cl.Extract (fn);
        cl.ExtractEnd ();

        if (help)
        {
            cout << "Usage: " << argv[0] << " [OPTION]... FILE..." << endl;
            cout << "Mimic command line options of the program 'touch'." << endl << endl;
            cout << "Mandatory arguments to long options are mandatory for short options too." << endl;
            cout << cl.Help ("  ", 23) << endl;
            cout << "Note that the -d and -t options accept different time-date formats." << endl << endl;
            cout << "If a FILE is -, touch standard output." << endl << endl;
            cout << "Report bugs to <jeffsp@gmail.com>." << endl;
            return 0;
        }

        if (version)
        {
            cout << "version 1.0.0" << endl;
            return 0;
        }

        if (fn.empty ())
        {
            cerr << argv[0] << ": missing file operand" << endl;
            cerr << "Try `" << argv[0] << " --help' for more information" << endl;
            return -1;
        }

        cout << "a: " << a << endl;
        cout << "nocreate: " << nocreate << endl;
        cout << "date: " << date << endl;
        cout << "f: " << f << endl;
        cout << "m: " << m << endl;
        cout << "reference: " << reference << endl;
        cout << "t: " << t << endl;
        cout << "FILE[0]: " << fn << endl;
        vector<string> leftover_args = cl.GetLeftOverArgs ();
        for (unsigned i = 0; i < leftover_args.size (); ++i)
            cout << "FILE[" << i + 1 << "]: " << leftover_args[i] << endl;

        return 0;
    }
    catch (const exception &e)
    {
        cerr << "Exception: " << e.what () << endl;
        return -1;
    }
}
