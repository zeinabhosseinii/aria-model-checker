                                    


#include "Types.h"
#include "CheckerActor.h"
#include <stdio.h>
#include <string.h>
#include <limits>

#include "ProducerActor.h"
#include "FilterAActor.h"
#include "FilterBActor.h"
#include "MergerActor.h"
#include "AnalyzerActor.h"
#include "LoggerActor.h"
#include "SinkActor.h"

// location counters (values stored in messageQueue[0])
/* There can be more than one location counter for each msgsrv when we
   want to have non-atomic message servers.
   Notice that values 0 and 1 are reserved for EMPTY and INITIAL, respectively.*/
#define VERIFY 1


// knownrebecs (those to whom message can be sent)
// the following format allows to use them as variables --> the ID of the rebec
#define _ref_sender (rebecs[senderQueue[0]])
#define _ref_self (dynamic_cast<CheckerActor*>(rebecs[(int)((unsigned char)myID)]))

    	#define _ref_sk (dynamic_cast<SinkActor*>(rebecs[(int)((unsigned char)known[1])]))
    
CheckerActor::CheckerActor(byte myID, char* myName, int maxQueueLength, int maxParamLength, vector<AbstractActor*>& rebecsRef
		#ifdef DEBUG_LEVEL_2 
			, ostream &out
		#endif







		,int stateSize, int hashmapSize














)
	:
		AbstractTimedActor (myID, myName, maxQueueLength, maxParamLength, rebecsRef
			#ifdef DEBUG_LEVEL_2 
				, out
			#endif







		, stateSize, hashmapSize














		)
	, AbstractActor (myID, myName, maxQueueLength, maxParamLength, rebecsRef
		#ifdef DEBUG_LEVEL_2 
			, out
		#endif







		, stateSize, hashmapSize














	)
{
    messageNames = new char *[1 + 1];
    messagePriorities = new int[1 + 1];
	messageNames[0] = (char*)"Empty";

	
		
	
		
       		 messageNames[1] = (char*)"VERIFY";
			messagePriorities[1] = std::numeric_limits<int>::max();








	if (table == (byte**)0xFFFFFFFF) {	// avoid reinitialization
		table = new byte* [1 << hashmapSize];
        memset (table, 0, sizeof(byte*)*(1 << hashmapSize));
	}








	if (hashtableTimeExtension == (vector<TimeFrame>**)0xFFFFFFFF) {	// avoid reinitialization
        hashtableTimeExtension = new vector<TimeFrame>*[1 << hashmapSize];
        memset (hashtableTimeExtension, 0, sizeof(vector<TimeFrame>*)*(1 << hashmapSize));
	}







	
	
	queueTail = 0;
}

CheckerActor::~CheckerActor (){
	delete [] messageNames;
	delete [] messageQueue;
	delete [] senderQueue;
}


void CheckerActor::exportAMessageInXML(ostream &out, string tab, int cnt) {
        int i = 0;
        switch (messageQueue[cnt]) {
            case VERIFY: {

            	int _ref_peak;
            
            
                	memcpy(&_ref_peak, &paramQueue[cnt] [i], (1 * INT_SIZE));
	                i += (1 * INT_SIZE);

            	int _ref_trough;
            
            
                	memcpy(&_ref_trough, &paramQueue[cnt] [i], (1 * INT_SIZE));
	                i += (1 * INT_SIZE);

            	int _ref_balanced;
            
            
                	memcpy(&_ref_balanced, &paramQueue[cnt] [i], (1 * INT_SIZE));
	                i += (1 * INT_SIZE);

            	int _ref_runs;
            
            
                	memcpy(&_ref_runs, &paramQueue[cnt] [i], (1 * INT_SIZE));
	                i += (1 * INT_SIZE);
                out << "verify("
                	<< ((int)_ref_peak)
					<< ", "
                	<< ((int)_ref_trough)
					<< ", "
                	<< ((int)_ref_balanced)
					<< ", "
                	<< ((int)_ref_runs)
                << ")";
                break;
               }
    	}
}


