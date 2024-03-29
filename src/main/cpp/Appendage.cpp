

#include "frc\Joystick.h"
#include "rev/CANSparkMax.h"
#include "frc/VictorSP.h"
#include "Robot.h"
#include "Appendage.h"
#include "Log.h"
#include "frc\Encoder.h"
#include <frc/util/color.h>
#include "rev/ColorSensorV3.h"
#include "rev/ColorMatch.h"
#include <frc/DriverStation.h>
#include "rev/CANSparkMax.h"
#include <frc/smartdashboard/SmartDashboard.h>
#include <frc\I2C.h>
#include <frc/DoubleSolenoid.h>
#include <rev/CANEncoder.h>
#include "rev/SparkMax.h"
#include <frc/DigitalInput.h>




using namespace std;

    std::string colorStringold;
    int colorcounter=0;

Appendage::Appendage() : Subsystem("Appendage") {
    // Define CAN and PWM Ids used in Drive here
    
    int controlpanelID = 14;
    int controlpanelencID_a = 4;
    int controlpanelencID_b = 5;
  
    int shooterID = 9;
    int shooterID2 = 10;
    int shooterencID_a = 6;
    int shooterencID_b = 7;

    int intakeIDa = 6;
    int intakeIDb = 7;
    int intakeIDc = 0;
    int conveyormID = 13;
    int elevatorSID = 6;
    int verticalinID = 2;
   // int shooter_feedID = 15;

    // Define motors, sensors, and pneumatics here
    m_controlpanel = new rev::CANSparkMax{controlpanelID, rev::CANSparkMax::MotorType::kBrushless};
    s_controlpanel_encoder = new frc::Encoder( controlpanelencID_a, controlpanelencID_b, false, frc::Encoder::k4X);

    m_shooter =new rev::CANSparkMax{shooterID, rev::CANSparkMax::MotorType::kBrushless};
    s_shooter_encoder = new  rev::CANEncoder(*m_shooter);
    m_shooter -> SetInverted(true);
    m_shooter2 =new rev::CANSparkMax{shooterID2, rev::CANSparkMax::MotorType::kBrushless};
    m_shooter2 -> SetInverted (true);

   
    m_colorSensor = new rev::ColorSensorV3(frc::I2C::Port::kOnboard);
    m_colorMatcher = new rev::ColorMatch;
    m_intake = new rev::SparkMax{intakeIDc};
    m_vert = new rev::SparkMax{verticalinID};
    //m_intake -> SetInverted(true);
    p_intake = new frc::DoubleSolenoid(2, intakeIDa, intakeIDb);
    m_conveyor = new rev::CANSparkMax{conveyormID, rev::CANSparkMax::MotorType::kBrushless};
    
    s_elevator = new frc::DigitalInput(elevatorSID);

   // m_shooterfeed = new rev::CANSparkMax{shooter_feedID, rev::CANSparkMax::MotorType::kBrushless};

    }


double Appendage::deadband(double input, double deadband_size){
  // Deadband function - Takes input and checks it againist a provided deadband
  // then returns the value or zero depending if it is within or outside of the deadband.
    if (abs(input) < deadband_size){
        input = 0;
    }
    return input;
}

double Appendage::Threshold(double in,double thres){
// Threshold function - Takes and input and crops it based upon and the provided threshold value
// Works with positive and negative inputs. Threshold value should always be positive.
  double out = in;
  if (in>thres){
    out = thres;
  }
  if(in<-1*thres){
    out = -1*thres;
  }
  return out;
}

