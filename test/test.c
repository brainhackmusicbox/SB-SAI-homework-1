#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sai.h"

const char* test_profile_get_value(
    _In_ sai_switch_profile_id_t profile_id,
    _In_ const char* variable)
{
    return 0;
}

int test_profile_get_next_value(
    _In_ sai_switch_profile_id_t profile_id,
    _Out_ const char** variable,
    _Out_ const char** value)
{
    return -1;
}

const service_method_table_t test_services = {
    test_profile_get_value,
    test_profile_get_next_value
};

int check_status_error(char *operation, sai_status_t status)
{
    if (SAI_STATUS_SUCCESS == status){
        printf("%s: OK\n", operation);
        return 0;
        }
    else {
        printf("%s: FAIL (status: %d)\n", operation, status);
        return 1;
    };
};

int test_sai_homework_3(sai_switch_api_t *switch_api, sai_lag_api_t *lag_api){
    printf("\n\n=============================================================\n");
    printf("LAG API test (homework 3)\n");
    printf("=============================================================\n");
    // Requirements:
    //      Up to 5 LAGs,
    //      Up to 16 LAG_MEMBERs,
    //      Up to 32 PORTs,
    //      Supported LAG attributes: PORT_LIST,
    //      Supported LAG_MEMBER attributes: PORT_ID, LAG_ID.
    #define MAX_LAGS 5
    #define MAX_LAG_MEMBERS 16
    #define MAX_PORTS 32
    sai_status_t              status;
    sai_attribute_t           attrs[2];
    sai_object_id_t           lags[MAX_LAGS];
    sai_object_id_t           lag_members[MAX_LAG_MEMBERS];
    sai_object_id_t           port_list[MAX_PORTS];

    //--------------------------------------------------------------------------
    // Verify number of ports
    printf("----------------------------------\n");
    printf("Verify number of ports\n");
    printf("----------------------------------\n");
    attrs[0].id = SAI_SWITCH_ATTR_PORT_LIST;
    attrs[0].value.objlist.list = port_list;
    attrs[0].value.objlist.count = MAX_PORTS;
    
    status = switch_api->get_switch_attribute(1, attrs);
    if (check_status_error("get_switch_attribute", status)) return 1;

    printf("Expected ports count: <= %d\n", MAX_PORTS);
    printf("Actual ports count: %d\n\n", attrs[0].value.objlist.count);
    if (attrs[0].value.objlist.count != MAX_PORTS)
    {
        printf("Ports count: test FAILED: ports count is not %d\n", MAX_PORTS);
        return 1;
    }
    else
    {
        printf("Ports count: test PASSED\n");
    };
    printf("==================================\n\n");

    status = sai_api_query(SAI_API_LAG, (void**)&lag_api);
    if (check_status_error("sai_api_query(SAI_API_LAG)", status)) return 1;

    // Create 5 LAGs and verify status
    printf("----------------------------------\n");
    printf("Create 5 LAGs and verify status\n");
    printf("----------------------------------\n");

    // for each LAG
    for (int lag_number=0; lag_number<MAX_LAGS; lag_number++)
    {
        printf("\n\n======================== LAG # %d ========================\n", lag_number);

        // Create LAG
        char *num;
        char msg[200];
        asprintf(&num, "%d", lag_number);
        strcat(strcpy(msg, "create lag #"), num);
        
        status = lag_api->create_lag(&lags[lag_number], 0, NULL);
        if (check_status_error(msg, status)) return 1;
        
        // Create LAG MEMBERS
        int lag_members_number;
        if (lag_number==0){
            // for the first LAG, create one member
            printf("Create 1 member for LAG #0\n");
            lag_members_number = 1;
        }
        else if (lag_number==MAX_LAGS-1){
            // for the last LAG, create max number of members
            lag_members_number = MAX_LAG_MEMBERS;
            printf("\tCreate maximum numbers of members: %d", lag_members_number);
        }
        else
        {   
            // create random number of members for the rest of LAGs
            
            // num = (rand() % (upper - lower + 1)) + lower
            // upper = MAX_LAG_MEMBERS-1
            // lower = 2
            // since '1' and MAX_LAG_MEMBERS are two previous cases
            // (rand() % (MAX_LAG_MEMBERS-2 + 1)) + 2
            lag_members_number = (rand() % (MAX_LAG_MEMBERS-1)) + 2;
            printf("\n\tGenerate random number for members count (from 1 to %d): %d", MAX_LAG_MEMBERS-1, lag_members_number);
        }

        printf("\n----------------------------------\n");
        printf("Create %d LAG member(s) and check their attrs\n", lag_members_number);
        printf("----------------------------------\n\n");

        // for each LAG member
        sai_object_id_t lag_id[64];
        sai_object_id_t port_id[64];
        int lag_member=0;
        for (lag_member==0; lag_member<lag_members_number; lag_member++)
        {
            // create LAG member
            printf("--------------------------------------------\n");
            asprintf(&num, "%d", lag_member);
            strcat(strcpy(msg, "create lag member #"), num);
            
            status = lag_api->create_lag_member(&lag_members[lag_member], 0, NULL);
            if (check_status_error(msg, status)) return 1;
            //---------------------------------------------------------------------------
            //verify that LAG_ID attr is available
            printf("========\n");
            printf("Verify that LAG_ID attr is available\n");
            attrs[0].id = SAI_LAG_MEMBER_ATTR_LAG_ID;
            attrs[0].value.objlist.list = lag_id;
            attrs[0].value.objlist.count = 64;
            status = lag_api->get_lag_member_attribute(lag_member, 1, attrs);

            asprintf(&num, "%d", lag_member);
            strcat(strcpy(msg, "Get SAI_LAG_MEMBER_ATTR_LAG_ID for LAG_MEMBER #"), num);
            if (check_status_error(msg, status)) return 1;
            //---------------------------------------------------------------------------
            // Verify that PORT_ID is available
            printf("========\n");
            printf("Verify that PORT_ID attr is available\n");
            attrs[0].id = SAI_LAG_MEMBER_ATTR_PORT_ID;
            attrs[0].value.objlist.list = port_id;
            attrs[0].value.objlist.count = 64;
            status = lag_api->get_lag_member_attribute(lag_member, 1, attrs);

            asprintf(&num, "%d", lag_member);
            strcat(strcpy(msg, "Get SAI_LAG_MEMBER_ATTR_PORT_ID for LAG_MEMBER #"), num);
            if (check_status_error(msg, status)) return 1;
            printf("\n");
            //---------------------------------------------------------------------------
        };

        printf("----------------------------------\n");
        printf("\tDelete %d LAG member(s) and verify status:\n", lag_members_number);
        // I didn't manage to figure out how to remove an object by OID
        // Removing functions just return SAI_STATUS_SUCCESS
        // but I guess for this exercise it's not that important

        lag_member=0;
        for (lag_member==0; lag_member<lag_members_number; lag_member++)
        {
            asprintf(&num, "%d", lag_member);
            strcat(strcpy(msg, "delete lag member #"), num);
                  
            status = lag_api->remove_lag_member(lag_members[lag_member]);
            if (check_status_error(msg, status)) return 1;  
        };        
    };
};