void CheckerActor::exportStateVariablesInXML(ostream &out, string tab) {
	out << tab << "<statevariables>" << endl;
	exportInsideStateVariablesTagInXML(out, tab);
	out << tab << "</statevariables>" << endl;
}

void CheckerActor::exportInsideStateVariablesTagInXML(ostream &out, string tab){


			    	out << tab << '\t' << "<variable name=\"Checker.checks\" type=\"int\">" <<  
			    		((int)_ref_checks) << "</variable>" << endl;
			    	out << tab << '\t' << "<variable name=\"Checker.failures\" type=\"int\">" <<  
			    		((int)_ref_failures) << "</variable>" << endl;
			    	out << tab << '\t' << "<variable name=\"Checker.lastPeak\" type=\"int\">" <<  
			    		((int)_ref_lastPeak) << "</variable>" << endl;
			    	out << tab << '\t' << "<variable name=\"Checker.lastTrough\" type=\"int\">" <<  
			    		((int)_ref_lastTrough) << "</variable>" << endl;
}


char* CheckerActor::getClassName() {
    return (char*)"Checker";
}
void CheckerActor::marshalVariablesToArray(byte * array){
	int marshalIndex = 0;
    	
            
                	memcpy(&array [marshalIndex], &_ref_checks, (1 * INT_SIZE));
	                marshalIndex += (1 * INT_SIZE);
            
                	memcpy(&array [marshalIndex], &_ref_failures, (1 * INT_SIZE));
	                marshalIndex += (1 * INT_SIZE);
            
                	memcpy(&array [marshalIndex], &_ref_lastPeak, (1 * INT_SIZE));
	                marshalIndex += (1 * INT_SIZE);
            
                	memcpy(&array [marshalIndex], &_ref_lastTrough, (1 * INT_SIZE));
	                marshalIndex += (1 * INT_SIZE);

    
	
}
void CheckerActor::marshalActorToArray(byte* array) {
    int i,j, marshalIndex = 0;

	for (i = (shift ? 1 : 0); i < maxQueueLength; i++) {
        array[marshalIndex++] = messageQueue[i];
    }

    if (shift) {
        array[marshalIndex++] = 0;
    }
    
	for (i = (shift ? 1 : 0); i < maxQueueLength; i++) {
        array[marshalIndex++] = senderQueue[i];
    }

    if (shift) {
        array[marshalIndex++] = 0;
    }

	if(maxParamLength != 0) {
        for (i = (shift ? 1 : 0); i < maxQueueLength; i++) {
            for (j = 0; j < maxParamLength; j++) {
                array[marshalIndex++] = paramQueue[i][j];
            }
        }
        if (shift) {
            for (j = 0; j < maxParamLength; j++) {
                array[marshalIndex++] = 0;
            }
        }
    }

	 #ifdef NO_ATOMIC_EXECUTION
    	memcpy(&array [marshalIndex], &__pc, 4);
    	marshalIndex += 4;
    	//array[marshalIndex++] = __pc;
    #endif

		marshalVariablesToArray( & array[marshalIndex]);

   

}

void CheckerActor::unmarshalActorFromArray(byte* array) {

    int index = 0, i, j;
    shift = 0;

	for (i = 0; i < maxQueueLength; i++) {
        messageQueue[i] = array[index++];
    }
	
	for (i = 0; i < maxQueueLength; i++) {
        senderQueue[i] = array[index++];
    }
    
	if(maxParamLength != 0) {
        for (i = 0; i < maxQueueLength; i++) {
            for (j = 0; j < maxParamLength; j++) {
                paramQueue[i][j] = array[index++];
            }
        }
    }
    
	#ifdef NO_ATOMIC_EXECUTION
    	memcpy(&__pc, &array [index], 4);
    	index += 4;
    	//__pc = array[index++];
    #endif
    
	unmarshalVariablesFromArray( & array[index]);
}

