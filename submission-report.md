# Submission Report

- Submission generated at 10/26/2026 at 16:34:56

- Machine info: Linux runnervmwhx2h 6.8.0-1064-azure #72~22.04.1-Ubuntu SMP Wed Jul 22 23:39:45 UTC 2026 x86_64 x86_64 x86_64 GNU/Linux

## Note to Students

Please read this report carefully before submission.
Ensure that all sections are complete and accurate.
Look for any errors in the build or test outputs.
If you find any issues, correct them before submitting.
Post any questions on the class discussion board for help.


---

## README

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
runner from ubuntu-latest to ubuntu-22.04 to address this. I still
need to confirm that the report generates successfully.

## Experience

I was really interested in this project because I got to build something we’ve been learning about in class and see it work. Subnetting makes more sense to me when I can follow what the code is doing and see how changing the prefix changes the network and host range.

The forwarding table was especially interesting to me. I liked seeing how an address could match several routes, but the program would pick the most specific one. Working through everything in smaller parts helped, especially with the /0, /31, and /32 cases. It was satisfying to finally see all the tests pass and get the results I expected!

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

---


## Build Output

This section was generated by running `make all` in the project root directory.

```bash
make[1]: Entering directory '/home/runner/work/cs425-p3/cs425-p3'
mkdir -p build/debug
cc -g -O0 -DDEBUG -fno-omit-frame-pointer -fsanitize=address -c src/main.c -o build/debug/main.c.o
mkdir -p build/debug
cc -g -O0 -DDEBUG -fno-omit-frame-pointer -fsanitize=address -c src/lab.c -o build/debug/lab.c.o
cc -g -O0 -DDEBUG -fno-omit-frame-pointer -fsanitize=address build/debug/main.c.o build/debug/lab.c.o -o build/debug/myapp_d -fsanitize=address
make[1]: Leaving directory '/home/runner/work/cs425-p3/cs425-p3'
make[1]: Entering directory '/home/runner/work/cs425-p3/cs425-p3'
mkdir -p build/release
cc -Wall -Wextra -O2 -fPIE -MMD -MP -Wformat -Wformat=2 -Wconversion -Wsign-conversion -Wimplicit-fallthrough -fstack-protector-strong -Werror=format-security -Werror=implicit -Werror=incompatible-pointer-types -Werror=int-conversion -c src/main.c -o build/release/main.c.o
mkdir -p build/release
cc -Wall -Wextra -O2 -fPIE -MMD -MP -Wformat -Wformat=2 -Wconversion -Wsign-conversion -Wimplicit-fallthrough -fstack-protector-strong -Werror=format-security -Werror=implicit -Werror=incompatible-pointer-types -Werror=int-conversion -c src/lab.c -o build/release/lab.c.o
cc -Wall -Wextra -O2 -fPIE -MMD -MP -Wformat -Wformat=2 -Wconversion -Wsign-conversion -Wimplicit-fallthrough -fstack-protector-strong -Werror=format-security -Werror=implicit -Werror=incompatible-pointer-types -Werror=int-conversion build/release/main.c.o build/release/lab.c.o -o build/release/myapp 
make[1]: Leaving directory '/home/runner/work/cs425-p3/cs425-p3'
make[1]: Entering directory '/home/runner/work/cs425-p3/cs425-p3'
mkdir -p build/tests
cc -g -O0 -DTEST -fprofile-arcs -ftest-coverage -c src/main.c -o build/tests/main.c.o
mkdir -p build/tests
cc -g -O0 -DTEST -fprofile-arcs -ftest-coverage -c src/lab.c -o build/tests/lab.c.o
mkdir -p build/tests/harness/
cc -g -O0 -DTEST -fprofile-arcs -ftest-coverage -c tests/harness/unity.c -o build/tests/harness/unity.c.o
mkdir -p build/tests/
cc -g -O0 -DTEST -fprofile-arcs -ftest-coverage -c tests/lab-test.c -o build/tests/lab-test.c.o
cc -g -O0 -DTEST -fprofile-arcs -ftest-coverage build/tests/main.c.o build/tests/lab.c.o build/tests/harness/unity.c.o build/tests/lab-test.c.o -o build/tests/myapp_t -fprofile-arcs -ftest-coverage
make[1]: Leaving directory '/home/runner/work/cs425-p3/cs425-p3'
make[1]: Entering directory '/home/runner/work/cs425-p3/cs425-p3'
mkdir -p build/debug-test
cc -g -O0 -DDEBUG -DTEST -fno-omit-frame-pointer -fsanitize=address -c src/main.c -o build/debug-test/main.c.o
mkdir -p build/debug-test
cc -g -O0 -DDEBUG -DTEST -fno-omit-frame-pointer -fsanitize=address -c src/lab.c -o build/debug-test/lab.c.o
mkdir -p build/debug-test/harness/
cc -g -O0 -DDEBUG -DTEST -fno-omit-frame-pointer -fsanitize=address -c tests/harness/unity.c -o build/debug-test/harness/unity.c.o
mkdir -p build/debug-test/
cc -g -O0 -DDEBUG -DTEST -fno-omit-frame-pointer -fsanitize=address -c tests/lab-test.c -o build/debug-test/lab-test.c.o
cc -g -O0 -DDEBUG -DTEST -fno-omit-frame-pointer -fsanitize=address build/debug-test/main.c.o build/debug-test/lab.c.o build/debug-test/harness/unity.c.o build/debug-test/lab-test.c.o -o build/debug-test/myapp_td -fsanitize=address
make[1]: Leaving directory '/home/runner/work/cs425-p3/cs425-p3'
Builds completed. You can run the application with: ./build/release/myapp
You can run the debug build with: ./build/debug/myapp_d
You can run the test build with: ./build/tests/myapp_t
You can run the debug-test build with: ./build/debug-test/myapp_td
```

---

## Coverage Report

This section was generated by running `make report` in the project root directory.

