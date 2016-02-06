// Program arguments
//
// Copyright (C) 2003-2006
// Jeff Perry
// Center for Perceptual Systems
// University of Texas at Austin
//
// jsp Thu Aug 24 18:56:40 CDT 2006

#ifndef ARGV_H
#define ARGV_H

#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace jsp
{

// Terms
//
// OPTION: command line option, like '-h' or '--radius=1.2'
//
// OPTION ARGUMENT: the optional argument of an option, like '1.2'
// in the option '--radius=1.2'
//
// ARGUMENT: command line argument that is not an option, like
// 'r+a' or 'file1' in the command 'chmod -R r+a file1'

// An 'Arg' specifies an option, an option with an option argument, or an
// argument that is not an option.
class Arg
{
    public:
    // 'long_name' is the POSIX-style name, and does not contain the
    // '--' prefix.
    //
    // 'short_name' is the traditional, one character name, and does
    // not contain the '-' prefix.
    //
    // 'description' is the description that will be shown in the
    // help.
    //
    // 'address' is the address of the variable that will contain the
    // value that gets extracted from the command line.
    //
    // 'flag' indicates whether or not this argument allows option
    // arguments.
    Arg (const std::string &long_name,
        char short_name,
        const std::string &description,
        const void *address,
        bool flag,
        const std::string &long_arg_name,
        const std::string &short_arg_name);
    // Access to private data
    std::string LongName () const;
    char ShortName () const;
    std::string Description () const;
    const void *Address () const;
    bool Flag () const;
    std::string LongArgName () const;
    std::string ShortArgName () const;
    // The 'str' data contain the string from the command line that is
    // associated with this argument.
    std::string GetStr () const;
    void SetStr (const std::string &s);
    private:
    std::string long_name;
    char short_name;
    std::string description;
    const void *address;
    std::string str;
    bool flag;
    std::string long_arg_name;
    std::string short_arg_name;
};

class CommandLine
{
    public:
    // Booleans are flags and therefore do not have argument options.
    void AddSpec (const std::string &long_name,
        char short_name,
        const bool &value,
        const std::string &description);
    // Anything that is not a boolean must specify an argument option.
    template <typename T>
    void AddSpec (const std::string &long_name,
        char short_name,
        const T &value,
        const std::string &description,
        const std::string &long_arg_name = "ARG",
        const std::string &short_arg_name = " ARG")
    {
        args.push_back (
            Arg (long_name, short_name, description,
                static_cast<const void *> (&value), false, long_arg_name, short_arg_name));
    }
    // Get a usage string
    std::string Usage () const;
    // Get a help string
    std::string Help (const std::string &indent = "\t", unsigned width = 25) const;
    // Group command line arguments
    void GroupArgs (int argc, char **argv, int begin = 1, int end = -1);
    // You must call 'ExtractBegin' before calling 'Extract'
    void ExtractBegin ();

    // Get an argument from the command line
    template <typename T>
    void Extract (T &value)
    {
        if (arg_map.empty ())
            throw std::runtime_error ("No arguments were found (was ExtractBegin() called?)");
        if (arg_map.find (&value) == arg_map.end ())
            throw std::runtime_error ("The specified value address was not found");
        if (!arg_map[&value]->GetStr ().empty ())
        {
            std::stringstream ss (arg_map[&value]->GetStr ());
            ss >> value;
        }
        arg_map.erase (&value);
    }
    // You must call 'ExtractEnd' after all calls to 'Extract'
    void ExtractEnd ();
    // Get any args that were not specified
    std::vector<std::string> GetLeftOverArgs () const;

    private:
    std::vector<Arg> args;
    std::vector<std::string> leftover_args;
    std::map<const void *, const Arg *> arg_map;
    int ProcessArgv (int argc, char *argv[], int current_argv, bool *no_more_options);
    void ProcessArg (int argc, char *argv[], int current_argv);
    void ProcessLongOpt (int argc, char *argv[], int current_argv);
    int ProcessShortOpt (int argc, char *argv[], int current_argv);
};

} // namespace jsp

#endif // ARGV_H