void CheckerActor::unmarshalVariablesFromArray(byte * array){

	int index = 0;

            
            
                	memcpy(&_ref_checks, &array [index], (1 * INT_SIZE));
	                index += (1 * INT_SIZE);

            
            
                	memcpy(&_ref_failures, &array [index], (1 * INT_SIZE));
	                index += (1 * INT_SIZE);

            
            
                	memcpy(&_ref_lastPeak, &array [index], (1 * INT_SIZE));
	                index += (1 * INT_SIZE);

            
            
                	memcpy(&_ref_lastTrough, &array [index], (1 * INT_SIZE));
	                index += (1 * INT_SIZE);
    
	
}

/*#ifdef DEBUG
void CheckerActor::printStateContent(long stateNo) {
	out << myName << "=>[";
	byte* currentValues = getTable()[stateNo];
	for (int cnt = 0; cnt < STATE_SIZE; cnt++)
		out << (int)currentValues[cnt] << ", ";
	out << "]";
}
#endif
*/

//---------------------------------------------------------
/**
  Checks which action of this rebec is enabled, and executes it. Actions are the
  same as the location-counter defined at the beginning of this file.
  In the case of non-atomic message servers, a location-counter may point to the
  middle of a message server.
  If this action leads to more than one transition (non_det), this method executes
  only one of these transitions. With the help of some static variables, consecutive
  calls to this method execute different transitions caused by this action.
  Local variables must be properly valuated before any call to this method (see marshal()).

  returns: >0 : there are more possible next states (non-det transitions).
		   =0 : no more non_det transitions
		   -1 : no enabled action (empty queue)
		   -2 : Local hash table is full - immediate termination of search
 */
long CheckerActor::methodSelectorAndExecutor() {

	
    switch (messageQueue[0]) {

            case VERIFY:
                {
                    int i = 0;

            	int _ref_peak;
            
            
                	memcpy(&_ref_peak, &paramQueue[0] [i], (1 * INT_SIZE));
	                i += (1 * INT_SIZE);

            	int _ref_trough;
            
            
                	memcpy(&_ref_trough, &paramQueue[0] [i], (1 * INT_SIZE));
	                i += (1 * INT_SIZE);

            	int _ref_balanced;
            
            
                	memcpy(&_ref_balanced, &paramQueue[0] [i], (1 * INT_SIZE));
	                i += (1 * INT_SIZE);

            	int _ref_runs;
            
            
                	memcpy(&_ref_runs, &paramQueue[0] [i], (1 * INT_SIZE));
	                i += (1 * INT_SIZE);
                    return msgsrvverify(
            _ref_peak
	        , 
            _ref_trough
	        , 
            _ref_balanced
	        , 
            _ref_runs
                    );
                }
    }
	return -1;
}

#define _RETURN_ return 0
    long CheckerActor::constructorChecker (byte creatorId 
    ) {
    			long arrayIndexChecker = 0;
		shift = 1;
		#ifdef SAFE_MODE
			string reactiveClassName = this->getName();
			string methodName = "Checker";
		#endif

	#ifdef TTS
	#endif

		{
			(_ref_checks=0);
			(_ref_failures=0);
			(_ref_lastPeak=0);
			(_ref_lastTrough=999);
		}
		shift = 0;
		queueTail = 0;
		while(messageQueue[queueTail]) {applyPolicy(false);queueTail++;}
		return 0;

	}

