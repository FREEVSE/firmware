//NO_POST
#if defined NO_POST
    #warning POST is disabled. Device will default to 120V mode
#endif

//NO_LCD
#if defined NO_LCD
    #warning LCD is disabled.
#endif

//NO_SAFETY_CHECKS
//Disables:
//   - AC detector
#if defined NO_SAFETY_CHECKS
    #ifdef EN_AC_DETECTOR
        #warning AC Detector will be disabled because NO_SAFETY_CHECKS is set
        #undef EN_AC_DETECTOR
    #endif
#endif

//OCD
//Disables:
//   - Control Pilot
#if defined OCD
    #ifdef EN_CP
        #warning Control Pilot will be disabled because OCD is set
        #undef EN_CP
    #endif
#endif