#ifndef CREDIT_MANAGER_H
#define CREDIT_MANAGER_H

struct credits_t *find_credit(char *name);
void deposit_credit(char *input);
void credit_manager(void);

#endif /* CREDIT_MANAGER_H */
