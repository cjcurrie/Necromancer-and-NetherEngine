#ifndef NEInc_SettingsManager_h
  #define NEInc_SettingsManager_h

  #include <map>

  #include "Singleton.h"
  #include "ManMemPointer.h"
  #include "BaseDator.h"



  namespace NE
  {
    class SettingsManager : public Singleton<SettingsManager>
    {
      
      public:
        enum SettingFlag {ADD, REMOVE, SET};    // In the settings config file, ADD = '+', REMOVE = '-', SET = '='
      
        SettingsManager();
        virtual ~SettingsManager();
        
        void RegisterVariable(std::string &name, ManMemPointer<BaseDator> &var);
      
        void SetVariable(std::string &name, std::string &value, SettingFlag settingFlag = SET);
        void CreateStandardSettings();
        void DestroyStandardSettings();
        
        void ParseSetting(std::string str);
      
        void ParseFile(std::string filename);
        
      protected:
        std::map< std::string, ManMemPointer<BaseDator> > settingMap;
    };
  }

#endif