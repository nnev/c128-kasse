#ifndef __BINLOG_H_
#define __BINLOG_H_

#include <stdbool.h>
#include <stdint.h>

/*
 * User anlegen
 * Item anlegen
 * Item unsichtbar machen (Preisänderung)
 * Guthaben einzahlen ✔ (credit)
 * Guthaben auszahlen ✔ (credit)
 * Verkauf (sale)
 *
 */

#define NAMELEN 16

typedef uint8_t user_id_t;
typedef uint8_t item_id_t;
typedef int16_t money_t;

struct user {
    user_id_t id;
    bool visible;
    money_t balance;
    char name[NAMELEN];
};

struct item {
    item_id_t id;
    bool visible;
    money_t cent;
    char name[NAMELEN];
};

enum entry_type {
  SALE = 1,
  CREDIT = 2,
  EMPTY = 255
};

struct binlog_entry {
    enum entry_type tag; //1
    uint32_t ts; //4
    union { //4
        struct sale { //3
            user_id_t user;
            item_id_t itemid;
            int8_t count_item;
        } sale;

        struct credit { //3
            user_id_t user;
            money_t amount_cent;
        } credit;
    };
    uint16_t crc; //2
};

user_id_t bl_user_create(const char *name);
bool bl_user_credit(const user_id_t id, money_t amount);
uint8_t bl_user_count(void);
bool bl_user_load(user_id_t id, struct user *user);

item_id_t bl_item_create(const char *name, money_t cent);
bool bl_item_hide(const item_id_t id);
bool bl_sell(const user_id_t user, const item_id_t item, const int8_t count);

#endif /* __BINLOG_H_ */
