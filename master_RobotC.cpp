/*
Project Name : 	NXTChessAssistant
Authors      : 	Yasir Al Shekerchi, David Perna, Pranav Sharma, Felicity Chan
Course(s)    : 	MTE100, GENE121
Date Created : 	November 17, 2015
Date Modified:	November 31, 2015
Description  : 	The code below contains all the necessary information to run 
                the NXTChessAssistant program on the Lego NXT Platform.
                The NXTRelocatorBot was a project with a team of four to 
                design and built a mechanical system which assists physically 
                disabled people play chess by moving the chess pieces. 
                The robot's mechanical system included a magnet which moved 
                magnetic chess pieced across the board according to user 
                instructions. The movement and logical algorithms were written 
                using RobotC in order to program the NXT MindStrom microprocessor 
                which controls various motors, as well as, appropriately 
                utilizes sensor feedback to accurately track position.
Specs        :  NXT RGB Colour Sensor in Sensor 1 Port
                NXT Touch Sensor in Sensor 2 Port
                X-Direction Motor in Motor A Port
                Y-Direction Motor in Motor B Port
                Z-Direction Motor in Motor C Port
*/

//Constants for encoder values to move the chess pieces
const float ENCODEX = 180/(4.5*PI);
const float ENCODEY = 160; //175

//Distance in cm for a square
const float PERSQ = 5.409;

//Degrees required to move magnet up
const float MAGUP = 90;

//Motor power level constants
const float POWERX = 20;
const float POWERY = 20;
const float POWERZ = 10;

//Stores the initial setup chess piece locations into an array(64) stores in 0-15, 48-63
void initialSetup(bool*chessboard)
{
    for(int i = 0; i < 64; i++)
    {
       chessboard[i] = false;
    }

    for (int w = 0; w < 16; w++)
    {
        chessboard[w] = true;
    }
    for (int b = 63; b > 47; b--)
    {
        chessboard[b] = true;
    }
}

//Returns an integer corresponding to the One-Dimensional location in a 2D Array
int toOneDimArray(int x, int y)
{
    int index = y * 8 + x;
    return index;
}

//Scans for user card colour and starts timer
void scanCard()
{
    displayString(0, "Scan your card");
    while (SensorValue[S1] != GREENCOLOR && SensorValue[S1] != REDCOLOR && SensorValue[S1] != YELLOWCOLOR )
    {}
		time1[T1]=0;
		if (SensorValue[S1] != GREENCOLOR && SensorValue[S1] != REDCOLOR)
			return true;
		else
			return false;
}

//Stops timer and displays elapsed time
void stopTime(long & timeTotal)
{
    long decisionTime = time1[T1];
    timeTotal += decisionTime;
    displayString(6, "Time Elapsed:");
    displayString(7, "%.2f seconds: ", timeTotal/1000.0);
    wait1Msec(2000);
    eraseDisplay();
}

//Prompts user for Row
int getRow()
{
    char data[8] = {'H','G','F','E','D','C','B','A'};
    int index = 7;
    displayString(1, "Select the row");

    while (nNxtButtonPressed != 3)
    {
        displayString(2, "%c", data[index]);
        while (nNxtButtonPressed == -1)
        {}
        if (nNxtButtonPressed == 1)
        {
            if (index > 0)
                index--;
            else
                index = 7;
            displayString(2, "%c" , data[index]);
        }
        else if (nNxtButtonPressed == 2)
        {
            if (index < 7)
                index++;
            else
                index = 0;
            displayString(2, "%c" , data[index]);
        }
        while (nNxtButtonPressed != -1 && nNxtButtonPressed != 3)
        {}
    }
    while (nNxtButtonPressed == 3)
    {}
    return index;
}

//Prompts user for columns
int getColumn()
{
    int data[8] = {1,2,3,4,5,6,7,8};
    int index = 0;
    displayString(3, "Select the column" );

    while (nNxtButtonPressed != 3)
    {
        displayString(4, "%d", data[index]);
        while (nNxtButtonPressed == -1)
        {}
        if (nNxtButtonPressed == 2)
        {
            if (index > 0)
                index--;
            else
                index = 7;
            displayString(4, "%d" , data[index]);
        }
        else if (nNxtButtonPressed == 1)
        {
            if (index < 7)
                index++;
            else
                index = 0;
            displayString(4, "%d" , data[index]);
        }
        while (nNxtButtonPressed != -1 && nNxtButtonPressed != 3)
        {}
    }
    while (nNxtButtonPressed == 3)
    {}
    return index;
}

