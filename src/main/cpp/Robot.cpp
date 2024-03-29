/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
/* ---------------------------------------------------------------------------
------------------------------NAMING CONVENTION---------------------------------

MOTORS: m_function  ex: m_leftdrive
SENSORS: s_type  ex: s_gyro
CONTROLLERS: c#_type_name  ex: c1_btn_a
PNEUMATICS: p_type
BRANCHES OF CODE: section/what you're working on     ex: Drive/JoystickControl
                  also: merge into master before each event, create event, create 
                  event branch and merge every night
--------------------------------------------------------------------------------
*/
#include "Robot.h"
#include "Drive.h"
#include "Paths.h"
#include "Led.h"
#include "Appendage.h"
#include "NetworkTables/NetworkTable.h"
#include "NetworkTables/NetworkTableInstance.h"
#include <iostream>
#include "Log.h"
#include <frc/smartdashboard/SmartDashboard.h>
#include <frc/Compressor.h>

void Robot::RobotInit() {
  bool leftbuttonstate = false;
  bool rightbuttonstate = false;
  auto trimstart=frc::SmartDashboard::GetNumber("Trim Start",0);

  int shootercounter = (int)trimstart;
  m_chooser.SetDefaultOption(kAutoNameDefault, kAutoNameDefault);
  m_chooser.AddOption(kAutoNameCustom, kAutoNameCustom);
  frc::SmartDashboard::PutData("Auto Modes", &m_chooser);

  // Set default positions and values upon start up.
  MyDrive.buddyclimb_in();
  bool buddyclimb_enable = false;
  MyDrive.buddyclimb_motor(0);
  bool climb_enable = false;
  MyDrive.shift_low();
  MyDrive.climberlock();
  MyAppendage.intake_in();
}

/**
 * This function is called every robot packet, no matter the mode. Use
 * this for items like diagnostics that you want ran during disabled,
 * autonomous, teleoperated and test.
 *
 * <p> This runs after the mode specific periodic functions, but before
 * LiveWindow and SmartDashboard integrated updating.
 */
void Robot::RobotPeriodic() {
  
}


/**
 * This autonomous (along with the chooser code above) shows how to select
 * between different autonomous modes using the dashboard. The sendable chooser
 * code works with the Java SmartDashboard. If you prefer the LabVIEW Dashboard,
 * remove all of the chooser code and uncomment the GetString line to get the
 * auto name from the text box below the Gyro.
 *
 * You can add additional auto modes by adding additional comparisons to the
 * if-else structure below with additional strings. If using the SendableChooser
 * make sure to add them to the chooser code above as well.
 */
void Robot::AutonomousInit() {


    m_autoSelected = m_chooser.GetSelected();
  // m_autoSelected = SmartDashboard::GetString("Auto Selector",
  //     kAutoNameDefault);
  std::cout << "Auto selected: " << m_autoSelected << std::endl;

  int count = 0;
  int count_delay = 0;
  bool path_a = false;
  bool redblue = true; //true is red

  MyDrive.gyro_reset();
  MyDrive.encoder_reset();

  if (m_autoSelected == kAutoNameCustom) {
    // Custom Auto goes here
  } else {
    // Default Auto goes here
  }
   // Read in camera Stuff
  
  std::shared_ptr<NetworkTable> table = nt::NetworkTableInstance::GetDefault().GetTable("limelight");
  
  table->PutNumber("ledMode", 1);
  table->PutNumber("camMode", 0);

  // -----------PIPELINE STUFF-----------//
  table->PutNumber("pipeline", 0);

  //--------CAMERA VALUES-----------------//
  float camera_x = table->GetNumber("tx", 0);
    
  float camera_exist = table->GetNumber("tv", 0);
  float image_size = table->GetNumber("ta", 0);
  float camera_y = table->GetNumber("ty", 0);
  float camera_s = table->GetNumber("ts", 0);

  // values to dashboard
  auto leftinstr = std::to_string(camera_x);
  frc::SmartDashboard::PutString("Limelight-TX", leftinstr);

  auto sstr = std::to_string(camera_s);
  frc::SmartDashboard::PutString("Limelight-TS", sstr);

  auto ystr = std::to_string(camera_y);
  frc::SmartDashboard::PutString("Limelight-TY", ystr);
  
  double d = MyDrive.camera_getdistance(camera_y);

  auto dstr = std::to_string(d);
  frc::SmartDashboard::PutString("Limelight-Distance", dstr);

}

