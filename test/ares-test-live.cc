// This file includes tests that attempt to do real lookups
// of DNS names using the local machine's live infrastructure.
// As a result, we don't check the results very closely, to allow
// for varying local configurations.

#include "ares-test.h"

#include <netdb.h>

namespace ares {
namespace test {

// Use the address of Google's public DNS servers as example addresses that are
// likely to be accessible everywhere/everywhen.
unsigned char gdns_addr4[4] = {0x08, 0x08, 0x08, 0x08};
unsigned char gdns_addr6[16] = {0x20, 0x01, 0x48, 0x60, 0x48, 0x60, 0x00, 0x00,
                                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x88, 0x88};

TEST_F(DefaultChannelTest, LiveGetHostByNameV4) {
  HostResult result;
  ares_gethostbyname(channel_, "www.google.com.", AF_INET, HostCallback, &result);
  Process();
  EXPECT_TRUE(result.done_);
  EXPECT_EQ(ARES_SUCCESS, result.status_);
  EXPECT_LT(0, (int)result.host_.addrs_.size());
  EXPECT_EQ(AF_INET, result.host_.addrtype_);
}

TEST_F(DefaultChannelTest, LiveGetHostByNameV6) {
  HostResult result;
  ares_gethostbyname(channel_, "www.google.com.", AF_INET6, HostCallback, &result);
  Process();
  EXPECT_TRUE(result.done_);
  EXPECT_EQ(ARES_SUCCESS, result.status_);
  EXPECT_LT(0, (int)result.host_.addrs_.size());
  EXPECT_EQ(AF_INET6, result.host_.addrtype_);
}

TEST_F(DefaultChannelTest, LiveGetHostByAddrV4) {
  HostResult result;
  ares_gethostbyaddr(channel_, gdns_addr4, sizeof(gdns_addr4), AF_INET, HostCallback, &result);
  Process();
  EXPECT_TRUE(result.done_);
  EXPECT_EQ(ARES_SUCCESS, result.status_);
  EXPECT_LT(0, (int)result.host_.addrs_.size());
  EXPECT_EQ(AF_INET, result.host_.addrtype_);
}

TEST_F(DefaultChannelTest, LiveGetHostByAddrV6) {
  HostResult result;
  ares_gethostbyaddr(channel_, gdns_addr6, sizeof(gdns_addr6), AF_INET6, HostCallback, &result);
  Process();
  EXPECT_TRUE(result.done_);
  EXPECT_EQ(ARES_SUCCESS, result.status_);
  EXPECT_LT(0, (int)result.host_.addrs_.size());
  EXPECT_EQ(AF_INET6, result.host_.addrtype_);
}

TEST_P(DefaultChannelModeTest, LiveGetLocalhostByNameV4) {
  HostResult result;
  ares_gethostbyname(channel_, "localhost", AF_INET, HostCallback, &result);
  Process();
  EXPECT_TRUE(result.done_);
  if (result.status_ != ARES_ENOTFOUND) {
    EXPECT_EQ(ARES_SUCCESS, result.status_);
    EXPECT_EQ(1, (int)result.host_.addrs_.size());
    EXPECT_EQ(AF_INET, result.host_.addrtype_);
    EXPECT_NE(std::string::npos, result.host_.name_.find("localhost"));
  }
}

TEST_P(DefaultChannelModeTest, LiveGetLocalhostByNameV6) {
  HostResult result;
  ares_gethostbyname(channel_, "localhost", AF_INET6, HostCallback, &result);
  Process();
  EXPECT_TRUE(result.done_);
  if (result.status_ == ARES_SUCCESS) {
    EXPECT_EQ(1, (int)result.host_.addrs_.size());
    EXPECT_EQ(AF_INET6, result.host_.addrtype_);
    std::stringstream ss;
    ss << HostEnt(result.host_);
    EXPECT_NE(std::string::npos, result.host_.name_.find("localhost"));
  }
}

TEST_P(DefaultChannelModeTest, LiveGetLocalhostByNameIPV4) {
  HostResult result;
  ares_gethostbyname(channel_, "127.0.0.1", AF_INET, HostCallback, &result);
  Process();
  EXPECT_TRUE(result.done_);
  EXPECT_EQ(ARES_SUCCESS, result.status_);
  EXPECT_EQ(1, (int)result.host_.addrs_.size());
  EXPECT_EQ(AF_INET, result.host_.addrtype_);
  std::stringstream ss;
  ss << HostEnt(result.host_);
  EXPECT_EQ("{'127.0.0.1' aliases=[] addrs=[127.0.0.1]}", ss.str());
}

TEST_P(DefaultChannelModeTest, LiveGetLocalhostByNameIPV6) {
  HostResult result;
  ares_gethostbyname(channel_, "::1", AF_INET6, HostCallback, &result);
  Process();
  EXPECT_TRUE(result.done_);
  if (result.status_ != ARES_ENOTFOUND) {
    EXPECT_EQ(ARES_SUCCESS, result.status_);
    EXPECT_EQ(1, (int)result.host_.addrs_.size());
    EXPECT_EQ(AF_INET6, result.host_.addrtype_);
    std::stringstream ss;
    ss << HostEnt(result.host_);
    EXPECT_EQ("{'::1' aliases=[] addrs=[0000:0000:0000:0000:0000:0000:0000:0001]}", ss.str());
  }
}

TEST_P(DefaultChannelModeTest, LiveGetLocalhostFailFamily) {
  HostResult result;
  ares_gethostbyname(channel_, "127.0.0.1", AF_INET+AF_INET6, HostCallback, &result);
  Process();
  EXPECT_TRUE(result.done_);
  EXPECT_EQ(ARES_ENOTIMP, result.status_);
}

TEST_P(DefaultChannelModeTest, LiveGetLocalhostByAddrV4) {
  HostResult result;
  struct in_addr addr;
  addr.s_addr = htonl(INADDR_LOOPBACK);
  ares_gethostbyaddr(channel_, &addr, sizeof(addr), AF_INET, HostCallback, &result);
  Process();
  EXPECT_TRUE(result.done_);
  if (result.status_ != ARES_ENOTFOUND) {
    EXPECT_EQ(ARES_SUCCESS, result.status_);
    EXPECT_LT(0, (int)result.host_.addrs_.size());
    EXPECT_EQ(AF_INET, result.host_.addrtype_);
    EXPECT_NE(std::string::npos,
              result.host_.name_.find("localhost"));
  }
}

TEST_P(DefaultChannelModeTest, LiveGetLocalhostByAddrV6) {
  HostResult result;
  struct in6_addr addr = in6addr_loopback;
  ares_gethostbyaddr(channel_, &addr, sizeof(addr), AF_INET6, HostCallback, &result);
  Process();
  EXPECT_TRUE(result.done_);
  if (result.status_ != ARES_ENOTFOUND) {
    EXPECT_EQ(ARES_SUCCESS, result.status_);
    EXPECT_LT(0, (int)result.host_.addrs_.size());
    EXPECT_EQ(AF_INET6, result.host_.addrtype_);
    EXPECT_NE(std::string::npos,
              result.host_.name_.find("localhost"));
  }
}

TEST_P(DefaultChannelModeTest, LiveGetHostByAddrFailFamily) {
  HostResult result;
  unsigned char addr[4] = {8, 8, 8, 8};
  ares_gethostbyaddr(channel_, addr, sizeof(addr), AF_INET6+AF_INET,
                     HostCallback, &result);
  EXPECT_TRUE(result.done_);
  EXPECT_EQ(ARES_ENOTIMP, result.status_);
}

TEST_P(DefaultChannelModeTest, LiveGetHostByAddrFailAddrSize) {
  HostResult result;
  unsigned char addr[4] = {8, 8, 8, 8};
  ares_gethostbyaddr(channel_, addr, sizeof(addr) - 1, AF_INET,
                     HostCallback, &result);
  EXPECT_TRUE(result.done_);
  EXPECT_EQ(ARES_ENOTIMP, result.status_);
}

TEST_P(DefaultChannelModeTest, LiveGetHostByAddrFailAlloc) {
  HostResult result;
  unsigned char addr[4] = {8, 8, 8, 8};
  SetAllocFail(1);
  ares_gethostbyaddr(channel_, addr, sizeof(addr), AF_INET,
                     HostCallback, &result);
  EXPECT_TRUE(result.done_);
  EXPECT_EQ(ARES_ENOMEM, result.status_);
}

INSTANTIATE_TEST_CASE_P(Modes, DefaultChannelModeTest,
                        ::testing::Values("f", "b", "fb", "bf"));

TEST_F(DefaultChannelTest, LiveSearchA) {
  SearchResult result;
  ares_search(channel_, "www.facebook.com.", ns_c_in, ns_t_a,
              SearchCallback, &result);
  Process();
  EXPECT_TRUE(result.done_);
  EXPECT_EQ(ARES_SUCCESS, result.status_);
}

TEST_F(DefaultChannelTest, LiveSearchEmptyA) {
  SearchResult result;
  ares_search(channel_, "", ns_c_in, ns_t_a,
              SearchCallback, &result);
  Process();
  EXPECT_TRUE(result.done_);
  EXPECT_NE(ARES_SUCCESS, result.status_);
}

TEST_F(DefaultChannelTest, LiveSearchNS) {
  SearchResult result;
  ares_search(channel_, "google.com.", ns_c_in, ns_t_ns,
              SearchCallback, &result);
  Process();
  EXPECT_TRUE(result.done_);
  EXPECT_EQ(ARES_SUCCESS, result.status_);
}

TEST_F(DefaultChannelTest, LiveSearchMX) {
  SearchResult result;
  ares_search(channel_, "google.com.", ns_c_in, ns_t_mx,
              SearchCallback, &result);
  Process();
  EXPECT_TRUE(result.done_);
  EXPECT_EQ(ARES_SUCCESS, result.status_);
}

TEST_F(DefaultChannelTest, LiveSearchTXT) {
  SearchResult result;
  ares_search(channel_, "google.com.", ns_c_in, ns_t_txt,
              SearchCallback, &result);
  Process();
  EXPECT_TRUE(result.done_);
  EXPECT_EQ(ARES_SUCCESS, result.status_);
}

TEST_F(DefaultChannelTest, LiveSearchSOA) {
  SearchResult result;
  ares_search(channel_, "google.com.", ns_c_in, ns_t_soa,
              SearchCallback, &result);
  Process();
  EXPECT_TRUE(result.done_);
  EXPECT_EQ(ARES_SUCCESS, result.status_);
}

TEST_F(DefaultChannelTest, LiveSearchSRV) {
  SearchResult result;
  ares_search(channel_, "_imap._tcp.gmail.com.", ns_c_in, ns_t_srv,
              SearchCallback, &result);
  Process();
  EXPECT_TRUE(result.done_);
  EXPECT_EQ(ARES_SUCCESS, result.status_);
}

TEST_F(DefaultChannelTest, LiveSearchANY) {
  SearchResult result;
  ares_search(channel_, "facebook.com.", ns_c_in, ns_t_any,
              SearchCallback, &result);
  Process();
  EXPECT_TRUE(result.done_);
  EXPECT_EQ(ARES_SUCCESS, result.status_);
}

TEST_F(DefaultChannelTest, LiveGetNameInfoV4) {
  NameInfoResult result;
  struct sockaddr_in sockaddr;
  memset(&sockaddr, 0, sizeof(sockaddr));
  sockaddr.sin_family = AF_INET;
  sockaddr.sin_port = htons(53);
  sockaddr.sin_addr.s_addr = htonl(0x08080808);
  ares_getnameinfo(channel_, (const struct sockaddr*)&sockaddr, sizeof(sockaddr),
                   ARES_NI_LOOKUPHOST|ARES_NI_LOOKUPSERVICE|ARES_NI_UDP,
                   NameInfoCallback, &result);
  Process();
  EXPECT_TRUE(result.done_);
  EXPECT_EQ(ARES_SUCCESS, result.status_);
}

TEST_F(DefaultChannelTest, LiveGetNameInfoV6) {
  NameInfoResult result;
  struct sockaddr_in6 sockaddr;
  memset(&sockaddr, 0, sizeof(sockaddr));
  sockaddr.sin6_family = AF_INET6;
  sockaddr.sin6_port = htons(53);
  memcpy(sockaddr.sin6_addr.s6_addr, gdns_addr6, 16);
  ares_getnameinfo(channel_, (const struct sockaddr*)&sockaddr, sizeof(sockaddr),
                   ARES_NI_LOOKUPHOST|ARES_NI_LOOKUPSERVICE|ARES_NI_UDP,
                   NameInfoCallback, &result);
  Process();
  EXPECT_TRUE(result.done_);
  EXPECT_EQ(ARES_SUCCESS, result.status_);
}

TEST_F(DefaultChannelTest, LiveGetNameInvalidFamily) {
  NameInfoResult result;
  struct sockaddr_in6 sockaddr;
  memset(&sockaddr, 0, sizeof(sockaddr));
  sockaddr.sin6_family = AF_INET6 + AF_INET;
  sockaddr.sin6_port = htons(53);
  memcpy(sockaddr.sin6_addr.s6_addr, gdns_addr6, 16);
  ares_getnameinfo(channel_, (const struct sockaddr*)&sockaddr, sizeof(sockaddr),
                   ARES_NI_LOOKUPHOST|ARES_NI_LOOKUPSERVICE|ARES_NI_UDP,
                   NameInfoCallback, &result);
  Process();
  EXPECT_TRUE(result.done_);
  EXPECT_EQ(ARES_ENOTIMP, result.status_);
}

TEST_F(DefaultChannelTest, LiveGetServiceInfo) {
  NameInfoResult result;
  struct sockaddr_in sockaddr;
  memset(&sockaddr, 0, sizeof(sockaddr));
  sockaddr.sin_family = AF_INET;
  sockaddr.sin_port = htons(53);
  sockaddr.sin_addr.s_addr = htonl(0x08080808);
  // Just look up service info
  ares_getnameinfo(channel_, (const struct sockaddr*)&sockaddr, sizeof(sockaddr),
                   ARES_NI_LOOKUPSERVICE|ARES_NI_UDP,
                   NameInfoCallback, &result);
  Process();
  EXPECT_TRUE(result.done_);
  EXPECT_EQ(ARES_SUCCESS, result.status_);
  EXPECT_EQ("", result.node_);
}

TEST_F(DefaultChannelTest, LiveGetNameInfoAllocFail) {
  NameInfoResult result;
  struct sockaddr_in sockaddr;
  memset(&sockaddr, 0, sizeof(sockaddr));
  sockaddr.sin_family = AF_INET;
  sockaddr.sin_port = htons(53);
  sockaddr.sin_addr.s_addr = htonl(0x08080808);
  SetAllocFail(1);
  ares_getnameinfo(channel_, (const struct sockaddr*)&sockaddr, sizeof(sockaddr),
                   ARES_NI_LOOKUPHOST|ARES_NI_LOOKUPSERVICE|ARES_NI_UDP,
                   NameInfoCallback, &result);
  Process();
  EXPECT_TRUE(result.done_);
  EXPECT_EQ(ARES_ENOMEM, result.status_);
}

}  // namespace test
}  // namespace ares