// manual control panel control
void Appendage::control_panel(double input){
    m_controlpanel->Set(input);
}
// automatic control panel control
void Appendage::controlpanel_rotation_auto(){
  // Spins the control panel 4 times automatically. Assumes control panel encoder is set to 
  // zero when the function is called.
    double setpoint = 100; // Must be updated once mechanism is created.
    double encoder_val = s_controlpanel_encoder->Get(); // Get encoder value
    double error = setpoint - encoder_val; // Calculate current error
    error = deadband(error, 10); // Apply a deadband to help overshoot.
    double kpe = .025; // P gain
    double output_e = error * kpe; // Calculate motor value
    //output_e = Threshold(output_e, 0.9); // Threshold motor value
    m_controlpanel->Set(output_e); // Set motor to value
    //auto encoder_valstr = std::to_string(encoder_val);
    //frc::SmartDashboard::PutString("DB/String 3",encoder_valstr);
}


// color sensing control panel
void Appendage::controlpanel_colorsense_init(){


  //Any updates here also have to be done in Appendage.h
  static constexpr frc::Color kBlueTarget = frc::Color(0.143, 0.427, 0.429);
  static constexpr frc::Color kGreenTarget = frc::Color(0.197, 0.561, 0.240);
  static constexpr frc::Color kRedTarget = frc::Color(0.561, 0.232, 0.114);
  static constexpr frc::Color kYellowTarget = frc::Color(0.361, 0.524, 0.113);
  static constexpr frc::Color kWhiteTarget = frc::Color(0.365, 0.464, 0.169);

  m_colorMatcher->AddColorMatch(kBlueTarget);
  m_colorMatcher->AddColorMatch(kGreenTarget);
  m_colorMatcher->AddColorMatch(kRedTarget);
  m_colorMatcher->AddColorMatch(kYellowTarget);
  m_colorMatcher->AddColorMatch(kWhiteTarget);

}

void Appendage::controlpanel_colorsense_periodic(){
    // Fucntion spins contorl panel to specified color recieved from driver station
    std::string color_in = driverstation_color(); // Get color from driver station
    if (color_in != "z"){

      frc::Color detectedColor = m_colorSensor->GetColor();
    
    // Get raw RGB values from color sensor and display on DS
    /*frc::SmartDashboard::PutNumber("Red", detectedColor.red);
    auto encoder_valstr = std::to_string(detectedColor.red);
    frc::SmartDashboard::PutString("DB/String 0",encoder_valstr);
    frc::SmartDashboard::PutNumber("Green", detectedColor.green);
    frc::SmartDashboard::PutNumber("Blue", detectedColor.blue);
    auto encoder_valstr1 = std::to_string(detectedColor.blue);
    frc::SmartDashboard::PutString("DB/String 1",encoder_valstr1);
    auto encoder_valstr2 = std::to_string(detectedColor.green);
    frc::SmartDashboard::PutString("DB/String 2",encoder_valstr2); */
      
    //Run the color match algorithm on our detected color

      std::string colorString;
      double confidence = 0.99;

      frc::Color matchedColor = m_colorMatcher->MatchClosestColor(detectedColor, confidence); // Determine color

      if (matchedColor == kBlueTarget) {
        colorString = "B";
      } else if (matchedColor == kRedTarget) {
        colorString = "R";
      } else if (matchedColor == kGreenTarget) {
        colorString = "G";
      } else if (matchedColor == kYellowTarget) {
        colorString = "Y";
      } else if (matchedColor == kWhiteTarget) {
        colorString = "W";
      } else {
        colorString = "Unknown";
      }
  frc::SmartDashboard::PutString("Current Color", colorString);
 if (colorString == color_in){
          m_controlpanel->Set(0); // If color matches desired stop motor
      }
      else {
          m_controlpanel->Set(0.6); // if color doesn't match desired color keep spinning
    }
  }
}

