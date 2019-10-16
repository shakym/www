/*@!Encoding:1252*/
/*
  Test case implementation for Edison E18-2evo
  Thresholds and behavior are project specific and might not apply to another project.
  
  Author: Alexander Niebling
*/

includes
{
  // API for WinIDEA
  #include "..\..\..\General_Libs\DeTeCAT\CAPL_Environment_HiL\IDE_Functions\Test_Environment_IDE_Function.cin"
  #include "..\..\Contactor\CSECtrl\Cell_Simulator_Functions.cin"
  //#include "..\..\CSECtrl\CSE_Simulator_CAPL_Library.cin"
  
      // Funtions to Control PSU and Relaiscard
//  #include "..\..\..\General_Libs\CPX400DP\Cpx400Lib.cin"
//  #include "..\..\..\General_Libs\CPX400DP\Logging.cin"
  #include "..\..\..\General_Libs\LABPS3005DN\LABPS3005DNlib.cin"
  #include "..\..\..\General_Libs\LABPS3005DN\Logging.cin"
  #include "..\..\..\General_Libs\ConradRelaisCard\RS232_Relais.cin"  
  
  #include "..\..\..\General_Libs\DeTeCAT\CAPL_Environment_HiL\Functions\Test_Environment_Functions.cin"
  
  // General functions for test executuion
  #include "..\..\Contactor\include\Functions.cin"
  
  // global variables and enums
  #include "..\..\Contactor\include\Variables.cin"
  
  //Test Report information and layout by arb
  #include "..\..\Contactor\include\Test_Report_arb.cin"
  
  // Functions by arb
  #include "..\..\Contactor\include\Functions_arb.cin"

  // Parameters which include Parameterlist, NAME and BMU_VARIABLE which is connected with parameter
  #include "..\Variables\Battery_Parameterlist_BMU_variables.cin"

}


variables
{
  
  //Project
  enum en_gProject_4TE_Type     // Set the actual project to get correct settings of TE 
  {
    en_gProject_None = 0,
    en_gProject_EVO,
    en_gProject_PLUGIN
  } gTE_Set_2_Project;

  // Enums
    enum en_gFeatureState_4TE_Type  // For current state of features 
    {
      en_gFeatureState_InitVal = 0,
      en_gFeatureState_Manual,
      en_gFeatureState_Automatic
    };  
  
 // PSU Control
    enum en_gFeatureState_4TE_Type  gTE_PSUstate_4TE;      
      // CPX Netzteil Steuerung
    byte    lv_PSU_comPort = 12;
    float   lv_min_AmpereOnActiveSystem   = 0.3;// Atleast 300mA are expected to accept system state /Active/
    float   lv_max_AmpereOnInactiveSystem = 0.1;// Max 100mA are expected to accept system state /Inactive/    
    char    lv_CH_one = 1;       // Channel 1 (Left on CPX400)
    char    lv_CH_two = 2;       // Channel 2 (Right on CPX400)
      
  // CSE Control
    enum en_gFeatureState_4TE_Type gTE_CSESimulatorState_4TE;

    
  char Kill_TCP_Proxy_CMD[1024]   = "D:/BMS_Projects/Dt._Accu/BMS_DtAccu_2013/40_SW/40_Software_Test/10_Test_Bench/DeTeCAT/TCP_Proxy/kill_Tcp_Proxy.bat";
  char Start_TCP_Proxy_CMD[1024]  = "D:/BMS_Projects/Dt._Accu/BMS_DtAccu_2013/40_SW/40_Software_Test/10_Test_Bench/DeTeCAT/TCP_Proxy/Start_TCP_Proxy.bat";
}

RS232OnSend(dword port, byte buffer[], dword number)
{
  if (port == @cpx::ComPort)
  {
    cpxRS232OnSend(port, buffer, number); 
  }
//  else if (port == lv_RelaisCard_Config.Port)
//  {
//    gFunc_MPRC_RS232OnSend(port, buffer, number);
//  }
  else if (port == @csesim::ComPort)
  {
    csesimRS232OnSend(port, buffer, number); 
  }
  else if (port == @relais::ComPort) {
    // command to relais card was send
  }
  else 
  {
    write("### RS232 FAULT ### RS232OnSend - Illegal Com Port Send Function! Send function for unknown ComPort executed.");
  }
}


RS232OnReceive(dword port, byte buffer[], dword number)
{
  if (port == @cpx::ComPort)
  {
    cpxRS232OnReceive(port, buffer, number);
  }
//  else if (port == lv_RelaisCard_Config.Port)
//  { 
//    gFunc_MPRC_RS232OnReceive(port, buffer, number);
//  }
  else if (port == @csesim::ComPort)
  {
    csesimRS232OnReceive(port, buffer, number); 
  }
  else if (port == @relais::ComPort) {
    // command to relais card was send
  }
  else 
  {
    write("### RS232 FAULT ### RS232OnReceive - Illegal Com Port Receive Function (Callbackhandler activated by unknown ComPort.");
  }
}


RS232OnError( dword port, dword errorFlags )
{
  if (port == @cpx::ComPort)
  {
    write("### RS232 FAULT ### RS232OnError - ErrorFlag explanation see below (Callbackhandler activated by CPX NT.");
  }
//  else if (port == lv_RelaisCard_Config.Port)
//  {
//    write("### RS232 FAULT ### RS232OnError - ErrorFlag explanation see below (Callbackhandler activated by MPRC Card.");
//  }
  else if (port == @csesim::ComPort)
  {
    write("### RS232 FAULT ### RS232OnError - ErrorFlag explanation see below (Callbackhandler activated by CSE Simulator.");
  }
  else if (port == @relais::ComPort) {
    // command to relais card was send
  }
  else 
  {
    write("### RS232 FAULT ### RS232OnError - Illegal Com Port Receive Function (Callbackhandler activated by unknown ComPort.");
  }
  
  switch (errorFlags)
  {
    case  0: 
      write("Send operation failed.");
      break;
    case  1: 
      write("Receive operation failed.");
      break;
    case  2: 
      write("Frame error. May be caused by  parity mismatch or any other frame mismatch (e.g. number of stop bits).");
      break;
    case  3: 
      write("Frame parity error. Is caused by parity mismatch.");
      break;
    case  4: 
      write("Buffer overrun. It is not specified if the driver of the sender cannot send fast enough, if it is up to the receiver which got too much data in too short time or anything else.");
      break;
    case  5: 
      write("Buffer overrun at receiver.");
      break;
    case  6: 
      write("Break state. Other end requested to pause.");
      break;  
    case  7: 
      write("Timeout. May be caused by wrongly set too short timeout. See RS232SetHandshake for setting the timeout.");
      break;  
    default:
      write("ErrorFlag unknown!");
      break;  
  } 
}


long lFunc_ConnectCSEandSetDefault(byte ConnectCSEandSetDefault_ComPort4CSE , long lv_TC_Starting_Result)
{
  
  if (lv_TC_Starting_Result == TRUE)
  {
      lv_TC_Starting_Result = gFunc_Connect_CseSimulator(ConnectCSEandSetDefault_ComPort4CSE);

      if (lv_TC_Starting_Result == TRUE)
      {
          /*
          Example:
          gFunc_Set_AdcValuesOnAllAsics(4000, 2500, EVO, 0);  //Set all cell voltages to 4V, all temp voltages to 2.5V, on project EVO, without save to flash
          gFunc_Set_AdcValuesOnAllAsics(dword Cellvoltage, dword Tempvoltage, enum g_Project_enum l_Project, int savetoflash);
          */
          switch (gTE_Set_2_Project)
          {
            case en_gProject_EVO:    // 4V CellVolt // 2,5V Vref Temp  // 1 == g_Project_enum -> EVO // 0 = No save to flash
                                  lv_TC_Starting_Result = gFunc_Set_AdcValuesOnAllAsics(4000, 2500, EVO, 0);
              break;
            case en_gProject_PLUGIN: // 4V CellVolt // 2,5V Vref Temp // 0 == g_Project_enum -> PLUGIN // 0 = No save to flash
                                  lv_TC_Starting_Result = gFunc_Set_AdcValuesOnAllAsics(3550, 2500, PLUGIN, 0);
              break;
            default:
                                  write("Error occured no known project set for CSE configuration!");
                                  lv_TC_Starting_Result = FALSE;
              break;
          }
          
          if (lv_TC_Starting_Result != TRUE)
          {
              write("Fault detected: CSE Simulator set default values procedure failed @lFunc_ConnectCSEandSetDefault()");
              testStepFail("Start TE","Fault detected: CSE Simulator set default values procedure failed @lFunc_ConnectCSEandSetDefault()"); 
          }
      }
      else 
      {
          write("Fault detected: CSE Simulator connect procedure failed @lFunc_ConnectCSEandSetDefault()");
          testStepFail("Start TE","Fault detected: CSE Simulator connect procedure failed @lFunc_ConnectCSEandSetDefault()");
      }
   }   
    
  return lv_TC_Starting_Result;
}

//testcase Starting(){
//  funcIDE_Reset();
//  funcIDE_Run();
//  TCP_Start();
// // power_BMU_without_HV(TRUE);
//}

testcase Starting()
{
  // long swVersion;  
  // char versionString[10];
  
  sysExec("k:\\General_Libs\\DeTeCAT\\TCP_Proxy\\kill_Tcp_Proxy.bat","","k:\\General_Libs\\DeTeCAT\\TCP_Proxy");
  testWaitForTimeout(g_StandardTimeout);
  sysExec("k:\\General_Libs\\DeTeCAT\\TCP_Proxy\\Start_TCP_Proxy.bat","","k:\\General_Libs\\DeTeCAT\\TCP_Proxy");
  testWaitForTimeout(1000);
  IDE = WinIdea;
  testWaitForTimeout(1000);
  TCP_Start();
  gTE_CSESimulatorState_4TE = en_gFeatureState_Automatic;
  gTE_Set_2_Project = en_gProject_PLUGIN;
  
  lFunc_ConnectCSEandSetDefault(@csesim::ComPort,TRUE);  

//  SysGetVariableString(sysvar::TestInfo::SoftwareVersion, versionString, elcount(versionString));
//  swVersion = atol(versionString);
//  if(swVersion <= 6)
//  {
////    L_MEM_HV_MEAS_THRESHOLD_U_PACK_OUT_OF_RANGE_HIGH_PARAM_VALUE = 600;
////    L_MEM_HV_MEAS_THRESHOLD_U_PACK_OUT_OF_RANGE_LOW_PARAM_VALUE = 0;
////    L_MEM_HV_MEAS_THRESHOLD_U_LINK_OUT_OF_RANGE_HIGH_PARAM_VALUE = 600;
//    // E-CLASS
//    L_MEM_HV_MEAS_THRESHOLD_U_PACK_OUT_OF_RANGE_HIGH_PARAM_VALUE = 495;
//    L_MEM_HV_MEAS_THRESHOLD_U_PACK_OUT_OF_RANGE_LOW_PARAM_VALUE = 100;
//    L_MEM_HV_MEAS_THRESHOLD_U_LINK_OUT_OF_RANGE_HIGH_PARAM_VALUE = 495;
//  }
//  else
//  {
//    L_MEM_HV_MEAS_THRESHOLD_U_PACK_OUT_OF_RANGE_HIGH_PARAM_VALUE = 495;
//    L_MEM_HV_MEAS_THRESHOLD_U_PACK_OUT_OF_RANGE_LOW_PARAM_VALUE = 100;
//    L_MEM_HV_MEAS_THRESHOLD_U_LINK_OUT_OF_RANGE_HIGH_PARAM_VALUE = 495;
//  }
}

testcase CommonSetup()
{
  /////////////////////////////////////////////////////////////////////////////////////////////////
  // Relais Card
  /////////////////////////////////////////////////////////////////////////////////////////////////
  
  /////////////////////////////////////////////////////////////////////////////////////////////////
  // Relais Card
  /////////////////////////////////////////////////////////////////////////////////////////////////
  
  // Connect Crash Signal (KL30C)
  setRelais(@relais::ComPort, @relais::card1::Id, @relais::card1::KL30C, del_rel);

  
  // Connect debugger
  setRelais(@relais::ComPort, @relais::card1::Id, @relais::card1::DebugReset, del_rel);
  setRelais(@relais::ComPort, @relais::card1::Id, @relais::card1::DebugFlash, del_rel);

  // Close HV Interlock Loops
  setRelais(@relais::ComPort, @relais::card3::Id, @relais::card3::HVILLoop1EXT, del_rel);
  setRelais(@relais::ComPort, @relais::card3::Id, @relais::card3::HVILLoop2, del_rel);
  setRelais(@relais::ComPort, @relais::card3::Id, @relais::card3::HVILLoop1INT, del_rel);
  
  // Set CMD and Data CAN to active
  setRelais(@relais::ComPort, @relais::card2::Id, @relais::card2::SC_CMD_CAN, del_rel);
  setRelais(@relais::ComPort, @relais::card2::Id, @relais::card2::MC_CMD_CAN, del_rel);
  setRelais(@relais::ComPort, @relais::card2::Id, @relais::card2::SSV_CMD_CAN, del_rel);
  setRelais(@relais::ComPort, @relais::card2::Id, @relais::card2::IMOC_CMD_CAN, del_rel);
  
  setRelais(@relais::ComPort, @relais::card2::Id, @relais::card2::SC_DATA_CAN, del_rel);
  setRelais(@relais::ComPort, @relais::card2::Id, @relais::card2::MC_DATA_CAN, del_rel);
  setRelais(@relais::ComPort, @relais::card2::Id, @relais::card2::SSV_DATA_CAN, del_rel);
  setRelais(@relais::ComPort, @relais::card2::Id, @relais::card2::IMOC_DATA_CAN, del_rel);

  // No short to ground of HV Link-Pack-Pos (ISO test case)
  setRelais(@relais::ComPort, @relais::card1::Id, @relais::card1::HVsettoGND, del_rel);
  
  // CLOSE LIN CONNECTIONS
  setRelais(@relais::ComPort, @relais::card1::Id, @relais::card1::Hvcs, del_rel);
  
  // Set Connection between Power Supply and BMU
  setRelais(@relais::ComPort, @relais::card1::Id, @relais::card1::KL30_15, set_rel);
  
}


testcase CommonCleanup()
{
  /////////////////////////////////////////////////////////////////////////////////////////////////
  // Relais Card
  /////////////////////////////////////////////////////////////////////////////////////////////////
  
  // Connect Crash Signal (KL30C)
  setRelais(@relais::ComPort, @relais::card1::Id, @relais::card1::KL30C, del_rel);
  
  // Connect debugger
  setRelais(@relais::ComPort, @relais::card1::Id, @relais::card1::DebugReset, del_rel);
  setRelais(@relais::ComPort, @relais::card1::Id, @relais::card1::DebugFlash, del_rel);

  // Close HV Interlock Loops
  setRelais(@relais::ComPort, @relais::card3::Id, @relais::card3::HVILLoop1EXT, del_rel);
  setRelais(@relais::ComPort, @relais::card3::Id, @relais::card3::HVILLoop2, del_rel);
  setRelais(@relais::ComPort, @relais::card3::Id, @relais::card3::HVILLoop1INT, del_rel);
  
  // Set CMD and Data CAN to active
  setRelais(@relais::ComPort, @relais::card2::Id, @relais::card2::SC_CMD_CAN, del_rel);
  setRelais(@relais::ComPort, @relais::card2::Id, @relais::card2::MC_CMD_CAN, del_rel);
  setRelais(@relais::ComPort, @relais::card2::Id, @relais::card2::SSV_CMD_CAN, del_rel);
  setRelais(@relais::ComPort, @relais::card2::Id, @relais::card2::IMOC_CMD_CAN, del_rel);
  
  setRelais(@relais::ComPort, @relais::card2::Id, @relais::card2::SC_DATA_CAN, del_rel);
  setRelais(@relais::ComPort, @relais::card2::Id, @relais::card2::MC_DATA_CAN, del_rel);
  setRelais(@relais::ComPort, @relais::card2::Id, @relais::card2::SSV_DATA_CAN, del_rel);
  setRelais(@relais::ComPort, @relais::card2::Id, @relais::card2::IMOC_DATA_CAN, del_rel);

  // No short to ground of HV Link-Pack-Pos (ISO test case)
  setRelais(@relais::ComPort, @relais::card1::Id, @relais::card1::HVsettoGND, del_rel);
  
  // CLOSE LIN CONNECTIONS
  setRelais(@relais::ComPort, @relais::card1::Id, @relais::card1::Hvcs, del_rel);
  // setRelais(@relais::ComPort, @relais::card2::Id, @relais::card2::LIN_SIM, del_rel);
}


testcase Power_off_BMU(){
  power_BMU_without_HV(FALSE);
  
  
  if (cpxCleanup() == 0) {
    logError("failed to close the serial connection");
  }
  // stop();
}


//testcase Stopping(){
//  testWaitForTimeout(d_StandardTimeout);
//  funcIDE_DeleteAllBP();
//  funcIDE_Reset();
//  funcIDE_Run();
//  TCPHost_stop();
//}

testcase Stopping(){
  testWaitForTimeout(d_StandardTimeout);
  funcIDE_DeleteAllBP();
  funcIDE_Reset();
  funcIDE_Run();
  TCPHost_stop();
  
  if (gTE_CSESimulatorState_4TE == en_gFeatureState_Automatic) 
  {  
      gFunc_Disconnect_CseSimulator();  // RESET CSE Simulator connection
      TestWaitForTimeout(2000); 
  }
  
  if (gTE_PSUstate_4TE == en_gFeatureState_Automatic) 
  {
//      cpxResetInit();  // Deinit CPX NT
//      sysSetVariableInt(sysvar::cpx::sys_CPX_init_performed,FALSE);
      TestWaitForTimeout(2000);
  }
}



testcase Power_on_BMU(){
   teststep("PSU", "Set Voltage and Current values for PSU and activate Power Supply");
  testWaitForTimeout(100);
  
  teststep("PSU","Init CPX in COM Port %d", @cpx::ComPort);
  
  if (cpxInit(@cpx::ComPort) == 0) {
    logError("initialization failed");
  }
  else {
    write("initialization completed");
  }
  
  testWaitForTimeout(3000);
  
  if (cpxDisableOutput(CPX_CH2) == 0)
    logError("disenabling output port failed");
  else write("disenabling output completed");
  
  testWaitForTimeout(2000);
  
  if (cpxSetOvercurrentProtection(CPX_CH2,5) == 0)
    logError("configuration of over current protection failed");
  else write("configuration of over current protection completed");
  
  if (cpxSetOvervoltageProtection(CPX_CH2,15) ==0)
    logError("configuration of over voltage protection failed");
  else write("configuration of over voltage protection completed");
  
  if (cpxSetCurrent(CPX_CH2,1.0) == 0)
    logError("configuration of maximum current failed");
  else write("configuration of over voltage protection completed");
  
  if (cpxSetVoltage(CPX_CH2,14.0) ==0)
    logError("configuration of output voltage failed");
  else testWaitForTimeout(250);
  power_BMU_without_HV(TRUE);
}

/***********************************************************************************************************
#
################################## Start of Testcases for Hgh Voltage ######################################
#
***********************************************************************************************************/


testcase ENG8_4233(){
  float expected;
  testCaseTitle("ENG8_4233","Parameter Check <L_Mem_UPackSlopeCal_Param>");
  testCaseDescription("The value of <L_Mem_UPackSlopeCal_Param> will be read by debugger. The value will be compared to the specified value/default value at the parameter specification.");
  
  // Preconditions
  func_Preconditions();
  
  // 2) Read <L_Mem_UPackSlopeCal_Param>
  testStep("ENG8_4233.2","Read <L_Mem_UPackSlopeCal_Param> \n default value: %d \n resolution: 0.0001", L_Mem_UPackSlopeCal_Param);
  expected = L_Mem_UPackSlopeCal_Param / 0.0001;
  testCaseComment("");
  func_ValueCompare(L_Mem_UPackSlopeCal_Param_NAME, funcIDE_ReadVariable(L_Mem_UPackSlopeCal_Param_BMU_VARIABLE), (long)expected, equal);
  
  // cleanUp
  func_CleanUp();
}

testcase ENG8_4234(){
  int expected;
  testCaseTitle("ENG8_4234","Parameter Check <L_Mem_UPackOffsetCal_Param>" );
  testCaseDescription("The value of <L_Mem_UPackOffsetCal_Param> will be read by debugger. The value will be compared to the specified value/default value at the parameter specification.");

    // Preconditions
  func_Preconditions();
  
  // 2) Read <L_Mem_UPackOffsetCal_Param>
  testStep("ENG8_4233.2","Read <L_Mem_UPackOffsetCal_Param> \n default value: %d \n resolution: 0.1", L_Mem_UPackOffsetCal_Param);
  expected= L_Mem_UPackOffsetCal_Param * 10;
  testCaseComment("");
  func_ValueCompare(L_Mem_UPackOffsetCal_Param_NAME, funcIDE_ReadVariable(L_Mem_UPackOffsetCal_Param_BMU_VARIABLE), expected, equal);
  
  // cleanUp
  func_CleanUp();
}

testcase ENG8_4235(){
  int expected;
  testCaseTitle("ENG8_4235","Parameter Check <L_Mem_HVMeas_ThresholdUPackOvervoltage_Param>");
  testCaseDescription("The value of <L_Mem_HVMeas_ThresholdUPackOvervoltage_Param> will be read by debugger. The value will be compared to the specified value/default value at the parameter specification.");
 
  // Preconditions
  func_Preconditions();
  
  // 2) Read <L_Mem_HVMeas_ThresholdUPackOvervoltage_Param>
  testStep("ENG8_4233.2","Read <L_Mem_HVMeas_ThresholdUPackOvervoltage_Param> \n default value: %d \n resolution: 0.025", L_Mem_HVMeas_ThresholdUPackOvervoltage_Param);
  expected= L_Mem_HVMeas_ThresholdUPackOvervoltage_Param / 0.025;
  testCaseComment("");
  func_ValueCompare(L_Mem_HVMeas_ThresholdUPackOvervoltage_Param_NAME, funcIDE_ReadVariable(L_Mem_HVMeas_ThresholdUPackOvervoltage_Param_BMU_VARIABLE), expected, equal);
  
  // cleanUp
  func_CleanUp();
}

testcase ENG8_4236(){
  int expected;
  testCaseTitle("ENG8_4236","Parameter Check <L_Mem_HVMeas_OvervoltageHysteresis_Param>");
  testCaseDescription("The value of <L_Mem_HVMeas_OvervoltageHysteresis_Param> will be read by debugger. The value will be compared to the specified value/default value at the parameter specification.");
  
  // Preconditions
  func_Preconditions();
  
  // 2) Read <L_Mem_HVMeas_OvervoltageHysteresis_Param>
  testStep("ENG8_4233.2","Read <L_Mem_HVMeas_OvervoltageHysteresis_Param> \n default value: %d \n resolution: 0,025", L_Mem_HVMeas_OvervoltageHysteresis_Param);
  expected=L_Mem_HVMeas_OvervoltageHysteresis_Param / 0.025;
  testCaseComment("");
  func_ValueCompare(L_Mem_HVMeas_OvervoltageHysteresis_Param_NAME, funcIDE_ReadVariable(L_Mem_HVMeas_OvervoltageHysteresis_Param_BMU_VARIABLE), expected, equal);
  
  // cleanUp
  func_CleanUp();
}

testcase ENG8_4237()
{
  float expected;
  testCaseTitle("ENG8_4237","Parameter Check <L_Mem_HVMeas_ThresholdUPackOutOfRangeHigh_Param>");
  testCaseDescription("The value of <L_Mem_HVMeas_ThresholdUPackOutOfRangeHigh_Param> will be read by debugger. The value will be compared to the specified value/default value at the parameter specification.");

  // Preconditions
  func_Preconditions();
  
  // 2) Read <L_Mem_HVMeas_ThresholdUPackOutOfRangeHigh_Param>
  testStep("ENG8_4233.2","Read <L_Mem_HVMeas_ThresholdUPackOutOfRangeHigh_Param> \n default value: %d \n resolution: 0.025", L_Mem_HVMeas_ThresholdUPackOutOfRangeHigh_Param);
  expected = L_Mem_HVMeas_ThresholdUPackOutOfRangeHigh_Param / 0.025;
  testCaseComment("");
  func_ValueCompare(L_Mem_HVMeas_ThresholdUPackOutOfRangeHigh_Param_NAME, funcIDE_ReadVariable(L_Mem_HVMeas_ThresholdUPackOutOfRangeHigh_Param_BMU_VARIABLE), (long)expected, equal);
  
  // cleanUp
  func_CleanUp();
}

testcase ENG8_4241(){
  int expected;
  testCaseTitle("ENG8_4241","Parameter Check <L_Mem_HVMeas_ThresholdUPackOutOfRangeLow_Param>");
  testCaseDescription("The value of <L_Mem_HVMeas_ThresholdUPackOutOfRangeLow_Param> will be read by debugger. The value will be compared to the specified value/default value at the parameter specification.");

  // Preconditions
  func_Preconditions();
  
  // 2) Read <L_Mem_HVMeas_ThresholdUPackOutOfRangeLow_Param>
  testStep("ENG8_4233.2","Read <L_Mem_HVMeas_ThresholdUPackOutOfRangeLow_Param> \n default value: %d \n resolution: 1", L_Mem_HVMeas_ThresholdUPackOutOfRangeLow_Param);
  expected = L_Mem_HVMeas_ThresholdUPackOutOfRangeLow_Param / 0.025;
  testCaseComment("");
  func_ValueCompare(L_Mem_HVMeas_ThresholdUPackOutOfRangeLow_Param_NAME, funcIDE_ReadVariable(L_Mem_HVMeas_ThresholdUPackOutOfRangeLow_Param_BMU_VARIABLE), expected, equal);
  
  // cleanUp
  func_CleanUp();
}

testcase ENG8_4238(){
  int expected;
  testCaseTitle("ENG8_4238","Parameter Check <L_Mem_HVMeas_UPackDiffMax_Param>");
  testCaseDescription("The value of <L_Mem_HVMeas_UPackDiffMax_Param> will be read by debugger. The value will be compared to the specified value/default value at the parameter specification.");

  // Preconditions
  func_Preconditions();
  
  // 2) Read <L_Mem_HVMeas_UPackDiffMax_Param>
  testStep("ENG8_4233.2","Read <L_Mem_HVMeas_UPackDiffMax_Param> \n default value: %d \n resolution: 0.025", L_Mem_HVMeas_UPackDiffMax_Param);
  expected = L_Mem_HVMeas_UPackDiffMax_Param / 0.025;
  testCaseComment("");
  func_ValueCompare(L_Mem_HVMeas_UPackDiffMax_Param_NAME, funcIDE_ReadVariable(L_Mem_HVMeas_UPackDiffMax_Param_BMU_VARIABLE), expected, equal);
  
  // cleanUp
  func_CleanUp();
}

testcase ENG8_4239(){
  int expected;
  testCaseTitle("ENG8_4239","Parameter Check <L_Mem_HVMeas_Timeout_Param>");
  testCaseDescription("The value of <L_Mem_HVMeas_Timeout_Param> will be read by debugger. The value will be compared to the specified value/default value at the parameter specification.");

  // Preconditions
  func_Preconditions();
  
  // 2) Read <L_Mem_HVMeas_Timeout_Param>
  testStep("ENG8_4233.2","Read <L_Mem_HVMeas_Timeout_Param> \n default value: %d \n resolution: 10", L_Mem_HVMeas_Timeout_Param);
  expected = L_Mem_HVMeas_Timeout_Param / 10;
  testCaseComment("");
  func_ValueCompare(L_Mem_HVMeas_Timeout_Param_NAME, funcIDE_ReadVariable(L_Mem_HVMeas_Timeout_Param_BMU_VARIABLE), expected, equal);
  
  // cleanUp
  func_CleanUp();
}

testcase ENG8_4240(){
  float expected;
  testCaseTitle("ENG8_4240","Parameter Check <L_Mem_ULinkSlopeCal_Param>");
  testCaseDescription("The value of <L_Mem_ULinkSlopeCal_Param> will be read by debugger. The value will be compared to the specified value/default value at the parameter specification.");

  // Preconditions
  func_Preconditions();
  
  // 2) Read <L_Mem_ULinkSlopeCal_Param>
  testStep("ENG8_4233.2","Read <L_Mem_ULinkSlopeCal_Param> \n default value: %d \n resolution: 0.0001", L_Mem_ULinkSlopeCal_Param);
  expected = L_Mem_ULinkSlopeCal_Param / 0.0001;
  testCaseComment("");
  func_ValueCompare(L_Mem_ULinkSlopeCal_Param_NAME, funcIDE_ReadVariable(L_Mem_ULinkSlopeCal_Param_BMU_VARIABLE), (long)expected, equal);
  
  // cleanUp
  func_CleanUp();
}

testcase ENG8_4242(){
  int expected;
  testCaseTitle("ENG8_4242","Parameter Check  <L_Mem_ULinkOffsetCal_Param>");
  testCaseDescription("The value of  <L_Mem_ULinkOffsetCal_Param> will be read by debugger. The value will be compared to the specified value/default value at the parameter specification.");

  // Preconditions
  func_Preconditions();
  
  // 2) Read <L_Mem_ULinkOffsetCal_Param>
  testStep("ENG8_4233.2","Read <L_Mem_ULinkOffsetCal_Param> \n default value: %d \n resolution: 0.1", L_Mem_ULinkOffsetCal_Param);
  expected = L_Mem_ULinkOffsetCal_Param /0.1;
  testCaseComment("");
  func_ValueCompare(L_Mem_ULinkOffsetCal_Param_NAME, funcIDE_ReadVariable(L_Mem_ULinkOffsetCal_Param_BMU_VARIABLE), expected, equal);
  
  // cleanUp
  func_CleanUp();
}

testcase ENG8_4243(){
  int expected;
  testCaseTitle("ENG8_4243","Parameter Check <L_Mem_HVMeas_ThresholdLinkUOvervoltage_Param>");
  testCaseDescription("The value of <L_Mem_HVMeas_ThresholdLinkUOvervoltage_Param> will be read by debugger. The value will be compared to the specified value/default value at the parameter specification.");

  // Preconditions
  func_Preconditions();
  
  // 2) Read <L_Mem_HVMeas_ThresholdLinkUOvervoltage_Param>
  testStep("ENG8_4233.2","Read <L_Mem_HVMeas_ThresholdLinkUOvervoltage_Param> \n default value: %d \n resolution: 0.025", L_Mem_HVMeas_ThresholdULinkOvervoltage_Param);
  expected = L_Mem_HVMeas_ThresholdULinkOvervoltage_Param / 0.025;
  testCaseComment("");
  func_ValueCompare(L_Mem_HVMeas_ThresholdULinkOvervoltage_Param_NAME, funcIDE_ReadVariable(L_Mem_HVMeas_ThresholdULinkOvervoltage_Param_BMU_VARIABLE), expected, equal);
  
  // cleanUp
  func_CleanUp();
}

testcase ENG8_4244(){
  float expected;
  testCaseTitle("ENG8_4244","Parameter Check <L_Mem_HVMeas_ThresholdULinkOutOfRangeHigh_Param>");
  testCaseDescription("The value of <L_Mem_HVMeas_ThresholdULinkOutOfRangeHigh_Param> will be read by debugger. The value will be compared to the specified value/default value at the parameter specification.");

  // Preconditions
  func_Preconditions();
  
  // 2) Read <L_Mem_HVMeas_ThresholdLinkUOutOfRangeHigh_Param>
  testStep("ENG8_4233.2","Read <L_Mem_HVMeas_ThresholdLinkUOutOfRangeHigh_Param> \n default value: %d \n resolution: 0.025", L_Mem_HVMeas_ThresholdULinkOutOfRangeHigh_Param);
  expected = L_Mem_HVMeas_ThresholdULinkOutOfRangeHigh_Param / 0.025;
  testCaseComment("");
  func_ValueCompare(L_Mem_HVMeas_ThresholdULinkOutOfRangeHigh_Param_NAME, funcIDE_ReadVariable(L_Mem_HVMeas_ThresholdULinkOutOfRangeHigh_Param_BMU_VARIABLE), (long)expected, equal);
  
  // cleanUp
  func_CleanUp();
}

testcase ENG8_13969(){
  float expected;
  testCaseTitle("ENG8_13969","Parameter Check <L_Mem_HVMeas_ThresholdULinkOutOfRangeLow_Param>");
  testCaseDescription("The value of <L_Mem_HVMeas_ThresholdULinkOutOfRangeLow_Param> will be read by debugger. The value will be compared to the specified value/default value at the parameter specification.");

  // Preconditions
  func_Preconditions();
  
  // 2) Read <L_Mem_HVMeas_PackLink_Param>
  testStep("ENG8_13969.2","Read <L_Mem_HVMeas_ThresholdULinkOutOfRangeLow_Param> \n default value: %d \n resolution: 0.025", L_Mem_HVMeas_ThresholdULinkOutOfRangeLow_Param);
  expected = L_Mem_HVMeas_ThresholdULinkOutOfRangeLow_Param / 0.025;
  testCaseComment("");
  func_ValueCompare(L_Mem_HVMeas_ThresholdULinkOutOfRangeLow_Param_NAME, funcIDE_ReadVariable(L_Mem_HVMeas_ThresholdULinkOutOfRangeLow_Param_BMU_VARIABLE), (long)expected, equal);
  
  // cleanUp
  func_CleanUp();
}

testcase ENG8_4245(){
  int expected;
  testCaseTitle("ENG8_4245","Parameter Check <L_Mem_HVMeas_ULinkDiffMax_Param>");
  testCaseDescription("The value of <L_Mem_HVMeas_ULinkDiffMax_Param> will be read by debugger. The value will be compared to the specified value/default value at the parameter specification.");

  // Preconditions
  func_Preconditions();
  
  // 2) Read <L_Mem_HVMeas_ULinkDiffMax_Param>
  testStep("ENG8_4233.2","Read <L_Mem_HVMeas_ULinkDiffMax_Param> \n default value: %d \n resolution: 0.025", L_Mem_HVMeas_ULinkDiffMax_Param);
  expected = L_Mem_HVMeas_ULinkDiffMax_Param /0.025;
  testCaseComment("");
  func_ValueCompare(L_Mem_HVMeas_ULinkDiffMax_Param_NAME, funcIDE_ReadVariable(L_Mem_HVMeas_ULinkDiffMax_Param_BMU_VARIABLE), expected, equal);
  
  // cleanUp
  func_CleanUp();
}

testcase ENG8_4246(){
  int expected;
  testCaseTitle("ENG8_4246","Parameter Check <L_Mem_HVMeas_PackLink_Param>");
  testCaseDescription("The value of <L_Mem_HVMeas_PackLink_Param> will be read by debugger. The value will be compared to the specified value/default value at the parameter specification.");

  // Preconditions
  func_Preconditions();
  
  // 2) Read <L_Mem_HVMeas_PackLink_Param>
  testStep("ENG8_4233.2","Read <L_Mem_HVMeas_PackLink_Param> \n default value: %d \n resolution: 0.025", L_Mem_HVMeas_PackLink_Param);
  expected = L_Mem_HVMeas_PackLink_Param / 0.025;
  testCaseComment("");
  func_ValueCompare(L_Mem_HVMeas_PackLink_Param_NAME, funcIDE_ReadVariable(L_Mem_HVMeas_PackLink_Param_BMU_VARIABLE), expected, equal);
  
  // cleanUp
  func_CleanUp();
}

/*************************************************************
*################# Pack Voltage Measurement ##################
**************************************************************/
testcase ENG8_4273 (){
  
  char inFunction[200] = "CtDaqMgr_HighVoltages";
  char inLine1[200] = "UOut_S16 = CtDaqMgr_HvVoltageFiltered(CtDaqMgr_ULink_S16, &tagISV_SHvDaq28_FilterLinkVoltage);";
  char inLine2[200] = "CtDaqMgr_ULinkFiltered_S16 = UOut_S16;";
  
  int invalid;
  
  long ULink_out;
  
  testCaseTitle("ENG8_4273","ULink value for external transmission - function execution");
  testCaseDescription("A breakpoint is set at the beginning of the function that calculates the average value of two consecutively measured link voltage values. After BMU start, when the SW initialization is completed, the link voltage state is set to /Valid/. It is checked whether the BMU is halted at the previously set breakpoint.");
  
  /* BP to set in function: CtDaqMgr_HighVoltages
    CtDaqMgr_ULinkFiltered_S16 = UOut_S16; <- maybe this must be overwritten
  */
   // Preconditions
  func_Preconditions();
  
  // 1) Set breakpoint
  teststep("","");
  testStep("ENG8_4273.1","Set a breakpoint at the beginning of the function that calculates the average value of two consecutively measured link voltage values.\n in Function: CtDaqMgr_HighVoltages \n in Line 1: UOut_S16_a = CtDaqMgr_HvVoltageFiltered(CtDaqMgr_ULink_S16, &tagISV_SHvDaq28_FilterLinkVoltage);\n in Line 2: CtDaqMgr_ULinkFiltered_S16 = UOut_S16_a;");
  funcWinIdea_SetBPSource(inFunction, inLine1);
  //funcWinIdea_SetBPSource(inFunction, inLine2);
  teststep("","");
  funcIDE_WaitForHalt(1000);
  teststep("","");
  
  // 2) Verify ULink_State.Invalid = /false/
  testStep("ENG8_4273.2","Verify ULink_State.Invalid = /false/");
  invalid = funcIDE_REadVariable("((Rte_CpApHvDaqMgr_PpCtApDaqMgrULink_ULink).State).bInvalid");
  func_ValueCompare( "ULink_State.Invalid" , invalid , FALSE , equal);
  teststep("","");

  // 4 ) run target till next stop at breakpoint
  teststep("ENG8_4273.4","Run target till next stop at breakpoint");
  funcIDE_RunAndWait(1000);
  teststep("","");
  
  //5) During halt at breakpoint, overwrite <P_HVMeas_ULink>^n = 155V - <P_HVMeas_ULink>^n-1 = 157V
  teststep("ENG8_4273.5a","Overwrite ULink to 155V");
  funcIDE_WriteVariable("CtDaqMgr_ULink_S16", 155);
  funcIDE_RunAndWait(2000);
  teststep("","");
  teststep("ENG8_4273.5b","Overwrite ULink to 157V");
  funcIDE_WriteVariable("CtDaqMgr_ULink_S16", 157);
  teststep("",""); 
  
  // 6) Set breakpoint one step further and check for averaged UPack^ext value
  teststep("ENG8_4273.6","Set breakpoint one step further and check for averaged ULink^ext value");
  funcWinIdea_SetBPSource(inFunction, inLine2);
  funcIDE_RunAndWait(2000);
  teststep("","");
  ULink_out = funcIDE_ReadVariable("UOut_S16");
  func_ValueCompare( "<P_HVMeas_ULink>^ext" , ULink_out , 156 , equal);
  
  func_CleanUp();
}

/*************************************************************
*############## Pack Voltage Plausibility Check ##############
**************************************************************/

