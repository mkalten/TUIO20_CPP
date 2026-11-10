/*
 TUIO2 C++ Library
 Copyright (c) 2009-2026 Martin Kaltenbrunner <martin@tuio.org>
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 3.0 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library.
*/

#ifndef INCLUDED_TUIOOBJECT_H
#define INCLUDED_TUIOOBJECT_H

#include "TuioTime.h"
#include "TuioToken.h"
#include "TuioPointer.h"
#include "TuioBounds.h"
#include "TuioToken3D.h"
#include "TuioPointer3D.h"
#include "TuioBounds3D.h"
#include "TuioSymbol.h"
#include "TuioGeometry.h"
#include "TuioControl.h"
#include "TuioData.h"
#include "TuioSignal.h"
#include "TuioAssociation.h"
#include "TuioContainer.h"
#include "TuioLink.h"
#include "TuioList.h"
#include "TuioTree.h"
#include "TuioSource.h"

namespace TUIO2 {
	
	/**
	 * The TuioObject class encapsulates all Tuio objects that share the same Session ID.
	 *
	 * @author Martin Kaltenbrunner
	 * @version 2.0.b1
	 */
    
	class LIBDECL TuioObject {
		
	protected:
		/**
		 * The shared session ID
		 */
		unsigned int session_id;
        /**
         * The associated TuioSource
         */
        TuioSource source;
        /**
         * the associated TuioToken
         */
        TuioToken *token;
        /**
         * the associated TuioPointer
         */
        TuioPointer *pointer;
        /**
         * the associated TuioBounds
         */
        TuioBounds *bounds;
        /**
         * the associated TuioSymbol
         */
        TuioSymbol *symbol;
        /**
         * the associated TuioToken3D
         */
        TuioToken3D *token3d;
        /**
         * the associated TuioPointer3D
         */
        TuioPointer3D *pointer3d;
        /**
         * the associated TuioBounds3D
         */
        TuioBounds3D *bounds3d;
        /**
         * the associated TuioGeometry
         */
        TuioGeometry *geometry;
        /**
         * the associated TuioControl
         */
        TuioControl *control;
        /**
         * the associated TuioData
         */
        TuioData *data;
        /**
         * the associated TuioSignal
         */
        TuioSignal *signal;
        /**
         * the associated TuioAssociations
         */
        std::list<TuioAssociation*> associations;
        /**
         * The time stamp of the last update represented as TuioTime (time since session start)
         */
        TuioTime currentTime;
		/**
		 * The creation time of this TuioObject represented as TuioTime (time since session start)
		 */
		TuioTime startTime;
        /**
         * Reflects the current state of the TuioObject
         */
        unsigned char state;
		
	public:
		/**
		 * The default constructor only takes the Session ID
		  * @param	s_id	the Session ID to assign
		 */
		TuioObject (unsigned int s_id);
        
        /**
         * This constructor takes a TuioTime and the Session ID
         * @param	ttime	the TuioTime to assign
         * @param	s_id	the Session ID to assign
         */
        TuioObject (TuioTime ttime, unsigned int s_id);
  
        /**
         * This constructor takes a TuioTime and the Session ID
         * @param	ttime	the TuioTime to assign
         * @param	src     the TuioSource to assign
         * @param	s_id	the Session ID to assign
         */
        TuioObject (TuioTime ttime, TuioSource *src, unsigned int s_id);
        
        /**
         * The default destructor also delets all assigned TUIO components
         */
        ~TuioObject ();
        
        /**
         * Returns the associated Session ID
         * @return	the associated Session ID
         */
        unsigned int getSessionID();
	
        /**
         * Sets the assotiated TUIO source
         *
         * @param	src         the TuioSource to assign
         */
        void setTuioSource(TuioSource *src);
  
        /**
         * Returns the associated TUIO source
         */
        TuioSource* getTuioSource();
                
		/**
		 * This method assigns a TuioToken to this TuioObject
		 * @param	ttok	the TuioToken to assign
		 */
		void setTuioToken(TuioToken *ttok);
	
        /**
         * This method assigns a TuioPointer to this TuioObject
         * @param	tptr	the TuioPointer to assign
         */
        void setTuioPointer(TuioPointer *tptr);
        
        /**
         * This method assigns a TuioBounds to this TuioObject
         * @param	tbnd	the TuioBounds to assign
         */
        void setTuioBounds(TuioBounds *tbnd);
        
        /**
         * This method assigns a TuioSymbol to this TuioObject
         * @param	tsym	the TuioSymbol to assign
         */
        void setTuioSymbol(TuioSymbol *tsym);