#define _RETURN_ return


		void CheckerActor::_msg_verify (byte senderId
	        ,
    		int
	         _ref_peak
	        , 
    		int
	         _ref_trough
	        , 
    		int
	         _ref_balanced
	        , 
    		int
	         _ref_runs
  	  ){
        queueTail = 0;
        enqueue(messageQueue, VERIFY);
        enqueue(senderQueue, senderId);

        if (maxParamLength != 0) {
            int index = 0;
            byte paramsValues[maxParamLength];
            memset(paramsValues, 0, maxParamLength);
    	
            
                	memcpy(&paramsValues [index], &_ref_peak, (1 * INT_SIZE));
	                index += (1 * INT_SIZE);
            
                	memcpy(&paramsValues [index], &_ref_trough, (1 * INT_SIZE));
	                index += (1 * INT_SIZE);
            
                	memcpy(&paramsValues [index], &_ref_balanced, (1 * INT_SIZE));
	                index += (1 * INT_SIZE);
            
                	memcpy(&paramsValues [index], &_ref_runs, (1 * INT_SIZE));
	                index += (1 * INT_SIZE);

            enqueue(paramQueue, paramsValues, maxParamLength);
        }
}









	byte** CheckerActor::table = (byte**)0xFFFFFFFF;

	byte** CheckerActor::getTable() {
		return table;
	}








	vector<TimeFrame>** CheckerActor::hashtableTimeExtension = (vector<TimeFrame>**)0xFFFFFFFF;
	
	vector<TimeFrame>** CheckerActor::getHashtableTimeExtension() {
		return hashtableTimeExtension;
	}








	    void CheckerActor::_timed_msg_verify (byte senderId
	        ,
    		int
	         _ref_peak
	        , 
    		int
	         _ref_trough
	        , 
    		int
	         _ref_balanced
	        , 
    		int
	         _ref_runs
, TIME_TYPE executionTime, TIME_TYPE deadline){
	    _msg_verify (senderId
	        ,
            _ref_peak
	        , 
            _ref_trough
	        , 
            _ref_balanced
	        , 
            _ref_runs
);
	    addTimedBundles(senderId, executionTime, deadline);
		}
	void CheckerActor::setNow(TIME_TYPE now) {
	
		AbstractTimedActor::setNow(now);
	}	
	

#ifdef SYMM


void CheckerActor::marshalVariablesToArrayWithPerm(byte* array, byte* perm) {
    int _mi = 0;
        memcpy(&array[_mi], &_ref_checks, (1 * INT_SIZE));
        _mi += (1 * INT_SIZE);
        memcpy(&array[_mi], &_ref_failures, (1 * INT_SIZE));
        _mi += (1 * INT_SIZE);
        memcpy(&array[_mi], &_ref_lastPeak, (1 * INT_SIZE));
        _mi += (1 * INT_SIZE);
        memcpy(&array[_mi], &_ref_lastTrough, (1 * INT_SIZE));
        _mi += (1 * INT_SIZE);
}

void CheckerActor::marshalParamsWithPerm(byte* output, byte* params, byte msgType, byte* perm) {
    if (maxParamLength > 0) memcpy(output, params, maxParamLength);
    switch (msgType) {
    case VERIFY: {
        break;
    }
    default: break;
    }
}
#endif // SYMM

	#define _RETURN_ goto _RETURN_POINT_VERIFY
	    long CheckerActor::msgsrvverify (
    		int
	         _ref_peak
	        , 
    		int
	         _ref_trough
	        , 
    		int
	         _ref_balanced
	        , 
    		int
	         _ref_runs
    ) {
    			long arrayIndexChecker = 0;SinkActor *temp0;
		shift = 1;
		#ifdef SAFE_MODE
			string reactiveClassName = this->getName();
			string methodName = "verify";
		#endif

	#ifdef TTS
	#endif

		{
			(_ref_checks=(_ref_checks+1));
			if ((_ref_peak<_ref_lastPeak)) {
				{
					(_ref_failures=(_ref_failures+1));
				}
}
			if ((_ref_trough>_ref_lastTrough)) {
				{
					(_ref_failures=(_ref_failures+1));
				}
}
			(_ref_lastPeak=_ref_peak);
			(_ref_lastTrough=_ref_trough);
			(temp0=_ref_sk, assertion(temp0!= null, "Null Pointer Exception in method " + reactiveClassName + "." + methodName + "line 183"), temp0)->_timed_msg_store(myID, _ref_failures, _ref_now, MAX_TIME);
			assertion((_ref_failures<=2));
			assertion((_ref_checks<=55));
		}
	_RETURN_POINT_VERIFY:
		return 0;

}
