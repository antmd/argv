// Test program options
//
// Copyright (C) 2003-2006
// Jeff Perry
// Center for Perceptual Systems
// University of Texas at Austin
//
// jsp Fri Aug 25 10:46:53 CDT 2006

#include "argv.h"
#include "verify.h"
#include <iostream>
#include <stdexcept>
#include <vector>

using namespace jsp;
using namespace jsp::utils;
using namespace std;

void test1 ()
{
    bool help = false;
    int x = 0;
    int y = 0;
    double f = 2.718;
    double z = 3.14;
    string fn;

    CommandLine cl;
    cl.AddSpec ("help",     'h',    help,   "Show help");
    cl.AddSpec ("x",        'x',    x,      "X coord");
    cl.AddSpec ("ycoord",   'y',    y,      "Y coord");
    cl.AddSpec ("fval",     '\0',   f,      "F Value");
    cl.AddSpec ("zvalue",   'z',    z,      "Z Value");
    cl.AddSpec ("",         '\0',   fn,     "Filename");

    char *argv[] = { "test.exe", "-h", "--x=-2", "-y", "3", "--fval=1.1", "asdf" };
    int argc = sizeof (argv) / sizeof (char *);

    cl.GroupArgs (argc, argv, 1);
    cl.ExtractBegin ();
    cl.Extract (help);
    cl.Extract (x);
    cl.Extract (y);
    cl.Extract (f);
    cl.Extract (z);
    cl.Extract (fn);
    cl.ExtractEnd ();

    VERIFY (help == true);
    VERIFY (x == -2);
    VERIFY (y == 3);
    VERIFY (f == 1.1);
    VERIFY (z == 3.14);
    VERIFY (fn == argv[6]);
}

void test2 ()
{
    bool help = false;
    bool one = false;
    int x = 0;
    int y = 0;
    double f = 2.718;
    double z = 3.14;
    string fn1;
    string fn2;

    CommandLine cl;
    cl.AddSpec ("help", 'h', help, "Show help");
    cl.AddSpec ("one", '1', one, "One");
    cl.AddSpec ("x", 'x', x, "X coord");
    cl.AddSpec ("", 'y', y, "Y coord");
    cl.AddSpec ("fval", 'f', f, "F Value");
    cl.AddSpec ("zvalue", 'z', z, "Z Value");
    cl.AddSpec ("", '\0', fn1, "file1");
    cl.AddSpec ("", '\0', fn2, "file2");

    char *argv[] = { "test.exe", "-x-1", "-hhy-2", "-1", "--", "-file1" ,"-z"};
    int argc = sizeof (argv) / sizeof (char *);

    cl.GroupArgs (argc, argv, 1);
    cl.ExtractBegin ();
    cl.Extract (help);
    cl.Extract (one);
    cl.Extract (x);
    cl.Extract (y);
    cl.Extract (f);
    cl.Extract (z);
    cl.Extract (fn1);
    cl.Extract (fn2);
    cl.ExtractEnd ();

    VERIFY (help == true);
    VERIFY (one == true);
    VERIFY (x == -1);
    VERIFY (y == -2);
    VERIFY (f == 2.718);
    VERIFY (z == 3.14);
    VERIFY (fn1 == "-file1");
    VERIFY (fn2 == "-z");
}

void test3 ()
{
    int x = 0;

    CommandLine cl;
    cl.AddSpec ("x", 'x', x, "X");

    {
    char *argv[] = { "test.exe", "-y" };
    int argc = sizeof (argv) / sizeof (char *);

    bool caught = false;
    try { cl.GroupArgs (argc, argv, 1);}
    catch (const exception &e)
    {
        // An invalid option was specified
        //cerr << e.what () << endl;
        e.what (); // get rid of warning
        caught = true;
    }

    VERIFY (caught);
    }

    {
    char *argv[] = { "test.exe", "--x", "asdf" };
    int argc = sizeof (argv) / sizeof (char *);

    bool caught = false;
    try { cl.GroupArgs (argc, argv, 1);}
    catch (const exception &e)
    {
        // No option arg was given
        //cerr << e.what () << endl;
        e.what (); // get rid of warning
        caught = true;
    }

    VERIFY (caught);
    }
}

void test4 ()
{
    int x = 0;
    string fn1;

    CommandLine cl;
    cl.AddSpec ("x", 'x', x, "X");
    cl.AddSpec ("", '\0', fn1, "Filename 1");

    char *argv[] = { "test.exe", "-x100", "-", "fn2", "fn3" };
    int argc = sizeof (argv) / sizeof (char *);

    cl.GroupArgs (argc, argv, 1);
    cl.ExtractBegin ();
    cl.Extract (x);
    cl.Extract (fn1);
    cl.ExtractEnd ();

    VERIFY (x == 100);
    VERIFY (fn1 == "-");
    // There are two leftover args
    VERIFY (cl.GetLeftOverArgs ().size () == 2);
    VERIFY (cl.GetLeftOverArgs ()[0] == "fn2");
    VERIFY (cl.GetLeftOverArgs ()[1] == "fn3");
}

void test5 ()
{
    int x = 0;
    bool f = 0;

    CommandLine cl;
    cl.AddSpec ("x", 'x', x, "X");
    cl.AddSpec ("f", 'f', f, "F");

    char *argv[] = { "test.exe",
        "-x", "1", "-f",
        "--f=0", "-x10",
        "-fx", "11",
        "-z",
        "-fff", "-x12",
        "asdf"
        };
    int argc = sizeof (argv) / sizeof (char *);

    cl.GroupArgs (argc, argv, 1, 4);
    cl.ExtractBegin (); cl.Extract (x); cl.Extract (f); cl.ExtractEnd ();
    VERIFY (x == 1);
    VERIFY (f == 1);

    cl.GroupArgs (argc, argv, 4, 6);
    cl.ExtractBegin (); cl.Extract (x); cl.Extract (f); cl.ExtractEnd ();
    VERIFY (x == 10);
    VERIFY (f == false);

    cl.GroupArgs (argc, argv, 6, 8);
    cl.ExtractBegin (); cl.Extract (x); cl.Extract (f); cl.ExtractEnd ();
    VERIFY (x == 11);
    VERIFY (f == true);

    f = 0;
    cl.GroupArgs (argc, argv, 9, 11);
    cl.ExtractBegin (); cl.Extract (x); cl.Extract (f); cl.ExtractEnd ();
    VERIFY (x == 12);
    VERIFY (f == true);
}

void test6 ()
{
    int x = 0;

    CommandLine cl;
    cl.AddSpec ("x", 'x', x, "X");

    char *argv[] = { "test.exe" };
    int argc = sizeof (argv) / sizeof (char *);

    cl.GroupArgs (argc, argv, 1);
    cl.ExtractBegin ();

    bool caught = false;
    try
    {
        cl.ExtractEnd ();
    }
    catch (const exception &e)
    {
        // Not all values were extracted
        //cerr << e.what () << endl;
        e.what (); // get rid of warning
        caught = true;
    }

    VERIFY (caught);
}

int main ()
{
    try
    {
        test1 ();
        test2 ();
        test3 ();
        test4 ();
        test5 ();
        test6 ();

        return 0;
    }
    catch (const exception &e)
    {
        cerr << "Exception: " << e.what () << endl;
        return -1;
    }
}

