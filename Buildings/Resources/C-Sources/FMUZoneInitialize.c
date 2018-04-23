/*
 * Modelica external function to intialize EnergyPlus.
 *
 * Michael Wetter, LBNL                  3/1/2018
 * Thierry S. Nouidui, LBNL              3/23/2018
 */

#include "FMUEnergyPlusStructure.h"
#include <stdlib.h>
extern void *dlmopen (long int __nsid, const char*, int);

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
HANDLE h;

#ifdef _MSC_VER
	h = LoadLibrary(libPath);
	if (!h) {
		ModelicaFormatError("****** Unable to load the EnergyPlus "
		"functions library with path %s ****** \n",
		libPath);
	}
#else
	h = dlopen(libPath, RTLD_LAZY);
	/*h=dlmopen(-1, libPath, RTLD_LAZY | RTLD_LOCAL | RTLD_DEEPBIND);*/
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
	return 0;

}

void FMUZoneInitialize(void* object, double tStart, double* AFlo, double* V, double* mSenFac){
  fmi2Byte msg[200];
  FMUZone* zone = (FMUZone*) object;
	/* Prevent this to be called multiple times */
  FMU* fmu;
	int cntr=0;
	int result;
	int i, j, k ;
	size_t totNumInp;
	size_t totNumOut;

  int nZon=zone->ptrBui->nZon;
	/* fmi2ValueReference* inputValueReferences=(fmi2ValueReference* )valgrin(nInp*sizeof(fmi2ValueReference)); */
	/* fmi2ValueReference* outputValueReferences=(fmi2ValueReference* )malloc(nOut*sizeof(fmi2ValueReference)); */
	int scaInp=1;
	int scaOut=4;
	int nInp = scaInp*nZon;
	int nOut = scaOut*nZon;

	char** inputNames=(char**)malloc(nInp*sizeof(char*));
	char** outputNames=(char**)malloc(nOut*sizeof(char*));
	fmi2ValueReference inputValueReferences [nInp];
	fmi2ValueReference outputValueReferences [nOut];

	/* const char* consInputNames[]={"T", "X", "mInlets_flow", "TInlet", "QGaiRad_flow"}; */
	/* const char* consOutputNames[]={"TRad", "QConSen_flow", "QLat_flow", "QPeo_flow"}; */

	const char* consInputNames[]={"T"};
	const char* consOutputNames[]={"QConSen_flow", "V", "AFlo", "mSenFac"};

	fmu = (FMU*)malloc(sizeof(FMU));
	FMUZone** tmpZon;
	tmpZon=(FMUZone**)malloc(nZon*sizeof(FMUZone*));

	for(i=0; i<nZon; i++){
		tmpZon[i] = (FMUZone*)malloc(sizeof(FMUZone));
		char* name = ((FMUZone*)(zone->ptrBui->zones[i]))->name;
		tmpZon[i]->name=name;
		zone->ptrBui->zones[i] = tmpZon[i];
	}

	/* Boolean to check that setupExperiment is only called once. */
	if (zone->ptrBui->fmu==NULL){
	 zone->ptrBui->_firstCall = 1;
	}
	else{
		zone->ptrBui->_firstCall=0;
	}

	zone->ptrBui->fmu = fmu;
	for (i=0; i<nInp; i++){
		inputValueReferences[i]=i;
	}

	for (i=0; i<nOut; i++){
		outputValueReferences[i]=i+nInp;
	}

	/* Save input value references at zone and building level */
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

	/* Save output value references at zone and building level */
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

	/* Compute the total number of input variables of the building model */
	totNumInp=sizeof(inputValueReferences)/sizeof(inputValueReferences[0]);
	/* Compute the total number of output variables of the building model */
	totNumOut=sizeof(outputValueReferences)/sizeof(outputValueReferences[0]);

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
	result = loadLib(zone->ptrBui->epLibName, zone->ptrBui->fmu);
	//result = loadLib("/home/thierry/TestCase/libepfmi.so", zone->ptrBui->fmu);
	/* Instantiate the building FMU*/
	result = zone->ptrBui->fmu->instantiate(((const char*)zone->ptrBui->name), /* input */
	                      (const char*)zone->ptrBui->weather,  /* weather */
	                      (const char*)zone->ptrBui->idd,  /* idd */
                         "Alpha",  /* instanceName */
                         NULL,  /* parameterNames */
                         NULL, /* parameterValueReferences[] */
                         0, /* nPar */
                         (const char**)inputNames, /* inputNames */
                         inputValueReferences, /* inputValueReferences[] */
                         totNumInp, /* nInp */
                         (const char**)outputNames, /* outputNames */
                         outputValueReferences, /* outputValueReferences[] */
                         totNumOut, /* nOut */
                         NULL); /*log); */

	if(result<0){
		ModelicaFormatError("Couldn't instantiate building = %s\n",
		zone->ptrBui->name);
	}

	int index;
	if (zone->ptrBui->_firstCall){
	/* This function can only be called once per building FMU */
 	/* change the directory to make sure that FMUs are not overwritten */
	result = zone->ptrBui->fmu->setupExperiment(tStart, 1, NULL);
	if(result<0){
		ModelicaFormatError("Failed to get setup experiment for building = %s\n",
	zone->ptrBui->name);
  }
  }
	double outputs[totNumOut] ;
	/* Get initial output variables */
	result = zone->ptrBui->fmu->getVariables(outputValueReferences, outputs, totNumOut, NULL);
	if(result<0){
		ModelicaFormatError("Failed to get initial outputs for building = %s\n",
		zone->ptrBui->name);
	}

	char tmp[100];
	const char* parNames[] = {"V","AFlo","mSenFac"};
	double parValues[3];

	/* Map the output values to correct parameters */
	for (i=0; i<3; i++){
		sprintf(tmp, "%s%s%s", zone->name, ",", parNames[i]);
		for (j=0; j<totNumOut; j++){
			if (strstr(outputNames[j], tmp)!=NULL){
				index = j;
				parValues[i] = outputs[j];
				break;
			}
		}
	}

	*V = parValues[0];
	*AFlo = parValues[1];
	*mSenFac = parValues[2];

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
