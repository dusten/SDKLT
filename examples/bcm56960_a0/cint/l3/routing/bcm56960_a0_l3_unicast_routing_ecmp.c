/*! \file bcm56960_a0_l3_unicast_routing_ecmp.c
 *
 * Demonstrate programming L3 IPv4 unicast routing with ecmp.
 */
/*
 * Copyright: (c) 2018 Broadcom. All Rights Reserved. "Broadcom" refers to 
 * Broadcom Limited and/or its subsidiaries.
 * 
 * Broadcom Switch Software License
 * 
 * This license governs the use of the accompanying Broadcom software. Your 
 * use of the software indicates your acceptance of the terms and conditions 
 * of this license. If you do not agree to the terms and conditions of this 
 * license, do not use the software.
 * 1. Definitions
 *    "Licensor" means any person or entity that distributes its Work.
 *    "Software" means the original work of authorship made available under 
 *    this license.
 *    "Work" means the Software and any additions to or derivative works of 
 *    the Software that are made available under this license.
 *    The terms "reproduce," "reproduction," "derivative works," and 
 *    "distribution" have the meaning as provided under U.S. copyright law.
 *    Works, including the Software, are "made available" under this license 
 *    by including in or with the Work either (a) a copyright notice 
 *    referencing the applicability of this license to the Work, or (b) a copy 
 *    of this license.
 * 2. Grant of Copyright License
 *    Subject to the terms and conditions of this license, each Licensor 
 *    grants to you a perpetual, worldwide, non-exclusive, and royalty-free 
 *    copyright license to reproduce, prepare derivative works of, publicly 
 *    display, publicly perform, sublicense and distribute its Work and any 
 *    resulting derivative works in any form.
 * 3. Grant of Patent License
 *    Subject to the terms and conditions of this license, each Licensor 
 *    grants to you a perpetual, worldwide, non-exclusive, and royalty-free 
 *    patent license to make, have made, use, offer to sell, sell, import, and 
 *    otherwise transfer its Work, in whole or in part. This patent license 
 *    applies only to the patent claims licensable by Licensor that would be 
 *    infringed by Licensor's Work (or portion thereof) individually and 
 *    excluding any combinations with any other materials or technology.
 *    If you institute patent litigation against any Licensor (including a 
 *    cross-claim or counterclaim in a lawsuit) to enforce any patents that 
 *    you allege are infringed by any Work, then your patent license from such 
 *    Licensor to the Work shall terminate as of the date such litigation is 
 *    filed.
 * 4. Redistribution
 *    You may reproduce or distribute the Work only if (a) you do so under 
 *    this License, (b) you include a complete copy of this License with your 
 *    distribution, and (c) you retain without modification any copyright, 
 *    patent, trademark, or attribution notices that are present in the Work.
 * 5. Derivative Works
 *    You may specify that additional or different terms apply to the use, 
 *    reproduction, and distribution of your derivative works of the Work 
 *    ("Your Terms") only if (a) Your Terms provide that the limitations of 
 *    Section 7 apply to your derivative works, and (b) you identify the 
 *    specific derivative works that are subject to Your Terms. 
 *    Notwithstanding Your Terms, this license (including the redistribution 
 *    requirements in Section 4) will continue to apply to the Work itself.
 * 6. Trademarks
 *    This license does not grant any rights to use any Licensor's or its 
 *    affiliates' names, logos, or trademarks, except as necessary to 
 *    reproduce the notices described in this license.
 * 7. Limitations
 *    Platform. The Work and any derivative works thereof may only be used, or 
 *    intended for use, with a Broadcom switch integrated circuit.
 *    No Reverse Engineering. You will not use the Work to disassemble, 
 *    reverse engineer, decompile, or attempt to ascertain the underlying 
 *    technology of a Broadcom switch integrated circuit.
 * 8. Termination
 *    If you violate any term of this license, then your rights under this 
 *    license (including the license grants of Sections 2 and 3) will 
 *    terminate immediately.
 * 9. Disclaimer of Warranty
 *    THE WORK IS PROVIDED "AS IS" WITHOUT WARRANTIES OR CONDITIONS OF ANY 
 *    KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WARRANTIES OR CONDITIONS OF 
 *    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE OR 
 *    NON-INFRINGEMENT. YOU BEAR THE RISK OF UNDERTAKING ANY ACTIVITIES UNDER 
 *    THIS LICENSE. SOME STATES' CONSUMER LAWS DO NOT ALLOW EXCLUSION OF AN 
 *    IMPLIED WARRANTY, SO THIS DISCLAIMER MAY NOT APPLY TO YOU.
 * 10. Limitation of Liability
 *    EXCEPT AS PROHIBITED BY APPLICABLE LAW, IN NO EVENT AND UNDER NO LEGAL 
 *    THEORY, WHETHER IN TORT (INCLUDING NEGLIGENCE), CONTRACT, OR OTHERWISE 
 *    SHALL ANY LICENSOR BE LIABLE TO YOU FOR DAMAGES, INCLUDING ANY DIRECT, 
 *    INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF 
 *    OR RELATED TO THIS LICENSE, THE USE OR INABILITY TO USE THE WORK 
 *    (INCLUDING BUT NOT LIMITED TO LOSS OF GOODWILL, BUSINESS INTERRUPTION, 
 *    LOST PROFITS OR DATA, COMPUTER FAILURE OR MALFUNCTION, OR ANY OTHER 
 *    COMMERCIAL DAMAGES OR LOSSES), EVEN IF THE LICENSOR HAS BEEN ADVISED OF 
 *    THE POSSIBILITY OF SUCH DAMAGES.
 */