testcase ENG8_4313()
{
 // int i, UPackDiffMax;
  int i;
  
  testCaseTitle("ENG8_4313","UPack - plausibility failed");
  testCaseDescription("After BMU start the UPack plausibility error is initially checked to be unequal to failed. UPack is set to a value greater than the sum of the cell voltages plus the maximum difference tolerance. The UPack plausibility error is observed to switch to failed.");
  
  // preconditions
  func_Preconditions();

  // Check <ErrQ_HVMeas_UPackPlausibility>
  func_ValueCompare("<ErrQ_HVMeas_UPackPlausibility>", getsignal(CAN2::SC_ERRORS_DAQ::ERR_UPack_Plaus), RUNNING_FAILED, not_equal);

  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "bDebOver_B_a = XcpSw_IsPackSna_B ^ bForceInv_B_a;");
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "CtDaqMgr_AddInfo_U8[4] = (UInt8) (((uint16) CtDaqMgr_UPack_S16) >> 8);");
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "UDAbs_S16 = (sint16) (CtDaqMgr_UPack_S16 - UCellSum_a.Value);");
  funcIDE_WaitForHalt(2000);

  for(i = 0; i < 55; i++)
  {
    // Set <L_CellVoltageMgr_CellVoltageSum> = 370V
    funcIDE_WriteVariable("UCellSum_a.Value", 370 * 40);
    funcIDE_WriteVariable("UCellSum_a.State.bInitDone", 1);
    funcIDE_WriteVariable("UCellSum_a.State.bInvalid", 0);
    
    funcIDE_RunAndWait(2000);
    
    // Set <P_HVMeas_UPack> <iL_HVMeas_UPack> =  <L_Mem_HVMeas_UPackDiffMax_Param> + <L_CellVoltageMgr_CellVoltageSum> + 1 unit
    funcIDE_WriteVariable("CtDaqMgr_UPack_S16", (370 + L_Mem_HVMeas_UPackDiffMax_Param + 1) * 40);
    
    funcIDE_RunAndWait(2000);

    funcIDE_WriteVariable("UPack.State.bInvalid", 0);

    funcIDE_RunAndWait(2000);
  }
  
  funcIDE_RunAndWait(2000);
  
  // Check <ErrQ_HVMeas_UPackPlausibility>
  func_ValueCompare("<ErrQ_HVMeas_UPackPlausibility>", getsignal(CAN2::SC_ERRORS_DAQ::ERR_UPack_Plaus), RUNNING_FAILED, equal);
  
  //cleanUp
  trickMemoryAfter();
  func_CleanUp();
}

testcase ENG8_4314()
{
  int i;
  
  testCaseTitle("ENG8_4314","UPack - plausibility passed");
  testCaseDescription("After BMU start the UPack plausibility error is initially checked to be unequal to passed. UPack is set to a value equal to the sum of the cell voltages plus the maximum difference tolerance. The UPack plausibility error is observed to switch to passed.");
  
  // preconditions
  func_Preconditions();

  // Check <ErrQ_HVMeas_UPackPlausibility>
  func_ValueCompare("<ErrQ_HVMeas_UPackPlausibility>", getsignal(CAN2::SC_ERRORS_DAQ::ERR_UPack_Plaus), RUNNING_PASSED, not_equal);

  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "bDebOver_B_a = XcpSw_IsPackSna_B ^ bForceInv_B_a;");
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "CtDaqMgr_AddInfo_U8[4] = (UInt8) (((uint16) CtDaqMgr_UPack_S16) >> 8);");
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "UDAbs_S16 = (sint16) (CtDaqMgr_UPack_S16 - UCellSum_a.Value);");
  funcIDE_WaitForHalt(2000);

  for(i = 0; i < 55; i++)
  {
    // Set <L_CellVoltageMgr_CellVoltageSum> = 370V
    funcIDE_WriteVariable("UCellSum_a.Value", 370 * 40);
    funcIDE_WriteVariable("UCellSum_a.State.bInitDone", 1);
    funcIDE_WriteVariable("UCellSum_a.State.bInvalid", 0);
    
    funcIDE_RunAndWait(2000);
    
    // Set <P_HVMeas_UPack> <iL_HVMeas_UPack> =  <L_Mem_HVMeas_UPackDiffMax_Param> + <L_CellVoltageMgr_CellVoltageSum> + 1 unit
    funcIDE_WriteVariable("CtDaqMgr_UPack_S16", (370 + L_Mem_HVMeas_UPackDiffMax_Param) * 40);
    
    funcIDE_RunAndWait(2000);

    funcIDE_WriteVariable("UPack.State.bInvalid", 0);

    funcIDE_RunAndWait(2000);
  }

  funcIDE_RunAndWait(2000);
  
  // Check <ErrQ_HVMeas_UPackPlausibility>
  func_ValueCompare("<ErrQ_HVMeas_UPackPlausibility>", getsignal(CAN2::SC_ERRORS_DAQ::ERR_UPack_Plaus), RUNNING_PASSED, equal);

  //cleanUp
  trickMemoryAfter();
  func_CleanUp();
}

/*************************************************************
*############## Link Voltage Plausibility Check ##############
**************************************************************/

testcase ENG8_4322()
{
  int i, ULinkDiffMax;
  
  testCaseTitle("ENG8_4322","ULink - plausibility failed");
  testCaseDescription("After BMU start the ULink plausibility error is initially checked to be unequal to failed. The main contactors are set to be closed. ULink is set to a value greater than the sum of the cell voltages plus the maximum difference tolerance. The ULink plausibility error is observed to switch to failed.");
  
  // preconditions
  func_Preconditions();
  ULinkDiffMax = funcIDE_ReadVariable("HvDaqMgrParameter.p_ULinkDiffMax");
  ULinkDiffMax = (int)((float)ULinkDiffMax*0.025);
  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1); // activation
  // Set <P_HVMeas_ULink> <iL_HVMeas_ULink> =  <L_Mem_HVMeas_ULinkDiffMax_Param> + <L_CellVoltageMgr_CellVoltageSum>
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", (370 + ULinkDiffMax) * 40);
  trickMemoryAfter();

  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "bDebOver_B_a = XcpSw_IsPackSna_B ^ bForceInv_B_a;");
  funcIDE_WaitForHalt(2000);

  funcIDE_WriteVariable ("ContDiag_Xcp_ContMainPos_State_out_ACT_U8", 170);
  funcIDE_WriteVariable ("ContDiag_Xcp_ContMainPos_State_out_VAL_U8", ENABLE_Cont);
  funcIDE_WriteVariable ("ContDiag_Xcp_ContMainNeg_State_out_ACT_U8", 170);
  funcIDE_WriteVariable ("ContDiag_Xcp_ContMainNeg_State_out_VAL_U8", ENABLE_Cont);

  for(i = 0; i < 55; i++)
  {
    // Set <L_CellVoltageMgr_CellVoltageSum> = 370V
    funcIDE_WriteVariable("UCellSum_a.Value", 370 * 40);
    funcIDE_WriteVariable("UCellSum_a.State.bInitDone", 1);
    funcIDE_WriteVariable("UCellSum_a.State.bInvalid", 0);
    
    funcIDE_RunAndWait(2000);    
  }

  funcIDE_RunAndWait(2000);
  
  // Check <ErrQ_HVMeas_ULinkPlausibility>
  func_ValueCompare("<ErrQ_HVMeas_ULinkPlausibility>", getsignal(CAN2::SC_ERRORS_DAQ::ERR_ULink_Plaus), RUNNING_FAILED, not_equal);
  // Set <P_HVMeas_ULink> <iL_HVMeas_ULink> =  <L_Mem_HVMeas_ULinkDiffMax_Param> + <L_CellVoltageMgr_CellVoltageSum> + 1 unit
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", (370 + ULinkDiffMax + 1) * 40);

  for(i = 0; i < 55; i++)
  {
    // Set <L_CellVoltageMgr_CellVoltageSum> = 370V
    funcIDE_WriteVariable("UCellSum_a.Value", 370 * 40);
    funcIDE_WriteVariable("UCellSum_a.State.bInitDone", 1);
    funcIDE_WriteVariable("UCellSum_a.State.bInvalid", 0);
    
    funcIDE_RunAndWait(2000);    
  }

  funcIDE_RunAndWait(2000);
  
  // Check <ErrQ_HVMeas_ULinkPlausibility>
  func_ValueCompare("<ErrQ_HVMeas_ULinkPlausibility>", getsignal(CAN2::SC_ERRORS_DAQ::ERR_ULink_Plaus), RUNNING_FAILED, equal);

  //cleanUp
  trickMemoryAfter();
  func_CleanUp();
}

testcase ENG8_4325()
{
  int i=0, ULinkDiffMax;
  
  testCaseTitle("ENG8_4325","ULink - plausibility passed");
  testCaseDescription("After BMU start the ULink plausibility error is initially checked to be unequal to failed. The main contactors are set to be closed. ULink is set to a value equal to the sum of the cell voltages plus the maximum difference tolerance. The ULink plausibility error is observed to switch to passed.");
  
  // preconditions
  func_Preconditions();
  ULinkDiffMax = funcIDE_ReadVariable("HvDaqMgrParameter.p_ULinkDiffMax");
  ULinkDiffMax = (int)((float)ULinkDiffMax*0.025);
  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1); // activation
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", (370 + ULinkDiffMax) * 40);
  trickMemoryAfter();

  // Check <ErrQ_HVMeas_ULinkPlausibility>
  func_ValueCompare("<ErrQ_HVMeas_ULinkPlausibility>", getsignal(CAN2::SC_ERRORS_DAQ::ERR_ULink_Plaus), RUNNING_PASSED, not_equal);

  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "bDebOver_B_a = XcpSw_IsPackSna_B ^ bForceInv_B_a;");
  funcIDE_WaitForHalt(2000);

  funcIDE_WriteVariable ("ContDiag_Xcp_ContMainPos_State_out_ACT_U8", 170);
  funcIDE_WriteVariable ("ContDiag_Xcp_ContMainPos_State_out_VAL_U8", ENABLE_Cont);
  funcIDE_WriteVariable ("ContDiag_Xcp_ContMainNeg_State_out_ACT_U8", 170);
  funcIDE_WriteVariable ("ContDiag_Xcp_ContMainNeg_State_out_VAL_U8", ENABLE_Cont);

  do
  {
    // Set <L_CellVoltageMgr_CellVoltageSum> = 370V
    funcIDE_WriteVariable("UCellSum_a.Value", 370 * 40);
    funcIDE_WriteVariable("UCellSum_a.State.bInitDone", 1);
    funcIDE_WriteVariable("UCellSum_a.State.bInvalid", 0);
    
    funcIDE_RunAndWait(2000);
    
    i = i + 1;
  } while ( (i<55) &&  (getsignal(CAN2::SC_ERRORS_DAQ::ERR_ULink_Plaus)!=RUNNING_PASSED) );
    
  // Check <ErrQ_HVMeas_ULinkPlausibility>
  func_ValueCompare("<ErrQ_HVMeas_ULinkPlausibility>", getsignal(CAN2::SC_ERRORS_DAQ::ERR_ULink_Plaus), RUNNING_PASSED, equal);

  //cleanUp
  trickMemoryAfter();
  func_CleanUp();
}

testcase ENG8_4327()
{
  int i, ULinkDiffMax;
  
  testCaseTitle("ENG8_4327","ULink - plausibility with open contactors and fault conditions");
  testCaseDescription("After BMU start the ULink plausibility error is initially checked to be unequal to failed. The main contactors are set to be open. ULink is set to a value greater than the sum of the cell voltages plus the maximum difference tolerance. The ULink plausibility error is observed to remain unchanged.");
  
  // preconditions
  func_Preconditions();
  Func_ContCtrl_MainPos(DISABLE_Cont);
  Func_ContCtrl_MainNeg(DISABLE_Cont);
  ULinkDiffMax = funcIDE_ReadVariable("HvDaqMgrParameter.p_ULinkDiffMax");
  ULinkDiffMax = (int)((float)ULinkDiffMax*0.025);
  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1); // activation
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", (370 + ULinkDiffMax + 1) * 40);
  trickMemoryAfter();

  // Check <ErrQ_HVMeas_ULinkPlausibility>
  func_ValueCompare("<ErrQ_HVMeas_ULinkPlausibility>", getsignal(CAN2::SC_ERRORS_DAQ::ERR_ULink_Plaus), RUNNING_FAILED, not_equal);

  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "bDebOver_B_a = XcpSw_IsPackSna_B ^ bForceInv_B_a;");
  funcIDE_WaitForHalt(2000);

  for(i = 0; i < 55; i++)
  {
    // Set <L_CellVoltageMgr_CellVoltageSum> = 370V
    funcIDE_WriteVariable("UCellSum_a.Value", 370 * 40);
    funcIDE_WriteVariable("UCellSum_a.State.bInitDone", 1);
    funcIDE_WriteVariable("UCellSum_a.State.bInvalid", 0);
    
    funcIDE_RunAndWait(2000);
  }
  
  funcIDE_RunAndWait(2000);
  
  // Check <ErrQ_HVMeas_ULinkPlausibility>
  func_ValueCompare("<ErrQ_HVMeas_ULinkPlausibility>", getsignal(CAN2::SC_ERRORS_DAQ::ERR_ULink_Plaus), RUNNING_FAILED, not_equal);

  //cleanUp
  trickMemoryAfter();
  func_CleanUp();
}

testcase ENG8_4329()
{
  int i, ULinkDiffMax;
  
  testCaseTitle("ENG8_4329","ULink - plausibility with open contactors and no fault condition");
  testCaseDescription("After BMU start the ULink plausibility error is initially checked to be unequal to passed. The main contactors are set to be open. ULink is set to a value equal to the sum of the cell voltages plus the maximum difference tolerance. The ULink plausibility error is observed to remain unchanged.");
  
  // preconditions
  func_Preconditions();
  Func_ContCtrl_MainPos(DISABLE_Cont);
  Func_ContCtrl_MainNeg(DISABLE_Cont);
  ULinkDiffMax = funcIDE_ReadVariable("HvDaqMgrParameter.p_ULinkDiffMax");
  ULinkDiffMax = (int)((float)ULinkDiffMax*0.025);
  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1); // activation
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", (370 + ULinkDiffMax) * 40);
  trickMemoryAfter();

  // Check <ErrQ_HVMeas_ULinkPlausibility>
  func_ValueCompare("<ErrQ_HVMeas_ULinkPlausibility>", getsignal(CAN2::SC_ERRORS_DAQ::ERR_ULink_Plaus), RUNNING_PASSED, not_equal);

  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "bDebOver_B_a = XcpSw_IsPackSna_B ^ bForceInv_B_a;");
  funcIDE_WaitForHalt(2000);

  for(i = 0; i < 51; i++)
  {
    // Set <L_CellVoltageMgr_CellVoltageSum> = 370V
    funcIDE_WriteVariable("UCellSum_a.Value", 370 * 40);
    funcIDE_WriteVariable("UCellSum_a.State.bInitDone", 1);
    funcIDE_WriteVariable("UCellSum_a.State.bInvalid", 0);
    
    funcIDE_RunAndWait(2000);
  }

  funcIDE_RunAndWait(2000);
  
  // Check <ErrQ_HVMeas_ULinkPlausibility>
  func_ValueCompare("<ErrQ_HVMeas_ULinkPlausibility>", getsignal(CAN2::SC_ERRORS_DAQ::ERR_ULink_Plaus), RUNNING_PASSED, not_equal);

  //cleanUp
  trickMemoryAfter();
  func_CleanUp();
}

testcase ENG8_4330 ()
{
  float newMeasValue, uPack;
  
  testCaseTitle("ENG8_4330","Pack-Link plausibility failed");
  testCaseDescription("After BMU start the Pack-Link plausibility error is initally checked to be unequal to failed. The main contactors are set to be closed. ULink is set to value greater than the sum of UPack and the max. allowed Pack-Link voltage difference. The Pack-Link plausibility error is observed to switch to failed.");
  
    //preconditions
  func_Preconditions();
  teststep("","");

  //2) Check <ErrQ_HVMeas_PackLinkPlausibility>
  testStep("ENG8_4330.2","Check <ErrQ_HVMeas_PackLinkPlausibility>");
  testCaseComment("");
  func_ValueCompare(ERR_Q_HV_MEAS_U_PACK_LINK_PLAUSIBILITY_NAME, getSignal(CAN2::SC_ERRORS_DAQ::ERR_UPackLink_Plaus),RUNNING_FAILED, not_equal );
  
  // 3) ) Set <L_ContCtrl_HVBatteryConnection_State> = /Connected/ <P_HVMeas_ULink> = <L_Mem_HVMeas_PackLink_Param> + <P_HVMeas_UPack> + 1 unit
  teststep("ENG8_4330.3","3) ) Set <L_ContCtrl_HVBatteryConnection_State> = /Connected/ <P_HVMeas_ULink> = <L_Mem_HVMeas_PackLink_Param> + <P_HVMeas_UPack> + 1 unit");
  testCaseComment("");  
  trickMemoryBefore();
  testStep("ENG8_4330.3a","Set HV contactor state to /Connected/.");
  funcIDE_WriteVariable("ContDiag_Xcp_ContMainPos_State_out_ACT_U8", 170);
  funcIDE_WriteVariable("ContDiag_Xcp_ContMainPos_State_out_VAL_U8", 2);
  funcIDE_WriteVariable("ContDiag_Xcp_ContMainNeg_State_out_ACT_U8", 170);
  funcIDE_WriteVariable("ContDiag_Xcp_ContMainNeg_State_out_VAL_U8", 2);
  trickMemoryAfter();
  
  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 355 * 40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", (L_Mem_HVMeas_PackLink_Param + 355 + 1) / 0.025);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
  trickMemoryAfter();
  
  testWaitForTimeout(10000);
  
  //4) Check <ErrQ_HVMeas_PackLinkPlausibility>
  testStep("ENG8_4330.4","Check <ErrQ_HVMeas_PackLinkPlausibility>");
  testCaseComment("");
  func_ValueCompare(ERR_Q_HV_MEAS_U_PACK_LINK_PLAUSIBILITY_NAME, getSignal(CAN2::SC_ERRORS_DAQ::ERR_UPackLink_Plaus),RUNNING_FAILED, equal );
  
  //cleanUp()
  func_CleanUp();
  
}

testcase ENG8_4349 ()
{
  float newMeasValue, uPack;
  
  testCaseTitle("ENG8_4349","Pack-Link plausibility - with open contactors and fault conditions");
  testCaseDescription("After BMU start the Pack-Link plausibility error is initally checked to be unequal to failed. The main contactors are set to be open. ULink is set to value greater than the sum of UPack and the max. allowed Pack-Link voltage difference. The Pack-Link plausibility error is observed to switch to remain unchanged.");
  
    //preconditions
  func_Preconditions();
  teststep("","");

  //2) Check <ErrQ_HVMeas_PackLinkPlausibility>
  testStep("ENG8_4349.2","Check <ErrQ_HVMeas_PackLinkPlausibility>");
  testCaseComment("");
  func_ValueCompare(ERR_Q_HV_MEAS_U_PACK_LINK_PLAUSIBILITY_NAME, getSignal(CAN2::SC_ERRORS_DAQ::ERR_UPackLink_Plaus),RUNNING_FAILED, not_equal );
  
  // 3) ) Set <L_ContCtrl_HVBatteryConnection_State> = /Disconnected/ <P_HVMeas_ULink> = <L_Mem_HVMeas_PackLink_Param> + <P_HVMeas_UPack> + 1 unit
  teststep("ENG8_4349.3","3) ) Set <L_ContCtrl_HVBatteryConnection_State> = /Connected/ <P_HVMeas_ULink> = <L_Mem_HVMeas_PackLink_Param> + <P_HVMeas_UPack> + 1 unit");
  testCaseComment("");  

  func_ValueCompare("<L_ContCtrl_ContMainPos_State>", getsignal(CAN2::SC_STATES_01::STAT_CONTACTOR_PLUS), CNT_OPEN, equal);
  func_ValueCompare("<L_ContCtrl_ContMainNeg_State>", getsignal(CAN2::SC_STATES_01::STAT_CONTACTOR_MINUS), CNT_OPEN, equal);
  
  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 355 * 40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", (L_Mem_HVMeas_PackLink_Param + 355 + 1) * 40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
  trickMemoryAfter();
  testWaitForTimeout(10000);
  
  //4) Check <ErrQ_HVMeas_PackLinkPlausibility>
  testStep("ENG8_4349.4","Check <ErrQ_HVMeas_PackLinkPlausibility>");
  testCaseComment("");
  func_ValueCompare(ERR_Q_HV_MEAS_U_PACK_LINK_PLAUSIBILITY_NAME, getSignal(CAN2::SC_ERRORS_DAQ::ERR_UPackLink_Plaus),RUNNING_FAILED, not_equal );
  
  //cleanUp()
  func_CleanUp();
  
}

testcase ENG8_4351 ()
{
  int UPackLinkDiffMax;
  float uPack;
  
  testCaseTitle("ENG8_4351","Pack-Link plausibility passed");
  testCaseDescription("");
  
    //preconditions
  func_Preconditions();
  UPackLinkDiffMax = funcIDE_ReadVariable("HvDaqMgrParameter.p_UPackLinkDiffMax");
  UPackLinkDiffMax = (int)((float)UPackLinkDiffMax*0.025);
  teststep("","");

  //2) Check <ErrQ_HVMeas_PackLinkPlausibility>
  testStep("ENG8_4351.2","Check <ErrQ_HVMeas_PackLinkPlausibility>");
  testCaseComment("");
  func_ValueCompare(ERR_Q_HV_MEAS_U_PACK_LINK_PLAUSIBILITY_NAME, getSignal(CAN2::SC_ERRORS_DAQ::ERR_UPackLink_Plaus),RUNNING_PASSED, not_equal );
  
  // 3) ) Set <L_ContCtrl_HVBatteryConnection_State> = /Connected/ <P_HVMeas_ULink> = <L_Mem_HVMeas_PackLink_Param> + <P_HVMeas_UPack>
  teststep("ENG8_4351.3","3) ) Set <L_ContCtrl_HVBatteryConnection_State> = /Connected/ <P_HVMeas_ULink> = <L_Mem_HVMeas_PackLink_Param> + <P_HVMeas_UPack> ");
  testCaseComment("");  
  trickMemoryBefore();
  testStep("ENG8_4351.3a","Set HV contactor state to /Connected/.");
  funcIDE_WriteVariable("ContDiag_Xcp_ContMainPos_State_out_ACT_U8", 170);
  funcIDE_WriteVariable("ContDiag_Xcp_ContMainPos_State_out_VAL_U8", 2);
  funcIDE_WriteVariable("ContDiag_Xcp_ContMainNeg_State_out_ACT_U8", 170);
  funcIDE_WriteVariable("ContDiag_Xcp_ContMainNeg_State_out_VAL_U8", 2);
  trickMemoryAfter();
  
  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 355 * 40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", (UPackLinkDiffMax + 355) * 40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
  trickMemoryAfter();

  testWaitForTimeout(10000);
  
  //4) Check <ErrQ_HVMeas_PackLinkPlausibility>
  testStep("ENG8_4351.4","Check <ErrQ_HVMeas_PackLinkPlausibility> if PASSED");
  testCaseComment("");
  func_ValueCompare(ERR_Q_HV_MEAS_U_PACK_LINK_PLAUSIBILITY_NAME, getSignal(CAN2::SC_ERRORS_DAQ::ERR_UPackLink_Plaus),RUNNING_PASSED, equal );
  
  //cleanUp()
  func_CleanUp();
}

testcase ENG8_4354 ()
{
  int UPackLinkDiffMax;
  float uPack;
  
  testCaseTitle("ENG8_4354","Pack-Link plausibility - with open contactors and no fault conditions");
  testCaseDescription("");
  
    //preconditions
  func_Preconditions();
  UPackLinkDiffMax = funcIDE_ReadVariable("HvDaqMgrParameter.p_UPackLinkDiffMax");
  UPackLinkDiffMax = (int)((float)UPackLinkDiffMax*0.025);
  teststep("","");

  //2) Check <ErrQ_HVMeas_PackLinkPlausibility>
  testStep("ENG8_4354.2","Check <ErrQ_HVMeas_PackLinkPlausibility>");
  testCaseComment("");
  func_ValueCompare(ERR_Q_HV_MEAS_U_PACK_LINK_PLAUSIBILITY_NAME, getSignal(CAN2::SC_ERRORS_DAQ::ERR_UPackLink_Plaus),RUNNING_PASSED, not_equal );
  
  // 3) ) Set <L_ContCtrl_HVBatteryConnection_State> = /Disconnected/ <P_HVMeas_ULink> = <L_Mem_HVMeas_PackLink_Param> + <P_HVMeas_UPack>
  teststep("ENG8_4354.3","3) ) Set <L_ContCtrl_HVBatteryConnection_State> = /Connected/ <P_HVMeas_ULink> = <L_Mem_HVMeas_PackLink_Param> + <P_HVMeas_UPack> ");
  testCaseComment("");  

  func_ValueCompare("<L_ContCtrl_ContMainPos_State>", getsignal(CAN2::SC_STATES_01::STAT_CONTACTOR_PLUS), CNT_OPEN, equal);
  func_ValueCompare("<L_ContCtrl_ContMainNeg_State>", getsignal(CAN2::SC_STATES_01::STAT_CONTACTOR_MINUS), CNT_OPEN, equal);
  
  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 355 * 40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", (UPackLinkDiffMax + 355) * 40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
  trickMemoryAfter();
  testWaitForTimeout(10000);
  
  //4) Check <ErrQ_HVMeas_PackLinkPlausibility>
  testStep("ENG8_4354.4","Check <ErrQ_HVMeas_PackLinkPlausibility> if PASSED");
  testCaseComment("");
  func_ValueCompare(ERR_Q_HV_MEAS_U_PACK_LINK_PLAUSIBILITY_NAME, getSignal(CAN2::SC_ERRORS_DAQ::ERR_UPackLink_Plaus), RUNNING_PASSED, not_equal );
  
  //cleanUp()
  func_CleanUp();
}

/*************************************************************
*###### Pack Voltage Fault Detection and Error Handling ######
**************************************************************/

testcase ENG8_4286 ()
{
  char CANSignal[100] = "SC_ERRORS_DAQ::ERR_UPack_OverVltg";
  int UPackHighThres;

  testCaseTitle("ENG8_4286","UPack overvoltage");
  testCaseDescription("After BMU start the UPack overvoltage error bit is initially checked to be unequal to failed. The UPack value is set to a value above the overvoltage threshold and the overvoltage error bit is observed to switch failed.");
  
  //precon
  func_Preconditions();
  UPackHighThres = funcIDE_ReadVariable("HvDaqMgrParameter.p_UPackHighThres");
  UPackHighThres = (int)((float)UPackHighThres*0.025);
  teststep("","");
  
  // 2) Check <ErrQ_HVMeas_UPackOvervoltage>
  testStep("ENG8_4286.2","Check %s initially", CANSignal );
  func_ValueCompare(CANSignal, getSignal(CAN2::SC_ERRORS_DAQ::ERR_UPack_OverVltg), RUNNING_FAILED, not_equal);
  teststep("","");
  
  // 3) Set <P_HVMeas_UPack> = <L_Mem_HVMeas_ThresholdUPackOvervoltage_Param> + 1unit
  testStep("ENG8_4286.3","Set UPack greater than overvoltage threshold\n\n<P_HVMeas_UPack> = <L_Mem_HVMeas_ThresholdUPackOvervoltage_Param> + 1unit");
  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", (UPackHighThres + 1) / 0.025);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 0);
  trickMemoryAfter();
  testWaitForTimeout(10000);  
  teststep("","");
  // 4) Check <ErrQ_HVMeas_UPackOvervoltage> 
  testStep("ENG8_4286.4","Check %s finally", CANSignal );
  func_ValueCompare(CANSignal, getSignal(CAN2::SC_ERRORS_DAQ::ERR_UPack_OverVltg), RUNNING_FAILED, equal);
   
  // cleanUp
  func_CleanUp();

}

testcase ENG8_4287 ()
{
  char CANSignal[100] = "SC_ERRORS_DAQ::ERR_UPack_OverVltg";
  int UPackHighThres, UPackHyst;

  testCaseTitle("ENG8_4287","UPack no overvoltage ");
  testCaseDescription("After BMU start the UPack overvoltage error bit is initially checked to be unequal to passed. The UPack value is set to a value equal to the overvoltage threshold and the overvoltage error bit is observed to switch passed.");
  
  //precon
  func_Preconditions();
  UPackHighThres = funcIDE_ReadVariable("HvDaqMgrParameter.p_UPackHighThres");
  UPackHighThres = (int)((float)UPackHighThres*0.025);
  UPackHyst = funcIDE_ReadVariable("HvDaqMgrParameter.p_UPackHyst");
  UPackHyst = (int)((float)UPackHyst*0.025);
  teststep("","");

  testStep("ENG8_4287.1","Set UPack greater than overvoltage threshold\n\n<P_HVMeas_UPack> = <L_Mem_HVMeas_ThresholdUPackOvervoltage_Param> + 1unit");
  teststep("","");
  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", (UPackHighThres + 1) / 0.025);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 0);
  trickMemoryAfter();
  testWaitForTimeout(10000);
  
  // 2) Check <ErrQ_HVMeas_UPackOvervoltage>
  testStep("ENG8_4287.2","Check <ErrQ_HVMeas_UPackOvervoltage>");
  testCaseComment("");
  func_ValueCompare(ERRQ_HV_MEAS_U_PACK_OVERVOLTAGE_NAME, getSignal(CAN2::SC_ERRORS_DAQ::ERR_UPack_OverVltg), RUNNING_PASSED, not_equal);  
  teststep("","");
  
  // 3) Set <P_HVMeas_UPack> = <L_Mem_HVMeas_ThresholdUPackOvervoltage_Param> - <L_Mem_HVMeas_OvervoltageHysteresis_Param> 
  testStep("ENG8_4287.3","Set UPack smaller than overvoltage threshold minus hysteresis \n\n<P_HVMeas_UPack> = <L_Mem_HVMeas_ThresholdUPackOvervoltage_Param> - <L_Mem_HVMeas_OvervoltageHysteresis_Param> ");
  testCaseComment("");
  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", ((UPackHighThres - UPackHyst /* - 1 */) / 0.025) ); 
  trickMemoryAfter();
  testWaitForTimeout(10000); 
  teststep("","");
  
  // 4) Check <ErrQ_HVMeas_UPackOvervoltage> 
  testStep("ENG8_4287.4","Check <ErrQ_HVMeas_UPackOvervoltage>");
  testCaseComment("");
  func_ValueCompare(ERRQ_HV_MEAS_U_PACK_OVERVOLTAGE_NAME, getSignal(CAN2::SC_ERRORS_DAQ::ERR_UPack_OverVltg), RUNNING_PASSED, equal);
  teststep("",""); 
  // cleanUp
  func_CleanUp();
}

testcase ENG8_4288 ()
{
  int errorBitBefore, errorBitAfter, UPackOutOfRangeHi;
  
  testCaseTitle("ENG8_4288","UPack out of range - upper limit");
  testCaseDescription("After BMU start the UPack out of range error bit for the upper limit is initially checked to be unequal to failed. The UPack value is set to a value above the out of range threshold and the out of range error bit for the upper limit is observed to switch failed.");

  //Preconditions
  func_Preconditions();
  UPackOutOfRangeHi = funcIDE_ReadVariable("HvDaqMgrParameter.p_UPackOutOfRangeHi");
  UPackOutOfRangeHi = (int)((float)UPackOutOfRangeHi*0.025);
  
  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", (UPackOutOfRangeHi / 0.025)); // 430 Resolution 0.025 = 17200 -> 431 with 0.025 = 17240
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 0);
  trickMemoryAfter();
  teststep("","");

  // 2) Check  ErrQ_HVMeas_UPackOutOfRangeHigh>
  testStep("ENG8_4288.2","Check  ErrQ_HVMeas_UPackOutOfRangeHigh>");
  testCaseComment("");
  errorBitBefore = getSignal(CAN2::SC_ERRORS_DAQ::ERR_UPack_RNGH);

  // 3) Set <P_HVMeas_UPack> = <L_Mem_HVMeas_ThresholdUPackOutOfRangeHigh_Param> + 1unit
  testStep("ENG8_4288.3","Set <P_HVMeas_UPack> = <L_Mem_HVMeas_ThresholdUPackOutOfRangeHigh_Param> + 1unit");
  testCaseComment("");
  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", (UPackOutOfRangeHi + 1) / 0.025);
  trickMemoryAfter();
  testWaitForTimeout(5000);
  
  // 4) Check  <ErrQ_HVMeas_UPackOutOfRangeHigh>
  testStep("ENG8_4288.4","Check  <ErrQ_HVMeas_UPackOutOfRangeHigh>");
  testCaseComment("");
  errorBitAfter = getSignal(CAN2::SC_ERRORS_DAQ::ERR_UPack_RNGH);
  
  // Assess
  func_ValueCompare("ErrQ_HVMeas_UPackOutOfRangeHigh", errorBitBefore, RUNNING_FAILED, not_equal);
  func_ValueCompare("ErrQ_HVMeas_UPackOutOfRangeHigh", errorBitAfter, RUNNING_FAILED, equal);
  
  //Cleanup
  func_CleanUp();
}

testcase ENG8_4289 ()
{
  int errorBitBefore, errorBitAfter, UPackOutOfRangeHi;
  
  testCaseTitle("ENG8_4289","UPack not out of range - upper limit");
  testCaseDescription("After BMU start the UPack out of range error bit for the upper limit is initially checked to be unequal to passed. The UPack value is set to a value equal to the out of range threshold and the out of range error bit for the upper limit is observed to switch passed.");
  
  //Preconditions
  func_Preconditions();
  UPackOutOfRangeHi = funcIDE_ReadVariable("HvDaqMgrParameter.p_UPackOutOfRangeHi");
  UPackOutOfRangeHi = (int)((float)UPackOutOfRangeHi*0.025);
  
  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", (UPackOutOfRangeHi + 1) / 0.025);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 0);
  trickMemoryAfter();
  testWaitForTimeout(5000);  
  teststep("","");


  // 2) Check  ErrQ_HVMeas_UPackOutOfRangeHigh>
  testStep("ENG8_4289.2","Check  ErrQ_HVMeas_UPackOutOfRangeHigh>");
  testCaseComment("");
  errorBitBefore = getSignal(CAN2::SC_ERRORS_DAQ::ERR_UPack_RNGH);

  // 3) Set <P_HVMeas_UPack> = <L_Mem_HVMeas_ThresholdUPackOutOfRangeHigh_Param> 
  testStep("ENG8_4289.3","Set <P_HVMeas_UPack> = <L_Mem_HVMeas_ThresholdUPackOutOfRangeHigh_Param> ");
  testCaseComment("");
  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", (UPackOutOfRangeHi / 0.025));
  trickMemoryAfter();
  testWaitForTimeout(5000);
  
  // 4) Check  <ErrQ_HVMeas_UPackOutOfRangeHigh>
  testStep("ENG8_4289.4","Check  <ErrQ_HVMeas_UPackOutOfRangeHigh>");
  testCaseComment("");
  errorBitAfter = getSignal(CAN2::SC_ERRORS_DAQ::ERR_UPack_RNGH);
  
  // Assess
  func_ValueCompare("ErrQ_HVMeas_UPackOutOfRangeHigh before", errorBitBefore, RUNNING_PASSED, not_equal);
  func_ValueCompare("ErrQ_HVMeas_UPackOutOfRangeHigh after", errorBitAfter, RUNNING_PASSED, equal);
  
  //Cleanup
  func_CleanUp();
}
//PLUGIN
//testcase ENG8_4296 ()
//{
//  int match, startTime, endTime, facHvMeasTimeout;
//  
//  testCaseTitle("ENG8_4296","UPack Timeout");
//  testCaseDescription("After BMU start the UPack timeout error bit is initially checked to be unequal to failed. The UPack state is set to invalid. After a defined timeout duration the error bit is checked to switch to failed.");
//
//  //Preconditions
//  func_Preconditions();
//  facHvMeasTimeout = funcIDE_ReadVariable("HvDaqMgrParameter.p_facHvMeasTimeout");
//  facHvMeasTimeout = facHvMeasTimeout * 10;
//
//  // 2) Check <ErrQ_HVMeas_UPackTimeout>
//  testStep("ENG8_4296.2","Check <ErrQ_HVMeas_UPackTimeout>");
//  testCaseComment("");
//  func_ValueCompare("Signal on CAN ErrQ_HVMeas_UPackTimeout", getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_UPack_Timeout),RUNNING_FAILED, not_equal);
//    
//  // 3) Set <L_HVMeas_UPack_State.bInvalid> == /true/
//  testStep("ENG8_4296.3","Set <L_HVMeas_UPack_State.bInvalid> == /true/"); // true == invalid
//  testCaseComment("");
//  trickMemoryBefore();
//  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1);
//  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 32767);
//  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 0);
//  funcIDE_DeleteAllBP();
//  
//  // 4) Wait L_Mem_HVMeas_Timeout_Param ms
//  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "CtDaqMgr_SetEventExWithStop(!(IsImocRunning), bLastInv_B_a, Event_Err_HVMeas_UPackTimeout, CtDaqMgr_AddInfo_U8);");
//  funcIDE_RunAndWait(2000);
//  startTime = funcIDE_ReadVariable("@STM_CNT");
//  match = testWaitForVariableValue("bLastInv_B_a", FAILED_INTERNAL, 20000);
//  endTime = funcIDE_ReadVariable("@STM_CNT");
//  check_equality(match, FAILED_INTERNAL, "The timeout error is set to FAILED_INTERNAL", "The timeout error is not set to FAILED_INTERNAL");
//  func_timeCompare ("duration", "ms", (endTime-startTime)/64000, facHvMeasTimeout, equal_or_less_than);
//  trickMemoryAfter();
//  testWaitForTimeout(4000);
//  
//  // 5) Check <ErrQ_HVMeas_UPackTimeout>  
//  testStep("ENG8_4296.5","Check <ErrQ_HVMeas_UPackTimeout>");
//  testCaseComment("");
//  func_ValueCompare("Signal on CAN ErrQ_HVMeas_UPackTimeout", getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_UPack_Timeout),RUNNING_FAILED, equal);
//
//  //cleanup
//  func_CleanUp();
//}

testcase ENG8_14979_Teststep1 ()
  
