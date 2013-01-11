#include "SettingsManager.h"

#include <iostream>
#include <fstream>


NE::SettingsManager::SettingsManager()
{
  settingMap.clear();
  CreateStandardSettings();
}

NE::SettingsManager::~SettingsManager()
{
  DestroyStandardSettings();
}

void NE::SettingsManager::RegisterVariable(std::string &name, ManMemPointer<BaseDator> &var)
{
  settingMap[name]=var;
}

void NE::SettingsManager::ParseFile(std::string filename)
{
  std::ifstream in(filename.c_str());
  
  if(!in.is_open())
     return; //couldn't open
  
  while(!in.eof())
  {
    char szBuf[1024];
    in.getline(szBuf,1024);
    ParseSetting(szBuf);
  }
}


// Each macro here takes the type of dator, the ManMemPointer<T> to store it in,
//  the variable it's bound to, and the name the manager should use to refer to it.
#define SETTING(type, target, var, name) target = new Dator<type>(var); \
  RegisterVariable(std::string(name),CMMPointer<BaseDator>(target));

#define LIST(type, target, var, name) target = new ListDator<type>(var); \
  RegisterVariable(std::string(name),CMMPointer<BaseDator>(target));

void NE::SettingsManager::CreateStandardSettings()
{
  //empty for the time being
}

void NE::SettingsManager::DestroyStandardSettings()
{
  //also empty
  
}


void NE::SettingsManager::SetVariable(std::string &name, std::string &value, SettingFlag settingFlag)
{
  if(!settingMap[name])
    return; //setting doesn't exist
  
  if(settingMap[name]->hasMultipleValues())   // For lists
  {
    std::list<std::string> valueList;
    valueList.clear();    // Initializes
    
    // Check for semicolon-seperated values
    if(value.find(';') != -1)
    {
      // Split the string into semicolor-seperated chunks
      
      unsigned long int first = 0, last;
      
      while( (last = value.find(';',first)) != -1 )
      {
        valueList.push_back(value.substr(first, last - first));
        first = last + 1;
      }
      
      valueList.push_back(value.substr(first));
    }
    
    else
    {
      valueList.push_back(value);
    }
    
    
    for(std::list <std::string>::iterator it = valueList.begin(); it != valueList.end(); it++)
    {
      if(settingFlag == ADD)
      {
        (*settingMap[name]) += (*it);
      }
      
      else if(settingFlag == REMOVE)
      {
        (*settingMap[name]) -= (*it);
      }
      
      else
      {
        (*settingMap[name]) = (*it);
      }
    }
  }
  
  else  // For unitary objects
  {
    //just assign the value
    (*settingMap[name]) = value;
  }
}

void NE::SettingsManager::ParseSetting(std::string str)
{
  SettingFlag settingFlag = SET;
  std::string name, value;
  
  // Test for ADD
  if( str[0] == '+' )
  {
    settingFlag = ADD;
    str = str.substr(1); //remove the first character from the string
  }
  
  // Test for REMOVE
  else if( str[0] == '-' )
  {
    settingFlag = REMOVE;
    str = str.substr(1); //remove the first character from the string
  }
  
  
  // Test for SET
  unsigned long eqPos = str.find('=');
  
  if(eqPos !=- 1)
  {
    // There's an = sign somewhere in there, so split either side of it.
    name = str.substr(0, eqPos);
    value = str.substr(eqPos + 1);
  }
  
  else
  {
    // No equal sign found, so the setting must be a boolean value rather than a number.
    if (settingFlag == ADD)
      value = "t";    // true
    else if (settingFlag == REMOVE)
      value = "f";    // false
  }
  
  //set the variable
  SetVariable(name, value, settingFlag);
}