#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "dns.h"
#include "test.h"

#define DEBUG
#include "debug.h"

int main() {
  char* lookup_req_dom = "www.lookupreq.com";
  char* rev_lookup_resp_dom = "www.revlookupresp.com";
  uint32_t lookup_resp_ip = 0xCAFEBABE;
  uint32_t rev_lookup_req_ip = 0xDEADBEEF;

  // XXX: Sanity!
  DUMPU(sizeof(dns_packet));
  DUMPU(sizeof(dns_message));
  DUMPU(sizeof(payload));
  DUMPU(sizeof(uint32_t));
  DUMPU(sizeof(char*));
  DUMPU(sizeof(dns_packet) - sizeof(payload) + strlen(lookup_req_dom) + 1);
  DUMPU(sizeof(dns_packet) - sizeof(payload) + sizeof(uint32_t));


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


  dns_packet* lookup_failed = new_dns_packet_lookup_failed(true);
  TEST_ASSERT(lookup_failed->msg = LOOKUP_RESPONSE);
  TEST_ASSERT(lookup_failed->len == 0);
  TEST_ASSERT(lookup_failed->checksum == checksum(lookup_failed));
  DUMPI(lookup_failed->checksum);
  TEST_ASSERT(lookup_failed->checksum == (4 + 4 + 4 + 4));

  dns_packet* rev_lookup_failed = new_dns_packet_lookup_failed(false);
  TEST_ASSERT(rev_lookup_failed->msg = REVERSE_LOOKUP_RESPONSE);
  TEST_ASSERT(rev_lookup_failed->len == 0);
  TEST_ASSERT(rev_lookup_failed->checksum == checksum(rev_lookup_failed));
  DUMPI(rev_lookup_failed->checksum);
  TEST_ASSERT(rev_lookup_failed->checksum == (4 + 4 + 4 + 4));

  // TODO checksum checking
  DUMPI(lookup_req->checksum); 
  DUMPI(lookup_resp->checksum); 
  DUMPI(rev_lookup_req->checksum); 
  DUMPI(rev_lookup_resp->checksum); 
  DUMPI(strlen(lookup_req_dom));
  DUMPI(strlen(rev_lookup_resp_dom));

  TEST_ASSERT(lookup_req->checksum == (4 + 4 + 18 + 4)); 
  TEST_ASSERT(lookup_resp->checksum == (4 + 4 + 4 + 4)); 
  TEST_ASSERT(rev_lookup_req->checksum == (4 + 4 + 4 + 4)); 
  TEST_ASSERT(rev_lookup_resp->checksum == (4 + 4 + 22 + 4)); 

  

  TEST_ASSERT(lookup_req->checksum == (checksum(lookup_req))); 
  TEST_ASSERT(lookup_resp->checksum == (checksum(lookup_resp))); 
  TEST_ASSERT(rev_lookup_req->checksum == (checksum(rev_lookup_req))); 
  TEST_ASSERT(rev_lookup_resp->checksum == (checksum(rev_lookup_resp))); 


  destroy_dns_packet(&lookup_req);
  TEST_ASSERT(lookup_req == NULL);


  destroy_dns_packet(&lookup_resp);
  TEST_ASSERT(lookup_resp == NULL);


  destroy_dns_packet(&rev_lookup_req);
  TEST_ASSERT(rev_lookup_req == NULL);


  destroy_dns_packet(&rev_lookup_resp);
  TEST_ASSERT(rev_lookup_resp == NULL);


  destroy_dns_packet(&lookup_failed);
  TEST_ASSERT(lookup_failed == NULL);


  destroy_dns_packet(&rev_lookup_failed);
  TEST_ASSERT(rev_lookup_failed == NULL);


  return 0;
}
