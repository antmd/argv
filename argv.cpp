// Program arguments
//
// Copyright (C) 2003-2006
// Jeff Perry
// Center for Perceptual Systems
// University of Texas at Austin
//
// jsp Thu Aug 24 18:56:40 CDT 2006

#include "argv.h"
#include <cassert>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using namespace std;

namespace jsp
{

Arg::Arg (const string &long_name,
    char short_name,
    const string &description,
    const void *address,
    bool flag,
    const string &long_arg_name,
    const string &short_arg_name) :
    long_name (long_name),
    short_name (short_name),
    description (description),
    address (address),
    flag (flag),
    long_arg_name (long_arg_name),
    short_arg_name (short_arg_name)
{
}

string Arg::LongName () const { return long_name; }

char Arg::ShortName () const { return short_name; }

string Arg::Description () const { return description; }

const void *Arg::Address () const { return address; }

bool Arg::Flag () const { return flag; }

string Arg::LongArgName () const { return long_arg_name; }

string Arg::ShortArgName () const { return short_arg_name; }

string Arg::GetStr () const { return str; }

void Arg::SetStr (const string &s) { str = s; }

void CommandLine::AddSpec (const string &long_name,
    char short_name,
    const bool &value,
    const string &description)
{
    args.push_back (
        Arg (long_name, short_name, description, static_cast<const void *> (&value), true, "", ""));
}

string CommandLine::Usage () const
{
    stringstream ss;
    for (unsigned i = 0; i < args.size (); ++i)
    {
        bool sne = args[i].ShortName () == '\0';
        bool lne = args[i].LongName ().empty ();
        if (!sne || !lne)
        {
            // Print an option spec
            ss << "[";
            if (!sne)
            {
                ss << "-" << args[i].ShortName ();
                if (!args[i].ShortArgName ().empty ())
                    ss << args[i].ShortArgName ();
            }
            if (!sne && !lne)
                ss << ",";
            if (!lne)
            {
                ss << "--" << args[i].LongName ();
                if (!args[i].LongArgName ().empty ())
                    ss << "=" << args[i].LongArgName ();
            }
            ss << "] ";
        }
        else
        {
            // Print an argument spec
            if (!args[i].LongArgName ().empty ())
            {
                ss << args[i].LongArgName ();
                if (i + 1 < args.size ())
                    ss << " ";
            }
        }
    }

    return ss.str ();
}

string CommandLine::Help (const string &indent, unsigned width) const
{
    stringstream ss;
    for (unsigned i = 0; i < args.size (); ++i)
    {
        bool sne = args[i].ShortName () == '\0';
        bool lne = args[i].LongName ().empty ();
        if (!sne || !lne)
        {
            // temp sstring for spec
            stringstream oss1;
            // Print an option spec
            if (!sne)
            {
                oss1 << "-" << args[i].ShortName ();
                if (!args[i].ShortArgName ().empty ())
                    oss1 << args[i].ShortArgName ();
            }
            if (!sne && !lne)
                oss1 << ",";
            if (!lne)
            {
                oss1 << "--" << args[i].LongName ();
                if (!args[i].LongArgName ().empty ())
                    oss1 << "=" << args[i].LongArgName ();
            }
            // temp sstring for for manipulating justification and
            // width
            stringstream oss2;
            oss2 << indent;
            oss2.setf (ios::left);
            oss2.width (width);
            oss2 << oss1.str ();
            // Send it to full help sstring
            ss << oss2.str ();
            ss << args[i].Description ();
            ss << endl;
        }
    }

    return ss.str ();
}

void CommandLine::GroupArgs (int argc, char **argv, int begin, int end)
{
    if (end < 0)
        end = argc;
    // The args that are left after everything gets parsed get
    // saved in a vector of strings.
    leftover_args.clear ();
    // This flag is for the special '--' option.
    bool no_more_options = false;
    for (int i = begin; i < end; )
    {
        // Consume argument 'i'
        int j = ProcessArgv (argc, argv, i, &no_more_options);
        // Skip to next unparsed arg
        i = j;
    }
}

void CommandLine::ExtractBegin ()
{
    // Start with an empty map
    arg_map.clear ();
    // Put all vector values in an assocative container that maps
    // the variable address to the arg.
    for (unsigned i = 0; i < args.size (); ++i)
        arg_map.insert (make_pair (args[i].Address (), &args[i]));
}

void CommandLine::ExtractEnd ()
{
    if (!arg_map.empty ())
        throw runtime_error ("Not all values were extracted");
}

vector<string> CommandLine::GetLeftOverArgs () const { return leftover_args; }

int CommandLine::ProcessArgv (int argc, char *argv[], int current_argv, bool *no_more_options)
{
    // If '--' was specified, parse an arg
    if (*no_more_options)
    {
        ProcessArg (argc, argv, current_argv);
        return current_argv + 1;
    }
    // otherwise, check to see if '--' was specified
    else if (argv[current_argv] &&
        argv[current_argv][0] == '-' &&
        argv[current_argv][1] == '-' &&
        argv[current_argv][2] == '\0')
    {
        *no_more_options = true;
        return current_argv + 1;
    }
    // otherwise, see if it's a long opt
    else if (argv[current_argv] &&
        argv[current_argv][0] == '-' &&
        argv[current_argv][1] == '-')
    {
        ProcessLongOpt (argc, argv, current_argv);
        return current_argv + 1;
    }
    // otherwise, see if it's a short opt
    //
    // Note that '-' by itself is a regular arg
    else if (argv[current_argv] &&
        argv[current_argv][0] == '-' &&
        argv[current_argv][1] != '\0')
    {
        int next_argv = ProcessShortOpt (argc, argv, current_argv);
        return next_argv;
    }
    // otherwise, it's a regular arg
    else
    {
        ProcessArg (argc, argv, current_argv);
        return current_argv + 1;
    }
}

void CommandLine::ProcessArg (int argc, char *argv[], int current_argv)
{
    assert (current_argv < argc);
    assert (argv[current_argv]);
    for (unsigned i = 0; i < args.size (); ++i)
    {
        if (args[i].LongName ().empty () &&
            args[i].ShortName () == '\0' &&
            args[i].GetStr ().empty ())
        {
            // Set this arg and then we're done...
            args[i].SetStr (argv[current_argv]);
            return;
        }
    }
    // If we get here, the argument was not specified.  That's OK,
    // many programs want an unspecified number fo command line
    // arguments, so we will savce them here...
    leftover_args.push_back (argv[current_argv]);
}

void CommandLine::ProcessLongOpt (int argc, char *argv[], int current_argv)
{
    assert (current_argv < argc);
    assert (argv[current_argv]);
    assert (argv[current_argv][0]);
    assert (argv[current_argv][1]);
    assert (argv[current_argv][2]);
    // Get the arg into a string
    string s (&argv[current_argv][2]);
    string option_arg;
    // See if it has an '='
    string::size_type idx = s.find ("=");
    if (idx != string::npos)
    {
        // We found a '=', so break it into two parts
        option_arg = s.substr (idx + 1);
        s = s.substr (0, idx);
    }
    // Find the option
    bool found = false;
    for (unsigned i = 0; i < args.size (); ++i)
    {
        if (!args[i].LongName ().empty () &&
            args[i].LongName () == s)
        {
            // We found it
            found = true;
            // Set it.
            if (!args[i].Flag () && option_arg.empty ())
            {
                // Make sure it is a flag if no '=' was given
                throw runtime_error ("Missing long option argument");
            }
            if (!option_arg.empty ())
                args[i].SetStr (option_arg);
            else
                args[i].SetStr ("1");
            // Don't break here.  We may find it again, in which
            // case this value will get overwritten
        }
    }
    // See if we got an invalid option
    if (!found)
    {
        string e ("Invalid long option: ");
        throw runtime_error (e + s);
    }
}

int CommandLine::ProcessShortOpt (int argc, char *argv[], int current_argv)
{
    assert (current_argv < argc);
    assert (argv[current_argv]);
    assert (argv[current_argv][0]);
    assert (argv[current_argv][1]);
    char *opts = &argv[current_argv][1];
    // For each single letter option...
    for (unsigned i = 0; opts[i] != '\0'; ++i)
    {
        // Find the option
        bool found = false;
        for (unsigned j = 0; j < args.size (); ++j)
        {
            if (args[j].ShortName () == opts[i])
            {
                // We found it
                found = true;
                if (!args[j].Flag ())
                {
                    // If it's not a flag, find the option's
                    // argument
                    if (opts[i+1] != '\0')
                    {
                        // If the option letter is not the last char
                        // in the argv, use the remaining chars as
                        // the option argument
                        args[j].SetStr (&opts[i + 1]);
                        // And skip over the remaining chars in
                        // the argv
                        i += static_cast<unsigned> (args[j].GetStr ().size ());
                        assert (opts[i + 1] == '\0');
                    }
                    else if (current_argv + 1 < argc &&
                        argv[current_argv + 1] &&
                        argv[current_argv + 1][0] != '-')
                    {
                        // Otherwise, if the next argv is not an
                        // option, use this value as the option
                        // argument
                        ++current_argv;
                        args[j].SetStr (argv[current_argv]);
                    }
                    else
                    {
                        // All options require arguments except bools
                        throw runtime_error ("Missing short option argument");
                    }
                }
                else
                {
                    // No option arguments.  Set the option to 'true'
                    args[j].SetStr ("1");
                }
            }
        }
        // See if we got an invalid option
        if (!found)
        {
            string e ("Invalid short option: ");
            throw runtime_error (e + opts[i]);
        }
    }
    return current_argv + 1;
}

} // namespace jsp
