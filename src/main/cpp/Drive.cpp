

#include "frc\Joystick.h"
#include "rev/CANSparkMax.h"
#include "frc/VictorSP.h"
#include "frc/DoubleSolenoid.h"
#include "frc/Encoder.h"
#include "Robot.h"
#include "Drive.h"
#include "Log.h"
#include <frc/ADXRS450_Gyro.h>



using namespace std;

double leftdriveold;
double rightdriveold;

Drive::Drive() : Subsystem("Drive") {
    // Define CAN and PWM Ids used in Drive here

    int leftdriveID = 3;
    int leftdriveID2 = 4;
    int rightdriveID = 5;
    int rightdriveID2 = 6;
    int driveshiftIDa = 4;
    int driveshiftIDb = 5;
    int buddyclimbID = 12;
    int buddyclimbpIDa = 4;
    int buddyclimbpIDb = 5;
    int leftclimbID = 7;
    int rightclimbID = 8;
    int leftdriveencID_a = 4;
    int leftdriveencID_b = 5;
    int rightdriveencID_a = 0;
    int rightdriveencID_b = 1;    
    int climberlockIDa = 6;
    int climberlockIDb = 7;


    // Define motors, sensors, and pneumatics here
    m_leftdrive = new rev::CANSparkMax{leftdriveID, rev::CANSparkMax::MotorType::kBrushless}; //actually 1
    m_leftdrive2 = new rev::CANSparkMax{leftdriveID2, rev::CANSparkMax::MotorType::kBrushless};
    m_leftdrive->SetInverted(true);
    m_leftdrive2->SetInverted(true);
    m_rightdrive = new rev::CANSparkMax{rightdriveID, rev::CANSparkMax::MotorType::kBrushless};
    m_rightdrive2 = new rev::CANSparkMax{rightdriveID2, rev::CANSparkMax::MotorType::kBrushless};

    s_gyro = new frc::ADXRS450_Gyro();

    m_leftdrive2->SetInverted(true);
    s_leftdrive_enc = new frc::Encoder( leftdriveencID_a, leftdriveencID_b, false, frc::Encoder::k4X);
    s_rightdrive_enc = new frc::Encoder( rightdriveencID_a, rightdriveencID_b, true, frc::Encoder::k4X);
    p_driveshift = new frc::DoubleSolenoid(2, driveshiftIDa, driveshiftIDb);
    m_buddyclimb = new rev::CANSparkMax{buddyclimbID, rev::CANSparkMax::MotorType::kBrushless};
    m_buddyclimb -> SetInverted(true);
    p_buddyclimb = new frc::DoubleSolenoid(1, buddyclimbpIDa, buddyclimbpIDb);
    m_leftclimb = new rev::CANSparkMax{leftclimbID, rev::CANSparkMax::MotorType::kBrushless};
    m_rightclimb = new rev::CANSparkMax{rightclimbID, rev::CANSparkMax::MotorType::kBrushless};
    m_leftclimb->SetInverted(true);
    s_leftclimb_enc = new  rev::CANEncoder(*m_leftclimb);
    s_rightclimb_enc = new  rev::CANEncoder(*m_rightclimb);

    p_climberlock = new frc::DoubleSolenoid(1, climberlockIDa, climberlockIDb);

    std::vector< double > arr;
}

/* DEADBAND FUNCTION */
/* use to create a deadband on the controls, passing the input and the deadband size */
double Drive::deadband(double input, double deadband_size){
    if (abs(input) < deadband_size){
        input = 0;
    }
    return input;
}

