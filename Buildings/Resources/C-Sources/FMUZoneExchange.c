/*
 * Modelica external function to communicate with EnergyPlus.
 *
 * Michael Wetter, LBNL                  2/14/2018
 * Thierry S. Nouidui, LBNL              4/16/2018
 */

#include "FMUEnergyPlusStructure.h"
#include <stdlib.h>
#include <math.h>

void setGetVariables(
  FMUZone* fmuZon,
  fmi2ValueReference inputValueReferences[],
  fmi2Real inputValues[],
  size_t nInp,
  fmi2ValueReference outputValueReferences[],
  fmi2Real outputValues[],
  size_t nOut)
  {
    int result = fmuZon->ptrBui->fmu->setVariables(inputValueReferences, inputValues, nInp, NULL);
    if(result < 0){
      ModelicaFormatError("Failed to set setup variables for building = %s\n",
      fmuZon->ptrBui->name);
    }
    result = fmuZon->ptrBui->fmu->getVariables(outputValueReferences, outputValues, 1, NULL);
    if(result < 0){
      ModelicaFormatError("Failed to get setup variables for building = %s\n",
      fmuZon->ptrBui->name);
    }
  }

void FMUZoneExchange(
  void* object,
  double T,
  double X,
  double *m_flow, /* size is nFluPor */
  double *TInlet, /* size is nFluPor */
  double QGaiRad_flow,
  double time,
  double* TRad,
  double* QConSen_flow,
  double* dQConSen_flow,
  double* QLat_flow,
  double* QPeo_flow,
  double* tNext){

  FMUZone* zone = (FMUZone*) object;
  double inputValues[1];
  double outputValues[1];
  int result;

  /* Emulate heat transfer to a surface at constant T=18 degC */
  /* *QConSen_flow = 10*((273.15+18)-T);*/
  /* snprintf(msg, 200, "local is %f\n", *QConSen_flow); */
  const double dT = 0.01; /* Increment for derivative approximation */
  double QConSenPer_flow;

  FMUZone* tmpZon = malloc(sizeof(FMUZone));
  tmpZon=(FMUZone*)zone->ptrBui->zones[zone->index-1];
  /* Time need to be guarded against rounding error */
  /* *tNext = round((floor(time/3600.0)+1) * 3600.0); */

  /*
	#ifdef _MSC_VER
	result=_chdir(zone->ptrBui->outputs);
	#else
	result=chdir(zone->ptrBui->outputs);
  #endif
	if(result<0){
		ModelicaFormatError("Couldn't change to the output directory %s "
    "for building = %s\n",
		zone->ptrBui->outputs, zone->ptrBui->name);
	}
  */

  result=zone->ptrBui->fmu->setTime(time, NULL);
  if(result<0){
    ModelicaFormatError("Failed to set time in building = %s\n",
    zone->ptrBui->name);
  }

  /* Forward difference for QConSen_flow */
  inputValues[0] = T - 273.15 + dT;
  setGetVariables(zone, tmpZon->inputValueReferences, inputValues, 1, tmpZon->outputValueReferences, outputValues, 1);
  QConSenPer_flow=outputValues[0];
  inputValues[0] = T - 273.15;
  setGetVariables(zone, tmpZon->inputValueReferences, inputValues, 1, tmpZon->outputValueReferences, outputValues, 1);
  *QConSen_flow=outputValues[0];
  *dQConSen_flow = (QConSenPer_flow-*QConSen_flow)/dT;

  /* Get next event time */
  result = zone->ptrBui->fmu->getNextEventTime(&zone->ptrBui->fmu->eventInfo, NULL);
  if(result<0){
    ModelicaFormatError("Failed to get next event time for building = %s, zone = %s, time = %f",
    zone->ptrBui->name, zone->name, time);
  }
  if(zone->ptrBui->fmu->eventInfo.terminateSimulation == fmi2True){
    ModelicaFormatError("EnergyPlus requested to terminate the simulation for building = %s, zone = %s, time = %f",
    zone->ptrBui->name, zone->name, time);
  }
  if(zone->ptrBui->fmu->eventInfo.nextEventTimeDefined == fmi2False){
    ModelicaFormatError("EnergyPlus failed to declare the next event time for building = %s, zone = %s, time = %f. Check with support.",
    zone->ptrBui->name, zone->name, time);
  }
  *tNext = zone->ptrBui->fmu->eventInfo.nextEventTime;
/*
  ModelicaFormatMessage("*** In exchange for bldg: %s; zone: %s, time = %f, tNext = %f, pointer to fmu %p.\n",
   zone->ptrBui->name,
   zone->name,
   time,
   *tNext,
   zone->ptrBui);
*/
  /*
  #ifdef _MSC_VER
  result=_chdir(zone->ptrBui->cwd);
  #else
  result=chdir(zone->ptrBui->cwd);
  #endif
  if(result<0){
    ModelicaFormatError("Couldn't change to the default working folder=%s "
    "for building = %s\n",
    zone->ptrBui->cwd, zone->ptrBui->name);
  }
  */
  *TRad = 293.15;
  *QLat_flow = 0;
  *QPeo_flow = 0;
  return;
}