/*
* $Copyright: Broadcom Ltd.$
*
*
*
*  Purpose: Demonstrate L3 unicast routing over ECMP.
*
*  Description: Route global route 192.168.0.100 to ECMP nexthop 2 which based
*               on hash computation would send the packet out of port 1 or 3.
*
*  Setup diagram:
*                      +--------------------------+      XXXXX  ECMP group 2
*                      |                          | 1   XX   XX
*  DIP: 192.168.0.100  |          SVK             +-----X-----X--->
*                      |                          |    XX     X
*         +----------> |                          | 3  X      X
*                   2  |                          +----X------X--->
*                      |                          |    XX     X
*                      +--------------------------+     X     X
*                                                       XXX  X
*                                                         XXX
*
*
*  Ingress port(s): 2
*
*  Egress ports(s): 1 or 3. In this example based on packet hash computed by
*                   the switch, packet would be sent to port 1.
*
*  Egress CoS queue: Not relevant.
*
*  Device: bcm56960_a0.
*
*  NGSDK Version: 0.7
*
*  Notes: This is self sufficient script i.e. it programs all the logical
*         tables required to make this flow work.
*
*  Verification method:
*       Packet sampling will be enabled on port 1. The packet should
*       be as follows:
*
* Watch information: (interface 1)
* [RX metadata information]
*         HGI                             :2
*         PKT_LENGTH                      :72
*         SRC_PORT_NUM                    :1
*         OUTER_VID                       :2
*         TIMESTAMP_TYPE                  :3
*         SWITCH                          :1
*         INCOMING_TAG_STATUS             :2
* [RX reasons]
*         CPU_SFLOW_SRC
* Packet raw data (68):
* 0000: 00 00 00 00 00 0b 00 00 - 00 aa aa aa 81 00 00 02
* 0010: 08 00 45 00 00 2e 00 00 - 00 00 3f 01 f8 b6 c0 a8
* 0020: 01 64 c0 a8 00 64 00 00 - b7 ae 00 00 00 00 00 01
* 0030: 02 03 04 05 06 07 08 09 - 0a 0b 0c 0d 0e 0f 10 11
* 0040: 79 ef 51 36
 */