void Appendage::controlpanel_colorsense_periodicrotation(){
    // Fucntion spins contorl panel to specified color recieved from driver station

      frc::Color detectedColor = m_colorSensor->GetColor();
    
    // Get raw RGB values from color sensor and display on DS
    /*frc::SmartDashboard::PutNumber("Red", detectedColor.red);
    auto encoder_valstr = std::to_string(detectedColor.red);
    frc::SmartDashboard::PutString("DB/String 0",encoder_valstr);
    frc::SmartDashboard::PutNumber("Green", detectedColor.green);
    frc::SmartDashboard::PutNumber("Blue", detectedColor.blue);
    auto encoder_valstr1 = std::to_string(detectedColor.blue);
    frc::SmartDashboard::PutString("DB/String 1",encoder_valstr1);
    auto encoder_valstr2 = std::to_string(detectedColor.green);
    frc::SmartDashboard::PutString("DB/String 2",encoder_valstr2); */
      
    //Run the color match algorithm on our detected color

      std::string colorString;
      double confidence = 0.99;

      frc::Color matchedColor = m_colorMatcher->MatchClosestColor(detectedColor, confidence); // Determine color

      if (matchedColor == kBlueTarget) {
        colorString = "B";
      } else if (matchedColor == kRedTarget) {
        colorString = "R";
      } else if (matchedColor == kGreenTarget) {
        colorString = "G";
      } else if (matchedColor == kYellowTarget) {
        colorString = "Y";
      } else if (matchedColor == kWhiteTarget) {
        colorString = "W";
      } else {
        colorString = "Unknown";
      }



      //Display what color is seen on DS
      frc::SmartDashboard::PutString("Current Color", colorString);
      if (colorStringold != colorString){
          colorStringold = colorString;
          colorcounter++;
      }

      //Display what color is seen on DS
      if (colorcounter < 28){
          m_controlpanel->Set(0.6); // If color matches desired stop motor
      }
      else {
          m_controlpanel->Set(0); // if color doesn't match desired color keep spinning
      }
      /*auto encoder_valstr = std::to_string(colorcounter);
      frc::SmartDashboard::PutString("DB/String 6",encoder_valstr);*/
    }
  


// driver station data
std::string Appendage::driverstation_color(){
    // This function reads desired color in from driver station or gives a placeholder if no color is given.
    std::string output = "z"; // Create default value
    std::string gameData;
    gameData = frc::DriverStation::GetInstance().GetGameSpecificMessage(); // Read in message
    if(gameData.length() > 0)
    {
      switch (gameData[0])
      {
          case 'B' :
          output = 'R';
          //Blue case code
          break;
          case 'G' :
          output = 'Y';
          //Green case code
          break;
          case 'R' :
          output = 'B';
          //Red case code
          break;
          case 'Y' :
          output = 'G';
          //Yellow case code
          break;
          
      }
    }
    return output; 
}

// set shooter wheel to raw value
void Appendage::shooter_raw(double input){
  m_shooter->Set(input); // Set motor to value
  m_shooter2->Set(input);
}

// use camera distance to figure out shooter speed
bool Appendage::shooter_pid(double distance, int trim){

    double setpoint = 0.3885*distance*distance - 94.713*distance + 13427; //  
    
    if (setpoint < 6500){
      setpoint = 6500;
    }
    
    setpoint = setpoint + setpoint * trim/100.0;
    
    double encoder_val = s_shooter_encoder->GetVelocity(); // Get encoder value
    
    
    double error = setpoint - encoder_val; // Calculate current error
    //error = deadband(error, 10); // Apply a deadband to help overshoot.
    double kpe = 0.00015; //
    //double val = frc::SmartDashboard::GetNumber("p input", 0);//.0005; // P gain
    double coach_marc = 0;
    if (setpoint > 6000 && setpoint < 7500){
        coach_marc = 0.65;
    }
    else if (setpoint > 7500 && setpoint < 9000){
        coach_marc = 0.75;
    }
    else if (setpoint >= 9000 && setpoint < 11000){
      coach_marc = 0.85;
    }
    else if (setpoint >= 11000){
      coach_marc = 0.9;
    }
    double output_e = error * kpe + coach_marc; // Calculate motor value


    m_shooter->Set(output_e); // Set motor to value
    m_shooter2->Set(output_e);
    auto encoder_valstr = std::to_string(encoder_val);
    frc::SmartDashboard::PutString("DB/String 3",encoder_valstr);
    auto encoder_valstr2 = std::to_string(setpoint);
    frc::SmartDashboard::PutString("DB/String 2",encoder_valstr2);
    auto encoder_valstr3 = std::to_string(output_e);
    frc::SmartDashboard::PutString("DB/String 1",encoder_valstr3);
    bool output = false;
    if (encoder_val > 0.93*setpoint && encoder_val < 1.07*setpoint){
      output = true;
    }
    return output;
}


