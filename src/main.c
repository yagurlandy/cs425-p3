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
