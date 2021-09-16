#include "hw3_p3.h"

IPV4_validation validateIPV4List(List_t* packets) {
    IPV4_validation valid;
    //Insert Code Here
	node_t* n;
	int offset_check = 0;
	IPV4_node* prev_ip_node = NULL;
	for(n=packets->head; n!=NULL; n=n->next){
		IPV4_node* ip_node = n->value;
		IPV4_header* ip_header = (IPV4_header*)(ip_node->sop);
		uint16_t check_sum = verifyIPV4Checksum(ip_header);
		if(check_sum != 0x0000){
			valid.packet = ip_node;
			valid.errcode = ERRSUM;
			return valid;
		}
		int pl_len = ip_header->total_length - ip_header->header_length*4;
		char* pl = (char*)ip_header + ip_header->header_length*4;
		uint16_t f_offset = ip_header->fragment_offset;
		if(offset_check != f_offset){
			valid.packet = prev_ip_node;
			valid.errcode = ERRHOLE;
			return valid;
		}
		offset_check = f_offset + pl_len;
		prev_ip_node = ip_node;
		ip_node->fragment_offset = f_offset;
		ip_node->payload = pl;
		ip_node->payload_len = pl_len;
	}
	valid.packet = NULL;
	valid.errcode = NOERR;

    return valid;
}
