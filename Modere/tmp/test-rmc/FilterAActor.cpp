                                    


#include "Types.h"
#include "FilterAActor.h"
#include <stdio.h>
#include <string.h>
#include <limits>

#include "ProducerActor.h"
#include "FilterBActor.h"
#include "MergerActor.h"
#include "AnalyzerActor.h"
#include "CheckerActor.h"
#include "LoggerActor.h"
#include "SinkActor.h"

// location counters (values stored in messageQueue[0])
/* There can be more than one location counter for each msgsrv when we
   want to have non-atomic message servers.
   Notice that values 0 and 1 are reserved for EMPTY and INITIAL, respectively.*/
#define PROCESS 1


// knownrebecs (those to whom message can be sent)
// the following format allows to use them as variables --> the ID of the rebec
#define _ref_sender (rebecs[senderQueue[0]])
#define _ref_self (dynamic_cast<FilterAActor*>(rebecs[(int)((unsigned char)myID)]))

    	#define _ref_mg (dynamic_cast<MergerActor*>(rebecs[(int)((unsigned char)known[1])]))
    
FilterAActor::FilterAActor(byte myID, char* myName, int maxQueueLength, int maxParamLength, vector<AbstractActor*>& rebecsRef
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

	
		
	
		
       		 messageNames[1] = (char*)"PROCESS";
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

FilterAActor::~FilterAActor (){
	delete [] messageNames;
	delete [] messageQueue;
	delete [] senderQueue;
}


void FilterAActor::exportAMessageInXML(ostream &out, string tab, int cnt) {
        int i = 0;
        switch (messageQueue[cnt]) {
            case PROCESS: {

            	int _ref_v;
            
            
                	memcpy(&_ref_v, &paramQueue[cnt] [i], (1 * INT_SIZE));
	                i += (1 * INT_SIZE);

            	int _ref_mode;
            
            
                	memcpy(&_ref_mode, &paramQueue[cnt] [i], (1 * INT_SIZE));
	                i += (1 * INT_SIZE);
                out << "process("
                	<< ((int)_ref_v)
					<< ", "
                	<< ((int)_ref_mode)
                << ")";
                break;
               }
    	}
}


void FilterAActor::exportStateVariablesInXML(ostream &out, string tab) {
	out << tab << "<statevariables>" << endl;
	exportInsideStateVariablesTagInXML(out, tab);
	out << tab << "</statevariables>" << endl;
}

void FilterAActor::exportInsideStateVariablesTagInXML(ostream &out, string tab){


			    	out << tab << '\t' << "<variable name=\"FilterA.accepted\" type=\"int\">" <<  
			    		((int)_ref_accepted) << "</variable>" << endl;
			    	out << tab << '\t' << "<variable name=\"FilterA.rejected\" type=\"int\">" <<  
			    		((int)_ref_rejected) << "</variable>" << endl;
			    	out << tab << '\t' << "<variable name=\"FilterA.last\" type=\"int\">" <<  
			    		((int)_ref_last) << "</variable>" << endl;
}


char* FilterAActor::getClassName() {
    return (char*)"FilterA";
}
void FilterAActor::marshalVariablesToArray(byte * array){
	int marshalIndex = 0;
    	
            
                	memcpy(&array [marshalIndex], &_ref_accepted, (1 * INT_SIZE));
	                marshalIndex += (1 * INT_SIZE);
            
                	memcpy(&array [marshalIndex], &_ref_rejected, (1 * INT_SIZE));
	                marshalIndex += (1 * INT_SIZE);
            
                	memcpy(&array [marshalIndex], &_ref_last, (1 * INT_SIZE));
	                marshalIndex += (1 * INT_SIZE);

    
	
}
void FilterAActor::marshalActorToArray(byte* array) {
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

void FilterAActor::unmarshalActorFromArray(byte* array) {

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

void FilterAActor::unmarshalVariablesFromArray(byte * array){

	int index = 0;

            
            
                	memcpy(&_ref_accepted, &array [index], (1 * INT_SIZE));
	                index += (1 * INT_SIZE);

            
            
                	memcpy(&_ref_rejected, &array [index], (1 * INT_SIZE));
	                index += (1 * INT_SIZE);

            
            
                	memcpy(&_ref_last, &array [index], (1 * INT_SIZE));
	                index += (1 * INT_SIZE);
    
	
}

/*#ifdef DEBUG
void FilterAActor::printStateContent(long stateNo) {
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
long FilterAActor::methodSelectorAndExecutor() {

	
    switch (messageQueue[0]) {

            case PROCESS:
                {
                    int i = 0;

            	int _ref_v;
            
            
                	memcpy(&_ref_v, &paramQueue[0] [i], (1 * INT_SIZE));
	                i += (1 * INT_SIZE);

            	int _ref_mode;
            
            
                	memcpy(&_ref_mode, &paramQueue[0] [i], (1 * INT_SIZE));
	                i += (1 * INT_SIZE);
                    return msgsrvprocess(
            _ref_v
	        , 
            _ref_mode
                    );
                }
    }
	return -1;
}

#define _RETURN_ return 0
    long FilterAActor::constructorFilterA (byte creatorId 
    ) {
    			long arrayIndexChecker = 0;
		shift = 1;
		#ifdef SAFE_MODE
			string reactiveClassName = this->getName();
			string methodName = "FilterA";
		#endif

	#ifdef TTS
	#endif

		{
			(_ref_accepted=0);
			(_ref_rejected=0);
			(_ref_last=0);
		}
		shift = 0;
		queueTail = 0;
		while(messageQueue[queueTail]) {applyPolicy(false);queueTail++;}
		return 0;

	}

#define _RETURN_ return


		void FilterAActor::_msg_process (byte senderId
	        ,
    		int
	         _ref_v
	        , 
    		int
	         _ref_mode
  	  ){
        queueTail = 0;
        enqueue(messageQueue, PROCESS);
        enqueue(senderQueue, senderId);

        if (maxParamLength != 0) {
            int index = 0;
            byte paramsValues[maxParamLength];
            memset(paramsValues, 0, maxParamLength);
    	
            
                	memcpy(&paramsValues [index], &_ref_v, (1 * INT_SIZE));
	                index += (1 * INT_SIZE);
            
                	memcpy(&paramsValues [index], &_ref_mode, (1 * INT_SIZE));
	                index += (1 * INT_SIZE);

            enqueue(paramQueue, paramsValues, maxParamLength);
        }
}









	byte** FilterAActor::table = (byte**)0xFFFFFFFF;

	byte** FilterAActor::getTable() {
		return table;
	}








	vector<TimeFrame>** FilterAActor::hashtableTimeExtension = (vector<TimeFrame>**)0xFFFFFFFF;
	
	vector<TimeFrame>** FilterAActor::getHashtableTimeExtension() {
		return hashtableTimeExtension;
	}








	    void FilterAActor::_timed_msg_process (byte senderId
	        ,
    		int
	         _ref_v
	        , 
    		int
	         _ref_mode
, TIME_TYPE executionTime, TIME_TYPE deadline){
	    _msg_process (senderId
	        ,
            _ref_v
	        , 
            _ref_mode
);
	    addTimedBundles(senderId, executionTime, deadline);
		}
	void FilterAActor::setNow(TIME_TYPE now) {
	
		AbstractTimedActor::setNow(now);
	}	
	

#ifdef SYMM


void FilterAActor::marshalVariablesToArrayWithPerm(byte* array, byte* perm) {
    int _mi = 0;
        memcpy(&array[_mi], &_ref_accepted, (1 * INT_SIZE));
        _mi += (1 * INT_SIZE);
        memcpy(&array[_mi], &_ref_rejected, (1 * INT_SIZE));
        _mi += (1 * INT_SIZE);
        memcpy(&array[_mi], &_ref_last, (1 * INT_SIZE));
        _mi += (1 * INT_SIZE);
}

void FilterAActor::marshalParamsWithPerm(byte* output, byte* params, byte msgType, byte* perm) {
    if (maxParamLength > 0) memcpy(output, params, maxParamLength);
    switch (msgType) {
    case PROCESS: {
        break;
    }
    default: break;
    }
}
#endif // SYMM

	#define _RETURN_ goto _RETURN_POINT_PROCESS
	    long FilterAActor::msgsrvprocess (
    		int
	         _ref_v
	        , 
    		int
	         _ref_mode
    ) {
    			long arrayIndexChecker = 0;MergerActor *temp0;
		shift = 1;
		#ifdef SAFE_MODE
			string reactiveClassName = this->getName();
			string methodName = "process";
		#endif

	#ifdef TTS
	#endif

		{
			if ((_ref_mode==0)) {
				{
					(_ref_accepted=(_ref_accepted+1));
					(_ref_last=_ref_v);
					(temp0=_ref_mg, assertion(temp0!= null, "Null Pointer Exception in method " + reactiveClassName + "." + methodName + "line 44"), temp0)->_timed_msg_collectA(myID, _ref_v, _ref_accepted, _ref_now, MAX_TIME);
				}
}
			else {
				{
					(_ref_rejected=(_ref_rejected+1));
				}
}
			assertion((_ref_rejected<=(_ref_accepted+3)));
		}
	_RETURN_POINT_PROCESS:
		return 0;

}