```bash
tests/lab-test.c:500:test_ip_parse_valid:PASS
tests/lab-test.c:501:test_ip_parse_invalid:PASS
tests/lab-test.c:502:test_ip_format:PASS
tests/lab-test.c:503:test_cidr_parse_valid:PASS
tests/lab-test.c:504:test_cidr_parse_invalid:PASS
tests/lab-test.c:505:test_cidr_format:PASS
tests/lab-test.c:506:test_subnet_masks:PASS
tests/lab-test.c:507:test_subnet_arithmetic:PASS
tests/lab-test.c:508:test_boundary_subnets:PASS
tests/lab-test.c:509:test_subnet_split_valid:PASS
tests/lab-test.c:510:test_subnet_split_invalid:PASS
tests/lab-test.c:511:test_route_parse_valid:PASS
tests/lab-test.c:512:test_route_parse_invalid:PASS
tests/lab-test.c:513:test_table_add_and_destroy:PASS
tests/lab-test.c:514:test_longest_prefix_in_both_orders:PASS
tests/lab-test.c:515:test_table_load_valid:PASS
tests/lab-test.c:516:test_table_load_bad_line:PASS
tests/lab-test.c:517:test_table_load_duplicate:PASS
tests/lab-test.c:518:test_table_load_empty_and_null:PASS
tests/lab-test.c:519:test_capacity_boundaries:PASS

-----------------------
20 Tests 0 Failures 0 Ignored 
OK
./build/tests/myapp_t
tests/lab-test.c:500:test_ip_parse_valid:PASS
tests/lab-test.c:501:test_ip_parse_invalid:PASS
tests/lab-test.c:502:test_ip_format:PASS
tests/lab-test.c:503:test_cidr_parse_valid:PASS
tests/lab-test.c:504:test_cidr_parse_invalid:PASS
tests/lab-test.c:505:test_cidr_format:PASS
tests/lab-test.c:506:test_subnet_masks:PASS
tests/lab-test.c:507:test_subnet_arithmetic:PASS
tests/lab-test.c:508:test_boundary_subnets:PASS
tests/lab-test.c:509:test_subnet_split_valid:PASS
tests/lab-test.c:510:test_subnet_split_invalid:PASS
tests/lab-test.c:511:test_route_parse_valid:PASS
tests/lab-test.c:512:test_route_parse_invalid:PASS
tests/lab-test.c:513:test_table_add_and_destroy:PASS
tests/lab-test.c:514:test_longest_prefix_in_both_orders:PASS
tests/lab-test.c:515:test_table_load_valid:PASS
tests/lab-test.c:516:test_table_load_bad_line:PASS
tests/lab-test.c:517:test_table_load_duplicate:PASS
tests/lab-test.c:518:test_table_load_empty_and_null:PASS
tests/lab-test.c:519:test_capacity_boundaries:PASS

-----------------------
20 Tests 0 Failures 0 Ignored 
OK
mkdir -p ./build/report/html
mkdir -p ./build/report/txt
gcovr -r . --html --html-details --exclude-directories build/tests/harness --exclude '.*main\.c$' --exclude '.*test\.c$' -o ./build/report/html/coverage_report.html
gcovr -r . --txt                 --exclude-directories build/tests/harness --exclude '.*main\.c$' --exclude '.*test\.c$'
------------------------------------------------------------------------------
                           GCC Code Coverage Report
Directory: .
------------------------------------------------------------------------------
File                                         Lines     Exec  Cover   Missing
------------------------------------------------------------------------------
src/lab.c                                      212      212   100%
------------------------------------------------------------------------------
TOTAL                                          212      212   100%
------------------------------------------------------------------------------
```

---

## Address Sanitizer Report

This section was generated by running `make leak-test` in the project root directory.

```bash
tests/lab-test.c:500:test_ip_parse_valid:PASS
tests/lab-test.c:501:test_ip_parse_invalid:PASS
tests/lab-test.c:502:test_ip_format:PASS
tests/lab-test.c:503:test_cidr_parse_valid:PASS
tests/lab-test.c:504:test_cidr_parse_invalid:PASS
tests/lab-test.c:505:test_cidr_format:PASS
tests/lab-test.c:506:test_subnet_masks:PASS
tests/lab-test.c:507:test_subnet_arithmetic:PASS
tests/lab-test.c:508:test_boundary_subnets:PASS
tests/lab-test.c:509:test_subnet_split_valid:PASS
tests/lab-test.c:510:test_subnet_split_invalid:PASS
tests/lab-test.c:511:test_route_parse_valid:PASS
tests/lab-test.c:512:test_route_parse_invalid:PASS
tests/lab-test.c:513:test_table_add_and_destroy:PASS
tests/lab-test.c:514:test_longest_prefix_in_both_orders:PASS
tests/lab-test.c:515:test_table_load_valid:PASS
tests/lab-test.c:516:test_table_load_bad_line:PASS
tests/lab-test.c:517:test_table_load_duplicate:PASS
tests/lab-test.c:518:test_table_load_empty_and_null:PASS
tests/lab-test.c:519:test_capacity_boundaries:PASS

-----------------------
20 Tests 0 Failures 0 Ignored 
OK
```

---

## Src Files
### lab.c

