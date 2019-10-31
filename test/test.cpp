#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../client.cpp"

TEST_CASE("tesing is_last_packet") {
    //these shouldn't be sent by the server, but should still pass/fail the function
    REQUIRE(is_last_packet(0x00) == false);
    REQUIRE(is_last_packet(0xFF) == true);
    //these are the two values that can be sent
    REQUIRE(is_last_packet(0xE0) == true);
    REQUIRE(is_last_packet(0x80) == false);
    

}