// gets distance from camera to give to shooter pid function
bool Appendage::shooter_get_distance(int trim){

    //double setpoint = distance * 30; // don't actually use this part we have no idea what's going on 

    double setpoint = trim*250;
    
    double encoder_val = s_shooter_encoder->GetVelocity(); // Get encoder value
    
    double error = setpoint - encoder_val; // Calculate current error
    //error = deadband(error, 10); // Apply a deadband to help overshoot.
    
    double kpe = 0.00015; //
    //double val = frc::SmartDashboard::GetNumber("p input", 0);//.0005; // P gain
    double coach_marc = 0;
    if (setpoint > 7500 && setpoint < 9000){
        coach_marc = 0.75;
    }
    else if (setpoint >= 9000 && setpoint < 11000){
      coach_marc = 0.85;
    }
    else if (setpoint >= 11000){
      coach_marc = 0.9;
    }
    double output_e = error * kpe + coach_marc; // Calculate motor value
    //output_e = Threshold(output_e, 0.9); // Threshold motor value
    m_shooter->Set(output_e); // Set motor to value
    m_shooter2->Set(output_e);
    auto encoder_valstr = std::to_string(encoder_val);
    frc::SmartDashboard::PutString("DB/String 3",encoder_valstr);
    auto encoder_valstr2 = std::to_string(setpoint);
    frc::SmartDashboard::PutString("DB/String 2",encoder_valstr2);
    auto encoder_valstr3 = std::to_string(output_e);
    frc::SmartDashboard::PutString("DB/String 1",encoder_valstr3);
    bool output = false;
    if ((encoder_val > (0.98*setpoint)) && (encoder_val < (1.02*setpoint))){
      output = true;
    }
    return output;
}


void Appendage::intakemotor(double input){

    m_intake->Set(input);
    m_vert->Set(input);

}

void Appendage::intake_out(){

    p_intake->Set(frc::DoubleSolenoid::Value::kForward);

}

void Appendage::intake_in(){

    p_intake->Set(frc::DoubleSolenoid::Value::kReverse);
    
}

void Appendage::conveyor_motor(double input){

  m_conveyor->Set(input);

}



/*void Appendage::shooter_feed(double input){

  m_shooterfeed->Set(input);

}*/

void Appendage::dashboard(){

  // sensor dashboard outputs
  double val_1 = s_controlpanel_encoder->Get();
  auto val_1_str = std::to_string(val_1);
  frc::SmartDashboard::PutString("Control Panel Encoder",val_1_str);

  double val_2 = s_shooter_encoder->GetVelocity();
  val_2 = -1*val_2;
  auto val_2_str = std::to_string(val_2);
  frc::SmartDashboard::PutString("Shooter Encoder", val_2_str);

  double shooter_log = m_shooter->Get();
  MyLog.CurrentCompare(11, shooter_log);
  MyLog.CurrentCompare(10, shooter_log);

  bool faye = s_elevator->Get();
  auto val_3_str = std::to_string(faye);
  frc::SmartDashboard::PutString("DB/String 9", val_3_str);
  
}

void Appendage::shooter_speed(double input){
  m_shooter->Set(input);
  m_shooter2->Set(input);
}

void Appendage::elevatorauto(){
    bool faye = s_elevator->Get();
    if (!faye) {
      m_conveyor->Set(0.6);
      }

      else {
      m_conveyor->Set(0);
      }
}