/* JOYSTICK DRIVE */
/* This function provides basic joystick control of the drive base*/
void Drive::Joystick_Drive(double LeftStick, double RightStick){
    
    
    // cubing function
    LeftStick = LeftStick * LeftStick * LeftStick;
    RightStick = RightStick * RightStick * RightStick;

    // deadband
    //LeftStick = deadband (LeftStick, 0.05);
    //RightStick = deadband (RightStick, 0.05);

    /*if (LeftStick > (leftdriveold + 0.3)){
        LeftStick = leftdriveold + 0.3;
    }
    else if (LeftStick < (leftdriveold - 0.3)){
        LeftStick = leftdriveold - 0.3;
    }

    if (RightStick > (rightdriveold + 0.3)){
        RightStick = rightdriveold + 0.3;
    }
    else if (RightStick < (rightdriveold - 0.3)){
        RightStick = rightdriveold - 0.3;
    }*/

    m_leftdrive->Set(LeftStick);
    m_leftdrive2->Set(LeftStick);
    leftdriveold = LeftStick;
    m_rightdrive->Set(RightStick);
    m_rightdrive2->Set(RightStick);
    rightdriveold = RightStick;



}

// buddy climb intake roller 
void Drive::buddyclimb_motor(double input){ 
    
    m_buddyclimb->Set(input);

}

// buddy climb deploy
void Drive::buddyclimb_out(){
    
    p_buddyclimb->Set(frc::DoubleSolenoid::Value::kForward);

}

// buddy climb retract
void Drive::buddyclimb_in(){
    
    p_buddyclimb->Set(frc::DoubleSolenoid::Value::kReverse);
    
}

// original climber control, NOT INDIVIDUAL
void Drive::climb(double input){
    m_leftclimb->Set(input);
    m_rightclimb->Set(input);    
    /* Run climber motors
    double lower_bound = 100;
    double upper_bound = 5000000000;
    double leftclimb_enc_val = s_leftclimb_enc->GetPosition();
    auto leftclimb_enc_val_str = std::to_string(leftclimb_enc_val);
    frc::SmartDashboard::PutString("DB/String 5",leftclimb_enc_val_str);
    double rightclimb_enc_val = s_rightclimb_enc->GetPosition();
    auto rightclimb_enc_val_str = std::to_string(rightclimb_enc_val);
    frc::SmartDashboard::PutString("DB/String 6",rightclimb_enc_val_str);
    if ((input < 0) && (leftclimb_enc_val > lower_bound) && (rightclimb_enc_val > lower_bound)){
        m_leftclimb->Set(input);
        m_rightclimb->Set(input);
    }
    else if ((input > 0) && (leftclimb_enc_val < upper_bound) && (rightclimb_enc_val < upper_bound)){
        m_leftclimb->Set(input);
        m_rightclimb->Set(input);
    }
    else {
        m_leftclimb->Set(0);
        m_rightclimb->Set(0);
    }*/
    

}

// right side climber control with soft stops
void Drive::climb_right(double input){
    //m_rightclimb->Set(input);    
    //Run climber motors
    double lower_bound = 100;
    double upper_bound = 5000000000;
    
    double rightclimb_enc_val = s_rightclimb_enc->GetPosition();
    auto rightclimb_enc_val_str = std::to_string(rightclimb_enc_val);
    frc::SmartDashboard::PutString("DB/String 6",rightclimb_enc_val_str);
    if ((input < 0) && (rightclimb_enc_val > lower_bound)){
           m_rightclimb->Set(input);
    }
    else if ((input > 0) && (rightclimb_enc_val < upper_bound)){
        m_rightclimb->Set(input);
    }
    else {
        
        m_rightclimb->Set(0);
    }
}

// left side climber control with soft stops
void Drive::climb_left(double input){
    //m_leftclimb->Set(input);    
    //Run climber motors
    double lower_bound = 100;
    double upper_bound = 5000000000;
    
    double leftclimb_enc_val = s_leftclimb_enc->GetPosition();
    auto leftclimb_enc_val_str = std::to_string(leftclimb_enc_val);
    frc::SmartDashboard::PutString("DB/String 7",leftclimb_enc_val_str);
    if ((input < 0) && (leftclimb_enc_val > lower_bound)){
           m_leftclimb->Set(input);
    }
    else if ((input > 0) && (leftclimb_enc_val < upper_bound)){
        m_leftclimb->Set(input);
    }
    else {
        
        m_leftclimb->Set(0);
    }
    
}

