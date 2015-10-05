; IDL-DODS interface to the CEDAR authentication.
; Written Jose H. Garcia
; Copyright 2001, UCAR
;
; Batch file to set up system variables and link external routines.
;
PRO cedar_authentication_setup

DEFSYSV, '!cedar_authenticate_user', EXISTS=cedar_authentication_defined
   
; If this is the first time run in this IDL session,
; then define system variables and link external routines.
IF (cedar_authentication_defined EQ 0) THEN BEGIN

        ;PRINT, 'Loading share object code...'   
        DEFSYSV, '!cedar_authenticate_user', 0
	DEFSYSV, '!cedar_logoff_user', 1
        DEFSYSV, '!cedar_create_session', 3

        so_file = 'libcedar_auth.so'

	LINKIMAGE, 'cedar_authenticate_user', so_file, 1
	LINKIMAGE, 'cedar_logoff_user', so_file, 1
        LINKIMAGE, 'cedar_create_session',so_file, 1
	LINKIMAGE, 'toggle_debug', so_file, 0
ENDIF ELSE BEGIN
	PRINT, 'Did not load share object code...'   
ENDELSE

END