```c

#include "lab.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#define _POSIX_C_SOURCE 200809L

/* Read a decimal number, rejecting signs and leading zeros. */
static bool parse_number(const char **cursor, unsigned limit,
                         unsigned *result)
{
    const char *p = *cursor;
    unsigned value = 0;

    if (*p < '0' || *p > '9') {
        return false;
    }

    if (*p == '0' && p[1] >= '0' && p[1] <= '9') {
        return false;
    }

    while (*p >= '0' && *p <= '9') {
        value = value * 10U + (unsigned)(*p - '0');

        if (value > limit) {
            return false;
        }

        ++p;
    }

    *cursor = p;
    *result = value;
    return true;
}

/* Read four octets and leave the cursor after the address. */
static bool parse_address(const char **cursor, uint32_t *addr)
{
    uint32_t value = 0;

    for (unsigned i = 0; i < 4; ++i) {
        unsigned octet;

        if (!parse_number(cursor, 255U, &octet)) {
            return false;
        }

        value = (value << 8) | (uint32_t)octet;

        if (i < 3) {
            if (**cursor != '.') {
                return false;
            }

            ++(*cursor);
        }
    }

    *addr = value;
    return true;
}

bool ip_parse(const char *text, uint32_t *addr)
{
    if (text == NULL || addr == NULL) {
        return false;
    }

    const char *cursor = text;
    uint32_t value;

    if (!parse_address(&cursor, &value) || *cursor != '\0') {
        return false;
    }

    *addr = value;
    return true;
}

void ip_format(uint32_t addr, char out[IP_STR_LEN])
{
    snprintf(out, IP_STR_LEN, "%u.%u.%u.%u",
             (unsigned)((addr >> 24) & 255U),
             (unsigned)((addr >> 16) & 255U),
             (unsigned)((addr >> 8) & 255U),
             (unsigned)(addr & 255U));
}

bool cidr_parse(const char *text, cidr_t *block)
{
    if (text == NULL || block == NULL) {
        return false;
    }

    const char *cursor = text;
    uint32_t addr;
    unsigned prefix;

    if (!parse_address(&cursor, &addr) || *cursor != '/') {
        return false;
    }

    ++cursor;

    if (!parse_number(&cursor, 32U, &prefix) || *cursor != '\0') {
        return false;
    }

    block->addr = addr;
    block->prefix = (uint8_t)prefix;
    return true;
}

void cidr_format(cidr_t block, char out[CIDR_STR_LEN])
{
    char address[IP_STR_LEN];
    unsigned prefix = block.prefix > 32 ? 32U : block.prefix;

    ip_format(block.addr, address);
    snprintf(out, CIDR_STR_LEN, "%s/%u", address, prefix);
}

uint32_t cidr_mask(uint8_t prefix)
{
    if (prefix == 0) {
        return 0;
    }

    if (prefix >= 32) {
        return UINT32_MAX;
    }

    return UINT32_MAX << (32U - prefix);
}

uint32_t cidr_network(cidr_t block)
{
    return block.addr & cidr_mask(block.prefix);
}

uint32_t cidr_broadcast(cidr_t block)
{
    return block.addr | ~cidr_mask(block.prefix);
}

uint32_t cidr_first_host(cidr_t block)
{
    uint32_t network = cidr_network(block);

    return block.prefix >= 31 ? network : network + 1U;
}

uint32_t cidr_last_host(cidr_t block)
{
    uint32_t broadcast = cidr_broadcast(block);

    return block.prefix >= 31 ? broadcast : broadcast - 1U;
}

uint64_t cidr_host_count(cidr_t block)
{
    if (block.prefix >= 32) {
        return 1;
    }

    if (block.prefix == 31) {
        return 2;
    }

    return (UINT64_C(1) << (32U - block.prefix)) - 2U;
}

bool cidr_contains(cidr_t block, uint32_t addr)
{
    uint32_t mask = cidr_mask(block.prefix);

    return (addr & mask) == (block.addr & mask);
}

bool cidr_is_network(cidr_t block)
{
    return block.addr == cidr_network(block);
}

bool cidr_subnet(cidr_t block, uint32_t n, uint32_t i,
                 cidr_t *out)
{
    if (out == NULL || !cidr_is_network(block)) {
        return false;
    }

    if (n == 0 || (n & (n - 1U)) != 0 || i >= n) {
        return false;
    }

    unsigned extra_bits = 0;

    for (uint32_t count = n; count > 1; count >>= 1) {
        ++extra_bits;
    }

    unsigned prefix = block.prefix;

    if (prefix > 32 || extra_bits > 32U - prefix) {
        return false;
    }

    prefix += extra_bits;

    uint64_t subnet_size = UINT64_C(1) << (32U - prefix);
    uint64_t address = (uint64_t)block.addr
                       + (uint64_t)i * subnet_size;

    out->addr = (uint32_t)address;
    out->prefix = (uint8_t)prefix;
    return true;
}

/* Copy one whitespace-delimited field without overflowing its buffer. */
static bool route_field(const char **cursor, char *out, size_t capacity)
{
    const char *p = *cursor;
    size_t length = 0;

    while (isspace((unsigned char)*p)) {
        ++p;
    }

    while (*p != '\0' && !isspace((unsigned char)*p)) {
        if (length + 1 >= capacity) {
            return false;
        }

        out[length++] = *p++;
    }

    if (length == 0) {
        return false;
    }

    out[length] = '\0';
    *cursor = p;
    return true;
}

bool route_parse(const char *line, route_t *route)
{
    if (line == NULL || route == NULL) {
        return false;
    }

    const char *cursor = line;
    char destination[CIDR_STR_LEN];
    char gateway[IP_STR_LEN];
    route_t parsed = {0};

    if (!route_field(&cursor, destination, sizeof destination)
        || !route_field(&cursor, gateway, sizeof gateway)
        || !route_field(&cursor, parsed.dev, sizeof parsed.dev)) {
        return false;
    }

    while (isspace((unsigned char)*cursor)) {
        ++cursor;
    }

    if (*cursor != '\0') {
        return false;
    }

    if (strcmp(destination, "default") == 0) {
        parsed.dest = (cidr_t){0, 0};
    } else if (!cidr_parse(destination, &parsed.dest)
               || !cidr_is_network(parsed.dest)) {
        return false;
    }

    if (strcmp(gateway, "direct") != 0
        && !ip_parse(gateway, &parsed.gateway)) {
        return false;
    }

    *route = parsed;
    return true;
}

struct fwd_table {
    route_t *routes;
    size_t count;
    size_t capacity;
};

fwd_table_t *table_create(void)
{
    return calloc(1, sizeof(fwd_table_t));
}

void table_destroy(fwd_table_t *table)
{
    if (table == NULL) {
        return;
    }

    free(table->routes);
    free(table);
}

size_t table_count(const fwd_table_t *table)
{
    return table == NULL ? 0 : table->count;
}

static size_t next_capacity(size_t current)
{
    if (current > SIZE_MAX / 2 / sizeof(route_t)) {
        return 0;
    }

    return current == 0 ? 4 : current * 2;
}

#ifdef TEST
size_t test_next_capacity(size_t current)
{
    return next_capacity(current);
}
#endif

bool table_add(fwd_table_t *table, const route_t *route)
{
    if (table == NULL || route == NULL) {
        return false;
    }

    size_t capacity = table->capacity;

    if (table->count == table->capacity) {
        capacity = next_capacity(table->capacity);
    }

    if (!cidr_is_network(route->dest) || capacity == 0) {
        return false;
    }

    for (size_t i = 0; i < table->count; ++i) {
        if (table->routes[i].dest.addr == route->dest.addr
            && table->routes[i].dest.prefix == route->dest.prefix) {
            return false;
        }
    }

    if (table->count == table->capacity) {

        route_t *grown = realloc(table->routes,
                                 capacity * sizeof *grown);

        if (grown == NULL) {
            return false; // GCOVR_EXCL_LINE
        }

        table->routes = grown;
        table->capacity = capacity;
    }

    table->routes[table->count++] = *route;
    return true;
}

const route_t *table_lookup(const fwd_table_t *table, uint32_t dest)
{
    if (table == NULL) {
        return NULL;
    }

    const route_t *best = NULL;

    for (size_t i = 0; i < table->count; ++i) {
        const route_t *candidate = &table->routes[i];

        if (cidr_contains(candidate->dest, dest)
            && (best == NULL
                || candidate->dest.prefix > best->dest.prefix)) {
            best = candidate;
        }
    }

    return best;
}


bool table_load(fwd_table_t *table, FILE *in, size_t *bad_line)
{
    if (bad_line != NULL) {
        *bad_line = 0;
    }

    if (table == NULL || in == NULL) {
        return false;
    }

    char *line = NULL;
    size_t capacity = 0;
    size_t line_number = 0;

    while (getline(&line, &capacity, in) != -1) {
        ++line_number;

        char *comment = strchr(line, '#');
        if (comment != NULL) {
            *comment = '\0';
        }

        const char *start = line;
        while (isspace((unsigned char)*start)) {
            ++start;
        }

        if (*start == '\0') {
            continue;
        }

        route_t route;

        if (!route_parse(start, &route)
            || !table_add(table, &route)) {
            if (bad_line != NULL) {
                *bad_line = line_number;
            }

            free(line);
            return false;
        }
    }

    bool success = feof(in) && !ferror(in);

    free(line);
    return success;
}

```

### lab.h