{
  int match, facHvMeasTimeout, i;
  int res_passed, res_failed, errortype;
  
  testCaseTitle("ENG8_14979_Teststep1","UPack Timeout");
  testCaseDescription("After BMU start the UPack timeout error bit is initially checked to be unequal to failed. The UPack_Pos or UPack_Neg  signal are set to SNA (depends on table, see bottom Testdescription). After a defined timeout duration the error bit is checked to switch to failed. After the Check the  ImocState_VoltageMeasurementMode is set to OnlyPackVoltageMeasurement or TotalVoltageMeasurement (depends on table, see bottom Testdescription). After a defined timeout duration the error bit is checked to switch to failed.");

  //Preconditions
  func_Preconditions();
  facHvMeasTimeout = L_Mem_HVMeas_Timeout_Param_WITH_TOLERANCE / 10;
      
  // 1b) Set <L_HVMeas_UPack_Pos> =0  and Set <L_HVMeas_UPack_Neg>=0
  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 200);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 200);
  funcIDE_DeleteAllBP();
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages","SHvDaq16_NoMeasurement = VoltageMeasurementMode == 0;");
  funcIDE_Run();
  funcIDE_WaitForHalt (2000); // time can be modified
  funcIDE_WriteVariable("VoltageMeasurementMode", NoVoltageMeasurement);
  funcIDE_DeleteAllBP();
  
   // 1c) Wait L_Mem_HVMeas_Timeout_Param ms and check ERR_UPack_Timeout != RUNNING_FAILED
  for (i = 0; i < facHvMeasTimeout; i++)
  { funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages","SHvDaq16_NoMeasurement = VoltageMeasurementMode == 0;");
    match = getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_UPack_Timeout);
    if (match != RUNNING_FAILED) res_passed++; else res_failed++; 
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);  
    funcIDE_WriteVariable("VoltageMeasurementMode", NoVoltageMeasurement);
    funcIDE_DeleteAllBP();
    funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "CtDaqMgr_SetEventExWithStop(SHvDaq16_NoMeasurement || (VoltageMeasurementMode == 1), bLastInv_B_a,");
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);  
    check_equality(funcIDE_ReadVariable("bLastInv_B_a"), PASSED_INTERNAL, "The timeout error is set to PASSED_INTERNAL", "The timeout error is not set to PASSED_INTERNAL");
    check_equality(funcIDE_ReadVariable("VoltageMeasurementMode"), NoVoltageMeasurement, "The <L_ImocCtrl_ImocState_VoltageMeasurementMode> is set to NoVoltageMeasurement", "The <L_ImocCtrl_ImocState_VoltageMeasurementMode> is not set to NoVoltageMeasurement");
    funcIDE_DeleteAllBP();
    if (i==10-1) teststep("","loop complete,ERR_UPack_Timeout != RUNNING_FAILED -> %d = passed; %d = failed.", res_passed, res_failed);
  }
  check_equality(res_failed, 0, "The Signal on CAN ErrQ_HVMeas_UPackTimeout was always as expected in != RUNNING_FAILED", "The Signal on CAN ErrQ_HVMeas_UPackTimeout switched in unexpected value.");
  
  // 2) Check <ErrQ_HVMeas_UPackTimeout>
  testStep("ENG8_14979_Teststep1.2","Check <ErrQ_HVMeas_UPackTimeout>");
  testCaseComment("");
  func_ValueCompare("Signal on CAN ErrQ_HVMeas_UPackTimeout", getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_UPack_Timeout),RUNNING_FAILED, not_equal);
  
  // 3) Set <L_HVMeas_UPack_Pos> =0  and Set <L_HVMeas_UPack_Neg>=1 
  testStep("ENG8_14979_Teststep1.3","Set <L_HVMeas_UPack_Pos> =0  and Set <L_HVMeas_UPack_Neg>=1 "); 
  testCaseComment("");
  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 200);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 32767);
  funcIDE_DeleteAllBP();
   
  // 4) Wait L_Mem_HVMeas_Timeout_Param ms and check ERR_UPack_Timeout !=RUNNING_FAILED
  testStep("ENG8_14979_Teststep1.4","Wait L_Mem_HVMeas_Timeout_Param ms");
  for (i = 0; i < facHvMeasTimeout; i++)
  { funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages","SHvDaq16_NoMeasurement = VoltageMeasurementMode == 0;");
    match = getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_UPack_Timeout);
    if (match != RUNNING_FAILED) res_passed++; else res_failed++; 
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);   // time can be modified
    funcIDE_WriteVariable("VoltageMeasurementMode", NoVoltageMeasurement);
    funcIDE_DeleteAllBP();
    funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "CtDaqMgr_SetEventExWithStop(SHvDaq16_NoMeasurement || (VoltageMeasurementMode == 1), bLastInv_B_a,");
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);   // time can be modified
    check_equality(funcIDE_ReadVariable("bLastInv_B_a"), PASSED_INTERNAL, "The timeout error is set to PASSED_INTERNAL", "The timeout error is not set to PASSED_INTERNAL");
    check_equality(funcIDE_ReadVariable("VoltageMeasurementMode"), NoVoltageMeasurement, "The <L_ImocCtrl_ImocState_VoltageMeasurementMode> is set to NoVoltageMeasurement", "The <L_ImocCtrl_ImocState_VoltageMeasurementMode> is not set to NoVoltageMeasurement");
    funcIDE_DeleteAllBP();
    if (i==10-1) teststep("","loop complete,ERR_UPack_Timeout != RUNNING_FAILED -> %d = passed; %d = failed.",res_passed,res_failed);
  }
  check_equality(res_failed, 0, "The Signal on CAN ErrQ_HVMeas_UPackTimeout was always as expected in != RUNNING_FAILED", "The Signal on CAN ErrQ_HVMeas_UPackTimeout switched in unexpected value.");
  
  // 5) Check <ErrQ_HVMeas_UPackTimeout>
  testStep("ENG8_14979_Teststep1.5","Check <ErrQ_HVMeas_UPackTimeout>");
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages","SHvDaq16_NoMeasurement = VoltageMeasurementMode == 0;");
  res_passed = 0;
  res_failed = 0;
  // Wait for CAN
  for (i = 0; i < 250; i++)
  { match = getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_UPack_Timeout);
    if (match != RUNNING_FAILED) res_passed++; else res_failed++; 
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);   // time can be modified
    funcIDE_WriteVariable("VoltageMeasurementMode", NoVoltageMeasurement);
    if (i == 250 - 1) teststep("","loop complete,ERR_UPack_Timeout != RUNNING_FAILED -> %d = passed; %d = failed.",res_passed,res_failed);
  }
  check_equality(res_failed, 0, "The Signal on CAN ErrQ_HVMeas_UPackTimeout was always as expected in != RUNNING_FAILED", "The Signal on CAN ErrQ_HVMeas_UPackTimeout switched in unexpected value.");
  
  // 6) Set <L_ImocCtrl_ImocState_VoltageMeasurementMode> = OnlyPackVoltageMeasurement
  testStep("ENG8_14979_Teststep1.6","Set <L_ImocCtrl_ImocState_VoltageMeasurementMode> = OnlyPackVoltageMeasurement");
  funcIDE_Run();
  funcIDE_WaitForHalt (2000); // time can be modified
  funcIDE_WriteVariable("VoltageMeasurementMode", OnlyPackVoltageMeasurement);
  funcIDE_DeleteAllBP();
  
  // 7) Wait L_Mem_HVMeas_Timeout_Param ms and check ERR_UPack_Timeout ==RUNNING_FAILED
  testStep("ENG8_14979_Teststep1.7","Wait L_Mem_HVMeas_Timeout_Param ms");
    
  for (i=0;i<facHvMeasTimeout;i++)
  { funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "SHvDaq16_NoMeasurement = VoltageMeasurementMode == 0;");
    match = getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_UPack_Timeout);
    if (match != RUNNING_FAILED) res_passed++; else res_failed++; 
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);   // time can be modified
    funcIDE_WriteVariable("VoltageMeasurementMode", OnlyPackVoltageMeasurement);
    funcIDE_DeleteAllBP();
    funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "CtDaqMgr_SetEventExWithStop(SHvDaq16_NoMeasurement || (VoltageMeasurementMode == 1), bLastInv_B_a,");
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);   // time can be modified
    check_equality(funcIDE_ReadVariable("bLastInv_B_a"), FAILED_INTERNAL, "The timeout error is set to FAILED_INTERNAL", "The timeout error is not set to FAILED_INTERNAL");
    check_equality(funcIDE_ReadVariable("VoltageMeasurementMode"), OnlyPackVoltageMeasurement, "The <L_ImocCtrl_ImocState_VoltageMeasurementMode> is set to OnlyPackVoltageMeasurement", "The <L_ImocCtrl_ImocState_VoltageMeasurementMode> is not set to OnlyPackVoltageMeasurement");
    funcIDE_DeleteAllBP();
    if (i == 10 - 1) teststep("","loop complete,ERR_UPack_Timeout != RUNNING_FAILED -> %d = passed; %d = failed.",res_passed,res_failed);
  }
  check_equality(res_failed, 0, "The Signal on CAN ErrQ_HVMeas_UPackTimeout was always as expected in != RUNNING_FAILED", "The Signal on CAN ErrQ_HVMeas_UPackTimeout switched in unexpected value.");
  
  // 8) Check <ErrQ_HVMeas_UPackTimeout>
  testStep("ENG8_14979_Teststep1.8", "Check <ErrQ_HVMeas_UPackTimeout>");
  // Wait for CAN
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "SHvDaq16_NoMeasurement = VoltageMeasurementMode == 0;");
  res_passed=0;
  res_failed=0;
    
  for (i = 0; i < 300; i++)
  { 
    match=getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_UPack_Timeout);
    if (match==RUNNING_FAILED)
      res_passed++; 
    else {
      res_failed++;
      errortype=match;}    
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);   // time can be modified
    funcIDE_WriteVariable("VoltageMeasurementMode", OnlyPackVoltageMeasurement);
    if (i==300-1) teststep("","loop complete,ERR_UPack_Timeout == RUNNING_FAILED -> %d = passed; %d = failed.", res_passed, res_failed);
  }
  check_equality(errortype, 4, "The Signal on CAN ErrQ_HVMeas_UPackTimeout switched also in Status /DEBOUNCING_PREVIOUS_PASS/", "The Signal on CAN ErrQ_HVMeas_UPackTimeout switched in unexpected value.");
  func_ValueCompare("The Signal on CAN ErrQ_HVMeas_UPackTimeout switched also in Status /DEBOUNCING_PREVIOUS_PASS/ as expected.", res_failed, 0, greater_than);
     
  testCaseComment("");
  func_ValueCompare("Signal on CAN ErrQ_HVMeas_UPackTimeout", getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_UPack_Timeout), RUNNING_FAILED, equal);
  
    
  //cleanup
  func_CleanUp();
}


testcase ENG8_14979_Teststep2 ()
  
{
  int match, facHvMeasTimeout, i;
  int res_passed, res_failed, errortype;
  
  testCaseTitle("ENG8_14979_Teststep2", "UPack Timeout");
  testCaseDescription("After BMU start the UPack timeout error bit is initially checked to be unequal to failed. The UPack_Pos or UPack_Neg  signal are set to SNA (depends on table, see bottom Testdescription). After a defined timeout duration the error bit is checked to switch to failed. After the Check the  ImocState_VoltageMeasurementMode is set to OnlyPackVoltageMeasurement or TotalVoltageMeasurement (depends on table, see bottom Testdescription). After a defined timeout duration the error bit is checked to switch to failed.");

  //Preconditions
  func_Preconditions();
  facHvMeasTimeout = L_Mem_HVMeas_Timeout_Param_WITH_TOLERANCE / 10;
      
  // 1b) Set <L_HVMeas_UPack_Pos> =0  and Set <L_HVMeas_UPack_Neg>=0
  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 200);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 200);
  funcIDE_DeleteAllBP();
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages","SHvDaq16_NoMeasurement = VoltageMeasurementMode == 0;");
  funcIDE_Run();
  funcIDE_WaitForHalt (2000); // time can be modified
  funcIDE_WriteVariable("VoltageMeasurementMode", NoVoltageMeasurement);
  funcIDE_DeleteAllBP();
  
   // 1c) Wait L_Mem_HVMeas_Timeout_Param ms and check ERR_UPack_Timeout !=RUNNING_FAILED
  for (i = 0; i < facHvMeasTimeout;i++)
  { funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages","SHvDaq16_NoMeasurement = VoltageMeasurementMode == 0;");
    match = getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_UPack_Timeout);
    if (match != RUNNING_FAILED) res_passed++; else res_failed++; 
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);   // time can be modified
    funcIDE_WriteVariable("VoltageMeasurementMode", NoVoltageMeasurement);
    funcIDE_DeleteAllBP();
    funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "CtDaqMgr_SetEventExWithStop(SHvDaq16_NoMeasurement || (VoltageMeasurementMode == 1), bLastInv_B_a,");
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);   // time can be modified
    check_equality(funcIDE_ReadVariable("bLastInv_B_a"), PASSED_INTERNAL, "The timeout error is set to PASSED_INTERNAL", "The timeout error is not set to PASSED_INTERNAL");
    check_equality(funcIDE_ReadVariable("VoltageMeasurementMode"), NoVoltageMeasurement, "The <L_ImocCtrl_ImocState_VoltageMeasurementMode> is set to NoVoltageMeasurement", "The <L_ImocCtrl_ImocState_VoltageMeasurementMode> is not set to NoVoltageMeasurement");
    funcIDE_DeleteAllBP();
    if (i == 10 - 1) teststep("","loop complete,ERR_UPack_Timeout != RUNNING_FAILED -> %d = passed; %d = failed.", res_passed, res_failed);
  }
  check_equality(res_failed, 0, "The Signal on CAN ErrQ_HVMeas_UPackTimeout was always as expected in != RUNNING_FAILED", "The Signal on CAN ErrQ_HVMeas_UPackTimeout switched in unexpected value.");
  
  // 2) Check <ErrQ_HVMeas_UPackTimeout>
  testStep("ENG8_14979_Teststep2.2","Check <ErrQ_HVMeas_UPackTimeout>");
  testCaseComment("");
  func_ValueCompare("Signal on CAN ErrQ_HVMeas_UPackTimeout", getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_UPack_Timeout),RUNNING_FAILED, not_equal);
  
  // 3) Set <L_HVMeas_UPack_Pos> =0  and Set <L_HVMeas_UPack_Neg>=1 
  testStep("ENG8_14979_Teststep2.3","Set <L_HVMeas_UPack_Pos> =0  and Set <L_HVMeas_UPack_Neg>=1 "); 
  testCaseComment("");
  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 200);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 32767);
  funcIDE_DeleteAllBP();
   
  // 4) Wait L_Mem_HVMeas_Timeout_Param ms and check ERR_UPack_Timeout !=RUNNING_FAILED
  testStep("ENG8_14979_Teststep2.4","Wait L_Mem_HVMeas_Timeout_Param ms");
  for (i = 0; i < facHvMeasTimeout; i++)
  { funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages","SHvDaq16_NoMeasurement = VoltageMeasurementMode == 0;");
    match = getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_UPack_Timeout);
    if (match != RUNNING_FAILED) res_passed++; else res_failed++; 
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);   // time can be modified
    funcIDE_WriteVariable("VoltageMeasurementMode", NoVoltageMeasurement);
    funcIDE_DeleteAllBP();
    funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "CtDaqMgr_SetEventExWithStop(SHvDaq16_NoMeasurement || (VoltageMeasurementMode == 1), bLastInv_B_a,");
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);   // time can be modified
    check_equality(funcIDE_ReadVariable("bLastInv_B_a"), PASSED_INTERNAL, "The timeout error is set to PASSED_INTERNAL", "The timeout error is not set to PASSED_INTERNAL");
    check_equality(funcIDE_ReadVariable("VoltageMeasurementMode"), NoVoltageMeasurement, "The <L_ImocCtrl_ImocState_VoltageMeasurementMode> is set to NoVoltageMeasurement", "The <L_ImocCtrl_ImocState_VoltageMeasurementMode> is not set to NoVoltageMeasurement");
    funcIDE_DeleteAllBP();
    if (i == 10 - 1) teststep("","loop complete,ERR_UPack_Timeout != RUNNING_FAILED -> %d = passed; %d = failed.", res_passed, res_failed);
  }
  check_equality(res_failed, 0, "The Signal on CAN ErrQ_HVMeas_UPackTimeout was always as expected in != RUNNING_FAILED", "The Signal on CAN ErrQ_HVMeas_UPackTimeout switched in unexpected value.");
  
  // 5) Check <ErrQ_HVMeas_UPackTimeout>
  testStep("ENG8_14979_Teststep2.5","Check <ErrQ_HVMeas_UPackTimeout>");
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages","SHvDaq16_NoMeasurement = VoltageMeasurementMode == 0;");
  res_passed = 0;
  res_failed = 0;
  // Wait for CAN
  for (i = 0; i < 250; i++)
  { match = getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_UPack_Timeout);
    if (match != RUNNING_FAILED) res_passed++; else res_failed++; 
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);   // time can be modified
    funcIDE_WriteVariable("VoltageMeasurementMode", NoVoltageMeasurement);
    if (i == 250 - 1) teststep("","loop complete,ERR_UPack_Timeout != RUNNING_FAILED -> %d = passed; %d = failed.", res_passed, res_failed);
  }
  check_equality(res_failed, 0, "The Signal on CAN ErrQ_HVMeas_UPackTimeout was always as expected in != RUNNING_FAILED", "The Signal on CAN ErrQ_HVMeas_UPackTimeout switched in unexpected value.");
  
  // 6) Set <L_ImocCtrl_ImocState_VoltageMeasurementMode> = TotalVoltageMeasurement
  testStep("ENG8_14979_Teststep2.6","Set <L_ImocCtrl_ImocState_VoltageMeasurementMode> = TotalVoltageMeasurement");
  funcIDE_Run();
  funcIDE_WaitForHalt (2000); // time can be modified
  funcIDE_WriteVariable("VoltageMeasurementMode", TotalVoltageMeasurement);
  funcIDE_DeleteAllBP();
  
  // 7) Wait L_Mem_HVMeas_Timeout_Param ms and check ERR_UPack_Timeout ==RUNNING_FAILED
  testStep("ENG8_14979_Teststep2.7","Wait L_Mem_HVMeas_Timeout_Param ms");
    
  for (i = 0; i < facHvMeasTimeout; i++)
  { funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages","SHvDaq16_NoMeasurement = VoltageMeasurementMode == 0;");
    match = getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_UPack_Timeout);
    if (match != RUNNING_FAILED) res_passed++; else res_failed++; 
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);   // time can be modified
    funcIDE_WriteVariable("VoltageMeasurementMode", TotalVoltageMeasurement);
    funcIDE_DeleteAllBP();
    funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "CtDaqMgr_SetEventExWithStop(SHvDaq16_NoMeasurement || (VoltageMeasurementMode == 1), bLastInv_B_a,");
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);   // time can be modified
    check_equality(funcIDE_ReadVariable("bLastInv_B_a"), FAILED_INTERNAL, "The timeout error is set to FAILED_INTERNAL", "The timeout error is not set to FAILED_INTERNAL");
    check_equality(funcIDE_ReadVariable("VoltageMeasurementMode"), TotalVoltageMeasurement, "The <L_ImocCtrl_ImocState_VoltageMeasurementMode> is set to TotalVoltageMeasurement", "The <L_ImocCtrl_ImocState_VoltageMeasurementMode> is not set to TotalVoltageMeasurement");
    funcIDE_DeleteAllBP();
    if (i == 10 - 1) teststep("","loop complete,ERR_UPack_Timeout != RUNNING_FAILED -> %d = passed; %d = failed.", res_passed, res_failed);
  }
  check_equality(res_failed, 0, "The Signal on CAN ErrQ_HVMeas_UPackTimeout was always as expected in != RUNNING_FAILED", "The Signal on CAN ErrQ_HVMeas_UPackTimeout switched in unexpected value.");
  
  // 8) Check <ErrQ_HVMeas_UPackTimeout>
  testStep("ENG8_14979_Teststep2.8","Check <ErrQ_HVMeas_UPackTimeout>");
  // Wait for CAN
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages","SHvDaq16_NoMeasurement = VoltageMeasurementMode == 0;");
  res_passed = 0;
  res_failed = 0;
    
  for (i = 0; i < 300; i++)
  { 
    match = getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_UPack_Timeout);
    if (match == RUNNING_FAILED)
      res_passed++; 
    else {
      res_failed++;
      errortype = match;}    
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);   // time can be modified
    funcIDE_WriteVariable("VoltageMeasurementMode", TotalVoltageMeasurement);
    if (i == 300 - 1) teststep("","loop complete,ERR_UPack_Timeout == RUNNING_FAILED -> %d = passed; %d = failed.", res_passed, res_failed);
  }
  check_equality(errortype, 4, "The Signal on CAN ErrQ_HVMeas_UPackTimeout switched also in Status /DEBOUNCING_PREVIOUS_PASS/", "The Signal on CAN ErrQ_HVMeas_UPackTimeout switched in unexpected value.");
  func_ValueCompare("The Signal on CAN ErrQ_HVMeas_UPackTimeout switched also in Status /DEBOUNCING_PREVIOUS_PASS/ as expected.", res_failed,0, greater_than);
  
     
  testCaseComment("");
  func_ValueCompare("Signal on CAN ErrQ_HVMeas_UPackTimeout", getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_UPack_Timeout), RUNNING_FAILED, equal);
  
    
  //cleanup
  func_CleanUp();
}



testcase ENG8_14979_Teststep3 ()
  
{
  int match, facHvMeasTimeout, i;
  int res_passed, res_failed, errortype;
  
  testCaseTitle("ENG8_14979_Teststep3","UPack Timeout");
  testCaseDescription("After BMU start the UPack timeout error bit is initially checked to be unequal to failed. The UPack_Pos or UPack_Neg  signal are set to SNA (depends on table, see bottom Testdescription). After a defined timeout duration the error bit is checked to switch to failed. After the Check the  ImocState_VoltageMeasurementMode is set to OnlyPackVoltageMeasurement or TotalVoltageMeasurement (depends on table, see bottom Testdescription). After a defined timeout duration the error bit is checked to switch to failed.");

  //Preconditions
  func_Preconditions();
  facHvMeasTimeout = L_Mem_HVMeas_Timeout_Param_WITH_TOLERANCE / 10;
      
  // 1b) Set <L_HVMeas_UPack_Pos> =0  and Set <L_HVMeas_UPack_Neg>=0
  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 200);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 200);
  funcIDE_DeleteAllBP();
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages","SHvDaq16_NoMeasurement = VoltageMeasurementMode == 0;");
  funcIDE_Run();
  funcIDE_WaitForHalt (2000); // time can be modified
  funcIDE_WriteVariable("VoltageMeasurementMode", NoVoltageMeasurement);
  funcIDE_DeleteAllBP();
  
   // 1c) Wait L_Mem_HVMeas_Timeout_Param ms and check ERR_UPack_Timeout !=RUNNING_FAILED
  for (i = 0; i < facHvMeasTimeout; i++)
  { funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages","SHvDaq16_NoMeasurement = VoltageMeasurementMode == 0;");
    match = getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_UPack_Timeout);
    if (match != RUNNING_FAILED) res_passed++; else res_failed++; 
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);   // time can be modified
    funcIDE_WriteVariable("VoltageMeasurementMode", NoVoltageMeasurement);
    funcIDE_DeleteAllBP();
    funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "CtDaqMgr_SetEventExWithStop(SHvDaq16_NoMeasurement || (VoltageMeasurementMode == 1), bLastInv_B_a,");
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);   // time can be modified
    check_equality(funcIDE_ReadVariable("bLastInv_B_a"), PASSED_INTERNAL, "The timeout error is set to PASSED_INTERNAL", "The timeout error is not set to PASSED_INTERNAL");
    check_equality(funcIDE_ReadVariable("VoltageMeasurementMode"), NoVoltageMeasurement, "The <L_ImocCtrl_ImocState_VoltageMeasurementMode> is set to NoVoltageMeasurement", "The <L_ImocCtrl_ImocState_VoltageMeasurementMode> is not set to NoVoltageMeasurement");
    funcIDE_DeleteAllBP();
    if (i == 10 - 1) teststep("","loop complete,ERR_UPack_Timeout != RUNNING_FAILED -> %d = passed; %d = failed.", res_passed, res_failed);
  }
  check_equality(res_failed, 0, "The Signal on CAN ErrQ_HVMeas_UPackTimeout was always as expected in != RUNNING_FAILED", "The Signal on CAN ErrQ_HVMeas_UPackTimeout switched in unexpected value.");
  
  // 2) Check <ErrQ_HVMeas_UPackTimeout>
  testStep("ENG8_14979_Teststep3.2","Check <ErrQ_HVMeas_UPackTimeout>");
  testCaseComment("");
  func_ValueCompare("Signal on CAN ErrQ_HVMeas_UPackTimeout", getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_UPack_Timeout), RUNNING_FAILED, not_equal);
  
  // 3) Set <L_HVMeas_UPack_Pos> =1  and Set <L_HVMeas_UPack_Neg>=0 
  testStep("ENG8_14979_Teststep3.3","Set <L_HVMeas_UPack_Pos> =0  and Set <L_HVMeas_UPack_Neg>=1 "); 
  testCaseComment("");
  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 200);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 32767);
  funcIDE_DeleteAllBP();
   
  // 4) Wait L_Mem_HVMeas_Timeout_Param ms and check ERR_UPack_Timeout !=RUNNING_FAILED
  testStep("ENG8_14979_Teststep3.4","Wait L_Mem_HVMeas_Timeout_Param ms");
  for (i = 0; i < facHvMeasTimeout; i++)
  { funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages","SHvDaq16_NoMeasurement = VoltageMeasurementMode == 0;");
    match = getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_UPack_Timeout);
    if (match != RUNNING_FAILED) res_passed++; else res_failed++; 
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);   // time can be modified
    funcIDE_WriteVariable("VoltageMeasurementMode", NoVoltageMeasurement);
    funcIDE_DeleteAllBP();
    funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "CtDaqMgr_SetEventExWithStop(SHvDaq16_NoMeasurement || (VoltageMeasurementMode == 1), bLastInv_B_a,");
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);   // time can be modified
    check_equality(funcIDE_ReadVariable("bLastInv_B_a"), PASSED_INTERNAL, "The timeout error is set to PASSED_INTERNAL", "The timeout error is not set to PASSED_INTERNAL");
    check_equality(funcIDE_ReadVariable("VoltageMeasurementMode"), NoVoltageMeasurement, "The <L_ImocCtrl_ImocState_VoltageMeasurementMode> is set to NoVoltageMeasurement", "The <L_ImocCtrl_ImocState_VoltageMeasurementMode> is not set to NoVoltageMeasurement");
    funcIDE_DeleteAllBP();
    if (i == 10 - 1) teststep("","loop complete,ERR_UPack_Timeout != RUNNING_FAILED -> %d = passed; %d = failed.", res_passed, res_failed);
  }
  check_equality(res_failed, 0, "The Signal on CAN ErrQ_HVMeas_UPackTimeout was always as expected in != RUNNING_FAILED", "The Signal on CAN ErrQ_HVMeas_UPackTimeout switched in unexpected value.");
  
  // 5) Check <ErrQ_HVMeas_UPackTimeout>
  testStep("ENG8_14979_Teststep3.5","Check <ErrQ_HVMeas_UPackTimeout>");
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages","SHvDaq16_NoMeasurement = VoltageMeasurementMode == 0;");
  res_passed = 0;
  res_failed = 0;
  // Wait for CAN
  for (i = 0; i < 250; i++)
  { match = getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_UPack_Timeout);
    if (match != RUNNING_FAILED) res_passed++; else res_failed++; 
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);   // time can be modified
    funcIDE_WriteVariable("VoltageMeasurementMode", NoVoltageMeasurement);
    if (i == 250 - 1) teststep("","loop complete,ERR_UPack_Timeout != RUNNING_FAILED -> %d = passed; %d = failed.", res_passed, res_failed);
  }
  check_equality(res_failed, 0, "The Signal on CAN ErrQ_HVMeas_UPackTimeout was always as expected in != RUNNING_FAILED", "The Signal on CAN ErrQ_HVMeas_UPackTimeout switched in unexpected value.");
  
  // 6) Set <L_ImocCtrl_ImocState_VoltageMeasurementMode> = OnlyPackVoltageMeasurement
  testStep("ENG8_14979_Teststep3.6","Set <L_ImocCtrl_ImocState_VoltageMeasurementMode> = OnlyPackVoltageMeasurement");
  funcIDE_Run();
  funcIDE_WaitForHalt (2000);  // time can be modified
  funcIDE_WriteVariable("VoltageMeasurementMode", OnlyPackVoltageMeasurement);
  funcIDE_DeleteAllBP();
  
  // 7) Wait L_Mem_HVMeas_Timeout_Param ms and check ERR_UPack_Timeout ==RUNNING_FAILED
  testStep("ENG8_14979_Teststep3.7","Wait L_Mem_HVMeas_Timeout_Param ms");
    
  for (i = 0; i < facHvMeasTimeout; i++)
  { funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages","SHvDaq16_NoMeasurement = VoltageMeasurementMode == 0;");
    match = getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_UPack_Timeout);
    if (match != RUNNING_FAILED) res_passed++; else res_failed++; 
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);   // time can be modified
    funcIDE_WriteVariable("VoltageMeasurementMode", OnlyPackVoltageMeasurement);
    funcIDE_DeleteAllBP();
    funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "CtDaqMgr_SetEventExWithStop(SHvDaq16_NoMeasurement || (VoltageMeasurementMode == 1), bLastInv_B_a,");
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);   // time can be modified
    check_equality(funcIDE_ReadVariable("bLastInv_B_a"), FAILED_INTERNAL, "The timeout error is set to FAILED_INTERNAL", "The timeout error is not set to FAILED_INTERNAL");
    check_equality(funcIDE_ReadVariable("VoltageMeasurementMode"), OnlyPackVoltageMeasurement, "The <L_ImocCtrl_ImocState_VoltageMeasurementMode> is set to OnlyPackVoltageMeasurement", "The <L_ImocCtrl_ImocState_VoltageMeasurementMode> is not set to OnlyPackVoltageMeasurement");
    funcIDE_DeleteAllBP();
    if (i == 10 - 1) teststep("","loop complete,ERR_UPack_Timeout != RUNNING_FAILED -> %d = passed; %d = failed.", res_passed, res_failed);
  }
  check_equality(res_failed, 0, "The Signal on CAN ErrQ_HVMeas_UPackTimeout was always as expected in != RUNNING_FAILED", "The Signal on CAN ErrQ_HVMeas_UPackTimeout switched in unexpected value.");
  
  // 8) Check <ErrQ_HVMeas_UPackTimeout>
  testStep("ENG8_14979_Teststep3.8","Check <ErrQ_HVMeas_UPackTimeout>");
  // Wait for CAN
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages","SHvDaq16_NoMeasurement = VoltageMeasurementMode == 0;");
  res_passed = 0;
  res_failed = 0;
    
  for (i = 0; i < 300; i++)
  { 
    match = getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_UPack_Timeout);
    if (match == RUNNING_FAILED)
      res_passed++; 
    else {
      res_failed++;
      errortype = match;}    
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);   // time can be modified
    funcIDE_WriteVariable("VoltageMeasurementMode", OnlyPackVoltageMeasurement);
    if (i == 300 - 1) teststep("","loop complete,ERR_UPack_Timeout == RUNNING_FAILED -> %d = passed; %d = failed.", res_passed, res_failed);
  }
  check_equality(errortype, 4, "The Signal on CAN ErrQ_HVMeas_UPackTimeout switched also in Status /DEBOUNCING_PREVIOUS_PASS/", "The Signal on CAN ErrQ_HVMeas_UPackTimeout switched in unexpected value.");
  func_ValueCompare("The Signal on CAN ErrQ_HVMeas_UPackTimeout switched also in Status /DEBOUNCING_PREVIOUS_PASS/ as expected.", res_failed,0, greater_than);
     
  testCaseComment("");
  func_ValueCompare("Signal on CAN ErrQ_HVMeas_UPackTimeout", getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_UPack_Timeout), RUNNING_FAILED, equal);
  
    
  //cleanup
  func_CleanUp();
}



testcase ENG8_14979_Teststep4 ()
  
{
  int match, facHvMeasTimeout, i;
  int res_passed, res_failed, errortype;
  
  testCaseTitle("ENG8_14979_Teststep4","UPack Timeout");
  testCaseDescription("After BMU start the UPack timeout error bit is initially checked to be unequal to failed. The UPack_Pos or UPack_Neg  signal are set to SNA (depends on table, see bottom Testdescription). After a defined timeout duration the error bit is checked to switch to failed. After the Check the  ImocState_VoltageMeasurementMode is set to OnlyPackVoltageMeasurement or TotalVoltageMeasurement (depends on table, see bottom Testdescription). After a defined timeout duration the error bit is checked to switch to failed.");

  //Preconditions
  func_Preconditions();
  facHvMeasTimeout = L_Mem_HVMeas_Timeout_Param_WITH_TOLERANCE / 10;
      
  // 1b) Set <L_HVMeas_UPack_Pos> =0  and Set <L_HVMeas_UPack_Neg>=0
  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 200);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 200);
  funcIDE_DeleteAllBP();
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages","SHvDaq16_NoMeasurement = VoltageMeasurementMode == 0;");
  funcIDE_Run();
  funcIDE_WaitForHalt (2000);  // time can be modified
  funcIDE_WriteVariable("VoltageMeasurementMode", NoVoltageMeasurement);
  funcIDE_DeleteAllBP();
  
   // 1c) Wait L_Mem_HVMeas_Timeout_Param ms and check ERR_UPack_Timeout !=RUNNING_FAILED
  for (i = 0; i < facHvMeasTimeout; i++)
  { funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages","SHvDaq16_NoMeasurement = VoltageMeasurementMode == 0;");
    match = getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_UPack_Timeout);
    if (match != RUNNING_FAILED) res_passed++; else res_failed++; 
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);   // time can be modified
    funcIDE_WriteVariable("VoltageMeasurementMode", NoVoltageMeasurement);
    funcIDE_DeleteAllBP();
    funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "CtDaqMgr_SetEventExWithStop(SHvDaq16_NoMeasurement || (VoltageMeasurementMode == 1), bLastInv_B_a,");
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);   // time can be modified
    check_equality(funcIDE_ReadVariable("bLastInv_B_a"), PASSED_INTERNAL, "The timeout error is set to PASSED_INTERNAL", "The timeout error is not set to PASSED_INTERNAL");
    check_equality(funcIDE_ReadVariable("VoltageMeasurementMode"), NoVoltageMeasurement, "The <L_ImocCtrl_ImocState_VoltageMeasurementMode> is set to NoVoltageMeasurement", "The <L_ImocCtrl_ImocState_VoltageMeasurementMode> is not set to NoVoltageMeasurement");
    funcIDE_DeleteAllBP();
    if (i == 10 - 1) teststep("","loop complete,ERR_UPack_Timeout != RUNNING_FAILED -> %d = passed; %d = failed.", res_passed, res_failed);
  }
  check_equality(res_failed, 0, "The Signal on CAN ErrQ_HVMeas_UPackTimeout was always as expected in != RUNNING_FAILED", "The Signal on CAN ErrQ_HVMeas_UPackTimeout switched in unexpected value.");
  
  // 2) Check <ErrQ_HVMeas_UPackTimeout>
  testStep("ENG8_14979_Teststep4.2","Check <ErrQ_HVMeas_UPackTimeout>");
  testCaseComment("");
  func_ValueCompare("Signal on CAN ErrQ_HVMeas_UPackTimeout", getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_UPack_Timeout),RUNNING_FAILED, not_equal);
  
  // 3) Set <L_HVMeas_UPack_Pos> =1  and Set <L_HVMeas_UPack_Neg>=0
  testStep("ENG8_14979_Teststep4.3","Set <L_HVMeas_UPack_Pos> =0  and Set <L_HVMeas_UPack_Neg>=1 "); 
  testCaseComment("");
  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 200);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 32767);
  funcIDE_DeleteAllBP();
   
  // 4) Wait L_Mem_HVMeas_Timeout_Param ms and check ERR_UPack_Timeout !=RUNNING_FAILED
  testStep("ENG8_14979_Teststep4.4","Wait L_Mem_HVMeas_Timeout_Param ms");
  for (i = 0; i < facHvMeasTimeout; i++)
  { funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages","SHvDaq16_NoMeasurement = VoltageMeasurementMode == 0;");
    match = getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_UPack_Timeout);
    if (match != RUNNING_FAILED) res_passed++; else res_failed++; 
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);   // time can be modified
    funcIDE_WriteVariable("VoltageMeasurementMode", NoVoltageMeasurement);
    funcIDE_DeleteAllBP();
    funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "CtDaqMgr_SetEventExWithStop(SHvDaq16_NoMeasurement || (VoltageMeasurementMode == 1), bLastInv_B_a,");
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);   // time can be modified
    check_equality(funcIDE_ReadVariable("bLastInv_B_a"), PASSED_INTERNAL, "The timeout error is set to PASSED_INTERNAL", "The timeout error is not set to PASSED_INTERNAL");
    check_equality(funcIDE_ReadVariable("VoltageMeasurementMode"), NoVoltageMeasurement, "The <L_ImocCtrl_ImocState_VoltageMeasurementMode> is set to NoVoltageMeasurement", "The <L_ImocCtrl_ImocState_VoltageMeasurementMode> is not set to NoVoltageMeasurement");
    funcIDE_DeleteAllBP();
    if (i == 10 - 1) teststep("","loop complete,ERR_UPack_Timeout != RUNNING_FAILED -> %d = passed; %d = failed.", res_passed, res_failed);
  }
  check_equality(res_failed, 0, "The Signal on CAN ErrQ_HVMeas_UPackTimeout was always as expected in != RUNNING_FAILED", "The Signal on CAN ErrQ_HVMeas_UPackTimeout switched in unexpected value.");
  
  // 5) Check <ErrQ_HVMeas_UPackTimeout>
  testStep("ENG8_14979_Teststep4.5","Check <ErrQ_HVMeas_UPackTimeout>");
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages","SHvDaq16_NoMeasurement = VoltageMeasurementMode == 0;");
  res_passed = 0;
  res_failed = 0;
  // Wait for CAN
  for (i = 0; i < 250; i++)
  { match = getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_UPack_Timeout);
    if (match != RUNNING_FAILED) res_passed++; else res_failed++; 
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);   // time can be modified
    funcIDE_WriteVariable("VoltageMeasurementMode", NoVoltageMeasurement);
    if (i == 250 - 1) teststep("","loop complete,ERR_UPack_Timeout != RUNNING_FAILED -> %d = passed; %d = failed.", res_passed, res_failed);
  }
  check_equality(res_failed, 0, "The Signal on CAN ErrQ_HVMeas_UPackTimeout was always as expected in != RUNNING_FAILED", "The Signal on CAN ErrQ_HVMeas_UPackTimeout switched in unexpected value.");
  
  // 6) Set <L_ImocCtrl_ImocState_VoltageMeasurementMode> = TotalVoltageMeasurement
  testStep("ENG8_14979_Teststep1.6","Set <L_ImocCtrl_ImocState_VoltageMeasurementMode> = TotalVoltageMeasurement");
  funcIDE_Run();
  funcIDE_WaitForHalt (2000);  // time can be modified
  funcIDE_WriteVariable("VoltageMeasurementMode", TotalVoltageMeasurement);
  funcIDE_DeleteAllBP();
  
  // 7) Wait L_Mem_HVMeas_Timeout_Param ms and check ERR_UPack_Timeout ==RUNNING_FAILED
  testStep("ENG8_14979_Teststep4.7","Wait L_Mem_HVMeas_Timeout_Param ms");
    
  for (i = 0; i < facHvMeasTimeout; i++)
  { funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages","SHvDaq16_NoMeasurement = VoltageMeasurementMode == 0;");
    match = getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_UPack_Timeout);
    if (match != RUNNING_FAILED) res_passed++; else res_failed++; 
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);   // time can be modified
    funcIDE_WriteVariable("VoltageMeasurementMode", TotalVoltageMeasurement);
    funcIDE_DeleteAllBP();
    funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "CtDaqMgr_SetEventExWithStop(SHvDaq16_NoMeasurement || (VoltageMeasurementMode == 1), bLastInv_B_a,");
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);   // time can be modified
    check_equality(funcIDE_ReadVariable("bLastInv_B_a"), FAILED_INTERNAL, "The timeout error is set to FAILED_INTERNAL", "The timeout error is not set to FAILED_INTERNAL");
    check_equality(funcIDE_ReadVariable("VoltageMeasurementMode"), TotalVoltageMeasurement, "The <L_ImocCtrl_ImocState_VoltageMeasurementMode> is set to TotalVoltageMeasurement", "The <L_ImocCtrl_ImocState_VoltageMeasurementMode> is not set to TotalVoltageMeasurement");
    funcIDE_DeleteAllBP();
    if (i == 10 - 1) teststep("","loop complete,ERR_UPack_Timeout != RUNNING_FAILED -> %d = passed; %d = failed.", res_passed, res_failed);
  }
  check_equality(res_failed, 0, "The Signal on CAN ErrQ_HVMeas_UPackTimeout was always as expected in != RUNNING_FAILED", "The Signal on CAN ErrQ_HVMeas_UPackTimeout switched in unexpected value.");
  
  // 8) Check <ErrQ_HVMeas_UPackTimeout>
  testStep("ENG8_14979_Teststep4.8","Check <ErrQ_HVMeas_UPackTimeout>");
  // Wait for CAN
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages","SHvDaq16_NoMeasurement = VoltageMeasurementMode == 0;");
  res_passed = 0;
  res_failed = 0;
    
  for (i = 0; i < 300; i++)
  { 
    match = getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_UPack_Timeout);
    if (match == RUNNING_FAILED)
      res_passed++; 
    else {
      res_failed++;
      errortype = match;}    
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);   // time can be modified
    funcIDE_WriteVariable("VoltageMeasurementMode", TotalVoltageMeasurement);
    if (i == 300 - 1) teststep("","loop complete,ERR_UPack_Timeout == RUNNING_FAILED -> %d = passed; %d = failed.", res_passed, res_failed);
  }
  check_equality(errortype, 4, "The Signal on CAN ErrQ_HVMeas_UPackTimeout switched also in Status /DEBOUNCING_PREVIOUS_PASS/", "The Signal on CAN ErrQ_HVMeas_UPackTimeout switched in unexpected value.");
  func_ValueCompare("The Signal on CAN ErrQ_HVMeas_UPackTimeout switched also in Status /DEBOUNCING_PREVIOUS_PASS/ as expected.", res_failed, 0, greater_than);
     
  testCaseComment("");
  func_ValueCompare("Signal on CAN ErrQ_HVMeas_UPackTimeout", getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_UPack_Timeout),RUNNING_FAILED, equal);
  
    
  //cleanup
  func_CleanUp();
}


