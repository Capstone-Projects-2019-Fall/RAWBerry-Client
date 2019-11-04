#define CATCH_CONFIG_MAIN
#include "./catch.hpp"
#include "../src/client.cpp"

using namespace std;

TEST_CASE("1: tesing is_last_packet()") {
    //these shouldn't be sent by the server, but should still pass/fail the function
    REQUIRE(is_last_packet(0x00) == false);
    REQUIRE(is_last_packet(0xFF) == true);
    //these are the values that can be sent by the server
    REQUIRE(is_last_packet(0xE0) == true);
    REQUIRE(is_last_packet(0x60) == false);
    REQUIRE(is_last_packet(0x80) == true);
}

TEST_CASE("2: testing get_sequence_number()"){
    
    //for each byte in the int, in the format of (dcba), a represents the first 8 bits of the number
    unsigned char a;
    unsigned char b;
    unsigned char c;
    unsigned char d;
    unsigned char packet[6] = { 0 };

    //checking 1 byte numbers
    SECTION("seq = 0x00 to 0x0A"){
        //create the individual bytes
        a, b, c, d = 0x00;
        
        for (size_t i = 0; i < 11; i ++){
            //put them into simulated packet
            packet[2] = a;
            packet[3] = b;
            packet[4] = c;
            packet[5] = d;
            //test
            REQUIRE(get_packet_sequence(packet) == i);
            a++;
        }

    }
    
    //check transition to second byte
    SECTION("Test: seq = 0xFF"){
        a = 0xFF;
        b = c = d = 0x00;
        //put them into simulated packet
        packet[2] = a;
        packet[3] = b;
        packet[4] = c;
        packet[5] = d;
        //test
        REQUIRE(get_packet_sequence(packet) == 0xFF);
    }
    SECTION("Test: seq = 0x0100"){
        a = c = d = 0x00;
        b = 0x01;
        //put them into simulated packet
        packet[2] = a;
        packet[3] = b;
        packet[4] = c;
        packet[5] = d;
        //test
        REQUIRE(get_packet_sequence(packet) == 0x0100);
    }

    //check for transition between 2 and 3 byte numbers
    SECTION("Test: seq = 0xFFFF"){
        a = b = 0xFF;
        c = d = 0x00;
        //put them into simulated packet
        packet[2] = a;
        packet[3] = b;
        packet[4] = c;
        packet[5] = d;
        REQUIRE(get_packet_sequence(packet) == 0xFFFF);
    }
    SECTION("Test: seq = 0x010000"){
        a = b = d = 0x00;
        c = 0x01;
        //put them into simulated packet
        packet[2] = a;
        packet[3] = b;
        packet[4] = c;
        packet[5] = d;
        REQUIRE(get_packet_sequence(packet) == 0x010000);
    }

    //check for transition from 3 to 4 byte numbers
    SECTION("Test: seq = 0x00FFFFFF"){
        a = b = c = 0xFF;
        d = 0x00;
        //put them into simulated packet
        packet[2] = a;
        packet[3] = b;
        packet[4] = c;
        packet[5] = d;
        REQUIRE(get_packet_sequence(packet) == 0x00FFFFFF);
    }
    SECTION("Test: seq = 0x01000000"){
        a = b = c = 0x00;
        d = 0x01;
        //put them into simulated packet
        packet[2] = a;
        packet[3] = b;
        packet[4] = c;
        packet[5] = d;
        REQUIRE(get_packet_sequence(packet) == 0x01000000);
    }
    //check max number
    SECTION("Test: seq = 0xFFFFFFFF"){
        a = b = c = d = 0xFF;
        //put them into simulated packet
        packet[2] = a;
        packet[3] = b;
        packet[4] = c;
        packet[5] = d;
        REQUIRE(get_packet_sequence(packet) == 0xFFFFFFFF);
    }
}