void Robot::AutonomousPeriodic() {



  // Read in camera Stuff
  
  std::shared_ptr<NetworkTable> table = nt::NetworkTableInstance::GetDefault().GetTable("limelight");
  
  table->PutNumber("ledMode", 0);
  table->PutNumber("camMode", 0);

  // -----------PIPELINE STUFF-----------//
  table->PutNumber("pipeline", 0);

  //--------CAMERA VALUES-----------------//
  float camera_x = table->GetNumber("tx", 0);
  float camera_exist = table->GetNumber("tv", 0);
  float image_size = table->GetNumber("ta", 0);
  float camera_y = table->GetNumber("ty", 0);
  float camera_s = table->GetNumber("ts", 0);

  if (count<2){

  if(image_size < 0.02){ // blue paths

    redblue = false;
  }
  else{ //red paths

    redblue = true; 
   
  }

   if (abs(camera_x)<10){
        path_a = true;
      }
      
      else{path_a = false;}

  }

  // values to dashboard
  auto leftinstr = std::to_string(camera_x);
  frc::SmartDashboard::PutString("Limelight-TX", leftinstr);

  auto sstr = std::to_string(path_a);
  frc::SmartDashboard::PutString("Limelight-TS", sstr);

  auto ystr = std::to_string(camera_y);
  frc::SmartDashboard::PutString("Limelight-TY", ystr);
  
  double d = MyDrive.camera_getdistance(camera_y);

  auto dstr = std::to_string(d);
  frc::SmartDashboard::PutString("Limelight-Distance", dstr);

 
  auto mode = frc::SmartDashboard::GetString("Autonomous","0");
  auto delay = frc::SmartDashboard::GetNumber("Auton Delay",0);
  int delay_int = (int)delay;
  delay_int = delay_int * 50; 

  double count_max = MyPaths.ReturnTableVal(0,5,0);
  int count_max_int = (int)count_max;
  bool aligned = false;
  bool wheel_speed = false;


  
    if (mode =="0"){
      // Salom

      MyDrive.shift_low();
      if (count < 831){
        double left_pos = MyPaths.ReturnTableVal(count,0,0);
        double left_speed = MyPaths.ReturnTableVal(count,1,0);
        double right_pos = MyPaths.ReturnTableVal(count,2,0);
        double right_speed = MyPaths.ReturnTableVal(count,3,0);
        double heading = MyPaths.ReturnTableVal(count,4,0);
        

        //Call PID Loop to follow path
        MyDrive.drive_PID(left_pos, right_pos, left_speed, right_speed,heading,count);
      }

      else {
        MyDrive.Joystick_Drive(0,0);
      } 
    

    }
    if (mode =="1"){
      // Barrel

      MyDrive.shift_low();
      if (count < 1113){
        double left_pos = MyPaths.ReturnTableVal(count,0,0);
        double left_speed = MyPaths.ReturnTableVal(count,1,0);
        double right_pos = MyPaths.ReturnTableVal(count,2,0);
        double right_speed = MyPaths.ReturnTableVal(count,3,0);
        double heading = MyPaths.ReturnTableVal(count,4,0);
        

        //Call PID Loop to follow path
        MyDrive.drive_PID(left_pos, right_pos, left_speed, right_speed,heading,count);
      }

      else {
        MyDrive.Joystick_Drive(0,0);
      } 
    

    }

     if (mode == "2"){ //Ball pickup

     if (count < 25){
       MyAppendage.intakemotor(0.9);
     }
     else if (count < 50){
       MyAppendage.intakemotor(0.0);
     }
     else{
       MyAppendage.intakemotor(0.9);
       MyAppendage.elevatorauto();
     }

     if (!redblue){  // Blue paths
        if (path_a){
                MyDrive.shift_low();
        if (count < 576){
          double left_pos = MyPaths.ReturnTableVal(count,0,2);
          double left_speed = MyPaths.ReturnTableVal(count,1,2);
          double right_pos = MyPaths.ReturnTableVal(count,2,2);
          double right_speed = MyPaths.ReturnTableVal(count,3,2);
          double heading = MyPaths.ReturnTableVal(count,4,2);
          

          //Call PID Loop to follow path
          MyDrive.drive_PID(-1*right_pos, -1*left_pos, -1*right_speed, -1*left_speed,heading,count);
        }

        else {
          MyDrive.Joystick_Drive(0,0);
        } 
        }
        // Custom Auto goes here
        else{
        MyDrive.shift_low();
        if (count < 529){
          double left_pos = MyPaths.ReturnTableVal(count,0,3);
          double left_speed = MyPaths.ReturnTableVal(count,1,3);
          double right_pos = MyPaths.ReturnTableVal(count,2,3);
          double right_speed = MyPaths.ReturnTableVal(count,3,3);
          double heading = MyPaths.ReturnTableVal(count,4,3);
          

          //Call PID Loop to follow path
          MyDrive.drive_PID(-1*right_pos, -1*left_pos, -1*right_speed, -1*left_speed,heading,count);
        }

        else {
          MyDrive.Joystick_Drive(0,0);
        } 
      }
     }

     else{  //Red paths
        if (path_a){
                MyDrive.shift_low();
        if (count < 750){
          double left_pos = MyPaths.ReturnTableVal(count,0,0);
          double left_speed = MyPaths.ReturnTableVal(count,1,0);
          double right_pos = MyPaths.ReturnTableVal(count,2,0);
          double right_speed = MyPaths.ReturnTableVal(count,3,0);
          double heading = MyPaths.ReturnTableVal(count,4,0);
          

          //Call PID Loop to follow path
          MyDrive.drive_PID(-1*right_pos, -1*left_pos, -1*right_speed, -1*left_speed,heading,count);
        }

        else {
          MyDrive.Joystick_Drive(0,0);
        } 
        }
        // Custom Auto goes here
        else{
        MyDrive.shift_low();
        if (count < 586){
          double left_pos = MyPaths.ReturnTableVal(count,0,1);
          double left_speed = MyPaths.ReturnTableVal(count,1,1);
          double right_pos = MyPaths.ReturnTableVal(count,2,1);
          double right_speed = MyPaths.ReturnTableVal(count,3,1);
          double heading = MyPaths.ReturnTableVal(count,4,1);
          

          //Call PID Loop to follow path
          MyDrive.drive_PID(-1*right_pos, -1*left_pos, -1*right_speed, -1*left_speed,heading,count);
        }

        else {
          MyDrive.Joystick_Drive(0,0);
        } 
      }

     }
    

    }
    if (mode =="3"){
      // Push auto

      MyDrive.shift_low();
      if (count < 362){
        double left_pos = MyPaths.ReturnTableVal(count,0,0);
        double left_speed = MyPaths.ReturnTableVal(count,1,0);
        double right_pos = MyPaths.ReturnTableVal(count,2,0);
        double right_speed = MyPaths.ReturnTableVal(count,3,0);
        double heading = MyPaths.ReturnTableVal(count,4,0);
        

        //Call PID Loop to follow path
        MyDrive.drive_PID(left_pos, right_pos, left_speed, right_speed,heading,count);
      }
      else if (count < (362+482)){
        double left_pos = MyPaths.ReturnTableVal(count-362,0,1);
        double left_speed = MyPaths.ReturnTableVal(count-362,1,1);
        double right_pos = MyPaths.ReturnTableVal(count-362,2,1);
        double right_speed = MyPaths.ReturnTableVal(count-362,3,1);
        double heading = MyPaths.ReturnTableVal(count-362,4,1);
        

        //Call PID Loop to follow path
        MyDrive.drive_PID(-1*right_pos, -1*left_pos, -1*right_speed, -1*left_speed,heading,count);
      }

      else if (count < (362+482+482)){
        double left_pos = MyPaths.ReturnTableVal(count-362-482,0,2);
        double left_speed = MyPaths.ReturnTableVal(count-362-482,1,2);
        double right_pos = MyPaths.ReturnTableVal(count-362-482,2,2);
        double right_speed = MyPaths.ReturnTableVal(count-362-482,3,2);
        double heading = MyPaths.ReturnTableVal(count-362-482,4,2);
        

        //Call PID Loop to follow path
        MyDrive.drive_PID(left_pos, right_pos, left_speed, right_speed,heading,count);
      }

      else if (count < (362+482+482+366)){
        double left_pos = MyPaths.ReturnTableVal(count-362-482-482,0,3);
        double left_speed = MyPaths.ReturnTableVal(count-362-482-482,1,3);
        double right_pos = MyPaths.ReturnTableVal(count-362-482-482,2,3);
        double right_speed = MyPaths.ReturnTableVal(count-362-482-482,3,3);
        double heading = MyPaths.ReturnTableVal(count-362-482-482,4,3);
        

        //Call PID Loop to follow path
        MyDrive.drive_PID(-1*right_pos, -1*left_pos, -1*right_speed, -1*left_speed,heading,count);
      }

      else {
        MyDrive.Joystick_Drive(0,0);
      } 
    

    }



    auto error_left_str = std::to_string(count);
  frc::SmartDashboard::PutString("DB/String 7", error_left_str);
    count ++;
  



  count_delay ++;

MyLog.Dashboard();
MyLog.PDPTotal();
MyDrive.dashboard();
MyAppendage.dashboard();
}