```c

#ifndef LAB_H
#define LAB_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

/*
 * Every address in this file is a uint32_t in host byte order, so the first
 * octet of 192.168.1.10 is the most significant byte: 0xC0A8010A.
 */

/** Room for the longest dotted quad, "255.255.255.255", and its '\0'. */
#define IP_STR_LEN 16

/** Room for the longest CIDR string, "255.255.255.255/32", and its '\0'. */
#define CIDR_STR_LEN 19

/** Room for an interface name; IFNAMSIZ on Linux, including the '\0'. */
#define DEV_NAME_LEN 16

/** An address block in CIDR notation: an address and a prefix length. */
typedef struct {
  uint32_t addr;  /**< any address in the block, host byte order */
  uint8_t prefix; /**< 0 to 32 */
} cidr_t;

/** One entry in a forwarding table. */
typedef struct {
  cidr_t dest;            /**< destination block; host bits must be zero */
  uint32_t gateway;       /**< next hop, or 0 when the block is directly connected */
  char dev[DEV_NAME_LEN]; /**< outgoing interface, for example "eth0" */
} route_t;

/** A forwarding table. Its layout is private to lab.c. */
typedef struct fwd_table fwd_table_t;

/* ------------------------------------------------------------------------
 * Task 1 - Address parsing and formatting
 * --------------------------------------------------------------------- */

/**
 * @brief Parse a dotted quad such as "192.168.1.10".
 *
 * Exactly four decimal octets from 0 to 255 separated by single periods, with
 * nothing before, between or after them. An octet with a leading zero ("010")
 * is rejected, because inet_aton() reads it as octal and that is never what a
 * person typing it meant.
 *
 * @param text The string to parse.
 * @param addr Where the address is stored on success; untouched on failure.
 * @return true if text is a valid dotted quad, false otherwise (including NULL).
 */
bool ip_parse(const char *text, uint32_t *addr);

/**
 * @brief Format an address as a dotted quad.
 * @param addr The address, host byte order.
 * @param out A buffer of at least IP_STR_LEN bytes.
 */
void ip_format(uint32_t addr, char out[IP_STR_LEN]);

/**
 * @brief Parse a block in CIDR notation such as "192.168.1.0/24".
 *
 * The address follows the rules of ip_parse() and the prefix is a decimal
 * number from 0 to 32 with no leading zero. The slash and the prefix are
 * required. Host bits may be set ("132.178.227.11/25" is an interface
 * address), so use cidr_is_network() when they must not be.
 *
 * @param text The string to parse.
 * @param block Where the block is stored on success; untouched on failure.
 * @return true if text is valid CIDR notation, false otherwise (including NULL).
 */
bool cidr_parse(const char *text, cidr_t *block);

/**
 * @brief Format a block as "a.b.c.d/p", using the address exactly as stored.
 *
 * A prefix over 32 is written as 32, the way cidr_mask() treats it.
 *
 * @param block The block.
 * @param out A buffer of at least CIDR_STR_LEN bytes.
 */
void cidr_format(cidr_t block, char out[CIDR_STR_LEN]);

/* ------------------------------------------------------------------------
 * Task 2 - Subnet arithmetic
 * --------------------------------------------------------------------- */

/**
 * @brief The subnet mask for a prefix length: /24 is 255.255.255.0.
 *
 * Careful with /0. Shifting a 32 bit value by 32 is undefined behavior in C.
 *
 * @param prefix 0 to 32; anything larger is treated as 32.
 * @return The mask, host byte order.
 */
uint32_t cidr_mask(uint8_t prefix);

/** @brief The lowest address in the block: the address with the host bits cleared. */
uint32_t cidr_network(cidr_t block);

/** @brief The highest address in the block: the address with the host bits set. */
uint32_t cidr_broadcast(cidr_t block);

/**
 * @brief The first address that can be given to a host.
 *
 * For /0 to /30 that is the network address plus one. A /31 is a point to
 * point link (RFC 3021) where both addresses are usable, and a /32 is a single
 * host, so for those two the first host is the network address.
 */
uint32_t cidr_first_host(cidr_t block);

/**
 * @brief The last address that can be given to a host.
 *
 * The broadcast address minus one for /0 to /30, and the broadcast address
 * itself for /31 and /32 (see cidr_first_host()).
 */
uint32_t cidr_last_host(cidr_t block);

/**
 * @brief How many addresses in the block can be given to hosts.
 *
 * 2^(32 - prefix) - 2 for /0 to /30, 2 for /31, and 1 for /32. For a /0 the
 * 2^32 in that formula needs 33 bits, more than a uint32_t holds, which is why
 * this returns a uint64_t.
 */
uint64_t cidr_host_count(cidr_t block);

/** @brief true if addr falls inside the block. */
bool cidr_contains(cidr_t block, uint32_t addr);

/** @brief true if the block's host bits are all zero, so it names a network. */
bool cidr_is_network(cidr_t block);

/**
 * @brief The i-th of n equally sized subnets of a block.
 *
 * Splitting a /24 into 4 gives four /26 blocks, and subnet 0 is the lowest.
 * Equal sizes are only possible when n is a power of two, and the new prefix
 * can not pass 32.
 *
 * @param block The block to split; it must be a network (cidr_is_network()).
 * @param n How many subnets, a power of two from 1 up.
 * @param i Which subnet, 0 to n - 1.
 * @param out Where the subnet is stored on success; untouched on failure.
 * @return false if out is NULL, the block is not a network, n is not a power
 *         of two, the split would need a prefix longer than 32, or i is out of
 *         range.
 */
bool cidr_subnet(cidr_t block, uint32_t n, uint32_t i, cidr_t *out);

/* ------------------------------------------------------------------------
 * Task 3 - Longest prefix match
 * --------------------------------------------------------------------- */

/**
 * @brief Parse one line of a forwarding table file.
 *
 * A line is three fields separated by spaces or tabs:
 *
 *     <destination> <gateway> <device>
 *
 * The destination is a block in CIDR notation with no host bits set, or the
 * word "default" for 0.0.0.0/0. The gateway is a dotted quad, or the word
 * "direct" for a directly connected block (stored as 0). The device is 1 to
 * DEV_NAME_LEN - 1 characters. Leading and trailing whitespace, including a
 * trailing newline, is ignored. Comments are not handled here.
 *
 * @param line The line.
 * @param route Where the route is stored on success; untouched on failure.
 * @return true if the line is a valid route, false otherwise (including NULL).
 */
bool route_parse(const char *line, route_t *route);

/**
 * @brief Create an empty forwarding table.
 * @return The table, which the caller frees with table_destroy(), or NULL if
 *         memory could not be allocated.
 */
fwd_table_t *table_create(void);

/** @brief Free a table and everything in it. NULL is allowed and does nothing. */
void table_destroy(fwd_table_t *table);

/** @brief The number of routes in the table; 0 for NULL. */
size_t table_count(const fwd_table_t *table);

/**
 * @brief Add a copy of a route to the table, growing it as needed.
 * @return false if either argument is NULL, the destination has host bits
 *         set, the table already holds a route to the same block, or memory
 *         could not be allocated. The table is unchanged on failure.
 */
bool table_add(fwd_table_t *table, const route_t *route);

/**
 * @brief Read a forwarding table file and add every route in it.
 *
 * Everything from a '#' to the end of a line is a comment, and a line that is
 * blank once the comment is removed is skipped. Every other line must be a
 * valid route (route_parse()) that table_add() accepts.
 *
 * @param table The table to add to.
 * @param in The open file to read.
 * @param bad_line If not NULL, set to the 1-based number of the first line
 *        that failed, or 0 if the arguments were NULL or the file could not
 *        be read.
 * @return true if every line loaded. On false, routes from the lines before
 *         the bad one stay in the table.
 */
bool table_load(fwd_table_t *table, FILE *in, size_t *bad_line);

/**
 * @brief Longest prefix match: the route whose block contains dest and has the
 *        longest prefix.
 *
 * A default route (0.0.0.0/0) matches everything, so it is chosen only when
 * nothing more specific does. The order the routes were added in does not
 * matter.
 *
 * @return The matching route, which stays owned by the table, or NULL if no
 *         route matches (or table is NULL).
 */
const route_t *table_lookup(const fwd_table_t *table, uint32_t dest);

#endif // LAB_H
```

### main.c

