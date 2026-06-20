#include "AbstractTimedRebecaAnalyzer.h"

		#define _ref_rm (dynamic_cast<ResourceManagerActor*>(rebecs[0]))
		#define _ref_am1 (dynamic_cast<AppMasterActor*>(rebecs[1]))
		#define _ref_am2 (dynamic_cast<AppMasterActor*>(rebecs[2]))
	
	

		#undef _ref_rm
		#undef _ref_am1
		#undef _ref_am2

bool operator > (const OpenBorderNode& node1, const OpenBorderNode& node2) {
    return node1.currentTime > node2.currentTime;
}

AbstractTimedRebecaAnalyzer::AbstractTimedRebecaAnalyzer(ostream &outStream, int hashmapSize) : analysisOutputStream(outStream){

	this->hashmapSize = hashmapSize;
	current.state = NULL;
}


void AbstractTimedRebecaAnalyzer::unmarshalRebecs(TimedRebecStateIndexType* node, TIME_TYPE *shift) {
	int i;
	for (i = 0; i < REBEC_COUNT; i++) {
        rebecs[i]->unmarshalActor(node[i], shift[i]);   // uncompress all states of all rebecs
    }
}

void AbstractTimedRebecaAnalyzer::marshalRebecs(TimedRebecStateIndexType* node, TIME_TYPE *shift) {
	int i;
	for (i=0; i<REBEC_COUNT; i++){
		node[i] = rebecs[i]->marshalActor(shift[i]);
	}
}

