#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "dns.h"
#include "test.h"

int main() {
  char* lookup_req_dom = "www.lookupreq.com";
  char* rev_lookup_resp_dom = "www.revlookupresp.com";
  uint32_t lookup_resp_ip = 0xCAFEBABE;
  uint32_t rev_lookup_req_ip = 0xDEADBEEF;


  dns_packet* lookup_req = new_dns_packet_dom(lookup_req_dom, true);
  
  TEST_ASSERT(lookup_req != NULL);
  TEST_ASSERT(is_valid_dns_packet(lookup_req));
  TEST_ASSERT(lookup_req->msg == LOOKUP_REQUEST);
  TEST_ASSERT(strcmp(lookup_req->contents.domain_name, lookup_req_dom) == 0);
  TEST_ASSERT(strlen(lookup_req->contents.domain_name) + 1 == lookup_req->len);


  dns_packet* lookup_resp = new_dns_packet_ip(lookup_resp_ip, false);
  
  TEST_ASSERT(lookup_resp != NULL);
  TEST_ASSERT(is_valid_dns_packet(lookup_resp));
  TEST_ASSERT(lookup_resp->msg == LOOKUP_RESPONSE);
  TEST_ASSERT(lookup_resp->contents.ipv4_addr == lookup_resp_ip);
  TEST_ASSERT(sizeof(lookup_resp->contents.ipv4_addr) == lookup_resp->len);
  

  dns_packet* rev_lookup_req = new_dns_packet_ip(rev_lookup_req_ip, true);

  TEST_ASSERT(rev_lookup_req != NULL);
  TEST_ASSERT(is_valid_dns_packet(rev_lookup_req));
  TEST_ASSERT(rev_lookup_req->msg == REVERSE_LOOKUP_REQUEST);
  TEST_ASSERT(rev_lookup_req->contents.ipv4_addr == rev_lookup_req_ip);
  TEST_ASSERT(sizeof(rev_lookup_req->contents.ipv4_addr) == rev_lookup_req->len);

  dns_packet* rev_lookup_resp = new_dns_packet_dom(rev_lookup_resp_dom, false);

  TEST_ASSERT(rev_lookup_resp != NULL);
  TEST_ASSERT(is_valid_dns_packet(rev_lookup_resp));
  TEST_ASSERT(rev_lookup_resp->msg == REVERSE_LOOKUP_RESPONSE);
  TEST_ASSERT(strcmp(rev_lookup_resp->contents.domain_name, rev_lookup_resp_dom) == 0);
  TEST_ASSERT(strlen(rev_lookup_resp->contents.domain_name) + 1 == rev_lookup_resp->len);


  destroy_dns_packet(&lookup_req);
  TEST_ASSERT(lookup_req == NULL);


  destroy_dns_packet(&lookup_resp);
  TEST_ASSERT(lookup_resp == NULL);


  destroy_dns_packet(&rev_lookup_req);
  TEST_ASSERT(rev_lookup_req == NULL);


  destroy_dns_packet(&rev_lookup_resp);
  TEST_ASSERT(rev_lookup_resp == NULL);


  // TODO checksum checking


  return 0;
}
