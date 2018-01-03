# zimbra2gmail

> `zimbra2gmail` is a compiler written in C language; and as a project
> available in github.com, it can also be used to learn the steps used
> in a construction of a simple compiler: a top-down, predictif compiler
> composed by:
> a lexical analyser,
> a sintax analyser and
> a semantic analyser with code generation

## What is zimbra2gmail for?
zimbra2gmail is written to have, as its source files, the Zimbra users
filters; and as its target, the gmail users filters.

Given the gmail users filters as text file, we can use a tool like gam
to apply this filters to gmail. Without a compiler like zimbra2gmail,
this task could be hard work for a single user account; and, me and
my colleagues, we have thousands of accounts to migrate from zimbra
to gmail.

## Usage

It is a project that just started, and is expected to become inactif during
these last days of 2017 (Christmas and New Year). It should be ready to use
in January, 2018.

Anyone can already use this project just as a learning (or teaching) tool
of basic compiler principles.

## Installation instructions

There is a file called compile.sh in this project; it is a Bash script with
the commands used to compile this project files with gcc. Your machine should
have gcc already installed.

After running compile.sh script, you should have availabe the executable file:

zimbra2gmail

In short, run these commands to compile and see it in action:

./compile.sh

./zimbra2gmail source-test-1

./zimbra2gmail source-test-2

Remember again: for the moment these project has only implemented the lexical analyser.

## LICENSE

*zimbra2gmail* is licensed under [GNU GPL v3](https://www.gnu.org/licenses/gpl-3.0.txt)



