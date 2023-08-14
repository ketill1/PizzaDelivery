#define RIGHT_MOTOR_BACKWARD 5
#define LEFT_MOTOR_BACKWARD  6
#define RIGHT_MOTOR_FORWARD  9
#define LEFT_MOTOR_FORWARD   10
#define RIGHT_MOTOR_ENABLE 12
#define LEFT_MOTOR_ENABLE 13

void initMotorController();
void setMotorSpeed(int i, int spd);
void setMotorSpeeds(int leftSpeed, int rightSpeed);