/* Custom commit function for LTs which does commit and status check */
int bcmlt_custom_entry_commit(bcmlt_entry_handle_t entry_hdl,
                              bcmlt_opcode_t op,
                              bcmlt_priority_level_t prio)
{
    int rv;
    bcmlt_entry_info_t entry_info;

    /* Commit the entry synchronously */
    rv = bcmlt_entry_commit(entry_hdl, op, prio);

    if (rv != SHR_E_NONE) {
        printf("bcmlt_entry_commit failed rv: %d msg: %s \n",
               rv, bcm_errmsg(rv));
        return rv;
    }

    rv = bcmlt_entry_info_get(entry_hdl, &entry_info);
    if (rv != SHR_E_NONE) {
        printf("Unable to get the entry_info rv: %d error: %s\n", rv,
               bcm_errmsg(rv));
        return rv;
    }

    if (entry_info.status != SHR_E_NONE) {
        printf("unit: %d commit to LT %s failed status: %s\n",
               entry_info.unit, entry_info.table_name,
               bcm_errmsg(entry_info.status));
        return entry_info.status;
    }
    printf("unit: %d LT %s commit success!\n", entry_info.unit,
           entry_info.table_name);
    return entry_info.status;

}

/* Custom commit function for PTs which does commit and status check */
int bcmlt_custom_pt_entry_commit(bcmlt_entry_handle_t entry_hdl,
                              bcmlt_pt_opcode_t op,
                              bcmlt_priority_level_t prio)
{
    int rv;
    bcmlt_entry_info_t entry_info;

    /* Commit the entry synchronously */
    rv = bcmlt_pt_entry_commit(entry_hdl, op, prio);

    if (rv != SHR_E_NONE) {
        printf("bcmlt_pt_entry_commit failed rv: %d msg: %s \n",
               rv, bcm_errmsg(rv));
        return rv;
    }

    rv = bcmlt_entry_info_get(entry_hdl, &entry_info);
    if (rv != SHR_E_NONE) {
        printf("Unable to get the entry_info rv: %d error: %s\n", rv,
               bcm_errmsg(rv));
        return rv;
    }

    if (entry_info.status != SHR_E_NONE) {
        printf("unit: %d commit to PT %s failed status: %s\n",
               entry_info.unit, entry_info.table_name,
               bcm_errmsg(entry_info.status));
        return entry_info.status;
    }
    printf("unit: %d PT %s commit success!\n", entry_info.unit,
           entry_info.table_name);
    return entry_info.status;
}