        /**
         * This method assigns a TuioToken3D to this TuioObject
         * @param	ttok	the TuioToken3D to assign
         */
        void setTuioToken3D(TuioToken3D *ttok);

        /**
         * This method assigns a TuioPointer3D to this TuioObject
         * @param	tptr	the TuioPointer3D to assign
         */
        void setTuioPointer3D(TuioPointer3D *tptr);

        /**
         * This method assigns a TuioBounds3D to this TuioObject
         * @param	tbnd	the TuioBounds3D to assign
         */
        void setTuioBounds3D(TuioBounds3D *tbnd);

        /**
         * This method assigns a TuioGeometry to this TuioObject
         * @param	tgeo	the TuioGeometry to assign
         */
        void setTuioGeometry(TuioGeometry *tgeo);

        /**
         * This method assigns a TuioControl to this TuioObject
         * @param	tctl	the TuioControl to assign
         */
        void setTuioControl(TuioControl *tctl);

        /**
         * This method assigns a TuioData to this TuioObject
         * @param	tdat	the TuioData to assign
         */
        void setTuioData(TuioData *tdat);

        /**
         * This method assigns a TuioSignal to this TuioObject
         * @param	tsig	the TuioSignal to assign
         */
        void setTuioSignal(TuioSignal *tsig);

        /**
         * This method adds a TuioAssociation to this TuioObject
         * @param	tasc	the TuioAssociation to add
         */
        void addTuioAssociation(TuioAssociation *tasc);
  
        /**
         * This method sets all TuioComponents in this TuioObject to TUIO_REMOVED state
         */
        void removeAllTuioComponents(TuioTime ttime);
        
        /**
         * This method sets the TuioToken in this TuioObject to TUIO_REMOVED state
         */
        void removeTuioToken(TuioTime ttime);
        
        /**
         * This method sets the TuioPointer in this TuioObject to TUIO_REMOVED state
         */
        void removeTuioPointer(TuioTime ttime);
        
        /**
         * This method sets the TuioBounds in this TuioObject to TUIO_REMOVED state
         */
        void removeTuioBounds(TuioTime ttime);
        
        /**
         * This method sets the TuioSymbol in this TuioObject to TUIO_REMOVED state
         */
        void removeTuioSymbol(TuioTime ttime);

        /**
         * This method sets the TuioToken3D in this TuioObject to TUIO_REMOVED state
         */
        void removeTuioToken3D(TuioTime ttime);

        /**
         * This method sets the TuioPointer3D in this TuioObject to TUIO_REMOVED state
         */
        void removeTuioPointer3D(TuioTime ttime);

        /**
         * This method sets the TuioBounds3D in this TuioObject to TUIO_REMOVED state
         */
        void removeTuioBounds3D(TuioTime ttime);

        /**
         * This method sets the TuioGeometry in this TuioObject to TUIO_REMOVED state
         */
        void removeTuioGeometry(TuioTime ttime);

        /**
         * This method sets the TuioControl in this TuioObject to TUIO_REMOVED state
         */
        void removeTuioControl(TuioTime ttime);

        /**
         * This method sets the TuioData in this TuioObject to TUIO_REMOVED state
         */
        void removeTuioData(TuioTime ttime);

        /**
         * This method sets the TuioSignal in this TuioObject to TUIO_REMOVED state
         */
        void removeTuioSignal(TuioTime ttime);

        /**
         * This method sets all TuioAssociations in this TuioObject to TUIO_REMOVED state
         */
        void removeTuioAssociations(TuioTime ttime);
        
        /**
         * This method deletes all TuioComponents in this TuioObject
         */
        void deleteAllTuioComponents();
        
        /**
         * This method deletes the TuioToken in this TuioObject
         */
        void deleteTuioToken();
        
        /**
         * This method deletes the TuioPointer in this TuioObject
         */
        void deleteTuioPointer();
        
        /**
         * This method deletes the TuioBounds in this TuioObject
         */
        void deleteTuioBounds();
        
        /**
         * This method deletes the TuioSymbol in this TuioObject
         */
        void deleteTuioSymbol();

        /**
         * This method deletes the TuioToken3D in this TuioObject
         */
        void deleteTuioToken3D();

        /**
         * This method deletes the TuioPointer3D in this TuioObject
         */
        void deleteTuioPointer3D();

        /**
         * This method deletes the TuioBounds3D in this TuioObject
         */
        void deleteTuioBounds3D();

        /**
         * This method deletes the TuioGeometry in this TuioObject
         */
        void deleteTuioGeometry();

