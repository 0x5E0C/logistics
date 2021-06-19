#ifndef PUBLIC_H
#define PUBLIC_H

#define OPENSTATE   0
#define CLOSESTATE  1

#define ADV     true
#define STOP    false

#define BASETIME    500
#define SENDTIME    100

#define AVOID_CMD  	0x01
#define ADV_CMD     0x02
#define TASK_CMD  	0x03
#define REPLY_CMD 	0x04
#define SENDPOS_CMD	0x05
#define STOP_CMD    0x06

#define PACKET_LENGTH  10

#define X_DIR       0
#define Y_DIR       1

const int safe_distance=120;    //安全距离
const double avoid_coe=1.2;     //避让距离系数（安全距离的倍数）

#endif // PUBLIC_H
