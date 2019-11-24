#ifndef __TAPDEV_H__
#define __TAPDEV_H__

void tapdev_init(void);
unsigned short int tapdev_read(void);
void tapdev_send(void);

#endif
