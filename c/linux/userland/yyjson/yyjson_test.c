#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "yyjson.h"

int parse_json()
{
    int ret = 0;

    const char *json_str = "{ \"Header\":{ \"Version\":1, \"User\":\"auser\", \"Announce\":\"293717623\", \"SessionID\":\"ee2a82f68416c38feb4e1971c8b35a47350bf\", \"RequestType\":\"UserManager\" }, \"Body\": { \"Processor\": \"AddUser\", \"RequestMessage\": { \"UserInfo\": { \"name\": \"xxxxxx\", \"available\": \"true\", \"authcode\": \"xxxxxx\", \"upks\": [\"upk1\", \"upk2\", \"upk3\"], \"storemode\": \"xxxxxx\", \"doublekeymode\": \"xxxxxx\", \"createtime\": \"xxxxxx\", \"expiretime\": \"xxxxxx\", \"keycountlimit\": 111, \"checkweekalg\": \"xxxxxx\", \"tokenlife\": 111, \"maxtoken\": 111, \"ontokenover\": \"xxxxxx\", \"cryptopow\": { \"hsm\": 11, \"cpu\": 11, \"remote\": 11 } } } } }";

    yyjson_doc *doc = yyjson_read(json_str, strlen(json_str), 0);

    yyjson_val *root = yyjson_doc_get_root(doc);

    yyjson_val *header = yyjson_obj_get(root, "Header");
    yyjson_val *body = yyjson_obj_get(root, "Body");

    yyjson_val *version = yyjson_obj_get(header, "Version");
    yyjson_val *user = yyjson_obj_get(header, "User");
    yyjson_val *announce = yyjson_obj_get(header, "Announce");
    yyjson_val *session_id = yyjson_obj_get(header, "SessionID");
    yyjson_val *request_type = yyjson_obj_get(header, "RequestType");

    printf("version: %d\n", yyjson_get_int(version));
    printf("user: %s\n", yyjson_get_str(user));
    printf("announce: %s\n", yyjson_get_str(announce));
    printf("session_id: %s\n", yyjson_get_str(session_id));
    printf("request_type: %s\n", yyjson_get_str(request_type));

    yyjson_val *processor = yyjson_obj_get(body, "Processor");
    printf("processor: %s\n", yyjson_get_str(processor));
    yyjson_val *request_message = yyjson_obj_get(body, "RequestMessage");
    yyjson_val *user_info = yyjson_obj_get(request_message, "UserInfo");
    yyjson_val *name = yyjson_obj_get(user_info, "name");
    yyjson_val *available = yyjson_obj_get(user_info, "available");
    yyjson_val *authcode = yyjson_obj_get(user_info, "authcode");
    printf("name: %s\n", yyjson_get_str(name));
    printf("available: %s\n", yyjson_get_str(available));
    printf("authcode: %s\n", yyjson_get_str(authcode));
    yyjson_val *upks = yyjson_obj_get(user_info, "upks");
    size_t upks_size = yyjson_arr_size(upks);
    int i = 0;
    for (i = 0; i < upks_size; i++) {
        yyjson_val *upk = yyjson_arr_get(upks, i);
        printf("upk: %s\n", yyjson_get_str(upk));
    }
    yyjson_val *storemode = yyjson_obj_get(user_info, "storemode");
    yyjson_val *doublekeymode = yyjson_obj_get(user_info, "doublekeymode");
    yyjson_val *createtime = yyjson_obj_get(user_info, "createtime");
    yyjson_val *expiretime = yyjson_obj_get(user_info, "expiretime");
    yyjson_val *keycountlimit = yyjson_obj_get(user_info, "keycountlimit");
    yyjson_val *checkweekalg = yyjson_obj_get(user_info, "checkweekalg");
    yyjson_val *tokenlife = yyjson_obj_get(user_info, "tokenlife");
    yyjson_val *maxtoken = yyjson_obj_get(user_info, "maxtoken");
    yyjson_val *ontokenover = yyjson_obj_get(user_info, "ontokenover");
    yyjson_val *cryptopow = yyjson_obj_get(user_info, "cryptopow");
    yyjson_val *hsm = yyjson_obj_get(cryptopow, "hsm");
    yyjson_val *cpu = yyjson_obj_get(cryptopow, "cpu");
    yyjson_val *remote = yyjson_obj_get(cryptopow, "remote");
    printf("storemode: %s\n", yyjson_get_str(storemode));
    printf("doublekeymode: %s\n", yyjson_get_str(doublekeymode));
    printf("createtime: %s\n", yyjson_get_str(createtime));
    printf("expiretime: %s\n", yyjson_get_str(expiretime));
    printf("keycountlimit: %d\n", yyjson_get_int(keycountlimit));
    printf("checkweekalg: %s\n", yyjson_get_str(checkweekalg));
    printf("tokenlife: %d\n", yyjson_get_int(tokenlife));
    printf("maxtoken: %d\n", yyjson_get_int(maxtoken));
    printf("ontokenover: %s\n", yyjson_get_str(ontokenover));
    printf("hsm: %d\n", yyjson_get_int(hsm));
    printf("cpu: %d\n", yyjson_get_int(cpu));
    printf("remote: %d\n", yyjson_get_int(remote));

    yyjson_doc_free(doc);

    return ret;
}


