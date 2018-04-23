/*
 * A structure to store the data needed to communicate with EnergyPlus.
 */

#ifndef Buildings_FMUEnergyPlusStructure_h /* Not needed since it is only a typedef; added for safety */
#define Buildings_FMUEnergyPlusStructure_h

#include <stddef.h>  /* stddef defines size_t */
#include<fmi2FunctionTypes.h>
//#include <EPFMI.hpp>

// Use windows.h only for Windows
#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#define PATH_SEP "\\"
#define WINDOWS 1
#define HANDLE HINSTANCE
#else
#include <sys/stat.h>
#include <unistd.h>
#include <libgen.h>
#define PATH_SEP "/"
#define WINDOWS 0
#define HANDLE void *
/* See http://www.yolinux.com/TUTORIALS/LibraryArchives-StaticAndDynamic.html */
#include <sys/stat.h> // for creating dirs on Linux
#include <sys/types.h>
#include<sys/sysinfo.h>
#include <dlfcn.h>
#endif

typedef unsigned int (*fInstantiate)(fmi2String input,
                         fmi2String weather,
                         fmi2String idd,
                         fmi2String instanceName,
                         fmi2String* parameterNames,
                         const unsigned int parameterValueReferences[],
                         size_t nPar,
                         fmi2String* inputNames,
                         const fmi2ValueReference inputValueReferences[],
                         size_t nInp,
                         fmi2String* outputNames,
                         const fmi2ValueReference outputValueReferences[],
                         size_t nOut,
                         fmi2String log);

typedef unsigned int (*fSetupExperiment)(fmi2Real tStart,
                             int stopTimeDefined,
                             fmi2String log);

typedef unsigned int (*fSetTime)(fmi2Real time,
                     fmi2String log);

typedef unsigned int (*fSetVariables)(const fmi2ValueReference valueReferences[],
                          const fmi2Real variablePointers[],
                          size_t nVars1,
                          fmi2String log);

typedef unsigned int (*fGetVariables)(const unsigned int valueReferences[],
                          fmi2Real variablePointers[],
                          size_t nVars2,
                          fmi2String log);

typedef unsigned int (*fGetNextEventTime)(fmi2EventInfo *eventInfo,
                              fmi2String log);

typedef unsigned int (*fTerminate)(fmi2String log);


typedef struct FMU{
	HANDLE dllHandle;
	fInstantiate instantiate;
	fSetupExperiment setupExperiment;
	fSetTime setTime;
	fSetVariables setVariables;
	fGetVariables getVariables;
	fGetNextEventTime getNextEventTime;
	fTerminate terminate;
  fmi2EventInfo eventInfo;
} FMU;

typedef struct FMUBuilding
{
  int _firstCall;
  fmi2Byte* name;
  fmi2Byte* weather;
  fmi2Byte* idd;
  fmi2Byte* instanceName;
  fmi2Byte* epLibName;
  fmi2Byte cwd[1024];
  fmi2Byte* outputs;
  fmi2Integer nZon; /* Number of zones that use this FMU */
  fmi2Byte** zoneNames; /* Names of zones in this FMU */
  void** zones; /* Pointers to all zones*/
  FMU* fmu;
} FMUBuilding;

typedef struct FMUZone
{
  int index;
  fmi2Byte* name;      /* Name of this zone */
  FMUBuilding* ptrBui; /* Pointer to building with this zone */
  fmi2ValueReference* valueReference; /* Value references for this zone */
  size_t nValueReference;
  size_t nInputValueReferences;/* Number of input value references*/
  fmi2Byte inputVariableNames[5][100]; /* Names of input variables*/
  fmi2ValueReference inputValueReferences[5]; /* Value reference of input variables*/
  size_t nOutputValueReferences;/* Number of output value references*/
  fmi2Byte outputVariableNames[7][100]; /* Names of output variables*/
  fmi2ValueReference outputValueReferences[7]; /* Value references of output variables*/
} FMUZone;

static struct FMUBuilding** Buildings_FMUS; /* Array with pointers to all FMUs */
static unsigned int Buildings_nFMU = 0;     /* Number of FMUs */
#endif
