// Part of Measurement Kit <https://measurement-kit.github.io/>.
// Measurement Kit is free software under the BSD license. See AUTHORS
// and LICENSE for more information on the copying conditions.

#include "test/winsock.hpp"

#include "include/private/catch.hpp"

#include "src/libmeasurement_kit/ffi.hpp"

// clang-format off
const uint8_t binary_input[] = {
  0x57, 0xe5, 0x79, 0xfb, 0xa6, 0xbb, 0x0d, 0xbc, 0xce, 0xbd, 0xa7, 0xa0,
  0xba, 0xa4, 0x78, 0x78, 0x12, 0x59, 0xee, 0x68, 0x39, 0xa4, 0x07, 0x98,
  0xc5, 0x3e, 0xbc, 0x55, 0xcb, 0xfe, 0x34, 0x3c, 0x7e, 0x1b, 0x5a, 0xb3,
  0x22, 0x9d, 0xc1, 0x2d, 0x6e, 0xca, 0x5b, 0xf1, 0x10, 0x25, 0x47, 0x1e,
  0x44, 0xe2, 0x2d, 0x60, 0x08, 0xea, 0xb0, 0x0a, 0xcc, 0x05, 0x48, 0xa0,
  0xf5, 0x78, 0x38, 0xf0, 0xdb, 0x3f, 0x9d, 0x9f, 0x25, 0x6f, 0x89, 0x00,
  0x96, 0x93, 0xaf, 0x43, 0xac, 0x4d, 0xc9, 0xac, 0x13, 0xdb, 0x22, 0xbe,
  0x7a, 0x7d, 0xd9, 0x24, 0xa2, 0x52, 0x69, 0xd8, 0x89, 0xc1, 0xd1, 0x57,
  0xaa, 0x04, 0x2b, 0xa2, 0xd8, 0xb1, 0x19, 0xf6, 0xd5, 0x11, 0x39, 0xbb,
  0x80, 0xcf, 0x86, 0xf9, 0x5f, 0x9d, 0x8c, 0xab, 0xf5, 0xc5, 0x74, 0x24,
  0x3a, 0xa2, 0xd4, 0x40, 0x4e, 0xd7, 0x10, 0x1f
};
// clang-format on

TEST_CASE("mk_event_create_() deals with invalid JSON value") {
  nlohmann::json json;
  json["string"] = std::string{(char *)binary_input, sizeof(binary_input)};
  // Before #1728 was fixed, the following statement was crashing
  mk_unique_event ev{mk_event_create_(json)};
  REQUIRE(ev != nullptr);
  REQUIRE(mk_event_serialization(ev.get()) != nullptr);
}

TEST_CASE("mk_event_create_() deals with invalid JSON event key") {
  nlohmann::json json;
  json["key"] = std::string{(char *)binary_input, sizeof(binary_input)};
  json["value"]["failure"] = "foobar";
  // Before #1728 was fixed, the following statement was crashing
  mk_unique_event ev{mk_event_create_(json)};
  REQUIRE(ev != nullptr);
  REQUIRE(mk_event_serialization(ev.get()) != nullptr);
}

TEST_CASE("mk_event_create_() correctly encodes NULL bytes") {
  const uint8_t zeroterm[] = {115, 98, 115, 0, 115, 98, 115};
  const std::string zerostring{(const char *)zeroterm, sizeof(zeroterm)};
  nlohmann::json json;
  json[zerostring] = zerostring;
  mk_unique_event ev{mk_event_create_(json)};
  REQUIRE(ev != nullptr);
  auto seriop = mk_event_serialization(ev.get());
  REQUIRE(seriop != nullptr);
  auto serios = std::string{seriop};
  REQUIRE(serios == R"({"sbs\u0000sbs":"sbs\u0000sbs"})");
}

TEST_CASE("mk_event_create_() correctly guarantees UTF-7 output") {
  // (Cultural note: despite being born in Sanremo I'm actually opposed to
  // the festival, which I consider low quality music, and I fancy much more
  // Rock in the Casbah, a summer festival in Sanremo's casbah.)
  const std::string sanremo{"Perché Sanremo è Sanremo 😇"};
  nlohmann::json json;
  json[sanremo] = sanremo;
  mk_unique_event ev{mk_event_create_(json)};
  REQUIRE(ev != nullptr);
  auto seriop = mk_event_serialization(ev.get());
  REQUIRE(seriop != nullptr);
  auto serios = std::string{seriop};
  auto ex = R"({"Perch\u00e9 Sanremo \u00e8 Sanremo \ud83d\ude07":"Perch\u00e9 Sanremo \u00e8 Sanremo \ud83d\ude07"})";
  REQUIRE(serios == ex);
}
