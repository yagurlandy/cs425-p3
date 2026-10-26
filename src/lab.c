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
