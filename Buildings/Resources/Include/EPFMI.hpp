/* #include <stddef.h> */
/* #include <fmi2FunctionTypes.h> */
/*extern "C" {*/

unsigned int instantiate(const char *input,
                         const char *weather,
                         const char *idd,
                         const char *instanceName,
                         const char ** parameterNames,
                         const unsigned int parameterValueReferences[],
                         size_t nPar,
                         const char ** inputNames,
                         const unsigned int inputValueReferences[],
                         size_t nInp,
                         const char ** outputNames,
                         const unsigned int outputValueReferences[],
                         size_t nOut,
                         const char *log);

unsigned int setupExperiment(double tStart,
                             int stopTimeDefined,
                             const char *log);

unsigned int setTime(double time,
                     const char *log);

unsigned int setVariables(const unsigned int valueReferences[],
                          const double variablePointers[],
                          size_t nVars1,
                          const char *log);

unsigned int getVariables(const unsigned int valueReferences[],
                          double variablePointers[],
                          size_t nVars2,
                          const char *log);

unsigned int getNextEventTime(fmi2EventInfo *eventInfo,
                              const char *log);
/* fixme: Add EP to the name of the functiions. terminate exists in
Modelica standard library *
unsigned int terminateEP(const char *log);

/* } */