        /**
         * This method deletes the TuioControl in this TuioObject
         */
        void deleteTuioControl();

        /**
         * This method deletes the TuioData in this TuioObject
         */
        void deleteTuioData();

        /**
         * This method deletes the TuioSignal in this TuioObject
         */
        void deleteTuioSignal();

        /**
         * This method deletes all TuioAssociations in this TuioObject
         */
        void deleteTuioAssociations();
        
        /**
         * This method clears all TuioComponents in this TuioObject
         */
        void clearAllTuioComponents();
        
        /**
         * This method clears the TuioToken in this TuioObject
         */
        void clearTuioToken();
        
        /**
         * This method clears the TuioPointer in this TuioObject
         */
        void clearTuioPointer();
        
        /**
         * This method clears the TuioBounds in this TuioObject
         */
        void clearTuioBounds();
        
        /**
         * This method clears the TuioSymbol in this TuioObject
         */
        void clearTuioSymbol();

        /**
         * This method clears the TuioToken3D in this TuioObject
         */
        void clearTuioToken3D();

        /**
         * This method clears the TuioPointer3D in this TuioObject
         */
        void clearTuioPointer3D();

        /**
         * This method clears the TuioBounds3D in this TuioObject
         */
        void clearTuioBounds3D();

        /**
         * This method clears the TuioGeometry in this TuioObject
         */
        void clearTuioGeometry();

        /**
         * This method clears the TuioControl in this TuioObject
         */
        void clearTuioControl();

        /**
         * This method clears the TuioData in this TuioObject
         */
        void clearTuioData();

        /**
         * This method clears the TuioSignal in this TuioObject
         */
        void clearTuioSignal();

        /**
         * This method clears all TuioAssociations in this TuioObject
         */
        void clearTuioAssociations();
        
        /**
         * This method tests for any TuioComponent in this TuioObject
         * @return true if any TuioComponent has been assigned
         */
        bool containsAnyTuioComponent();
        
        /**
         * This method tests for a TuioToken in this TuioObject
         * @return true if a TuioToken has been assigned
         */
        bool containsTuioToken();
        
        /**
         * This method tests for a TuioPointer in this TuioObject
         * @return true if a TuioPointer has been assigned
         */
        bool containsTuioPointer();
        
        /**
         * This method tests for a TuioBounds in this TuioObject
         * @return true if a TuioBounds has been assigned
         */
        bool containsTuioBounds();
        
        /**
         * This method tests for a TuioSymbol in this TuioObject
         * @return true if a TuioSymbol has been assigned
         */
        bool containsTuioSymbol();

        /**
         * This method tests for a TuioToken3D in this TuioObject
         * @return true if a TuioToken3D has been assigned
         */
        bool containsTuioToken3D();

        /**
         * This method tests for a TuioPointer3D in this TuioObject
         * @return true if a TuioPointer3D has been assigned
         */
        bool containsTuioPointer3D();

        /**
         * This method tests for a TuioBounds3D in this TuioObject
         * @return true if a TuioBounds3D has been assigned
         */
        bool containsTuioBounds3D();

        /**
         * This method tests for a TuioGeometry in this TuioObject
         * @return true if a TuioGeometry has been assigned
         */
        bool containsTuioGeometry();

        /**
         * This method tests for a TuioControl in this TuioObject
         * @return true if a TuioControl has been assigned
         */
        bool containsTuioControl();

        /**
         * This method tests for a TuioData in this TuioObject
         * @return true if a TuioData has been assigned
         */
        bool containsTuioData();

        /**
         * This method tests for a TuioSignal in this TuioObject
         * @return true if a TuioSignal has been assigned
         */
        bool containsTuioSignal();

        /**
         * This method tests for any TuioAssociation in this TuioObject
         * @return true if a TuioAssociation has been assigned
         */
        bool containsTuioAssociation();
        
        /**
         * This method tests if a new TuioToken has been added to this TuioObject
         * @return true if a TuioToken has been added
         */
        bool containsNewTuioToken();
        
        /**
         * This method tests if a new TuioPointer has been added this TuioObject
         * @return true if a TuioPointer has been added
         */
        bool containsNewTuioPointer();
        
        /**
         * This method tests if a new TuioBounds has been added to this TuioObject
         * @return true if a TuioBounds has been added
         */
        bool containsNewTuioBounds();
        
        /**
         * This method tests if a new TuioSymbol has beed added to this TuioObject
         * @return true if a TuioSymbol has been added
         */
        bool containsNewTuioSymbol();