int construct_json()
{
    int ret = 0;

/*
{
    "Header": {
        "Version": 1,
        "User": "auser",
        "Announce": "293717623",
        "SessionID": "",
        "RequestType": "UserManager"
    },
    "Body": {
        "Processor": "QueryUserList",
        "ProcessCode": 0,
        "ProcessMessage": "OK",
        "ResponseMessage": {
            "UserNames": [
                "xxxxxx1",
                "xxxxxx2"
            ]
        }
    }
}
*/

    yyjson_mut_doc *doc = NULL;
    yyjson_mut_val *root = NULL;
    yyjson_mut_val *header = NULL;
    yyjson_mut_val *body = NULL;

    doc = yyjson_mut_doc_new(NULL);
    root = yyjson_mut_obj(doc);

    header = yyjson_mut_obj(doc);
    body = yyjson_mut_obj(doc);

    yyjson_mut_obj_add_str(doc, header, "Version", "1");
    yyjson_mut_obj_add_str(doc, header, "User", "auser");
    yyjson_mut_obj_add_str(doc, header, "Announce", "293717623");
    yyjson_mut_obj_add_str(doc, header, "SessionID", "");
    yyjson_mut_obj_add_str(doc, header, "RequestType", "UserManager");
    yyjson_mut_obj_add_val(doc, root, "Header", header);

    yyjson_mut_obj_add_str(doc, body, "Processor", "QueryUserList");
    yyjson_mut_obj_add_int(doc, body, "ProcessCode", 0);
    yyjson_mut_obj_add_str(doc, body, "ProcessMessage", "OK");
    yyjson_mut_val *response_message = yyjson_mut_obj(doc);
    yyjson_mut_val *user_names = yyjson_mut_arr(doc);
    yyjson_mut_arr_add_str(doc, user_names, "xxxxxx1");
    yyjson_mut_arr_add_str(doc, user_names, "xxxxxx2");
    yyjson_mut_obj_add_val(doc, response_message, "UserNames", user_names);
    yyjson_mut_obj_add_val(doc, body, "ResponseMessage", response_message);
    yyjson_mut_obj_add_val(doc, root, "Body", body);

    yyjson_mut_doc_set_root(doc, root);


    char *json_str = yyjson_mut_write(doc, 0, NULL);
    printf("json_str: %s\n", json_str);

    // 注意要free内存
    free(json_str);

    yyjson_mut_doc_free(doc);

    return ret;
}


int main(int argc, char *argvp[])
{
    int ret = 0;

    parse_json();

    construct_json();

    return ret;
}