//PLUGIN
//testcase ENG8_4297 (){
//  int match, startTime, endTime, facHvMeasTimeout;
//  
//  testCaseTitle("ENG8_4297","UPack Timeout reset");
//  testCaseDescription("After BMU start the UPack state is set to invalid. After a defined timeout duration the error bit is checked to switch to failed. The UPack state is validated again and the UPack timeout error bit observed to switch to passed.");
//  
//    //Preconditions
//  func_Preconditions();
//  facHvMeasTimeout = funcIDE_ReadVariable("HvDaqMgrParameter.p_facHvMeasTimeout");
//  facHvMeasTimeout = facHvMeasTimeout * 10;
//    
//  // 2) Set <L_HVMeas_UPack_State.bInvalid> == /true/
//  testStep("ENG8_4297.2","Set <L_HVMeas_UPack_State.bInvalid> == /true/"); // true == invalid
//  testCaseComment("");
//  trickMemoryBefore();
//  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1);
//  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 32767);
//  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 600);
//  funcIDE_DeleteAllBP();
//  
//  // 3) Wait L_Mem_HVMeas_Timeout_Param ms
//  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "CtDaqMgr_SetEventExWithStop(!(IsImocRunning), bLastInv_B_a, Event_Err_HVMeas_UPackTimeout, CtDaqMgr_AddInfo_U8);");
//  funcIDE_RunAndWait(2000);
//  startTime = funcIDE_ReadVariable("@STM_CNT");
//  match = testWaitForVariableValue("bLastInv_B_a", FAILED_INTERNAL, 20000);
//  endTime = funcIDE_ReadVariable("@STM_CNT");
//  func_timeCompare ("duration", "ms", (endTime-startTime)/64000, facHvMeasTimeout, equal_or_less_than);
//
//  // 4) Check <ErrQ_HVMeas_UPackTimeout>
//  check_equality(funcIDE_ReadVariable("UPack.State.bInvalid"), TRUE, "<iL_HVMeas_UPack_State.bInvalid> == /true/", "<iL_HVMeas_UPack_State.bInvalid> != /true/");
//  check_equality(match, FAILED_INTERNAL, "The internal timeout error is set to FAILED_INTERNAL", "The internal timeout error is not set to FAILED_INTERNAL");
//  trickMemoryAfter();
//  match = testWaitForSignalMatch(CAN2::SC_ERRORS_DAQ_HALL::ERR_UPack_Timeout, RUNNING_FAILED, 2000);
//  check_equality(match, 1, "The timeout error is set to RUNNING_FAILED", "The timeout error is not set to RUNNING_FAILED");
//  
//  // 5) Ignition switch (Reset)
//  reset_BMS(2000);
//  SetBlemHooksLoop(TRUE);
//  
//  // 6) Set <L_HVMeas_UPack_State.bInvalid> == /true/
//  testStep("ENG8_4297.6","Set <L_HVMeas_UPack_State.bInvalid> == /false/"); // false == valid
//  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "{");
//  funcIDE_RunAndWait(2000);
//  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1);
//  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 600*40);
//  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 300*40);
//  funcIDE_WriteVariable("CtDaqMgr_Xcp_IsPackSna_ACT_U8", 170);
//  funcIDE_WriteVariable("CtDaqMgr_Xcp_IsPackSna_VAL_B", FALSE);
//  funcIDE_DeleteAllBP();
//
//  // 7) Check <ErrQ_HVMeas_UPackTimeout>
//  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "CtDaqMgr_SetEventExWithStop(!(IsImocRunning), bLastInv_B_a, Event_Err_HVMeas_UPackTimeout, CtDaqMgr_AddInfo_U8);");
//  funcIDE_RunAndWait(2000);
//  check_equality(getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_UPack_Timeout), PREFAILED, "The timeout error is set to PREFAILED", "The timeout error is not set to PREFAILED");
//
//  // 8) Wait L_Mem_HVMeas_Timeout_Param ms
//  startTime = funcIDE_ReadVariable("@STM_CNT");
//  match = testWaitForVariableValue("bLastInv_B_a", PASSED_INTERNAL, 20000);
//  endTime = funcIDE_ReadVariable("@STM_CNT");
//  func_timeCompare ("duration", "ms", (endTime-startTime)/64000, facHvMeasTimeout, equal_or_less_than);
//
//  // 9) Check <ErrQ_HVMeas_UPackTimeout>
//  check_equality(funcIDE_ReadVariable("UPack.State.bInvalid"), FALSE, "<iL_HVMeas_UPack_State.bInvalid> != /true/", "<iL_HVMeas_UPack_State.bInvalid> == /true/");
//  check_equality(match, PASSED_INTERNAL, "The timeout error is set to PASSED_INTERNAL", "The timeout error is not set to PASSED_INTERNAL");
//  trickMemoryAfter();
//  match = testWaitForSignalMatch(CAN2::SC_ERRORS_DAQ_HALL::ERR_UPack_Timeout, RUNNING_PASSED, 4000);
//  check_equality(match, 1, "The timeout error is set to RUNNING_PASSED", "The timeout error is not set to RUNNING_PASSED");
//    
//  //cleanup
//  func_CleanUp();
//}
//

testcase ENG8_14980_Teststep1 ()
{
  int match, startTime, endTime, facHvMeasTimeout, i;
  int res_passed, res_failed, errortype;
    
  testCaseTitle("ENG8_14980_Teststep1","UPack Timeout reset");
  testCaseDescription("After BMU start the UPack state is set to invalid. After a defined timeout duration the error bit is checked to switch to failed. After the Check the  ImocState_VoltageMeasurementMode is set to OnlyPackVoltageMeasurement or TotalVoltageMeasurement (depends on table, see bottom Testdescription). The UPack state is validated again and the UPack timeout error bit observed to switch to passed.");
  
  //Preconditions
  func_Preconditions();
  facHvMeasTimeout = funcIDE_ReadVariable("HvDaqMgrParameter.p_facHvMeasTimeout");
    
  // 2) Set <iL_HVMeas_UPack_State.bInvalid>== /true/ <L_ImocCtrl_ImocState_VoltageMeasurementMode> = OnlyPackVoltageMeasurement
  testStep("ENG8_14980_Teststep1.2","Set <L_HVMeas_UPack_State.bInvalid> == /true/"); // true == invalid
  testCaseComment("");
  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 200);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 32767);
  funcIDE_DeleteAllBP();
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "SHvDaq16_NoMeasurement = VoltageMeasurementMode == 0;");
  funcIDE_Run();
  funcIDE_WaitForHalt (2000); // time can be modified
  funcIDE_WriteVariable("VoltageMeasurementMode", OnlyPackVoltageMeasurement );
  funcIDE_DeleteAllBP();
  
  // 3) Wait L_Mem_HVMeas_Timeout_Param ms and check ERR_UPack_Timeout ==RUNNING_FAILED
  testStep("ENG8_14980_Teststep1.3","Wait L_Mem_HVMeas_Timeout_Param ms");
  // Wait for CAN
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages","SHvDaq16_NoMeasurement = VoltageMeasurementMode == 0;");
  res_passed=0;
  res_failed=0;
    
  for (i=0;i<300;i++)
  { 
    match=getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_UPack_Timeout);
    if (match==RUNNING_FAILED)
      res_passed++; 
    else {
      res_failed++;
      errortype=match;}    
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);   // time can be modified
    funcIDE_WriteVariable("VoltageMeasurementMode", OnlyPackVoltageMeasurement);
    if (i==300-1) teststep("","loop complete,ERR_UPack_Timeout == RUNNING_FAILED -> %d = passed; %d = failed.",res_passed,res_failed);
  }
  check_equality(errortype, 4, "The Signal on CAN ErrQ_HVMeas_UPackTimeout switched also in Status /DEBOUNCING_PREVIOUS_PASS/", "The Signal on CAN ErrQ_HVMeas_UPackTimeout switched in unexpected value.");
  func_ValueCompare("The Signal on CAN ErrQ_HVMeas_UPackTimeout switched also in Status /DEBOUNCING_PREVIOUS_PASS/ as expected.", res_failed,0, greater_than);

   // 4) Check <ErrQ_HVMeas_UPackTimeout>
  testStep("ENG8_14980_Teststep1.4","Check <ErrQ_HVMeas_UPackTimeout>");
  testCaseComment("");
  func_ValueCompare("Signal on CAN ErrQ_HVMeas_UPackTimeout", getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_UPack_Timeout),RUNNING_FAILED, equal);
  
  
  // 5) Set <iL_HVMeas_UPack_State.bInvalid>== /false/ <L_ImocCtrl_ImocState_VoltageMeasurementMode> = OnlyPackVoltageMeasurement
  testStep("ENG8_14980_Teststep1.5","Set <iL_HVMeas_UPack_State.bInvalid>== /false/ <L_ImocCtrl_ImocState_VoltageMeasurementMode> = OnlyPackVoltageMeasurement"); // true == invalid
  testCaseComment("");
  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 600*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 300*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_IsPackSna_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_IsPackSna_VAL_B", FALSE);
  funcIDE_DeleteAllBP();
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "SHvDaq16_NoMeasurement = VoltageMeasurementMode == 0;");
  funcIDE_Run();
  funcIDE_WaitForHalt (2000); // time can be modified
  funcIDE_WriteVariable("VoltageMeasurementMode", OnlyPackVoltageMeasurement);
   // Check <ErrQ_HVMeas_UPackTimeout>
  res_passed=0;
  res_failed=0;
  
  for (i=0;i<200;i++)
  { match=getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_UPack_Timeout);
    if (match!=RUNNING_PASSED) res_passed++; else res_failed++; 
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);   // time can be modified
    funcIDE_WriteVariable("VoltageMeasurementMode", OnlyPackVoltageMeasurement);
    if (i==200-1) teststep("","loop complete,ERR_UPack_Timeout != RUNNING_PASSED -> %d = passed; %d = failed.",res_passed,res_failed);
  }
  check_equality(res_failed, 0, "The Signal on CAN ErrQ_HVMeas_UPackTimeout was always as expected in != RUNNING_PASSED", "The Signal on CAN ErrQ_HVMeas_UPackTimeout switched in unexpected value.");

  //''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''' 
  // 6) Ignition switch (Reset)
  //'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
  
  reset_BMS(2000);
  //SetBlemHooksLoop(TRUE);
  SetBlemHooksLoop_afterReset_noRun(TRUE);
  func_Preconditions();
  
  // >ReSet Conditions <iL_HVMeas_UPack_State.bInvalid>== /true/ <L_ImocCtrl_ImocState_VoltageMeasurementMode> = OnlyPackVoltageMeasurement
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "SHvDaq16_NoMeasurement = VoltageMeasurementMode == 0;");
  funcIDE_RunAndWait(4000);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 600*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 300*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_IsPackSna_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_IsPackSna_VAL_B", FALSE);
  funcIDE_WriteVariable("VoltageMeasurementMode", OnlyPackVoltageMeasurement);
  funcIDE_DeleteAllBP();
    
  // 7) check<ErrQ_HVMeas_UPackTimeout>
  testStep("ENG8_14980_Teststep1.7","Check <ErrQ_HVMeas_UPackTimeout>");
  func_ValueCompare("Signal on CAN ErrQ_HVMeas_UPackTimeout", getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_UPack_Timeout),RUNNING_FAILED, not_equal);
  
  // 8) Wait L_Mem_HVMeas_Timeout_Param ms
  testStep("ENG8_14980_Teststep1.8","Wait L_Mem_HVMeas_Timeout_Param ms");
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "CtDaqMgr_SetEventExWithStop(SHvDaq16_NoMeasurement || (VoltageMeasurementMode == 1), bLastInv_B_a,");
  match = testWaitForVariableValue("bLastInv_B_a", PASSED_INTERNAL, 20000);
  check_equality(match, PASSED_INTERNAL, "The timeout error is set to PASSED_INTERNAL", "The timeout error is not set to PASSED_INTERNAL");
  check_equality(funcIDE_ReadVariable("VoltageMeasurementMode"), OnlyPackVoltageMeasurement, "The <L_ImocCtrl_ImocState_VoltageMeasurementMode> is set to OnlyPackVoltageMeasurement", "The <L_ImocCtrl_ImocState_VoltageMeasurementMode> is not set to OnlyPackVoltageMeasurement.");
  funcIDE_DeleteAllBP();
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages","SHvDaq16_NoMeasurement = VoltageMeasurementMode == 0;");
  
  res_passed=0;
  res_failed=0;
    
  for (i=0;i<300;i++)
  { 
    match=getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_UPack_Timeout);
    if (match==RUNNING_PASSED)
      res_passed++; 
    else {
      res_failed++;
    }    
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);   // time can be modified
    funcIDE_WriteVariable("VoltageMeasurementMode", OnlyPackVoltageMeasurement);
    if (i==300-1) teststep("","loop complete,ERR_UPack_Timeout == RUNNING_PASSED -> %d = passed; %d = failed.",res_passed,res_failed);
  }
  
   // 9) Check <ErrQ_HVMeas_UPackTimeout>
  check_equality(getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_UPack_Timeout), 0, "The timeout error is set to RUNNING_PASSED", "The timeout error is not set to RUNNING_PASSED");
  trickMemoryAfter();
    
  //cleanup
  func_CleanUp();
}


testcase ENG8_14980_Teststep2 ()
{
  int match, startTime, endTime, facHvMeasTimeout, i;
  int res_passed, res_failed, errortype;
    
  testCaseTitle("ENG8_14980_Teststep2","UPack Timeout reset");
  testCaseDescription("After BMU start the UPack state is set to invalid. After a defined timeout duration the error bit is checked to switch to failed. After the Check the  ImocState_VoltageMeasurementMode is set to OnlyPackVoltageMeasurement or TotalVoltageMeasurement (depends on table, see bottom Testdescription). The UPack state is validated again and the UPack timeout error bit observed to switch to passed.");
  
  //Preconditions
  func_Preconditions();
  facHvMeasTimeout = funcIDE_ReadVariable("HvDaqMgrParameter.p_facHvMeasTimeout");
    
  // 2) Set <iL_HVMeas_UPack_State.bInvalid>== /true/ <L_ImocCtrl_ImocState_VoltageMeasurementMode> = TotalVoltageMeasurement 
  testStep("ENG8_14980_Teststep2.2","Set <L_HVMeas_UPack_State.bInvalid> == /true/"); // true == invalid
  testCaseComment("");
  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 200);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 32767);
  funcIDE_DeleteAllBP();
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "SHvDaq16_NoMeasurement = VoltageMeasurementMode == 0;");
  funcIDE_Run();
  funcIDE_WaitForHalt (2000); // time can be modified
  funcIDE_WriteVariable("VoltageMeasurementMode", TotalVoltageMeasurement  );
  funcIDE_DeleteAllBP();
  
  // 3) Wait L_Mem_HVMeas_Timeout_Param ms and check ERR_UPack_Timeout ==RUNNING_FAILED
  testStep("ENG8_14980_Teststep2.3","Wait L_Mem_HVMeas_Timeout_Param ms");
  // Wait for CAN
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages","SHvDaq16_NoMeasurement = VoltageMeasurementMode == 0;");
  res_passed=0;
  res_failed=0;
    
  for (i=0;i<300;i++)
  { 
    match=getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_UPack_Timeout);
    if (match==RUNNING_FAILED)
      res_passed++; 
    else {
      res_failed++;
      errortype=match;}    
    funcIDE_Run();
    funcIDE_WaitForHalt (2000); // time can be modified
    funcIDE_WriteVariable("VoltageMeasurementMode", TotalVoltageMeasurement );
    if (i==300-1) teststep("","loop complete,ERR_UPack_Timeout == RUNNING_FAILED -> %d = passed; %d = failed.",res_passed,res_failed);
  }
  check_equality(errortype, 4, "The Signal on CAN ErrQ_HVMeas_UPackTimeout switched also in Status /DEBOUNCING_PREVIOUS_PASS/", "The Signal on CAN ErrQ_HVMeas_UPackTimeout switched in unexpected value.");
  func_ValueCompare("The Signal on CAN ErrQ_HVMeas_UPackTimeout switched also in Status /DEBOUNCING_PREVIOUS_PASS/ as expected.", res_failed,0, greater_than);

   // 4) Check <ErrQ_HVMeas_UPackTimeout>
  testStep("ENG8_14980_Teststep2.4","Check <ErrQ_HVMeas_UPackTimeout>");
  testCaseComment("");
  func_ValueCompare("Signal on CAN ErrQ_HVMeas_UPackTimeout", getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_UPack_Timeout),RUNNING_FAILED, equal);
  
  
  // 5) Set <iL_HVMeas_UPack_State.bInvalid>== /false/ <L_ImocCtrl_ImocState_VoltageMeasurementMode> = TotalVoltageMeasurement 
  testStep("ENG8_14980_Teststep2.5","Set <iL_HVMeas_UPack_State.bInvalid>== /false/ <L_ImocCtrl_ImocState_VoltageMeasurementMode> = TotalVoltageMeasurement "); // true == invalid
  testCaseComment("");
  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 600*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 300*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_IsPackSna_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_IsPackSna_VAL_B", FALSE);
  funcIDE_DeleteAllBP();
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "SHvDaq16_NoMeasurement = VoltageMeasurementMode == 0;");
  funcIDE_Run();
  funcIDE_WaitForHalt (2000); // time can be modified
  funcIDE_WriteVariable("VoltageMeasurementMode", TotalVoltageMeasurement );
   // Check <ErrQ_HVMeas_UPackTimeout>
  res_passed=0;
  res_failed=0;
  
  for (i=0;i<200;i++)
  { match=getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_UPack_Timeout);
    if (match!=RUNNING_PASSED) res_passed++; else res_failed++; 
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);   // time can be modified
    funcIDE_WriteVariable("VoltageMeasurementMode", TotalVoltageMeasurement );
    if (i==200-1) teststep("","loop complete,ERR_UPack_Timeout != RUNNING_PASSED -> %d = passed; %d = failed.",res_passed,res_failed);
  }
  check_equality(res_failed, 0, "The Signal on CAN ErrQ_HVMeas_UPackTimeout was always as expected in != RUNNING_PASSED", "The Signal on CAN ErrQ_HVMeas_UPackTimeout switched in unexpected value.");

  //''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''' 
  // 6) Ignition switch (Reset)
  //'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
  
  reset_BMS(2000);
  //SetBlemHooksLoop(TRUE);
  SetBlemHooksLoop_afterReset_noRun(TRUE);
  func_Preconditions();
  
  // >ReSet Conditions <iL_HVMeas_UPack_State.bInvalid>== /true/ <L_ImocCtrl_ImocState_VoltageMeasurementMode> = TotalVoltageMeasurement 
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "SHvDaq16_NoMeasurement = VoltageMeasurementMode == 0;");
  funcIDE_RunAndWait(4000);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 600*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 300*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_IsPackSna_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_IsPackSna_VAL_B", FALSE);
  funcIDE_WriteVariable("VoltageMeasurementMode", TotalVoltageMeasurement );
  funcIDE_DeleteAllBP();
    
  // 7) check<ErrQ_HVMeas_UPackTimeout>
  testStep("ENG8_14980_Teststep2.7","Check <ErrQ_HVMeas_UPackTimeout>");
  func_ValueCompare("Signal on CAN ErrQ_HVMeas_UPackTimeout", getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_UPack_Timeout),RUNNING_FAILED, not_equal);
  
  // 8) Wait L_Mem_HVMeas_Timeout_Param ms
  testStep("ENG8_14980_Teststep2.8","Wait L_Mem_HVMeas_Timeout_Param ms");
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "CtDaqMgr_SetEventExWithStop(SHvDaq16_NoMeasurement || (VoltageMeasurementMode == 1), bLastInv_B_a,");
  match = testWaitForVariableValue("bLastInv_B_a", PASSED_INTERNAL, 20000);
  check_equality(match, PASSED_INTERNAL, "The timeout error is set to PASSED_INTERNAL", "The timeout error is not set to PASSED_INTERNAL");
  check_equality(funcIDE_ReadVariable("VoltageMeasurementMode"), TotalVoltageMeasurement , "The <L_ImocCtrl_ImocState_VoltageMeasurementMode> is set to TotalVoltageMeasurement ", "The <L_ImocCtrl_ImocState_VoltageMeasurementMode> is not set to TotalVoltageMeasurement .");
  funcIDE_DeleteAllBP();
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages","SHvDaq16_NoMeasurement = VoltageMeasurementMode == 0;");
  
  res_passed=0;
  res_failed=0;
    
  for (i=0;i<300;i++)
  { 
    match=getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_UPack_Timeout);
    if (match==RUNNING_PASSED)
      res_passed++; 
    else {
      res_failed++;
    }    
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);   // time can be modified
    funcIDE_WriteVariable("VoltageMeasurementMode", OnlyPackVoltageMeasurement);
    if (i==300-1) teststep("","loop complete,ERR_UPack_Timeout == RUNNING_PASSED -> %d = passed; %d = failed.",res_passed,res_failed);
  }
  
   // 9) Check <ErrQ_HVMeas_UPackTimeout>
  check_equality(getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_UPack_Timeout), 0, "The timeout error is set to RUNNING_PASSED", "The timeout error is not set to RUNNING_PASSED");
  trickMemoryAfter();
    
  //cleanup
  func_CleanUp();
}

/*************************************************************
*###### Link Voltage Fault Detection and Error Handling ######
**************************************************************/

testcase ENG8_4298 ()
{
  char CANSignal[100] = "SC_ERRORS_DAQ::ERR_ULink_OverVltg";
  int ULinkHighThres;
  
  testCaseTitle("ENG8_4298","ULink overvoltage");
  testCaseDescription("After BMU start the ULink overvoltage error bit is initially checked to be unequal to failed. The ULink value is set to a value above the overvoltage threshold and the overvoltage error bit is observed to switch failed.");

  //preconditions
  func_Preconditions();
  ULinkHighThres = funcIDE_ReadVariable("HvDaqMgrParameter.p_ULinkHighThres");
  ULinkHighThres = ULinkHighThres / 40;
  teststep("","");
  
  // 2) Check <ErrQ_HVMeas_ULinkOvervoltage>
  testStep("ENG8_4298.2","Check %s initially", CANSignal);
  teststep("","");
  func_ValueCompare("Check on CAN <ErrQ_HVMeas_ULinkOvervoltage>", getSignal(CAN2::SC_ERRORS_DAQ::ERR_ULink_OverVltg), RUNNING_FAILED, not_equal);

  // 3) Set <P_HVMeas_ULink> = <L_Mem_HVMeas_ThresholdLinkUOvervoltage_Param> + 1unit
  testStep("ENG8_4298.3a","Set HV contactor state to /Connected/.");
  trickMemoryBefore();
  funcIDE_WriteVariable("ContDiag_Xcp_ContMainPos_State_out_ACT_U8", 170);
  funcIDE_WriteVariable("ContDiag_Xcp_ContMainPos_State_out_VAL_U8", CONT_CLOSED);
  funcIDE_WriteVariable("ContDiag_Xcp_ContMainNeg_State_out_ACT_U8", 170);
  funcIDE_WriteVariable("ContDiag_Xcp_ContMainNeg_State_out_VAL_U8", CONT_CLOSED);
  teststep("","");
  testStep("ENG8_4298.3b","Set ULink greater than overvoltage threshold\n\n<P_HVMeas_ULink> = <L_Mem_HVMeas_ThresholdLinkUOvervoltage_Param> + 1unit");
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", (ULinkHighThres + 1) / 0.025);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
  trickMemoryAfter();
  testWaitForTimeout(10000);  
  teststep("","");
  // 4) Check <ErrQ_HVMeas_ULinkOvervoltage> 
  testStep("ENG8_4298.4","Check %s finally", CANSignal);
  testCaseComment("");
  func_ValueCompare("Check on CAN <ErrQ_HVMeas_ULinkOvervoltage>", getSignal(CAN2::SC_ERRORS_DAQ::ERR_ULink_OverVltg), RUNNING_FAILED, equal);

  //cleanup
  func_CleanUp();
}

testcase ENG8_4299 ()
{
  char CANSignal[100] = "SC_ERRORS_DAQ::ERR_ULink_OverVltg";
  int ULinkHighThres, UPackHyst;
  
  testCaseTitle("ENG8_4299","ULink no overvoltage ");
  testCaseDescription("After BMU start the ULink overvoltage error bit is initially checked to be unequal to passed. The ULink value is set to a value equal to the overvoltage threshold and the overvoltage error bit is observed to switch passed.");

  //preconditions
  func_Preconditions();
  ULinkHighThres = funcIDE_ReadVariable("HvDaqMgrParameter.p_ULinkHighThres");
  ULinkHighThres = ULinkHighThres / 40;
  UPackHyst = funcIDE_ReadVariable("HvDaqMgrParameter.p_UPackHyst");
  UPackHyst = UPackHyst / 40;
  teststep("","");
  
  // 1) Set <P_HVMeas_ULink> = <L_Mem_HVMeas_ThresholdLinkUOvervoltage_Param> + 1unit
  testStep("ENG8_4299.1a","Set HV contactor state to /Connected/.");
  trickMemoryBefore();
  funcIDE_WriteVariable("ContDiag_Xcp_ContMainPos_State_out_ACT_U8", 170);
  funcIDE_WriteVariable("ContDiag_Xcp_ContMainPos_State_out_VAL_U8", CONT_CLOSED);
  funcIDE_WriteVariable("ContDiag_Xcp_ContMainNeg_State_out_ACT_U8", 170);
  funcIDE_WriteVariable("ContDiag_Xcp_ContMainNeg_State_out_VAL_U8", CONT_CLOSED);
  teststep("","");
  testStep("ENG8_4299.1b","Set ULink greater than overvoltage threshold\n\n<P_HVMeas_ULink> = <L_Mem_HVMeas_ThresholdLinkUOvervoltage_Param> + 1unit");
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", (ULinkHighThres + 1) / 0.025);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
  trickMemoryAfter();
  testWaitForTimeout(10000);  
  teststep("","");

  // 2) Check <ErrQ_HVMeas_ULinkOvervoltage>
  testStep("ENG8_4299.2","Check %s initially", CANSignal);
  testCaseComment("");
  func_ValueCompare("Check on CAN <ErrQ_HVMeas_ULinkOvervoltage>", getSignal(CAN2::SC_ERRORS_DAQ::ERR_ULink_OverVltg), RUNNING_PASSED, not_equal);

  // 3) Set <P_HVMeas_ULink> = <L_Mem_HVMeas_ThresholdLinkUOvervoltage_Param> - <L_Mem_HVMeas_OvervoltageHysteresis_Param> 
  testStep("ENG8_4299.2","Set ULink smaller than overvoltage threshold minus hysteresis\n\n<P_HVMeas_ULink> = <L_Mem_HVMeas_ThresholdLinkUOvervoltage_Param> - <L_Mem_HVMeas_OvervoltageHysteresis_Param>");
  testCaseComment("");
  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", (ULinkHighThres - UPackHyst /*-1*/) / 0.025);  
  trickMemoryAfter();
  testWaitForTimeout(10000); 
  
  // 4) Check <ErrQ_HVMeas_ULinkOvervoltage>
  testStep("ENG8_4299.2","Check %s finally", CANSignal);
  testCaseComment("");
  func_ValueCompare("Check on CAN <ErrQ_HVMeas_ULinkOvervoltage>", getSignal(CAN2::SC_ERRORS_DAQ::ERR_ULink_OverVltg), RUNNING_PASSED, equal);
  
  // cleanup
  func_CleanUp();
  
}

testcase ENG8_4300 ()
{
  int ULinkOutOfRangeHi;
  
  testCaseTitle("ENG8_4300","ULink out of range - upper limit");
  testCaseDescription("After BMU start the ULink out of range error bit for the upper limit is initially checked to be unequal to failed. The ULink value is set to a value above the out of range threshold and the out of range error bit for the upper limit is observed to switch failed.");
  
  // preconditions
  func_Preconditions();
  ULinkOutOfRangeHi = funcIDE_ReadVariable("HvDaqMgrParameter.p_ULinkOutOfRangeHi");
  ULinkOutOfRangeHi = ULinkOutOfRangeHi / 40;
  
  // 1) Set <P_HVMeas_ULink> = <L_Mem_HVMeas_ThresholdLinkUOutOfRangeHigh_Param> + 1unit
  testStep("ENG8_4300.1a","Set HV contactor state to /Connected/.");
  trickMemoryBefore();
  funcIDE_WriteVariable("ContDiag_Xcp_ContMainPos_State_out_ACT_U8", 170);
  funcIDE_WriteVariable("ContDiag_Xcp_ContMainPos_State_out_VAL_U8", CONT_CLOSED);
  funcIDE_WriteVariable("ContDiag_Xcp_ContMainNeg_State_out_ACT_U8", 170);
  funcIDE_WriteVariable("ContDiag_Xcp_ContMainNeg_State_out_VAL_U8", CONT_CLOSED);
  teststep("","");
  testStep("ENG8_4300.1b","Set ULink <L_Mem_HVMeas_ThresholdLinkUOutOfRangeHigh_Param> \n\n<P_HVMeas_ULink> = <L_Mem_HVMeas_ThresholdLinkUOutOfRangeHigh_Param>> ");
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", ULinkOutOfRangeHi / 0.025);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
  trickMemoryAfter();
  testWaitForTimeout(5000);  
  teststep("","");
  
  // 2) Check ErrQ_HVMeas_ULinkOutOfRangeHigh>
  testStep("ENG8_4300.2","Check ErrQ_HVMeas_ULinkOutOfRangeHigh>");
  testCaseComment("");
  func_ValueCompare("Check ErrQ_HVMeas_ULinkOutOfRangeHigh>", getSignal(CAN2::SC_ERRORS_DAQ::ERR_ULink_RNGH), RUNNING_FAILED, not_equal);

  // 3) Set <P_HVMeas_ULink> = <L_Mem_HVMeas_ThresholdLinkUOutOfRangeHigh_Param> + 1unit
  testStep("ENG8_4300.2","");
  testCaseComment("");
  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", (ULinkOutOfRangeHi + 1) / 0.025);
  trickMemoryAfter();
  testWaitForTimeout(5000);
  
  // 4) Check <ErrQ_HVMeas_ULinkOutOfRangeHigh>
  testStep("ENG8_4300.2","");
  testCaseComment("");
  func_ValueCompare("Check ErrQ_HVMeas_ULinkOutOfRangeHigh>", getSignal(CAN2::SC_ERRORS_DAQ::ERR_ULink_RNGH), RUNNING_FAILED, equal);

  //cleanup
  func_Cleanup();
}

testcase ENG8_4301 ()
{
  int ULinkOutOfRangeHi;
  
  testCaseTitle("ENG8_4301","ULink not out of range - upper limit");
  testCaseDescription("After BMU start the ULink out of range error bit for the upper limit is initially checked to be unequal to passed. The ULink value is set to a value equal to the out of range threshold and the out of range error bit for the upper limit is observed to switch passed.");
  
  //Preconditions
  func_Preconditions();
  ULinkOutOfRangeHi = funcIDE_ReadVariable("HvDaqMgrParameter.p_ULinkOutOfRangeHi");
  ULinkOutOfRangeHi = ULinkOutOfRangeHi / 40;
  
  // 1) Set <P_HVMeas_ULink> = <L_Mem_HVMeas_ThresholdLinkUOutOfRangeHigh_Param> + 1unit
  testStep("ENG8_4301.1a","Set HV contactor state to /Connected/.");
  trickMemoryBefore();
  funcIDE_WriteVariable("ContDiag_Xcp_ContMainPos_State_out_ACT_U8", 170);
  funcIDE_WriteVariable("ContDiag_Xcp_ContMainPos_State_out_VAL_U8", 2);
  funcIDE_WriteVariable("ContDiag_Xcp_ContMainNeg_State_out_ACT_U8", 170);
  funcIDE_WriteVariable("ContDiag_Xcp_ContMainNeg_State_out_VAL_U8", 2);
  teststep("","");
  testStep("ENG8_4301.1b","Set ULink <L_Mem_HVMeas_ThresholdLinkUOutOfRangeHigh_Param> \n\n<P_HVMeas_ULink> = <L_Mem_HVMeas_ThresholdLinkUOutOfRangeHigh_Param>> ");
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", (ULinkOutOfRangeHi + 1) / 0.025);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
  trickMemoryAfter();
  testWaitForTimeout(5000);  
  teststep("","");
  
  //2) Check <ErrQ_HVMeas_ULinkOutOfRangeHigh>
  testStep("ENG8_4301.2","");
  testCaseComment("");
  func_ValueCompare("Check ErrQ_HVMeas_ULinkOutOfRangeHigh>", getSignal(CAN2::SC_ERRORS_DAQ::ERR_ULink_RNGH), RUNNING_PASSED, not_equal);


  //3) Set <P_HVMeas_ULink> = <L_Mem_HVMeas_ThresholdLinkUOutOfRangeHigh_Param> 
  testStep("ENG8_4301.3","Set <P_HVMeas_ULink> = <L_Mem_HVMeas_ThresholdLinkUOutOfRangeHigh_Param> ");
  testCaseComment("");
  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", ULinkOutOfRangeHi / 0.025);
  trickMemoryAfter();
  testWaitForTimeout(5000);  

  //4) Check <ErrQ_HVMeas_ULinkOutOfRangeHigh> 
  testStep("ENG8_4301.4","Check <ErrQ_HVMeas_ULinkOutOfRangeHigh>> ");
  testCaseComment("");
  func_ValueCompare("Check ErrQ_HVMeas_ULinkOutOfRangeHigh>", getSignal(CAN2::SC_ERRORS_DAQ::ERR_ULink_RNGH), RUNNING_PASSED, equal);
  
  // cleanup
  func_CleanUp();
}
//
//testcase ENG8_4304 ()
//{
//  int ULinkOutOfRangeHi, startTime, endTime, match, facHvMeasTimeout;
//  
//  testCaseTitle("ENG8_4304","ULink Timeout");
//  testCaseDescription("After BMU start the ULink timeout error bit is initially checked to be unequal to failed. The ULink state is set to invalid. After a defined timeout duration the error bit is checked to switch to failed.");
//  
//  //Preconditions
//  func_Preconditions();
//  ULinkOutOfRangeHi = funcIDE_ReadVariable("HvDaqMgrParameter.p_ULinkOutOfRangeHi");
//  ULinkOutOfRangeHi = ULinkOutOfRangeHi / 40;
//  facHvMeasTimeout = funcIDE_ReadVariable("HvDaqMgrParameter.p_facHvMeasTimeout");
//  facHvMeasTimeout = facHvMeasTimeout * 10;
//  
//  // 2) Check <ErrQ_HVMeas_ULinkTimeout>
//  testStep("ENG8_4304.2","Check <ErrQ_HVMeas_ULinkTimeout>");
//  testCaseComment("");
//  func_ValueCompare("Check <ErrQ_HVMeas_ULinkTimeout>", getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_ULink_Timeout), RUNNING_FAILED, not_equal);
//
//  // 3) Set <P_HVMeas_ULink> = <L_Mem_HVMeas_ThresholdLinkUOutOfRangeHigh_Param> + 1unit
//  testStep("ENG8_4304.3a","Set HV contactor state to /Connected/.");
//  Func_ContCtrl_MainPos(ENABLE_Cont);
//  Func_ContCtrl_MainNeg(ENABLE_Cont);
//  teststep("","");
//  testStep("ENG8_4304.3b","Set ULink <L_Mem_HVMeas_ThresholdLinkUOutOfRangeHigh_Param> \n\n<P_HVMeas_ULink> = <L_Mem_HVMeas_ThresholdLinkUOutOfRangeHigh_Param>> ");
//  trickMemoryBefore();
//  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1);
//  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", (ULinkOutOfRangeHi + 1) * 40);
//  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
//  funcIDE_WriteVariable("CtDaqMgr_Xcp_IsLinkSna_ACT_U8", 170);
//  funcIDE_WriteVariable("CtDaqMgr_Xcp_IsLinkSna_VAL_B", 1);
//  trickMemoryAfter();
//  teststep("","");
//
//  // 4) Wait L_Mem_HVMeas_Timeout_Param ms
//  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "   CtDaqMgr_SetEventExWithStop(!(IsImocRunning), bLastInv_B, Event_Err_HVMeas_ULinkTimeout, CtDaqMgr_AddInfo_U8);");
//  funcIDE_RunAndWait(2000);
//  startTime = funcIDE_ReadVariable("osSystemCounter");
//  match = testWaitForVariableValue("bLastInv_B", FAILED_INTERNAL, 20000);
//  endTime = funcIDE_ReadVariable("osSystemCounter");
//  check_equality(match, FAILED_INTERNAL, "The timeout error is set to FAILED_INTERNAL", "The timeout error is not set to FAILED_INTERNAL");
//  func_timeCompare ("duration", "ms", endTime-startTime, facHvMeasTimeout, equal_or_less_than);
//  trickMemoryAfter();
//  testWaitForTimeout(4000);
//
//  // 5) Check <ErrQ_HVMeas_ULinkTimeout>
//  testStep("ENG8_4304.2","Check <ErrQ_HVMeas_ULinkTimeout>");
//  testCaseComment("");
//  func_ValueCompare("Check <ErrQ_HVMeas_ULinkTimeout>", getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_ULink_Timeout), RUNNING_FAILED, equal);
//  
//  //cleanup
//  func_cleanUp();
//}