```c

#include "lab.h"
#include <inttypes.h>
#include <string.h>

#ifdef TEST
#define main main_exclude
#endif

static void usage(FILE *out)
{
    fprintf(out, "Usage: myapp info <address>/<prefix>\n");
    fprintf(out, "       myapp split <network>/<prefix> <count>\n");
    fprintf(out, "       myapp lookup <table-file> <address>...\n");
}

static void print_address(const char *label, uint32_t addr)
{
    char text[IP_STR_LEN];

    ip_format(addr, text);
    printf("%-11s%s\n", label, text);
}

static int command_info(const char *text)
{
    cidr_t block;

    if (!cidr_parse(text, &block)) {
        fprintf(stderr, "myapp: %s: not a valid CIDR address\n", text);
        return 1;
    }

    print_address("address", block.addr);
    printf("%-11s/%u\n", "prefix", (unsigned)block.prefix);
    print_address("netmask", cidr_mask(block.prefix));
    print_address("network", cidr_network(block));
    print_address("broadcast", cidr_broadcast(block));
    print_address("first host", cidr_first_host(block));
    print_address("last host", cidr_last_host(block));
    printf("%-11s%" PRIu64 "\n", "hosts", cidr_host_count(block));

    return 0;
}

static bool parse_count(const char *text, uint32_t *out)
{
    if (*text == '\0') {
        return false;
    }

    uint32_t value = 0;

    for (const char *p = text; *p != '\0'; ++p) {
        if (*p < '0' || *p > '9') {
            return false;
        }

        uint32_t digit = (uint32_t)(*p - '0');

        if (value > (UINT32_MAX - digit) / 10U) {
            return false;
        }

        value = value * 10U + digit;
    }

    *out = value;
    return true;
}

static int command_split(const char *text, const char *count_text)
{
    cidr_t block;
    cidr_t subnet;
    uint32_t count;

    if (!cidr_parse(text, &block)) {
        fprintf(stderr, "myapp: %s: not a valid CIDR address\n", text);
        return 1;
    }

    if (!cidr_is_network(block)) {
        fprintf(stderr, "myapp: %s: network has host bits set\n", text);
        return 1;
    }

    if (!parse_count(count_text, &count)
        || !cidr_subnet(block, count, 0, &subnet)) {
        fprintf(stderr,
                "myapp: %s: count must be a power of two "
                "that fits in the network\n",
                count_text);
        return 1;
    }

    for (uint32_t i = 0; i < count; ++i) {
        char output[CIDR_STR_LEN];

        cidr_subnet(block, count, i, &subnet);
        cidr_format(subnet, output);
        puts(output);
    }

    return 0;
}

static int command_lookup(const char *path, int count, char **addresses)
{
    FILE *file = fopen(path, "r");

    if (file == NULL) {
        fprintf(stderr, "myapp: %s: cannot open table file\n", path);
        return 2;
    }

    fwd_table_t *table = table_create();

    if (table == NULL) {
        fprintf(stderr, "myapp: cannot allocate forwarding table\n");
        fclose(file);
        return 2;
    }

    size_t bad_line = 0;
    bool loaded = table_load(table, file, &bad_line);
    fclose(file);

    if (!loaded) {
        if (bad_line != 0) {
            fprintf(stderr, "myapp: %s:%zu: not a valid route\n",
                    path, bad_line);
        } else {
            fprintf(stderr, "myapp: %s: cannot read table file\n", path);
        }

        table_destroy(table);
        return 2;
    }

    bool malformed = false;
    bool unreachable = false;

    for (int i = 0; i < count; ++i) {
        uint32_t addr;

        if (!ip_parse(addresses[i], &addr)) {
            fprintf(stderr, "myapp: %s: not a valid IPv4 address\n",
                    addresses[i]);
            malformed = true;
            continue;
        }

        char destination[IP_STR_LEN];
        ip_format(addr, destination);

        const route_t *route = table_lookup(table, addr);

        if (route == NULL) {
            printf("%s unreachable\n", destination);
            unreachable = true;
            continue;
        }

        if (route->gateway != 0) {
            char gateway[IP_STR_LEN];
            ip_format(route->gateway, gateway);

            printf("%s via %s dev %s\n",
                   destination, gateway, route->dev);
        } else {
            printf("%s dev %s\n", destination, route->dev);
        }
    }

    table_destroy(table);

    if (malformed) {
        return 1;
    }

    return unreachable ? 3 : 0;
}

int main(int argc, char **argv)
{
    if (argc == 1) {
        usage(stdout);
        return 0;
    }

    if (strcmp(argv[1], "info") == 0 && argc == 3) {
        return command_info(argv[2]);
    }

    if (strcmp(argv[1], "split") == 0 && argc == 4) {
        return command_split(argv[2], argv[3]);
    }

    if (strcmp(argv[1], "lookup") == 0 && argc >= 4) {
        return command_lookup(argv[2], argc - 3, argv + 3);
    }

    fprintf(stderr, "myapp: invalid command or argument count\n");
    usage(stderr);
    return 1;
}

```

## Tests Files
### lab-test.c

