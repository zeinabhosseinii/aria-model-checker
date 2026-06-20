                                    


#include "Types.h"
#include "ResourceManagerActor.h"
#include <stdio.h>
#include <string.h>
#include <limits>

#include "AppMasterActor.h"

// location counters (values stored in messageQueue[0])
/* There can be more than one location counter for each msgsrv when we
   want to have non-atomic message servers.
   Notice that values 0 and 1 are reserved for EMPTY and INITIAL, respectively.*/
#define CHECKQUEUE 1
#define UPDATE 2


// knownrebecs (those to whom message can be sent)
// the following format allows to use them as variables --> the ID of the rebec
#define _ref_sender (rebecs[senderQueue[0]])
#define _ref_self (dynamic_cast<ResourceManagerActor*>(rebecs[(int)((unsigned char)myID)]))

    	#define _ref_am1 (dynamic_cast<AppMasterActor*>(rebecs[(int)((unsigned char)known[1])]))
    	#define _ref_am2 (dynamic_cast<AppMasterActor*>(rebecs[(int)((unsigned char)known[2])]))
    
ResourceManagerActor::ResourceManagerActor(byte myID, char* myName, int maxQueueLength, int maxParamLength, vector<AbstractActor*>& rebecsRef
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

	
		
	
		
       		 messageNames[1] = (char*)"CHECKQUEUE";
			messagePriorities[1] = std::numeric_limits<int>::max();
       		 messageNames[2] = (char*)"UPDATE";
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

ResourceManagerActor::~ResourceManagerActor (){
	delete [] messageNames;
	delete [] messageQueue;
	delete [] senderQueue;
}


void ResourceManagerActor::exportAMessageInXML(ostream &out, string tab, int cnt) {
        int i = 0;
        switch (messageQueue[cnt]) {
            case CHECKQUEUE: {
                out << "checkQueue("
                << ")";
                break;
               }
            case UPDATE: {

            	boolean _ref_deadline_miss;
            
            
_ref_deadline_miss = (bool) (paramQueue[cnt] [i] & AbstractActor::coeff[0]);
++i;                out << "update("
                	<< (_ref_deadline_miss? "true" : "false")
                << ")";
                break;
               }
    	}
}


void ResourceManagerActor::exportStateVariablesInXML(ostream &out, string tab) {
	out << tab << "<statevariables>" << endl;
	exportInsideStateVariablesTagInXML(out, tab);
	out << tab << "</statevariables>" << endl;
}

void ResourceManagerActor::exportInsideStateVariablesTagInXML(ostream &out, string tab){


			    	out << tab << '\t' << "<variable name=\"ResourceManager.FREE\" type=\"int\">" <<  
			    		((int)_ref_FREE) << "</variable>" << endl;
			    	out << tab << '\t' << "<variable name=\"ResourceManager.BUSY\" type=\"int\">" <<  
			    		((int)_ref_BUSY) << "</variable>" << endl;
			    	out << tab << '\t' << "<variable name=\"ResourceManager.appMaster1\" type=\"int\">" <<  
			    		((int)_ref_appMaster1) << "</variable>" << endl;
			    	out << tab << '\t' << "<variable name=\"ResourceManager.appMaster2\" type=\"int\">" <<  
			    		((int)_ref_appMaster2) << "</variable>" << endl;
			    	out << tab << '\t' << "<variable name=\"ResourceManager.m_queue_misses\" type=\"int\">" <<  
			    		((int)_ref_m_queue_misses) << "</variable>" << endl;
			    	out << tab << '\t' << "<variable name=\"ResourceManager.m_update_miss\" type=\"int\">" <<  
			    		((int)_ref_m_update_miss) << "</variable>" << endl;
			    	out << tab << '\t' << "<variable name=\"ResourceManager.m_job_complete\" type=\"int\">" <<  
			    		((int)_ref_m_job_complete) << "</variable>" << endl;
			    	out << tab << '\t' << "<variable name=\"ResourceManager.fifo_queue\" type=\"int[3]\">" <<  
			    		"[" << ((int)_ref_fifo_queue[0]) << ", " << ((int)_ref_fifo_queue[1]) << ", " << ((int)_ref_fifo_queue[2]) << ", " << "]" << "</variable>" << endl;
			    	out << tab << '\t' << "<variable name=\"ResourceManager.DEFAULT_DEADLINE\" type=\"int\">" <<  
			    		((int)_ref_DEFAULT_DEADLINE) << "</variable>" << endl;
			    	out << tab << '\t' << "<variable name=\"ResourceManager.QUEUE_SIZE\" type=\"int\">" <<  
			    		((int)_ref_QUEUE_SIZE) << "</variable>" << endl;
}


char* ResourceManagerActor::getClassName() {
    return (char*)"ResourceManager";
}
void ResourceManagerActor::marshalVariablesToArray(byte * array){
	int marshalIndex = 0;
    	
            
                	memcpy(&array [marshalIndex], &_ref_FREE, (1 * INT_SIZE));
	                marshalIndex += (1 * INT_SIZE);
            
                	memcpy(&array [marshalIndex], &_ref_BUSY, (1 * INT_SIZE));
	                marshalIndex += (1 * INT_SIZE);
            
                	memcpy(&array [marshalIndex], &_ref_appMaster1, (1 * INT_SIZE));
	                marshalIndex += (1 * INT_SIZE);
            
                	memcpy(&array [marshalIndex], &_ref_appMaster2, (1 * INT_SIZE));
	                marshalIndex += (1 * INT_SIZE);
            
                	memcpy(&array [marshalIndex], &_ref_m_queue_misses, (1 * INT_SIZE));
	                marshalIndex += (1 * INT_SIZE);
            
                	memcpy(&array [marshalIndex], &_ref_m_update_miss, (1 * INT_SIZE));
	                marshalIndex += (1 * INT_SIZE);
            
                	memcpy(&array [marshalIndex], &_ref_m_job_complete, (1 * INT_SIZE));
	                marshalIndex += (1 * INT_SIZE);
            
					memcpy(&array [marshalIndex], _ref_fifo_queue.data(), (1 * INT_SIZE) * 3);					
					marshalIndex += (1 * INT_SIZE) * 3;
            
                	memcpy(&array [marshalIndex], &_ref_DEFAULT_DEADLINE, (1 * INT_SIZE));
	                marshalIndex += (1 * INT_SIZE);
            
                	memcpy(&array [marshalIndex], &_ref_QUEUE_SIZE, (1 * INT_SIZE));
	                marshalIndex += (1 * INT_SIZE);

    
	
}
void ResourceManagerActor::marshalActorToArray(byte* array) {
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

void ResourceManagerActor::unmarshalActorFromArray(byte* array) {

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

void ResourceManagerActor::unmarshalVariablesFromArray(byte * array){

	int index = 0;

            
            
                	memcpy(&_ref_FREE, &array [index], (1 * INT_SIZE));
	                index += (1 * INT_SIZE);

            
            
                	memcpy(&_ref_BUSY, &array [index], (1 * INT_SIZE));
	                index += (1 * INT_SIZE);

            
            
                	memcpy(&_ref_appMaster1, &array [index], (1 * INT_SIZE));
	                index += (1 * INT_SIZE);

            
            
                	memcpy(&_ref_appMaster2, &array [index], (1 * INT_SIZE));
	                index += (1 * INT_SIZE);

            
            
                	memcpy(&_ref_m_queue_misses, &array [index], (1 * INT_SIZE));
	                index += (1 * INT_SIZE);

            
            
                	memcpy(&_ref_m_update_miss, &array [index], (1 * INT_SIZE));
	                index += (1 * INT_SIZE);

            
            
                	memcpy(&_ref_m_job_complete, &array [index], (1 * INT_SIZE));
	                index += (1 * INT_SIZE);

            
            
					memcpy(_ref_fifo_queue.data(), &array [index], (1 * INT_SIZE) * 3);					
					index += (1 * INT_SIZE) * 3;

            
            
                	memcpy(&_ref_DEFAULT_DEADLINE, &array [index], (1 * INT_SIZE));
	                index += (1 * INT_SIZE);

            
            
                	memcpy(&_ref_QUEUE_SIZE, &array [index], (1 * INT_SIZE));
	                index += (1 * INT_SIZE);
    
	
}

/*#ifdef DEBUG
void ResourceManagerActor::printStateContent(long stateNo) {
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
long ResourceManagerActor::methodSelectorAndExecutor() {

	
    switch (messageQueue[0]) {

            case CHECKQUEUE:
                {
                    int i = 0;
                    return msgsrvcheckQueue(
                    );
                }
            case UPDATE:
                {
                    int i = 0;

            	boolean _ref_deadline_miss;
            
            
_ref_deadline_miss = (bool) (paramQueue[0] [i] & AbstractActor::coeff[0]);
++i;                    return msgsrvupdate(
            _ref_deadline_miss
                    );
                }
    }
	return -1;
}

#define _RETURN_ return 0
    long ResourceManagerActor::constructorResourceManager (byte creatorId 
    ) {
    			long arrayIndexChecker = 0;ResourceManagerActor *temp0;
		shift = 1;
		#ifdef SAFE_MODE
			string reactiveClassName = this->getName();
			string methodName = "ResourceManager";
		#endif

	#ifdef TTS
	#endif

		{
			(_ref_FREE=1);
			(_ref_BUSY=0);
			(_ref_appMaster1=_ref_FREE);
			(_ref_appMaster2=_ref_FREE);
			(_ref_m_queue_misses=0);
			(_ref_m_update_miss=0);
			(_ref_m_job_complete=0);
			(_ref_DEFAULT_DEADLINE=3);
			(_ref_fifo_queue[(arrayIndexChecker=0, assertion(arrayIndexChecker >= 0 && arrayIndexChecker <3, string("Array index out of bound: ") + to_string((long long)arrayIndexChecker) + ", method \"" + reactiveClassName + "." + methodName + "\" line 33") , arrayIndexChecker)]=_ref_DEFAULT_DEADLINE);
			(_ref_fifo_queue[(arrayIndexChecker=1, assertion(arrayIndexChecker >= 0 && arrayIndexChecker <3, string("Array index out of bound: ") + to_string((long long)arrayIndexChecker) + ", method \"" + reactiveClassName + "." + methodName + "\" line 34") , arrayIndexChecker)]=_ref_DEFAULT_DEADLINE);
			(_ref_fifo_queue[(arrayIndexChecker=2, assertion(arrayIndexChecker >= 0 && arrayIndexChecker <3, string("Array index out of bound: ") + to_string((long long)arrayIndexChecker) + ", method \"" + reactiveClassName + "." + methodName + "\" line 35") , arrayIndexChecker)]=_ref_DEFAULT_DEADLINE);
			(_ref_QUEUE_SIZE=3);
			(temp0=_ref_self, assertion(temp0!= null, "Null Pointer Exception in method " + reactiveClassName + "." + methodName + "line 37"), temp0)->_timed_msg_checkQueue(myID, _ref_now, MAX_TIME);
		}
		shift = 0;
		queueTail = 0;
		while(messageQueue[queueTail]) {applyPolicy(false);queueTail++;}
		return 0;

	}

#define _RETURN_ return


		void ResourceManagerActor::_msg_checkQueue (byte senderId
  	  ){
        queueTail = 0;
        enqueue(messageQueue, CHECKQUEUE);
        enqueue(senderQueue, senderId);

        if (maxParamLength != 0) {
            int index = 0;
            byte paramsValues[maxParamLength];
            memset(paramsValues, 0, maxParamLength);
            enqueue(paramQueue, paramsValues, maxParamLength);
        }
}

		void ResourceManagerActor::_msg_update (byte senderId
	        ,
    		boolean
	         _ref_deadline_miss
  	  ){
        queueTail = 0;
        enqueue(messageQueue, UPDATE);
        enqueue(senderQueue, senderId);

        if (maxParamLength != 0) {
            int index = 0;
            byte paramsValues[maxParamLength];
            memset(paramsValues, 0, maxParamLength);
    	
            

paramsValues [index] = 0;
paramsValues [index] |= (_ref_deadline_miss ? 0xFF : 0x00) & AbstractActor::coeff[0]; 
++index;            enqueue(paramQueue, paramsValues, maxParamLength);
        }
}









	byte** ResourceManagerActor::table = (byte**)0xFFFFFFFF;

	byte** ResourceManagerActor::getTable() {
		return table;
	}








	vector<TimeFrame>** ResourceManagerActor::hashtableTimeExtension = (vector<TimeFrame>**)0xFFFFFFFF;
	
	vector<TimeFrame>** ResourceManagerActor::getHashtableTimeExtension() {
		return hashtableTimeExtension;
	}








	    void ResourceManagerActor::_timed_msg_checkQueue (byte senderId
, TIME_TYPE executionTime, TIME_TYPE deadline){
	    _msg_checkQueue (senderId
);
	    addTimedBundles(senderId, executionTime, deadline);
		}
	    void ResourceManagerActor::_timed_msg_update (byte senderId
	        ,
    		boolean
	         _ref_deadline_miss
, TIME_TYPE executionTime, TIME_TYPE deadline){
	    _msg_update (senderId
	        ,
            _ref_deadline_miss
);
	    addTimedBundles(senderId, executionTime, deadline);
		}
	void ResourceManagerActor::setNow(TIME_TYPE now) {
	
		AbstractTimedActor::setNow(now);
	}	
	

#ifdef SYMM


void ResourceManagerActor::marshalVariablesToArrayWithPerm(byte* array, byte* perm) {
    int _mi = 0;
        memcpy(&array[_mi], &_ref_FREE, (1 * INT_SIZE));
        _mi += (1 * INT_SIZE);
        memcpy(&array[_mi], &_ref_BUSY, (1 * INT_SIZE));
        _mi += (1 * INT_SIZE);
        memcpy(&array[_mi], &_ref_appMaster1, (1 * INT_SIZE));
        _mi += (1 * INT_SIZE);
        memcpy(&array[_mi], &_ref_appMaster2, (1 * INT_SIZE));
        _mi += (1 * INT_SIZE);
        memcpy(&array[_mi], &_ref_m_queue_misses, (1 * INT_SIZE));
        _mi += (1 * INT_SIZE);
        memcpy(&array[_mi], &_ref_m_update_miss, (1 * INT_SIZE));
        _mi += (1 * INT_SIZE);
        memcpy(&array[_mi], &_ref_m_job_complete, (1 * INT_SIZE));
        _mi += (1 * INT_SIZE);
        memcpy(&array[_mi], &_ref_fifo_queue, (3 * INT_SIZE));
        _mi += (3 * INT_SIZE);
        memcpy(&array[_mi], &_ref_DEFAULT_DEADLINE, (1 * INT_SIZE));
        _mi += (1 * INT_SIZE);
        memcpy(&array[_mi], &_ref_QUEUE_SIZE, (1 * INT_SIZE));
        _mi += (1 * INT_SIZE);
}

void ResourceManagerActor::marshalParamsWithPerm(byte* output, byte* params, byte msgType, byte* perm) {
    if (maxParamLength > 0) memcpy(output, params, maxParamLength);
    switch (msgType) {
    case CHECKQUEUE: {
        break;
    }
    case UPDATE: {
        break;
    }
    default: break;
    }
}
#endif // SYMM

	#define _RETURN_ goto _RETURN_POINT_CHECKQUEUE
	    long ResourceManagerActor::msgsrvcheckQueue (
    ) {
    			long arrayIndexChecker = 0;AppMasterActor *temp0;AppMasterActor *temp1;ResourceManagerActor *temp2;
		shift = 1;
		#ifdef SAFE_MODE
			string reactiveClassName = this->getName();
			string methodName = "checkQueue";
		#endif

	#ifdef TTS
	#endif

		{
			(_ref_m_queue_misses=0);
			(_ref_m_update_miss=0);
			(_ref_m_job_complete=0);
			int _ref_I = 0;

			if ((_ref_appMaster1==_ref_FREE)) {
				{
					(_ref_appMaster1=_ref_BUSY);
					(temp0=_ref_am1, assertion(temp0!= null, "Null Pointer Exception in method " + reactiveClassName + "." + methodName + "line 52"), temp0)->_timed_msg_runJob(myID, _ref_fifo_queue[(arrayIndexChecker=0, assertion(arrayIndexChecker >= 0 && arrayIndexChecker <3, string("Array index out of bound: ") + to_string((long long)arrayIndexChecker) + ", method \"" + reactiveClassName + "." + methodName + "\" line 52") , arrayIndexChecker)], _ref_now, MAX_TIME);
					(_ref_I=0);
					while ((_ref_I<(_ref_QUEUE_SIZE-1))) {
						{
							(_ref_fifo_queue[(arrayIndexChecker=_ref_I, assertion(arrayIndexChecker >= 0 && arrayIndexChecker <3, string("Array index out of bound: ") + to_string((long long)arrayIndexChecker) + ", method \"" + reactiveClassName + "." + methodName + "\" line 56") , arrayIndexChecker)]=_ref_fifo_queue[(arrayIndexChecker=(_ref_I+1), assertion(arrayIndexChecker >= 0 && arrayIndexChecker <3, string("Array index out of bound: ") + to_string((long long)arrayIndexChecker) + ", method \"" + reactiveClassName + "." + methodName + "\" line 56") , arrayIndexChecker)]);
							(_ref_I++);
						}
}
					(_ref_fifo_queue[(arrayIndexChecker=(_ref_QUEUE_SIZE-1), assertion(arrayIndexChecker >= 0 && arrayIndexChecker <3, string("Array index out of bound: ") + to_string((long long)arrayIndexChecker) + ", method \"" + reactiveClassName + "." + methodName + "\" line 59") , arrayIndexChecker)]=_ref_DEFAULT_DEADLINE);
				}
}
			if ((_ref_appMaster2==_ref_FREE)) {
				{
					(_ref_appMaster2=_ref_BUSY);
					(temp1=_ref_am2, assertion(temp1!= null, "Null Pointer Exception in method " + reactiveClassName + "." + methodName + "line 64"), temp1)->_timed_msg_runJob(myID, _ref_fifo_queue[(arrayIndexChecker=0, assertion(arrayIndexChecker >= 0 && arrayIndexChecker <3, string("Array index out of bound: ") + to_string((long long)arrayIndexChecker) + ", method \"" + reactiveClassName + "." + methodName + "\" line 64") , arrayIndexChecker)], _ref_now, MAX_TIME);
					(_ref_I=0);
					while ((_ref_I<(_ref_QUEUE_SIZE-1))) {
						{
							(_ref_fifo_queue[(arrayIndexChecker=_ref_I, assertion(arrayIndexChecker >= 0 && arrayIndexChecker <3, string("Array index out of bound: ") + to_string((long long)arrayIndexChecker) + ", method \"" + reactiveClassName + "." + methodName + "\" line 68") , arrayIndexChecker)]=_ref_fifo_queue[(arrayIndexChecker=(_ref_I+1), assertion(arrayIndexChecker >= 0 && arrayIndexChecker <3, string("Array index out of bound: ") + to_string((long long)arrayIndexChecker) + ", method \"" + reactiveClassName + "." + methodName + "\" line 68") , arrayIndexChecker)]);
							(_ref_I++);
						}
}
					(_ref_fifo_queue[(arrayIndexChecker=(_ref_QUEUE_SIZE-1), assertion(arrayIndexChecker >= 0 && arrayIndexChecker <3, string("Array index out of bound: ") + to_string((long long)arrayIndexChecker) + ", method \"" + reactiveClassName + "." + methodName + "\" line 71") , arrayIndexChecker)]=_ref_DEFAULT_DEADLINE);
				}
}
			(_ref_I=0);
			int _ref_J = 0;

			while ((_ref_I<_ref_QUEUE_SIZE)) {
				{
					(_ref_fifo_queue[(arrayIndexChecker=_ref_I, assertion(arrayIndexChecker >= 0 && arrayIndexChecker <3, string("Array index out of bound: ") + to_string((long long)arrayIndexChecker) + ", method \"" + reactiveClassName + "." + methodName + "\" line 77") , arrayIndexChecker)]--);
					if ((_ref_fifo_queue[(arrayIndexChecker=_ref_I, assertion(arrayIndexChecker >= 0 && arrayIndexChecker <3, string("Array index out of bound: ") + to_string((long long)arrayIndexChecker) + ", method \"" + reactiveClassName + "." + methodName + "\" line 78") , arrayIndexChecker)]==0)) {
						{
							(_ref_m_queue_misses++);
							(_ref_J=_ref_I);
							while ((_ref_J<(_ref_QUEUE_SIZE-1))) {
								{
									(_ref_fifo_queue[(arrayIndexChecker=_ref_J, assertion(arrayIndexChecker >= 0 && arrayIndexChecker <3, string("Array index out of bound: ") + to_string((long long)arrayIndexChecker) + ", method \"" + reactiveClassName + "." + methodName + "\" line 84") , arrayIndexChecker)]=_ref_fifo_queue[(arrayIndexChecker=(_ref_J+1), assertion(arrayIndexChecker >= 0 && arrayIndexChecker <3, string("Array index out of bound: ") + to_string((long long)arrayIndexChecker) + ", method \"" + reactiveClassName + "." + methodName + "\" line 84") , arrayIndexChecker)]);
									(_ref_J++);
								}
}
							(_ref_fifo_queue[(arrayIndexChecker=(_ref_QUEUE_SIZE-1), assertion(arrayIndexChecker >= 0 && arrayIndexChecker <3, string("Array index out of bound: ") + to_string((long long)arrayIndexChecker) + ", method \"" + reactiveClassName + "." + methodName + "\" line 87") , arrayIndexChecker)]=_ref_DEFAULT_DEADLINE);
						}
}
					(_ref_I++);
				}
}
			(temp2=_ref_self, assertion(temp2!= null, "Null Pointer Exception in method " + reactiveClassName + "." + methodName + "line 91"), temp2)->_timed_msg_checkQueue(myID, _ref_now, MAX_TIME);
		}
	_RETURN_POINT_CHECKQUEUE:
		return 0;

}
	#define _RETURN_ goto _RETURN_POINT_UPDATE
	    long ResourceManagerActor::msgsrvupdate (
    		boolean
	         _ref_deadline_miss
    ) {
    			long arrayIndexChecker = 0;
		shift = 1;
		#ifdef SAFE_MODE
			string reactiveClassName = this->getName();
			string methodName = "update";
		#endif

	#ifdef TTS
	#endif

		{
			(_ref_m_queue_misses=0);
			(_ref_m_update_miss=0);
			(_ref_m_job_complete=0);
			if ((_ref_deadline_miss==true)) {
				{
					(_ref_m_update_miss=1);
				}
}
			else {
				{
					(_ref_m_job_complete=1);
				}
}
			if ((_ref_sender==_ref_am1)) {
				{
					(_ref_appMaster1=_ref_FREE);
				}
}
			else {
				if ((_ref_sender==_ref_am2)) {
					{
						(_ref_appMaster2=_ref_FREE);
					}
}}
		}
	_RETURN_POINT_UPDATE:
		return 0;

}