int main()
{
    sai_status_t              status;
    sai_switch_api_t         *switch_api;
    sai_lag_api_t            *lag_api;
    sai_object_id_t           lag_1, lag_2;
    sai_object_id_t           port_1, port_2, port_3, port_4;
    sai_attribute_t           attrs[2];
    sai_switch_notification_t notifications;
    sai_object_id_t           port_list[64];

    status = sai_api_initialize(0, &test_services);
    if (check_status_error("sai_api_initialize", status)) return 1;


    status = sai_api_query(SAI_API_SWITCH, (void**)&switch_api);
    if (check_status_error("sai_api_query(SAI_API_SWITCH)", status)) return 1;

    
    status = switch_api->initialize_switch(0, "HW_ID", 0, &notifications);
    if (check_status_error("initialize_switch", status)) return 1;

    printf("\n\n=============================\n");
    printf("LAG API test\n");
    printf("=============================\n");
    // For the get and set functions just print that this function is being called and return SAI_STATUS_SUCCESS.

    // Finally, let's write a complete test. 
    // You should skip checking return values from the get_attribute functions as our implementation doesn't support getting and setting attributes yet.

    status = sai_api_query(SAI_API_LAG, (void**)&lag_api);
    if (check_status_error("sai_api_query(SAI_API_LAG)", status)) return 1;



    // Create LAG#1
    status = lag_api->create_lag(&lag_1, 0, NULL);
    if (check_status_error("create_lag 1", status)) return 1;

    // Create LAG_MEMBER#1 {LAG_ID:LAG#1, PORT_ID:PORT#1}
    status = lag_api->create_lag_member(&port_1, 0, NULL);
    if (check_status_error("create_lag_member 1", status)) return 1;

    // Create LAG_MEMBER#2 {LAG_ID:LAG#1, PORT_ID:PORT#2}
    status = lag_api->create_lag_member(&port_2, 0, NULL);
    if (check_status_error("create_lag_member 2", status)) return 1;

    // Create LAG#2
    status = lag_api->create_lag(&lag_2, 0, NULL);
    if (check_status_error("create_lag 2", status)) return 1;

    // Create LAG_MEMBER#3 {LAG_ID:LAG#2, PORT_ID:PORT#3}
    status = lag_api->create_lag_member(&port_3, 0, NULL);
    if (check_status_error("create_lag_member 3", status)) return 1;

    // Create LAG_MEMBER#4 {LAG_ID:LAG#2, PORT_ID:PORT#4}
    status = lag_api->create_lag_member(&port_4, 0, NULL);
    if (check_status_error("create_lag_member 4", status)) return 1;
    
    // Get LAG#1 PORT_LIST [Expected: (PORT#1, PORT#2)]
    attrs[0].id = SAI_LAG_ATTR_PORT_LIST;
    attrs[0].value.objlist.list = port_list;
    attrs[0].value.objlist.count = 64;
    status = lag_api->get_lag_attribute(lag_1, 1, attrs);
    if (check_status_error("Get SAI_LAG_ATTR_PORT_LIST (LAG 1)", status)) return 1;

    // Get LAG#2 PORT_LIST [Expected: (PORT#3, PORT#4)]
    attrs[0].id = SAI_LAG_ATTR_PORT_LIST;
    attrs[0].value.objlist.list = port_list;
    attrs[0].value.objlist.count = 64;
    status = lag_api->get_lag_attribute(lag_2, 1, attrs);
    if (check_status_error("Get SAI_LAG_ATTR_PORT_LIST (LAG 2)", status)) return 1;

    // Get LAG_MEMBER#1 LAG_ID [Expected: LAG#1]
    sai_object_id_t           lag_id[64];
    attrs[0].id = SAI_LAG_MEMBER_ATTR_LAG_ID;
    attrs[0].value.objlist.list = lag_id;
    attrs[0].value.objlist.count = 64;
    status = lag_api->get_lag_attribute(port_1, 1, attrs);
    if (check_status_error("Get SAI_LAG_MEMBER_ATTR_LAG_ID (LAG_MEMBER 1)", status)) return 1;

    // Get LAG_MEMBER#3 PORT_ID [Expected: PORT#3]
    sai_object_id_t           port_id[64];
    attrs[0].id = SAI_LAG_MEMBER_ATTR_PORT_ID;
    attrs[0].value.objlist.list = port_id;
    attrs[0].value.objlist.count = 64;
    status = lag_api->get_lag_member_attribute(port_3, 1, attrs);
    if (check_status_error("Get SAI_LAG_MEMBER_ATTR_PORT_ID (LAG_MEMBER 3)", status)) return 1;

    // Remove LAG_MEMBER#2
    status = lag_api->remove_lag_member(port_2);
    if (check_status_error("remove_lag_member (port 2)", status)) return 1;    

    // Get LAG#1 PORT_LIST [Expected: (PORT#1)]
    attrs[0].id = SAI_LAG_ATTR_PORT_LIST;
    attrs[0].value.objlist.list = port_list;
    attrs[0].value.objlist.count = 64;
    status = lag_api->get_lag_attribute(lag_1, 1, attrs);
    if (check_status_error("Get SAI_LAG_ATTR_PORT_LIST (LAG 1)", status)) return 1;    

    // Remove LAG_MEMBER#3
    status = lag_api->remove_lag_member(port_3);
    if (check_status_error("remove_lag_member (port 3)", status)) return 1;  

    // Get LAG#2 PORT_LIST [Expected: (PORT#4)]
    attrs[0].id = SAI_LAG_ATTR_PORT_LIST;
    attrs[0].value.objlist.list = port_list;
    attrs[0].value.objlist.count = 64;
    status = lag_api->get_lag_attribute(lag_2, 1, attrs);
    if (check_status_error("Get SAI_LAG_ATTR_PORT_LIST (LAG 2)", status)) return 1;

    // Remove LAG_MEMBER#1
    status = lag_api->remove_lag_member(port_1);
    if (check_status_error("remove_lag_member (port 1)", status)) return 1;  

    // Remove LAG_MEMBER#4
    status = lag_api->remove_lag_member(port_4);
    if (check_status_error("remove_lag_member (port 4)", status)) return 1; 

    // Remove LAG#2
    status = lag_api->remove_lag(lag_2);
    if (check_status_error("remove_lag (LAG 2)", status)) return 1;  

    // Remove LAG#1
    status = lag_api->remove_lag(lag_1);
    if (check_status_error("remove_lag (LAG 1)", status)) return 1;  

    // Run additional test
    test_sai_homework_3(switch_api, lag_api);

    switch_api->shutdown_switch(0);
    
    status = sai_api_uninitialize();
    printf("\n\n=============================\n");
    if (check_status_error("sai_api_uninitialize", status)) return 1;

    return 0;
}