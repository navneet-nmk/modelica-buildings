/*
 * Modelica external function to intialize EnergyPlus.
 *
 * Michael Wetter, LBNL                  3/1/2018
 * Thierry S. Nouidui, LBNL              3/23/2018
 */

#include "FMUEnergyPlusStructure.h"
#include <stdlib.h>

static void* getAdr(FMU *fmu, const char* functionName){
	void* fp;
#ifdef _MSC_VER
	fp = GetProcAddress(fmu->dllHandle, functionName);
#else
	fp = dlsym(fmu->dllHandle, functionName);
#endif
	if (!fp) {
		ModelicaFormatError("****** Function %s,  not "
		"found in the EnergyPlus functions library****** \n",
		functionName);
	}
	return fp;
}

static int loadLib(const char* libPath, FMU *fmu) {
#ifdef _MSC_VER
	HINSTANCE h;
#else
	void *h;
#endif

#ifdef _MSC_VER
	h = LoadLibrary(libPath);
	if (h == NULL) {
		ModelicaFormatError("****** Unable to load the EnergyPlus "
		"functions library with path %s ****** \n",
		libPath);
	}
	if (!h) {
		ModelicaFormatError("****** Unable to load the EnergyPlus "
		"functions library with path %s ****** \n",
		libPath);
	}
#else
	h = dlopen(libPath, RTLD_LAZY);
	if (h == NULL) {
		ModelicaFormatError("****** Unable to load the EnergyPlus "
		"functions library with path %s ****** \n",
		libPath);
	}
	if (!h) {
		ModelicaFormatError("****** Unable to load the EnergyPlus "
		"functions library with path %s ****** \n",
		libPath);
	}
#endif

	fmu->dllHandle = h;

	fmu->instantiate = (fInstantiate)getAdr(fmu, "instantiate");
	if (!(fmu->instantiate)) {
		ModelicaError("Can't find function instantiate().\n");
	}

	fmu->setupExperiment = (fSetupExperiment)getAdr(fmu, "setupExperiment");
	if (!(fmu->setupExperiment)) {
		ModelicaError("Can't find function setupExperiment().\n");
	}

	fmu->setTime = (fSetTime)getAdr(fmu, "setTime");
	if (!(fmu->setTime)) {
		ModelicaMessage("Can't find function setTime().\n");
	}

	fmu->setVariables = (fSetVariables) getAdr(fmu, "setVariables");
	if (!(fmu->setVariables)) {
		ModelicaError("Can't find function setVariables().\n");
	}
	fmu->getVariables = (fGetVariables)getAdr(fmu, "getVariables");
	if (!(fmu->getVariables)) {
		ModelicaError("Can't find function getVariables().\n");
	}

	fmu->getNextEventTime = (fGetNextEventTime)getAdr(fmu, "getNextEventTime");
	if (!(fmu->getNextEventTime)) {
		ModelicaError("Can't find function getNextEventTime().\n");
	}

	fmu->terminate = (fTerminate)getAdr(fmu, "terminate");
	if (!(fmu->terminate)) {
		ModelicaError("Can't find function terminate().\n");
	}
	return 0; //success

}

