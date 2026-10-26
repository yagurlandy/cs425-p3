# Project 3

- Name: Andy Lopez-Martinez
- Email: andylopezmartine@u.boisestate.edu
- Class: CS425-001

## Overview

For this project, I built a program that works with IPv4 addresses
and forwarding tables. It can show subnet information, split a network
into smaller subnets, and find the best route for a destination address.

## Building and Running

To build the program:

    make all

To get information about a subnet:

    ./build/release/myapp info 132.178.227.11/25

To split a network into four subnets:

    ./build/release/myapp split 192.168.1.0/24 4

To look up addresses in a forwarding table:

    ./build/release/myapp lookup tables/example.txt 8.8.8.8 10.1.2.200

Running the program without arguments shows the usage instructions.

## Design

I split the code into address parsing, subnet calculations, and
forwarding table functions. The command-line code calls those
functions based on which command the user enters.

The address parser reads each octet separately and checks that it is
between 0 and 255. It rejects leading zeros, missing parts, and extra
characters. If parsing fails, the original output value stays unchanged.

For the subnet calculations, I used masks and bitwise operations.
I handled /0 separately because shifting a 32-bit number by 32 would
cause undefined behavior. I also used 64-bit arithmetic for the host
count so the /0 calculation would fit.

The /31 and /32 cases needed their own rules. Both addresses in a /31
are usable, while a /32 is just one host. For splitting, the count has
to be a power of two, and the new prefix cannot go past 32.

The forwarding table stores copies of routes in an array that grows
with realloc. It rejects duplicate destination blocks. When looking
up an address, it checks every route and keeps the match with the
longest prefix. That way, the order the routes were added does not
change the answer.

The file loader removes comments and skips blank lines. If a route
is invalid, it reports the line number and keeps the routes loaded
before it. The line buffer and table memory are freed when finished.

## Testing

To run the tests and check coverage:

    make check
    make report

To check for leaks and memory errors:

    make leak
    make leak-test

All 20 Unity tests passed in Codespaces. The tests cover valid and
invalid addresses, formatting, subnet calculations, splitting, and
the /0, /31, and /32 boundaries.

I also tested route parsing, duplicate routes, table growth, and
longest-prefix matching. Adding the same routes in reverse order
gave the same lookup results. File-loading tests checked comments,
blank lines, duplicate entries, and a bad line in the middle.

The command-line checks returned the expected errors and exit codes.
A lookup with an invalid address still processed the other addresses.
Both sanitizer checks finished without reporting errors.

## Known Bugs or Issues

I have not found any functional issues in the tests so far.
The project builds on both Codespaces and Onyx. All 20 tests and both
sanitizer checks passed on both systems. GitHub CI also passed.

The submission report workflow failed during Python setup because
it could not find Python 3.12.7 on Ubuntu 26.04. I changed the report
runner from ubuntu-latest to ubuntu-22.04 to address this. Confirmed that 
the report generated successfully.

## Experience

I was really interested in this project because I got to build something 
we’ve been learning about in class and see it work. Subnetting makes more 
sense to me when I can follow what the code is doing and see how changing 
the prefix changes the network and host range.

The forwarding table was especially interesting to me. I liked seeing how an 
address could match several routes, but the program would pick the most 
specific one. Working through everything in smaller parts helped, 
especially with the /0, /31, and /32 cases. It was satisfying to finally 
see all the tests pass and get the results I expected!

## Analysis

For 132.178.227.11/25, the program returned 132.178.227.0 as the
network address and 132.178.227.127 as the broadcast address.
The usable range was 132.178.227.1 through 132.178.227.126,
which gives 126 hosts.

Splitting 192.168.1.0/24 into four parts returned four /26 networks.
Their starting addresses were .0, .64, .128, and .192, which matches
the expected spacing of 64 addresses per subnet.

The lookup results also matched the examples. Even though
10.1.2.200 matched multiple routes, the program picked the /25
because it was the most specific. For 8.8.8.8, it used the default
route because there was no more specific match.

The coverage report showed 100% line coverage for lab.c, with the
realloc failure return excluded. This means the tests ran every
included line, but it does not mean every possible case was tested.