void Robot::TeleopInit() {

  MyAppendage.controlpanel_colorsense_init(); //Initalize the color sensor
  bool first = true;    
  
}

void Robot::TeleopPeriodic() {


// Read in camera Stuff
  
  std::shared_ptr<NetworkTable> table = nt::NetworkTableInstance::GetDefault().GetTable("limelight");
  bool ellie = frc::SmartDashboard::GetBoolean("Auto Elevator", true);
  table->PutNumber("ledMode", 0);
  table->PutNumber("camMode", 0);

  // -----------PIPELINE STUFF-----------//
  table->PutNumber("pipeline", 0);

  //--------CAMERA VALUES-----------------//
  float camera_x = table->GetNumber("tx", 0);
  float camera_exist = table->GetNumber("tv", 0);
  float image_size = table->GetNumber("ta", 0);
  float camera_y = table->GetNumber("ty", 0);
  float camera_s = table->GetNumber("ts", 0);

  // values to dashboard
  auto leftinstr = std::to_string(camera_x);
  frc::SmartDashboard::PutString("Limelight-TX", leftinstr);

  auto sstr = std::to_string(camera_s);
  frc::SmartDashboard::PutString("Limelight-TS", sstr);

  auto ystr = std::to_string(camera_y);
  frc::SmartDashboard::PutString("Limelight-TY", ystr);
  
  double d = MyDrive.camera_getdistance(camera_y);

  auto dstr = std::to_string(d);
  frc::SmartDashboard::PutString("Limelight-Distance", dstr);

  //create compressor

  Compressor = new frc::Compressor(1);


  

  //********** Read in Joystick Values ******************************************
  //------------- Driver Controller ---------------------------------------------

  double c1_joy_leftdrive = controller1.GetRawAxis(1);
  double c1_joy_rightdrive = controller1.GetRawAxis(5);
  bool c1_btn_back = controller1.GetRawButton(7);
  bool c1_btn_start = controller1.GetRawButton(8);
  double c1_righttrigger = controller1.GetRawAxis(3);
  double c1_lefttrigger = controller1.GetRawAxis(2);
  bool c1_leftbmp = controller1.GetRawButton(5);
  bool c1_rightbmp = controller1.GetRawButton(6);
  bool c1_btn_b = controller1.GetRawButton(2);
  bool c1_btn_x = controller1.GetRawButton(3);

  //-----------------------------------------------------------------------------
  //------------ Operator Controller --------------------------------------------
  double c2_joy_left = controller2.GetRawAxis(1);
  bool c2_btn_a = controller2.GetRawButton(1);
  bool c2_btn_b = controller2.GetRawButton(2);
  bool c2_btn_y = controller2.GetRawButton(4);
  bool c2_btn_x = controller2.GetRawButton(3);
  bool c2_btn_lb = controller2.GetRawButton(5);
  bool c2_btn_rb = controller2.GetRawButton(6);
  double c2_dpad = controller2.GetPOV(0);
  bool c2_btn_back = controller2.GetRawButton(7);
  bool c2_btn_start = controller2.GetRawButton(8);
  bool c2_rightbumper = controller2.GetRawButton(6);
  bool c2_leftbumper = controller2.GetRawButton(5);
  bool c2_right_trigger = controller2.GetRawAxis(3);
  bool c2_left_trigger = controller2.GetRawAxis(2);

  //----------------------------------------------------------------------------
  // ***************************************************************************
  //************* Drive Code ***************************************************

  bool aligned = false;
  
  if (c1_btn_b && camera_exist == 1){
    MyDrive.shift_low();
    aligned = MyDrive.camera_centering(camera_x, camera_s, d);
    
  }
  else if (c1_btn_x){
    MyDrive.drive_straight(c1_joy_leftdrive, first);
    first = false;
  }
  else {

    MyDrive.Joystick_Drive(c1_joy_leftdrive,c1_joy_rightdrive); // Basic joystick drive
    first = true;

  }

  //------------ Shifting Logic ----------------------------------------
  if (c1_leftbmp){
    MyDrive.shift_low();

  }
  else if (c1_rightbmp){
    MyDrive.shift_high();
  }
  else {
    //MyDrive.shift_auto();
    MyDrive.shift_low();
  }
  //--------------------------------------------------------------------
//******************************************************************************
//************* Appendage Code *************************************************

  

//--------------- control panel ------------------------------------------------- 
if (!buddyclimb_enable){

    if (c2_btn_b){
      MyAppendage.control_panel(0.6);
    }
    else if (c2_btn_y){
      MyAppendage.controlpanel_colorsense_periodicrotation();
    }
    else if (c2_btn_x){
      MyAppendage.controlpanel_colorsense_periodic();
    }
    else if (c2_btn_a){
        MyAppendage.control_panel(-0.6);
    }
    else {
      MyAppendage.control_panel(0);
    }

  }
//-----------------------------------------------------------------------------
//----------- buddy climb ----------------------------------------------------- 

if (c2_btn_back && c2_btn_start){

  buddyclimb_enable = true;

}

if (buddyclimb_enable){

    if (c2_btn_b){
      MyDrive.buddyclimb_in();
    }
    else if (c2_btn_y){
      MyDrive.buddyclimb_out();
    }
    if (c2_btn_a){
      MyDrive.buddyclimb_motor(0.8);
    }
    else {
      MyDrive.buddyclimb_motor(0);
    }

}
//------------------------------------------------------------------------
//----------- climber code -----------------------------------------------

if (c1_btn_back && c1_btn_start){

  climb_enable = true;

}

if (climb_enable){
  

  if (c1_lefttrigger > 0.5 ){

    MyDrive.climberunlock();

    MyDrive.climb_left(-1);

  }
  
  else if (c1_leftbmp){
    
    MyDrive.climberunlock();

    MyDrive.climb_left(0.7);
  }

  else{

    MyDrive.climberlock();

    MyDrive.climb_left(0);
  }


  if (c1_righttrigger > 0.5 ){

    MyDrive.climberunlock();

    MyDrive.climb_right(-1);
  }
  
  else if (c1_rightbmp){

    MyDrive.climberunlock();

    MyDrive.climb_right(0.7);
  }
  else{

    MyDrive.climberlock();

    MyDrive.climb_right(0);
  }


}


// ---- shooter trim value ---------//
if (c2_dpad > 45 && c2_dpad < 135){
  if (!leftbuttonstate){
    leftbuttonstate = true;
    shootercounter--;
  }
}
else{leftbuttonstate=false;}
if (c2_dpad > 225 && c2_dpad < 315){
  if (!rightbuttonstate){
  
    rightbuttonstate = true;
    shootercounter++;
  }
}
else {rightbuttonstate=false;}
frc::SmartDashboard::PutNumber("Current Trim",shootercounter);



//intake code
if (buddyclimb_enable){
  if(c2_rightbumper){ 
    MyAppendage.intake_out();
  }
}
else{
  if (c2_dpad > 160 && c2_dpad < 200){
    MyAppendage.intake_out();
  }
  if (c2_btn_y){
    MyAppendage.intake_in();
  }
  if (c2_joy_left < -0.9){
    MyAppendage.conveyor_motor(0.95);
  }
  else if (c2_joy_left > 0.9){
    MyAppendage.conveyor_motor(-0.8);
    MyAppendage.shooter_raw(-0.3);
  }
  else{
  if(c2_rightbumper){ 
    //MyAppendage.intake_out();
    MyAppendage.intakemotor(0.9);

    //MyAppendage.conveyor_motor(0.95);
    //MyAppendage.shooter_feed(-0.8);
    MyAppendage.shooter_raw(-0.3);

    if (ellie){
    MyAppendage.elevatorauto();
    }
  }
  
  else if (c2_leftbumper){
    MyAppendage.intakemotor(-0.9);
    //MyAppendage.conveyor_motor(-0.95);
    //MyAppendage.shooter_feed(-0.8);
    //MyAppendage.shooter_raw(-0.3);
    
  }
  else {
    MyAppendage.intakemotor(0);
    //MyAppendage.intake_in();
   // MyAppendage.shooter_feed(0);
    
  }
  }
}


// ---------- compressor start/stop --------------------

if (climb_enable || c2_left_trigger > 0.5){

  Compressor->Stop();
}
else {

  Compressor->Start();
}



//------------------------------------------------------------
//----------shooter modes-------------------------------------

bool wheel_speed = false;
auto shootertrimstr = std::to_string(shootercounter);
frc::SmartDashboard::PutString("Shooter Trim", shootertrimstr);
if (c2_left_trigger > 0.5){
  if (camera_exist==1){
    //wheel_speed = MyAppendage.shooter_get_distance(shootercounter);
    wheel_speed = MyAppendage.shooter_pid(d, shootercounter);

      if (aligned && wheel_speed && c2_right_trigger > 0.5){

        MyAppendage.conveyor_motor(0.95);
        //MyAppendage.shooter_feed(0.8);
      }
      else {

        MyAppendage.conveyor_motor(0);
       // MyAppendage.shooter_feed(0);

    }
    }
  else {
    wheel_speed = MyAppendage.shooter_pid(120, shootercounter);

      if (c2_right_trigger > 0.5){

        MyAppendage.conveyor_motor(0.8);
       // MyAppendage.shooter_feed(0.8);
      }
      else {

        MyAppendage.conveyor_motor(0);
        //MyAppendage.shooter_feed(0);

      }
    }
}
else{
  bool bumper = false;
  if (!c2_rightbumper && ellie){
    bumper = true;
  }
  else if (!ellie){
    bumper = true;
  }
  if((c2_joy_left > -0.9) && (c2_joy_left < 0.9) && (bumper)) {
    MyAppendage.conveyor_motor(0);
    MyAppendage.shooter_raw(0);
  }
 
  
  //MyAppendage.shooter_feed(0);
 
}
//---------------------LED CODE----------------------------------
bool ready_to_fire = false;
if (camera_exist && aligned && wheel_speed){

  MyLed.led_control("Hot_Pink");
  ready_to_fire = true;
  
}
else if ((camera_exist && !aligned && wheel_speed) || (camera_exist && aligned && !wheel_speed)){

  MyLed.led_control("Blue");
  
}

else if (camera_exist && !aligned && !wheel_speed){

  MyLed.led_control("White");
  
}
else {

  MyLed.led_control("Black");
  
}
bool camera_exist1 = false;
if (camera_exist == 1){
  camera_exist1 = true;
}

  frc::SmartDashboard::PutBoolean("Ready to Fire", ready_to_fire);
  frc::SmartDashboard::PutBoolean("In Camera", camera_exist1);
  frc::SmartDashboard::PutBoolean("Wheel at Speed", wheel_speed);
  frc::SmartDashboard::PutBoolean("Aligned", aligned);

// --------- dashboard code ---------------

MyLog.Dashboard();
MyLog.PDPTotal();
MyDrive.dashboard();
MyAppendage.dashboard();



} // End of TeleOpPeriodic


void Robot::TestPeriodic() {}

#ifndef RUNNING_FRC_TESTS
int main() { return frc::StartRobot<Robot>(); }
#endif
