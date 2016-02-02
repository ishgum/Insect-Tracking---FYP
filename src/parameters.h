
#ifndef PARAMETERS_H_
#define PARAMETERS_H_

// Connection parameters
#define UAV_UART_CONNECT_ACM0 "/dev/ttyACM0"
#define UAV_UART_CONNECT_ACM1 "/dev/ttyACM1"
#define UAV_UART_CONNECT_USB0 "/dev/ttyUSB0"
#define UAV_UART_CONNECT_USB1 "/dev/ttyUSB1"

#define UAV_UART_CONNECT_DEFAULT "/dev/ttyACM0"
#define UAV_BAUDRATE 57600


// Layout parameters

#define DATA_WIDTH 20

#define INSECT_DATA_Y 5
#define UAV_DATA_Y 15
#define CAMERA_DATA_Y 23
#define FLAG_DATA_Y 30

#define INSECT_DATA_X 2
#define UAV_DATA_X 9
#define CAMERA_DATA_X 6
#define FLAG_DATA_X 6

#define DATA_COLUMN 27

#define OUTPUT_STREAM_HEIGHT 32
#define OUTPUT_STREAM_WIDTH 39
#define OUTPUT_STREAM_Y 7
#define OUTPUT_STREAM_X 49


//Conversion between pixels and metres. Should react at 1 m/s.
#define SPEED_SCALE 500		

#endif //PARAMETERS_H_
