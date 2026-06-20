                                    


#include "Types.h"
#include "AnalyzerActor.h"
#include <stdio.h>
#include <string.h>
#include <limits>

#include "ProducerActor.h"
#include "FilterAActor.h"
#include "FilterBActor.h"
#include "MergerActor.h"
#include "CheckerActor.h"
#include "LoggerActor.h"
#include "SinkActor.h"

// location counters (values stored in messageQueue[0])
/* There can be more than one location counter for each msgsrv when we
   want to have non-atomic message servers.
   Notice that values 0 and 1 are reserved for EMPTY and INITIAL, respectively.*/
#define ANALYZE 1


// knownrebecs (those to whom message can be sent)
// the following format allows to use them as variables --> the ID of the rebec
#define _ref_sender (rebecs[senderQueue[0]])
#define _ref_self (dynamic_cast<AnalyzerActor*>(rebecs[(int)((unsigned char)myID)]))

    	#define _ref_ch (dynamic_cast<CheckerActor*>(rebecs[(int)((unsigned char)known[1])]))
    	#define _ref_sk (dynamic_cast<SinkActor*>(rebecs[(int)((unsigned char)known[2])]))
    
AnalyzerActor::AnalyzerActor(byte myID, char* myName, int maxQueueLength, int maxParamLength, vector<AbstractActor*>& rebecsRef
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

	
		
	
		
       		 messageNames[1] = (char*)"ANALYZE";
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

AnalyzerActor::~AnalyzerActor (){
	delete [] messageNames;
	delete [] messageQueue;
	delete [] senderQueue;
}


void AnalyzerActor::exportAMessageInXML(ostream &out, string tab, int cnt) {
        int i = 0;
        switch (messageQueue[cnt]) {
            case ANALYZE: {

            	int _ref_combined;
            
            
                	memcpy(&_ref_combined, &paramQueue[cnt] [i], (1 * INT_SIZE));
	                i += (1 * INT_SIZE);

            	int _ref_cntA;
            
            
                	memcpy(&_ref_cntA, &paramQueue[cnt] [i], (1 * INT_SIZE));
	                i += (1 * INT_SIZE);

            	int _ref_cntB;
            
            
                	memcpy(&_ref_cntB, &paramQueue[cnt] [i], (1 * INT_SIZE));
	                i += (1 * INT_SIZE);
                out << "analyze("
                	<< ((int)_ref_combined)
					<< ", "
                	<< ((int)_ref_cntA)
					<< ", "
                	<< ((int)_ref_cntB)
                << ")";
                break;
               }
    	}
}


void AnalyzerActor::exportStateVariablesInXML(ostream &out, string tab) {
	out << tab << "<statevariables>" << endl;
	exportInsideStateVariablesTagInXML(out, tab);
	out << tab << "</statevariables>" << endl;
}

void AnalyzerActor::exportInsideStateVariablesTagInXML(ostream &out, string tab){


			    	out << tab << '\t' << "<variable name=\"Analyzer.runs\" type=\"int\">" <<  
			    		((int)_ref_runs) << "</variable>" << endl;
			    	out << tab << '\t' << "<variable name=\"Analyzer.peak\" type=\"int\">" <<  
			    		((int)_ref_peak) << "</variable>" << endl;
			    	out << tab << '\t' << "<variable name=\"Analyzer.trough\" type=\"int\">" <<  
			    		((int)_ref_trough) << "</variable>" << endl;
			    	out << tab << '\t' << "<variable name=\"Analyzer.sumAll\" type=\"int\">" <<  
			    		((int)_ref_sumAll) << "</variable>" << endl;
			    	out << tab << '\t' << "<variable name=\"Analyzer.balanced\" type=\"int\">" <<  
			    		((int)_ref_balanced) << "</variable>" << endl;
}


char* AnalyzerActor::getClassName() {
    return (char*)"Analyzer";
}
void AnalyzerActor::marshalVariablesToArray(byte * array){
	int marshalIndex = 0;
    	
            
                	memcpy(&array [marshalIndex], &_ref_runs, (1 * INT_SIZE));
	                marshalIndex += (1 * INT_SIZE);
            
                	memcpy(&array [marshalIndex], &_ref_peak, (1 * INT_SIZE));
	                marshalIndex += (1 * INT_SIZE);
            
                	memcpy(&array [marshalIndex], &_ref_trough, (1 * INT_SIZE));
	                marshalIndex += (1 * INT_SIZE);
            
                	memcpy(&array [marshalIndex], &_ref_sumAll, (1 * INT_SIZE));
	                marshalIndex += (1 * INT_SIZE);
            
                	memcpy(&array [marshalIndex], &_ref_balanced, (1 * INT_SIZE));
	                marshalIndex += (1 * INT_SIZE);

    
	
}
void AnalyzerActor::marshalActorToArray(byte* array) {
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

void AnalyzerActor::unmarshalActorFromArray(byte* array) {

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

void AnalyzerActor::unmarshalVariablesFromArray(byte * array){

	int index = 0;

            
            
                	memcpy(&_ref_runs, &array [index], (1 * INT_SIZE));
	                index += (1 * INT_SIZE);

            
            
                	memcpy(&_ref_peak, &array [index], (1 * INT_SIZE));
	                index += (1 * INT_SIZE);

            
            
                	memcpy(&_ref_trough, &array [index], (1 * INT_SIZE));
	                index += (1 * INT_SIZE);

            
            
                	memcpy(&_ref_sumAll, &array [index], (1 * INT_SIZE));
	                index += (1 * INT_SIZE);

            
            
                	memcpy(&_ref_balanced, &array [index], (1 * INT_SIZE));
	                index += (1 * INT_SIZE);
    
	
}

/*#ifdef DEBUG
void AnalyzerActor::printStateContent(long stateNo) {
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
long AnalyzerActor::methodSelectorAndExecutor() {

	
    switch (messageQueue[0]) {

            case ANALYZE:
                {
                    int i = 0;

            	int _ref_combined;
            
            
                	memcpy(&_ref_combined, &paramQueue[0] [i], (1 * INT_SIZE));
	                i += (1 * INT_SIZE);

            	int _ref_cntA;
            
            
                	memcpy(&_ref_cntA, &paramQueue[0] [i], (1 * INT_SIZE));
	                i += (1 * INT_SIZE);

            	int _ref_cntB;
            
            
                	memcpy(&_ref_cntB, &paramQueue[0] [i], (1 * INT_SIZE));
	                i += (1 * INT_SIZE);
                    return msgsrvanalyze(
            _ref_combined
	        , 
            _ref_cntA
	        , 
            _ref_cntB
                    );
                }
    }
	return -1;
}

#define _RETURN_ return 0
    long AnalyzerActor::constructorAnalyzer (byte creatorId 
    ) {
    			long arrayIndexChecker = 0;
		shift = 1;
		#ifdef SAFE_MODE
			string reactiveClassName = this->getName();
			string methodName = "Analyzer";
		#endif

	#ifdef TTS
	#endif

		{
			(_ref_runs=0);
			(_ref_peak=0);
			(_ref_trough=999);
			(_ref_sumAll=0);
			(_ref_balanced=0);
		}
		shift = 0;
		queueTail = 0;
		while(messageQueue[queueTail]) {applyPolicy(false);queueTail++;}
		return 0;

	}

#define _RETURN_ return


		void AnalyzerActor::_msg_analyze (byte senderId
	        ,
    		int
	         _ref_combined
	        , 
    		int
	         _ref_cntA
	        , 
    		int
	         _ref_cntB
  	  ){
        queueTail = 0;
        enqueue(messageQueue, ANALYZE);
        enqueue(senderQueue, senderId);

        if (maxParamLength != 0) {
            int index = 0;
            byte paramsValues[maxParamLength];
            memset(paramsValues, 0, maxParamLength);
    	
            
                	memcpy(&paramsValues [index], &_ref_combined, (1 * INT_SIZE));
	                index += (1 * INT_SIZE);
            
                	memcpy(&paramsValues [index], &_ref_cntA, (1 * INT_SIZE));
	                index += (1 * INT_SIZE);
            
                	memcpy(&paramsValues [index], &_ref_cntB, (1 * INT_SIZE));
	                index += (1 * INT_SIZE);

            enqueue(paramQueue, paramsValues, maxParamLength);
        }
}









	byte** AnalyzerActor::table = (byte**)0xFFFFFFFF;

	byte** AnalyzerActor::getTable() {
		return table;
	}








	vector<TimeFrame>** AnalyzerActor::hashtableTimeExtension = (vector<TimeFrame>**)0xFFFFFFFF;
	
	vector<TimeFrame>** AnalyzerActor::getHashtableTimeExtension() {
		return hashtableTimeExtension;
	}








	    void AnalyzerActor::_timed_msg_analyze (byte senderId
	        ,
    		int
	         _ref_combined
	        , 
    		int
	         _ref_cntA
	        , 
    		int
	         _ref_cntB
, TIME_TYPE executionTime, TIME_TYPE deadline){
	    _msg_analyze (senderId
	        ,
            _ref_combined
	        , 
            _ref_cntA
	        , 
            _ref_cntB
);
	    addTimedBundles(senderId, executionTime, deadline);
		}
	void AnalyzerActor::setNow(TIME_TYPE now) {
	
		AbstractTimedActor::setNow(now);
	}	
	

#ifdef SYMM


void AnalyzerActor::marshalVariablesToArrayWithPerm(byte* array, byte* perm) {
    int _mi = 0;
        memcpy(&array[_mi], &_ref_runs, (1 * INT_SIZE));
        _mi += (1 * INT_SIZE);
        memcpy(&array[_mi], &_ref_peak, (1 * INT_SIZE));
        _mi += (1 * INT_SIZE);
        memcpy(&array[_mi], &_ref_trough, (1 * INT_SIZE));
        _mi += (1 * INT_SIZE);
        memcpy(&array[_mi], &_ref_sumAll, (1 * INT_SIZE));
        _mi += (1 * INT_SIZE);
        memcpy(&array[_mi], &_ref_balanced, (1 * INT_SIZE));
        _mi += (1 * INT_SIZE);
}

void AnalyzerActor::marshalParamsWithPerm(byte* output, byte* params, byte msgType, byte* perm) {
    if (maxParamLength > 0) memcpy(output, params, maxParamLength);
    switch (msgType) {
    case ANALYZE: {
        break;
    }
    default: break;
    }
}
#endif // SYMM

	#define _RETURN_ goto _RETURN_POINT_ANALYZE
	    long AnalyzerActor::msgsrvanalyze (
    		int
	         _ref_combined
	        , 
    		int
	         _ref_cntA
	        , 
    		int
	         _ref_cntB
    ) {
    			long arrayIndexChecker = 0;SinkActor *temp0;CheckerActor *temp1;
		shift = 1;
		#ifdef SAFE_MODE
			string reactiveClassName = this->getName();
			string methodName = "analyze";
		#endif

	#ifdef TTS
	#endif

		{
			(_ref_runs=(_ref_runs+1));
			(_ref_sumAll=(_ref_sumAll+_ref_combined));
			if ((_ref_combined>_ref_peak)) {
				{
					(_ref_peak=_ref_combined);
				}
}
			if ((_ref_combined<_ref_trough)) {
				{
					(_ref_trough=_ref_combined);
				}
}
			if ((_ref_cntA==_ref_cntB)) {
				{
					(_ref_balanced=(_ref_balanced+1));
				}
}
			(temp0=_ref_sk, assertion(temp0!= null, "Null Pointer Exception in method " + reactiveClassName + "." + methodName + "line 150"), temp0)->_timed_msg_store(myID, _ref_sumAll, _ref_now, MAX_TIME);
			(temp1=_ref_ch, assertion(temp1!= null, "Null Pointer Exception in method " + reactiveClassName + "." + methodName + "line 151"), temp1)->_timed_msg_verify(myID, _ref_peak, _ref_trough, _ref_balanced, _ref_runs, _ref_now, MAX_TIME);
			assertion((_ref_trough<=_ref_peak));
			assertion((_ref_runs<=60));
		}
	_RETURN_POINT_ANALYZE:
		return 0;

}