void FMUZoneInitialize(void* object, double* AFlo, double* V, double* mSenFac){
  fmi2Byte msg[200];
  FMUZone* zone = (FMUZone*) object;
	/*Prevent this to be called multiple times*/
  FMU* fmu;
	int cntr=0;
	int result;
	int i, j, k ;
	size_t totNumInp;
	size_t totNumOut;

  int nZon=zone->ptrBui->nZon;
	//fmi2ValueReference* inputValueReferences=(fmi2ValueReference* )malloc(nInp*sizeof(fmi2ValueReference));
	//fmi2ValueReference* outputValueReferences=(fmi2ValueReference* )malloc(nOut*sizeof(fmi2ValueReference));
	int scaInp=1;
	int scaOut=4;
	int nInp = scaInp*nZon;
	int nOut = scaOut*nZon;

	const char** inputNames=(char**)malloc(nInp*sizeof(char*));
	const char** outputNames=(char**)malloc(nOut*sizeof(char*));
	fmi2ValueReference inputValueReferences [nInp];
	fmi2ValueReference outputValueReferences [nOut];

	//const char* consInputNames[]={"T", "X", "mInlets_flow", "TInlet", "QGaiRad_flow"};
	//const char* consOutputNames[]={"TRad", "QConSen_flow", "QLat_flow", "QPeo_flow"};

	const char* consInputNames[]={"T"};
	const char* consOutputNames[]={"QConSen_flow", "V", "AFlo", "mSenFac"};
	int _setupExperiment;

	fmu = (FMU*)malloc(sizeof(FMU));
	FMUZone** tmpZon;
	tmpZon=(FMUZone**)malloc(nZon*sizeof(FMUZone*));

	for(i=0; i<nZon; i++){
		tmpZon[i] = (FMUZone*)malloc(sizeof(FMUZone));
		char* name = ((FMUZone*)(zone->ptrBui->zones[i]))->name;
		tmpZon[i]->name=name;
		zone->ptrBui->zones[i] = tmpZon[i];
	}

	/*Boolean to check that setupExperiment is only called once.*/
	if (zone->ptrBui->fmu==NULL){
	 _setupExperiment = 1;
	}
	else{
		_setupExperiment=0;
	}

	zone->ptrBui->fmu = fmu;
	for (i=0; i<nInp; i++){
		inputValueReferences[i]=i;
	}

	for (i=0; i<nOut; i++){
		outputValueReferences[i]=i+nInp;
	}

	/* Save input value references at zone and building level*/
	while (cntr<nInp){
		for (k=0; k<scaInp; k++){
			for (j=0; j<nZon; j++) {
				inputNames[cntr]=(char*)malloc((strlen(tmpZon[j]->name)+strlen(consInputNames[k]) + 2)*sizeof(char));
				sprintf(inputNames[cntr], "%s%s%s", tmpZon[j]->name, ",", consInputNames[k]);
				strcpy(tmpZon[j]->inputVariableNames[k], inputNames[cntr]);
				tmpZon[j]->inputValueReferences[k]=inputValueReferences[cntr];
				cntr++;
			}
		}
	}

	/* Save output value references at zone and building level*/
	cntr=0;
	while (cntr<nInp){
		for (k=0; k<scaOut; k++){
			for (j=0; j<nZon; j++){
			  outputNames[cntr]=(char*)malloc((strlen(tmpZon[j]->name)+strlen(consOutputNames[k]) + 2)*sizeof(char));
				sprintf(outputNames[cntr], "%s%s%s", tmpZon[j]->name, ",", consOutputNames[k]);
				strcpy(tmpZon[j]->outputVariableNames[k], outputNames[cntr]);
				tmpZon[j]->outputValueReferences[k]=outputValueReferences[cntr];
				cntr++;
			}
		}
	}

	/*Compute the total number of input variables of the building model*/
	totNumInp=sizeof(inputValueReferences)/sizeof(inputValueReferences[0]);
	/*Compute the total number of output variables of the building model*/
	totNumOut=sizeof(outputValueReferences)/sizeof(outputValueReferences[0]);

// for (i=0; i<scaInp; i++) {
// 	ModelicaFormatMessage("input reference in zone1 %d\n", ((FMUZone*)(zone->ptrBui->zones[0]))->inputValueReferences[i]);
// 	ModelicaFormatMessage("input name in zone1 %s\n", ((FMUZone*)(zone->ptrBui->zones[0]))->inputVariableNames[i]);
// }
// for (i=0; i<scaInp; i++) {
// 	ModelicaFormatMessage("input reference in zone2 %d\n", ((FMUZone*)(zone->ptrBui->zones[1]))->inputValueReferences[i]);
// 	ModelicaFormatMessage("input name in zone2 %s\n", ((FMUZone*)(zone->ptrBui->zones[1]))->inputVariableNames[i]);
// }
//
//
// for (i=0; i<scaOut; i++) {
// 	ModelicaFormatMessage("output reference in zone1 %d\n", ((FMUZone*)(zone->ptrBui->zones[0]))->outputValueReferences[i]);
// 	ModelicaFormatMessage("output name in zone1 %s\n", ((FMUZone*)(zone->ptrBui->zones[0]))->outputVariableNames[i]);
// }
// for (i=0; i<scaOut; i++) {
// 	ModelicaFormatMessage("output reference in zone2 %d\n", ((FMUZone*)(zone->ptrBui->zones[1]))->outputValueReferences[i]);
// 	ModelicaFormatMessage("output name in zone2 %s\n", ((FMUZone*)(zone->ptrBui->zones[1]))->outputVariableNames[i]);
// }

//if(zone){
  result = loadLib(zone->ptrBui->epLib, zone->ptrBui->fmu);
//
//   //ModelicaFormatMessage ("The number of input variables is %d\n", sizeof(inputValueReferences)/sizeof(inputValueReferences[0]));
//   //ModelicaFormatMessage ("The number of output variables is %s\n", strlen(outputValueReferences));
//
//   //snprintf(msg, 200, "The number of zones of the building is %d\n.", zone->ptrBui->nZon);
//   //ModelicaMessage(msg);
//
//   //snprintf(msg, 200, "The name of the building is %s\n.", zone->ptrBui->name);
//   //ModelicaMessage(msg);
//
//   snprintf(msg, 200, "Ready to instantiate the E+ FMU for current zone %s\n", zone->name);
//   ModelicaMessage(msg);
//   //
//
//

// const char* inputNames3[] = {"Attic,T", "Core_ZN,T", "Perimeter_ZN_1,T", "Perimeter_ZN_2,T", "Perimeter_ZN_3,T", "Perimeter_ZN_4,T"};
// const unsigned int inputValueReferences3[] = {0, 1, 2, 3, 4, 5,};
//
// const char * outputNames3[] = {
// "Attic,QConSen_flow", "Core_ZN,QConSen_flow", "Perimeter_ZN_1,QConSen_flow", "Perimeter_ZN_2,QConSen_flow", "Perimeter_ZN_3,QConSen_flow", "Perimeter_ZN_4,QConSen_flow",
// "Attic,V", "Core_ZN,V", "Perimeter_ZN_1,V", "Perimeter_ZN_2,V", "Perimeter_ZN_3,V", "Perimeter_ZN_4,V",
// "Attic,AFlo", "Core_ZN,AFlo", "Perimeter_ZN_1,AFlo", "Perimeter_ZN_2,AFlo", "Perimeter_ZN_3,AFlo", "Perimeter_ZN_4,AFlo",
// "Attic,mSenFac", "Core_ZN,mSenFac", "Perimeter_ZN_1,mSenFac", "Perimeter_ZN_2,mSenFac", "Perimeter_ZN_3,mSenFac", "Perimeter_ZN_4,mSenFac"
// };
// const unsigned int outputValueReferences3[] = {
//   6, 7, 8, 9, 10, 11,
//   12, 13, 14, 15, 16, 17,
//   18, 19, 20, 21, 22, 23,
//   24, 25, 26, 27, 28, 29
// };
//

// for (int i=0; i<totNumInp; i++){
//  ModelicaFormatMessage("%s\n", inputNames[i]);
//  }
//
//  for (int i=0; i<totNumInp; i++){
// 	 ModelicaFormatMessage("%d\n", inputValueReferences[i]);
// 	}
//
//  for (int i=0; i<totNumOut; i++){
//  	ModelicaFormatMessage("%s\n", outputNames[i]);
//   }
//
// 	for (int i=0; i<totNumOut; i++){
//   	ModelicaFormatMessage("%d\n", outputValueReferences[i]);
//    }

	/* Instantiate the building FMU*/
	result = zone->ptrBui->fmu->instantiate(zone->ptrBui->name, // input
	                           zone->ptrBui->weather, // weather
	                           zone->ptrBui->idd, // idd
	                           "Alpha", // instanceName
	                           NULL, // parameterNames
	                           NULL, // parameterValueReferences[]
	                           0, // nPar
	                           inputNames, // inputNames
	                           inputValueReferences, // inputValueReferences[]
	                           totNumInp, // nInp
	                           outputNames, // outputNames
	                           outputValueReferences, // outputValueReferences[]
	                           totNumOut, // nOut
	                           NULL); //log);

	if(result<0){
		ModelicaFormatMessage("Couldn't instantiate building FMU with name %s\n",
		zone->ptrBui->name);
	}

	 double tStart = 0.0;
	 int index;
	// snprintf(msg, 200, "The zone index is %d\n.", zone->index);
	// ModelicaMessage(msg);
	//
	// snprintf(msg, 200, "Currently calling zone: %s\n.", zone->name);
	// ModelicaMessage(msg);

	if (_setupExperiment){
		/*This function can only be called once per building FMU*/
		result = zone->ptrBui->fmu->setupExperiment(tStart, 1, NULL);
		if(result<0){
			ModelicaFormatMessage("Failed to get setup experiment for building FMU with name %s\n",
			zone->ptrBui->name);
		}
	}

	double outputs[totNumOut] ;
	/*Get initial output variables*/
	result = zone->ptrBui->fmu->getVariables(outputValueReferences, outputs, totNumOut, NULL);
	if(result<0){
		ModelicaFormatMessage("Failed to get initial outputs for building FMU with name %s\n",
		zone->ptrBui->name);
	}

	char tmp[100];
	const char* parNames[] = {"V","AFlo","mSenFac"};
	double parValues[3];

	/*Map the output values to correct parameters*/
	for (i=0; i<3; i++){
		sprintf(tmp, "%s%s%s", zone->name, ",", parNames[i]);
		for (j=0; j<totNumOut; j++){
			if (strstr(outputNames[j], tmp)!=NULL){
				index = j;
				//snprintf(msg, 200, "Found: %s in position %d with value reference %d. The value is %f\n.",
				//outputNames[j], j, outputValueReferences[j], outputs[j]);
				parValues[i] = outputs[j];
				//ModelicaMessage(msg);
				break;
			}
		}
	}

	*V = parValues[0];
	*AFlo = parValues[1];
	*mSenFac = parValues[2];
/* Obtain the floor area and the volume of the zone */

/*  snprintf(msg, 200,
    "*** In exchange for bldg: %s; zone: %s, n = %d, pointer to fmu %p.\n",
    zone->ptrBui->name,
    zone->name,
    zone->nValueReference,
    zone->ptrBui);
  ModelicaMessage(msg);
*/

return;
}