// path following 
void Drive::drive_PID(double setpoint_left_pos, double setpoint_right_pos, double setpoint_left_speed, double setpoint_right_speed, double heading, int count) {
  
  if(count ==0){
    //Gyro->Reset();
    s_leftdrive_enc->Reset();
    s_rightdrive_enc->Reset();
  }
  double encoder_val_left = s_leftdrive_enc->Get();
  double encoder_val_right = s_rightdrive_enc->Get();
  //double encoder_speed_left = s_leftdrive_enc->GetRate();
  //double encoder_speed_right = s_rightdrive_enc->GetRate();
  double gyro_val = s_gyro->GetAngle();

  double error_left_pos = setpoint_left_pos - encoder_val_left;
  double error_right_pos = setpoint_right_pos - encoder_val_right;
  //double error_left_speed = setpoint_left_speed - encoder_speed_left;
  //double error_right_speed = setpoint_right_speed - encoder_speed_right;
  double error_heading = heading - gyro_val;
  


  double max_speed = frc::SmartDashboard::GetNumber("p input 2", 8250);//9000,8000//frc::SmartDashboard::GetNumber("p input 2", 9750);//8250
  double kp_speed = -1/(max_speed);
  double kp_pos = 0; //-0.002;//frc::SmartDashboard::GetNumber("p input", -0.025);//-0.074;
  
  double kph = frc::SmartDashboard::GetNumber("p input", -0.0072);//-0.0072;//-0.01;  //0.01;

  double output_left = (error_left_pos * kp_pos) + kp_speed*setpoint_left_speed;
  double output_right = (error_right_pos * kp_pos) + kp_speed*setpoint_right_speed;

  double turn_val = kph * error_heading;

  auto error_right_str = std::to_string(output_left);
  frc::SmartDashboard::PutString("DB/String 8", error_right_str);
  auto error_left_str = std::to_string(output_right);
  frc::SmartDashboard::PutString("DB/String 6", error_left_str);
    error_left_str = std::to_string(turn_val);
  frc::SmartDashboard::PutString("DB/String 5", error_left_str);

  m_leftdrive->Set(output_left + turn_val);
  m_leftdrive2->Set(output_left + turn_val);
  m_rightdrive->Set(output_right - turn_val);
  m_rightdrive2->Set(output_right - turn_val);

  
}


// camera auto centering
bool Drive::camera_centering(float camera_x, float camera_s, double d){

    double setpoint = 0.0;
    double denominator = 20.0;
    double numerator = 20.0;
    double x = 30.0;
    

    if (camera_s < -80 && camera_s > -90){

        camera_s = camera_s + 90;

    }

    /*if (camera_s > -6 && camera_s < 6){

        numerator = d * (sin(camera_s*3.1415/180.0));
        denominator = (d* cos(camera_s*3.14159/180.0) + x);

        setpoint = ((numerator/denominator)*180.0/3.14159);

    }
    else {*/

        setpoint = 0;
   // }
      
    //auto setpointstr = std::to_string(setpoint);
    //frc::SmartDashboard::PutString("DB/String 5", setpointstr);
    
    double error = setpoint - camera_x;
    
    //double kp_c = .015;
    //double output = kp_c * error;

    

    arr.push_back(error);
    double error_i = 0;
    for (int i = 0; i <= arr.size(); i++){
        error_i = error_i + arr[i];
    }
    arr.resize(10);
    double kp_i = 0; //frc::SmartDashboard::GetNumber("p input 2", -0.0072);//0.015;

    double kp_c = 0.015; //frc::SmartDashboard::GetNumber("p input", -0.0072);//.01;
    double output = kp_c * error + kp_i * error_i;

    auto integralstr = std::to_string(error_i);
    frc::SmartDashboard::PutString("DB/String 9", integralstr);
    
    m_leftdrive->Set(output);
    m_leftdrive2->Set(output);
    m_rightdrive->Set(-output);
    m_rightdrive2->Set(-output);
    bool output1 = false;
    if (abs(error) < 2){ // not sure if it's one so yeah
      output1 = true;
    }
    return output1;
  
}

// manual shift into low gear
void Drive::shift_low(){
    // Set drive transmissions into low gear
    p_driveshift->Set(frc::DoubleSolenoid::Value::kForward);

}

