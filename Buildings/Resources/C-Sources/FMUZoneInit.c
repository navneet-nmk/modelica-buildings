/*
 * Modelica external function to communicate with EnergyPlus.
 *
 * Michael Wetter, LBNL                  2/14/2018
 */

#include "FMUEnergyPlusStructure.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Create the structure and return a pointer to its address. */
FMUBuilding* instantiateEnergyPlusFMU(const char* idfName, const char* instanceName,
  const char* weaName, const char* iddName, const char* zoneName,
  const char* epLibName, FMUZone* zone )
{
  struct stat stat_p;
  char *tmp_str;
  char *bname;
  int result;
  /* Allocate memory */
  if (Buildings_nFMU == 0){
    Buildings_FMUS = malloc(sizeof(struct FMUBuilding*));
    if ( Buildings_FMUS == NULL )
      ModelicaError("Not enough memory in FMUZoneInit.c. to allocate array for Buildings_FMU.");
  }
  else{
    Buildings_FMUS = realloc(Buildings_FMUS, (Buildings_nFMU+1) * sizeof(struct FMUBuilding*));
  }

  Buildings_FMUS[Buildings_nFMU] = malloc(sizeof(FMUBuilding));
  if ( Buildings_FMUS[Buildings_nFMU] == NULL )
    ModelicaError("Not enough memory in FMUZoneInit.c. to allocate array for Buildings_FMU[0].");
  Buildings_FMUS[Buildings_nFMU]->zoneNames = malloc(sizeof(char*));
  if ( Buildings_FMUS[Buildings_nFMU]->zoneNames == NULL )
    ModelicaError("Not enough memory in FMUZoneInit.c. to allocate array for Buildings_FMUS[0]->zoneNames.");

  /* Assign the fmu name */
  Buildings_FMUS[Buildings_nFMU]->name = (char*) malloc((strlen(idfName)+1) * sizeof(char));
  if ( Buildings_FMUS[Buildings_nFMU]->name == NULL )
    ModelicaError("Not enough memory in FMUZoneInit.c. to allocate fmu name.");
  strcpy(Buildings_FMUS[Buildings_nFMU]->name, idfName);

  /* Assign the weather name */
  Buildings_FMUS[Buildings_nFMU]->weather = (char*) malloc((strlen(weaName)+1) * sizeof(char));
  if ( Buildings_FMUS[Buildings_nFMU]->weather == NULL )
    ModelicaError("Not enough memory in FMUZoneInit.c. to allocate weather name.");
  strcpy(Buildings_FMUS[Buildings_nFMU]->weather, weaName);

  /* Assign the idd name */
  Buildings_FMUS[Buildings_nFMU]->idd = (char*) malloc((strlen(iddName)+1) * sizeof(char));
  if ( Buildings_FMUS[Buildings_nFMU]->idd == NULL )
    ModelicaError("Not enough memory in FMUZoneInit.c. to allocate IDD name.");
  strcpy(Buildings_FMUS[Buildings_nFMU]->idd, iddName);

  // /* Assign the Energyplus instance name */
  // Buildings_FMUS[Buildings_nFMU]->instanceName = (char*) malloc((strlen(instanceName)+1) * sizeof(char));
  // if ( Buildings_FMUS[Buildings_nFMU]->instanceName == NULL )
  //   ModelicaError("Not enough memory in FMUZoneInit.c. to allocate instance name.");
  // strcpy(Buildings_FMUS[Buildings_nFMU]->instanceName, instanceName);

  /* Assign the Energyplus library name */
  /*Buildings_FMUS[Buildings_nFMU]->epLibName = (char*) malloc((strlen(epLibName)+1) * sizeof(char));*/
  bname= basename((char*)instanceName);
  /*
  if ( Buildings_FMUS[Buildings_nFMU]->epLibName == NULL )
    ModelicaError("Not enough memory in FMUZoneInit.c. to allocate EnergPlus library name.");
  */
  /*strcpy(Buildings_FMUS[Buildings_nFMU]->epLibName, epLibName);*/

  /* get current working directory */
  #ifdef _MSC_VER
  	if (_getcwd(Buildings_FMUS[Buildings_nFMU]->cwd, sizeof(Buildings_FMUS[Buildings_nFMU]->cwd))==NULL)
  #else
  	if (getcwd(Buildings_FMUS[Buildings_nFMU]->cwd, sizeof(Buildings_FMUS[Buildings_nFMU]->cwd))==NULL)
  #endif
  {
    ModelicaFormatError("Cannot get the current working directory for building = %s\n",
    Buildings_FMUS[Buildings_nFMU]->name);
  }
  else{
    /* create the output folder for current FMU in working directory */
    /* fixme needs to find the equivalent for Windows and mac */
    Buildings_FMUS[Buildings_nFMU]->outputs=(char*)malloc((strlen (bname) + strlen (Buildings_FMUS[Buildings_nFMU]->cwd) +
    10)*sizeof(char));
    sprintf(Buildings_FMUS[Buildings_nFMU]->outputs, "%s%s%s%s", Buildings_FMUS[Buildings_nFMU]->cwd,
    PATH_SEP, bname, PATH_SEP);
  }
 /*Check if output directory exists*/
  result=stat (Buildings_FMUS[Buildings_nFMU]->outputs, &stat_p);
  if (result>=0){
		ModelicaFormatMessage("****** Output directory=%s exists for building = %s. "
		"The output directory won't be re-created.\n", Buildings_FMUS[Buildings_nFMU]->outputs,
    Buildings_FMUS[Buildings_nFMU]->name);
	}
	else{
			// The 30 are for the additional characters in tmp_str
      ModelicaFormatMessage("****** Creating the output directory %s\n", Buildings_FMUS[Buildings_nFMU]->outputs);
			tmp_str=(char*)malloc((strlen(Buildings_FMUS[Buildings_nFMU]->outputs) + 30)*sizeof(char));
			sprintf(tmp_str, "mkdir %s", Buildings_FMUS[Buildings_nFMU]->outputs);
			result=system (tmp_str);
      if (result>=0){
        ModelicaFormatError("****** Output directory=%s couldn't be created = %s\n",
        Buildings_FMUS[Buildings_nFMU]->outputs);
      }
			free(tmp_str);
		}

    Buildings_FMUS[Buildings_nFMU]->epLibName = (char*) malloc((strlen(Buildings_FMUS[Buildings_nFMU]->outputs)+
    2*strlen(bname) + 20) * sizeof(char));
    if ( Buildings_FMUS[Buildings_nFMU]->epLibName == NULL )
      ModelicaError("Not enough memory in FMUZoneInit.c. to allocate EnergPlus library name.");
  #ifdef _MSC_VER
  	//"\"" are quotes needed for path with spaces in the names
  	/*sprintf(tmp_str, "xcopy %s%s %s%s%s /Y /I", "\"", _c->tmpResCon, "\"", _c->fmuOutput, "\"");*/
  #elif __linux__
    sprintf (Buildings_FMUS[Buildings_nFMU]->epLibName, "%s%s%s%s%s%s", Buildings_FMUS[Buildings_nFMU]->cwd,
    PATH_SEP, bname, PATH_SEP, bname, ".so");
    tmp_str=(char*)(malloc((strlen(epLibName) + strlen (Buildings_FMUS[Buildings_nFMU]->epLibName) + 10)*sizeof(char)));
  	sprintf(tmp_str, "cp -f %s%s%s %s", "\"", epLibName,  "\"", Buildings_FMUS[Buildings_nFMU]->epLibName);
  #elif __APPLE__
  #else
  	ModelicaMessage ("Cannot execute %s. The FMU export is only supported on Windows, Linux and Mac OS.\n", tmp_str);
  #endif
  /*Check if library exists*/
  result=result=stat (Buildings_FMUS[Buildings_nFMU]->epLibName, &stat_p);
  if (result>=0){
    ModelicaFormatMessage("****** EnergyPlus library name=%s exists for building = %s. "
    "The library won't be renamed.\n", Buildings_FMUS[Buildings_nFMU]->epLibName,
    Buildings_FMUS[Buildings_nFMU]->name);
  }
  else{
    result=system (tmp_str);
    if (result>=0){
      ModelicaFormatError("****** EnergyPlus library=%s couldn't be renamed to = %s\n",
      epLibName, Buildings_FMUS[Buildings_nFMU]->epLibName);
    }
    free(tmp_str);
  }

  /* Assign the Energyplus instance name */
  /*  Buildings_FMUS[Buildings_nFMU]->instanceName = (char*) malloc((strlen(bname)+1) * sizeof(char));*/
  // if ( Buildings_FMUS[Buildings_nFMU]->instanceName == NULL )
  //   ModelicaError("Not enough memory in FMUZoneInit.c. to allocate IDD name.");
  // strcpy(Buildings_FMUS[Buildings_nFMU]->instanceName, bname);

  /* Assign the zone name */
  Buildings_FMUS[Buildings_nFMU]->zoneNames[0] = malloc((strlen(zoneName)+1) * sizeof(char));
  if ( Buildings_FMUS[Buildings_nFMU]->zoneNames[0] == NULL )
    ModelicaError("Not enough memory in FMUZoneInit.c. to allocate zone name.");
  strcpy(Buildings_FMUS[Buildings_nFMU]->zoneNames[0], zoneName);

  Buildings_FMUS[Buildings_nFMU]->nZon = 1;

  Buildings_FMUS[Buildings_nFMU]->zones=malloc(sizeof(FMUZone *));
  if ( Buildings_FMUS[Buildings_nFMU]->zones== NULL )
    ModelicaError("Not enough memory in FMUZoneInit.c. to allocate zones.");

  /* Assign the zone */
  Buildings_FMUS[Buildings_nFMU]->zones[0] = zone;

  Buildings_nFMU++;
  /* Return the pointer to the FMU for this EnergyPlus instance */
  return Buildings_FMUS[Buildings_nFMU-1];
};