OpenBorderNode AbstractTimedRebecaAnalyzer::storeRecentlyCreatedState (byte &result,
	TIME_TYPE &shift, TimedBFSState* parent, int parentBundleIndex, int executedRebecIndex) {

    numberOfTransitions++;

	TimedBFSState temp;
    RebecsShiftTime rebecsLocalShift;
    OpenBorderNode node;
    node.stateActiveBundleNumber = 0;

    marshalRebecs(temp.rebsIndex, rebecsLocalShift.shift);

    #ifdef DEBUG_LEVEL_2
	    analysisOutputStream << "State is marshalled to these values [";
	    for (int cnt2 = 0; cnt2 < REBEC_COUNT; cnt2++) {
	    	analysisOutputStream << "<" << (long)temp.rebsIndex[cnt2].tableIndex << "," << (int)rebecsLocalShift.shift[cnt2] << ">, ";
	    }
	    analysisOutputStream << "]" << endl;
	#endif
    #ifdef SYMM
    #ifdef ON_THE_FLY
    {
        byte perm[REBEC_COUNT];
        sort(temp.rebsIndex);
        // perm[physical] = canonical position
        for (int _i = 0; _i < REBEC_COUNT; _i++) perm[order[_i]] = _i;
        {
            TimedRebecStateIndexType newRebsIndex[REBEC_COUNT];
            TIME_TYPE newShift[REBEC_COUNT];
            for (int _r = 0; _r < REBEC_COUNT; _r++) {
                int _can = perm[_r];
                TIME_TYPE sh = rebecsLocalShift.shift[_r];
                // Marshal physical rebec _r's state with sender IDs permuted,
                // then store in canonical rebec _can's hash table so that
                // later unmarshalRebecs(node[_can]) looks up the right table.
                int _sz = rebecs[_r]->getStateSize();
                byte _canonBytes[_sz];
                memset(_canonBytes, 0, _sz);
                rebecs[_r]->marshalRebecVarsToArray(_canonBytes, perm);
                long _stateNo = rebecs[_can]->storeLocalState(_canonBytes);
                // Use src=rebecs[_r] so the TimeFrame is built from _r's time
                // fields, not _can's — they diverge whenever _can != _r.
                newRebsIndex[_can] = rebecs[_can]->storeLocalState(_stateNo, sh, rebecs[_r]);
                newShift[_can] = sh;
            }
            memcpy(temp.rebsIndex, newRebsIndex, sizeof(TimedRebecStateIndexType) * REBEC_COUNT);
            memcpy(rebecsLocalShift.shift, newShift, sizeof(TIME_TYPE) * REBEC_COUNT);
        }
    }
    #endif // ON_THE_FLY
    #endif // SYMM
	result = NEW_STATE;
	if ((node.state = bfsStorage->exists(temp)) != null) {
	    #ifdef DEBUG_LEVEL_2
	        analysisOutputStream << "Untimed part of the state is found in the hash table." << endl;
	    #endif
        bool uniformShift = false;
		#ifdef DEBUG_LEVEL_2
        	analysisOutputStream <<"new state: [";
			for(int cnt = 0; cnt < REBEC_COUNT; cnt++) {
				analysisOutputStream <<(int)rebecsLocalShift.shift[cnt] << ",";
			}
			analysisOutputStream <<"]" << endl;
		#endif
		for (std::vector<RebecsShiftTime>::iterator it = node.state->shiftTimes.begin() ; (it != (node.state->shiftTimes.end())) && !uniformShift; ++it) {
            shift = 0;
            boolean first = true;
			uniformShift = true;
            #ifdef DEBUG_LEVEL_2
            	analysisOutputStream << "timed bundle " << (int)node.stateActiveBundleNumber << ": [";
				for(int cnt = 0; cnt < REBEC_COUNT; cnt++) {
					analysisOutputStream << (int)(*it).shift[cnt] << ",";
				}
				analysisOutputStream << "}" << endl;
            #endif
			for(int cnt = 0; cnt < REBEC_COUNT; cnt++) {
				if (!rebecs[cnt]->messageQueue[0])
					continue;
				if (first) {
					first = false;
					shift = rebecsLocalShift.shift[cnt] - (*it).shift[cnt];
				}
				else if (shift != rebecsLocalShift.shift[cnt] - (*it).shift[cnt]) {
					uniformShift = false;
					break;
				}
			}
			if (!uniformShift) {
				node.stateActiveBundleNumber++;
			}
		}

		if (uniformShift) {
		    #ifdef DEBUG_LEVEL_2
		        analysisOutputStream << "Uniform shift is found between this state and state \"" << node.state->stateID << "_" << (int)node.stateActiveBundleNumber << "\"."<< endl;
		    #endif
			result = ~NEW_STATE;
		} else {
		    #ifdef DEBUG_LEVEL_2
		        analysisOutputStream << "New time bundle is attached." << endl;
		    #endif
			shift = 0;
			numberOfStates++;
            numberOfTimedBundles++;
            node.state->shiftTimes.push_back(rebecsLocalShift);
	    	//node.state->stateID = numberOfStates;
        }
	}
	else {
	    #ifdef DEBUG_LEVEL_2
	        analysisOutputStream << "This state has new untimed part." << endl;
	    #endif
        shift = 0;
        node.state = bfsStorage->put(temp, parent, executedRebecIndex, rebecsLocalShift);
        numberOfStates++;
        node.state->stateID = numberOfStates;
        node.state->parents.front().timeBundleIndex = parentBundleIndex;
        numberOfTimedBundles++;
	}
	return node;
}

void AbstractTimedRebecaAnalyzer::changeOrder(int rebecId, int alternativesCounter) {
	for(int shiftQueue = alternativesCounter; shiftQueue > 0; shiftQueue--) {
		typedSwap(rebecs[rebecId]->executionTime[shiftQueue], rebecs[rebecId]->executionTime[shiftQueue - 1], TIME_TYPE)
		typedSwap(rebecs[rebecId]->deadline[shiftQueue], rebecs[rebecId]->deadline[shiftQueue - 1], TIME_TYPE)
		typedSwap(rebecs[rebecId]->messageQueue[shiftQueue], rebecs[rebecId]->messageQueue[shiftQueue - 1], byte)
		typedSwap(rebecs[rebecId]->senderQueue[shiftQueue], rebecs[rebecId]->senderQueue[shiftQueue - 1], byte)
		typedSwap(rebecs[rebecId]->paramQueue[shiftQueue], rebecs[rebecId]->paramQueue[shiftQueue - 1], byte*)
	}
}