testcase ENG8_14981_Teststep1 ()
  
{
  int match, facHvMeasTimeout, i;
  int res_passed, res_failed, errortype;
  
  testCaseTitle("ENG8_14981_Teststep1","ULink Timeout");
  testCaseDescription("After BMU start the ULink timeout error bit is initially checked to be unequal to failed. The ULink state is set to invalid and the  ImocState_VoltageMeasurementMode is set to OnlyLinkVoltageMeasurement or TotalVoltageMeasurement (depends on table, see bottom Testdescription).  After a defined timeout duration the error bit is checked to switch to failed");

  //Preconditions
  func_Preconditions();         
  facHvMeasTimeout = L_Mem_HVMeas_Timeout_Param_WITH_TOLERANCE / 10;
      
  // 1b)<iL_HVMeas_ULink_State.bInvalid> = /false and <L_ImocCtrl_ImocState_VoltageMeasurementMode> = NoVoltageMeasurement

  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", 200);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 200);
  funcIDE_DeleteAllBP();
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages","SHvDaq16_NoMeasurement = VoltageMeasurementMode == 0;");
  funcIDE_Run();
  funcIDE_WaitForHalt (2000); // time can be modified
  funcIDE_WriteVariable("VoltageMeasurementMode", NoVoltageMeasurement);
  funcIDE_DeleteAllBP();
  
   // 1c) Wait L_Mem_HVMeas_Timeout_Param ms and check ERR_ULink_Timeout !=RUNNING_FAILED
  for (i = 0; i < facHvMeasTimeout; i++)
  { funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages","SHvDaq16_NoMeasurement = VoltageMeasurementMode == 0;");
    match = getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_ULink_Timeout);
    if (match != RUNNING_FAILED) res_passed++; else res_failed++; 
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);   // time can be modified
    funcIDE_WriteVariable("VoltageMeasurementMode", NoVoltageMeasurement);
    funcIDE_DeleteAllBP();
    funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "CtDaqMgr_SetEventExWithStop(SHvDaq16_NoMeasurement || (VoltageMeasurementMode == 2), bLastInv_B,");
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);   // time can be modified
    check_equality(funcIDE_ReadVariable("bLastInv_B"), PASSED_INTERNAL, "The timeout error is set to PASSED_INTERNAL", "The timeout error is not set to PASSED_INTERNAL");
    check_equality(funcIDE_ReadVariable("VoltageMeasurementMode"), NoVoltageMeasurement, "The <L_ImocCtrl_ImocState_VoltageMeasurementMode> is set to NoVoltageMeasurement", "The <L_ImocCtrl_ImocState_VoltageMeasurementMode> is not set to NoVoltageMeasurement");
    funcIDE_DeleteAllBP();
    if (i == 10 - 1) teststep("","loop complete,ERR_ULink_Timeout != RUNNING_FAILED -> %d = passed; %d = failed.", res_passed, res_failed);
  }
  check_equality(res_failed, 0, "The Signal on CAN ErrQ_HVMeas_ULinkTimeout was always as expected in != RUNNING_FAILED", "The Signal on CAN ErrQ_HVMeas_ULinkTimeout switched in unexpected value.");
  
  // 2) Check <ErrQ_HVMeas_ULinkTimeout>
  testStep("ENG8_14981_Teststep1.2","Check <ErrQ_HVMeas_ULinkTimeout>");
  testCaseComment("");
  func_ValueCompare("Signal on CAN ErrQ_HVMeas_ULinkTimeout", getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_ULink_Timeout),RUNNING_FAILED, not_equal);
  
  // 3) Set <L_HVMeas_ULink_Pos> =0  and Set <L_HVMeas_ULink_Neg>=1 
  testStep("ENG8_14981_Teststep1.3","Set <L_HVMeas_ULink_Pos> =0  and Set <L_HVMeas_ULink_Neg>=1 "); 
  testCaseComment("");
  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", 200);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 32767);
  funcIDE_WriteVariable("ContCtrl_Xcp_ContMainPos_State_in_ACT_U8", 170);
  funcIDE_WriteVariable("ContCtrl_Xcp_ContMainPos_State_in_VAL_U8", CNT_CLOSED);
  funcIDE_WriteVariable("ContCtrl_Xcp_ContMainNeg_State_in_ACT_U8", 170);
  funcIDE_WriteVariable("ContCtrl_Xcp_ContMainNeg_State_in_VAL_U8", CNT_CLOSED);
  funcIDE_DeleteAllBP();
   
  // 4) Wait L_Mem_HVMeas_Timeout_Param ms and check ERR_ULink_Timeout !=RUNNING_FAILED
  testStep("ENG8_14981_Teststep1.4","Wait L_Mem_HVMeas_Timeout_Param ms");
  for (i = 0; i < facHvMeasTimeout; i++)
  { funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages","SHvDaq16_NoMeasurement = VoltageMeasurementMode == 0;");
    match = getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_ULink_Timeout);
    if (match != RUNNING_FAILED) res_passed++; else res_failed++; 
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);   // time can be modified
    funcIDE_WriteVariable("VoltageMeasurementMode", NoVoltageMeasurement);
    funcIDE_DeleteAllBP();
    funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "CtDaqMgr_SetEventExWithStop(SHvDaq16_NoMeasurement || (VoltageMeasurementMode == 2), bLastInv_B,");
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);   // time can be modified
    check_equality(funcIDE_ReadVariable("bLastInv_B"), PASSED_INTERNAL, "The timeout error is set to PASSED_INTERNAL", "The timeout error is not set to PASSED_INTERNAL");
    check_equality(funcIDE_ReadVariable("VoltageMeasurementMode"), NoVoltageMeasurement, "The <L_ImocCtrl_ImocState_VoltageMeasurementMode> is set to NoVoltageMeasurement", "The <L_ImocCtrl_ImocState_VoltageMeasurementMode> is not set to NoVoltageMeasurement");
    funcIDE_DeleteAllBP();
    if (i == 10 - 1) teststep("","loop complete,ERR_ULink_Timeout != RUNNING_FAILED -> %d = passed; %d = failed.", res_passed, res_failed);
  }
  check_equality(res_failed, 0, "The Signal on CAN ErrQ_HVMeas_ULinkTimeout was always as expected in != RUNNING_FAILED", "The Signal on CAN ErrQ_HVMeas_ULinkTimeout switched in unexpected value.");
  
  // 5) Check <ErrQ_HVMeas_ULinkTimeout>
  testStep("ENG8_14981_Teststep1.5","Check <ErrQ_HVMeas_ULinkTimeout>");
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages","SHvDaq16_NoMeasurement = VoltageMeasurementMode == 0;");
  res_passed = 0;
  res_failed = 0;
  // Wait for CAN
  for (i = 0; i < 250; i++)
  { match = getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_ULink_Timeout);
    if (match != RUNNING_FAILED) res_passed++; else res_failed++; 
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);   // time can be modified
    funcIDE_WriteVariable("VoltageMeasurementMode", NoVoltageMeasurement);
    if (i == 250 - 1) teststep("","loop complete,ERR_ULink_Timeout != RUNNING_FAILED -> %d = passed; %d = failed.", res_passed, res_failed);
  }
  check_equality(res_failed, 0, "The Signal on CAN ErrQ_HVMeas_ULinkTimeout was always as expected in != RUNNING_FAILED", "The Signal on CAN ErrQ_HVMeas_ULinkTimeout switched in unexpected value.");
  
  // 6) Set <L_ImocCtrl_ImocState_VoltageMeasurementMode> = OnlyLinkVoltageMeasurement
  testStep("ENG8_14981_Teststep1.6","Set <L_ImocCtrl_ImocState_VoltageMeasurementMode> = OnlyLinkVoltageMeasurement");
  funcIDE_Run();
  funcIDE_WaitForHalt (2000); // time can be modified
  funcIDE_WriteVariable("VoltageMeasurementMode", OnlyLinkVoltageMeasurement);
  funcIDE_DeleteAllBP();
  
  // 7) Wait L_Mem_HVMeas_Timeout_Param ms and check ERR_ULink_Timeout ==RUNNING_FAILED
  testStep("ENG8_14981_Teststep1.7","Wait L_Mem_HVMeas_Timeout_Param ms");
    
  for (i = 0; i < facHvMeasTimeout; i++)
  { funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages","SHvDaq16_NoMeasurement = VoltageMeasurementMode == 0;");
    match = getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_ULink_Timeout);
    if (match != RUNNING_FAILED) res_passed++; else res_failed++; 
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);   // time can be modified
    funcIDE_WriteVariable("VoltageMeasurementMode", OnlyLinkVoltageMeasurement);
    funcIDE_DeleteAllBP();
    funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "CtDaqMgr_SetEventExWithStop(SHvDaq16_NoMeasurement || (VoltageMeasurementMode == 2), bLastInv_B,");
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);   // time can be modified
    check_equality(funcIDE_ReadVariable("bLastInv_B"), FAILED_INTERNAL, "The timeout error was set to FAILED_INTERNAL", "The timeout error is not set to FAILED_INTERNAL");
    check_equality(funcIDE_ReadVariable("VoltageMeasurementMode"), OnlyLinkVoltageMeasurement, "The <L_ImocCtrl_ImocState_VoltageMeasurementMode> is set to OnlyLinkVoltageMeasurement", "The <L_ImocCtrl_ImocState_VoltageMeasurementMode> is not set to OnlyLinkVoltageMeasurement");
    funcIDE_DeleteAllBP();
    if (i == 10 - 1) teststep("","loop complete,ERR_ULink_Timeout != RUNNING_FAILED -> %d = passed; %d = failed.", res_passed, res_failed);
  }
  check_equality(res_failed, 0, "The Signal on CAN ErrQ_HVMeas_ULinkTimeout was always as expected in != RUNNING_FAILED", "The Signal on CAN ErrQ_HVMeas_ULinkTimeout switched in unexpected value.");
  
  // 8) Check <ErrQ_HVMeas_ULinkTimeout>
  testStep("ENG8_14981_Teststep1.8","Check <ErrQ_HVMeas_ULinkTimeout>");
  // Wait for CAN
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages","SHvDaq16_NoMeasurement = VoltageMeasurementMode == 0;");
  res_passed = 0;
  res_failed = 0;
    
  for (i = 0; i < 300; i++)
  { 
    match = getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_ULink_Timeout);
    if (match == RUNNING_FAILED)
      res_passed++; 
    else {
      res_failed++;
      errortype = match;}    
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);   // time can be modified
    funcIDE_WriteVariable("VoltageMeasurementMode", OnlyLinkVoltageMeasurement);
    if (i == 300 - 1) teststep("","loop complete,ERR_UPack_Timeout == RUNNING_FAILED -> %d = passed; %d = failed.", res_passed, res_failed);
  }
  check_equality(errortype, 4, "The Signal on CAN ErrQ_HVMeas_ULinkTimeout switched also in Status /DEBOUNCING_PREVIOUS_PASS/", "The Signal on CAN ErrQ_HVMeas_ULinkTimeout switched in unexpected value.");
  func_ValueCompare("The Signal on CAN ErrQ_HVMeas_ULinkTimeout switched also in Status /DEBOUNCING_PREVIOUS_PASS/ as expected.", res_failed, 0, greater_than);
     
  testCaseComment("");
  func_ValueCompare("Signal on CAN ErrQ_HVMeas_ULinkTimeout", getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_ULink_Timeout),RUNNING_FAILED, equal);
  
    
  //cleanup
  func_CleanUp();
}



testcase ENG8_14981_Teststep2 ()
  
{
  int match, facHvMeasTimeout, i;
  int res_passed, res_failed, errortype;
  
  testCaseTitle("ENG8_14981_Teststep2","ULink Timeout");
  testCaseDescription("After BMU start the ULink timeout error bit is initially checked to be unequal to failed. The ULink state is set to invalid and the  ImocState_VoltageMeasurementMode is set to OnlyLinkVoltageMeasurement or TotalVoltageMeasurement (depends on table, see bottom Testdescription).  After a defined timeout duration the error bit is checked to switch to failed");

  //Preconditions
  func_Preconditions();         
  facHvMeasTimeout = L_Mem_HVMeas_Timeout_Param_WITH_TOLERANCE / 10;
      
  // 1b)<iL_HVMeas_ULink_State.bInvalid> = /false and <L_ImocCtrl_ImocState_VoltageMeasurementMode> = NoVoltageMeasurement

  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", 200);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 200);
  funcIDE_DeleteAllBP();
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages","SHvDaq16_NoMeasurement = VoltageMeasurementMode == 0;");
  funcIDE_Run();
  funcIDE_WaitForHalt (2000); // time can be modified
  funcIDE_WriteVariable("VoltageMeasurementMode", NoVoltageMeasurement);
  funcIDE_DeleteAllBP();
  
   // 1c) Wait L_Mem_HVMeas_Timeout_Param ms and check ERR_ULink_Timeout !=RUNNING_FAILED
  for (i = 0; i < facHvMeasTimeout; i++)
  { funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages","SHvDaq16_NoMeasurement = VoltageMeasurementMode == 0;");
    match = getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_ULink_Timeout);
    if (match != RUNNING_FAILED) res_passed++; else res_failed++; 
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);   // time can be modified
    funcIDE_WriteVariable("VoltageMeasurementMode", NoVoltageMeasurement);
    funcIDE_DeleteAllBP();
    funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "CtDaqMgr_SetEventExWithStop(SHvDaq16_NoMeasurement || (VoltageMeasurementMode == 2), bLastInv_B,");
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);   // time can be modified
    check_equality(funcIDE_ReadVariable("bLastInv_B"), PASSED_INTERNAL, "The timeout error is set to PASSED_INTERNAL", "The timeout error is not set to PASSED_INTERNAL");
    check_equality(funcIDE_ReadVariable("VoltageMeasurementMode"), NoVoltageMeasurement, "The <L_ImocCtrl_ImocState_VoltageMeasurementMode> is set to NoVoltageMeasurement", "The <L_ImocCtrl_ImocState_VoltageMeasurementMode> is not set to NoVoltageMeasurement");
    funcIDE_DeleteAllBP();
    if (i == 10 - 1) teststep("","loop complete,ERR_ULink_Timeout != RUNNING_FAILED -> %d = passed; %d = failed.", res_passed, res_failed);
  }
  check_equality(res_failed, 0, "The Signal on CAN ErrQ_HVMeas_ULinkTimeout was always as expected in != RUNNING_FAILED", "The Signal on CAN ErrQ_HVMeas_ULinkTimeout switched in unexpected value.");
  
  // 2) Check <ErrQ_HVMeas_ULinkTimeout>
  testStep("ENG8_14981_Teststep2.2","Check <ErrQ_HVMeas_ULinkTimeout>");
  testCaseComment("");
  func_ValueCompare("Signal on CAN ErrQ_HVMeas_ULinkTimeout", getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_ULink_Timeout),RUNNING_FAILED, not_equal);
  
  // 3) Set <L_HVMeas_ULink_Pos> =0  and Set <L_HVMeas_ULink_Neg>=1 
  testStep("ENG8_14981_Teststep2.3","Set <L_HVMeas_ULink_Pos> =0  and Set <L_HVMeas_ULink_Neg>=1 "); 
  testCaseComment("");
  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", 32767);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 200);
  funcIDE_WriteVariable("ContCtrl_Xcp_ContMainPos_State_in_ACT_U8", 170);
  funcIDE_WriteVariable("ContCtrl_Xcp_ContMainPos_State_in_VAL_U8", CNT_CLOSED);
  funcIDE_WriteVariable("ContCtrl_Xcp_ContMainNeg_State_in_ACT_U8", 170);
  funcIDE_WriteVariable("ContCtrl_Xcp_ContMainNeg_State_in_VAL_U8", CNT_CLOSED);
  funcIDE_DeleteAllBP();
   
  // 4) Wait L_Mem_HVMeas_Timeout_Param ms and check ERR_ULink_Timeout !=RUNNING_FAILED
  testStep("ENG8_14981_Teststep2.4","Wait L_Mem_HVMeas_Timeout_Param ms");
  for (i = 0; i < facHvMeasTimeout; i++)
  { funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages","SHvDaq16_NoMeasurement = VoltageMeasurementMode == 0;");
    match = getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_ULink_Timeout);
    if (match != RUNNING_FAILED) res_passed++; else res_failed++; 
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);   // time can be modified
    funcIDE_WriteVariable("VoltageMeasurementMode", NoVoltageMeasurement);
    funcIDE_DeleteAllBP();
    funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "CtDaqMgr_SetEventExWithStop(SHvDaq16_NoMeasurement || (VoltageMeasurementMode == 2), bLastInv_B,");
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);   // time can be modified
    check_equality(funcIDE_ReadVariable("bLastInv_B"), PASSED_INTERNAL, "The timeout error is set to PASSED_INTERNAL", "The timeout error is not set to PASSED_INTERNAL");
    check_equality(funcIDE_ReadVariable("VoltageMeasurementMode"), NoVoltageMeasurement, "The <L_ImocCtrl_ImocState_VoltageMeasurementMode> is set to NoVoltageMeasurement", "The <L_ImocCtrl_ImocState_VoltageMeasurementMode> is not set to NoVoltageMeasurement");
    funcIDE_DeleteAllBP();
    if (i == 10 - 1) teststep("","loop complete,ERR_ULink_Timeout != RUNNING_FAILED -> %d = passed; %d = failed.", res_passed, res_failed);
  }
  check_equality(res_failed, 0, "The Signal on CAN ErrQ_HVMeas_ULinkTimeout was always as expected in != RUNNING_FAILED", "The Signal on CAN ErrQ_HVMeas_ULinkTimeout switched in unexpected value.");
  
  // 5) Check <ErrQ_HVMeas_ULinkTimeout>
  testStep("ENG8_14981_Teststep2.5","Check <ErrQ_HVMeas_ULinkTimeout>");
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages","SHvDaq16_NoMeasurement = VoltageMeasurementMode == 0;");
  res_passed = 0;
  res_failed = 0;
  // Wait for CAN
  for (i = 0; i < 250; i++)
  { match = getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_ULink_Timeout);
    if (match != RUNNING_FAILED) res_passed++; else res_failed++; 
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);   // time can be modified
    funcIDE_WriteVariable("VoltageMeasurementMode", NoVoltageMeasurement);
    if (i == 250 - 1) teststep("","loop complete,ERR_ULink_Timeout != RUNNING_FAILED -> %d = passed; %d = failed.", res_passed, res_failed);
  }
  check_equality(res_failed, 0, "The Signal on CAN ErrQ_HVMeas_ULinkTimeout was always as expected in != RUNNING_FAILED", "The Signal on CAN ErrQ_HVMeas_ULinkTimeout switched in unexpected value.");
  
  // 6) Set <L_ImocCtrl_ImocState_VoltageMeasurementMode> = TotalVoltageMeasurement 
  testStep("ENG8_14981_Teststep2.6","Set <L_ImocCtrl_ImocState_VoltageMeasurementMode> = TotalVoltageMeasurement ");
  funcIDE_Run();
  funcIDE_WaitForHalt (2000); // time can be modified
  funcIDE_WriteVariable("VoltageMeasurementMode", TotalVoltageMeasurement );
  funcIDE_DeleteAllBP();
  
  // 7) Wait L_Mem_HVMeas_Timeout_Param ms and check ERR_ULink_Timeout ==RUNNING_FAILED
  testStep("ENG8_14981_Teststep2.7","Wait L_Mem_HVMeas_Timeout_Param ms");
    
  for (i = 0; i < facHvMeasTimeout; i++)
  { funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages","SHvDaq16_NoMeasurement = VoltageMeasurementMode == 0;");
    match = getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_ULink_Timeout);
    if (match != RUNNING_FAILED) res_passed++; else res_failed++; 
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);   // time can be modified
    funcIDE_WriteVariable("VoltageMeasurementMode", TotalVoltageMeasurement );
    funcIDE_DeleteAllBP();
    funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "CtDaqMgr_SetEventExWithStop(SHvDaq16_NoMeasurement || (VoltageMeasurementMode == 2), bLastInv_B,");
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);   // time can be modified
    check_equality(funcIDE_ReadVariable("bLastInv_B"), FAILED_INTERNAL, "The timeout error was set to FAILED_INTERNAL", "The timeout error is not set to FAILED_INTERNAL");
    check_equality(funcIDE_ReadVariable("VoltageMeasurementMode"), TotalVoltageMeasurement , "The <L_ImocCtrl_ImocState_VoltageMeasurementMode> is set to TotalVoltageMeasurement ", "The <L_ImocCtrl_ImocState_VoltageMeasurementMode> is not set to TotalVoltageMeasurement ");
    funcIDE_DeleteAllBP();
    if (i == 10 - 1) teststep("","loop complete,ERR_ULink_Timeout != RUNNING_FAILED -> %d = passed; %d = failed.", res_passed, res_failed);
  }
  check_equality(res_failed, 0, "The Signal on CAN ErrQ_HVMeas_ULinkTimeout was always as expected in != RUNNING_FAILED", "The Signal on CAN ErrQ_HVMeas_ULinkTimeout switched in unexpected value.");
  
  // 8) Check <ErrQ_HVMeas_ULinkTimeout>
  testStep("ENG8_14981_Teststep2.8","Check <ErrQ_HVMeas_ULinkTimeout>");
  // Wait for CAN
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages","SHvDaq16_NoMeasurement = VoltageMeasurementMode == 0;");
  res_passed = 0;
  res_failed = 0;
    
  for (i = 0; i < 300; i++)
  { 
    match = getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_ULink_Timeout);
    if (match == RUNNING_FAILED)
      res_passed++; 
    else {
      res_failed++;
      errortype = match;}    
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);   // time can be modified
    funcIDE_WriteVariable("VoltageMeasurementMode", TotalVoltageMeasurement );
    if (i == 300 - 1) teststep("","loop complete,ERR_UPack_Timeout == RUNNING_FAILED -> %d = passed; %d = failed.", res_passed, res_failed);
  }
  check_equality(errortype, 4, "The Signal on CAN ErrQ_HVMeas_ULinkTimeout switched also in Status /DEBOUNCING_PREVIOUS_PASS/", "The Signal on CAN ErrQ_HVMeas_ULinkTimeout switched in unexpected value.");
  func_ValueCompare("The Signal on CAN ErrQ_HVMeas_ULinkTimeout switched also in Status /DEBOUNCING_PREVIOUS_PASS/ as expected.", res_failed, 0, greater_than);
     
  testCaseComment("");
  func_ValueCompare("Signal on CAN ErrQ_HVMeas_ULinkTimeout", getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_ULink_Timeout),RUNNING_FAILED, equal);
  
    
  //cleanup
  func_CleanUp();
}




//
//testcase ENG8_4305 ()
//{
//  int ULinkOutOfRangeHi, startTime, endTime, match, facHvMeasTimeout;
//  testCaseTitle("ENG8_4305","ULink Timeout reset");
//  testCaseDescription("After BMU start the ULink state is set to invalid. After a defined timeout duration the error bit is checked to switch to failed. The ULink state is validated again and the ULink timeout error bit observed to switch to passed.");
//  
//    //Preconditions
//  func_Preconditions();
//  ULinkOutOfRangeHi = funcIDE_ReadVariable("HvDaqMgrParameter.p_ULinkOutOfRangeHi");
//  ULinkOutOfRangeHi = ULinkOutOfRangeHi / 40;
//  facHvMeasTimeout = funcIDE_ReadVariable("HvDaqMgrParameter.p_facHvMeasTimeout");
//  facHvMeasTimeout = facHvMeasTimeout * 10;
//  
//  // 2) Check <ErrQ_HVMeas_ULinkTimeout>
//  testStep("ENG8_4305.2","Check <ErrQ_HVMeas_ULinkTimeout>");
//  testCaseComment("");
//  func_ValueCompare("Check <ErrQ_HVMeas_ULinkTimeout>", getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_ULink_Timeout), RUNNING_FAILED, not_equal);
//
//  // 3) Set <P_HVMeas_ULink> = <L_Mem_HVMeas_ThresholdLinkUOutOfRangeHigh_Param> + 1unit
//  testStep("ENG8_4305.3a","Set HV contactor state to /Connected/.");
//  trickMemoryBefore();
//  funcIDE_WriteVariable("ContDiag_Xcp_ContMainPos_State_out_ACT_U8", 170);
//  funcIDE_WriteVariable("ContDiag_Xcp_ContMainPos_State_out_VAL_U8", ENABLE_Cont);
//  funcIDE_WriteVariable("ContDiag_Xcp_ContMainNeg_State_out_ACT_U8", 170);
//  funcIDE_WriteVariable("ContDiag_Xcp_ContMainNeg_State_out_VAL_U8", ENABLE_Cont);
//  trickMemoryAfter();
//  teststep("","");
//  testStep("ENG8_4305.3b","Set ULink <L_Mem_HVMeas_ThresholdLinkUOutOfRangeHigh_Param> \n\n<P_HVMeas_ULink> = <L_Mem_HVMeas_ThresholdLinkUOutOfRangeHigh_Param>> ");
//  trickMemorybefore();
//  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1);
//  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", (ULinkOutOfRangeHi + 1) * 40);
//  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
//  funcIDE_WriteVariable("CtDaqMgr_Xcp_IsLinkSna_ACT_U8", 170);
//  funcIDE_WriteVariable("CtDaqMgr_Xcp_IsLinkSna_VAL_B", TRUE);
//  funcIDE_DeleteAllBP();
//
//  // 4) Wait L_Mem_HVMeas_Timeout_Param ms
//  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "   CtDaqMgr_SetEventExWithStop(!(IsImocRunning), bLastInv_B, Event_Err_HVMeas_ULinkTimeout, CtDaqMgr_AddInfo_U8);");
//  funcIDE_RunAndWait(2000);
//  startTime = funcIDE_ReadVariable("osSystemCounter");
//  match = testWaitForVariableValue("bLastInv_B", FAILED_INTERNAL, 20000);
//  endTime = funcIDE_ReadVariable("osSystemCounter");
//  func_timeCompare ("duration", "ms", endTime-startTime, facHvMeasTimeout, equal_or_less_than);
//
//  // 5) Check <ErrQ_HVMeas_ULinkTimeout>
//  check_equality(funcIDE_ReadVariable("ULink.State.bInvalid"), TRUE, "<iL_HVMeas_ULink_State.bInvalid> == /true/", "<iL_HVMeas_ULink_State.bInvalid> != /true/");
//  check_equality(match, FAILED_INTERNAL, "The internal timeout error is set to FAILED_INTERNAL", "The internal timeout error is not set to FAILED_INTERNAL");
//  trickMemoryAfter();
//  funcIDE_Run();
//  match = testWaitForSignalMatch(CAN2::SC_ERRORS_DAQ_HALL::ERR_ULink_Timeout, RUNNING_FAILED, 2000);
//  check_equality(match, 1, "The timeout error is set to RUNNING_FAILED", "The timeout error is not set to RUNNING_FAILED");
//  
//  // 6) Ignition switch (Reset)
//  reset_BMS(2000);
//  SetBlemHooksLoop(TRUE);
//  
//  // 7) Set <L_HVMeas_ULink_State> = /Valid/
//  trickMemoryBefore();
//  funcIDE_WriteVariable("ContDiag_Xcp_ContMainPos_State_out_ACT_U8", 170);
//  funcIDE_WriteVariable("ContDiag_Xcp_ContMainPos_State_out_VAL_U8", ENABLE_Cont);
//  funcIDE_WriteVariable("ContDiag_Xcp_ContMainNeg_State_out_ACT_U8", 170);
//  funcIDE_WriteVariable("ContDiag_Xcp_ContMainNeg_State_out_VAL_U8", ENABLE_Cont);
//  teststep("","");
//  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1);
//  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", ULinkOutOfRangeHi * 40);
//  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
//  funcIDE_WriteVariable("CtDaqMgr_Xcp_IsLinkSna_ACT_U8", 170);
//  funcIDE_WriteVariable("CtDaqMgr_Xcp_IsLinkSna_VAL_B", FALSE);
//  funcIDE_DeleteAllBP();
//  
//  // 8) Check <ErrQ_HVMeas_ULinkTimeout>
//  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "   CtDaqMgr_SetEventExWithStop(!(IsImocRunning), bLastInv_B, Event_Err_HVMeas_ULinkTimeout, CtDaqMgr_AddInfo_U8);");
//  funcIDE_RunAndWait(2000);
//  check_equality(getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_ULink_Timeout), PREFAILED, "The timeout error is set to PREFAILED", "The timeout error is not set to PREFAILED");
//  
//  // 9) Wait L_Mem_HVMeas_Timeout_Param ms
//  startTime = funcIDE_ReadVariable("@STM_CNT");
//  match = testWaitForVariableValue("bLastInv_B", PASSED_INTERNAL, 20000);
//  endTime = funcIDE_ReadVariable("@STM_CNT");
//  func_timeCompare ("duration", "ms", (endTime-startTime)/64000, facHvMeasTimeout, equal_or_less_than);
//
//  // 10) Check <ErrQ_HVMeas_ULinkTimeout>
//  check_equality(funcIDE_ReadVariable("ULink.State.bInvalid"), FALSE, "<iL_HVMeas_ULink_State.bInvalid> != /true/", "<iL_HVMeas_ULink_State.bInvalid> == /true/");
//  check_equality(match, PASSED_INTERNAL, "The internal timeout error is set to RUNNING_PASSED", "The internal timeout error is not set to RUNNING_PASSED");
//  trickMemoryAfter();
//  match = testWaitForSignalMatch(CAN2::SC_ERRORS_DAQ_HALL::ERR_ULink_Timeout, RUNNING_PASSED, 4000);
//  check_equality(match, 1, "The timeout error is set to RUNNING_PASSED", "The timeout error is not set to RUNNING_PASSED");
//
//  //cleanup
//  func_cleanUp();
//}


testcase ENG8_14982_Teststep1 ()
{
  int match, startTime, endTime, facHvMeasTimeout, i;
  int res_passed, res_failed, errortype;
    
  testCaseTitle("ENG8_14982_Teststep1","UPack Timeout reset");
  testCaseDescription("After BMU start the UPack state is set to invalid. After a defined timeout duration the error bit is checked to switch to failed. After the Check the  ImocState_VoltageMeasurementMode is set to OnlyPackVoltageMeasurement or TotalVoltageMeasurement (depends on table, see bottom Testdescription). The UPack state is validated again and the UPack timeout error bit observed to switch to passed.");
  
  //Preconditions
  func_Preconditions();
  facHvMeasTimeout = funcIDE_ReadVariable("HvDaqMgrParameter.p_facHvMeasTimeout");
    
  // 2) Set <iL_HVMeas_UPack_State.bInvalid>== /true/ <L_ImocCtrl_ImocState_VoltageMeasurementMode> = OnlyLinkVoltageMeasurement
  testStep("ENG8_14982_Teststep1.2","Set <L_HVMeas_ULink_State.bInvalid> == /true/"); // true == invalid
  testCaseComment("");
  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", 32767);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 200);
  funcIDE_WriteVariable("ContCtrl_Xcp_ContMainPos_State_in_ACT_U8", 170);
  funcIDE_WriteVariable("ContCtrl_Xcp_ContMainPos_State_in_VAL_U8", CNT_CLOSED);
  funcIDE_WriteVariable("ContCtrl_Xcp_ContMainNeg_State_in_ACT_U8", 170);
  funcIDE_WriteVariable("ContCtrl_Xcp_ContMainNeg_State_in_VAL_U8", CNT_CLOSED);  
  funcIDE_DeleteAllBP();
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "SHvDaq16_NoMeasurement = VoltageMeasurementMode == 0;");
  funcIDE_Run();
  funcIDE_WaitForHalt (2000); // time can be modified
  funcIDE_WriteVariable("VoltageMeasurementMode", OnlyLinkVoltageMeasurement );
  funcIDE_DeleteAllBP();
  
  // 3) Wait L_Mem_HVMeas_Timeout_Param ms and check ERR_ULink_Timeout ==RUNNING_FAILED
  testStep("ENG8_14982_Teststep1.3","Wait L_Mem_HVMeas_Timeout_Param ms");
  // Wait for CAN
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages","SHvDaq16_NoMeasurement = VoltageMeasurementMode == 0;");
  res_passed=0;
  res_failed=0;
    
  for (i=0;i<300;i++)
  { 
    match=getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_ULink_Timeout);
    if (match==RUNNING_FAILED)
      res_passed++; 
    else {
      res_failed++;
      errortype=match;}    
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);   // time can be modified
    funcIDE_WriteVariable("VoltageMeasurementMode", OnlyLinkVoltageMeasurement);
    if (i==300-1) teststep("","loop complete,ERR_ULink_Timeout == RUNNING_FAILED -> %d = passed; %d = failed.",res_passed,res_failed);
  }
  check_equality(errortype, 4, "The Signal on CAN ErrQ_HVMeas_ULinkTimeout switched also in Status /DEBOUNCING_PREVIOUS_PASS/", "The Signal on CAN ErrQ_HVMeas_ULinkTimeout switched in unexpected value.");
  func_ValueCompare("The Signal on CAN ErrQ_HVMeas_ULinkTimeout switched also in Status /DEBOUNCING_PREVIOUS_PASS/ as expected.", res_failed,0, greater_than);

   // 4) Check <ErrQ_HVMeas_ULinkTimeout>
  testStep("ENG8_14982_Teststep1.4","Check <ErrQ_HVMeas_ULinkTimeout>");
  testCaseComment("");
  func_ValueCompare("Signal on CAN ErrQ_HVMeas_ULinkTimeout", getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_ULink_Timeout),RUNNING_FAILED, equal);
  
  
  // 5) Set <iL_HVMeas_ULink_State.bInvalid>== /false/ <L_ImocCtrl_ImocState_VoltageMeasurementMode> = OnlyLinkVoltageMeasurement
  testStep("ENG8_14982_Teststep1.5","Set <iL_HVMeas_ULink_State.bInvalid>== /false/ <L_ImocCtrl_ImocState_VoltageMeasurementMode> = OnlyLinkVoltageMeasurement"); // true == invalid
  testCaseComment("");
  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", 600*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 300*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_IsLinkSna_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_IsLinkSna_VAL_B", FALSE);
  funcIDE_DeleteAllBP();
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "SHvDaq16_NoMeasurement = VoltageMeasurementMode == 0;");
  funcIDE_Run();
  funcIDE_WaitForHalt (2000); // time can be modified
  funcIDE_WriteVariable("VoltageMeasurementMode", OnlyLinkVoltageMeasurement);
   // Check <ErrQ_HVMeas_ULinkTimeout>
  res_passed=0;
  res_failed=0;
  
  for (i=0;i<200;i++)
  { match=getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_ULink_Timeout);
    if (match!=RUNNING_PASSED) res_passed++; else res_failed++; 
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);   // time can be modified
    funcIDE_WriteVariable("VoltageMeasurementMode", OnlyLinkVoltageMeasurement);
    if (i==200-1) teststep("","loop complete,ERR_ULink_Timeout != RUNNING_PASSED -> %d = passed; %d = failed.",res_passed,res_failed);
  }
  check_equality(res_failed, 0, "The Signal on CAN ErrQ_HVMeas_ULinkTimeout was always as expected in != RUNNING_PASSED", "The Signal on CAN ErrQ_HVMeas_ULinkTimeout switched in unexpected value.");

  //''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''' 
  // 6) Ignition switch (Reset)
  //'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
  
  reset_BMS(2000);
  //SetBlemHooksLoop(TRUE);
  SetBlemHooksLoop_afterReset_noRun(TRUE);
  func_Preconditions();
  
  // >ReSet Conditions <iL_HVMeas_ULink_State.bInvalid>== /true/ <L_ImocCtrl_ImocState_VoltageMeasurementMode> = OnlyLinkVoltageMeasurement
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "SHvDaq16_NoMeasurement = VoltageMeasurementMode == 0;");
  funcIDE_RunAndWait(4000);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", 600*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 300*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_IsLinkSna_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_IsLinkSna_VAL_B", FALSE);
  funcIDE_WriteVariable("VoltageMeasurementMode", OnlyLinkVoltageMeasurement);
  funcIDE_DeleteAllBP();
    
  // 7) check<ErrQ_HVMeas_ULinkTimeout>
  testStep("ENG8_14982_Teststep1.7","Check <ErrQ_HVMeas_ULinkTimeout>");
  func_ValueCompare("Signal on CAN ErrQ_HVMeas_ULinkTimeout", getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_ULink_Timeout),RUNNING_FAILED, not_equal);
  
  // 8) Wait L_Mem_HVMeas_Timeout_Param ms
  testStep("ENG8_14982_Teststep1.8","Wait L_Mem_HVMeas_Timeout_Param ms");
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "CtDaqMgr_SetEventExWithStop(SHvDaq16_NoMeasurement || (VoltageMeasurementMode == 1), bLastInv_B_a,");
  match = testWaitForVariableValue("bLastInv_B_a", PASSED_INTERNAL, 20000);
  check_equality(match, PASSED_INTERNAL, "The timeout error is set to PASSED_INTERNAL", "The timeout error is not set to PASSED_INTERNAL");
  check_equality(funcIDE_ReadVariable("VoltageMeasurementMode"), OnlyLinkVoltageMeasurement, "The <L_ImocCtrl_ImocState_VoltageMeasurementMode> is set to OnlyLinkVoltageMeasurement", "The <L_ImocCtrl_ImocState_VoltageMeasurementMode> is not set to OnlyLinkVoltageMeasurement.");
  funcIDE_DeleteAllBP();
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages","SHvDaq16_NoMeasurement = VoltageMeasurementMode == 0;");
  
  res_passed=0;
  res_failed=0;
    
  for (i=0;i<300;i++)
  { 
    match=getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_ULink_Timeout);
    if (match==RUNNING_PASSED)
      res_passed++; 
    else {
      res_failed++;
    }    
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);   // time can be modified
    funcIDE_WriteVariable("VoltageMeasurementMode", OnlyLinkVoltageMeasurement);
    if (i==300-1) teststep("","loop complete,ERR_ULink_Timeout == RUNNING_PASSED -> %d = passed; %d = failed.",res_passed,res_failed);
  }
  
   // 9) Check <ErrQ_HVMeas_ULinkTimeout>
  check_equality(getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_ULink_Timeout), RUNNING_PASSED, "The timeout error is set to RUNNING_PASSED", "The timeout error is not set to RUNNING_PASSED");
  trickMemoryAfter();
    
  //cleanup
  func_CleanUp();
}


