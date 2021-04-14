/* Created on: 10/03/2021
 *     Author: Ziniu Lu (luziniuoskar@outlook.com)
 */

#pragma once
#include<string>

# define PROJECT_NAMESPACE Scan2knit
# define BEGIN_PROJECT_NAMESPACE namespace PROJECT_NAMESPACE {
# define END_PROJECT_NAMESPACE }
//using namespace std;

BEGIN_PROJECT_NAMESPACE

// basic log utility
enum severity_level {trace, debug, info, warning, error, fatal};
extern const std::string severity_names[6];

/*
# define LOG( level, message )                                              \
{                                                                           \
  if( severity_level::level > severity_level::warning )                     \
  {                                                                         \
    std::cout<<"["<<__DATE__<< " " << __TIME__ <<"]"                        \
             << severity_names[severity_level::level]<< " "                 \
             << message                                                     \
             << " (in "<< __FILE__<<":"<< __LINE__ <<")"                    \
             << std::endl;                                                  \
  }                                                                         \
  else                                                                      \
  {                                                                         \
    std::cout<<"["<<__DATE__<< " " << __TIME__ <<"]"                        \
             << severity_names[severity_level::level]<< " "                 \
             << message << std::endl;                                       \
  }                                                                         \
}
*/

END_PROJECT_NAMESPACE