int AbstractTimedRebecaAnalyzer::getNumberOfAlternatives(int rebecId, TIME_TYPE executionTime) {
	int numberOfAlternatives = 1;
	long baseTime = rebecs[rebecId]->executionTime[0];
	int basePriority = rebecs[rebecId]->messagePriorities[rebecs[rebecId]->messageQueue[0]];
    while((numberOfAlternatives < rebecs[rebecId]->maxQueueLength) &&
		(rebecs[rebecId]->messageQueue[numberOfAlternatives]) &&
		(rebecs[rebecId]->executionTime[numberOfAlternatives] == baseTime) &&
		(rebecs[rebecId]->messagePriorities[rebecs[rebecId]->messageQueue[numberOfAlternatives]] == basePriority)) {
    	numberOfAlternatives++;
    }
    
    return numberOfAlternatives;
}

TIME_TYPE AbstractTimedRebecaAnalyzer::prepareWithNewState() {
    current = openBorderQueue.top();
    openBorderQueue.pop();
    unmarshalRebecs(current.state->rebsIndex, 
    		current.state->shiftTimes[current.stateActiveBundleNumber].shift);
    return current.currentTime;
}

void AbstractTimedRebecaAnalyzer::instantiationPart() {
	vector<AbstractActor*>* referenceToRebecs = new vector<AbstractActor*>();
	byte maxRebecID = 0;
    	ResourceManagerActor* _ref_rm = new ResourceManagerActor(maxRebecID++, "rm", 5, 1, *referenceToRebecs 
    											#ifdef DEBUG_LEVEL_2 
    											, analysisOutputStream 
    											#endif
    											, 63
    											, hashmapSize
    											);
        rebecs.push_back(_ref_rm);
        referenceToRebecs->push_back(_ref_rm);
    	AppMasterActor* _ref_am1 = new AppMasterActor(maxRebecID++, "am1", 5, 4, *referenceToRebecs 
    											#ifdef DEBUG_LEVEL_2 
    											, analysisOutputStream 
    											#endif
    											, 34
    											, hashmapSize
    											);
        rebecs.push_back(_ref_am1);
        referenceToRebecs->push_back(_ref_am1);
    	AppMasterActor* _ref_am2 = new AppMasterActor(maxRebecID++, "am2", 5, 4, *referenceToRebecs 
    											#ifdef DEBUG_LEVEL_2 
    											, analysisOutputStream 
    											#endif
    											, 34
    											, hashmapSize
    											);
        rebecs.push_back(_ref_am2);
        referenceToRebecs->push_back(_ref_am2);

    	_ref_rm->known = new byte[MAX_KNOWN];
    	_ref_rm->known[0] = _ref_rm->myID;
 _ref_rm->known[1] = _ref_am1->myID;
 _ref_rm->known[2] = _ref_am2->myID;

		_ref_rm->constructorResourceManager(INITIAL_SENDER
		);
    	_ref_am1->known = new byte[MAX_KNOWN];
    	_ref_am1->known[0] = _ref_am1->myID;
 _ref_am1->known[1] = _ref_rm->myID;

		_ref_am1->constructorAppMaster(INITIAL_SENDER
		);
    	_ref_am2->known = new byte[MAX_KNOWN];
    	_ref_am2->known[0] = _ref_am2->myID;
 _ref_am2->known[1] = _ref_rm->myID;

		_ref_am2->constructorAppMaster(INITIAL_SENDER
		);

	numberOfStates = 0;
	numberOfTransitions = 0;
    numberOfTimedBundles = 0;

    #ifdef DEBUG
    	analysisOutputStream << "Rebecs are instanciated successfully." << endl;
    #endif

}

void AbstractTimedRebecaAnalyzer::exportState(OpenBorderNode &current, ostream& outStream) {
	outStream << "<state id=\"" << current.state->stateID << "_" << 
		(int)current.stateActiveBundleNumber << "\" ";
	outStream << "atomicpropositions=\""; 
	outStream << "\" ";
	#ifdef SIMPLIFIED_STATESPACE
		#ifdef TTS
			outStream << " now=\"" << rebecs[0]->getNow() << "\"";
		#endif 
		outStream << ">" << endl;
	#else
		outStream << ">" << endl;
		for (int cnt = 0; cnt < REBEC_COUNT; cnt++)
			rebecs[cnt]->exportStateInXML(outStream, "\t");
	#endif
	outStream << "</state>" << endl;
}

