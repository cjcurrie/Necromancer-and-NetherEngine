This is the "working directory" for all MacOSX builds.
This will later be the working directory for all builds.

The file "logMessageStringTable" is a list of error messages used by the Log.cpp class to cache common error messages. Each line is a single error, corresponding to Log::logStrings[lineNumber]. The length of logStrings, is set by Log::MAX_LOG_Strings = 256. The max length (in characters) of a single message line is set by Log::MAX_LINE_LENGTH = 512.