```c

#include "harness/unity.h"
#include "../src/lab.h"

void setUp(void) {}
void tearDown(void) {}

void test_ip_parse_valid(void)
{
    uint32_t addr;

    TEST_ASSERT_TRUE(ip_parse("192.168.1.10", &addr));
    TEST_ASSERT_EQUAL_HEX32(0xC0A8010A, addr);

    TEST_ASSERT_TRUE(ip_parse("0.0.0.0", &addr));
    TEST_ASSERT_EQUAL_UINT32(0, addr);

    TEST_ASSERT_TRUE(ip_parse("255.255.255.255", &addr));
    TEST_ASSERT_EQUAL_HEX32(UINT32_MAX, addr);
}

void test_ip_parse_invalid(void)
{
    const char *invalid[] = {
        "", "1.2.3", "1.2.3.4.5", "1..2.3",
        "256.1.1.1", "010.0.0.1", "1.2.3.4x",
        " 1.2.3.4", "1.2.3.4 ", "+1.2.3.4",
        "-1.2.3.4", "1.2.3.04", "1,2.3.4",
        "1.2.3.", "99999999999999999999.1.1.1",
        "1.2.3.4/24"
    };

    for (size_t i = 0; i < sizeof invalid / sizeof invalid[0]; ++i) {
        uint32_t addr = 12345;

        TEST_ASSERT_FALSE_MESSAGE(ip_parse(invalid[i], &addr),
                                  invalid[i]);
        TEST_ASSERT_EQUAL_UINT32(12345, addr);
    }

    uint32_t addr = 12345;
    TEST_ASSERT_FALSE(ip_parse(NULL, &addr));
    TEST_ASSERT_EQUAL_UINT32(12345, addr);
    TEST_ASSERT_FALSE(ip_parse("1.2.3.4", NULL));
}

void test_ip_format(void)
{
    char out[IP_STR_LEN];

    ip_format(0xC0A8010A, out);
    TEST_ASSERT_EQUAL_STRING("192.168.1.10", out);

    ip_format(0, out);
    TEST_ASSERT_EQUAL_STRING("0.0.0.0", out);

    ip_format(UINT32_MAX, out);
    TEST_ASSERT_EQUAL_STRING("255.255.255.255", out);
}

void test_cidr_parse_valid(void)
{
    cidr_t block;

    TEST_ASSERT_TRUE(cidr_parse("132.178.227.11/25", &block));
    TEST_ASSERT_EQUAL_HEX32(0x84B2E30B, block.addr);
    TEST_ASSERT_EQUAL_UINT8(25, block.prefix);

    TEST_ASSERT_TRUE(cidr_parse("0.0.0.0/0", &block));
    TEST_ASSERT_EQUAL_UINT32(0, block.addr);
    TEST_ASSERT_EQUAL_UINT8(0, block.prefix);

    TEST_ASSERT_TRUE(cidr_parse("255.255.255.255/32", &block));
    TEST_ASSERT_EQUAL_HEX32(UINT32_MAX, block.addr);
    TEST_ASSERT_EQUAL_UINT8(32, block.prefix);

    TEST_ASSERT_TRUE(cidr_parse("10.0.0.0/31", &block));
    TEST_ASSERT_EQUAL_UINT8(31, block.prefix);
}

void test_cidr_parse_invalid(void)
{
    const char *invalid[] = {
        "", "1.2.3.4", "1.2.3.4/33", "1.2.3.4/024",
        "1.2.3.4/", "1.2.3/24", "1.2.3.4/-1",
        "1.2.3.4/+1", "1.2.3.4/24x", "1.2.3.4/24 ",
        "1.2.3.4/ 24", "1.2.3.4/00", "1.2.3.4//24",
        "010.0.0.1/8", "1.2.3.4/99999999999999999999"
    };

    for (size_t i = 0; i < sizeof invalid / sizeof invalid[0]; ++i) {
        cidr_t block = {12345, 17};

        TEST_ASSERT_FALSE_MESSAGE(cidr_parse(invalid[i], &block),
                                  invalid[i]);
        TEST_ASSERT_EQUAL_UINT32(12345, block.addr);
        TEST_ASSERT_EQUAL_UINT8(17, block.prefix);
    }

    cidr_t block = {12345, 17};
    TEST_ASSERT_FALSE(cidr_parse(NULL, &block));
    TEST_ASSERT_EQUAL_UINT32(12345, block.addr);
    TEST_ASSERT_EQUAL_UINT8(17, block.prefix);
    TEST_ASSERT_FALSE(cidr_parse("1.2.3.4/24", NULL));
}

void test_cidr_format(void)
{
    char out[CIDR_STR_LEN];

    cidr_format((cidr_t){0x84B2E30B, 25}, out);
    TEST_ASSERT_EQUAL_STRING("132.178.227.11/25", out);

    cidr_format((cidr_t){0, 0}, out);
    TEST_ASSERT_EQUAL_STRING("0.0.0.0/0", out);

    cidr_format((cidr_t){UINT32_MAX, 32}, out);
    TEST_ASSERT_EQUAL_STRING("255.255.255.255/32", out);

    cidr_format((cidr_t){UINT32_MAX, 255}, out);
    TEST_ASSERT_EQUAL_STRING("255.255.255.255/32", out);
}

void test_subnet_masks(void)
{
    TEST_ASSERT_EQUAL_HEX32(0, cidr_mask(0));
    TEST_ASSERT_EQUAL_HEX32(0x80000000, cidr_mask(1));
    TEST_ASSERT_EQUAL_HEX32(0xFFFFFF00, cidr_mask(24));
    TEST_ASSERT_EQUAL_HEX32(0xFFFFFF80, cidr_mask(25));
    TEST_ASSERT_EQUAL_HEX32(0xFFFFFFFE, cidr_mask(31));
    TEST_ASSERT_EQUAL_HEX32(UINT32_MAX, cidr_mask(32));
    TEST_ASSERT_EQUAL_HEX32(UINT32_MAX, cidr_mask(255));
}

void test_subnet_arithmetic(void)
{
    cidr_t block = {0x84B2E30B, 25};

    TEST_ASSERT_EQUAL_HEX32(0x84B2E300, cidr_network(block));
    TEST_ASSERT_EQUAL_HEX32(0x84B2E37F, cidr_broadcast(block));
    TEST_ASSERT_EQUAL_HEX32(0x84B2E301, cidr_first_host(block));
    TEST_ASSERT_EQUAL_HEX32(0x84B2E37E, cidr_last_host(block));
    TEST_ASSERT_EQUAL_UINT64(126, cidr_host_count(block));

    TEST_ASSERT_TRUE(cidr_contains(block, 0x84B2E300));
    TEST_ASSERT_TRUE(cidr_contains(block, 0x84B2E332));
    TEST_ASSERT_TRUE(cidr_contains(block, 0x84B2E37F));
    TEST_ASSERT_FALSE(cidr_contains(block, 0x84B2E380));
    TEST_ASSERT_FALSE(cidr_is_network(block));
    TEST_ASSERT_TRUE(cidr_is_network((cidr_t){0x84B2E300, 25}));
}

void test_boundary_subnets(void)
{
    cidr_t block = {0xC0A8010A, 0};

    TEST_ASSERT_EQUAL_UINT32(0, cidr_network(block));
    TEST_ASSERT_EQUAL_HEX32(UINT32_MAX, cidr_broadcast(block));
    TEST_ASSERT_EQUAL_UINT32(1, cidr_first_host(block));
    TEST_ASSERT_EQUAL_HEX32(0xFFFFFFFE, cidr_last_host(block));
    TEST_ASSERT_EQUAL_UINT64(UINT64_C(4294967294),
                             cidr_host_count(block));
    TEST_ASSERT_TRUE(cidr_contains(block, 0));
    TEST_ASSERT_TRUE(cidr_contains(block, UINT32_MAX));

    block = (cidr_t){0xC0A8010B, 31};
    TEST_ASSERT_EQUAL_HEX32(0xC0A8010A, cidr_network(block));
    TEST_ASSERT_EQUAL_HEX32(0xC0A8010B, cidr_broadcast(block));
    TEST_ASSERT_EQUAL_HEX32(0xC0A8010A, cidr_first_host(block));
    TEST_ASSERT_EQUAL_HEX32(0xC0A8010B, cidr_last_host(block));
    TEST_ASSERT_EQUAL_UINT64(2, cidr_host_count(block));

    block = (cidr_t){UINT32_MAX, 32};
    TEST_ASSERT_EQUAL_HEX32(UINT32_MAX, cidr_network(block));
    TEST_ASSERT_EQUAL_HEX32(UINT32_MAX, cidr_broadcast(block));
    TEST_ASSERT_EQUAL_HEX32(UINT32_MAX, cidr_first_host(block));
    TEST_ASSERT_EQUAL_HEX32(UINT32_MAX, cidr_last_host(block));
    TEST_ASSERT_EQUAL_UINT64(1, cidr_host_count(block));
    TEST_ASSERT_TRUE(cidr_contains(block, UINT32_MAX));
    TEST_ASSERT_FALSE(cidr_contains(block, 0));
    TEST_ASSERT_TRUE(cidr_is_network(block));

    block = (cidr_t){0, 32};
    TEST_ASSERT_EQUAL_UINT32(0, cidr_first_host(block));
    TEST_ASSERT_EQUAL_UINT32(0, cidr_last_host(block));
    TEST_ASSERT_EQUAL_UINT64(1, cidr_host_count(block));
}

void test_subnet_split_valid(void)
{
    cidr_t out;

    for (uint32_t i = 0; i < 4; ++i) {
        TEST_ASSERT_TRUE(cidr_subnet((cidr_t){0xC0A80100, 24},
                                    4, i, &out));
        TEST_ASSERT_EQUAL_HEX32(0xC0A80100 + i * 64U, out.addr);
        TEST_ASSERT_EQUAL_UINT8(26, out.prefix);
    }

    TEST_ASSERT_TRUE(cidr_subnet((cidr_t){0, 0}, 1, 0, &out));
    TEST_ASSERT_EQUAL_UINT32(0, out.addr);
    TEST_ASSERT_EQUAL_UINT8(0, out.prefix);

    TEST_ASSERT_TRUE(cidr_subnet((cidr_t){0, 0},
                                UINT32_C(2147483648),
                                UINT32_C(2147483647), &out));
    TEST_ASSERT_EQUAL_HEX32(0xFFFFFFFE, out.addr);
    TEST_ASSERT_EQUAL_UINT8(31, out.prefix);

    TEST_ASSERT_TRUE(cidr_subnet((cidr_t){0xFFFFFFFE, 31},
                                2, 1, &out));
    TEST_ASSERT_EQUAL_HEX32(UINT32_MAX, out.addr);
    TEST_ASSERT_EQUAL_UINT8(32, out.prefix);

    TEST_ASSERT_TRUE(cidr_subnet((cidr_t){UINT32_MAX, 32},
                                1, 0, &out));
    TEST_ASSERT_EQUAL_HEX32(UINT32_MAX, out.addr);
    TEST_ASSERT_EQUAL_UINT8(32, out.prefix);
}

void test_subnet_split_invalid(void)
{
    cidr_t out = {12345, 17};
    cidr_t network = {0xC0A80100, 24};

    TEST_ASSERT_FALSE(cidr_subnet(network, 4, 0, NULL));
    TEST_ASSERT_FALSE(cidr_subnet((cidr_t){0xC0A80105, 24},
                                 4, 0, &out));
    TEST_ASSERT_FALSE(cidr_subnet(network, 0, 0, &out));
    TEST_ASSERT_FALSE(cidr_subnet(network, 3, 0, &out));
    TEST_ASSERT_FALSE(cidr_subnet(network, 4, 4, &out));
    TEST_ASSERT_FALSE(cidr_subnet(network, 512, 0, &out));
    TEST_ASSERT_FALSE(cidr_subnet((cidr_t){0, 32}, 2, 0, &out));
    TEST_ASSERT_FALSE(cidr_subnet((cidr_t){0, 33}, 1, 0, &out));

    TEST_ASSERT_EQUAL_UINT32(12345, out.addr);
    TEST_ASSERT_EQUAL_UINT8(17, out.prefix);
}

void test_route_parse_valid(void)
{
    route_t route;

    TEST_ASSERT_TRUE(route_parse(
        "default 132.178.227.1 eno1", &route));
    TEST_ASSERT_EQUAL_UINT32(0, route.dest.addr);
    TEST_ASSERT_EQUAL_UINT8(0, route.dest.prefix);
    TEST_ASSERT_EQUAL_HEX32(0x84B2E301, route.gateway);
    TEST_ASSERT_EQUAL_STRING("eno1", route.dev);

    TEST_ASSERT_TRUE(route_parse(
        " \t10.1.2.0/24\t direct  eth2 \r\n", &route));
    TEST_ASSERT_EQUAL_HEX32(0x0A010200, route.dest.addr);
    TEST_ASSERT_EQUAL_UINT8(24, route.dest.prefix);
    TEST_ASSERT_EQUAL_UINT32(0, route.gateway);
    TEST_ASSERT_EQUAL_STRING("eth2", route.dev);

    TEST_ASSERT_TRUE(route_parse(
        "255.255.255.255/32 direct abcdefghijklmno", &route));
    TEST_ASSERT_EQUAL_HEX32(UINT32_MAX, route.dest.addr);
    TEST_ASSERT_EQUAL_UINT8(32, route.dest.prefix);
    TEST_ASSERT_EQUAL_STRING("abcdefghijklmno", route.dev);
}

void test_route_parse_invalid(void)
{
    const char *invalid[] = {
        "",
        "   \t\n",
        "default",
        "default direct",
        "default direct eth0 extra",
        "10.1.2.5/24 direct eth0",
        "10.0.0.0/33 direct eth0",
        "not-a-network direct eth0",
        "default 256.1.1.1 eth0",
        "default 010.0.0.1 eth0",
        "default direct abcdefghijklmnop",
        "default direct eth0 # comment",
        "255.255.255.255/320 direct eth0",
        "default 255.255.255.2555 eth0"
    };

    for (size_t i = 0; i < sizeof invalid / sizeof invalid[0]; ++i) {
        route_t route = {
            .dest = {12345, 17},
            .gateway = 67890,
            .dev = "unchanged"
        };

        TEST_ASSERT_FALSE_MESSAGE(
            route_parse(invalid[i], &route), invalid[i]);
        TEST_ASSERT_EQUAL_UINT32(12345, route.dest.addr);
        TEST_ASSERT_EQUAL_UINT8(17, route.dest.prefix);
        TEST_ASSERT_EQUAL_UINT32(67890, route.gateway);
        TEST_ASSERT_EQUAL_STRING("unchanged", route.dev);
    }

    route_t route = {0};
    TEST_ASSERT_FALSE(route_parse(NULL, &route));
    TEST_ASSERT_FALSE(route_parse("default direct eth0", NULL));
}

void test_table_add_and_destroy(void)
{
    TEST_ASSERT_EQUAL_UINT32(0, table_count(NULL));
    TEST_ASSERT_NULL(table_lookup(NULL, 0));
    table_destroy(NULL);

    fwd_table_t *table = table_create();
    TEST_ASSERT_NOT_NULL(table);
    TEST_ASSERT_EQUAL_UINT32(0, table_count(table));
    TEST_ASSERT_NULL(table_lookup(table, 0));

    route_t route;
    TEST_ASSERT_TRUE(route_parse("10.0.0.0/8 direct eth0", &route));

    TEST_ASSERT_FALSE(table_add(NULL, &route));
    TEST_ASSERT_FALSE(table_add(table, NULL));
    TEST_ASSERT_TRUE(table_add(table, &route));

    /* Changing the original must not change the stored copy. */
    route.gateway = 123;
    const route_t *found = table_lookup(table, 0x0A010203);
    TEST_ASSERT_NOT_NULL(found);
    TEST_ASSERT_EQUAL_UINT32(0, found->gateway);

    TEST_ASSERT_FALSE(table_add(table, &route));
    TEST_ASSERT_EQUAL_UINT32(1, table_count(table));

    route.dest = (cidr_t){0x0A000001, 8};
    TEST_ASSERT_FALSE(table_add(table, &route));
    TEST_ASSERT_EQUAL_UINT32(1, table_count(table));
    TEST_ASSERT_NULL(table_lookup(table, 0x08080808));

    table_destroy(table);
}

void test_longest_prefix_in_both_orders(void)
{
    const char *lines[] = {
        "default 132.178.227.1 eno1",
        "10.0.0.0/8 10.1.0.1 eth1",
        "10.1.0.0/16 direct eth1",
        "10.1.2.0/24 direct eth2",
        "10.1.2.128/25 10.1.2.1 eth2",
        "10.1.2.200/32 direct host0"
    };

    const uint32_t addresses[] = {
        0x08080808, 0x0A020304, 0x0A010304,
        0x0A010205, 0x0A0102C9, 0x0A0102C8
    };

    const uint8_t prefixes[] = {0, 8, 16, 24, 25, 32};

    for (unsigned reverse = 0; reverse < 2; ++reverse) {
        fwd_table_t *table = table_create();
        TEST_ASSERT_NOT_NULL(table);

        for (size_t i = 0; i < 6; ++i) {
            size_t index = reverse ? 5 - i : i;
            route_t route;

            TEST_ASSERT_TRUE(route_parse(lines[index], &route));
            TEST_ASSERT_TRUE(table_add(table, &route));
        }

        TEST_ASSERT_EQUAL_UINT32(6, table_count(table));

        for (size_t i = 0; i < 6; ++i) {
            const route_t *found = table_lookup(table, addresses[i]);

            TEST_ASSERT_NOT_NULL(found);
            TEST_ASSERT_EQUAL_UINT8(prefixes[i], found->dest.prefix);
        }

        table_destroy(table);
    }
}

void test_table_load_valid(void)
{
    fwd_table_t *table = table_create();
    FILE *file = tmpfile();
    TEST_ASSERT_NOT_NULL(table);
    TEST_ASSERT_NOT_NULL(file);

    fputs("# example table\n"
          "\n"
          " \t # another comment\n"
          "default 132.178.227.1 eno1\n"
          "10.1.2.0/24 direct eth2 # local network\n"
          "10.1.2.128/25 10.1.2.1 eth2", file);
    rewind(file);

    size_t bad_line = 999;
    TEST_ASSERT_TRUE(table_load(table, file, &bad_line));
    TEST_ASSERT_EQUAL_UINT32(0, bad_line);
    TEST_ASSERT_EQUAL_UINT32(3, table_count(table));

    const route_t *found = table_lookup(table, 0x0A0102C8);
    TEST_ASSERT_NOT_NULL(found);
    TEST_ASSERT_EQUAL_UINT8(25, found->dest.prefix);
    TEST_ASSERT_EQUAL_HEX32(0x0A010201, found->gateway);

    found = table_lookup(table, 0x08080808);
    TEST_ASSERT_NOT_NULL(found);
    TEST_ASSERT_EQUAL_UINT8(0, found->dest.prefix);

    fclose(file);
    table_destroy(table);
}

void test_table_load_bad_line(void)
{
    fwd_table_t *table = table_create();
    FILE *file = tmpfile();
    TEST_ASSERT_NOT_NULL(table);
    TEST_ASSERT_NOT_NULL(file);

    fputs("# header\n"
          "10.0.0.0/8 direct eth0\n"
          "not a valid route\n"
          "default direct eth1\n", file);
    rewind(file);

    size_t bad_line = 0;
    TEST_ASSERT_FALSE(table_load(table, file, &bad_line));
    TEST_ASSERT_EQUAL_UINT32(3, bad_line);
    TEST_ASSERT_EQUAL_UINT32(1, table_count(table));
    TEST_ASSERT_NOT_NULL(table_lookup(table, 0x0A010203));
    TEST_ASSERT_NULL(table_lookup(table, 0x08080808));

    fclose(file);
    table_destroy(table);
}

void test_table_load_duplicate(void)
{
    fwd_table_t *table = table_create();
    FILE *file = tmpfile();
    TEST_ASSERT_NOT_NULL(table);
    TEST_ASSERT_NOT_NULL(file);

    fputs("default direct eth0\n"
          "default direct eth1\n", file);
    rewind(file);

    size_t bad_line = 0;
    TEST_ASSERT_FALSE(table_load(table, file, &bad_line));
    TEST_ASSERT_EQUAL_UINT32(2, bad_line);
    TEST_ASSERT_EQUAL_UINT32(1, table_count(table));

    rewind(file);
    TEST_ASSERT_FALSE(table_load(table, file, NULL));

    fclose(file);
    table_destroy(table);
}

void test_table_load_empty_and_null(void)
{
    fwd_table_t *table = table_create();
    FILE *file = tmpfile();
    TEST_ASSERT_NOT_NULL(table);
    TEST_ASSERT_NOT_NULL(file);

    size_t bad_line = 999;
    TEST_ASSERT_FALSE(table_load(NULL, file, &bad_line));
    TEST_ASSERT_EQUAL_UINT32(0, bad_line);

    bad_line = 999;
    TEST_ASSERT_FALSE(table_load(table, NULL, &bad_line));
    TEST_ASSERT_EQUAL_UINT32(0, bad_line);
    TEST_ASSERT_FALSE(table_load(NULL, NULL, NULL));

    TEST_ASSERT_TRUE(table_load(table, file, NULL));
    TEST_ASSERT_EQUAL_UINT32(0, table_count(table));

    fclose(file);
    table_destroy(table);
}

/* Available only in test builds; the required lab.h stays unchanged. */
extern size_t test_next_capacity(size_t current);

void test_capacity_boundaries(void)
{
    size_t limit = SIZE_MAX / 2 / sizeof(route_t);

    TEST_ASSERT_EQUAL_UINT64(4, test_next_capacity(0));
    TEST_ASSERT_EQUAL_UINT64(8, test_next_capacity(4));
    TEST_ASSERT_EQUAL_UINT64(limit * 2, test_next_capacity(limit));
    TEST_ASSERT_EQUAL_UINT64(0, test_next_capacity(limit + 1));
    TEST_ASSERT_EQUAL_UINT64(0, test_next_capacity(SIZE_MAX));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_ip_parse_valid);
    RUN_TEST(test_ip_parse_invalid);
    RUN_TEST(test_ip_format);
    RUN_TEST(test_cidr_parse_valid);
    RUN_TEST(test_cidr_parse_invalid);
    RUN_TEST(test_cidr_format);
    RUN_TEST(test_subnet_masks);
    RUN_TEST(test_subnet_arithmetic);
    RUN_TEST(test_boundary_subnets);
    RUN_TEST(test_subnet_split_valid);
    RUN_TEST(test_subnet_split_invalid);
    RUN_TEST(test_route_parse_valid);
    RUN_TEST(test_route_parse_invalid);
    RUN_TEST(test_table_add_and_destroy);
    RUN_TEST(test_longest_prefix_in_both_orders);
    RUN_TEST(test_table_load_valid);
    RUN_TEST(test_table_load_bad_line);
    RUN_TEST(test_table_load_duplicate);
    RUN_TEST(test_table_load_empty_and_null);
    RUN_TEST(test_capacity_boundaries);
    return UNITY_END();
}

```

## Scripts Files
Report generated on 10/26/2026 at 16:34:58


---

## End of Report

SHA-256 Hash of the report: e6c5b35fcea8f51281ffcb5d4c86f88fe434a3d236b206bdcdc0c222b216c8e5

Do not edit the generated report. Any changes will be reported as academic dishonesty

---
## GitHub Info
- GitHub repo name: yagurlandy/cs425-p3
- The repository visibility is public.
- The workflow was triggered by yagurlandy
