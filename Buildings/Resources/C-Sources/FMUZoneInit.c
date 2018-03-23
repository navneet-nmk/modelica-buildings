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
FMUBuilding* instantiateEnergyPlusFMU(const char* fmuName, const char* zoneName, FMUZone* zone)
{
  char msg[200];

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
  Buildings_FMUS[Buildings_nFMU]->name = (char*) malloc(strlen(fmuName) * sizeof(char));
  if ( Buildings_FMUS[Buildings_nFMU]->name == NULL )
    ModelicaError("Not enough memory in FMUZoneInit.c. to allocate fmu name.");
  strcpy(Buildings_FMUS[Buildings_nFMU]->name, fmuName);

  /* Assign the zone name */
  Buildings_FMUS[Buildings_nFMU]->zoneNames[0] = malloc(strlen(zoneName) * sizeof(char));
  if ( Buildings_FMUS[Buildings_nFMU]->zoneNames[0] == NULL )
    ModelicaError("Not enough memory in FMUZoneInit.c. to allocate zone name.");
  strcpy(Buildings_FMUS[Buildings_nFMU]->zoneNames[0], zoneName);

  Buildings_FMUS[Buildings_nFMU]->nZon = 1;

   /* Assign the zone */
  Buildings_FMUS[Buildings_nFMU]->zones = zone;

  snprintf(msg, 200, "****** Output value references in zone %s, number = %d ****** \n", ((FMUZone*)(Buildings_FMUS[Buildings_nFMU]->zones))->name, Buildings_nFMU);
  ModelicaMessage(msg);

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
void* FMUZoneInit(const char* fmuName, const char* zoneName, int nFluPor, int nInputValueReferences,
                  char** inputVariableNames, int* inputValueReferences, int nOutputValueReferences,
                  char** outputVariableNames, int* outputValueReferences)
{
  /* Note: The fmuName is needed to unpack the fmu so that the valueReference
     for the zone with zoneName can be obtained */
  char msg[200];
  unsigned int i;

  snprintf(msg, 200, "****** Initializing zone %s, fmu = %s, nFluPor = %d ****** \n", zoneName, fmuName, nFluPor);
  ModelicaMessage(msg);

  /* ********************************************************************** */
  /* Initialize the zone */
  FMUZone* zone = (FMUZone*) malloc(sizeof(FMUZone));
  if ( zone == NULL )
    ModelicaError("Not enough memory in FMUZoneInit.c. to allocate zone.");
  zone->valueReference = NULL;
  zone->inputValueReferences = NULL;
  zone->outputValueReferences = NULL;

  zone->valueReference = malloc(nFluPor * sizeof(unsigned int));
  if ( zone->valueReference == NULL )
    ModelicaError("Not enough memory in FMUZoneInit.c. to allocate valueReference.");
  zone->nValueReference = nFluPor; /* Will need to be fixed later */

  /* Assign the value reference. This should be done by using the values from modelDescription.xml */
  for(i = 0; i < nFluPor; i++){
    zone->valueReference[i] = i;
  }

  /* Assign the zone name */
  zone->name = malloc(strlen(zoneName) * sizeof(char));
  if ( zone->name == NULL )
    ModelicaError("Not enough memory in FMUZoneInit.c. to allocate zone name.");
  strcpy(zone->name, zoneName);


 /* Allocate memory for the output names and value references */
  zone->inputVariableNames = malloc(nInputValueReferences * sizeof(char *));
  if ( zone->inputVariableNames == NULL )
    ModelicaError("Not enough memory in FMUZoneInit.c. to allocate inputVariableNames.");
  
  zone->inputValueReferences = malloc(nInputValueReferences * sizeof(unsigned int));
  if ( zone->inputValueReferences == NULL )
    ModelicaError("Not enough memory in FMUZoneInit.c. to allocate inputValueReferences.");
  zone->nInputValueReferences = nInputValueReferences; 

  /* Assign the output names and value references*/
  for(i = 0; i < nInputValueReferences; i++){
    zone->inputValueReferences[i] = inputValueReferences[i];
    zone->inputVariableNames[i] = malloc(strlen(inputVariableNames[i]) * sizeof(char));
    strcpy (zone->inputVariableNames[i], inputVariableNames[i]);
  }

  //snprintf(msg, 200, "****** Input value references in zone %s, fmu = %s, inputValueReferences = %d ****** \n", zoneName, fmuName, zone->inputValueReferences[0]);
  //ModelicaMessage(msg);

  //snprintf(msg, 200, "****** Input value references in zone %s, fmu = %s, inputValueReferences = %s ****** \n", zoneName, fmuName, inputVariableNames[0]);
  //ModelicaMessage(msg);


 /* Allocate memory for the output names and value references */
  zone->outputVariableNames = malloc(nOutputValueReferences * sizeof(char *));
  if ( zone->outputVariableNames == NULL )
    ModelicaError("Not enough memory in FMUZoneInit.c. to allocate outputVariableNames.");
  
  zone->outputValueReferences = malloc(nOutputValueReferences * sizeof(unsigned int));
  if ( zone->outputValueReferences == NULL )
    ModelicaError("Not enough memory in FMUZoneInit.c. to allocate outputValueReferences.");
  zone->nOutputValueReferences = nOutputValueReferences; 

  /* Assign the output names and value references*/
  for(i = 0; i < nOutputValueReferences; i++){
    zone->outputValueReferences[i] = outputValueReferences[i];
    zone->outputVariableNames[i] = malloc(strlen(outputVariableNames[i]) * sizeof(char));
    strcpy (zone->outputVariableNames[i], outputVariableNames[i]);
  }

  //snprintf(msg, 200, "****** Output value references in zone %s, fmu = %s, outputValueReferences = %d ****** \n", zoneName, fmuName, zone->outputValueReferences[0]);
  //ModelicaMessage(msg);

  //snprintf(msg, 200, "****** Output value references in zone %s, fmu = %s, outputValueReferences = %s ****** \n", zoneName, fmuName, outputVariableNames[0]);
  //ModelicaMessage(msg);

  /* ********************************************************************** */
  /* Initialize the pointer for the FMU to which this zone belongs */
  /* Check if there are any zones */
  if (Buildings_nFMU == 0){
    /* No FMUs exist. Instantiate an FMU and */
    /* assign this fmu pointer to the zone that will invoke its setXXX and getXXX */
    zone->ptrBui = instantiateEnergyPlusFMU(fmuName, zoneName, zone);
  } else {
    /* There is already a Buildings FMU allocated.
       Check if the current zone is for this FMU. */
      zone->ptrBui = NULL;
      for(i = 0; i < Buildings_nFMU; i++){

        if (strcmp(fmuName, Buildings_FMUS[i]->name) == 0){
          /* This is the same FMU as before. */
          if (! zoneIsUnique(Buildings_FMUS[i], zoneName)){
            snprintf(msg, 200, "Modelica model specifies zone %s twice for the FMU %s. Each zone must only be specified once.", zoneName, Buildings_FMUS[i]->name);
            ModelicaError(msg);
          }

          struct FMUBuilding* bld = Buildings_FMUS[i];
          zone->ptrBui = bld;
          /* Increment size of vector that contains the zone names. */
          bld->zoneNames = realloc(bld->zoneNames, (bld->nZon + 1) * sizeof(char*));
          if (bld->zoneNames == NULL){
            ModelicaError("Not enough memory in FMUZoneInit.c. to allocate memory for bld->zoneNames.");
          }
          /* Add storage for new zone name, and copy the zone name */
          bld->zoneNames[bld->nZon] = malloc(strlen(zoneName) * sizeof(char));
          if ( bld->zoneNames[bld->nZon] == NULL )
            ModelicaError("Not enough memory in FMUZoneInit.c. to allocate zone name.");
          strcpy(bld->zoneNames[bld->nZon], zoneName);
          /* Increment the count of zones to this building. */
          bld->nZon++;
          break;
        }
      }
      /* Check if we found an FMU */
      if (zone->ptrBui == NULL){
        /* Did not find an FMU. */
        zone->ptrBui = instantiateEnergyPlusFMU(fmuName, zoneName, zone);
      }
  }
  /* Return a pointer to this zone */
  return (void*) zone;
};