int main(int unit)
{
    bcmlt_entry_handle_t entry_hdl;
    int rv;
    int start_port = 1;
    int end_port = 3;
    int port;
    uint64_t data;
    uint32_t up;
    char* vlan_stg_str[136] = {NULL};
    uint64_t nhop_id_arr[128] = {0};
    uint64_t vlan_pbmp[3] = {0};
    int vlan_id;

    /* Diag shell commands can be executed using bshell API.*/

    /* Setup packet IO from diag shell. */
    bshell(unit, "pktdev init");
    /* Map all queues to channel 1. */
    bshell(unit, "pktdev chan queuemap 1 highword=0xffff lowword=0xffffffff");
    /* Start packet watcher to capture packets at CPU. */
    bshell(unit, "rx watcher create");

/*
 *
 * Procedure for programming an entry:
 * 1. Allocate entry handle using the API bcmlt_entry_allocate.
 *    a. Same handle can be used for multiple commits for the same table.
 *
 * 2. Set fields of the entry.
 *    a. Use the API bcmlt_entry_field_add if the field value is integer.
 *    b. Use the API bcmlt_entry_symbol_add if the field value is string
 *       (fields of type enum).
 *    c. Use the API bcmlt_entry_array_add if the field is of type array
 *       and accepts integer values(array of integers).
 *    d. Use the API bcmlt_entry_array_symbol_add if the field is of type array
 *       and accepts string values(array of enums).
 *
 * 3. Commit entry using bcmlt_entry_commit.
 *    a. To create new entry use opcode BCMLT_OPCODE_INSERT.
 *    b. To retrieve entry use opcode BCMLT_OPCODE_LOOKUP.
 *    c. To update non key fields of the entry use opcode BCMLT_OPCODE_UPDATE.
 *
 * 4. To find the status of the commit, use the API bcmlt_entry_info_get.
 *    a. bcmlt_entry_info_t.status will tell the status of the commit for a
 *       given entry.
 *
 * 5. Free the handle allocated using the API bcmlt_entry_free.
 */

/*
 * This example uses a custom bcmlt_custom_entry_commit() function which commits
 * the entry and checks the commit status.
 */

/* For more details about bcmlt APIs see logical table documentation. */

    /* Enable Link Manager(linkscan task). */

    rv = bcmlt_entry_allocate(unit, "LM_CONTROL", &entry_hdl);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_add(entry_hdl, "SCAN_ENABLE", 1);
    if (rv != SHR_E_NONE) return rv;

    /* Commit the entry synchronously */
    rv = bcmlt_custom_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT,
                                   BCMLT_PRIORITY_NORMAL);
    if (rv != SHR_E_NONE) return rv;

    /* Free the entry handle */
    rv = bcmlt_entry_free(entry_hdl);
    if (rv != SHR_E_NONE) return rv;

    /* Configure software Linkscan mode for ports 1-3. */
    rv = bcmlt_entry_allocate(unit, "LM_PORT_CONTROL", &entry_hdl);
    if (rv != SHR_E_NONE) return rv;

    for (port = start_port; port <= end_port; port++) {
        rv = bcmlt_entry_field_add(entry_hdl, "PORT_ID", port);
        if (rv != SHR_E_NONE) return rv;

        rv = bcmlt_entry_field_symbol_add(entry_hdl, "LINKSCAN_MODE",
                                          "SOFTWARE");
        if (rv != SHR_E_NONE) return rv;

        /* Commit the entry synchronously */
        rv = bcmlt_custom_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT,
                                       BCMLT_PRIORITY_NORMAL);
        if (rv != SHR_E_NONE) return rv;
    }

    /* Free the entry handle */
    rv = bcmlt_entry_free(entry_hdl);
    if (rv != SHR_E_NONE) return rv;

    /* Configure ports 1-3 in loopback mode. */

    rv = bcmlt_entry_allocate(unit, "PC_PORT", &entry_hdl);
    if (rv != SHR_E_NONE) return rv;

    for (port = start_port; port <= end_port; port++) {
        rv = bcmlt_entry_field_add(entry_hdl, "PORT_ID", port);
        if (rv != SHR_E_NONE) return rv;

        rv = bcmlt_entry_field_symbol_add(entry_hdl, "LOOPBACK_MODE",
                                          "PC_LPBK_MAC");
        if (rv != SHR_E_NONE) return rv;

        /* Commit the entry synchronously */
        rv = bcmlt_custom_entry_commit(entry_hdl, BCMLT_OPCODE_UPDATE,
                                       BCMLT_PRIORITY_NORMAL);
        if (rv != SHR_E_NONE) return rv;
    }

    /* Free the entry handle */
    rv = bcmlt_entry_free(entry_hdl);
    if (rv != SHR_E_NONE) return rv;

    /*
     * Allow Link Manager to collect status of the ports 1-3.
     * Default Link Manager scan interval is 250 ms.
     */

    sleep(2);

    /* Check port 1-3 status. LINK_STATE=1 indicates port is UP. */

    rv = bcmlt_entry_allocate(unit, "LM_LINK_STATE", &entry_hdl);
    if (rv != SHR_E_NONE) return rv;

    for (port = start_port; port <= end_port; port++) {
        rv = bcmlt_entry_field_add(entry_hdl, "PORT_ID", port);
        if (rv != SHR_E_NONE) return rv;

        /* Commit the entry synchronously */
        rv = bcmlt_custom_entry_commit(entry_hdl, BCMLT_OPCODE_LOOKUP,
                                       BCMLT_PRIORITY_NORMAL);
        if (rv != SHR_E_NONE) return rv;

        data = 0;
        rv = bcmlt_entry_field_get(entry_hdl, "LINK_STATE", &data);
        if (rv != SHR_E_NONE) return rv;

        up = data & 0xffffffff;
        if (up) {
            printf("Port %d is UP!\n", port);
        } else {
            printf("Port %d is DOWN!\n", port);
        }
    }

    /* Free the entry handle */
    rv = bcmlt_entry_free(entry_hdl);
    if (rv != SHR_E_NONE) return rv;

    /* Enable MAC counters for port 1-3. */

    rv = bcmlt_entry_allocate(unit, "CTR_MAC", &entry_hdl);
    if (rv != SHR_E_NONE) return rv;

    for (port = start_port; port <= end_port; port++) {
        rv = bcmlt_entry_field_add(entry_hdl, "PORT_ID", port);
        if (rv != SHR_E_NONE) return rv;

        /* Commit the entry synchronously */
        rv = bcmlt_custom_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT,
                                       BCMLT_PRIORITY_NORMAL);
        if (rv != SHR_E_NONE) return rv;
    }

    /* Free the entry handle */
    rv = bcmlt_entry_free(entry_hdl);
    if (rv != SHR_E_NONE) return rv;

    /* Configure ports 1-3 in forwarding state. */
    rv = bcmlt_entry_allocate(unit, "VLAN_STG", &entry_hdl);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_add(entry_hdl, "VLAN_STG_ID", 1);
    if (rv != SHR_E_NONE) return rv;

    for (port = start_port; port <= end_port; port++) {
        vlan_stg_str[port - start_port] = "FORWARD";
    }
    rv = bcmlt_entry_field_array_symbol_add(entry_hdl, "STATE", start_port,
             &vlan_stg_str, (end_port - start_port + 1));
    if (rv != SHR_E_NONE) return rv;

    /* Commit the entry synchronously */
    rv = bcmlt_custom_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT,
                                   BCMLT_PRIORITY_NORMAL);
    if (rv != SHR_E_NONE) return rv;

    /* Free the entry handle */
    rv = bcmlt_entry_free(entry_hdl);
    if (rv != SHR_E_NONE) return rv;

    /* Configure PORT_PKT_CONTROL to send arp request,reply to CPU. */

    rv = bcmlt_entry_allocate(unit, "PORT_PKT_CONTROL", &entry_hdl);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_add(entry_hdl, "PORT_PKT_CONTROL_ID", 3);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_add(entry_hdl, "ARP_REQUEST_TO_CPU", 1);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_add(entry_hdl, "ARP_REPLY_TO_CPU", 1);
    if (rv != SHR_E_NONE) return rv;

    /* Commit the entry synchronously */
    rv = bcmlt_custom_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT,
                                   BCMLT_PRIORITY_NORMAL);
    if (rv != SHR_E_NONE) return rv;

    /* Free the entry handle */
    rv = bcmlt_entry_free(entry_hdl);
    if (rv != SHR_E_NONE) return rv;

    /* Add VLAN tag for untagged packets. */

    rv = bcmlt_entry_allocate(unit, "VLAN_ING_TAG_ACTION_PROFILE", &entry_hdl);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_add(entry_hdl, "VLAN_ING_TAG_ACTION_PROFILE_ID", 1);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_symbol_add(entry_hdl, "UT_OTAG", "ADD");
    if (rv != SHR_E_NONE) return rv;

    /* Commit the entry synchronously */
    rv = bcmlt_custom_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT,
                                   BCMLT_PRIORITY_NORMAL);
    if (rv != SHR_E_NONE) return rv;

    /* Free the entry handle */
    rv = bcmlt_entry_free(entry_hdl);
    if (rv != SHR_E_NONE) return rv;

    /* Create VLAN 2 and include port 1. */

    rv = bcmlt_entry_allocate(unit, "VLAN", &entry_hdl);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_add(entry_hdl, "VLAN_ID", 2);
    if (rv != SHR_E_NONE) return rv;

    vlan_pbmp[0] = 0x2;
    rv = bcmlt_entry_field_array_add(entry_hdl, "EGR_MEMBER_PORTS", 0,
                                     vlan_pbmp, 3);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_array_add(entry_hdl, "ING_MEMBER_PORTS", 0,
                                     vlan_pbmp, 3);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_add(entry_hdl, "VLAN_STG_ID", 1);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_add(entry_hdl, "L3_IIF_ID", 1);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_custom_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT,
                                   BCMLT_PRIORITY_NORMAL);
    if (rv != SHR_E_NONE) return rv;

    /* Create VLAN 3 and include port 2. */
    rv = bcmlt_entry_field_add(entry_hdl, "VLAN_ID", 3);
    if (rv != SHR_E_NONE) return rv;

    vlan_pbmp[0] = 0x4;
    rv = bcmlt_entry_field_array_add(entry_hdl, "EGR_MEMBER_PORTS", 0,
                                     vlan_pbmp, 3);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_array_add(entry_hdl, "ING_MEMBER_PORTS", 0,
                                     vlan_pbmp, 3);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_add(entry_hdl, "VLAN_STG_ID", 1);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_add(entry_hdl, "L3_IIF_ID", 1);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_custom_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT,
                                   BCMLT_PRIORITY_NORMAL);
    if (rv != SHR_E_NONE) return rv;

    /* Create VLAN 4 and include port 3. */

    rv = bcmlt_entry_field_add(entry_hdl, "VLAN_ID", 4);
    if (rv != SHR_E_NONE) return rv;

    vlan_pbmp[0] = 0x8;
    rv = bcmlt_entry_field_array_add(entry_hdl, "EGR_MEMBER_PORTS", 0,
                                     vlan_pbmp, 3);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_array_add(entry_hdl, "ING_MEMBER_PORTS", 0,
                                     vlan_pbmp, 3);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_add(entry_hdl, "VLAN_STG_ID", 1);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_add(entry_hdl, "L3_IIF_ID", 1);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_custom_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT,
                                   BCMLT_PRIORITY_NORMAL);
    if (rv != SHR_E_NONE) return rv;

    /* Free the entry handle */
    rv = bcmlt_entry_free(entry_hdl);
    if (rv != SHR_E_NONE) return rv;

    /* Configure max MTU for VLANs 2-4. */

    rv = bcmlt_entry_allocate(unit, "L3_UC_MTU", &entry_hdl);
    if (rv != SHR_E_NONE) return rv;

    for (vlan_id = 2; vlan_id <= 4; vlan_id++) {
        rv = bcmlt_entry_field_add(entry_hdl, "VLAN_ID", vlan_id);
        if (rv != SHR_E_NONE) return rv;

        rv = bcmlt_entry_field_add(entry_hdl, "L3_MTU", 0x3fff);
        if (rv != SHR_E_NONE) return rv;

        rv = bcmlt_custom_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT,
                                       BCMLT_PRIORITY_NORMAL);
        if (rv != SHR_E_NONE) return rv;
    }

    /* Free the entry handle */
    rv = bcmlt_entry_free(entry_hdl);
    if (rv != SHR_E_NONE) return rv;

    /* Enable IPv4 routing on Port 2. */

    rv = bcmlt_entry_allocate(unit, "PORT", &entry_hdl);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_add(entry_hdl, "PORT_ID", 2);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_add(entry_hdl, "V4L3", 1);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_symbol_add(entry_hdl, "PORT_TYPE", "ETHERNET");
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_add(entry_hdl, "MY_MODID", 0);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_add(entry_hdl, "PORT_PKT_CONTROL_ID", 3);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_add(entry_hdl, "VLAN_ING_TAG_ACTION_PROFILE_ID", 1);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_add(entry_hdl, "ING_OVID", 3);
    if (rv != SHR_E_NONE) return rv;

    /* Commit the entry synchronously */
    rv = bcmlt_custom_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT,
                                   BCMLT_PRIORITY_NORMAL);
    if (rv != SHR_E_NONE) return rv;

    /* Free the entry handle */
    rv = bcmlt_entry_free(entry_hdl);
    if (rv != SHR_E_NONE) return rv;

    /* Create L3_IIF_PROFILE 2 and enable IPv4 routing. */

    rv = bcmlt_entry_allocate(unit, "L3_IIF_PROFILE", &entry_hdl);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_add(entry_hdl, "L3_IIF_PROFILE_ID", 2);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_add(entry_hdl, "IPV4_UC", 1);
    if (rv != SHR_E_NONE) return rv;

    /* Commit the entry synchronously */
    rv = bcmlt_custom_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT,
                                   BCMLT_PRIORITY_NORMAL);
    if (rv != SHR_E_NONE) return rv;

    /* Free the entry handle */
    rv = bcmlt_entry_free(entry_hdl);
    if (rv != SHR_E_NONE) return rv;

    /* Create L3_IIF index 1 and set VRF_ID=0. */

    rv = bcmlt_entry_allocate(unit, "L3_IIF", &entry_hdl);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_add(entry_hdl, "L3_IIF_ID", 1);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_add(entry_hdl, "VRF_ID", 0);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_add(entry_hdl, "L3_IIF_PROFILE_ID", 2);
    if (rv != SHR_E_NONE) return rv;

    /* Commit the entry synchronously */
    rv = bcmlt_custom_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT,
                                   BCMLT_PRIORITY_NORMAL);
    if (rv != SHR_E_NONE) return rv;

    /* Free the entry handle */
    rv = bcmlt_entry_free(entry_hdl);
    if (rv != SHR_E_NONE) return rv;

    /*
     * Program L2_MY_STATION to enable routing for
     * 00:00:00:bb:bb:bb + vlan_id = 3.
     */

    rv = bcmlt_entry_allocate(unit, "L2_MY_STATION", &entry_hdl);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_add(entry_hdl, "VLAN_ID", 3);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_add(entry_hdl, "VLAN_ID_MASK", 0xfff);
    if (rv != SHR_E_NONE) return rv;

    /* Beware of sign extension!. Always use ULL extension for long values */
    rv = bcmlt_entry_field_add(entry_hdl, "MAC_ADDR", 0x0000000000BBBBBBULL);
    if (rv != SHR_E_NONE) return rv;

    /* Beware of sign extension!. Always use ULL extension for long values */
    rv = bcmlt_entry_field_add(entry_hdl, "MAC_ADDR_MASK",
                               0x0000ffffffffffffULL);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_add(entry_hdl, "IPV4_TERMINATION", 1);
    if (rv != SHR_E_NONE) return rv;

    /* Commit the entry synchronously */
    rv = bcmlt_custom_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT,
                                   BCMLT_PRIORITY_NORMAL);
    if (rv != SHR_E_NONE) return rv;

    /* Free the entry handle */
    rv = bcmlt_entry_free(entry_hdl);
    if (rv != SHR_E_NONE) return rv;

    /* Create egress L3 interfaces(L3_EIF) */

    rv = bcmlt_entry_allocate(unit, "L3_EIF", &entry_hdl);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_add(entry_hdl, "L3_EIF_ID", 1);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_add(entry_hdl, "VLAN_ID", 2);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_add(entry_hdl, "MAC_SA", 0x000000AAAAAA);
    if (rv != SHR_E_NONE) return rv;

    /* Commit the entry synchronously */
    rv = bcmlt_custom_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT,
                                   BCMLT_PRIORITY_NORMAL);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_add(entry_hdl, "L3_EIF_ID", 3);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_add(entry_hdl, "VLAN_ID", 4);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_add(entry_hdl, "MAC_SA", 0x000000CCCCCC);
    if (rv != SHR_E_NONE) return rv;

    /* Commit the entry synchronously */
    rv = bcmlt_custom_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT,
                                   BCMLT_PRIORITY_NORMAL);
    if (rv != SHR_E_NONE) return rv;

    /* Free the entry handle */
    rv = bcmlt_entry_free(entry_hdl);
    if (rv != SHR_E_NONE) return rv;

    /* Create L3 nexthops. */

    rv = bcmlt_entry_allocate(unit, "L3_UC_NHOP", &entry_hdl);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_add(entry_hdl, "NHOP_ID", 5);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_add(entry_hdl, "MODPORT", 1);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_add(entry_hdl, "MODID", 0);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_add(entry_hdl, "VLAN_ID", 2);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_add(entry_hdl, "L3_EIF_ID", 1);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_add(entry_hdl, "MAC_DA", 0x00000000000B);
    if (rv != SHR_E_NONE) return rv;

    /* Commit the entry synchronously */
    rv = bcmlt_custom_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT,
                                   BCMLT_PRIORITY_NORMAL);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_add(entry_hdl, "NHOP_ID", 10);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_add(entry_hdl, "MODPORT", 3);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_add(entry_hdl, "MODID", 0);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_add(entry_hdl, "VLAN_ID", 4);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_add(entry_hdl, "L3_EIF_ID", 3);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_add(entry_hdl, "MAC_DA", 0x00000000000C);
    if (rv != SHR_E_NONE) return rv;

    /* Commit the entry synchronously */
    rv = bcmlt_custom_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT,
                                   BCMLT_PRIORITY_NORMAL);
    if (rv != SHR_E_NONE) return rv;

    /* Free the entry handle */
    rv = bcmlt_entry_free(entry_hdl);
    if (rv != SHR_E_NONE) return rv;

    /* Create ECMP object 2. */

    rv = bcmlt_entry_allocate(unit, "ECMP", &entry_hdl);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_add(entry_hdl, "ECMP_ID", 2);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_add(entry_hdl, "MAX_PATHS", 2);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_add(entry_hdl, "NUM_PATHS", 2);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_add(entry_hdl, "NHOP_SORTED", 1);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_symbol_add(entry_hdl, "LB_MODE", "REGULAR");
    if (rv != SHR_E_NONE) return rv;

    nhop_id_arr[0] = 5;
    nhop_id_arr[1] = 10;
    rv = bcmlt_entry_field_array_add(entry_hdl, "NHOP_ID", 0,
             nhop_id_arr, 2);
    if (rv != SHR_E_NONE) return rv;

    /* Commit the entry synchronously */
    rv = bcmlt_custom_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT,
                                   BCMLT_PRIORITY_NORMAL);
    if (rv != SHR_E_NONE) return rv;

    /* Free the entry handle */
    rv = bcmlt_entry_free(entry_hdl);
    if (rv != SHR_E_NONE) return rv;

    /* Create LPM route pointing to ECMP object 2. */

    rv = bcmlt_entry_allocate(unit, "L3_IPV4_UC_ROUTE", &entry_hdl);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_add(entry_hdl, "IPV4", 0xC0A80064ULL);
    if (rv != SHR_E_NONE) return rv;

    /* Beware of sign extension!. Always use ULL extension for long values */
    rv = bcmlt_entry_field_add(entry_hdl, "IPV4_MASK", 0xFFFFFF00ULL);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_add(entry_hdl, "ECMP_ID", 2);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_add(entry_hdl, "ECMP_NHOP", 1);
    if (rv != SHR_E_NONE) return rv;

    /* Commit the entry synchronously */
    rv = bcmlt_custom_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT,
                                   BCMLT_PRIORITY_NORMAL);
    if (rv != SHR_E_NONE) return rv;

    /* Free the entry handle */
    rv = bcmlt_entry_free(entry_hdl);
    if (rv != SHR_E_NONE) return rv;

    /* Enable sFlow on port 1 */
    rv = bcmlt_entry_allocate(unit, "MIRROR_PORT_ENCAP_SFLOW", &entry_hdl);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_add(entry_hdl, "PORT_ID", 1);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_add(entry_hdl, "SAMPLE_ING", 1);
    if (rv != SHR_E_NONE) return rv;

    rv = bcmlt_entry_field_add(entry_hdl, "SAMPLE_ING_CPU", 1);
    if (rv != SHR_E_NONE) return rv;

    /* Beware of sign extension!. Always use ULL extension for long values */
    rv = bcmlt_entry_field_add(entry_hdl, "SAMPLE_ING_RATE", 0xFFFFFFFULL);
    if (rv != SHR_E_NONE) return rv;

    /* Commit the entry synchronously */
    rv = bcmlt_custom_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT,
                                   BCMLT_PRIORITY_NORMAL);
    if (rv != SHR_E_NONE) return rv;

    /* Free the entry handle */
    rv = bcmlt_entry_free(entry_hdl);
    if (rv != SHR_E_NONE) return rv;

    bshell(unit, "tx 1 portlist=2 DATA=000000BBBBBB00000000000208004500002E000000004001F7B6C0A80164C0A800640000B7AE00000000000102030405060708090A0B0C0D0E0F101179EF5136");
    printf("Done\n");

}

main(0);