void AbstractTimedRebecaAnalyzer::exportTransition(string source, string destination, 
		string sender, string owner, string label, TIME_TYPE executionTime, TIME_TYPE shift, ostream& outStream) {
    outStream << "<transition source=\"" << source << "\" destination=\"" << 
		destination << "\" executionTime=\"" << (int)executionTime << "\" shift=\"" << (int) shift << 
    	"\"> <messageserver sender=\"" << sender << "\" owner=\"" << 
    	owner << "\" title=\"" << label << "\"/></transition>" << endl;
}


void AbstractTimedRebecaAnalyzer::exportTransition(OpenBorderNode &source, OpenBorderNode &destination,
		string sender, string owner, string label, TIME_TYPE executionTime, TIME_TYPE shift, ostream& outStream) {
		exportTransition(to_string(source.state->stateID) + "_" + to_string((long)source.stateActiveBundleNumber),
		to_string(destination.state->stateID) + "_" + to_string((long)destination.stateActiveBundleNumber),
		sender, owner, label, executionTime, shift, outStream);
		}
#ifdef SYMM

void AbstractTimedRebecaAnalyzer::sort(TimedRebecStateIndexType* sys) {
    // Initialise order[0..REBEC_COUNT-1] = 0..REBEC_COUNT-1
    for (int i = 0; i < REBEC_COUNT; i++) order[i] = i;
    // Insertion sort within same-type groups only — never cross type boundaries.
    // Rebecs with a unique type stay fixed (perm[i]=i).
    // We skip the queue bytes (messageQueue + senderQueue + paramQueue) when
    // comparing, so that physical sender IDs do not contaminate the ordering.
    // Only state-variable bytes are compared; this keeps the sort permutation-
    // neutral for self-messages and produces correct canonical forms.
    for (int i = 1; i < REBEC_COUNT; i++) {
        byte tmp = order[i];
        byte tmpType = symmTypeIds[tmp];
        int j = i;
        while (j > 0 && symmTypeIds[order[j-1]] == tmpType) {
            int ai = order[j-1];
            byte* a = rebecs[ai]->getLocalStateBytes(sys[ai].tableIndex);
            byte* b = rebecs[tmp]->getLocalStateBytes(sys[tmp].tableIndex);
            if (a == NULL || b == NULL) break;
            int sz = rebecs[ai]->getStateSize();
            // Skip queue bytes so physical sender IDs don't affect sort order.
            int queueBytes = rebecs[ai]->maxQueueLength * (2 + rebecs[ai]->maxParamLength);
            int cmpSz = sz - queueBytes;
            if (cmpSz <= 0 || memcmp(a + queueBytes, b + queueBytes, cmpSz) <= 0) break;
            order[j] = order[j-1];
            j--;
        }
        order[j] = tmp;
    }
}

bool AbstractTimedRebecaAnalyzer::isAssigned(int r, byte* perm) {
    return (unsigned char)perm[r] != 255;
}

bool AbstractTimedRebecaAnalyzer::check(int i, int j, byte* perm, TimedRebecStateIndexType* sys) {
    // Try to assign physical rebec j to canonical position i.
    // Uses a perm checkpoint so all assignments are undone atomically on failure.
    if (symmTypeIds[i] != symmTypeIds[j]) return false;
    if (isAssigned(i, perm) || isAssigned(j, perm)) return false;

    // Save checkpoint before making any assignments
    byte savedPerm[REBEC_COUNT];
    memcpy(savedPerm, perm, REBEC_COUNT);

    perm[j] = i;
    perm[i] = j;

    // Propagate through known-rebec chain
    int krCount = symmKRSize[i];
    for (int k = 0; k < krCount; k++) {
        int physKR = (int)(unsigned char)symmKRTable[j][k];
        int canKR  = (int)(unsigned char)symmKRTable[i][k];
        if (physKR == canKR) continue;
        // If already consistently assigned, continue; else try to assign
        if (isAssigned(canKR, perm) || isAssigned(physKR, perm)) {
            // Check existing assignments are consistent
            if (!isAssigned(canKR, perm) || !isAssigned(physKR, perm) ||
                perm[physKR] != canKR || perm[canKR] != physKR) {
                memcpy(perm, savedPerm, REBEC_COUNT);
                return false;
            }
            continue;
        }
        if (!check(canKR, physKR, perm, sys)) {
            memcpy(perm, savedPerm, REBEC_COUNT);
            return false;
        }
    }
    return true;
}
#endif // SYMM