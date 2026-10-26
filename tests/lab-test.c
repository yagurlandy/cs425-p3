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