testcase ENG8_14982_Teststep2 ()
{
  int match, startTime, endTime, facHvMeasTimeout, i;
  int res_passed, res_failed, errortype;
    
  testCaseTitle("ENG8_14982_Teststep2","ULink Timeout reset");
  testCaseDescription("After BMU start the ULink state is set to invalid. After a defined timeout duration the error bit is checked to switch to failed. After the Check the  ImocState_VoltageMeasurementMode is set to OnlyPackVoltageMeasurement or TotalVoltageMeasurement (depends on table, see bottom Testdescription). The ULink state is validated again and the ULink timeout error bit observed to switch to passed.");
  
  //Preconditions
  func_Preconditions();
  facHvMeasTimeout = funcIDE_ReadVariable("HvDaqMgrParameter.p_facHvMeasTimeout");
    
  // 2) Set <iL_HVMeas_UPack_State.bInvalid>== /true/ <L_ImocCtrl_ImocState_VoltageMeasurementMode> = TotalVoltageMeasurement
  testStep("ENG8_14982_Teststep2.2","Set <L_HVMeas_ULink_State.bInvalid> == /true/"); // true == invalid
  testCaseComment("");
  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", 32767);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 200);
  funcIDE_WriteVariable("ContCtrl_Xcp_ContMainPos_State_in_ACT_U8", 170);
  funcIDE_WriteVariable("ContCtrl_Xcp_ContMainPos_State_in_VAL_U8", CNT_CLOSED);
  funcIDE_WriteVariable("ContCtrl_Xcp_ContMainNeg_State_in_ACT_U8", 170);
  funcIDE_WriteVariable("ContCtrl_Xcp_ContMainNeg_State_in_VAL_U8", CNT_CLOSED);  
  funcIDE_DeleteAllBP();
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "SHvDaq16_NoMeasurement = VoltageMeasurementMode == 0;");
  funcIDE_Run();
  funcIDE_WaitForHalt (2000); // time can be modified
  funcIDE_WriteVariable("VoltageMeasurementMode", TotalVoltageMeasurement );
  funcIDE_DeleteAllBP();
  
  // 3) Wait L_Mem_HVMeas_Timeout_Param ms and check ERR_ULink_Timeout ==RUNNING_FAILED
  testStep("ENG8_14982_Teststep2.3","Wait L_Mem_HVMeas_Timeout_Param ms");
  // Wait for CAN
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages","SHvDaq16_NoMeasurement = VoltageMeasurementMode == 0;");
  res_passed=0;
  res_failed=0;
    
  for (i=0;i<300;i++)
  { 
    match=getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_ULink_Timeout);
    if (match==RUNNING_FAILED)
      res_passed++; 
    else {
      res_failed++;
      errortype=match;}    
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);   // time can be modified
    funcIDE_WriteVariable("VoltageMeasurementMode", TotalVoltageMeasurement);
    if (i==300-1) teststep("","loop complete,ERR_ULink_Timeout == RUNNING_FAILED -> %d = passed; %d = failed.",res_passed,res_failed);
  }
  check_equality(errortype, 4, "The Signal on CAN ErrQ_HVMeas_ULinkTimeout switched also in Status /DEBOUNCING_PREVIOUS_PASS/", "The Signal on CAN ErrQ_HVMeas_ULinkTimeout switched in unexpected value.");
  func_ValueCompare("The Signal on CAN ErrQ_HVMeas_ULinkTimeout switched also in Status /DEBOUNCING_PREVIOUS_PASS/ as expected.", res_failed,0, greater_than);

   // 4) Check <ErrQ_HVMeas_ULinkTimeout>
  testStep("ENG8_14982_Teststep2.4","Check <ErrQ_HVMeas_ULinkTimeout>");
  testCaseComment("");
  func_ValueCompare("Signal on CAN ErrQ_HVMeas_ULinkTimeout", getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_ULink_Timeout),RUNNING_FAILED, equal);
  
  
  // 5) Set <iL_HVMeas_ULink_State.bInvalid>== /false/ <L_ImocCtrl_ImocState_VoltageMeasurementMode> = TotalVoltageMeasurement
  testStep("ENG8_14982_Teststep2.5","Set <iL_HVMeas_ULink_State.bInvalid>== /false/ <L_ImocCtrl_ImocState_VoltageMeasurementMode> = TotalVoltageMeasurement"); // true == invalid
  testCaseComment("");
  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", 600*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 300*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_IsLinkSna_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_IsLinkSna_VAL_B", FALSE);
  funcIDE_DeleteAllBP();
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "SHvDaq16_NoMeasurement = VoltageMeasurementMode == 0;");
  funcIDE_Run();
  funcIDE_WaitForHalt (2000); // time can be modified
  funcIDE_WriteVariable("VoltageMeasurementMode", TotalVoltageMeasurement);
   // Check <ErrQ_HVMeas_ULinkTimeout>
  res_passed=0;
  res_failed=0;
  
  for (i=0;i<200;i++)
  { match=getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_ULink_Timeout);
    if (match!=RUNNING_PASSED) res_passed++; else res_failed++; 
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);   // time can be modified
    funcIDE_WriteVariable("VoltageMeasurementMode", TotalVoltageMeasurement);
    if (i==200-1) teststep("","loop complete,ERR_ULink_Timeout != RUNNING_PASSED -> %d = passed; %d = failed.",res_passed,res_failed);
  }
  check_equality(res_failed, 0, "The Signal on CAN ErrQ_HVMeas_ULinkTimeout was always as expected in != RUNNING_PASSED", "The Signal on CAN ErrQ_HVMeas_ULinkTimeout switched in unexpected value.");

  //''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''' 
  // 6) Ignition switch (Reset)
  //'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
  
  reset_BMS(2000);
  //SetBlemHooksLoop(TRUE);
  SetBlemHooksLoop_afterReset_noRun(TRUE);
  func_Preconditions();
  
  // >ReSet Conditions <iL_HVMeas_ULink_State.bInvalid>== /true/ <L_ImocCtrl_ImocState_VoltageMeasurementMode> = OnlyLinkVoltageMeasurement
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "SHvDaq16_NoMeasurement = VoltageMeasurementMode == 0;");
  funcIDE_RunAndWait(4000);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", 600*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 300*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_IsLinkSna_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_IsLinkSna_VAL_B", FALSE);
  funcIDE_WriteVariable("VoltageMeasurementMode", OnlyLinkVoltageMeasurement);
  funcIDE_DeleteAllBP();
    
  // 7) check<ErrQ_HVMeas_ULinkTimeout>
  testStep("ENG8_14982_Teststep2.7","Check <ErrQ_HVMeas_ULinkTimeout>");
  func_ValueCompare("Signal on CAN ErrQ_HVMeas_ULinkTimeout", getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_ULink_Timeout),RUNNING_FAILED, not_equal);
  
  // 8) Wait L_Mem_HVMeas_Timeout_Param ms
  testStep("ENG8_14982_Teststep2.8","Wait L_Mem_HVMeas_Timeout_Param ms");
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "CtDaqMgr_SetEventExWithStop(SHvDaq16_NoMeasurement || (VoltageMeasurementMode == 1), bLastInv_B_a,");
  match = testWaitForVariableValue("bLastInv_B_a", PASSED_INTERNAL, 20000);
  check_equality(match, PASSED_INTERNAL, "The timeout error is set to PASSED_INTERNAL", "The timeout error is not set to PASSED_INTERNAL");
  check_equality(funcIDE_ReadVariable("VoltageMeasurementMode"), OnlyLinkVoltageMeasurement, "The <L_ImocCtrl_ImocState_VoltageMeasurementMode> is set to OnlyLinkVoltageMeasurement", "The <L_ImocCtrl_ImocState_VoltageMeasurementMode> is not set to OnlyLinkVoltageMeasurement.");
  funcIDE_DeleteAllBP();
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages","SHvDaq16_NoMeasurement = VoltageMeasurementMode == 0;");
  
  res_passed=0;
  res_failed=0;
    
  for (i=0;i<300;i++)
  { 
    match=getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_ULink_Timeout);
    if (match==RUNNING_PASSED)
      res_passed++; 
    else {
      res_failed++;
    }    
    funcIDE_Run();
    funcIDE_WaitForHalt (2000);   // time can be modified
    funcIDE_WriteVariable("VoltageMeasurementMode", OnlyLinkVoltageMeasurement);
    if (i==300-1) teststep("","loop complete,ERR_ULink_Timeout == RUNNING_PASSED -> %d = passed; %d = failed.",res_passed,res_failed);
  }
  
   // 9) Check <ErrQ_HVMeas_ULinkTimeout>
  check_equality(getSignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_ULink_Timeout), RUNNING_PASSED, "The timeout error is set to RUNNING_PASSED", "The timeout error is not set to RUNNING_PASSED");
  trickMemoryAfter();
    
  //cleanup
  func_CleanUp();
}


testcase ENG8_13411()
{
  long ULinkPlausibilityParam;
  
  testCaseTitle("ENG8_13411","Pack-Link Plausibility Error with Open Contactors is OK");
  testCaseDescription("After BMU start the HvEm01DcVolt signal is checked to be valid, and the ULink signal is set to a value such that its absolute difference with HvEm01DcVolt is less than the plausibilization param for ULink. At the end of the Test Case the PackLink error with open contactors is observed to be OK.");
  
  // preconditions
  func_Preconditions();
  
  // Open All Contactors
  Func_ContCtrl_MainPos(DISABLE_Cont);
  Func_ContCtrl_MainNeg(DISABLE_Cont);
  
  // Set <L_Mem_HVMeas_ULinkPlausibility_Param> >= abs(<iL_HVMeas_ULink> - <L_SafetyMgr_EM01_DC_Volt>.Value)
  ULinkPlausibilityParam = funcIDE_ReadVariable("HvDaqMgrParameter.p_ULinkPlaus");
  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1); // activation
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", ULinkPlausibilityParam - 1);
  trickMemoryAfter();

  // check <iL_HVMeas_ULink_State.bInvalid> == /false/
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "UDCeLkAbs_S16 = (sint16) (CtDaqMgr_ULink_S16 - UCellSum_a.Value);");
  funcIDE_WaitForHalt(2000);  
  func_ValueCompare("<iL_HVMeas_ULink_State.bInvalid>", funcIDE_ReadVariable("ULink.State.bInvalid"), FALSE, equal);
  trickMemoryAfter();

  // check <L_Safety_EM01_DC_Volt>.State = EM01_DC_Volt_OK
  func_ValueCompare("<L_Safety_EM01_DC_Volt>.State", funcIDE_ReadVariable("g_dataSafetyMgrEM01_DC_Volt.State"), EM01_DC_Volt_OK, equal);

  // Check <Err_HVMeas_ULinkPlausOpnContactor>
  func_ValueCompare("<Err_HVMeas_ULinklausOpnContactor>", getsignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_LinkPlausOpnContactor), RUNNING_PASSED, equal);

  //cleanUp
  func_CleanUp();
}

testcase ENG8_14833()
{
  long ULinkPlausibilityParam;
  
  testCaseTitle("ENG8_14833","Pack-Link Plausibility Error with Open Contactors is OK");
  testCaseDescription("After BMU start the HvEm01DcVolt signal is checked to be valid, and the ULink signal is set to a value such that its absolute difference with HvEm01DcVolt is less than the plausibilization param for ULink. At the end of the Test Case the PackLink error with open ocntactors is observed to be OK.");
  
  // preconditions
  func_Preconditions();
  
  // Open All Contactors
  Func_ContCtrl_MainPos(DISABLE_Cont);
  Func_ContCtrl_MainNeg(DISABLE_Cont);
  
  // Set <L_Mem_HVMeas_ULinkPlausibility_Param> >= abs(<iL_HVMeas_ULink> - <L_SafetyMgr_EM01_DC_Volt>.Value)
  ULinkPlausibilityParam = funcIDE_ReadVariable("HvDaqMgrParameter.p_ULinkPlaus");
  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1); // activation
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", ULinkPlausibilityParam - 1);
  trickMemoryAfter();

  // check <iL_HVMeas_ULink_State.bInvalid> == /false/
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "UDCeLkAbs_S16 = (sint16) (CtDaqMgr_ULink_S16 - UCellSum_a.Value);");
  funcIDE_WaitForHalt(2000);  
  func_ValueCompare("<iL_HVMeas_ULink_State.bInvalid>", funcIDE_ReadVariable("ULink.State.bInvalid"), FALSE, equal);
  trickMemoryAfter();

  // check <L_Safety_EM01_DC_Volt>.State = EM01_DC_Volt_OK
  func_ValueCompare("<L_Safety_EM01_DC_Volt>.State", funcIDE_ReadVariable("g_dataSafetyMgrEM01_DC_Volt.State"), EM01_DC_Volt_OK, equal);

  // Check <Err_HVMeas_ULinkPlausOpnContactor>
  func_ValueCompare("<Err_HVMeas_ULinklausOpnContactor>", getsignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_LinkPlausOpnContactor), RUNNING_PASSED, equal);

  //cleanUp
  func_CleanUp();
}


testcase ENG8_13412()
{
  long ULinkPlausibilityParam, ULinkDiffMaxParam;
  
  testCaseTitle("ENG8_13412","Pack-Link Plausibility Error with Open Contactors is not OK");
  testCaseDescription("After BMU start the HvEm01DcVolt signal is checked to be valid, and the ULink signal is set to a value such that its absolute difference with HvEm01DcVolt is bigger than the plausibilization param for ULink. At the end of the Test Case the PackLink error with open contactors is observed to be not OK.");
  
  // preconditions
  func_Preconditions();
  
  // Open All Contactors
  Func_ContCtrl_MainPos(DISABLE_Cont);
  Func_ContCtrl_MainNeg(DISABLE_Cont);
  
  // Set <L_Mem_HVMeas_ULinkPlausibility_Param> >= abs(<iL_HVMeas_ULink> - <L_SafetyMgr_EM01_DC_Volt>.Value)
  ULinkPlausibilityParam = funcIDE_ReadVariable("HvDaqMgrParameter.p_ULinkPlaus");
  ULinkDiffMaxParam = funcIDE_ReadVariable("HvDaqMgrParameter.p_ULinkDiffMax");
  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1); // activation
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", ULinkPlausibilityParam + ULinkDiffMaxParam + 1);
  trickMemoryAfter();

  // check <iL_HVMeas_ULink_State.bInvalid> == /false/
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "UDCeLkAbs_S16 = (sint16) (CtDaqMgr_ULink_S16 - UCellSum_a.Value);");
  funcIDE_WaitForHalt(2000);  
  func_ValueCompare("<iL_HVMeas_ULink_State.bInvalid>", funcIDE_ReadVariable("ULink.State.bInvalid"), FALSE, equal);
  trickMemoryAfter();

  // check <L_Safety_EM01_DC_Volt>.State = EM01_DC_Volt_OK
  func_ValueCompare("<L_Safety_EM01_DC_Volt>.State", funcIDE_ReadVariable("g_dataSafetyMgrEM01_DC_Volt.State"), EM01_DC_Volt_OK, equal);

  testWaitForTimeout(2000);
  // Check <Err_HVMeas_ULinkPlausOpnContactor>
  func_ValueCompare("<Err_HVMeas_ULinklausOpnContactor>", getsignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_LinkPlausOpnContactor), RUNNING_FAILED, equal);

  //cleanUp
  func_CleanUp();
}

testcase ENG8_14834()
{
  long ULinkPlausibilityParam, ULinkDiffMaxParam;
  
  testCaseTitle("ENG8_14834","Pack-Link Plausibility Error with Open Contactors is not OK");
  testCaseDescription("Check if the Pack-Link plausibility with open Contactors is not OK when HvEm01DcVolt  is a valid signal and ULink is set to a value such that its absolute difference with HvEm01DcVolt  is bigger than the plausibilization param for ULink.");
  
  // preconditions
  func_Preconditions();
  
  // Open All Contactors 
  Func_ContCtrl_MainPos(DISABLE_Cont);
  Func_ContCtrl_MainNeg(DISABLE_Cont);
  
  // Set <L_Mem_HVMeas_ULinkPlausibility_Param> >= abs(<iL_HVMeas_ULink> - <L_SafetyMgr_EM01_DC_Volt>.Value)
  ULinkPlausibilityParam = funcIDE_ReadVariable("HvDaqMgrParameter.p_ULinkPlaus");
  ULinkDiffMaxParam = funcIDE_ReadVariable("HvDaqMgrParameter.p_ULinkDiffMax");
  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1); // activation
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", ULinkPlausibilityParam + ULinkDiffMaxParam + 1);
  trickMemoryAfter();

  // check <iL_HVMeas_ULink_State.bInvalid> == /false/
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "UDCeLkAbs_S16 = (sint16) (CtDaqMgr_ULink_S16 - UCellSum_a.Value);");
  funcIDE_WaitForHalt(2000);  
  func_ValueCompare("<iL_HVMeas_ULink_State.bInvalid>", funcIDE_ReadVariable("ULink.State.bInvalid"), FALSE, equal);
  trickMemoryAfter();

  // check <L_Safety_EM01_DC_Volt>.State = EM01_DC_Volt_OK
  func_ValueCompare("<L_Safety_EM01_DC_Volt>.State", funcIDE_ReadVariable("g_dataSafetyMgrEM01_DC_Volt.State"), EM01_DC_Volt_OK, equal);

  testWaitForTimeout(2000);
  // Check <Err_HVMeas_ULinkPlausOpnContactor>
  func_ValueCompare("<Err_HVMeas_ULinklausOpnContactor>", getsignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_LinkPlausOpnContactor), RUNNING_FAILED, equal);

  //cleanUp
  func_CleanUp();
}

testcase ENG8_13413()
{
  int i;
  long ULinkPlausibilityParam, ULinkDiffMaxParam;
  
  testCaseTitle("ENG8_13413","Pack-Link Plausibility Error with Open Contactors is stopped");
  testCaseDescription("After BMU start the HvEm01DcVolt signal is checked to be invalid, and its value is set to Cx3FFF_SNA. At the end of the Test Case the Pack Link error with open contactors is observed to be Stopped.");
  
  // preconditions
  func_Preconditions();
  
  // Open All Contactors
  Func_ContCtrl_MainPos(DISABLE_Cont);
  Func_ContCtrl_MainNeg(DISABLE_Cont);
  
  // Set <L_Mem_HVMeas_ULinkPlausibility_Param> >= abs(<iL_HVMeas_ULink> - <L_SafetyMgr_EM01_DC_Volt>.Value)
  ULinkPlausibilityParam = funcIDE_ReadVariable("HvDaqMgrParameter.p_ULinkPlaus");
  ULinkDiffMaxParam = funcIDE_ReadVariable("HvDaqMgrParameter.p_ULinkDiffMax");
  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1); // activation
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", ULinkPlausibilityParam + ULinkDiffMaxParam + 1);
  trickMemoryAfter();

  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "Rte_Write_PpCtApDaqMgrULink_ULink(&ULink);");
  funcIDE_WaitForHalt(2000);  

  for(i=0; i<30; i++)
  {
    // set <iL_HVMeas_ULink_State.bInvalid> == /true/
    funcIDE_WriteVariable("ULink.State.bInvalid", TRUE);

    // set <L_Safety_EM01_DC_Volt>.Value = Cx3FFF_SNA
    funcIDE_WriteVariable("EM01_DC_Volt.Value", 0x3fff);
    
//    // set <L_Safety_EM01_DC_Volt>.State = EM01_DC_Volt_NOT_OK
//    funcIDE_WriteVariable("EM01_DC_Volt.State", EM01_DC_Volt_NOT_OK);
    
    funcIDE_RunAndWait(2000);
  }
  
  // Check <Err_HVMeas_ULinkPlausOpnContactor>
  func_ValueCompare("<Err_HVMeas_ULinklausOpnContactor>", getsignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_LinkPlausOpnContactor), STOPPED, equal);

  //cleanUp
  trickMemoryAfter();
  func_CleanUp();
}

testcase ENG8_14835()
{
  int i; 
  long pre_result;
  long ULinkPlausibilityParam, ULinkDiffMaxParam;
  
  testCaseTitle("ENG8_14835","Pack-Link Plausibility Error with active event HvULinkSigStateIsInvalid is stopped");
  testCaseDescription("Check if the Pack-Link plausibility is stopped, when  <iL_HVMeas_ULink_State.bInvalid> == /true/ ");
  
  // preconditions
  func_Preconditions();
  
  // Open All Contactors
  Func_ContCtrl_MainPos(DISABLE_Cont);
  Func_ContCtrl_MainNeg(DISABLE_Cont);
  
  // Set <L_Mem_HVMeas_ULinkPlausibility_Param> >= abs(<iL_HVMeas_ULink> - <L_SafetyMgr_EM01_DC_Volt>.Value)
  ULinkPlausibilityParam = funcIDE_ReadVariable("HvDaqMgrParameter.p_ULinkPlaus");
  ULinkDiffMaxParam = funcIDE_ReadVariable("HvDaqMgrParameter.p_ULinkDiffMax");
  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1); // activation
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", ULinkPlausibilityParam + ULinkDiffMaxParam + 1);
  trickMemoryAfter();
  
  // Check <Err_HVMeas_ULinkPlausOpnContactor> != STOPPED
  func_ValueCompare("<Err_HVMeas_ULinklausOpnContactor>", getsignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_LinkPlausOpnContactor), STOPPED, not_equal);
   
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "Rte_Write_PpCtApDaqMgrULink_ULink(&ULink);");
  funcIDE_WaitForHalt(2000);  

  for(i=0; i<30; i++)
  {
  // set <iL_HVMeas_ULink_State.bInvalid> == /true/  -  // STOPPED CONDITION 
   funcIDE_WriteVariable("ULink.State.bInvalid", TRUE);
  // set <L_Safety_EM01_DC_Volt>.State = EM01_DC_Volt_OK
   funcIDE_WriteVariable("EM01_DC_Volt.State", EM01_DC_Volt_OK);    
   funcIDE_RunAndWait(2000);
  }
  // Check <Err_HVMeas_ULinkPlausOpnContactor>
  func_ValueCompare("<Err_HVMeas_ULinklausOpnContactor>", getsignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_LinkPlausOpnContactor), STOPPED, equal);

  //cleanUp
  trickMemoryAfter();
  func_CleanUp();
}

testcase ENG8_15024()
{
  int i;
  long ULinkPlausibilityParam, ULinkDiffMaxParam;
  
  testCaseTitle("ENG8_15024","Pack-Link Plausibility Error with active event HvEm01DcVoltIsInvalid (<L_SafetyMgr_EM01_DC_Volt>.State != EM01_DC_Volt_OK)");
  testCaseDescription("Check if the Pack-Link plausibility is stopped, when  the event event HvEm01DcVoltIsInvalid is set");
  
  // preconditions
  func_Preconditions();
  
  // Open All Contactors
  Func_ContCtrl_MainPos(DISABLE_Cont);
  Func_ContCtrl_MainNeg(DISABLE_Cont);
  
  // Set <L_Mem_HVMeas_ULinkPlausibility_Param> >= abs(<iL_HVMeas_ULink> - <L_SafetyMgr_EM01_DC_Volt>.Value)
  ULinkPlausibilityParam = funcIDE_ReadVariable("HvDaqMgrParameter.p_ULinkPlaus");
  ULinkDiffMaxParam = funcIDE_ReadVariable("HvDaqMgrParameter.p_ULinkDiffMax");
  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1); // activation
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", ULinkPlausibilityParam + ULinkDiffMaxParam + 1);
  trickMemoryAfter();
  
  // Check <Err_HVMeas_ULinkPlausOpnContactor> != STOPPED
  func_ValueCompare("<Err_HVMeas_ULinklausOpnContactor>", getsignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_LinkPlausOpnContactor), STOPPED, not_equal);
   

  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "Rte_Write_PpCtApDaqMgrULink_ULink(&ULink);");
  funcIDE_WaitForHalt(2000);  

  for(i=0; i<30; i++)
  {
    // set <iL_HVMeas_ULink_State.bInvalid> == /false/ 
    funcIDE_WriteVariable("ULink.State.bInvalid", FALSE);
    // set <L_Safety_EM01_DC_Volt>.State = EM01_DC_Volt_NOT_OK
    funcIDE_WriteVariable("EM01_DC_Volt.State", EM01_DC_Volt_NOT_OK);
    
  funcIDE_RunAndWait(2000);
  }
  // Check <Err_HVMeas_ULinkPlausOpnContactor>
  func_ValueCompare("<Err_HVMeas_ULinklausOpnContactor>", getsignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_LinkPlausOpnContactor), STOPPED, equal);

  //cleanUp
  trickMemoryAfter();
  func_CleanUp();
}

testcase ENG8_15025()
{
  int i;
  long ULinkPlausibilityParam, ULinkDiffMaxParam;
  
  testCaseTitle("ENG8_15025","Pack-Link Plausibility Error with active event HvEm01DcVoltIsInvalid (<L_SafetyMgr_EM01_DC_Volt>.Value == Cx3FFF_SNA)");
  testCaseDescription("Check if the Pack-Link plausibility is stopped, when  the event event HvEm01DcVoltIsInvalid is set");
  
  // preconditions
  func_Preconditions();
  
  // Open All Contactors
  Func_ContCtrl_MainPos(DISABLE_Cont);
  Func_ContCtrl_MainNeg(DISABLE_Cont);
  
  // Set <L_Mem_HVMeas_ULinkPlausibility_Param> >= abs(<iL_HVMeas_ULink> - <L_SafetyMgr_EM01_DC_Volt>.Value)
  ULinkPlausibilityParam = funcIDE_ReadVariable("HvDaqMgrParameter.p_ULinkPlaus");
  ULinkDiffMaxParam = funcIDE_ReadVariable("HvDaqMgrParameter.p_ULinkDiffMax");
  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1); // activation
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", ULinkPlausibilityParam + ULinkDiffMaxParam + 1);
  trickMemoryAfter();
  
  // Check <Err_HVMeas_ULinkPlausOpnContactor> != STOPPED
  func_ValueCompare("<Err_HVMeas_ULinklausOpnContactor>", getsignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_LinkPlausOpnContactor), STOPPED, not_equal);
     
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "Rte_Write_PpCtApDaqMgrULink_ULink(&ULink);");
  funcIDE_WaitForHalt(2000);  

  for(i=0; i<30; i++)
  {
    // set <iL_HVMeas_ULink_State.bInvalid> == /false/ 
    funcIDE_WriteVariable("ULink.State.bInvalid", FALSE);
    // set <L_Safety_EM01_DC_Volt>.Value = Cx3FFF_SNA
    funcIDE_WriteVariable("EM01_DC_Volt.Value", 0x3fff);
   
    funcIDE_RunAndWait(2000);
  }
  // Check <Err_HVMeas_ULinkPlausOpnContactor>
  func_ValueCompare("<Err_HVMeas_ULinklausOpnContactor>", getsignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_LinkPlausOpnContactor), STOPPED, equal);

  //cleanUp
  trickMemoryAfter();
  func_CleanUp();
}

testcase ENG8_15026()
{
  int i;
  long ULinkPlausibilityParam, ULinkDiffMaxParam;
  
  testCaseTitle("ENG8_15026","Pack-Link Plausibility Error with active event HvAnyContactorClosed(neg. MainContactor = closed)");
  testCaseDescription("Check if the Pack-Link plausibility is stopped, when  HvAnyContactorClosed is set");
  
  // preconditions
  func_Preconditions();
  
  // Open All Contactors
  Func_ContCtrl_MainPos(DISABLE_Cont);
  Func_ContCtrl_MainNeg(DISABLE_Cont);
  
  // Set <L_Mem_HVMeas_ULinkPlausibility_Param> >= abs(<iL_HVMeas_ULink> - <L_SafetyMgr_EM01_DC_Volt>.Value)
  ULinkPlausibilityParam = funcIDE_ReadVariable("HvDaqMgrParameter.p_ULinkPlaus");
  ULinkDiffMaxParam = funcIDE_ReadVariable("HvDaqMgrParameter.p_ULinkDiffMax");
  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1); // activation
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", ULinkPlausibilityParam + ULinkDiffMaxParam + 1);
  trickMemoryAfter();

  // Check <Err_HVMeas_ULinkPlausOpnContactor> != STOPPED
  func_ValueCompare("<Err_HVMeas_ULinklausOpnContactor>", getsignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_LinkPlausOpnContactor), STOPPED, not_equal);
  Func_ContCtrl_MainNeg(ENABLE_Cont);
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "Rte_Write_PpCtApDaqMgrULink_ULink(&ULink);");
  funcIDE_WaitForHalt(4000); 
  for(i=0; i<30; i++)
  {
    // set <iL_HVMeas_ULink_State.bInvalid> == /FALSE/
    funcIDE_WriteVariable("ULink.State.bInvalid", FALSE);
    // set <L_Safety_EM01_DC_Volt>.State = EM01_DC_Volt_NOT_OK
    funcIDE_WriteVariable("EM01_DC_Volt.State", EM01_DC_Volt_OK);
    
    funcIDE_RunAndWait(2000);
  }
  // Check <Err_HVMeas_ULinkPlausOpnContactor>
  func_ValueCompare("<L_ContCtrl_ContMainNeg_State> == /Connected/", funcIDE_ReadVariable("ContDiag_Xcp_ContMainNeg_State_out_DISP_U8"),ENABLE_Cont, equal);
  func_ValueCompare("<Err_HVMeas_ULinklausOpnContactor>", getsignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_LinkPlausOpnContactor), STOPPED, equal);

  //cleanUp
  trickMemoryAfter();
  func_CleanUp();
}

testcase ENG8_15027()
{
  int i;
  long ULinkPlausibilityParam, ULinkDiffMaxParam;
  
  testCaseTitle("ENG8_15027","Pack-Link Plausibility Error with active event HvAnyContactorClosed(neg. MainContactor = closed)");
  testCaseDescription("Check if the Pack-Link plausibility is stopped, when  HvAnyContactorClosed is set");
  
  // preconditions
  func_Preconditions();
  
  // Open All Contactors
  Func_ContCtrl_MainPos(DISABLE_Cont);
  Func_ContCtrl_MainNeg(DISABLE_Cont);
  
  // Set <L_Mem_HVMeas_ULinkPlausibility_Param> >= abs(<iL_HVMeas_ULink> - <L_SafetyMgr_EM01_DC_Volt>.Value)
  ULinkPlausibilityParam = funcIDE_ReadVariable("HvDaqMgrParameter.p_ULinkPlaus");
  ULinkDiffMaxParam = funcIDE_ReadVariable("HvDaqMgrParameter.p_ULinkDiffMax");
  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1); // activation
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", ULinkPlausibilityParam + ULinkDiffMaxParam + 1);
  trickMemoryAfter();
  
  // Check <Err_HVMeas_ULinkPlausOpnContactor> != STOPPED
  func_ValueCompare("<Err_HVMeas_ULinklausOpnContactor>", getsignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_LinkPlausOpnContactor), STOPPED, not_equal);
  Func_ContCtrl_MainPos(ENABLE_Cont);
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "Rte_Write_PpCtApDaqMgrULink_ULink(&ULink);");
  funcIDE_WaitForHalt(4000);  
  for(i=0; i<30; i++)
  {
    // set <iL_HVMeas_ULink_State.bInvalid> == /FALSE/
    funcIDE_WriteVariable("ULink.State.bInvalid", FALSE);
    // set <L_Safety_EM01_DC_Volt>.State = EM01_DC_Volt_NOT_OK
    funcIDE_WriteVariable("EM01_DC_Volt.State", EM01_DC_Volt_OK);
    
    funcIDE_RunAndWait(2000);
  }
  // Check <Err_HVMeas_ULinkPlausOpnContactor>
  func_ValueCompare("<L_ContCtrl_ContMainPos_State> == /Connected/", funcIDE_ReadVariable("ContDiag_Xcp_ContMainPos_State_out_DISP_U8"),ENABLE_Cont, equal);
  func_ValueCompare("<Err_HVMeas_ULinklausOpnContactor>", getsignal(CAN2::SC_ERRORS_DAQ_HALL::ERR_LinkPlausOpnContactor), STOPPED, equal);

  //cleanUp
  trickMemoryAfter();
  func_CleanUp();
}


testcase ENG8_4290 ()
{
  int errorBitBefore, errorBitAfter, UPackOutOfRangeLo;
  
  testCaseTitle("ENG8_4290","UPack out of range - lower limit");
  testCaseDescription("After BMU start the UPack out of range error bit for the lower limit is initially checked to be unequal to failed. The UPack value is set to a value below the out of range threshold and the out of range error bit for the lower limit is observed to switch failed. Hint: The value of <P_HVMeas_UPack> might become negative, since the OutOfRangeLow_param is defined as 0.");

    //Preconditions
  func_Preconditions();
  UPackOutOfRangeLo = funcIDE_ReadVariable("HvDaqMgrParameter.p_UPackOutOfRangeLo");
  UPackOutOfRangeLo = (int)((float)UPackOutOfRangeLo*0.025);
  
  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", UPackOutOfRangeLo / 0.025);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 0);
  trickMemoryAfter();  
  testWaitForTimeout(5000);  
  teststep("","");

  // 2) Check  ErrQ_HVMeas_UPackOutOfRangeLow>
  testStep("ENG8_4290.2","Check  ErrQ_HVMeas_UPackOutOfRangeLow>");
  testCaseComment("");
  errorBitBefore = getSignal(CAN2::SC_ERRORS_DAQ::ERR_UPack_RNGL);

  // 3) Set <P_HVMeas_UPack> = <L_Mem_HVMeas_ThresholdUPackOutOfRangeHigh_Param> + 1unit
  testStep("ENG8_4290.3","Set <P_HVMeas_UPack> = <L_Mem_HVMeas_ThresholdUPackOutOfRangeHigh_Param> - 1unit");
  testCaseComment("");
  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", (UPackOutOfRangeLo - 1) / 0.025);
  trickMemoryAfter();
  testWaitForTimeout(5000);
  
  // 4) Check  <ErrQ_HVMeas_UPackOutOfRangeLow>
  testStep("ENG8_4290.4","Check  <ErrQ_HVMeas_UPackOutOfRangeLow>");
  testCaseComment("");
  errorBitAfter = getSignal(CAN2::SC_ERRORS_DAQ::ERR_UPack_RNGL);
  
  // Assess
  func_ValueCompare("ErrQ_HVMeas_UPackOutOfRangeLow", errorBitBefore, RUNNING_FAILED, not_equal);
  func_ValueCompare("ErrQ_HVMeas_UPackOutOfRangeLow", errorBitAfter, RUNNING_FAILED, equal);
  
  //Cleanup
  func_CleanUp();  
}

testcase ENG8_4291 ()
{
  int errorBitBefore, errorBitAfter, UPackOutOfRangeLo;
  
  testCaseTitle("ENG8_4291","UPack not out of range - lower limit");
  testCaseDescription("After BMU start the UPack out of range error bit for the lower limit is initially checked to be unequal to passed. The UPack value is set to a value equal to the out of range threshold and the out of range error bit for the lower limit is observed to switch passed.");
  
  //Preconditions
  func_Preconditions();
  UPackOutOfRangeLo = funcIDE_ReadVariable("HvDaqMgrParameter.p_UPackOutOfRangeLo");
  UPackOutOfRangeLo = (int)((float)UPackOutOfRangeLo*0.025);
  
  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", (UPackOutOfRangeLo - 1) / 0.025);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 0);
  trickMemoryAfter();
  testWaitForTimeout(5000);  
  teststep("","");

  // 2) Check  ErrQ_HVMeas_UPackOutOfRangeLow>
  testStep("ENG8_4291.2","Check  ErrQ_HVMeas_UPackOutOfRangeLow>");
  testCaseComment("");
  errorBitBefore = getSignal(CAN2::SC_ERRORS_DAQ::ERR_UPack_RNGL);

  // 3) Set <P_HVMeas_UPack> = <L_Mem_HVMeas_ThresholdUPackOutOfRangeHigh_Param> + 1unit
  testStep("ENG8_4291.3","Set <P_HVMeas_UPack> = <L_Mem_HVMeas_ThresholdUPackOutOfRangeHigh_Param> + 1unit");
  testCaseComment("");
  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", UPackOutOfRangeLo / 0.025);
  trickMemoryAfter();
  testWaitForTimeout(5000);
  
  // 4) Check  <ErrQ_HVMeas_UPackOutOfRangeLow>
  testStep("ENG8_4291.4","Check  <ErrQ_HVMeas_UPackOutOfRangeLow>");
  testCaseComment("");
  errorBitAfter = getSignal(CAN2::SC_ERRORS_DAQ::ERR_UPack_RNGL);
  
  // Assess
  func_ValueCompare("ErrQ_HVMeas_UPackOutOfRangeLow", errorBitBefore, RUNNING_PASSED, not_equal);
  func_ValueCompare("ErrQ_HVMeas_UPackOutOfRangeLow", errorBitAfter, RUNNING_PASSED, equal);

  //Cleanup
  func_CleanUp();  
}

testcase ENG8_13970 ()
{
  int ULinkOutOfRangeLo;
  
  testCaseTitle("ENG8_13970","ULink out of range low <Err_HVMeas_ULinkOutOfRangeLow> = /FAILED/");
  testCaseDescription("The ULink value <iL_HVMeas_ULink>  is set to a value less <L_Mem_HVMeas_ThresholdULinkOutOfRangeLow_Param> the contactor states <L_ContCtrl_ContMainPos_State> and <L_ContCtrl_ContMainNeg_State> are set to /closed/. After this the undervoltage error <Err_HVMeas_ULinkOutOfRangeLow> is observed to switch to /FAILED/.");

    //Preconditions
  func_Preconditions();
  ULinkOutOfRangeLo = funcIDE_ReadVariable("HvDaqMgrParameter.p_ULinkOutOfRangeLo");
  ULinkOutOfRangeLo = ULinkOutOfRangeLo / 40;
  
  Func_ContCtrl_MainPos(ENABLE_Cont);
  Func_ContCtrl_MainNeg(ENABLE_Cont);
  
  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", 370 * 40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
  trickMemoryAfter();  
  testWaitForTimeout(5000);  
  teststep("","");

  // Check  <Err_HVMeas_ULinkOutOfRangeLow>
  func_ValueCompare("<Err_HVMeas_ULinkOutOfRangeLow>", getsignal(CAN2::SC_ERRORS_DAQ::ERR_ULink_RNGL), RUNNING_FAILED, not_equal);

  // Set <iL_HVMeas_ULink> = <L_Mem_HVMeas_ThresholdULinkOutOfRangeLow_Param> - 1unit
  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", (ULinkOutOfRangeLo - 1) * 40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
  trickMemoryAfter();
  testWaitForTimeout(5000);
  
  // Check  <Err_HVMeas_ULinkOutOfRangeLow>
  func_ValueCompare("<Err_HVMeas_ULinkOutOfRangeLow>", getsignal(CAN2::SC_ERRORS_DAQ::ERR_ULink_RNGL), RUNNING_FAILED, equal);
    
  //Cleanup
  func_CleanUp();  
}

