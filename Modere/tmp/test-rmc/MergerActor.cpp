                                    


#include "Types.h"
#include "MergerActor.h"
#include <stdio.h>
#include <string.h>
#include <limits>

#include "ProducerActor.h"
#include "FilterAActor.h"
#include "FilterBActor.h"
#include "AnalyzerActor.h"
#include "CheckerActor.h"
#include "LoggerActor.h"
#include "SinkActor.h"

// location counters (values stored in messageQueue[0])
/* There can be more than one location counter for each msgsrv when we
   want to have non-atomic message servers.
   Notice that values 0 and 1 are reserved for EMPTY and INITIAL, respectively.*/
#define COLLECTA 1
#define COLLECTB 2


// knownrebecs (those to whom message can be sent)
// the following format allows to use them as variables --> the ID of the rebec
#define _ref_sender (rebecs[senderQueue[0]])
#define _ref_self (dynamic_cast<MergerActor*>(rebecs[(int)((unsigned char)myID)]))

    	#define _ref_az (dynamic_cast<AnalyzerActor*>(rebecs[(int)((unsigned char)known[1])]))
    	#define _ref_lg (dynamic_cast<LoggerActor*>(rebecs[(int)((unsigned char)known[2])]))
    
MergerActor::MergerActor(byte myID, char* myName, int maxQueueLength, int maxParamLength, vector<AbstractActor*>& rebecsRef
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
    messageNames = new char *[2 + 1];
    messagePriorities = new int[2 + 1];
	messageNames[0] = (char*)"Empty";

	
		
	
		
       		 messageNames[1] = (char*)"COLLECTA";
			messagePriorities[1] = std::numeric_limits<int>::max();
       		 messageNames[2] = (char*)"COLLECTB";
			messagePriorities[2] = std::numeric_limits<int>::max();








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

MergerActor::~MergerActor (){
	delete [] messageNames;
	delete [] messageQueue;
	delete [] senderQueue;
}


void MergerActor::exportAMessageInXML(ostream &out, string tab, int cnt) {
        int i = 0;
        switch (messageQueue[cnt]) {
            case COLLECTA: {

            	int _ref_v;
            
            
                	memcpy(&_ref_v, &paramQueue[cnt] [i], (1 * INT_SIZE));
	                i += (1 * INT_SIZE);

            	int _ref_weight;
            
            
                	memcpy(&_ref_weight, &paramQueue[cnt] [i], (1 * INT_SIZE));
	                i += (1 * INT_SIZE);
                out << "collectA("
                	<< ((int)_ref_v)
					<< ", "
                	<< ((int)_ref_weight)
                << ")";
                break;
               }
            case COLLECTB: {

            	int _ref_v;
            
            
                	memcpy(&_ref_v, &paramQueue[cnt] [i], (1 * INT_SIZE));
	                i += (1 * INT_SIZE);

            	int _ref_streak;
            
            
                	memcpy(&_ref_streak, &paramQueue[cnt] [i], (1 * INT_SIZE));
	                i += (1 * INT_SIZE);
                out << "collectB("
                	<< ((int)_ref_v)
					<< ", "
                	<< ((int)_ref_streak)
                << ")";
                break;
               }
    	}
}


void MergerActor::exportStateVariablesInXML(ostream &out, string tab) {
	out << tab << "<statevariables>" << endl;
	exportInsideStateVariablesTagInXML(out, tab);
	out << tab << "</statevariables>" << endl;
}

void MergerActor::exportInsideStateVariablesTagInXML(ostream &out, string tab){


			    	out << tab << '\t' << "<variable name=\"Merger.fromA\" type=\"int\">" <<  
			    		((int)_ref_fromA) << "</variable>" << endl;
			    	out << tab << '\t' << "<variable name=\"Merger.fromB\" type=\"int\">" <<  
			    		((int)_ref_fromB) << "</variable>" << endl;
			    	out << tab << '\t' << "<variable name=\"Merger.totalA\" type=\"int\">" <<  
			    		((int)_ref_totalA) << "</variable>" << endl;
			    	out << tab << '\t' << "<variable name=\"Merger.totalB\" type=\"int\">" <<  
			    		((int)_ref_totalB) << "</variable>" << endl;
			    	out << tab << '\t' << "<variable name=\"Merger.combined\" type=\"int\">" <<  
			    		((int)_ref_combined) << "</variable>" << endl;
}


char* MergerActor::getClassName() {
    return (char*)"Merger";
}
void MergerActor::marshalVariablesToArray(byte * array){
	int marshalIndex = 0;
    	
            
                	memcpy(&array [marshalIndex], &_ref_fromA, (1 * INT_SIZE));
	                marshalIndex += (1 * INT_SIZE);
            
                	memcpy(&array [marshalIndex], &_ref_fromB, (1 * INT_SIZE));
	                marshalIndex += (1 * INT_SIZE);
            
                	memcpy(&array [marshalIndex], &_ref_totalA, (1 * INT_SIZE));
	                marshalIndex += (1 * INT_SIZE);
            
                	memcpy(&array [marshalIndex], &_ref_totalB, (1 * INT_SIZE));
	                marshalIndex += (1 * INT_SIZE);
            
                	memcpy(&array [marshalIndex], &_ref_combined, (1 * INT_SIZE));
	                marshalIndex += (1 * INT_SIZE);

    
	
}
void MergerActor::marshalActorToArray(byte* array) {
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

void MergerActor::unmarshalActorFromArray(byte* array) {

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

void MergerActor::unmarshalVariablesFromArray(byte * array){

	int index = 0;

            
            
                	memcpy(&_ref_fromA, &array [index], (1 * INT_SIZE));
	                index += (1 * INT_SIZE);

            
            
                	memcpy(&_ref_fromB, &array [index], (1 * INT_SIZE));
	                index += (1 * INT_SIZE);

            
            
                	memcpy(&_ref_totalA, &array [index], (1 * INT_SIZE));
	                index += (1 * INT_SIZE);

            
            
                	memcpy(&_ref_totalB, &array [index], (1 * INT_SIZE));
	                index += (1 * INT_SIZE);

            
            
                	memcpy(&_ref_combined, &array [index], (1 * INT_SIZE));
	                index += (1 * INT_SIZE);
    
	
}

/*#ifdef DEBUG
void MergerActor::printStateContent(long stateNo) {
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
long MergerActor::methodSelectorAndExecutor() {

	
    switch (messageQueue[0]) {

            case COLLECTA:
                {
                    int i = 0;

            	int _ref_v;
            
            
                	memcpy(&_ref_v, &paramQueue[0] [i], (1 * INT_SIZE));
	                i += (1 * INT_SIZE);

            	int _ref_weight;
            
            
                	memcpy(&_ref_weight, &paramQueue[0] [i], (1 * INT_SIZE));
	                i += (1 * INT_SIZE);
                    return msgsrvcollectA(
            _ref_v
	        , 
            _ref_weight
                    );
                }
            case COLLECTB:
                {
                    int i = 0;

            	int _ref_v;
            
            
                	memcpy(&_ref_v, &paramQueue[0] [i], (1 * INT_SIZE));
	                i += (1 * INT_SIZE);

            	int _ref_streak;
            
            
                	memcpy(&_ref_streak, &paramQueue[0] [i], (1 * INT_SIZE));
	                i += (1 * INT_SIZE);
                    return msgsrvcollectB(
            _ref_v
	        , 
            _ref_streak
                    );
                }
    }
	return -1;
}

#define _RETURN_ return 0
    long MergerActor::constructorMerger (byte creatorId 
    ) {
    			long arrayIndexChecker = 0;
		shift = 1;
		#ifdef SAFE_MODE
			string reactiveClassName = this->getName();
			string methodName = "Merger";
		#endif

	#ifdef TTS
	#endif

		{
			(_ref_fromA=0);
			(_ref_fromB=0);
			(_ref_totalA=0);
			(_ref_totalB=0);
			(_ref_combined=0);
		}
		shift = 0;
		queueTail = 0;
		while(messageQueue[queueTail]) {applyPolicy(false);queueTail++;}
		return 0;

	}

#define _RETURN_ return


		void MergerActor::_msg_collectA (byte senderId
	        ,
    		int
	         _ref_v
	        , 
    		int
	         _ref_weight
  	  ){
        queueTail = 0;
        enqueue(messageQueue, COLLECTA);
        enqueue(senderQueue, senderId);

        if (maxParamLength != 0) {
            int index = 0;
            byte paramsValues[maxParamLength];
            memset(paramsValues, 0, maxParamLength);
    	
            
                	memcpy(&paramsValues [index], &_ref_v, (1 * INT_SIZE));
	                index += (1 * INT_SIZE);
            
                	memcpy(&paramsValues [index], &_ref_weight, (1 * INT_SIZE));
	                index += (1 * INT_SIZE);

            enqueue(paramQueue, paramsValues, maxParamLength);
        }
}

		void MergerActor::_msg_collectB (byte senderId
	        ,
    		int
	         _ref_v
	        , 
    		int
	         _ref_streak
  	  ){
        queueTail = 0;
        enqueue(messageQueue, COLLECTB);
        enqueue(senderQueue, senderId);

        if (maxParamLength != 0) {
            int index = 0;
            byte paramsValues[maxParamLength];
            memset(paramsValues, 0, maxParamLength);
    	
            
                	memcpy(&paramsValues [index], &_ref_v, (1 * INT_SIZE));
	                index += (1 * INT_SIZE);
            
                	memcpy(&paramsValues [index], &_ref_streak, (1 * INT_SIZE));
	                index += (1 * INT_SIZE);

            enqueue(paramQueue, paramsValues, maxParamLength);
        }
}









	byte** MergerActor::table = (byte**)0xFFFFFFFF;

	byte** MergerActor::getTable() {
		return table;
	}








	vector<TimeFrame>** MergerActor::hashtableTimeExtension = (vector<TimeFrame>**)0xFFFFFFFF;
	
	vector<TimeFrame>** MergerActor::getHashtableTimeExtension() {
		return hashtableTimeExtension;
	}








	    void MergerActor::_timed_msg_collectA (byte senderId
	        ,
    		int
	         _ref_v
	        , 
    		int
	         _ref_weight
, TIME_TYPE executionTime, TIME_TYPE deadline){
	    _msg_collectA (senderId
	        ,
            _ref_v
	        , 
            _ref_weight
);
	    addTimedBundles(senderId, executionTime, deadline);
		}
	    void MergerActor::_timed_msg_collectB (byte senderId
	        ,
    		int
	         _ref_v
	        , 
    		int
	         _ref_streak
, TIME_TYPE executionTime, TIME_TYPE deadline){
	    _msg_collectB (senderId
	        ,
            _ref_v
	        , 
            _ref_streak
);
	    addTimedBundles(senderId, executionTime, deadline);
		}
	void MergerActor::setNow(TIME_TYPE now) {
	
		AbstractTimedActor::setNow(now);
	}	
	

#ifdef SYMM


void MergerActor::marshalVariablesToArrayWithPerm(byte* array, byte* perm) {
    int _mi = 0;
        memcpy(&array[_mi], &_ref_fromA, (1 * INT_SIZE));
        _mi += (1 * INT_SIZE);
        memcpy(&array[_mi], &_ref_fromB, (1 * INT_SIZE));
        _mi += (1 * INT_SIZE);
        memcpy(&array[_mi], &_ref_totalA, (1 * INT_SIZE));
        _mi += (1 * INT_SIZE);
        memcpy(&array[_mi], &_ref_totalB, (1 * INT_SIZE));
        _mi += (1 * INT_SIZE);
        memcpy(&array[_mi], &_ref_combined, (1 * INT_SIZE));
        _mi += (1 * INT_SIZE);
}

void MergerActor::marshalParamsWithPerm(byte* output, byte* params, byte msgType, byte* perm) {
    if (maxParamLength > 0) memcpy(output, params, maxParamLength);
    switch (msgType) {
    case COLLECTA: {
        break;
    }
    case COLLECTB: {
        break;
    }
    default: break;
    }
}
#endif // SYMM

	#define _RETURN_ goto _RETURN_POINT_COLLECTA
	    long MergerActor::msgsrvcollectA (
    		int
	         _ref_v
	        , 
    		int
	         _ref_weight
    ) {
    			long arrayIndexChecker = 0;LoggerActor *temp0;AnalyzerActor *temp1;
		shift = 1;
		#ifdef SAFE_MODE
			string reactiveClassName = this->getName();
			string methodName = "collectA";
		#endif

	#ifdef TTS
	#endif

		{
			(_ref_fromA=(_ref_fromA+1));
			(_ref_totalA=(_ref_totalA+_ref_v));
			(_ref_combined=(_ref_combined+(_ref_v*_ref_weight)));
			(temp0=_ref_lg, assertion(temp0!= null, "Null Pointer Exception in method " + reactiveClassName + "." + methodName + "line 105"), temp0)->_timed_msg_logIt(myID, _ref_totalA, _ref_now, MAX_TIME);
			(temp1=_ref_az, assertion(temp1!= null, "Null Pointer Exception in method " + reactiveClassName + "." + methodName + "line 106"), temp1)->_timed_msg_analyze(myID, _ref_combined, _ref_fromA, _ref_fromB, _ref_now, MAX_TIME);
			assertion((_ref_totalA<=(_ref_totalB+50)));
		}
	_RETURN_POINT_COLLECTA:
		return 0;

}
	#define _RETURN_ goto _RETURN_POINT_COLLECTB
	    long MergerActor::msgsrvcollectB (
    		int
	         _ref_v
	        , 
    		int
	         _ref_streak
    ) {
    			long arrayIndexChecker = 0;LoggerActor *temp0;AnalyzerActor *temp1;
		shift = 1;
		#ifdef SAFE_MODE
			string reactiveClassName = this->getName();
			string methodName = "collectB";
		#endif

	#ifdef TTS
	#endif

		{
			(_ref_fromB=(_ref_fromB+1));
			(_ref_totalB=(_ref_totalB+_ref_v));
			(_ref_combined=(_ref_combined+_ref_v));
			(temp0=_ref_lg, assertion(temp0!= null, "Null Pointer Exception in method " + reactiveClassName + "." + methodName + "line 113"), temp0)->_timed_msg_logIt(myID, _ref_totalB, _ref_now, MAX_TIME);
			(temp1=_ref_az, assertion(temp1!= null, "Null Pointer Exception in method " + reactiveClassName + "." + methodName + "line 114"), temp1)->_timed_msg_analyze(myID, _ref_combined, _ref_fromA, _ref_fromB, _ref_now, MAX_TIME);
			assertion((_ref_totalB<=(_ref_totalA+50)));
		}
	_RETURN_POINT_COLLECTB:
		return 0;

}