// manual shift into high gear
void Drive::shift_high(){
    // Set drive transmissions into high gear
    p_driveshift->Set(frc::DoubleSolenoid::Value::kReverse);

}

// autoshifting function 
void Drive::shift_auto(){
    // Auto shifting logic

    double enc_left_rate = s_leftdrive_enc->GetRate();
    double enc_right_rate = s_rightdrive_enc->GetRate();

    enc_left_rate = enc_left_rate * -1; //Inverting left drive rate
    double enc_avg_rate = (enc_left_rate + enc_right_rate)/2; // Avg rate
    enc_avg_rate = abs(enc_avg_rate); // Absolute value in encoder rate

    if (enc_left_rate * enc_right_rate > 0){ // Checking if left and right side are going the same direction

        if (enc_avg_rate > 5000){ // 5000 is a fake number 
            // If wheel speed is high enough shift into high gear
            shift_high();
        }
        
    }
    else {
        // Default is low gear
        shift_low();
    }

}

// distance conversions from camera
double Drive::camera_getdistance(float camera_y){

    double h2 = 91; // height to the center of the vision target on the goal
    double h1 = 37.75; // height of camera on robot
    double a1 = 26; // angle of the camera on the robot relative to the ground
    double a2 = camera_y;

    double d = (h2-h1)/tan((a1+a2)*3.14159/180);

    auto dstr = std::to_string(d);
    frc::SmartDashboard::PutString("DB/String 3", dstr);

    return d;
}

// dashboard sensor output 
void Drive::dashboard(){

    // sensor outputs
    double val_1 = s_leftdrive_enc->Get();
    auto val_1_str = std::to_string(val_1);
    frc::SmartDashboard::PutString("Left Drive Encoder",val_1_str);
    frc::SmartDashboard::PutString("DB/String 0",val_1_str);

    double val_2 = s_rightdrive_enc->Get();
    auto val_2_str = std::to_string(val_2);
    frc::SmartDashboard::PutString("Right Drive Encoder",val_2_str);
    frc::SmartDashboard::PutString("DB/String 1",val_2_str);

    double val_3 = s_gyro->GetAngle();
    auto val_3_str = std::to_string(val_3);
    frc::SmartDashboard::PutString("Gyro", val_3_str);

    double leftdrive_log = m_leftdrive->Get();
    double rightdrive_log = m_rightdrive->Get();
    MyLog.CurrentCompare(1, leftdrive_log);
    MyLog.CurrentCompare(14, leftdrive_log);
    MyLog.CurrentCompare(0, rightdrive_log);
    MyLog.CurrentCompare(15, rightdrive_log);


}

// climber brakes off
void Drive::climberunlock(){
    // Deploy buddy climb claw
    p_climberlock->Set(frc::DoubleSolenoid::Value::kForward);
}

// climber brakes on
void Drive::climberlock(){
    // Retract buddy climb claw
    p_climberlock->Set(frc::DoubleSolenoid::Value::kReverse);

}

// reset gyro
void Drive::gyro_reset(){
    s_gyro->Reset();
}

// reset encoder
void Drive::encoder_reset(){
    s_leftdrive_enc->Reset();
    s_rightdrive_enc->Reset();
}

// turn to angle
void Drive::turn_to(double angle){
    double gyro_val = s_gyro->GetAngle();
    double error = angle - gyro_val;
    double kp = 0.01;
    double output = error * kp;

    m_leftdrive->Set(-output);
    m_leftdrive2->Set(-output);
    m_rightdrive->Set(output);
    m_rightdrive2->Set(output);

}

void Drive::drive_straight(double Leftstick, bool first){
    if (first){
        gyro_reset();

    }
    double gyro_val = s_gyro->GetAngle();
    double error = 0 - gyro_val;
    double kp = 0.01;
    double output = kp * error;

    Leftstick=(Leftstick*Leftstick*Leftstick);

    m_leftdrive->Set(Leftstick - output);
    m_leftdrive2->Set(Leftstick - output);
    m_rightdrive->Set(Leftstick + output);
    m_rightdrive2->Set(Leftstick + output);
    
}