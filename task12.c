#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_OBJECTS 30
#define MAX_DOMAINS 30
#define MAX_RIGHTS 3

const char RIGHTS[] = {'r', 'w', 'x'};

typedef struct {
    int domain_id;
    char access_rights[MAX_RIGHTS + 1];
} ACL;

typedef struct {
    int object_id;
    ACL acl_list[MAX_DOMAINS];
    int acl_count;
} ObjectACL;

ObjectACL objects[MAX_OBJECTS];
int domains[MAX_DOMAINS];
int object_count = 0, domain_count = 0;

void create_object(int object_id) {
    if (object_count >= MAX_OBJECTS) {
        printf("Max number of objects reached\n");
        return;
    }

    objects[object_count].object_id = object_id;
    objects[object_count].acl_count = 0;
    object_count++;
    printf("Object %d created\n", object_id);
}

void delete_object(int object_id) {
    for (int i = 0; i < object_count; i++) {
        if (objects[i].object_id == object_id) {
            for (int j = i; j < object_count - 1; j++) {
                objects[j] = objects[j + 1];
            }
            object_count--;
            printf("Object %d deleted\n", object_id);
            return;
        }
    }
    printf("Object %d not found\n", object_id);
}

void create_domain(int domain_id) {
    if (domain_count >= MAX_DOMAINS) {
        printf("Max number of domains reached\n");
        return;
    }

    domains[domain_count++] = domain_id;
    printf("Domain %d created\n", domain_id);
}

void delete_domain(int domain_id) {
    for (int i = 0; i < domain_count; i++) {
        if (domains[i] == domain_id) {
            for (int j = i; j < domain_count - 1; j++) {
                domains[j] = domains[j + 1];
            }
            domain_count--;
            printf("Domain %d deleted\n", domain_id);
            return;
        }
    }
    printf("Domain %d not found\n", domain_id);
}

void grant_access(int object_id, int domain_id, const char *rights) {
    for (int i = 0; i < object_count; i++) {
        if (objects[i].object_id == object_id) {
            for (int j = 0; j < objects[i].acl_count; j++) {
                if (objects[i].acl_list[j].domain_id == domain_id) {
                    strncpy(objects[i].acl_list[j].access_rights, rights, MAX_RIGHTS);
                    printf("Rights changed for domain %d on object %d\n", domain_id, object_id);
                    return;
                }
            }

            if (objects[i].acl_count < MAX_DOMAINS) {
                objects[i].acl_list[objects[i].acl_count].domain_id = domain_id;
                strncpy(objects[i].acl_list[objects[i].acl_count].access_rights, rights, MAX_RIGHTS);
                objects[i].acl_count++;
                printf("Rights given to domain %d, object: %d\n", domain_id, object_id);
            } else {
                printf("ACL list full, object: %d\n", object_id);
            }
            return;
        }
    }
    printf("Object %d not found\n", object_id);
}

void revoke_access(int object_id, int domain_id) {
    for (int i = 0; i < object_count; i++) {
        if (objects[i].object_id == object_id) {
            for (int j = 0; j < objects[i].acl_count; j++) {
                if (objects[i].acl_list[j].domain_id == domain_id) {
                    for (int k = j; k < objects[i].acl_count - 1; k++) {
                        objects[i].acl_list[k] = objects[i].acl_list[k + 1];
                    }
                    objects[i].acl_count--;
                    printf("Rights revoked for domain %d, object: %d\n", domain_id, object_id);
                    return;
                }
            }
            printf("Domain %d not found in ACL, object: %d\n", domain_id, object_id);
            return;
        }
    }
    printf("Object %d not found\n", object_id);
}

int main() {
    create_domain(1);
    create_object(101);
    grant_access(101, 1, "rw");
    revoke_access(101, 1);
    delete_object(101);
    delete_domain(1);

    return 0;
}