// Function that determines if there is a piece in the destined location, prompts the user to remove the piece
void removePiece(bool * chessboard, int rowF, int colF)
{
    char row[8] = {'H', 'G', 'F', 'E', 'D', 'C', 'B', 'A'};
    if (chessboard[toOneDimArray(rowF, colF)])
    {
        displayString (0, "Remove piece at");
        displayString (1, "%c%d", row[rowF], colF + 1);
        displayString (3, "Please press the");
        displayString (4, "centre button");
        displayString (5, "once completed");
        while(nNxtButtonPressed != 3) {}
        eraseDisplay();
    }
    chessboard[toOneDimArray(rowF, colF)] = false;
}


// Drives robot backward until touch sensor hits back base
void returnToX()
{
    // moves back on x until touch sensor hits back base
    while(SensorValue[S2] == 0)
        motor[motorA] = -POWERX;
    motor[motorA] = 0;
    nMotorEncoder[motorA] = 0;
}

// Drives robot back to the reference column
void returnToY(float distY)
{
    if (distY<0)
        while (nMotorEncoder[motorB] !=0)
            motor[motorB] = POWERY;
    else
        while (nMotorEncoder[motorB] != 0)
            motor[motorB] = -POWERY;

    motor[motorB]=0;
}

//Moves the robot along the X axis
void moveX(float init_x, float & distX)
{
    nMotorEncoder[motorA] = 0;
    distX = PERSQ*ENCODEX*(init_x);

    while (nMotorEncoder[motorA]<distX)
        motor[motorA] = POWERX;
    motor[motorA] = 0;
}

//Moves the robot along the Y axis
void moveY(float init_y, float & distY)
{
    nMotorEncoder[motorB] = 0;
    distY = (ENCODEY*(5-init_y));
    while (abs(nMotorEncoder[motorB]) < abs(distY) + ENCODEY / 2)
        if (distY<0)
            motor[motorB] = -POWERY;
        else
            motor[motorB] = POWERY;

    motor[motorB] = 0;
}

//Raises robot arm up
void raiseZ()
{
    // grab piece with arm
    nMotorEncoder[motorC] = 0;
    while(nMotorEncoder[motorC] > -MAGUP)
        motor[motorC] = -POWERZ;
    motor[motorC] = 0;
}

//Lowers robot arm down
void lowerZ()
{
    // release piece with arm
    while(nMotorEncoder[motorC] < 0)
        motor[motorC] = POWERZ;
    motor[motorC] = 0;
}

void movePiece(float init_x, float init_y, float final_x, float final_y)
{
    float distX = 0;
    float distY = 0;

    moveX(init_x, distX);
    moveY(init_y, distY);
    raiseZ();

    //Move half a row forward
    while(nMotorEncoder[motorA] < distX+ENCODEX*PERSQ/2+30)
        motor[motorA] = POWERX;
    motor[motorA] = 0;

    returnToY(distY);
    returnToX();

    moveX (final_x, distX);

    //Move half a row forward
    while(nMotorEncoder[motorA] < distX+ENCODEX*PERSQ/2)
        motor[motorA] = POWERX;
    motor[motorA] = 0;

    moveY (final_y, distY);

    //Move half a row backward
    while(nMotorEncoder[motorA] > distX-10) 
        motor[motorA] = -POWERX;
    motor[motorA] = 0;

    lowerZ();
    returnToY(distY);
    returnToX();
}

task main ()
{
    //Declare distX, distY
    int init_x=0, init_y=0, final_x=0, final_y=0;
    bool chessboard[64];
    long timeTotal = 0;
    
    //Initialize Sensor Types
    SensorType[S1] = sensorColorNxtFULL;
    SensorType[S2] = sensorTouch;

    //Initialize the chessboard array
    initialSetup(chessboard);
    
    while(scanCard()){
        //Prompt user for initial and final location
        displayString(0, "INITIAL LOCATION");
        init_x = getRow();
        init_y = getColumn();
        eraseDisplay();

        displayString(0, "FINAL LOCATION");
        final_x = getRow(); 
        final_y = getColumn();
        eraseDisplay();

        stopTime(timeTotal);
        removePiece(chessboard, final_x, final_y);
        movePiece(init_x+0.5, init_y, final_x+1.7, final_y-1);
        chessboard[toOneDimArray(final_x, final_y)] = true;
    }
}