testcase ENG8_13971 ()
{
  int ULinkOutOfRangeLo;
  
  testCaseTitle("ENG8_13971","ULink out of range low <Err_HVMeas_ULinkOutOfRangeLow> = /PASSED/");
  testCaseDescription("The ULink value <iL_HVMeas_ULink>  is set to a value equal or greater than <L_Mem_HVMeas_ThresholdULinkOutOfRangeLow_Param> the contactor states <L_ContCtrl_ContMainPos_State> and <L_ContCtrl_ContMainNeg_State> are set to /closed/. After this the undervoltage error <Err_HVMeas_ULinkOutOfRangeLow> is observed to switch to /PASSED/.");

    //Preconditions
  func_Preconditions();
  ULinkOutOfRangeLo = funcIDE_ReadVariable("HvDaqMgrParameter.p_ULinkOutOfRangeLo");
  ULinkOutOfRangeLo = ULinkOutOfRangeLo / 40;
  
  Func_ContCtrl_MainPos(ENABLE_Cont);
  Func_ContCtrl_MainNeg(ENABLE_Cont);
  
  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 1);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", (ULinkOutOfRangeLo - 1) * 40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
  trickMemoryAfter();  
  testWaitForTimeout(5000);  
  teststep("","");

  // Check  <Err_HVMeas_ULinkOutOfRangeLow>
  func_ValueCompare("<Err_HVMeas_ULinkOutOfRangeLow>", getsignal(CAN2::SC_ERRORS_DAQ::ERR_ULink_RNGL), RUNNING_PASSED, not_equal);

  // Set <iL_HVMeas_ULink> = <L_Mem_HVMeas_ThresholdULinkOutOfRangeLow_Param> - 1unit
  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", (ULinkOutOfRangeLo/* + 1 */) * 40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
  trickMemoryAfter();
  testWaitForTimeout(5000);
  
  // Check  <Err_HVMeas_ULinkOutOfRangeLow>
  func_ValueCompare("<Err_HVMeas_ULinkOutOfRangeLow>", getsignal(CAN2::SC_ERRORS_DAQ::ERR_ULink_RNGL), RUNNING_PASSED, equal);
    
  //Cleanup
  func_CleanUp();  
}

testcase ENG8_4256_4271()
{
  float startTime = 0.0, stopTime = 0.0, low = 0.0, high = 0.0, sum = 0.0, avg = 0.0;
  float req = 10.0, min = 8.0, max = 12.0;
  int i;
  testCaseTitle("ENG8_4256_4271", "UPack / ULink measurement update time");
  testCaseDescription("It is checked that the function that reads the pack / link voltage value is executed every 10ms with a jitter of 2ms.");
  
  // Preconditions
  func_Preconditions();

  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "{");
  
  for(i=0; i<20; i++)
  {
    funcIDE_WaitForHalt(2000);
    startTime = (float)funcIDE_ReadVariable("osSystemCounter");
    funcIDE_RunAndWait(2000);
    stopTime = (float)funcIDE_ReadVAriable("osSystemCounter");
    if(i==0)
    {
      low = high = stopTime-startTime;
    }
    else
    {
      if(stopTime-startTime < low) low = stopTime-startTime;
      if(stopTime-startTime > high) high = stopTime-startTime;
    }
    sum += stopTime - startTime;    
  }
  
  avg = sum / 20;
  testStep("", "low: %f", low);
  testStep("", "high: %f", high);
  testStep("", "sum: %f", sum);
  testStep("", "avg: %f", avg);
  
  if( (low >= min) && (high <= max) )
  {
    testStepPass("ENG8_4256", "Measurement cycle time passed: %f", avg);
  }
  else
  {
    testStepFail("ENG8_4256", "Measurement cycle time failed: %f", avg);
  }

  // Postconditions
  func_CleanUp();
}

testcase ENG8_4264_4265()
{
  long answer;
  testCaseTitle("ENG8_4264_4265", "UPack value for external transmission - (no) function execution");
  testCaseDescription("");
  
  // Preconditions
  func_Preconditions();

  // P_BMUTS_ENG8_4264
  
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "UDAbs_S16 = (sint16) (CtDaqMgr_UPack_S16 - UCellSum_a.Value);");
  funcIDE_WaitForHalt(2000);
  
  funcIDE_WriteVariable("UPack.State.bInvalid", 0);
  funcIDE_WriteVariable("ULink.State.bInvalid", 1);
  
  funcIDE_DeleteAllBP();
  funcWinIdea_SetBPSource("CtDaqMgr_HvVoltageFiltered", "{");
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "{");
  funcIDE_RunAndWait(2000);
  
  funcWinIdea_GetFunctionAtPositionAndCheck ("CtDaqMgr_HvVoltageFiltered");
  testWaitForTimeout(10000);
  
  // P_BMUTS_ENG8_4265
  
  funcIDE_DeleteAllBP();
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "UDAbs_S16 = (sint16) (CtDaqMgr_UPack_S16 - UCellSum_a.Value);");
  funcIDE_RunAndWait(2000);
  
  funcIDE_WriteVariable("UPack.State.bInvalid", 1);
  funcIDE_WriteVariable("ULink.State.bInvalid", 1);

  funcIDE_DeleteAllBP();
  funcWinIdea_SetBPSource("CtDaqMgr_HvVoltageFiltered", "{");
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "{");
  funcIDE_RunAndWait(2000);

  funcWinIdea_GetFunctionAtPositionAndCheck ("CtDaqMgr_HighVoltages");

  // Postconditions
  trickMemoryAfter();
  func_CleanUp();
}

testcase ENG8_4273_4274()
{
  long answer;
  testCaseTitle("ENG8_4273_4274", "ULink value for external transmission - (no) function execution");
  testCaseDescription("");
  
  // Preconditions
  func_Preconditions();

  // P_BMUTS_ENG8_4264
  
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "UDAbs_S16 = (sint16) (CtDaqMgr_UPack_S16 - UCellSum_a.Value);");
  funcIDE_WaitForHalt(2000);
  
  funcIDE_WriteVariable("UPack.State.bInvalid", 1);
  funcIDE_WriteVariable("ULink.State.bInvalid", 0);
  
  funcIDE_DeleteAllBP();
  funcWinIdea_SetBPSource("CtDaqMgr_HvVoltageFiltered", "{");
  funcIDE_RunAndWait(2000);
  
  funcWinIdea_GetFunctionAtPositionAndCheck ("CtDaqMgr_HvVoltageFiltered");
  testWaitForTimeout(10000);
  
  // P_BMUTS_ENG8_4274
  
  funcIDE_DeleteAllBP();
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "UDAbs_S16 = (sint16) (CtDaqMgr_UPack_S16 - UCellSum_a.Value);");
  funcIDE_RunAndWait(2000);
  
  funcIDE_WriteVariable("UPack.State.bInvalid", 1);
  funcIDE_WriteVariable("ULink.State.bInvalid", 1);

  funcIDE_DeleteAllBP();
  funcWinIdea_SetBPSource("CtDaqMgr_HvVoltageFiltered", "{");
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "{");
  funcIDE_RunAndWait(2000);

  funcWinIdea_GetFunctionAtPositionAndCheck ("CtDaqMgr_HighVoltages");

  // Postconditions
  trickMemoryAfter();
  func_CleanUp();
}

testcase ENG8_4258()
{
  testCaseTitle("ENG8_4258", "UPack value for external transmission - correct value calculation");
  testCaseDescription("");
  
  // Preconditions
  func_Preconditions();

  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_IsPackSna_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_IsPackSna_VAL_B", 0);
  trickMemoryAfter();
  
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "{");
  funcIDE_WaitForHalt(2000);
  
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 155 * 40);
  funcIDE_RunAndWait(2000);
  
  check_equality(funcIDE_ReadVariable("CtDaqMgr_UPackFiltered_S16" /*"UOut_S16_b"*/), (0 + (155*40)) / 2, "Voltage calculation ok", "Voltage calculation not ok");
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 157 * 40);  
  funcIDE_RunAndWait(2000);
  check_equality(funcIDE_ReadVariable("CtDaqMgr_UPackFiltered_S16" /*"UOut_S16_b"*/), ( (155*40) + (157*40) ) / 2, "Voltage calculation ok", "Voltage calculation not ok");
  
  // Postconditions
  trickMemoryAfter();
  func_CleanUp();
}

testcase ENG8_4275()
{
  testCaseTitle("ENG8_4275", "ULink value for external transmission - correct value calculation");
  testCaseDescription("");
  
  // Preconditions
  func_Preconditions();

  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_IsLinkSna_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_IsLinkSna_VAL_B", 0);
  trickMemoryAfter();
  
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "{");
  funcIDE_WaitForHalt(2000);
  
  check_equality(funcIDE_ReadVariable("CtDaqMgr_ULinkFiltered_S16"), 0, "Voltage calculation ok", "Voltage calculation not ok");
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", 155);  
  funcIDE_RunAndWait(2000);
  
  check_equality(funcIDE_ReadVariable("CtDaqMgr_ULinkFiltered_S16"), (0 + 155) / 2, "Voltage calculation ok", "Voltage calculation not ok");
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", 157);  
  funcIDE_RunAndWait(2000);
  check_equality(funcIDE_ReadVariable("CtDaqMgr_ULinkFiltered_S16"), (155 + 157) / 2, "Voltage calculation ok", "Voltage calculation not ok");
  
  // Postconditions
  trickMemoryAfter();
  func_CleanUp();
}

testcase ENG8_11098()
{
  testCaseTitle("ENG8_11098", "Event HvULinkPosUpdate occurs when (HvULinkUpdate  AND HvULinkSigStateIsValid AND HvIsoGndRelayStateClosed)");
  testCaseDescription("");
  
  // CR 706968: The signal CtDaqMgr_UIsoGndFiltered_U16 is already converted for the 13-bit signal on CAN with a resolution of 0.1 V and the resulting SNA value of raw 0x1FFF.
  
  // Preconditions
  func_Preconditions();

  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_IsLinkSna_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_IsLinkSna_VAL_B", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_IsoGndRelayState_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_IsoGndRelayState_VAL_B", 1);
  trickMemoryAfter();
  
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "Rte_Write_PpCtApDaqMgrULink_ULink(&ULink);");
  funcIDE_WaitForHalt(2000);
  
  check_equality(funcIDE_ReadVariable("CtDaqMgr_UIsoGndFiltered_U16"), 0, "Voltage calculation ok", "Voltage calculation not ok");
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", 155);
  
  funcIDE_RunAndWait(2000);
    
  check_equality(funcIDE_ReadVariable("CtDaqMgr_UIsoGndFiltered_U16"), ( (0 + 155) / 2 ) / 4, "Voltage calculation ok", "Voltage calculation not ok");
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", 157);  
  funcIDE_RunAndWait(2000);
  check_equality(funcIDE_ReadVariable("CtDaqMgr_UIsoGndFiltered_U16"), ( (155 + 157) / 2 ) / 4, "Voltage calculation ok", "Voltage calculation not ok");
  
  // Postconditions
  trickMemoryAfter();
  func_CleanUp();
}

testcase ENG8_11099()
{
  long os1, os2, retVal;
  testCaseTitle("ENG8_11099", "Event HvULinkPosSNA occurs during [0,20] ms when (HvULinkSigStateIsInvalid OR HvIsoGndRelayStateOpen)");
  testCaseDescription("");
  
  // CR 706968: The signal CtDaqMgr_UIsoGndFiltered_U16 is already converted for the 13-bit signal on CAN with a resolution of 0.1 V and the resulting SNA value of raw 0x1FFF.

  // Preconditions
  func_Preconditions();

  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_IsLinkSna_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_IsLinkSna_VAL_B", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_IsoGndRelayState_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_IsoGndRelayState_VAL_B", 1);
  trickMemoryAfter();
  
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "Rte_Write_PpCtApDaqMgrULink_ULink(&ULink);");
  funcIDE_WaitForHalt(2000);
  
  check_equality(funcIDE_ReadVariable("CtDaqMgr_UIsoGndFiltered_U16"), 0, "Voltage calculation ok", "Voltage calculation not ok");
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", 0x7fff);
  os1 = funcIDE_ReadVariable("osSystemCounter");
  
  retVal = testWaitForVariableValue("CtDaqMgr_UIsoGndFiltered_U16", 0x1fff, 10000);
  os2 = funcIDE_ReadVariable("osSystemCounter");
    
  check_equality(retVal, 0x1fff, "Voltage calculation ok", "Voltage calculation not ok");
  func_timeCompare("time measurement", "ms", os2-os1, 20, equal_or_less_than);
  
  // Postconditions
  trickMemoryAfter();
  func_CleanUp();
}

testcase ENG8_4272()
{
  long os1, os2, ULinkMeasured, ULinkExpected;
  long ULinkSlopeCal, ULinkOffsetCal;
  testCaseTitle("ENG8_4272", "ULink  measurement value update");
  testCaseDescription("");
  
  // Preconditions
  func_Preconditions();

  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_IsLinkSna_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_IsLinkSna_VAL_B", 0);
  trickMemoryAfter();
  
  ULinkSlopeCal = funcIDE_ReadVariable("HvDaqMgrCalPara.ULinkSlopeCal");
  ULinkOffsetCal = funcIDE_ReadVariable("HvDaqMgrCalPara.ULinkOffsetCal");

  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "Rte_Write_PpCtApDaqMgrULink_ULink(&ULink);");
  funcIDE_WaitForHalt(2000);
  check_equality(funcIDE_ReadVariable("CtDaqMgr_ULink_S16"), 0, "Voltage calculation ok", "Voltage calculation not ok");

  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 10);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", 110);
  os1 = funcIDE_ReadVariable("@STM_CNT");
  
  ULinkExpected = ( (110 - 10) * (ULinkSlopeCal * 0.0001) ) + ULinkOffsetCal;  
  ULinkMeasured = testWaitForVariableValue("CtDaqMgr_ULink_S16", ULinkExpected, 10000);
  os2 = funcIDE_ReadVariable("@STM_CNT");
    
  check_equality(ULinkMeasured, ULinkExpected, "Voltage calculation ok", "Voltage calculation not ok");
  func_timeCompare("time measurement", "ms", (os2-os1)/64000, 10+2 /* for jitter see P_BMUSRS_8409 */, equal_or_less_than);
  func_timeCompare("time measurement", "ms", (os2-os1)/64000, 10-2 /* for jitter see P_BMUSRS_8409 */, equal_or_greater_than);
  
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 55);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", 255);
  os1 = funcIDE_ReadVariable("@STM_CNT");
  
  ULinkExpected = ( (255 - 55) * (ULinkSlopeCal * 0.0001) ) + ULinkOffsetCal;  
  ULinkMeasured = testWaitForVariableValue("CtDaqMgr_ULink_S16", ULinkExpected, 10000);
  os2 = funcIDE_ReadVariable("@STM_CNT");
    
  check_equality(ULinkMeasured, ULinkExpected, "Voltage calculation ok", "Voltage calculation not ok");
  func_timeCompare("time measurement", "ms", (os2-os1)/64000, 10+2 /* for jitter see P_BMUSRS_8409 */, equal_or_less_than);
  func_timeCompare("time measurement", "ms", (os2-os1)/64000, 10-2 /* for jitter see P_BMUSRS_8409 */, equal_or_greater_than);

  // Postconditions
  trickMemoryAfter();
  func_CleanUp();
}

testcase ENG8_4257()
{
  long os1, os2, UPackMeasured, UPackExpected;
  long UPackSlopeCal, UPackOffsetCal;
  testCaseTitle("ENG8_4257", "UPack  measurement value update");
  testCaseDescription("");
  
  // Preconditions
  func_Preconditions();

  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_IsPackSna_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_IsPackSna_VAL_B", 0);
  trickMemoryAfter();
  
  UPackSlopeCal = funcIDE_ReadVariable("HvDaqMgrCalPara.UPackSlopeCal");
  UPackOffsetCal = funcIDE_ReadVariable("HvDaqMgrCalPara.UPackOffsetCal");

  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "Rte_Write_PpCtApDaqMgrULink_ULink(&ULink);");
  funcIDE_WaitForHalt(2000);
  check_equality(funcIDE_ReadVariable("CtDaqMgr_UPack_S16"), 0, "Voltage calculation ok", "Voltage calculation not ok");

  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 10*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 110*40);
  os1 = funcIDE_ReadVariable("@STM_CNT");
  
  UPackExpected = ( ( (110*40) - (10*40) ) * (UPackSlopeCal * 0.0001) ) + UPackOffsetCal;  
  UPackMeasured = testWaitForVariableValue("CtDaqMgr_UPack_S16", UPackExpected, 10000);
  os2 = funcIDE_ReadVariable("@STM_CNT");
    
  check_equality(UPackMeasured, UPackExpected, "Voltage calculation ok", "Voltage calculation not ok");
  func_timeCompare("time measurement", "ms", (os2-os1)/64000, 10+2 /* for jitter see P_BMUSRS_8391 */, equal_or_less_than);
  func_timeCompare("time measurement", "ms", (os2-os1)/64000, 10-2 /* for jitter see P_BMUSRS_8391 */, equal_or_greater_than);
  
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 55*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 255*40);
  os1 = funcIDE_ReadVariable("@STM_CNT");
  
  UPackExpected = ( ( (255*40) - (55*40) ) * (UPackSlopeCal * 0.0001) ) + UPackOffsetCal;  
  UPackMeasured = testWaitForVariableValue("CtDaqMgr_UPack_S16", UPackExpected, 10000);
  os2 = funcIDE_ReadVariable("@STM_CNT");
    
  check_equality(UPackMeasured, UPackExpected, "Voltage calculation ok", "Voltage calculation not ok");
  func_timeCompare("time measurement", "ms", (os2-os1)/64000, 10+2 /* for jitter see P_BMUSRS_8391 */, equal_or_less_than);
  func_timeCompare("time measurement", "ms", (os2-os1)/64000, 10-2 /* for jitter see P_BMUSRS_8391 */, equal_or_greater_than);

  // Postconditions
  trickMemoryAfter();
  func_CleanUp();
}

testcase ENG8_4266()
{
  float startTime = 0.0, stopTime = 0.0, low = 0.0, high = 0.0, sum = 0.0, avg = 0.0;
  float req = 20.0, min = 0.0, max = 20.0;
  int i;
  testCaseTitle("ENG8_4266", "UPack value for external transmission - measurement update time");
  testCaseDescription("");
  
  // Preconditions
  func_Preconditions();

  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_IsPackSna_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_IsPackSna_VAL_B", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", 32767);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_IsLinkSna_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_IsLinkSna_VAL_B", 1);
  trickMemoryAfter();

  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "{");
  
  for(i=0; i<20; i++)
  {
    funcIDE_WaitForHalt(2000);
    startTime = (float)funcIDE_ReadVariable("osSystemCounter");
    funcIDE_RunAndWait(2000);
    stopTime = (float)funcIDE_ReadVAriable("osSystemCounter");
    if(i==0)
    {
      low = high = stopTime-startTime;
    }
    else
    {
      if(stopTime-startTime < low) low = stopTime-startTime;
      if(stopTime-startTime > high) high = stopTime-startTime;
    }
    sum += stopTime - startTime;    
  }
  
  avg = sum / 20;
  testStep("", "low: %f", low);
  testStep("", "high: %f", high);
  testStep("", "sum: %f", sum);
  testStep("", "avg: %f", avg);
  
  if( (low >= min)&& (high <= max) )
  {
    testStepPass("ENG8_4266", "Measurement cycle time passed: %f", avg);
  }
  else
  {
    testStepFail("ENG8_4266", "Measurement cycle time failed: %f", avg);
  }

  // Postconditions
  func_CleanUp();
}

testcase ENG8_4276()
{
  float startTime = 0.0, stopTime = 0.0, low = 0.0, high = 0.0, sum = 0.0, avg = 0.0;
  float req = 20.0, min = 0.0, max = 20.0;
  int i;
  testCaseTitle("ENG8_4276", "ULink value for external transmission - measurement update time");
  testCaseDescription("");
  
  // Preconditions
  func_Preconditions();

  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 32767);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_IsPackSna_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_IsPackSna_VAL_B", 1);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_IsLinkSna_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_IsLinkSna_VAL_B", 0);
  funcIDE_WriteVariable("HvDaqMgrParameter.p_facRelayDebounce", 0xff);
  trickMemoryAfter();

  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "{");
  
  for(i=0; i<20; i++)
  {
    funcIDE_WaitForHalt(2000);
    startTime = (float)funcIDE_ReadVariable("osSystemCounter");
    funcIDE_RunAndWait(2000);
    stopTime = (float)funcIDE_ReadVAriable("osSystemCounter");
    if(i==0)
    {
      low = high = stopTime-startTime;
    }
    else
    {
      if(stopTime-startTime < low) low = stopTime-startTime;
      if(stopTime-startTime > high) high = stopTime-startTime;
    }
    sum += stopTime - startTime;    
  }
  
  avg = sum / 20;
  testStep("", "low: %f", low);
  testStep("", "high: %f", high);
  testStep("", "sum: %f", sum);
  testStep("", "avg: %f", avg);
  
  if( (low >= min)&& (high <= max) )
  {
    testStepPass("ENG8_4276", "Measurement cycle time passed: %f", avg);
  }
  else
  {
    testStepFail("ENG8_4276", "Measurement cycle time failed: %f", avg);
  }

  // Postconditions
  func_CleanUp();
}

testcase ENG8_10999()
{
  long os1 = 0, os2 = 0;
  long outdated = 0, invalid = 0;
  
  testCaseTitle("ENG8_10999", "Event HvU[Pack|Link]Outdated when (HvU[Pack|Link]PosSigIsInvalid == 0) AND (HvU[Pack|Link]NegSigIsInvalid == 0) AND (HvU[Pack|Link]E2EInvalid == 1) ");
  testCaseDescription("");
  // Preconditions
  func_Preconditions();

  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 100*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", TRUE); // PLUGIN
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", TRUE); // PLUGIN 
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_VAL_B", 1);
  trickMemoryAfter();
  
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "Rte_Write_PpCtApDaqMgrULink_ULink(&ULink);");
  funcIDE_WaitForHalt(2000);

  invalid = testWaitForVariableValue("UPack.State.bInvalid", 0, 20000);
  check_equality(invalid, 0, "<iL_HVMeas_UPack_State.bInvalid> has reached the expected value", "<iL_HVMeas_UPack_State.bInvalid> has not reached the expected value");
  
  func_ValueCompare("<iL_HVMeas_UPack_State.bOutdated>", funcIDE_ReadVariable("UPack.State.bOutdated"), 0, equal);
  os1 = funcIDE_ReadVariable("@STM_CNT");
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_VAL_B", 0);
  
  outdated = testWaitForVariableValue("UPack.State.bOutdated", 1, 20000);

  os2 = funcIDE_ReadVariable("@STM_CNT");
  check_equality(outdated, 1, "<iL_HVMeas_UPack_State.bOutdated> has reached the expected value", "<iL_HVMeas_UPack_State.bOutdated> has not reached the expected value");
  
  ///////////////////// NEW CODE //////////////////////
  // func_timeCompare("time measurement", "ms", (os2 - os1)/64000, funcIDE_ReadVariable("HvDaqMgrParameter.p_facHvMeasTimeout") * 10, less_than);
  func_timeCompare("time measurement", "ms", (os2 - os1) / 64000, L_Mem_HVMeas_Timeout_Param_WITH_TOLERANCE, less_than);
  ///////////////////// NEW CODE //////////////////////
  
  // Postconditions
  func_CleanUp();
}

testcase ENG8_11039()
{
  long os1 = 0, os2 = 0;
  long outdated = 0;
  
  testCaseTitle("ENG8_11039", "");
  testCaseDescription("");
  // Preconditions
  func_Preconditions();

  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 100*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", TRUE); // PLUGIN
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", TRUE); // PLUGIN 
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_VAL_B", 1);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_LinkRelayState_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_LinkRelayState_VAL_B", 1);
  trickMemoryAfter();
  
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "Rte_Write_PpCtApDaqMgrULink_ULink(&ULink);");
  funcIDE_WaitForHalt(2000);
  
  func_ValueCompare("<iL_HVMeas_UPack_State.bOutdated>", funcIDE_ReadVariable("ULink.State.bOutdated"), 0, equal);
  os1 = funcIDE_ReadVariable("osSystemCounter");
  funcIDE_WriteVariable("CtDaqMgr_Xcp_LinkRelayState_VAL_B", 0);

  outdated = testWaitForVariableValue("ULink.State.bOutdated", 1, 10000);

  os2 = funcIDE_ReadVariable("osSystemCounter");
  check_equality(outdated, 1, "<iL_HVMeas_ULink_State.bOutdated> has reached the expected value", "<iL_HVMeas_ULink_State.bOutdated> has not reached the expected value");
  func_timeCompare("time measurement", "ms", os2 - os1, L_Mem_HVMeas_Timeout_Param_WITH_TOLERANCE, less_than);
  
  // Postconditions
  func_CleanUp();
}

testcase ENG8_11367_11382()
{
  long os1 = 0, os2 = 0;
  long outdated = 0;
  
  testCaseTitle("ENG8_11367_11382", "Event HvU[Pack|Link]Outdated when (HvU[Pack|Link]PosSigIsInvalid == 0) AND (HvU[Pack|Link]NegSigIsInvalid == 0) AND (HvU[Pack|Link]E2EInvalid == 1) ");
  testCaseDescription("");

  // Preconditions
  func_Preconditions();

  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 100*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", 100*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", TRUE); // PLUGIN
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", TRUE); // PLUGIN 
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_VAL_B", 1);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_LinkRelayState_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_LinkRelayState_VAL_B", 1);
  trickMemoryAfter();
  
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "Rte_Write_PpCtApDaqMgrULink_ULink(&ULink);");
  funcIDE_WaitForHalt(2000);
  
  func_ValueCompare("<iL_HVMeas_UPack_State.bOutdated>", funcIDE_ReadVariable("UPack.State.bOutdated"), 0, equal);
  func_ValueCompare("<iL_HVMeas_ULink_State.bOutdated>", funcIDE_ReadVariable("ULink.State.bOutdated"), 0, equal);
  
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "bLinkComErr_B = (X_SHvDaq25_StatusCnt > 10) || Status_B;");  
  funcIDE_RunAndWait(2000);
  os1 = funcIDE_ReadVariable("@STM_CNT");
  
  funcIDE_WriteVariable("Status_B", 1); // E2E error
  funcIDE_RunAndWait(2000);
  os2 = funcIDE_ReadVariable("@STM_CNT");
  
  check_equality(funcIDE_ReadVariable("UPack.State.bOutdated"), 1, "<iL_HVMeas_UPack_State.bOutdated> has reached the expected value", "<iL_HVMeas_UPack_State.bOutdated> has not reached the expected value");
  check_equality(funcIDE_ReadVariable("ULink.State.bOutdated"), 1, "<iL_HVMeas_ULink_State.bOutdated> has reached the expected value", "<iL_HVMeas_ULink_State.bOutdated> has not reached the expected value");
  // in fact it happens in one task cycle
  func_timeCompare("time measurement", "ms", (os2 - os1) / 64000, L_Mem_HVMeas_Timeout_Param_WITH_TOLERANCE, less_than);
  
  // Postconditions
  func_CleanUp();
}

testcase ENG8_11368()
{
  long os1 = 0, os2 = 0, result = 0;
  long outdated = 0;
  
  testCaseTitle("ENG8_11368", "Event HvUPackOutdated when (HvUPackPosSigIsInvalid == 0) AND (HvUPackNegSigIsInvalid == 0) AND (HvUPackE2EInvalid == 1) AND (HvPackRelayStateOpen == 1)");
  testCaseDescription("");

  // Preconditions
  func_Preconditions();

  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 100*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", TRUE); // PLUGIN
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", TRUE); // PLUGIN 
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_VAL_B", 1);
  trickMemoryAfter();
  
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "Rte_Write_PpCtApDaqMgrULink_ULink(&ULink);");
  funcIDE_WaitForHalt(2000);
  
  func_ValueCompare("<iL_HVMeas_UPack_State.bOutdated>", funcIDE_ReadVariable("UPack.State.bOutdated"), 0, equal);
  os1 = funcIDE_ReadVariable("@STM_CNT");
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_VAL_B", 0);

  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "bLinkComErr_B = (X_SHvDaq25_StatusCnt > 10) || Status_B;");  
  funcIDE_RunAndWait(2000);
  
  funcIDE_WriteVariable("Status_B", 1); // E2E error
  
  //funcIDE_RunAndWait(2000);
  result =testWaitForVariableValue("UPack.State.bOutdated",1,11000);
  os2 = funcIDE_ReadVariable("@STM_CNT");
  
  check_equality(result, 1, "<iL_HVMeas_UPack_State.bOutdated> has reached the expected value", "<iL_HVMeas_UPack_State.bOutdated> has not reached the expected value");
  func_timeCompare("time measurement", "ms", (os2 - os1)/64000, L_Mem_HVMeas_Timeout_Param_WITH_TOLERANCE, less_than);
  
  // Postconditions
  func_CleanUp();
}

testcase ENG8_11383()
{
  long os1 = 0, os2 = 0;
  long outdated = 0;
  
  testCaseTitle("ENG8_11383", "Event HvULinkOutdated when (HvULinkPosSigIsInvalid == 0) AND (HvULinkNegSigIsInvalid == 0) AND (HvULinkE2EInvalid == 1) AND (HvULinkRelayStateOpen == 1)");
  testCaseDescription("");

  // Preconditions
  func_Preconditions();

  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", 100*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 100*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", TRUE); // PLUGIN
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", TRUE); // PLUGIN 
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_VAL_B", 1);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_LinkRelayState_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_LinkRelayState_VAL_B", 1);
  trickMemoryAfter();
  
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "Rte_Write_PpCtApDaqMgrULink_ULink(&ULink);");
  funcIDE_WaitForHalt(2000);
  
  func_ValueCompare("<iL_HVMeas_ULink_State.bOutdated>", funcIDE_ReadVariable("ULink.State.bOutdated"), 0, equal);
  os1 = funcIDE_ReadVariable("osSystemCounter");
  funcIDE_WriteVariable("CtDaqMgr_Xcp_LinkRelayState_VAL_B", 0);

  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "bLinkComErr_B = (X_SHvDaq25_StatusCnt > 10) || Status_B;");  
  funcIDE_RunAndWait(2000);
  
  funcIDE_WriteVariable("Status_B", 1); // E2E error
  funcIDE_RunAndWait(2000);
  os2 = funcIDE_ReadVariable("osSystemCounter");
  
  check_equality(funcIDE_ReadVariable("ULink.State.bOutdated"), 1, "<iL_HVMeas_ULink_State.bOutdated> has reached the expected value", "<iL_HVMeas_ULink_State.bOutdated> has not reached the expected value");
  func_timeCompare("time measurement", "ms", os2 - os1, L_Mem_HVMeas_Timeout_Param_WITH_TOLERANCE, less_than);
  
  // Postconditions
  func_CleanUp();
}

testcase ENG8_11369()
{
  long os1 = 0, os2 = 0;
  long outdated = 0;
  
  testCaseTitle("ENG8_11369", "Event HvUPackOutdated when (HvUPackPosSigIsInvalid == 0) AND (HvUPackNegSigIsInvalid == 1) AND (HvUPackE2EInvalid == 0) AND (HvUPackRelayStateOpen == 0)");
  testCaseDescription("");

  // Preconditions
  func_Preconditions();

  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 100*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", TRUE); // PLUGIN
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", TRUE); // PLUGIN 
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_VAL_B", 1);
  trickMemoryAfter();
  
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "Rte_Write_PpCtApDaqMgrULink_ULink(&ULink);");
  funcIDE_WaitForHalt(2000);
  
  func_ValueCompare("<iL_HVMeas_UPack_State.bOutdated>", funcIDE_ReadVariable("UPack.State.bOutdated"), 0, equal);
  os1 = funcIDE_ReadVariable("@STM_CNT");
  
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 32767);
  
  outdated = testWaitForVariableValue("UPack.State.bOutdated", 1, 10000);

  os2 = funcIDE_ReadVariable("@STM_CNT");
  check_equality(outdated, 1, "<iL_HVMeas_UPack_State.bOutdated> has reached the expected value", "<iL_HVMeas_UPack_State.bOutdated> has not reached the expected value");
  func_timeCompare("time measurement", "ms", (os2 - os1) / 64000, L_Mem_HVMeas_Timeout_Param_WITH_TOLERANCE, less_than);
  
  // Postconditions
  func_CleanUp();
}

testcase ENG8_11384()
{
  long os1 = 0, os2 = 0;
  long outdated = 0;
  
  testCaseTitle("ENG8_11384", "Event HvULinkOutdated when (HvULinkPosSigIsInvalid == 0) AND (HvULinkNegSigIsInvalid == 1) AND (HvULinkE2EInvalid == 0) AND (HvULinkRelayStateOpen == 0)");
  testCaseDescription("");

  // Preconditions
  func_Preconditions();

  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 100*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", TRUE); // PLUGIN
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", TRUE); // PLUGIN 
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_VAL_B", 1);
  trickMemoryAfter();
  
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "Rte_Write_PpCtApDaqMgrULink_ULink(&ULink);");
  funcIDE_WaitForHalt(2000);
  
  func_ValueCompare("<iL_HVMeas_UPack_State.bOutdated>", funcIDE_ReadVariable("ULink.State.bOutdated"), 0, equal);
  os1 = funcIDE_ReadVariable("@STM_CNT");
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 32767);

  outdated = testWaitForVariableValue("ULink.State.bOutdated", 1, 10000);

  os2 = funcIDE_ReadVariable("@STM_CNT");
  check_equality(outdated, 1, "<iL_HVMeas_ULink_State.bOutdated> has reached the expected value", "<iL_HVMeas_ULink_State.bOutdated> has not reached the expected value");
  func_timeCompare("time measurement", "ms", (os2 - os1) / 64000, L_Mem_HVMeas_Timeout_Param_WITH_TOLERANCE, less_than);
  
  // Postconditions
  func_CleanUp();
}

testcase ENG8_11370()
{
  long os1 = 0, os2 = 0;
  long outdated = 0;
  
  testCaseTitle("ENG8_11370", "Event HvUPackOutdated when (HvUPackPosSigIsInvalid == 0) AND (HvUPackNegSigIsInvalid == 1) AND (HvUPackE2EInvalid == 0) AND (HvUPackRelayStateOpen == 1)");
  testCaseDescription("");

  // Preconditions
  func_Preconditions();

  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 100*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", TRUE); // PLUGIN
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", TRUE); // PLUGIN 
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_VAL_B", 1);
  trickMemoryAfter();
  
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "Rte_Write_PpCtApDaqMgrULink_ULink(&ULink);");
  funcIDE_WaitForHalt(2000);
  
  func_ValueCompare("<iL_HVMeas_UPack_State.bOutdated>", funcIDE_ReadVariable("UPack.State.bOutdated"), 0, equal);
  os1 = funcIDE_ReadVariable("@STM_CNT");
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_VAL_B", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 32767);
  testWaitForTimeout(2000);
  
  outdated = testWaitForVariableValue("UPack.State.bOutdated", 1, 10000);
  os2 = funcIDE_ReadVariable("@STM_CNT");
  check_equality(outdated, 1, "<iL_HVMeas_UPack_State.bOutdated> has reached the expected value", "<iL_HVMeas_UPack_State.bOutdated> has not reached the expected value");
  func_timeCompare("time measurement", "ms", (os2 - os1) / 64000, L_Mem_HVMeas_Timeout_Param_WITH_TOLERANCE, less_than);
  
  // Postconditions
  func_CleanUp();
}

testcase ENG8_11385()
{
  long os1 = 0, os2 = 0;
  long outdated = 0;
  
  testCaseTitle("ENG8_11385", "Event HvULinkOutdated when (HvULinkPosSigIsInvalid == 0) AND (HvULinkNegSigIsInvalid == 1) AND (HvULinkE2EInvalid == 0) AND (HvULinkRelayStateOpen == 1)");
  testCaseDescription("");

  // Preconditions
  func_Preconditions();

  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", 100*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 100*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", TRUE); // PLUGIN
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", TRUE); // PLUGIN 
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_LinkRelayState_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_LinkRelayState_VAL_B", 1);
  trickMemoryAfter();
  
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "Rte_Write_PpCtApDaqMgrULink_ULink(&ULink);");
  funcIDE_WaitForHalt(2000);
  
  func_ValueCompare("<iL_HVMeas_ULink_State.bOutdated>", funcIDE_ReadVariable("ULink.State.bOutdated"), 0, equal);
  os1 = funcIDE_ReadVariable("@STM_CNT");
  funcIDE_WriteVariable("CtDaqMgr_Xcp_LinkRelayState_VAL_B", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 32767);
  testWaitForTimeout(2000);

  outdated = testWaitForVariableValue("ULink.State.bOutdated", 1, 10000);
  os2 = funcIDE_ReadVariable("@STM_CNT");
  check_equality(outdated, 1, "<iL_HVMeas_ULink_State.bOutdated> has reached the expected value", "<iL_HVMeas_ULink_State.bOutdated> has not reached the expected value");
  func_timeCompare("time measurement", "ms", (os2 - os1) / 64000, L_Mem_HVMeas_Timeout_Param_WITH_TOLERANCE, less_than);
  
  // Postconditions
  func_CleanUp();
}

testcase ENG8_11371()
{
  long os1 = 0, os2 = 0;
  long outdated = 0;
  
  testCaseTitle("ENG8_11371", "Event HvUPackOutdated when (HvUPackPosSigIsInvalid == 0) AND (HvUPackNegSigIsInvalid == 1) AND (HvUPackE2EInvalid == 1) AND (HvUPackRelayStateOpen == 0)");
  testCaseDescription("");

  // Preconditions
  func_Preconditions();

  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 100*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", 100*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", TRUE); // PLUGIN
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", TRUE); // PLUGIN 
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_VAL_B", 1);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_LinkRelayState_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_LinkRelayState_VAL_B", 1);
  trickMemoryAfter();
  
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "Rte_Write_PpCtApDaqMgrULink_ULink(&ULink);");
  funcIDE_WaitForHalt(2000);
  
  func_ValueCompare("<iL_HVMeas_UPack_State.bOutdated>", funcIDE_ReadVariable("UPack.State.bOutdated"), 0, equal);
  os1 = funcIDE_ReadVariable("@STM_CNT");
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 32767);
  
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "bLinkComErr_B = (X_SHvDaq25_StatusCnt > 10) || Status_B;");  
  funcIDE_RunAndWait(2000);
  
  funcIDE_WriteVariable("Status_B", 1); // E2E error
  funcIDE_RunAndWait(2000);
  os2 = funcIDE_ReadVariable("@STM_CNT");
  
  check_equality(funcIDE_ReadVariable("UPack.State.bOutdated"), 1, "<iL_HVMeas_UPack_State.bOutdated> has reached the expected value", "<iL_HVMeas_UPack_State.bOutdated> has not reached the expected value");
  func_timeCompare("time measurement", "ms", (os2 - os1) / 64000, L_Mem_HVMeas_Timeout_Param_WITH_TOLERANCE, less_than);
  
  // Postconditions
  func_CleanUp();
}

testcase ENG8_11386()
{
  long os1 = 0, os2 = 0;
  long outdated = 0;
  
  testCaseTitle("ENG8_11386", "Event HvULinkOutdated when (HvULinkPosSigIsInvalid == 0) AND (HvULinkNegSigIsInvalid == 1) AND (HvULinkE2EInvalid == 1) AND (HvULinkRelayStateOpen == 0)");
  testCaseDescription("");

  // Preconditions
  func_Preconditions();

  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 100*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", 100*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", TRUE); // PLUGIN
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", TRUE); // PLUGIN 
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_VAL_B", 1);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_LinkRelayState_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_LinkRelayState_VAL_B", 1);
  trickMemoryAfter();
  
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "Rte_Write_PpCtApDaqMgrULink_ULink(&ULink);");
  funcIDE_WaitForHalt(2000);
  
  func_ValueCompare("<iL_HVMeas_ULink_State.bOutdated>", funcIDE_ReadVariable("ULink.State.bOutdated"), 0, equal);
  os1 = funcIDE_ReadVariable("@STM_CNT");
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 32767);
  
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "bLinkComErr_B = (X_SHvDaq25_StatusCnt > 10) || Status_B;");  
  funcIDE_RunAndWait(2000);
  
  funcIDE_WriteVariable("Status_B", 1); // E2E error
  funcIDE_RunAndWait(2000);
  os2 = funcIDE_ReadVariable("@STM_CNT");
  
  check_equality(funcIDE_ReadVariable("ULink.State.bOutdated"), 1, "<iL_HVMeas_ULink_State.bOutdated> has reached the expected value", "<iL_HVMeas_ULink_State.bOutdated> has not reached the expected value");
  func_timeCompare("time measurement", "ms", (os2 - os1) / 64000, L_Mem_HVMeas_Timeout_Param_WITH_TOLERANCE, less_than);
  
  // Postconditions
  func_CleanUp();
}