        /**
         * This method tests if a new TuioToken3D has been added to this TuioObject
         * @return true if a TuioToken3D has been added
         */
        bool containsNewTuioToken3D();

        /**
         * This method tests if a new TuioPointer3D has been added to this TuioObject
         * @return true if a TuioPointer3D has been added
         */
        bool containsNewTuioPointer3D();

        /**
         * This method tests if a new TuioBounds3D has been added to this TuioObject
         * @return true if a TuioBounds3D has been added
         */
        bool containsNewTuioBounds3D();

        /**
         * This method tests if a new TuioGeometry has been added to this TuioObject
         * @return true if a TuioGeometry has been added
         */
        bool containsNewTuioGeometry();

        /**
         * This method tests if a new TuioControl has been added to this TuioObject
         * @return true if a TuioControl has been added
         */
        bool containsNewTuioControl();

        /**
         * This method tests if a new TuioData has been added to this TuioObject
         * @return true if a TuioData has been added
         */
        bool containsNewTuioData();

        /**
         * This method tests if a new TuioSignal has been added to this TuioObject
         * @return true if a TuioSignal has been added
         */
        bool containsNewTuioSignal();

        /**
         * This method tests if a new TuioAssociation has been added to this TuioObject
         * @return true if a TuioAssociation has been added
         */
        bool containsNewTuioAssociation();

        /**
         * This method returns the TuioToken associated to this TuioObject
         * @return	the associated TuioToken
         */
        TuioToken* getTuioToken();

        /**
         * This method returns the TuioPointer associated to this TuioObject
         * @return	the associated TuioPointer
         */
        TuioPointer* getTuioPointer();
 
        /**
         * This method returns the TuioBounds associated to this TuioObject
         * @return	the associated TuioBounds
         */
        TuioBounds* getTuioBounds();
        
        /**
         * This method returns the TuioSymbol associated to this TuioObject
         * @return	the associated TuioSymbol
         */
        TuioSymbol* getTuioSymbol();

        /**
         * This method returns the TuioToken3D associated to this TuioObject
         * @return	the associated TuioToken3D
         */
        TuioToken3D* getTuioToken3D();

        /**
         * This method returns the TuioPointer3D associated to this TuioObject
         * @return	the associated TuioPointer3D
         */
        TuioPointer3D* getTuioPointer3D();

        /**
         * This method returns the TuioBounds3D associated to this TuioObject
         * @return	the associated TuioBounds3D
         */
        TuioBounds3D* getTuioBounds3D();

        /**
         * This method returns the TuioGeometry associated to this TuioObject
         * @return	the associated TuioGeometry
         */
        TuioGeometry* getTuioGeometry();

        /**
         * This method returns the TuioControl associated to this TuioObject
         * @return	the associated TuioControl
         */
        TuioControl* getTuioControl();

        /**
         * This method returns the TuioData associated to this TuioObject
         * @return	the associated TuioData
         */
        TuioData* getTuioData();

        /**
         * This method returns the TuioSignal associated to this TuioObject
         * @return	the associated TuioSignal
         */
        TuioSignal* getTuioSignal();

        /**
         * This method returns the TuioAssociations associated to this TuioObject
         * @return	the associated TuioAssociations
         */
        std::list<TuioAssociation*> getTuioAssociationList();

        /**
         * This method deletes the provided TuioAssociation from this TuioObject
         * @param	tasc	the TuioAssociation to delete
         */
        void deleteTuioAssociation(TuioAssociation *tasc);
        
        /**
         * This method stops all encapsulated TuioComponents
         */
        void stop(TuioTime ttime);
        
        /**
         * This method removes all encapsulated TuioComponents
         */
        void remove(TuioTime ttime);
        
        /**
         * This method returns true if any encapsulated TuioComponent is moving
         */
        bool isMoving();
        
        /**
         * This method refreshes the currentTime after an update
         */
        void update(TuioTime ttime);
    
        /**
         * Returns current time stamp of this TuioPoint as TuioTime
         *
         * @return	the  time stamp of this TuioPoint as TuioTime
         */
        TuioTime getTuioTime() const;

        /**
		 * Returns the start time of this TuioPoint as TuioTime. 
		 *
		 * @return	the start time of this TuioPoint as TuioTime
		 */
		TuioTime getStartTime() const;
        
        /**
         * Returns the TUIO state of this TuioObject.
         * @return	the TUIO state of this TuioObject
         */
        int getTuioState() const;
	};
}
#endif // INCLUDED_TUIOOBJECT_H
