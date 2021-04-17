//NO_POST
#ifdef NO_POST
    #warning POST is disabled. Device will default to 120V mode
#endif

//NO_LCD
#ifdef NO_LCD
    #ifdef EN_LCD
        #warning LCD is disabled.
        #undef EN_LCD
    #endif
#endif

//NO_SAFETY_CHECKS
//Disables:
//   - AC detector
#ifdef NO_SAFETY_CHECKS
    #ifdef EN_AC_DETECTOR
        #warning AC Detector will be disabled because NO_SAFETY_CHECKS is set
        #undef EN_AC_DETECTOR
    #endif

    #ifdef EN_TEMP_SENSOR
        #warning Temperature Sensor will be disabled because NO_SAFETY_CHECKS is set
        #undef EN_TEMP_SENSOR
    #endif

    #ifdef EN_GFCI
        #warning GFCI will be disabled because NO_SAFETY_CHECKS is set
        #undef EN_GFCI
    #endif
#endif

//OCD
//Disables:
//   - Control Pilot
#ifdef OCD
    #ifdef EN_CP
        #warning Control Pilot will be disabled because OCD is set
        #undef EN_CP
    #endif
#endif