testcase ENG8_11372()
{
  long os1 = 0, os2 = 0;
  long outdated = 0;
  
  testCaseTitle("ENG8_11372", "Event HvUPackOutdated when (HvUPackPosSigIsInvalid == 0) AND (HvUPackNegSigIsInvalid == 1) AND (HvUPackE2EInvalid == 1) AND (HvUPackRelayStateOpen == 1)");
  testCaseDescription("");

  // Preconditions
  func_Preconditions();

  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 100*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", 100*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", TRUE); // PLUGIN
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", TRUE); // PLUGIN 
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_VAL_B", 1);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_LinkRelayState_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_LinkRelayState_VAL_B", 1);
  trickMemoryAfter();
  
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "Rte_Write_PpCtApDaqMgrULink_ULink(&ULink);");
  funcIDE_WaitForHalt(2000);
  
  func_ValueCompare("<iL_HVMeas_UPack_State.bOutdated>", funcIDE_ReadVariable("UPack.State.bOutdated"), 0, equal);
  os1 = funcIDE_ReadVariable("@STM_CNT");
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 32767);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_VAL_B", 0);
  
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "bLinkComErr_B = (X_SHvDaq25_StatusCnt > 10) || Status_B;");  
  funcIDE_RunAndWait(2000);
  
  funcIDE_WriteVariable("Status_B", 1); // E2E error
  funcIDE_RunAndWait(2000);
  os2 = funcIDE_ReadVariable("@STM_CNT");
  
  check_equality(funcIDE_ReadVariable("UPack.State.bOutdated"), 1, "<iL_HVMeas_UPack_State.bOutdated> has reached the expected value", "<iL_HVMeas_UPack_State.bOutdated> has not reached the expected value");
  func_timeCompare("time measurement", "ms", (os2 - os1) / 64000, L_Mem_HVMeas_Timeout_Param_WITH_TOLERANCE, less_than);
  
  // Postconditions
  func_CleanUp();
}

testcase ENG8_11387()
{
  long os1 = 0, os2 = 0;
  long outdated = 0;
  
  testCaseTitle("ENG8_11387", "Event HvULinkOutdated when (HvULinkPosSigIsInvalid == 0) AND (HvULinkNegSigIsInvalid == 1) AND (HvULinkE2EInvalid == 1) AND (HvULinkRelayStateOpen == 1)");
  testCaseDescription("");

  // Preconditions
  func_Preconditions();

  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 100*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", 100*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", TRUE); // PLUGIN
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", TRUE); // PLUGIN 
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_VAL_B", 1);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_LinkRelayState_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_LinkRelayState_VAL_B", 1);
  trickMemoryAfter();
  
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "Rte_Write_PpCtApDaqMgrULink_ULink(&ULink);");
  funcIDE_WaitForHalt(2000);
  
  func_ValueCompare("<iL_HVMeas_ULink_State.bOutdated>", funcIDE_ReadVariable("ULink.State.bOutdated"), 0, equal);
  os1 = funcIDE_ReadVariable("@STM_CNT");
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 32767);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_LinkRelayState_VAL_B", 0);
  
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "bLinkComErr_B = (X_SHvDaq25_StatusCnt > 10) || Status_B;");  
  funcIDE_RunAndWait(2000);
  
  funcIDE_WriteVariable("Status_B", 1); // E2E error
  funcIDE_RunAndWait(2000);
  os2 = funcIDE_ReadVariable("@STM_CNT");
  
  check_equality(funcIDE_ReadVariable("ULink.State.bOutdated"), 1, "<iL_HVMeas_ULink_State.bOutdated> has reached the expected value", "<iL_HVMeas_ULink_State.bOutdated> has not reached the expected value");
  func_timeCompare("time measurement", "ms", (os2 - os1) / 64000, L_Mem_HVMeas_Timeout_Param_WITH_TOLERANCE, less_than);
  
  // Postconditions
  func_CleanUp();
}

testcase ENG8_11373()
{
  long os1 = 0, os2 = 0;
  long outdated = 0;
  
  testCaseTitle("ENG8_11373", "Event HvUPackOutdated when (HvUPackPosSigIsInvalid == 1) AND (HvUPackNegSigIsInvalid == 0) AND (HvUPackE2EInvalid == 0) AND (HvUPackRelayStateOpen == 0)");
  testCaseDescription("");

  // Preconditions
  func_Preconditions();

  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 100*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", TRUE); // PLUGIN
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", TRUE); // PLUGIN 
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_VAL_B", 1);
  trickMemoryAfter();
  
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "Rte_Write_PpCtApDaqMgrULink_ULink(&ULink);");
  funcIDE_WaitForHalt(2000);
  
  func_ValueCompare("<iL_HVMeas_UPack_State.bOutdated>", funcIDE_ReadVariable("UPack.State.bOutdated"), 0, equal);
  os1 = funcIDE_ReadVariable("@STM_CNT");
  
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 32767);
  testWaitForTimeout(2000);

  outdated = testWaitForVariableValue("UPack.State.bOutdated", 1, 10000);

  os2 = funcIDE_ReadVariable("@STM_CNT");
  check_equality(outdated, 1, "<iL_HVMeas_UPack_State.bOutdated> has reached the expected value", "<iL_HVMeas_UPack_State.bOutdated> has not reached the expected value");
  func_timeCompare("time measurement", "ms", (os2 - os1) / 64000, L_Mem_HVMeas_Timeout_Param_WITH_TOLERANCE, less_than);
  
  // Postconditions
  func_CleanUp();
}

testcase ENG8_11388()
{
  long os1 = 0, os2 = 0;
  long outdated = 0;
  
  testCaseTitle("ENG8_11388", "Event HvULinkOutdated when (HvULinkPosSigIsInvalid == 1) AND (HvULinkNegSigIsInvalid == 0) AND (HvULinkE2EInvalid == 0) AND (HvULinkRelayStateOpen == 0)");
  testCaseDescription("");

  // Preconditions
  func_Preconditions();

  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", 100*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 100*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", TRUE); // PLUGIN
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", TRUE); // PLUGIN 
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_VAL_B", 1);
  trickMemoryAfter();
  
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "Rte_Write_PpCtApDaqMgrULink_ULink(&ULink);");
  funcIDE_WaitForHalt(2000);
  
  func_ValueCompare("<iL_HVMeas_UPack_State.bOutdated>", funcIDE_ReadVariable("ULink.State.bOutdated"), 0, equal);
  os1 = funcIDE_ReadVariable("@STM_CNT");
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", 32767);
  testWaitForTimeout(2000);

  outdated = testWaitForVariableValue("ULink.State.bOutdated", 1, 10000);

  os2 = funcIDE_ReadVariable("@STM_CNT");
  check_equality(outdated, 1, "<iL_HVMeas_ULink_State.bOutdated> has reached the expected value", "<iL_HVMeas_ULink_State.bOutdated> has not reached the expected value");
  func_timeCompare("time measurement", "ms", (os2 - os1) / 64000, L_Mem_HVMeas_Timeout_Param_WITH_TOLERANCE, less_than);
  
  // Postconditions
  func_CleanUp();
}

testcase ENG8_11374()
{
  long os1 = 0, os2 = 0;
  long outdated = 0;
  
  testCaseTitle("ENG8_11374", "Event HvUPackOutdated when (HvUPackPosSigIsInvalid == 1) AND (HvUPackNegSigIsInvalid == 0) AND (HvUPackE2EInvalid == 0) AND (HvUPackRelayStateOpen == 1)");
  testCaseDescription("");

  // Preconditions
  func_Preconditions();

  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 100*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", TRUE); // PLUGIN
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", TRUE); // PLUGIN 
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_VAL_B", 1);
  trickMemoryAfter();
  
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "Rte_Write_PpCtApDaqMgrULink_ULink(&ULink);");
  funcIDE_WaitForHalt(2000);
  
  func_ValueCompare("<iL_HVMeas_UPack_State.bOutdated>", funcIDE_ReadVariable("UPack.State.bOutdated"), 0, equal);
  os1 = funcIDE_ReadVariable("@STM_CNT");
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_VAL_B", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 32767);
  testWaitForTimeout(2000);
  
  outdated = testWaitForVariableValue("UPack.State.bOutdated", 1, 10000);
  os2 = funcIDE_ReadVariable("@STM_CNT");
  check_equality(outdated, 1, "<iL_HVMeas_UPack_State.bOutdated> has reached the expected value", "<iL_HVMeas_UPack_State.bOutdated> has not reached the expected value");
  func_timeCompare("time measurement", "ms", (os2 - os1) / 64000, L_Mem_HVMeas_Timeout_Param_WITH_TOLERANCE, less_than);
  
  // Postconditions
  func_CleanUp();
}

testcase ENG8_11389()
{
  long os1 = 0, os2 = 0;
  long outdated = 0;
  
  testCaseTitle("ENG8_11389", "Event HvULinkOutdated when (HvULinkPosSigIsInvalid == 1) AND (HvULinkNegSigIsInvalid == 0) AND (HvULinkE2EInvalid == 0) AND (HvULinkRelayStateOpen == 1)");
  testCaseDescription("");

  // Preconditions
  func_Preconditions();

  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", 100*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 100*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", TRUE); // PLUGIN
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", TRUE); // PLUGIN 
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", FALSE);;
  funcIDE_WriteVariable("CtDaqMgr_Xcp_LinkRelayState_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_LinkRelayState_VAL_B", 1);
  trickMemoryAfter();
  
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "Rte_Write_PpCtApDaqMgrULink_ULink(&ULink);");
  funcIDE_WaitForHalt(2000);
  
  func_ValueCompare("<iL_HVMeas_ULink_State.bOutdated>", funcIDE_ReadVariable("ULink.State.bOutdated"), 0, equal);
  os1 = funcIDE_ReadVariable("@STM_CNT");
  funcIDE_WriteVariable("CtDaqMgr_Xcp_LinkRelayState_VAL_B", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", 32767);
  testWaitForTimeout(2000);

  outdated = testWaitForVariableValue("ULink.State.bOutdated", 1, 10000);
  os2 = funcIDE_ReadVariable("@STM_CNT");
  check_equality(outdated, 1, "<iL_HVMeas_ULink_State.bOutdated> has reached the expected value", "<iL_HVMeas_ULink_State.bOutdated> has not reached the expected value");
  func_timeCompare("time measurement", "ms", (os2 - os1) / 64000, L_Mem_HVMeas_Timeout_Param_WITH_TOLERANCE, less_than);
  
  // Postconditions
  func_CleanUp();
}

testcase ENG8_11375()
{
  long os1 = 0, os2 = 0;
  long outdated = 0;
  
  testCaseTitle("ENG8_11375", "Event HvUPackOutdated when (HvUPackPosSigIsInvalid == 1) AND (HvUPackNegSigIsInvalid == 0) AND (HvUPackE2EInvalid == 1) AND (HvUPackRelayStateOpen == 0)");
  testCaseDescription("");

  // Preconditions
  func_Preconditions();

  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 100*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", 100*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", TRUE); // PLUGIN
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", TRUE); // PLUGIN 
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_VAL_B", 1);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_LinkRelayState_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_LinkRelayState_VAL_B", 1);
  trickMemoryAfter();
  
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "Rte_Write_PpCtApDaqMgrULink_ULink(&ULink);");
  funcIDE_WaitForHalt(2000);
  
  func_ValueCompare("<iL_HVMeas_UPack_State.bOutdated>", funcIDE_ReadVariable("UPack.State.bOutdated"), 0, equal);
  os1 = funcIDE_ReadVariable("@STM_CNT");
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 32767);
  testWaitForTimeout(2000);
  
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "bLinkComErr_B = (X_SHvDaq25_StatusCnt > 10) || Status_B;");  
  funcIDE_RunAndWait(2000);
  
  funcIDE_WriteVariable("Status_B", 1); // E2E error
  funcIDE_RunAndWait(2000);
  os2 = funcIDE_ReadVariable("@STM_CNT");
  
  check_equality(funcIDE_ReadVariable("UPack.State.bOutdated"), 1, "<iL_HVMeas_UPack_State.bOutdated> has reached the expected value", "<iL_HVMeas_UPack_State.bOutdated> has not reached the expected value");
  func_timeCompare("time measurement", "ms", (os2 - os1) / 64000, L_Mem_HVMeas_Timeout_Param_WITH_TOLERANCE, less_than);
  
  // Postconditions
  func_CleanUp();
}

testcase ENG8_11390()
{
  long os1 = 0, os2 = 0;
  long outdated = 0;
  
  testCaseTitle("ENG8_11390", "Event HvULinkOutdated when (HvULinkPosSigIsInvalid == 1) AND (HvULinkNegSigIsInvalid == 0) AND (HvULinkE2EInvalid == 1) AND (HvULinkRelayStateOpen == 0)");
  testCaseDescription("");

  // Preconditions
  func_Preconditions();

  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 100*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", 100*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", TRUE); // PLUGIN
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", TRUE); // PLUGIN 
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_VAL_B", 1);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_LinkRelayState_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_LinkRelayState_VAL_B", 1);
  trickMemoryAfter();
  
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "Rte_Write_PpCtApDaqMgrULink_ULink(&ULink);");
  funcIDE_WaitForHalt(2000);
  
  func_ValueCompare("<iL_HVMeas_ULink_State.bOutdated>", funcIDE_ReadVariable("ULink.State.bOutdated"), 0, equal);
  os1 = funcIDE_ReadVariable("@STM_CNT");
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", 32767);
  testWaitForTimeout(2000);
  
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "bLinkComErr_B = (X_SHvDaq25_StatusCnt > 10) || Status_B;");  
  funcIDE_RunAndWait(2000);
  
  funcIDE_WriteVariable("Status_B", 1); // E2E error
  funcIDE_RunAndWait(2000);
  os2 = funcIDE_ReadVariable("@STM_CNT");
  
  check_equality(funcIDE_ReadVariable("ULink.State.bOutdated"), 1, "<iL_HVMeas_ULink_State.bOutdated> has reached the expected value", "<iL_HVMeas_ULink_State.bOutdated> has not reached the expected value");
  func_timeCompare("time measurement", "ms", (os2 - os1) / 64000, L_Mem_HVMeas_Timeout_Param_WITH_TOLERANCE, less_than);
  
  // Postconditions
  func_CleanUp();
}

testcase ENG8_11376()
{
  long os1 = 0, os2 = 0;
  long outdated = 0;
  
  testCaseTitle("ENG8_11376", "Event HvUPackOutdated when (HvUPackPosSigIsInvalid == 1) AND (HvUPackNegSigIsInvalid == 0) AND (HvUPackE2EInvalid == 1) AND (HvUPackRelayStateOpen == 1)");
  testCaseDescription("");

  // Preconditions
  func_Preconditions();

  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 100*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", 100*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", TRUE); // PLUGIN
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", TRUE); // PLUGIN 
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_VAL_B", 1);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_LinkRelayState_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_LinkRelayState_VAL_B", 1);
  trickMemoryAfter();
  
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "Rte_Write_PpCtApDaqMgrULink_ULink(&ULink);");
  funcIDE_WaitForHalt(2000);
  
  func_ValueCompare("<iL_HVMeas_UPack_State.bOutdated>", funcIDE_ReadVariable("UPack.State.bOutdated"), 0, equal);
  os1 = funcIDE_ReadVariable("@STM_CNT");
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 32767);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_VAL_B", 0);
  testWaitForTimeout(2000);
  
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "bLinkComErr_B = (X_SHvDaq25_StatusCnt > 10) || Status_B;");  
  funcIDE_RunAndWait(2000);
  
  funcIDE_WriteVariable("Status_B", 1); // E2E error
  funcIDE_RunAndWait(2000);
  os2 = funcIDE_ReadVariable("@STM_CNT");
  
  check_equality(funcIDE_ReadVariable("UPack.State.bOutdated"), 1, "<iL_HVMeas_UPack_State.bOutdated> has reached the expected value", "<iL_HVMeas_UPack_State.bOutdated> has not reached the expected value");
  func_timeCompare("time measurement", "ms", (os2 - os1) / 64000, L_Mem_HVMeas_Timeout_Param_WITH_TOLERANCE, less_than);
  
  // Postconditions
  func_CleanUp();
}

testcase ENG8_11391()
{
  long os1 = 0, os2 = 0;
  long outdated = 0;
  
  testCaseTitle("ENG8_11391", "Event HvULinkOutdated when (HvULinkPosSigIsInvalid == 1) AND (HvULinkNegSigIsInvalid == 0) AND (HvULinkE2EInvalid == 1) AND (HvULinkRelayStateOpen == 1)");
  testCaseDescription("");

  // Preconditions
  func_Preconditions();

  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 100*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", 100*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", TRUE); // PLUGIN
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", TRUE); // PLUGIN 
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_VAL_B", 1);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_LinkRelayState_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_LinkRelayState_VAL_B", 1);
  trickMemoryAfter();
  
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "Rte_Write_PpCtApDaqMgrULink_ULink(&ULink);");
  funcIDE_WaitForHalt(2000);
  
  func_ValueCompare("<iL_HVMeas_ULink_State.bOutdated>", funcIDE_ReadVariable("ULink.State.bOutdated"), 0, equal);
  os1 = funcIDE_ReadVariable("@STM_CNT");
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", 32767);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_LinkRelayState_VAL_B", 0);
  testWaitForTimeout(2000);
  
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "bLinkComErr_B = (X_SHvDaq25_StatusCnt > 10) || Status_B;");  
  funcIDE_RunAndWait(2000);
  
  funcIDE_WriteVariable("Status_B", 1); // E2E error
  
  outdated = testWaitForVariableValue("ULink.State.bOutdated", 1, 10000);
  os2 = funcIDE_ReadVariable("@STM_CNT");
  check_equality(outdated, 1, "<iL_HVMeas_ULink_State.bOutdated> has reached the expected value", "<iL_HVMeas_ULink_State.bOutdated> has not reached the expected value");
  func_timeCompare("time measurement", "ms", (os2 - os1) / 64000, L_Mem_HVMeas_Timeout_Param_WITH_TOLERANCE, less_than);
  
  // Postconditions
  func_CleanUp();
}

testcase ENG8_11377()
{
  long os1 = 0, os2 = 0;
  long outdated = 0;
  
  testCaseTitle("ENG8_11377", "Event HvUPackOutdated when (HvUPackPosSigIsInvalid == 1) AND (HvUPackNegSigIsInvalid == 1) AND (HvUPackE2EInvalid == 0) AND (HvUPackRelayStateOpen == 0)");
  testCaseDescription("");

  // Preconditions
  func_Preconditions();

  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 100*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", TRUE); // PLUGIN
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", TRUE); // PLUGIN 
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_VAL_B", 1);
  trickMemoryAfter();
  
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "Rte_Write_PpCtApDaqMgrULink_ULink(&ULink);");
  funcIDE_WaitForHalt(2000);
  
  func_ValueCompare("<iL_HVMeas_UPack_State.bOutdated>", funcIDE_ReadVariable("UPack.State.bOutdated"), 0, equal);
  os1 = funcIDE_ReadVariable("@STM_CNT");
  
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 32767);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 32767);
  testWaitForTimeout(2000);

  outdated = testWaitForVariableValue("UPack.State.bOutdated", 1, 10000);

  os2 = funcIDE_ReadVariable("@STM_CNT");
  check_equality(outdated, 1, "<iL_HVMeas_UPack_State.bOutdated> has reached the expected value", "<iL_HVMeas_UPack_State.bOutdated> has not reached the expected value");
  func_timeCompare("time measurement", "ms", (os2 - os1) / 64000, L_Mem_HVMeas_Timeout_Param_WITH_TOLERANCE, less_than);
  
  // Postconditions
  func_CleanUp();
}

testcase ENG8_11392()
{
  long os1 = 0, os2 = 0;
  long outdated = 0;
  
  testCaseTitle("ENG8_11392", "Event HvULinkOutdated when (HvULinkPosSigIsInvalid == 1) AND (HvULinkNegSigIsInvalid == 1) AND (HvULinkE2EInvalid == 0) AND (HvULinkRelayStateOpen == 0)");
  testCaseDescription("");

  // Preconditions
  func_Preconditions();

  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", 100*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 100*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", TRUE); // PLUGIN
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", TRUE); // PLUGIN 
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_VAL_B", 1);
  trickMemoryAfter();
  
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "Rte_Write_PpCtApDaqMgrULink_ULink(&ULink);");
  funcIDE_WaitForHalt(2000);
  
  func_ValueCompare("<iL_HVMeas_UPack_State.bOutdated>", funcIDE_ReadVariable("ULink.State.bOutdated"), 0, equal);
  os1 = funcIDE_ReadVariable("@STM_CNT");
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", 32767);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 32767);
  testWaitForTimeout(2000);

  outdated = testWaitForVariableValue("ULink.State.bOutdated", 1, 10000);

  os2 = funcIDE_ReadVariable("@STM_CNT");
  check_equality(outdated, 1, "<iL_HVMeas_ULink_State.bOutdated> has reached the expected value", "<iL_HVMeas_ULink_State.bOutdated> has not reached the expected value");
  func_timeCompare("time measurement", "ms", (os2 - os1) / 64000, L_Mem_HVMeas_Timeout_Param_WITH_TOLERANCE, less_than);
  
  // Postconditions
  func_CleanUp();
}

testcase ENG8_11378()
{
  long os1 = 0, os2 = 0;
  long outdated = 0;
  
  testCaseTitle("ENG8_11378", "Event HvUPackOutdated when (HvUPackPosSigIsInvalid == 1) AND (HvUPackNegSigIsInvalid == 1) AND (HvUPackE2EInvalid == 0) AND (HvUPackRelayStateOpen == 1)");
  testCaseDescription("");

  // Preconditions
  func_Preconditions();

  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 100*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", TRUE); // PLUGIN
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", TRUE); // PLUGIN 
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_VAL_B", 1);
  trickMemoryAfter();
  
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "Rte_Write_PpCtApDaqMgrULink_ULink(&ULink);");
  funcIDE_WaitForHalt(2000);
  
  func_ValueCompare("<iL_HVMeas_UPack_State.bOutdated>", funcIDE_ReadVariable("UPack.State.bOutdated"), 0, equal);
  os1 = funcIDE_ReadVariable("@STM_CNT");
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_VAL_B", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 32767);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 32767);
  testWaitForTimeout(2000);
  
  outdated = testWaitForVariableValue("UPack.State.bOutdated", 1, 10000);
  os2 = funcIDE_ReadVariable("@STM_CNT");
  check_equality(outdated, 1, "<iL_HVMeas_UPack_State.bOutdated> has reached the expected value", "<iL_HVMeas_UPack_State.bOutdated> has not reached the expected value");
  func_timeCompare("time measurement", "ms", (os2 - os1) / 64000, L_Mem_HVMeas_Timeout_Param_WITH_TOLERANCE, less_than);
  
  // Postconditions
  func_CleanUp();
}

testcase ENG8_11393()
{
  long os1 = 0, os2 = 0;
  long outdated = 0;
  
  testCaseTitle("ENG8_11393", "Event HvULinkOutdated when (HvULinkPosSigIsInvalid == 1) AND (HvULinkNegSigIsInvalid == 1) AND (HvULinkE2EInvalid == 0) AND (HvULinkRelayStateOpen == 1)");
  testCaseDescription("");

  // Preconditions
  func_Preconditions();

  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", 100*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 100*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", TRUE); // PLUGIN
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", TRUE); // PLUGIN 
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_LinkRelayState_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_LinkRelayState_VAL_B", 1);
  trickMemoryAfter();
  
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "Rte_Write_PpCtApDaqMgrULink_ULink(&ULink);");
  funcIDE_WaitForHalt(2000);
  
  func_ValueCompare("<iL_HVMeas_ULink_State.bOutdated>", funcIDE_ReadVariable("ULink.State.bOutdated"), 0, equal);
  os1 = funcIDE_ReadVariable("@STM_CNT");
  funcIDE_WriteVariable("CtDaqMgr_Xcp_LinkRelayState_VAL_B", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", 32767);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 32767);
  testWaitForTimeout(2000);

  outdated = testWaitForVariableValue("ULink.State.bOutdated", 1, 10000);
  os2 = funcIDE_ReadVariable("@STM_CNT");
  check_equality(outdated, 1, "<iL_HVMeas_ULink_State.bOutdated> has reached the expected value", "<iL_HVMeas_ULink_State.bOutdated> has not reached the expected value");
  func_timeCompare("time measurement", "ms", (os2 - os1) / 64000, L_Mem_HVMeas_Timeout_Param_WITH_TOLERANCE, less_than);
  
  // Postconditions
  func_CleanUp();
}

testcase ENG8_11379()
{
  long os1 = 0, os2 = 0;
  long outdated = 0;
  
  testCaseTitle("ENG8_11379", "Event HvUPackOutdated when (HvUPackPosSigIsInvalid == 1) AND (HvUPackNegSigIsInvalid == 1) AND (HvUPackE2EInvalid == 1) AND (HvUPackRelayStateOpen == 0)");
  testCaseDescription("");

  // Preconditions
  func_Preconditions();

  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 100*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", 100*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", TRUE); // PLUGIN
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", TRUE); // PLUGIN 
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_VAL_B", 1);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_LinkRelayState_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_LinkRelayState_VAL_B", 1);
  trickMemoryAfter();
  
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "Rte_Write_PpCtApDaqMgrULink_ULink(&ULink);");
  funcIDE_WaitForHalt(2000);
  
  func_ValueCompare("<iL_HVMeas_UPack_State.bOutdated>", funcIDE_ReadVariable("UPack.State.bOutdated"), 0, equal);
  os1 = funcIDE_ReadVariable("@STM_CNT");
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 32767);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 32767);
  testWaitForTimeout(2000);
  
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "bLinkComErr_B = (X_SHvDaq25_StatusCnt > 10) || Status_B;");  
  funcIDE_RunAndWait(2000);
  
  funcIDE_WriteVariable("Status_B", 1); // E2E error
  funcIDE_RunAndWait(2000);
  os2 = funcIDE_ReadVariable("@STM_CNT");
  
  check_equality(funcIDE_ReadVariable("UPack.State.bOutdated"), 1, "<iL_HVMeas_UPack_State.bOutdated> has reached the expected value", "<iL_HVMeas_UPack_State.bOutdated> has not reached the expected value");
  func_timeCompare("time measurement", "ms", (os2 - os1) / 64000, L_Mem_HVMeas_Timeout_Param_WITH_TOLERANCE, less_than);
  
  // Postconditions
  func_CleanUp();
}

testcase ENG8_11394()
{
  long os1 = 0, os2 = 0;
  long outdated = 0;
  
  testCaseTitle("ENG8_11394", "Event HvULinkOutdated when (HvULinkPosSigIsInvalid == 1) AND (HvULinkNegSigIsInvalid == 1) AND (HvULinkE2EInvalid == 1) AND (HvULinkRelayStateOpen == 0)");
  testCaseDescription("");

  // Preconditions
  func_Preconditions();

  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 100*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", 100*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", TRUE); // PLUGIN
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", TRUE); // PLUGIN 
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_VAL_B", 1);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_LinkRelayState_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_LinkRelayState_VAL_B", 1);
  trickMemoryAfter();
  
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "Rte_Write_PpCtApDaqMgrULink_ULink(&ULink);");
  funcIDE_WaitForHalt(2000);
  
  func_ValueCompare("<iL_HVMeas_ULink_State.bOutdated>", funcIDE_ReadVariable("ULink.State.bOutdated"), 0, equal);
  os1 = funcIDE_ReadVariable("@STM_CNT");
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", 32767);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 32767);
  testWaitForTimeout(2000);
  
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "bLinkComErr_B = (X_SHvDaq25_StatusCnt > 10) || Status_B;");  
  funcIDE_RunAndWait(2000);
  
  funcIDE_WriteVariable("Status_B", 1); // E2E error
  funcIDE_RunAndWait(2000);
  os2 = funcIDE_ReadVariable("@STM_CNT");
  
  check_equality(funcIDE_ReadVariable("ULink.State.bOutdated"), 1, "<iL_HVMeas_ULink_State.bOutdated> has reached the expected value", "<iL_HVMeas_ULink_State.bOutdated> has not reached the expected value");
  func_timeCompare("time measurement", "ms", (os2 - os1) / 64000, L_Mem_HVMeas_Timeout_Param_WITH_TOLERANCE, less_than);
  
  // Postconditions
  func_CleanUp();
}

testcase ENG8_11380()
{
  long os1 = 0, os2 = 0;
  long outdated = 0;
  
  testCaseTitle("ENG8_11380", "Event HvUPackOutdated when (HvUPackPosSigIsInvalid == 1) AND (HvUPackNegSigIsInvalid == 1) AND (HvUPackE2EInvalid == 1) AND (HvUPackRelayStateOpen == 1)");
  testCaseDescription("");

  // Preconditions
  func_Preconditions();

  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 100*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", 100*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", TRUE); // PLUGIN
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", TRUE); // PLUGIN 
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_VAL_B", 1);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_LinkRelayState_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_LinkRelayState_VAL_B", 1);
  trickMemoryAfter();
  
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "Rte_Write_PpCtApDaqMgrULink_ULink(&ULink);");
  funcIDE_WaitForHalt(2000);
  
  func_ValueCompare("<iL_HVMeas_UPack_State.bOutdated>", funcIDE_ReadVariable("UPack.State.bOutdated"), 0, equal);
  os1 = funcIDE_ReadVariable("@STM_CNT");
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 32767);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 32767);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_VAL_B", 0);
  testWaitForTimeout(2000);
  
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "bLinkComErr_B = (X_SHvDaq25_StatusCnt > 10) || Status_B;");  
  funcIDE_RunAndWait(2000);
  
  funcIDE_WriteVariable("Status_B", 1); // E2E error
  funcIDE_RunAndWait(2000);
  os2 = funcIDE_ReadVariable("@STM_CNT");
  
  check_equality(funcIDE_ReadVariable("UPack.State.bOutdated"), 1, "<iL_HVMeas_UPack_State.bOutdated> has reached the expected value", "<iL_HVMeas_UPack_State.bOutdated> has not reached the expected value");
  func_timeCompare("time measurement", "ms", (os2 - os1) / 64000, L_Mem_HVMeas_Timeout_Param_WITH_TOLERANCE, less_than);
  
  // Postconditions
  func_CleanUp();
}

testcase ENG8_11395()
{
  long os1 = 0, os2 = 0;
  long outdated = 0;
  
  testCaseTitle("ENG8_11395", "Event HvULinkOutdated when (HvULinkPosSigIsInvalid == 1) AND (HvULinkNegSigIsInvalid == 1) AND (HvULinkE2EInvalid == 1) AND (HvULinkRelayStateOpen == 1)");
  testCaseDescription("");

  // Preconditions
  func_Preconditions();

  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 100*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", 100*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", TRUE); // PLUGIN
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", TRUE); // PLUGIN 
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_VAL_B", 1);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_LinkRelayState_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_LinkRelayState_VAL_B", 1);
  trickMemoryAfter();
  
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "Rte_Write_PpCtApDaqMgrULink_ULink(&ULink);");
  funcIDE_WaitForHalt(2000);
  
  func_ValueCompare("<iL_HVMeas_ULink_State.bOutdated>", funcIDE_ReadVariable("ULink.State.bOutdated"), 0, equal);
  os1 = funcIDE_ReadVariable("@STM_CNT");
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkPos_S16", 32767);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkNeg_S16", 32767);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_LinkRelayState_VAL_B", 0);
  testWaitForTimeout(2000);
  
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "bLinkComErr_B = (X_SHvDaq25_StatusCnt > 10) || Status_B;");  
  funcIDE_RunAndWait(2000);
  
  funcIDE_WriteVariable("Status_B", 1); // E2E error
  funcIDE_RunAndWait(2000);
  os2 = funcIDE_ReadVariable("@STM_CNT");
  
  check_equality(funcIDE_ReadVariable("ULink.State.bOutdated"), 1, "<iL_HVMeas_ULink_State.bOutdated> has reached the expected value", "<iL_HVMeas_ULink_State.bOutdated> has not reached the expected value");
  func_timeCompare("time measurement", "ms", (os2 - os1) / 64000, L_Mem_HVMeas_Timeout_Param_WITH_TOLERANCE, less_than);
  
  // Postconditions
  func_CleanUp();
}

testcase ENG8_11096()
{
  long os1 = 0, os2 = 0;
  long outdated = 0, invalid = 0;
  
  testCaseTitle("ENG8_11096", "Event HvUPackSigStateIsInvalid occurs when HvUPackOutdated holds under HvSymContPosition during [0, <L_Mem_HVMeas_Timeout_Param>] ms.");
  testCaseDescription("");

  // Preconditions
  func_Preconditions();

  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 100*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", TRUE); // PLUGIN
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", TRUE); // PLUGIN 
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_VAL_B", 1);
  trickMemoryAfter();
  
  func_ValueCompare("<L_ContCtrl_ContMainNeg_State>", getsignal(CAN2::SC_STATES_01::STAT_CONTACTOR_MINUS), CNT_OPEN, equal);
  func_ValueCompare("<L_ContCtrl_ContMainPos_State>", getsignal(CAN2::SC_STATES_01::STAT_CONTACTOR_PLUS), CNT_OPEN, equal);
  func_ValueCompare("<L_ContCtrl_ContPreCharge_State>", getsignal(CAN2::SC_STATES_01::STAT_CONTACTOR_PRECHARGE), CNT_OPEN, equal);
  
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "Rte_Write_PpCtApDaqMgrULink_ULink(&ULink);");
  funcIDE_WaitForHalt(2000);
  
  func_ValueCompare("<iL_HVMeas_UPack_State.bOutdated>", funcIDE_ReadVariable("UPack.State.bOutdated"), 0, equal);
  func_ValueCompare("<iL_HVMeas_UPack_State.bInvalid>", funcIDE_ReadVariable("UPack.State.bInvalid"), 0, equal);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_VAL_B", 0);

  outdated = testWaitForVariableValue("UPack.State.bOutdated", 1, 20000);

  os1 = funcIDE_ReadVariable("@STM_CNT");
  check_equality(outdated, 1, "<iL_HVMeas_UPack_State.bOutdated> has reached the expected value", "<iL_HVMeas_UPack_State.bOutdated> has not reached the expected value");
  func_ValueCompare("<iL_HVMeas_UPack_State.bInvalid>", funcIDE_ReadVariable("UPack.State.bInvalid"), 0, equal);
  
  invalid = testWaitForVariableValue("UPack.State.bInvalid", 1, 20000);
  check_equality(invalid, 1, "<iL_HVMeas_UPack_State.bInvalid> has reached the expected value", "<iL_HVMeas_UPack_State.bInvalid> has not reached the expected value");
  os2 = funcIDE_ReadVariable("@STM_CNT");
  func_ValueCompare("<iL_HVMeas_UPack_State.bOutdated>", funcIDE_ReadVariable("UPack.State.bOutdated"), 0, equal);

  func_timeCompare("time measurement", "ms", (os2 - os1) / 64000, L_Mem_HVMeas_Timeout_Param_WITH_TOLERANCE, equal_or_less_than);
  
  // Postconditions
  func_CleanUp();
}

testcase ENG8_11097()
{
  long os1 = 0, os2 = 0;
  long outdated = 0, invalid = 0;
  
  testCaseTitle("ENG8_11097", "Event HvULinkSigStateIsInvalid occurs when HvULinkOutdated holds under HvSymContPosition during [0, <L_Mem_HVMeas_Timeout_Param>] ms.");
  testCaseDescription("");

  // Preconditions
  func_Preconditions();

  trickMemoryBefore();
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UHigh_SW_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackNeg_S16", 0);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackPos_S16", 100*40);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", TRUE); // PLUGIN
  funcIDE_WriteVariable("CtDaqMgr_Xcp_UPackE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_ACT_U8", 170);
  //funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", TRUE); // PLUGIN 
  funcIDE_WriteVariable("CtDaqMgr_Xcp_ULinkE2eOk_VAL_B", FALSE);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_PackRelayState_VAL_B", 1);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_LinkRelayState_ACT_U8", 170);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_LinkRelayState_VAL_B", 1);
  trickMemoryAfter();
  
  func_ValueCompare("<L_ContCtrl_ContMainNeg_State>", getsignal(CAN2::SC_STATES_01::STAT_CONTACTOR_MINUS), CNT_OPEN, equal);
  func_ValueCompare("<L_ContCtrl_ContMainPos_State>", getsignal(CAN2::SC_STATES_01::STAT_CONTACTOR_PLUS), CNT_OPEN, equal);
  func_ValueCompare("<L_ContCtrl_ContPreCharge_State>", getsignal(CAN2::SC_STATES_01::STAT_CONTACTOR_PRECHARGE), CNT_OPEN, equal);
  
  funcWinIdea_SetBPSource("CtDaqMgr_HighVoltages", "Rte_Write_PpCtApDaqMgrULink_ULink(&ULink);");
  funcIDE_WaitForHalt(2000);
  
  func_ValueCompare("<iL_HVMeas_ULink_State.bOutdated>", funcIDE_ReadVariable("ULink.State.bOutdated"), 0, equal);
  func_ValueCompare("<iL_HVMeas_ULink_State.bInvalid>", funcIDE_ReadVariable("ULink.State.bInvalid"), 0, equal);
  funcIDE_WriteVariable("CtDaqMgr_Xcp_LinkRelayState_VAL_B", 0);

  outdated = testWaitForVariableValue("ULink.State.bOutdated", 1, 20000);

  os1 = funcIDE_ReadVariable("@STM_CNT");
  check_equality(outdated, 1, "<iL_HVMeas_ULink_State.bOutdated> has reached the expected value", "<iL_HVMeas_ULink_State.bOutdated> has not reached the expected value");
  func_ValueCompare("<iL_HVMeas_ULink_State.bInvalid>", funcIDE_ReadVariable("ULink.State.bInvalid"), 0, equal);
  
  invalid = testWaitForVariableValue("ULink.State.bInvalid", 1, 20000);
  check_equality(invalid, 1, "<iL_HVMeas_ULink_State.bInvalid> has reached the expected value", "<iL_HVMeas_ULink_State.bInvalid> has not reached the expected value");
  os2 = funcIDE_ReadVariable("@STM_CNT");
  func_ValueCompare("<iL_HVMeas_ULink_State.bOutdated>", funcIDE_ReadVariable("ULink.State.bOutdated"), 0, equal);

  func_timeCompare("time measurement", "ms", (os2 - os1) / 64000, L_Mem_HVMeas_Timeout_Param_WITH_TOLERANCE, equal_or_less_than);
  
  // Postconditions
  func_CleanUp();
}