int zoneIsUnique(const struct FMUBuilding* fmuBld, const char* zoneName){
  int iZ;
  int isUnique = 1;
  for(iZ = 0; iZ < fmuBld->nZon; iZ++){
    if (!strcmp(zoneName, fmuBld->zoneNames[iZ])){
      isUnique = 0;
      break;
    }
  }
  return isUnique;
}

/* Create the structure and return a pointer to its address. */
void* FMUZoneInit(const char* idfName, const char* instanceName, const char* weaName,
  const char* iddName, const char* zoneName, const char* epLibName)
/*void* FMUZoneInit(const char* idfName, const char* zoneName, int nFluPor,
  const char** varNamSen, size_t nVarSen, const char** varNamRec, size_t nVarRec, int* valRefVarRec, size_t nValRefVarRec)*/
{
  /* Note: The idfName is needed to unpack the fmu so that the valueReference
     for the zone with zoneName can be obtained */
  unsigned int i;

  ModelicaFormatMessage("****** Initializing zone %s, fmu = %s****** \n", zoneName, idfName);

  /* ********************************************************************** */
  /* Initialize the zone */
  FMUZone* zone = (FMUZone*) malloc(sizeof(FMUZone));
  if ( zone == NULL )
    ModelicaError("Not enough memory in FMUZoneInit.c. to allocate zone.");

  /* Assign the value reference. This should be done by using the values from modelDescription.xml */
  /*for(i = 0; i < nFluPor; i++){
    zone->valueReference[i] = i;
  }
  */

  /* Assign the zone name */
  zone->name = malloc((strlen(zoneName)+1) * sizeof(char));
  if ( zone->name == NULL )
    ModelicaError("Not enough memory in FMUZoneInit.c. to allocate zone name.");
  strcpy(zone->name, zoneName);

  /* ********************************************************************** */
  /* Initialize the pointer for the FMU to which this zone belongs */
  /* Check if there are any zones */
  if (Buildings_nFMU == 0){
    /* No FMUs exist. Instantiate an FMU and */
    /* assign this fmu pointer to the zone that will invoke its setXXX and getXXX */
    zone->ptrBui = instantiateEnergyPlusFMU(idfName, instanceName,
      weaName, iddName, zoneName, epLibName, zone);
    zone->index = 1;
  } else {
    /* There is already a Buildings FMU allocated.
       Check if the current zone is for this FMU. */
      zone->ptrBui = NULL;
      for(i = 0; i < Buildings_nFMU; i++){

        if (strcmp(idfName, Buildings_FMUS[i]->name) == 0){
          /* This is the same FMU as before. */
          if (! zoneIsUnique(Buildings_FMUS[i], zoneName)){
            ModelicaFormatError("Modelica model specifies zone %s twice for the FMU %s. Each zone must only be specified once.",
            zoneName, Buildings_FMUS[i]->name);
          }

          struct FMUBuilding* bld = Buildings_FMUS[i];
          zone->ptrBui = bld;
          /* Increment size of vector that contains the zone names. */
          bld->zoneNames = realloc(bld->zoneNames, (bld->nZon + 1) * sizeof(char*));
          bld->zones = realloc(bld->zones, (bld->nZon + 1) * sizeof(FMUZone*));
          if (bld->zoneNames == NULL){
            ModelicaError("Not enough memory in FMUZoneInit.c. to allocate memory for bld->zoneNames.");
          }
          /* Add storage for new zone name, and copy the zone name */
          bld->zoneNames[bld->nZon] = malloc((strlen(zoneName)+1) * sizeof(char));
          bld->zones[bld->nZon] = zone;
          if ( bld->zoneNames[bld->nZon] == NULL )
            ModelicaError("Not enough memory in FMUZoneInit.c. to allocate zone name.");
          strcpy(bld->zoneNames[bld->nZon], zoneName);
          /* Increment the count of zones to this building. */
          bld->nZon++;
          zone->index = bld->nZon;
          break;
        }
      }
      /* Check if we found an FMU */
      if (zone->ptrBui == NULL){
        /* Did not find an FMU. */
        zone->ptrBui = instantiateEnergyPlusFMU(idfName, instanceName,
          weaName, iddName, zoneName, epLibName, zone);
      }
  }
  /*Set the fmu to null to control execution*/
  zone->ptrBui->fmu=NULL;
  /* Return a pointer to this zone */
  return (void*) zone;
};
