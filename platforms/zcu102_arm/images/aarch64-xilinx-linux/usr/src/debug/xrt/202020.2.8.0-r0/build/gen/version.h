#ifndef _XRT_VERSION_H_
#define _XRT_VERSION_H_

static const char xrt_build_version[] = "2.8.0";

static const char xrt_build_version_branch[] = "2020.2";

static const char xrt_build_version_hash[] = "f19a872233fbfe2eb933f25fa3d9a780ced774e5";

static const char xrt_build_version_hash_date[] = "Tue, 10 Nov 2020 21:28:23 -0800";

static const char xrt_build_version_date_rfc[] = "Tue, 30 Mar 2021 19:53:41 +0000";

static const char xrt_build_version_date[] = "2021-03-30 19:53:41";

static const char xrt_modified_files[] = "";

#define XRT_DRIVER_VERSION "2.8.0,f19a872233fbfe2eb933f25fa3d9a780ced774e5"

# ifdef __cplusplus
#include <iostream>
#include <string>

namespace xrt { 

class version {
 public:
  static void print(std::ostream & output)
  {
     output << "       XRT Build Version: " << xrt_build_version << std::endl;
     output << "    Build Version Branch: " << xrt_build_version_branch << std::endl;
     output << "      Build Version Hash: " << xrt_build_version_hash << std::endl;
     output << " Build Version Hash Date: " << xrt_build_version_hash_date << std::endl;
     output << "      Build Version Date: " << xrt_build_version_date_rfc << std::endl;
  
     std::string modifiedFiles(xrt_modified_files);
     if ( !modifiedFiles.empty() ) {
        const std::string& delimiters = ",";      // Our delimiter
        std::string::size_type lastPos = 0;
        int runningIndex = 1;
        while(lastPos < modifiedFiles.length() + 1) {
          if (runningIndex == 1) {
             output << "  Current Modified Files: ";
          } else {
             output << "                          ";
          }
          output << runningIndex++ << ") ";
  
          std::string::size_type pos = modifiedFiles.find_first_of(delimiters, lastPos);
  
          if (pos == std::string::npos) {
            pos = modifiedFiles.length();
          }
  
          output << modifiedFiles.substr(lastPos, pos-lastPos) << std::endl;
  
          lastPos = pos + 1;
        }
     }
  }
};
}
#endif

#endif 

